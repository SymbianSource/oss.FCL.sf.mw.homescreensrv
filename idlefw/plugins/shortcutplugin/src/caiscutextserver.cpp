/*
* Copyright (c) 2005-2006 Nokia Corporation and/or its subsidiary(-ies).
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
* Description:  Shortcut plug-in xSP extension server class.
*
*/


#include <centralrepository.h>
#include <apgcli.h>
#include <apadef.h>
#include <apgwgnam.h>
#include <bautils.h>
#include <e32property.h>

#include <aknmessagequerydialog.h>
#include <StringLoader.h>

#include <aiscutplugindomaincrkeys.h>
#include <aiscutpluginres.rsg>
#include <aiscutextdefs.h>

#include "caiscutextserver.h"
#include "caiscutextsession.h"
#include "taiscutparser.h"
#include "caiscutextserver.h"
#include "activeidle2domainpskeys.h"


// Amount of bytes the data buffer increases at a time.
const TInt KAppsListDataGranularity = 100;

// The length of '0x' in front of a hex literal.
const TInt KTokenPrefix = 2;

// Index to UID 3
const TInt KUidIndex = 2;

// Count of characters one UID occupies in CenRep (prefix + 8 digits + spacer)
const TInt KUidStorageLength = KTokenPrefix + 8 + 1;

_LIT(KUidFormat, "0x%08X ");

_LIT(KResource, "z:\\resource\\aiscutpluginres.rsc");

_LIT(KRomDrive, "z");


CLinkParams::CLinkParams(CAiScutExtServer& aServer, HBufC* aTargetDefinition)
    : iServer(aServer)
    {
    iTargetDefinition = aTargetDefinition;
    }

CLinkParams::~CLinkParams()
    {
    delete iTargetDefinition;
    }

// ======== MEMBER FUNCTIONS ========


// ---------------------------------------------------------------------------
//
// ---------------------------------------------------------------------------
//
CAiScutExtServer::CAiScutExtServer( MAiScutExtMessageHandler& aObserver, RWsSession& aWsSession )
    : CServer2( CActive::EPriorityStandard ), iObserver( aObserver ), iWsSession(aWsSession)
    {
    }

// ---------------------------------------------------------------------------
//
// ---------------------------------------------------------------------------
//
void CAiScutExtServer::ConstructL()
{
    iIdle = CIdle::NewL( EPriorityStandard );

    TFileName fileName( KResource );
    CEikonEnv* env = CEikonEnv::Static();
    BaflUtils::NearestLanguageFile( env->FsSession(), fileName );
    iResourceOffset = env->AddResourceFileL( fileName );    

    ReadAppListL();
}

// ---------------------------------------------------------------------------
//
// ---------------------------------------------------------------------------
//
CAiScutExtServer::~CAiScutExtServer()
    {
    if( iIdle )
        {
        iIdle->Cancel();
        delete iIdle;
        }

    CEikonEnv::Static()->DeleteResourceFile( iResourceOffset );

    iShowUids.Close();
    iUids.Close();
    }

// ---------------------------------------------------------------------------
//
// ---------------------------------------------------------------------------
//
CSession2* CAiScutExtServer::NewSessionL(
    const TVersion& aVersion, const RMessage2& /*aMessage*/ ) const
    {
    TVersion version(
        KAiScutExtServMajorVersionNumber,
        KAiScutExtServMinorVersionNumber,
        KAiScutExtServBuildVersionNumber );

    if ( !User::QueryVersionSupported( version, aVersion ) )
        {
        User::Leave( KErrNotSupported );
        }
    CAiScutExtSession* session = new( ELeave ) CAiScutExtSession( iObserver );
    return session;
}

// ---------------------------------------------------------------------------
// Read a list of application UIDs which we must not show the tip.
// ---------------------------------------------------------------------------
//
void CAiScutExtServer::ReadAppListL()
    {
    CRepository* repository = CRepository::NewL( KCRUidShortcutItems );
    CleanupStack::PushL( repository );

    RBuf buffer;
    buffer.CreateL( KAppsListDataGranularity );
    buffer.CleanupClosePushL();

    TInt err = repository->Get( KAIxSPNoTipAppsList, buffer );
    while( err == KErrOverflow )
        {
        buffer.ReAllocL( buffer.MaxLength() + KAppsListDataGranularity );
        err = repository->Get( KAIxSPNoTipAppsList, buffer );
        }

    // Handle error silently if reading from Central Repository fails.
    if( err == KErrNone )
        {
        // Parse application UIDs and put them to list.
        TLex lex(buffer);

        while( !lex.Eos() )
            {
            TPtrC token( lex.NextToken() );
            if( token.Length() < KTokenPrefix )
                {
                break;
                }
            TLex lexUid( token.Right( token.Length() - KTokenPrefix ) );

            TUint32 uid;
            if( lexUid.Val( uid, EHex ) == KErrNone )
                {
                iUids.AppendL( TUid::Uid( uid ) );
                }
            }
        }

    CleanupStack::PopAndDestroy();      // buffer
    CleanupStack::PopAndDestroy( repository );
    }

// ---------------------------------------------------------------------------
// Saves the list of applications UIDs.
// ---------------------------------------------------------------------------
//
void CAiScutExtServer::WriteAppListL()
    {
    CRepository* repository = CRepository::NewL( KCRUidShortcutItems );
    CleanupStack::PushL( repository );

    RBuf buffer;
    buffer.CreateL( iUids.Count() * KUidStorageLength );
    buffer.Zero();
    buffer.CleanupClosePushL();

    TBuf<KUidStorageLength> uidString;

    for(TInt c = 0; c < iUids.Count(); c++)
        {
        uidString.Format( KUidFormat, iUids[c] );
        buffer.Append( uidString );   // Ignore error
        }

    User::LeaveIfError( repository->Set( KAIxSPNoTipAppsList, buffer ) );

    CleanupStack::PopAndDestroy();    // buffer
    CleanupStack::PopAndDestroy( repository );
    }

