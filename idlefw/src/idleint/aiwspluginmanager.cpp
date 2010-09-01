/*
* Copyright (c) 2006 Nokia Corporation and/or its subsidiary(-ies).
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
* Description:  Window server plug-in manager.
*
*/


#include "aiwspluginmanagerimpl.h"
#include <coemain.h>
#include <apgtask.h>
#include <aiutility.h>
#include <telinformationpskeys.h>
#include <aipspropertyobserver.h>

CAiWsPluginManagerImpl::CAiWsPluginManagerImpl(RWsSession& aWsSession) :
    iAnimDll( aWsSession ),
    iWnd( aWsSession ),
    iAnim( iAnimDll )
    {
    }

void CAiWsPluginManagerImpl::ConstructL(CCoeEnv& aCoeEnv)
    {
    User::LeaveIfError( iAnimDll.Load( KAiWsPluginAnimDllName ) );
    iCoeEnv = &aCoeEnv;
    iPhoneStatusObserver = AiUtility::CreatePSPropertyObserverL
        ( TCallBack( HandlePhoneEvent, this ),
            KPSUidTelInformation, KTelPhoneUid );
    TryLoadWsPluginL();
    }

CAiWsPluginManagerImpl* CAiWsPluginManagerImpl::NewL( CCoeEnv& aCoeEnv )
    {
    CAiWsPluginManagerImpl* self = 
        new(ELeave) CAiWsPluginManagerImpl( aCoeEnv.WsSession() );
    CleanupStack::PushL( self );
    self->ConstructL( aCoeEnv );
    CleanupStack::Pop( self );
    return self;
    }

CAiWsPluginManagerImpl::~CAiWsPluginManagerImpl()
    {
    iAnim.Close();
    iWnd.Close();
    Release( iPhoneStatusObserver );
    iAnimDll.Close();
    }

EXPORT_C CAiWsPluginManager* CAiWsPluginManager::NewL
        (CCoeEnv& aCoeEnv)
    {
    return CAiWsPluginManagerImpl::NewL(aCoeEnv);
    }

TInt CAiWsPluginManagerImpl::HandlePhoneEvent( TAny* aPtr )
	{
    CAiWsPluginManagerImpl* self = 
        static_cast<CAiWsPluginManagerImpl*>( aPtr );
    TInt err = KErrNone;
    if( self )
        {
        TRAP( err, self->TryLoadWsPluginL() );
        }
    return err;
	}
	
void CAiWsPluginManagerImpl::TryLoadWsPluginL()
    {
    TInt phoneUidVal;
    iPhoneStatusObserver->Get( phoneUidVal );
    const TUid phoneUid = TUid::Uid( phoneUidVal );
    
    if ( phoneUid != KNullUid )
        {
	    RWsSession& wsSession = iCoeEnv->WsSession();
		TApaTaskList taskList( wsSession );
		TApaTask phoneTask( taskList.FindApp( phoneUid ) );
		if ( phoneTask.Exists() )
			{
            iAnim.Close();
            iWnd.Close();
        
         	RWindowGroup& aiRootWg = iCoeEnv->RootWin();
    	    // RWindow::Construct requires a unique non-null handle. Active Idle
    	    // does not care about the handle value and just uses this pointer to have
    	    // something non-null.
    	    const TUint32 dummyHandle = reinterpret_cast<TUint32>(this);
  	        User::LeaveIfError( iWnd.Construct(aiRootWg, dummyHandle ) );

    	    const TInt aiWgId = aiRootWg.Identifier();
    	    const TInt phoneWgId = phoneTask.WgId();
    	    User::LeaveIfError( iAnim.Construct( iWnd, aiWgId, phoneWgId ) );
    	    
    	    // No need to monitor Phone app status anymore
    	    Release( iPhoneStatusObserver );
    	    iPhoneStatusObserver = NULL;
			}
	    }
    }
    
