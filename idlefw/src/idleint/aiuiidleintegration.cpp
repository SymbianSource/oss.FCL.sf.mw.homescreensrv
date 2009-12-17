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
* Description:  Window server plug-in manager.
*
*/


#include "aiuiidleintegrationimpl.h"
#include "aifweventhandler.h"

#include <coemain.h>
#include <coeaui.h>
#include <eikenv.h>
#include <apgtask.h>
#include <AknIncallBubbleNotify.h>
#include <aknsoundsystem.h>
#include <apgwgnam.h> 
#include <AknDef.h>
#include <AknCapServerDefs.h> 
#include <startupdomainpskeys.h>
#include <aiutility.h>
#include <aipspropertyobserver.h>
#include <aisystemuids.hrh>

#include "aistate.h"
#include "aifwpanic.h"
#include "activeidle2domainpskeys.h"

#include <ctsydomainpskeys.h>

#include <AknSgcc.h>

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
    
    Release( iSystemStateObserver );
    
    Release( iUiStartupStateObserver );
    
    Release( iCallStatusObserver );
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
	iIncallBubble = CAknIncallBubble::NewL();
	
    iActiveIdleState = CActiveIdleState::NewL();
           
    // Set up keysounds
    if( aKeySoundConfig.iKeySounds )
        {
        aKeySoundConfig.iKeySounds
            ->PushContextL( aKeySoundConfig.iContextResId );            
        }
    
    // Set Active Idle application to be system application and disable
    // priority switching performed by window server.
    iEikEnv.SetSystem( ETrue );
    iEikEnv.WsSession().ComputeMode( RWsSession::EPriorityControlDisabled ); 
             
    // Eikon server window group
    iThisApplicationWgId = iEikEnv.RootWin().Identifier();

    iEikEnv.RootWin().EnableFocusChangeEvents(); 
    iEikEnv.RootWin().EnableGroupChangeEvents();
    
    CApaWindowGroupName::FindByAppUid( KAknCapServerUid, 
                                       iEikEnv.WsSession(), 
                                       iEikonServerWgId );
	           
    iActiveIdleState->SetIsIdleForeground( iThisApplicationForeground );
      
    iSystemStateObserver = AiUtility::CreatePSPropertyObserverL(                      
                            TCallBack( HandleSystemStateChange, this ),            
                                       KPSUidStartup, 
                                       KPSGlobalSystemState );
              
    iUiStartupStateObserver = AiUtility::CreatePSPropertyObserverL(                           
                                TCallBack( HandleUiStartupStateChange, this ),            
                                           KPSUidStartup, 
                                           KPSStartupUiPhase );        
                 
    iCallStatusObserver = AiUtility::CreatePSPropertyObserverL(          
                            TCallBack( HandleCallEvent, this ),  
                                       KPSUidCtsyCallInformation, 
                                       KCTsyCallState );  
    
    // Update state flags.    
    CAiUiIdleIntegrationImpl::HandleSystemStateChange( this );
    CAiUiIdleIntegrationImpl::HandleUiStartupStateChange( this );
    }

// ----------------------------------------------------------------------------
// CAiUiIdleIntegrationImpl::ActivateUI()
// ----------------------------------------------------------------------------
//
void CAiUiIdleIntegrationImpl::ActivateUI()
    {       
    if( iUiStartupPhaseOk && iSystemStateOk )
        {      
        RWsSession& wsSession( iEikEnv.WsSession() );
        
        TInt focusWgId( wsSession.GetFocusWindowGroup() );
        
        if( iThisApplicationWgId != focusWgId )
            {                       
            TApaTaskList taskList( wsSession );
            
            TApaTask task( taskList.FindApp( TUid::Uid( AI_UID3_AIFW_EXE ) ) );
            
            task.SendSystemEvent( EApaSystemEventBroughtToForeground );            
            }

        iAiFwEventHandler->HandleActivateUI();
        }    
    }

