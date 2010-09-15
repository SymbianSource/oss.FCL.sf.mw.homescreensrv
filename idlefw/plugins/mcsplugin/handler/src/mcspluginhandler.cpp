/*
* Copyright (c) 2009 Nokia Corporation and/or its subsidiary(-ies).
* All rights reserved.
* This component and the accompanying materials are made available
* under the terms of "Eclipse Public License v1.0"
* which accompanies this distribution, and is available
* at the URL "http://www.eclipse.org/legal/epl-v10.html".
*
* Initial Contributors:
* Nokia Corporation - initial contribution.
*
* Contributors:
*
* Description:
*
*/

// System includes
#include <ecom/implementationproxy.h>
#include <eikenv.h>
#include <sendui.h>
#include <SendUiConsts.h>
#include <viewcli.h>                // For CVwsSessionWrapper
#ifdef SYMBIAN_ENABLE_SPLIT_HEADERS
#include <viewclipartner.h>
#endif

#include <vwsdef.h>                 // For TVwsViewId
#include <mcsmenuutils.h>
#include <mcsmenuitem.h>
#include <mcspluginparamval.h>
#include <LogsUiCmdStarter.h>
#include <apgcli.h>

// User includes
#include "mcsplugincompletedoperation.h"
#include "mcspluginhandler.h"
#include "mcspluginuids.hrh"

// Constants
_LIT( KMenuTypeShortcut, "menu:shortcut" ); ///< Menu folder type.
_LIT( KMenuAttrParamLogs, "logs:dialed" );
_LIT( KMenuAttrProfiles, "profiles" );

/** Argument value for parameter*/
_LIT( KMenuAttrParam, "param" );

const TUid KProfilesAppUid = { 0x100058F8 };

#define KMCSCmailUidValue 0x2001E277
#define KMCSCmailMailboxDefaultViewIdValue 0x1
#define KMCSCmailMailboxViewIdValue 0x2
#define KMCSCmailMtmUidValue 0x2001F406

const TInt KImplUidMCSPluginHandler = AI_UID_ECOM_IMPLEMENTATION_MCSPLUGIN_HANDLER;

const TImplementationProxy ImplementationTable[] = 
    {
    IMPLEMENTATION_PROXY_ENTRY( KImplUidMCSPluginHandler, CMCSPluginHandler::NewL)
    };

// ================= MEMBER FUNCTIONS =======================

// ---------------------------------------------------------
// CMCSPluginHandler::~CMCSPluginHandler
// ---------------------------------------------------------
//
CMCSPluginHandler::~CMCSPluginHandler()
    {
    delete iVwsSession;
    delete iMsvSession;
    }

// ---------------------------------------------------------
// CMCSPluginHandler::NewL
// ---------------------------------------------------------
//
CMCSPluginHandler* CMCSPluginHandler::NewL( RMenu &aMenu )
    {
    CMCSPluginHandler* handler = new (ELeave) CMCSPluginHandler( aMenu );
    CleanupStack::PushL( handler );
    handler->ConstructL();
    CleanupStack::Pop( handler );
    return handler;
    }

// ---------------------------------------------------------
// CMCSPluginHandler::CMCSPluginHandler
// ---------------------------------------------------------
//
CMCSPluginHandler::CMCSPluginHandler( RMenu &aMenu )
: CMenuHandlerPlugin( aMenu )
    {
    iEikEnv = CEikonEnv::Static();
    }

// ---------------------------------------------------------
// CMCSPluginHandler::ConstructL
// ---------------------------------------------------------
//
void CMCSPluginHandler::ConstructL()
    {
    BaseConstructL();
    iVwsSession = CVwsSessionWrapper::NewL();
    iMsvSession = CMsvSession::OpenAsObserverL(*this);
    }