// ---------------------------------------------------------------------------
// Tests if the client has connected us before, i.e. determines if
// the tip message should be shown.
// ---------------------------------------------------------------------------
//
TBool CAiScutExtServer::HasConnectedBefore( TUid& aUid, const RMessage2& aMessage )
    {
    // First test if the current theme supports the xSP feature
    TInt isXspSupported = EPSAiXspNotSupported;
    TInt err = RProperty::Get( KPSUidAiInformation, KActiveIdleThemeSupportsXsp,
        isXspSupported );

    if( err != KErrNone || isXspSupported == EPSAiXspNotSupported)
    {
        // xSP is not supported so don't show the dialog. Also if we could not
        // read the P&S key we won't show the dialog.
        return ETrue;
    }

    RThread threadClient;

    if( aMessage.Client( threadClient ) != KErrNone )
        {
        // In case of error we report that the client has connected us
        // before in order not to show the dialog.
        return ETrue;
        }

    RProcess processClient;

    if( threadClient.Process(processClient) != KErrNone )
        {
        // Prevent dialog from showing in case of error.
        return ETrue;
        }

    // See if client application is in ROM
    TFileName fileName = processClient.FileName();
    TPtr processDrive( fileName.LeftTPtr( KRomDrive().Length() ) );
    processDrive.LowerCase();

    if( processDrive == KRomDrive() )
        {
        // Don't show tip message for ROM clients.
        return ETrue;
        }

    TUidType uidType = processClient.Type();

    // UID3 of the client process
    TUid uid3( uidType[ KUidIndex ] );
    aUid = uid3;

    // See if the uid is already in the list
    if( iUids.Find( uid3 ) != KErrNotFound )
        {
        return ETrue;
        }

    return EFalse;
    }

void CAiScutExtServer::ShowTipMessage( TUid aUid )
    {
    if( iShowUids.Find( aUid ) == KErrNotFound )
        {
        iShowUids.Append( aUid );
        iIdle->Cancel();
        iIdle->Start( TCallBack( CAiScutExtServer::IdleCallBack, this ) );
        }
    }

TInt CAiScutExtServer::IdleCallBack(TAny* aParam)
    {
    CAiScutExtServer* p = (CAiScutExtServer*) aParam;

    p->iIdle->Cancel();

    if( p->iIsForeground )
        {
        TRAP_IGNORE( p->DoShowTipMessageL() );
        }

    return KErrNone;
    }

// ---------------------------------------------------------------------------
// Shows a dialog
// ---------------------------------------------------------------------------
//
void CAiScutExtServer::DoShowTipMessageL()
    {
    // Find out the name of the application
    RApaLsSession als;
    User::LeaveIfError( als.Connect() );
    CleanupClosePushL( als );

    while( iShowUids.Count() > 0 && !iIsTipMessageVisible)
        {
        TUid uid = iShowUids[ 0 ];
        TApaAppInfo appInfo;
        User::LeaveIfError( als.GetAppInfo( appInfo, uid ) );

        // Prepare the parameters for the link function
        HBufC* definitionBuf = HBufC::NewLC( KMaxDefinitionLength );
        TPtr definition( definitionBuf->Des() );
        definition.Format( KScutFormatApplication, uid );

        TBool isInShortcuts = iObserver.HandleIsInShortcutsL( definition );

        CLinkParams* params = new( ELeave) CLinkParams( *this, definitionBuf );
        CleanupStack::Pop( definitionBuf );
        CleanupStack::PushL( params );

        TCallBack cb(CAiScutExtServer::LinkCallBack, params);

        // Read and format tip message from the resource file
        HBufC* messageBuf = StringLoader::LoadLC( R_AI_SC_QUERY_MODIFY_GS, appInfo.iCaption );
        TPtr message( messageBuf->Des() );

        // Don't show the tip if the application is already in shortcuts
        if( !isInShortcuts )
            {
            iIsTipMessageVisible = ETrue;
            CAknMessageQueryDialog* note = CAknMessageQueryDialog::NewL( message );
            params->iNote = note;

            note->SetLink( cb );
            note->ExecuteLD( R_SCUT_XSPTIP_NOTE );
            iIsTipMessageVisible = EFalse;
            }

        // Update the list so we don't show the tip message
        // again for this application.
        iUids.Append( uid );   // Ignore error
        iShowUids.Remove( 0 );

        // Save the list of uids to persistant memory.
        WriteAppListL();

        CleanupStack::PopAndDestroy( messageBuf );
        CleanupStack::PopAndDestroy( params );
        }

    CleanupStack::PopAndDestroy();  // als
    }

// ---------------------------------------------------------------------------
// Opens the shurtcuts settings view
// ---------------------------------------------------------------------------
//
TInt CAiScutExtServer::LinkCallBack(TAny* aParam)
    {
    CLinkParams* params = (CLinkParams*) aParam;

    TRAP_IGNORE(
        params->iServer.iObserver.HandleIssuePutInShortcutsL(
            params->iTargetDefinition->Des() );
    );

    return KErrNone;
    }

// ---------------------------------------------------------------------------
//
// ---------------------------------------------------------------------------
//
void CAiScutExtServer::ResumeL( TAiTransitionReason aReason )
    {
    switch( aReason )
        {
        case EAiIdleForeground:
            iIsForeground = ETrue;
            DoShowTipMessageL();
            break;

        case EAiIdleBackground:
            iIsForeground = EFalse;
            break;

        default:
            break;
        }
    }

// End of File.