// ----------------------------------------------------------------------------
// CAiUiIdleIntegrationImpl::HandleWsEventL()
// ----------------------------------------------------------------------------
//
void CAiUiIdleIntegrationImpl::HandleWsEventL( const TWsEvent& aEvent, 
    CCoeControl* /*aDestination*/ )
    {
    if( !iSystemStateOk || !iUiStartupPhaseOk )
        {                
        return;
        }
      
    switch ( aEvent.Type() )
    	{
    	case KAknFullOrPartialForegroundGained:	
        	{
            if ( !iThisApplicationForeground )
            	{
                iThisApplicationForeground = ETrue;
                iActiveIdleState->SetIsIdleForeground( ETrue );
               	SetCallBubbleIfNeededL();
                }
          	break;
        	}
                
        case KAknFullOrPartialForegroundLost:
        	{
        	if ( iThisApplicationForeground )
            	{
            	iThisApplicationForeground = EFalse;
                iActiveIdleState->SetIsIdleForeground( EFalse );
                ClearCallBubbleL();
                }
        	break;
        	}
        
    	case EEventKeyDown:
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
    	    break;
    	    }
                
    	default:
    	    {
    		break;
    	    }
    	}
    }

// ----------------------------------------------------------------------------
// CAiUiIdleIntegrationImpl::SetCallBubbleIfNeededL()
// ----------------------------------------------------------------------------
//
void CAiUiIdleIntegrationImpl::SetCallBubbleIfNeededL()
	{
    if( !iIncallBubbleAllowed )
    	{
        TInt callStatus( 0 );
            	
        TInt err( RProperty::Get( KPSUidCtsyCallInformation,
                                  KCTsyCallState,
                                  callStatus ) );
        
        // Call ongoing => show bubble
	  	if( err == KErrNone && callStatus > EPSCTsyCallStateNone )
	      	{
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
	 if( iIncallBubbleAllowed )
    	{
    	iIncallBubble->SetIncallBubbleAllowedInIdleL( EFalse );
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
// CAiUiIdleIntegrationImpl::HandleSystemStateChange()
// ----------------------------------------------------------------------------
//
TInt CAiUiIdleIntegrationImpl::HandleSystemStateChange( TAny* aPtr )
    {
    __ASSERT_DEBUG( aPtr, 
            AiFwPanic::Panic( AiFwPanic::EAiFwPanic_NullPointerReference ) );
    
    CAiUiIdleIntegrationImpl* self = 
        static_cast<CAiUiIdleIntegrationImpl*>( aPtr );
           
    if( !self->iSystemStateOk )
        {
        TInt state( 0 );
        
        self->iSystemStateObserver->Get( state );               

        if ( state == ESwStateCriticalPhaseOK ||
             state == ESwStateNormalRfOn || 
             state == ESwStateNormalRfOff ||
             state == ESwStateNormalBTSap )        
            {
            self->iSystemStateOk = ETrue;            
            self->ActivateUI();
            }        
        }
              
    return KErrNone;
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
        static_cast<CAiUiIdleIntegrationImpl*>( aPtr );

    if( !self->iUiStartupPhaseOk )
        {
        TInt state( 0 );
        
        self->iUiStartupStateObserver->Get( state );
        
        if( state == EStartupUiPhaseAllDone )
            {
            self->iUiStartupPhaseOk = ETrue;
            
            self->ActivateUI();
            }
        }
    
    return KErrNone;
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
        static_cast<CAiUiIdleIntegrationImpl*>( aPtr );
        
	TInt callStatus( EPSCTsyCallStateNone );
	
	TInt err( self->iCallStatusObserver->Get( callStatus ) );
	
	if( err == KErrNone )
		{
		// Call ongoing => show bubble if not showing already
		TBool allowed = EFalse;
		
		if( !self->iIncallBubbleAllowed &&
		     self->iThisApplicationForeground &&
		    ( callStatus > EPSCTsyCallStateNone ) )
			{
			allowed = ETrue;
    		
			TRAP( err, 
                self->iIncallBubble->SetIncallBubbleAllowedInIdleL( allowed ) );
			        
    		
			if( err == KErrNone )
    		    {
    			self->iIncallBubbleAllowed = allowed;
    		    }
			}
		// No call ongoing => hide if bubble is visible			
		else if( self->iIncallBubbleAllowed && callStatus <= EPSCTsyCallStateNone )
			{
			allowed = EFalse;
			
    		TRAP( err, 
                self->iIncallBubble->SetIncallBubbleAllowedInIdleL( allowed ) );
    		
    		if( err == KErrNone )
    		    {
    			self->iIncallBubbleAllowed = allowed;
    		    }
			}
		}
	
	return err;
	}

// End of file.