// ---------------------------------------------------------
// Supported menu item types
// ---------------------------------------------------------
//
TBool CMCSPluginHandler::SupportsType( const TDesC& aType )
    {
    if ( !aType.Compare( KMenuTypeFolder() ) ||
         !aType.Compare( KMenuTypeSuite() )  ||
         !aType.Compare( KMenuTypeShortcut ) )
        {
        return ETrue;
        }
    return EFalse;
    }

// ---------------------------------------------------------
// Main command handler
// ---------------------------------------------------------
//
CMenuOperation* CMCSPluginHandler::HandleCommandL(
        CMenuItem& aItem,
        const TDesC8& aCommand,
        const TDesC8& /*aParams*/,
        TRequestStatus& aStatus )
    {
    if ( aCommand != KMenuCmdOpen() )
        {
        User::Leave ( KErrNotSupported );
        }

    if( aItem.Type() == KMenuTypeFolder() )
        {
        
        return CMCSPluginCompletedOperation::NewL
            ( iMenu, CActive::EPriorityStandard, aStatus, KErrNone );
        }
    else if ( aItem.Type() == KMenuTypeSuite() )
        {
        return CMCSPluginCompletedOperation::NewL
            ( iMenu, CActive::EPriorityStandard, aStatus, KErrNone );
        }
    else if ( aItem.Type() == KMenuTypeShortcut() )
        {
        LaunchShortcutL( aItem );
        return CMCSPluginCompletedOperation::NewL
            ( iMenu, CActive::EPriorityStandard, aStatus, KErrNone );
        }
    else
        {
        User::Leave ( KErrNotSupported );
        }
    return NULL;
    }

// ---------------------------------------------------------------------------
// From class MMsvSessionObserver.
// Handles an event from the message server.
// ---------------------------------------------------------------------------
//
void CMCSPluginHandler::HandleSessionEventL(
          TMsvSessionEvent /*aEvent*/, 
          TAny* /*aArg1*/, 
          TAny* /*aArg2*/,
          TAny* /*aArg3*/ )
    {
    // No event handling
    }

