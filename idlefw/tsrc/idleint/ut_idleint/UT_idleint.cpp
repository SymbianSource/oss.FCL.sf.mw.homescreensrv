/*
* Copyright (c) 2004 Nokia Corporation and/or its subsidiary(-ies).
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
#include <e32base.h>
#include <bautils.h>
#include <e32property.h>
#include <eikapp.h>
#include <eikenv.h>
#include <vwsdef.h>
#include <akndef.h>

#include <digia/eunit/eunitmacros.h>
#include <digia/eunit/teunitassertioninfo.h>
#include <digia/eunit/eunitdecorators.h>

// User includes
#include "UT_IdleInt.h"
#include "caouserafter.h"
#include "aifweventhandler.h"

//#include <TelephonyInternalPSKeys.h>
#include <activeidle2domainpskeys.h>
#include <TelephonyDomainPSKeys.h>
#include <StartupDomainPSKeys.h>
#include <ctsydomainpskeys.h>
#define private public

// Tested classes
//#include "aifwphoneidlesynchronizer.h"
#include "aiuiidleintegration.h"
//#include "aifwphoneidlesynchronizerimpl.h"
#include "aistate.h"
#include "aiuiidleintegrationimpl.h"
#include "aiwspluginmanager.h"
#include "aiidleappregister.h"
#include "caipspropertyobserver.h"

class CNullEventHandler :
    public CBase,
    public MAiFwEventHandler
    {
public:
    // from MAiFwEventHandler
    void AppEnvReadyL()
        {
        };
    void HandleUiReadyEventL(CAiUiController& /*aUiController*/)
        {
        };

    void HandlePluginEvent(const TDesC& /*aParam*/)
        {
        };

    TBool HasMenuItem(const TDesC& /*aPluginName*/, const TDesC& /*aMenuItem*/)
        {
        	return ETrue;
        };

    void HandleUiLayoutChangeL(CAiUiController& /*aUiController*/)
        {
        };

    void HandleUiThemeChangeStartL(CAiUiController& /*aUiController*/)
        {
        };

    void HandleUiShutdown(CAiUiController& /*aUiController*/)
        {
        };

    void HandleAllPluginsLoaded()
        {
        };

    TBool RefreshContent(const TDesC& /*aContentCid*/)
        {
        	return ETrue;
        };    
        
    TBool RefreshContent( const THsPublisherInfo& /*aPublisherInfo*/, const TDesC& /*aContentCid*/ )
    {
    	return ETrue;
    };
    
    TBool SuspendContent( const THsPublisherInfo& /*aPublisherInfo*/, const TDesC& /*aContentCid*/ )
    {
    	return ETrue;
    };    

		/*
    void DestroyingPlugin( CAiContentPublisher& aPlugin )
        {
        };
		*/
		
    void CriticalStartupPhaseOver( TInt /*aStageInteger*/ )
        {
        };
    };
    
// CONSTRUCTION
UT_IdleInt* UT_IdleInt::NewL()
    {
    UT_IdleInt* self = UT_IdleInt::NewLC();
    CleanupStack::Pop();

    return self;
    }

UT_IdleInt* UT_IdleInt::NewLC()
    {
    UT_IdleInt* self = new( ELeave ) UT_IdleInt();
    CleanupStack::PushL( self );

    self->ConstructL();

    return self;
    }

// Destructor (virtual by CBase)
UT_IdleInt::~UT_IdleInt()
    {
    }

// Default constructor
UT_IdleInt::UT_IdleInt()
    {
    iPhoneStatusOwned = ETrue;
    iSystemStatusOwned = ETrue;
    iUiStartupStatusOwned = ETrue;
    }

// Second phase construct
void UT_IdleInt::ConstructL()
    {
    // The ConstructL from the base class CEUnitTestSuiteClass must be called.
    // It generates the test case table.
    CEUnitTestSuiteClass::ConstructL();
    }

//  METHODS

CPSPropertyObserver* UT_IdleInt::PSPropertyObserver( TUid aCategory, TInt aKey )
    {
    UT_IdleInt* self = reinterpret_cast<UT_IdleInt*>(Dll::Tls());
            
    if( aCategory == KPSUidCtsyCallInformation && aKey == KCTsyCallState )
        {        
        self->iPhoneStatusOwned = EFalse;
        return self->iPhoneStatusObserver;
        }
    else if( aCategory == KPSUidTelInformation && aKey == KTelPhoneUid )
        {        
        self->iPhoneStatusOwned = EFalse;
        return self->iPhoneStatusObserver;
        }
    else if( aCategory == KPSUidStartup && aKey == KPSGlobalSystemState )
        {
        self->iSystemStatusOwned = EFalse;
        return self->iSystemStatusObserver;
        }
    else if( aCategory == KPSUidStartup && aKey == KPSStartupUiPhase )
        {
        self->iUiStartupStatusOwned = EFalse;
        return self->iUiStartupStatusObserver;
        }
    else
        {
        return NULL;
        }
    }
    
