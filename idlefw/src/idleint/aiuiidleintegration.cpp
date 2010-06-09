/*
* Copyright (c) 2006-2007 Nokia Corporation and/or its subsidiary(-ies).
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
* Description:  Idle integration
*
*/


// System includes
#include <startupdomainpskeys.h>
#include <coeaui.h>
#include <eikenv.h>
#include <AknIncallBubbleNotify.h>
#include <aknsoundsystem.h>
#include <AknDef.h>
#include <ctsydomainpskeys.h>
#include <apgtask.h>


// User includes
#include <aisystemuids.hrh>
#include <aiutility.h>
#include <aipspropertyobserver.h>
#include "activeidle2domainpskeys.h"
#include "aiuiidleintegrationimpl.h"
#include "aifweventhandler.h"
#include "aistate.h"
#include "aifwpanic.h"
#include "debug.h"


// ======== MEMBER FUNCTIONS ========

// ----------------------------------------------------------------------------
// CAiUiIdleIntegrationImpl::NewL()
// ----------------------------------------------------------------------------
//
CAiUiIdleIntegrationImpl* CAiUiIdleIntegrationImpl::NewL( CEikonEnv& aEikEnv,         
    const TAiIdleKeySoundConfig& aKeySoundConfig,
    MAiFwEventHandler* aAiFwEventHandler )
    {
    CAiUiIdleIntegrationImpl* self = 
        new ( ELeave ) CAiUiIdleIntegrationImpl( aEikEnv, aAiFwEventHandler );
                                              
    CleanupStack::PushL( self );
    self->ConstructL( aKeySoundConfig );
    CleanupStack::Pop( self );
    return self;
    }

// ----------------------------------------------------------------------------
// CAiUiIdleIntegrationImpl::~CAiUiIdleIntegrationImpl()
// ----------------------------------------------------------------------------
//
CAiUiIdleIntegrationImpl::~CAiUiIdleIntegrationImpl()
    {
    delete iActiveIdleState;
    
    delete iIncallBubble;
    
    Release( iCallStatusObserver );       
    
    Release( iUiStartupStateObserver );
    }

// ----------------------------------------------------------------------------
// CAiUiIdleIntegrationImpl::CAiUiIdleIntegrationImpl()
// ----------------------------------------------------------------------------
//
CAiUiIdleIntegrationImpl::CAiUiIdleIntegrationImpl( CEikonEnv& aEikEnv,
    MAiFwEventHandler* aAiFwEventHandler )
    : iEikEnv( aEikEnv ),
      iAiFwEventHandler( aAiFwEventHandler )
    {
    }

// ----------------------------------------------------------------------------
// CAiUiIdleIntegrationImpl::ConstructL()
// ----------------------------------------------------------------------------
//
void CAiUiIdleIntegrationImpl::ConstructL(
    const TAiIdleKeySoundConfig& aKeySoundConfig )       
    {   
    __PRINTS( "*** CAiUiIdleIntegrationImpl::ConstructL" );
    __TIME_MARK( time );
    
	iIncallBubble = CAknIncallBubble::NewL();
	
    iActiveIdleState = CActiveIdleState::NewL();
           
    // Set up keysounds
    if( aKeySoundConfig.iKeySounds )
        {
        aKeySoundConfig.iKeySounds->PushContextL( 
            aKeySoundConfig.iContextResId );                       
        }
    
    iEikEnv.SetSystem( ETrue );
             
    RWindowGroup& windowGroup = iEikEnv.RootWin();
    windowGroup.AutoForeground(EFalse);
    TInt wgId( windowGroup.Identifier() );
    TInt focusWgId( iEikEnv.WsSession().GetFocusWindowGroup() );
    
    if ( focusWgId == wgId )
        {
        __PRINTS( "*** CAiUiIdleIntegrationImpl::ConstructL - iForeground: 1" );
    
        iForeground = ETrue;
        }
    
    iActiveIdleState->SetIsIdleForeground( iForeground );
      
    iCallStatusObserver = AiUtility::CreatePSPropertyObserverL(          
                            TCallBack( HandleCallEvent, this ),  
                                       KPSUidCtsyCallInformation, 
                                       KCTsyCallState );  
    
    iUiStartupStateObserver = AiUtility::CreatePSPropertyObserverL(                           
                                TCallBack( HandleUiStartupStateChange, this ),            
                                           KPSUidStartup, 
                                           KPSStartupUiPhase );        
    
    HandleUiStartupStateChange( this );
                 
    __TIME_ENDMARK( "CAiUiIdleIntegrationImpl::ConstructL, done", time );
    }

