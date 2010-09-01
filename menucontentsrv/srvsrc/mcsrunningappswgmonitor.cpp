/*
* Copyright (c) 2008 Nokia Corporation and/or its subsidiary(-ies).
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

#include <apgwgnam.h>
#include <apgtask.h>
#include <widgetregistryclient.h>
#include <mmf/common/mmfcontrollerpluginresolver.h>

#include "mcsrunningappswgmonitor.h"
#include "mcsrunningappshandler.h"
#include "menueng.h"

const TInt KWindowGroupPosition = -1;
const TInt KWindowGroupPriority = -1;

// ---------------------------------------------------------
// CMcsRunningAppsWgMonitor::NewL
// ---------------------------------------------------------
//
CMcsRunningAppsWgMonitor* CMcsRunningAppsWgMonitor::NewL(
    CMenuEng& aEng, MMcsRunningAppsHandler* aRunningAppsHandler,
    TInt aPriority )
	{
	CMcsRunningAppsWgMonitor* self =
		new ( ELeave ) CMcsRunningAppsWgMonitor( aEng, aRunningAppsHandler,
	   aPriority );
	CleanupStack::PushL( self );
	self->ConstructL();
	CleanupStack::Pop( self );
	return self;
	}

// ---------------------------------------------------------
// CMcsRunningAppsWgMonitor::~CWindowGroupChangeMonitor
// ---------------------------------------------------------
//
CMcsRunningAppsWgMonitor::~CMcsRunningAppsWgMonitor()
    {
    Cancel();
    iEng.DequeueOperation( *this );
    iRunningApps.Close();
    delete iWgName;
    iWg.Close();
    iWidgetSession.Close();    
    iWsSession.Close();
    }

// ---------------------------------------------------------
// CMcsRunningAppsWgMonitor::CMcsRunningAppsWgMonitor
// ---------------------------------------------------------
//
CMcsRunningAppsWgMonitor::CMcsRunningAppsWgMonitor(
    CMenuEng& aEng, MMcsRunningAppsHandler* aRunningAppsHandler,
    TInt aPriority ):
	CActive( aPriority ),
    iEng(aEng),
    iRunningAppsHandler(aRunningAppsHandler)
    {
    CActiveScheduler::Add( this );
    }

// ---------------------------------------------------------
// CMcsRunningAppsWgMonitor::ConstructL
// ---------------------------------------------------------
//
void CMcsRunningAppsWgMonitor::ConstructL()
	{
	iOperationInProgress = EFalse;
    User::LeaveIfError( iWsSession.Connect() );
    User::LeaveIfError( iWidgetSession.Connect() );

    iWg = RWindowGroup( iWsSession );

    // Make a handle from the address of window group obj.
    User::LeaveIfError( iWg.Construct( 
                    reinterpret_cast<TUint32>( &iWg ) ) );

    //myWindowGroup.AutoForeground( EFalse );
    iWg.DisableFocusChangeEvents();
    iWg.DisableModifierChangedEvents();
    iWg.DisableOnEvents();
    iWg.DisableScreenChangeEvents();
    iWg.SetOrdinalPosition( KWindowGroupPosition, KWindowGroupPriority );
    User::LeaveIfError( iWg.EnableGroupListChangeEvents() );    
    //it makes the soft keys working
    iWg.EnableReceiptOfFocus( EFalse );
    
    iWgName = CApaWindowGroupName::NewL( iWsSession );
    iWgName->SetSystem( ETrue );
    iWgName->SetHidden( ETrue );
    iWgName->SetWindowGroupName( iWg );

    
    UpdateRunningApplicationsL();
    Start();
	}

// ---------------------------------------------------------
// CMcsRunningAppsWgMonitor::Start
// ---------------------------------------------------------
//
void CMcsRunningAppsWgMonitor::Start()
    {
    if (!IsActive())
        {
        iWsSession.EventReady( &iStatus );
        SetActive();
        }
    }

// ---------------------------------------------------------
// CMcsRunningAppsWgMonitor::DoCancel
// ---------------------------------------------------------
//
void CMcsRunningAppsWgMonitor::DoCancel()
    {
    iWsSession.EventReadyCancel();
    }

// ---------------------------------------------------------
// CMcsRunningAppsWgMonitor::RunL
// ---------------------------------------------------------
//
void CMcsRunningAppsWgMonitor::RunL()
    {
    User::LeaveIfError( iStatus.Int() ); // Handle errors in RunL.
    // AppArc app scan complete, we have the app list.
    // Now queue this operation to be executed by the Engine.
    
    TWsEvent event;
    iWsSession.GetEvent(event);
    Start();
    
    if (!iOperationInProgress && 
    		(event.Type() == EEventWindowGroupListChanged ))
        {
        iOperationInProgress = ETrue;
        iEng.QueueOperationL( *this );
        }
    else if ( event.Type() == EEventPowerMgmt ) 
    	{
    	TApaSystemEvent systemEvent( *(TApaSystemEvent*)(event.EventData()) );
    	// System secure shutdown, stop server
    	if ( systemEvent == EApaSystemEventSecureShutdown )
    		{
    		CActiveScheduler::Stop();
    		}
    	}
    }

// ---------------------------------------------------------
// CMcsRunningAppsWgMonitor::RunError()
// ---------------------------------------------------------
//
TInt CMcsRunningAppsWgMonitor::RunError( TInt /*aError*/ )
    {
    Start();
    return KErrNone;
    }