void UT_IdleInt::KeySetTo( TInt aValue )
    {
    UT_IdleInt* self = reinterpret_cast<UT_IdleInt*>(Dll::Tls());
    self->iIdleStatus = aValue;
    }

void UT_IdleInt::SetupL(  )
    {
    iPhoneStatusObserver = CPSPropertyObserver::NewL(KPSUidAiInformation,
                                                     KActiveIdleUid);
    iSystemStatusObserver = CPSPropertyObserver::NewL(KPSUidStartup,
                                                      KPSGlobalSystemState);
    iUiStartupStatusObserver = CPSPropertyObserver::NewL(KPSUidStartup,
                                   KPSStartupUiPhase);

    iPhoneStatusObserver->SetOtherObserver( iSystemStatusObserver );
    iSystemStatusObserver->SetOtherObserver( iPhoneStatusObserver );
    iUiStartupStatusObserver->SetOtherObserver( iSystemStatusObserver );
    

    Dll::SetTls( this );
    TAiIdleKeySoundConfig config;
    config.iKeySounds = &iSoundSystemStub;
    iIdleInt = CAiUiIdleIntegrationImpl::NewL( *CEikonEnv::Static(), config, this );
    iState = CActiveIdleState::NewL();
    }

void UT_IdleInt::Teardown(  )
    {
    delete iState;
    delete iIdleInt;
    if( iPhoneStatusOwned )
        delete iPhoneStatusObserver;
    if( iSystemStatusOwned )
        delete iSystemStatusObserver;
    if( iUiStartupStatusOwned )
        delete iUiStartupStatusObserver;
    }

void UT_IdleInt::TestStartUpL()
    { 
    iIdleInt->ActivateUI();
    iSystemStatusObserver->DoEvent();
    TWsEvent event;
    event.SetType( KAknFullOrPartialForegroundGained );
    iIdleInt->HandleWsEventL( event, NULL );
    CAOUserAfter::AfterLD( 1000*1000 ); // wait a second for ps to update
//    EUNIT_ASSERT( iIdleStatus != 0 ); // assert we have foreground
                                      // means that status updated correctly

    event.SetType( KAknFullOrPartialForegroundGained );
    iIdleInt->HandleWsEventL( event, NULL );
    CAOUserAfter::AfterLD( 1000*1000 ); // wait a second for ps to update
//    EUNIT_ASSERT( iIdleStatus != 0 ); // assert we have foreground
                                      // means that status updated correctly
    
    event.SetType( KAknFullOrPartialForegroundLost );
    iIdleInt->HandleWsEventL( event, NULL );
    CAOUserAfter::AfterLD( 1000*1000 ); // wait a second for ps to update
//    EUNIT_ASSERT( iIdleStatus == 0 ); // assert we dont have foreground
                                      // means that status updated correctly

    event.SetType( KAknFullOrPartialForegroundLost );
    iIdleInt->HandleWsEventL( event, NULL );
    CAOUserAfter::AfterLD( 1000*1000 ); // wait a second for ps to update
//    EUNIT_ASSERT( iIdleStatus == 0 ); // assert we dont have foreground
                                      // means that status updated correctly
                
    iAiWsPluginManagerImpl = CAiWsPluginManager::NewL( *CCoeEnv::Static() );
      CAOUserAfter::AfterLD( 1000*1000 ); // wait a second for ps to update
      
    iAiIdleAppRegister = CAiIdleAppRegister::NewLC();
      CAOUserAfter::AfterLD( 1000*1000 ); // wait a second for ps to update
    
    iAiIdleAppRegister->RegisterL();
      CAOUserAfter::AfterLD( 1000*1000 ); // wait a second for ps to update

      if (iAiIdleAppRegister)
          {
          delete iAiIdleAppRegister;    
          iAiIdleAppRegister = NULL;    
          }
      
      if (iAiWsPluginManagerImpl)
          {
          delete iAiWsPluginManagerImpl;    
          iAiWsPluginManagerImpl = NULL;    
          }


    }    
    
void UT_IdleInt::TestIdleIntEventControlNullAndOthersL()
    {
    TWsEvent event;
    iIdleInt->HandleWsEventL( event, NULL );

    event.SetType( EEventWindowGroupsChanged );
    iIdleInt->HandleWsEventL( event, NULL );

    event.SetType( EEventFocusGroupChanged );
    iIdleInt->HandleWsEventL( event, NULL );

    event.SetType( EEventFocusGained );
    iIdleInt->HandleWsEventL( event, NULL );
    }
//  TEST TABLE