// ----------------------------------------------------------------------------
// CAiUiIdleIntegrationImpl::ActivateUI()
// ----------------------------------------------------------------------------
//
void CAiUiIdleIntegrationImpl::ActivateUI()
    {   
    __TICK( "CAiUiIdleIntegrationImpl::ActivateUI" );
    
    Release( iUiStartupStateObserver );
    iUiStartupStateObserver = NULL;
    
    iAiFwEventHandler->HandleActivateUI();
    
    RWsSession& wsSession( iEikEnv.WsSession() );
    
    TApaTaskList taskList( wsSession );
    
    TApaTask task( taskList.FindApp( TUid::Uid( AI_UID3_AIFW_EXE ) ) );
    
    task.BringToForeground(); 
        
    __PRINTS( "*** CAiUiIdleIntegrationImpl::ActivateUI - done" );    
    }

// ----------------------------------------------------------------------------
// CAiUiIdleIntegrationImpl::HandleWsEventL()
// ----------------------------------------------------------------------------
//
void CAiUiIdleIntegrationImpl::HandleWsEventL( const TWsEvent& aEvent, 
    CCoeControl* /*aDestination*/ )
    {
    TInt type( aEvent.Type() );
    
    if ( type == KAknFullOrPartialForegroundGained )
        {
        __PRINTS( "*** CAiUiIdleIntegrationImpl::HandleWsEventL - Foreground" );
        
        if ( !iForeground )
            {
            iForeground = ETrue;
            iActiveIdleState->SetIsIdleForeground( ETrue );
            SetCallBubbleIfNeededL();
            }    
        }
    else if ( type == KAknFullOrPartialForegroundLost )
        {
        __PRINTS( "*** CAiUiIdleIntegrationImpl::HandleWsEventL - Background" );
    
        if ( iForeground )
            {
            iForeground = EFalse;
            iActiveIdleState->SetIsIdleForeground( EFalse );
            ClearCallBubbleL();
            }    
        }
    else if ( type == EEventKeyDown )
        {
        if( aEvent.Key()->iScanCode == EStdKeyDevice0 )
            {
            TBool isDialog( iEikEnv.AppUi()->IsDisplayingMenuOrDialog() );
            
            if( isDialog || iAiFwEventHandler->QueryIsMenuOpen() )
                {
                RProperty::Set(
                    KPSUidAiInformation, 
                    KActiveIdlePopupState, 
                    EPSAiDisplayingMenuOrDialog );
                }
            else
                {
                RProperty::Set(
                    KPSUidAiInformation, 
                    KActiveIdlePopupState, 
                    EPSAiNotDisplayingMenuOrDialog );
                }    
            }
        }        
    }

// ----------------------------------------------------------------------------
// CAiUiIdleIntegrationImpl::SetCallBubbleIfNeededL()
// ----------------------------------------------------------------------------
//
void CAiUiIdleIntegrationImpl::SetCallBubbleIfNeededL()
	{
    __PRINTS( "*** CAiUiIdleIntegrationImpl::SetCallBubbleIfNeededL" );
    
    if( !iIncallBubbleAllowed )
    	{
        TInt callStatus( 0 );
            	
        TInt err( RProperty::Get( KPSUidCtsyCallInformation,
                                  KCTsyCallState,
                                  callStatus ) );
        
        // Call ongoing => show bubble
	  	if( err == KErrNone && callStatus > EPSCTsyCallStateNone )
	      	{
			__PRINTS( "*** CAiUiIdleIntegrationImpl::SetCallBubbleIfNeededL - enable" );

            iIncallBubble->SetIncallBubbleAllowedInUsualL( ETrue );
            iIncallBubble->SetIncallBubbleAllowedInIdleL( ETrue );
            iIncallBubbleAllowed = ETrue;
	       	}    
    	}
	}