// ---------------------------------------------------------
// Handles menu:shortcut specific commands
// Must be extended to launch e.g. MailBoxes
// caiscuttargetmessagingview.cpp implements the mailbox launching in old shortcut plugin
// ---------------------------------------------------------
//
void CMCSPluginHandler::LaunchShortcutL( CMenuItem& aItem )
    {

    // menu:shortcut type of items are launched based on attribute param
    TBool attrExists = ETrue;
    TPtrC param = aItem.GetAttributeL( KMenuAttrParam, attrExists );

    if ( !attrExists )
        {
        User::Leave( KErrCorrupt );
        }

    // Messaging specific commands
    CSendUi* sendUi = CSendUi::NewLC();
    if ( param == KParamValueSelType ) // Select message type
        {
        TSendingCapabilities capabs( 0, 0, TSendingCapabilities::ESupportsEditor );

        TUid uid = sendUi->ShowTypedQueryL( CSendUi::EWriteMenu, NULL, capabs, NULL, KNullDesC );
        if ( uid != KNullUid )
            {
            sendUi->ServiceCapabilitiesL( uid, capabs );
            sendUi->CreateAndSendMessageL( uid, NULL, KNullUid, EFalse ); // launch standalone
            }
        }
    else if ( param == KParamValueMsg ) // New text message
        {
        sendUi->CreateAndSendMessageL(KSenduiMtmUniMessageUid, NULL, KNullUid, EFalse);
        }
    else if ( param == KParamValueEmail ) // New email
        {
        if ( GetEmailAccountCountL() > 0 )
            {
            sendUi->CreateAndSendMessageL( KSenduiMtmSmtpUid, NULL, KNullUid, EFalse );
            }
        else
            {
            const TVwsViewId viewId( TUid::Uid( KMCSCmailUidValue ), 
                    TUid::Uid( KMCSCmailMailboxDefaultViewIdValue ) );
            iVwsSession->CreateActivateViewEvent( viewId, 
                    TUid::Uid( KMCSCmailUidValue ), KNullDesC8() );
            }

        }
#ifdef __SYNCML_DS_EMAIL
    else if ( param == KParamValueSyncMLMail ) // New SyncML mail
        {
        sendUi->CreateAndSendMessageL( KSenduiMtmSyncMLEmailUid, NULL, KNullUid, EFalse );
        }
#endif
    else if ( param == KParamValuePostcard ) // New postcard
        {
        sendUi->CreateAndSendMessageL( KSenduiMtmPostcardUid, NULL, KNullUid, EFalse );
        }
    else if ( param == KParamValueAudioMsg ) // New audio message
        {
        sendUi->CreateAndSendMessageL( KSenduiMtmAudioMessageUid, NULL, KNullUid, EFalse );
        }
    else if ( param.Find( KParamValueMailbox ) != KErrNotFound ) // Mailbox
        {
        TInt pos = param.Locate( TChar( ':' ) ) + 1;
        TPtrC mailboxId = param.Mid( pos );
        TInt number;
        TLex16 lextmp( mailboxId );
        lextmp.Val( number );
        TUid uId = TUid::Uid( number );
        const TVwsViewId viewId( TUid::Uid( KMCSCmailUidValue ), TUid::Uid( KMCSCmailMailboxViewIdValue ) );
        iVwsSession->CreateActivateViewEvent( viewId, uId, KNullDesC8() );
        }
    else if ( param.Find( KMenuAttrParamLogs ) != KErrNotFound )
        {
        LogsUiCmdStarter::CmdStartL( LogsUiCmdStarterConsts::KDialledView() );
        }
    else if ( param.Find( KMenuAttrProfiles ) != KErrNotFound )
        {
        RApaLsSession apaLsSession;
        User::LeaveIfError( apaLsSession.Connect() );
        CleanupClosePushL( apaLsSession );

        TApaAppInfo appInfo;
        TInt retVal = apaLsSession.GetAppInfo( appInfo, KProfilesAppUid );

        if ( retVal == KErrNone )
            {
            CApaCommandLine* cmdLine = CApaCommandLine::NewLC();
            cmdLine->SetExecutableNameL( appInfo.iFullName );
            cmdLine->SetCommandL( EApaCommandRun );
            User::LeaveIfError( apaLsSession.StartApp( *cmdLine ) );

            CleanupStack::PopAndDestroy( cmdLine );
            }

        CleanupStack::PopAndDestroy( &apaLsSession );
        }
    CleanupStack::PopAndDestroy( sendUi );
    }

// ---------------------------------------------------------------------------
// Returns count of Email accounts
// ---------------------------------------------------------------------------
//
TInt CMCSPluginHandler::GetEmailAccountCountL()
    {
    CMsvEntry* rootEntry = iMsvSession->GetEntryL( KMsvRootIndexEntryIdValue );
    CleanupStack::PushL(rootEntry);
    
    TInt cnt = rootEntry->Count();
    if ( cnt > 0 )
        {
        cnt = 0;
        rootEntry->SetSortTypeL( TMsvSelectionOrdering( 
            KMsvGroupByType | KMsvGroupByStandardFolders, 
            EMsvSortByDetailsReverse, ETrue ) );
    
        for ( TInt i = rootEntry->Count(); --i >= 0; )
            {
            const TMsvEntry& tentry = (*rootEntry)[i];
    
            if (tentry.iMtm.iUid == KMCSCmailMtmUidValue )
                {
                cnt++;
                }
            }
        }
    CleanupStack::PopAndDestroy(rootEntry);
    return cnt;
    }

// ============================ GLOBAL FUNCTIONS ===============================

EXPORT_C const TImplementationProxy* ImplementationGroupProxy( TInt& aTableCount )
    {
    aTableCount = sizeof(ImplementationTable) / sizeof(TImplementationProxy);
    return ImplementationTable;
    }
