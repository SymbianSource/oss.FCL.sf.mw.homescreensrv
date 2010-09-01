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
#include "menuapphandler.h"
#include "mcsmenuitem.h"
#include "menucompletedoperation.h"
#include "menuuninstalloperation.h"
#include "menutasklist.h"

#include <mcsmenuutils.h>
#include <w32std.h>
#include <apgtask.h>
#include <apgcli.h>
#include <eikenv.h>
#include <eikappui.h>
#include <vwsdef.h>

// ================= MEMBER FUNCTIONS =======================

// ---------------------------------------------------------
// CMenuAppHandler::~CMenuAppHandler
// ---------------------------------------------------------
//
CMenuAppHandler::~CMenuAppHandler()
    {
    }

// ---------------------------------------------------------
// CMenuAppHandler::NewL
// ---------------------------------------------------------
//
CMenuAppHandler* CMenuAppHandler::NewL( RMenu &aMenu )
    {
    CMenuAppHandler* handler = new (ELeave) CMenuAppHandler( aMenu );
    CleanupStack::PushL( handler );
    handler->ConstructL();
    CleanupStack::Pop( handler );
    return handler;
    }

// ---------------------------------------------------------
// CMenuAppHandler::CMenuAppHandler
// ---------------------------------------------------------
//
CMenuAppHandler::CMenuAppHandler( RMenu &aMenu )
: CMenuHandlerPlugin( aMenu )
    {
    iEikEnv = CEikonEnv::Static();
    }

// ---------------------------------------------------------
// CMenuAppHandler::ConstructL
// ---------------------------------------------------------
//
void CMenuAppHandler::ConstructL()
    {
    BaseConstructL();
    }

// ---------------------------------------------------------
// CMenuAppHandler::SupportsType
// ---------------------------------------------------------
//
TBool CMenuAppHandler::SupportsType( const TDesC& aType )
    {
    if ( !aType.Compare( KMenuTypeApp() ) )
        {
        return ETrue;
        }
    return EFalse;
    }

// ---------------------------------------------------------
// CMenuAppHandler::HandleCommandL
// ---------------------------------------------------------
//
CMenuOperation* CMenuAppHandler::HandleCommandL(
        CMenuItem& aItem,
        const TDesC8& aCommand,
        const TDesC8& aParams,
        TRequestStatus& aStatus )
    {
    if ( aCommand == KMenuCmdOpen() && aItem.Type() == KMenuTypeApp() )
        {
        TBool attrExists = ETrue;
        TPtrC url = aItem.GetAttributeL( KMenuAttrUid(), attrExists );

        if ( !attrExists )
            {
            User::Leave( KErrCorrupt );
            }
            
        TUint appUidNo;
        TInt err = MenuUtils::GetTUint( url, appUidNo );
        if ( err != KErrNone )
            {
            User::Leave( KErrCorrupt );
            }
            
        TInt viewId;
        TPtrC view = aItem.GetAttributeL( KMenuAttrView(), attrExists );

        if ( attrExists )
            {
            err = MenuUtils::GetTUint( view, (TUint &)viewId );
            if( err != KErrNone )
                {
                User::Leave( KErrCorrupt );
                }
            }
           else
            {
            viewId = -1;
            }
        
        LaunchApplicationL( TUid::Uid( appUidNo ), aParams, viewId );

        return CMenuCompletedOperation::NewL
            ( iMenu, CActive::EPriorityStandard, aStatus, KErrNone );
        }
    else if ( aCommand == KMenuCmdRemove() && aItem.Type() == KMenuTypeApp() )
        {
        return CMenuUninstallOperation::NewL
            ( iMenu, CActive::EPriorityStandard, aStatus, aItem );
        }
    User::Leave ( KErrNotSupported );
    return NULL;
    }

// ---------------------------------------------------------
// CMenuAppHandler::LaunchApplicationL
// ---------------------------------------------------------
//
void CMenuAppHandler::LaunchApplicationL( const TUid aUid, const TDesC8 &aParam, TInt aViewId )
    {
    if ( aViewId > 0 && iEikEnv )
        {
        TUid viewId = TUid::Uid( aViewId );
        TVwsViewId view( aUid, viewId );
        iEikEnv->EikAppUi()->ActivateViewL( view );
        }
    else
        {
        RWsSession wsSession;
    	User::LeaveIfError( wsSession.Connect() );
    	CleanupClosePushL<RWsSession>( wsSession );
    	
        CMenuTaskList* taskList = CMenuTaskList::NewL( wsSession ); 
        TApaTask task = taskList->FindRootApp( aUid );
        delete taskList;

        if ( task.Exists() )
            {
            task.BringToForeground();
            }
        else
            {
            TApaAppInfo appInfo;
            TApaAppCapabilityBuf capabilityBuf;
    	    RApaLsSession appArcSession;
    	    User::LeaveIfError( appArcSession.Connect() );
    	    CleanupClosePushL<RApaLsSession>( appArcSession );

            User::LeaveIfError( appArcSession.GetAppInfo( appInfo, aUid ) );
            User::LeaveIfError( appArcSession.GetAppCapability( capabilityBuf, aUid ) );

            TApaAppCapability& caps = capabilityBuf();
            TFileName appName = appInfo.iFullName;
            CApaCommandLine* cmdLine = CApaCommandLine::NewLC();
	        cmdLine->SetExecutableNameL( appName );

            if ( caps.iLaunchInBackground )
                {
                cmdLine->SetCommandL( EApaCommandBackground );
                }
            else
                {
                cmdLine->SetCommandL( EApaCommandRun );
                }

            cmdLine->SetTailEndL( aParam );
            
            User::LeaveIfError( appArcSession.StartApp( *cmdLine ) );

    	    CleanupStack::PopAndDestroy( cmdLine );
    	    CleanupStack::PopAndDestroy( &appArcSession );
            }
        CleanupStack::PopAndDestroy( &wsSession );
        }
    }