// ----------------------------------------------------------------------------
// CAiUiIdleIntegrationImpl::ClearCallBubbleL()
// ----------------------------------------------------------------------------
//
void CAiUiIdleIntegrationImpl::ClearCallBubbleL()
	{
    __PRINTS( "*** CAiUiIdleIntegrationImpl::ClearCallBubbleL" );

	 if( iIncallBubbleAllowed )
    	{
         __PRINTS( "*** CAiUiIdleIntegrationImpl::ClearCallBubbleL - disable" );

    	iIncallBubble->SetIncallBubbleAllowedInIdleL( EFalse );
    	iIncallBubble->SetIncallBubbleAllowedInUsualL( EFalse );
    	iIncallBubbleAllowed = EFalse;
    	}
	}
    
// ----------------------------------------------------------------------------
// CAiUiIdleIntegration::NewL()
// ----------------------------------------------------------------------------
//
EXPORT_C CAiUiIdleIntegration* CAiUiIdleIntegration::NewL( CEikonEnv& aEikEnv, 
    const TAiIdleKeySoundConfig& aKeySoundConfig,
    MAiFwEventHandler* aAiFwEventHandler )
    {
    return CAiUiIdleIntegrationImpl::NewL( aEikEnv, aKeySoundConfig,                                             
                                           aAiFwEventHandler );
    }

// ----------------------------------------------------------------------------
// CAiUiIdleIntegrationImpl::HandleCallEvent()
// ----------------------------------------------------------------------------
//
TInt CAiUiIdleIntegrationImpl::HandleCallEvent( TAny* aPtr )
	{
	__ASSERT_DEBUG( aPtr, 
        AiFwPanic::Panic( AiFwPanic::EAiFwPanic_NullPointerReference ) );
    
	CAiUiIdleIntegrationImpl* self = 
        static_cast< CAiUiIdleIntegrationImpl* >( aPtr );
        
	TInt callStatus( EPSCTsyCallStateNone );
	
	TInt err( self->iCallStatusObserver->Get( callStatus ) );
	
	if ( err == KErrNone )
		{
		// Call ongoing => show bubble if not showing already
		if ( !self->iIncallBubbleAllowed &&
		     self->iForeground &&
		    ( callStatus > EPSCTsyCallStateNone ) )
			{
            TRAP_IGNORE( self->SetCallBubbleIfNeededL() );
			}
		// No call ongoing => hide if bubble is visible			
		else if ( self->iIncallBubbleAllowed && callStatus <= EPSCTsyCallStateNone )
			{
    		TRAP_IGNORE( self->ClearCallBubbleL() );
			}
		}
	
	return err;
	}

// ----------------------------------------------------------------------------
// CAiUiIdleIntegrationImpl::HandleUiStartupStateChange()
// ----------------------------------------------------------------------------
//
TInt CAiUiIdleIntegrationImpl::HandleUiStartupStateChange( TAny *aPtr )
    {
    __ASSERT_DEBUG( aPtr, 
        AiFwPanic::Panic( AiFwPanic::EAiFwPanic_NullPointerReference ) );
    
    CAiUiIdleIntegrationImpl* self = 
        static_cast< CAiUiIdleIntegrationImpl* >( aPtr );

    if ( !self->iUiStartupPhaseOk )
        {
        TInt state( 0 );
        
        self->iUiStartupStateObserver->Get( state );
        
        if ( state == EStartupUiPhaseAllDone )
            {
            self->iUiStartupPhaseOk = ETrue;
            
            self->ActivateUI();                        
            }
        }
    
    return KErrNone;
    }

// End of file