void UT_IdleInt::TestStateChangesL()
    {
    iState->SetIsIdleForeground( ETrue );
    CAOUserAfter::AfterLD( 1 * 500000 );
    iState->SetIsIdleForeground( EFalse );
    CAOUserAfter::AfterLD( 1 * 500000 );
    iState->SetIsIdleForeground( ETrue );
    CAOUserAfter::AfterLD( 1 * 500000 );
    iState->SetIsIdleForeground( EFalse );
    iState->SetIsIdleForeground( ETrue );
    iState->SetIsIdleForeground( EFalse );
    iState->SetIsIdleForeground( ETrue ); 
    iState->SetIsIdleForeground( EFalse );
    }

void UT_IdleInt::AppEnvReadyL()
    {
    }

void UT_IdleInt::HandleUiReadyEventL( CAiUiController& /*aUiController*/ )
    {
    }

void UT_IdleInt::HandlePluginEvent( const TDesC& /*aParam*/ )
    {
    }

TBool UT_IdleInt::HasMenuItem(const TDesC& /*aPluginName*/, const TDesC& /*aMenuItem*/)
    {
    return ETrue;
    }

void UT_IdleInt::HandleUiLayoutChangeL( CAiUiController& /*aUiController*/ )
    {
    }

void UT_IdleInt::HandleUiThemeChangeStartL( CAiUiController& /*aUiController*/ )
    {
    }

void UT_IdleInt::HandleUiShutdown( CAiUiController& /*aUiController*/ )
    {
    }

void UT_IdleInt::HandleAllPluginsLoaded()
    {
    }

void UT_IdleInt::HandleEnterEditMode()
    {
    }

void UT_IdleInt::HandleExitEditMode()
    {
    }

void UT_IdleInt::HandleUIChangeEventL()
    {
    }

void UT_IdleInt::HandleActivateUI()
    {
    }

void UT_IdleInt::HandleLoadPluginL( const THsPublisherInfo& /*aPublisherInfo*/ )
    {
    }

void UT_IdleInt::HandleDestroyPluginL( const THsPublisherInfo& /*aPublisherInfo*/ )
    {
    }

void UT_IdleInt::HandlePluginEventL( const THsPublisherInfo& /*aPublisherInfo*/, const TDesC& /*aParam*/ )
    {
    }

TBool UT_IdleInt::HasMenuItemL( const THsPublisherInfo& /*aPublisherInfo*/, const TDesC& /*aMenuItem*/ )
    {
    }

TBool UT_IdleInt::IsPluginsOnline()
    {
    return ETrue;
    }

void UT_IdleInt::SetPluginsOnlineStatus(TBool /*aStatus*/)
    {
    }

TBool UT_IdleInt::IsRoaming()
    {
    return ETrue;
    }

void UT_IdleInt::ShowRoamingNotificationL()
    {
    }

TBool UT_IdleInt::RefreshContent( const TDesC& /*aContentCid*/ )
    {
    return ETrue;
    }

TBool UT_IdleInt::RefreshContent( const THsPublisherInfo& /*aPublisherInfo*/, const TDesC& /*aContentCid*/ )
{
	return ETrue;
}
    
TBool UT_IdleInt::SuspendContent( const THsPublisherInfo& /*aPublisherInfo*/, const TDesC& /*aContentCid*/ )
{
	return ETrue;
};   
/*
void UT_IdleInt::DestroyingPlugin( CAiContentPublisher& aPlugin )
    {
    }
*/
void UT_IdleInt::CriticalStartupPhaseOver( TInt /*aStageInteger*/ )
    {
    }

TBool UT_IdleInt::QueryIsMenuOpen()
    {
    return ETrue;
    }

/*
void UT_IdleInt::ProcessStateChange( TAifwStates aState )
	{
	}
*/
/*
void UT_IdleInt::HandlePluginsOnlineStatus( 
    const RAiPublisherInfoArray& aPlugins,TBool aStatus )
    {
    }
*/
void UT_IdleInt::SetPluginsVisibility( TBool /*aStatus*/ )
    {
    }

void UT_IdleInt::ProcessOnlineState( TBool /*aOnline*/ )
    {
    }


EUNIT_BEGIN_TEST_TABLE(
    UT_IdleInt,
    "Unit test suite for class Active Idle Framework",
    "CAiFw" )

EUNIT_TEST(
    "Test IdleInt event control NULL",
    "CAiUiIdleIntegrationImpl",
    "",
    "FUNCTIONALITY",
    SetupL, TestIdleIntEventControlNullAndOthersL, Teardown)

EUNIT_TEST(
    "Test State changes",
    "CActiveIdleState",
    "",
    "FUNCTIONALITY",
    SetupL, TestStateChangesL, Teardown)

    /*
EUNIT_TEST(
    "Test Start Up",
    "CAiFwPhoneIdleSynchronizerImpl",
    "",
    "FUNCTIONALITY",
    SetupL, TestStartUpL, Teardown)   
*/

EUNIT_END_TEST_TABLE

//  END OF FILE