// ---------------------------------------------------------
// CMcsRunningAppsWgMonitor::RunMenuEngOperationL
// ---------------------------------------------------------
//
void CMcsRunningAppsWgMonitor::RunMenuEngOperationL()
    {
    iOperationInProgress = EFalse;
    UpdateRunningApplicationsL();
    } 
       
// ---------------------------------------------------------
// CMcsRunningAppsWgMonitor::CompletedMenuEngOperation
// ---------------------------------------------------------
//
void CMcsRunningAppsWgMonitor::CompletedMenuEngOperation( TInt /*aErr*/ )
    {
    // If there was error, ignore it (what else could we do?).
    // When next AppArc update occurs, we will run again.
    }

// ---------------------------------------------------------
// CMcsRunningAppsWgMonitor::UpdateRunningApplicationsL
// ---------------------------------------------------------
//
void CMcsRunningAppsWgMonitor::UpdateRunningApplicationsL()
    {
    iRunningApps.Reset();
    
    RArray<RWsSession::TWindowGroupChainInfo> windowGroupIds;
    CleanupClosePushL( windowGroupIds );
   
    User::LeaveIfError( iWsSession.WindowGroupList(0, &windowGroupIds) );
	const TInt count = windowGroupIds.Count();

	for (TInt i=0; i<count; i++)
	    {
		TInt wgId = windowGroupIds[i].iId;
		iWgName->ConstructFromWgIdL(wgId);

        // find the window group id and check that it has no parent
		if (windowGroupIds[i].iParentId <= 0)
			{
			TUid appUid = iWgName->AppUid();
			if ( ( appUid.iUid ) &&
					( iRunningApps.Find( appUid ) == KErrNotFound ))
				{
				iRunningApps.AppendL( appUid );
				}
			}
	    }

	CleanupStack::PopAndDestroy( &windowGroupIds );
	
	UpdateWidgetTaskListL();
	
	iRunningAppsHandler->HandleListUpdateL( iRunningApps, 
			MMcsRunningAppsHandler::ERunningApps );
    }

// ---------------------------------------------------------
// CMcsRunningAppsWgMonitor::UpdateWidgetTaskListL
// ---------------------------------------------------------
//
void CMcsRunningAppsWgMonitor::UpdateWidgetTaskListL( )
    {    
    RWidgetInfoArray widgetInfoArr;
    CleanupResetAndDestroyPushL( widgetInfoArr );
    
    iWidgetSession.RunningWidgetsL( widgetInfoArr );
    
    for (TInt i=0; i<widgetInfoArr.Count(); i++)
	    {
	    TUid wgUid( widgetInfoArr[i]->iUid );
	    if( iWidgetSession.IsWidgetInFullView( wgUid ) )
	    	{
	    	iRunningApps.AppendL( wgUid );
	    	}
	    }
    
    CleanupStack::PopAndDestroy( &widgetInfoArr ); // widgetInfoArr
    }
//  End of File  
