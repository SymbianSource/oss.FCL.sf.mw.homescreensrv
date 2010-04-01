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
* Description:  System state observer and notifier
*
*/


#include <connect/sbdefs.h>
#include <aipspropertyobserver.h>
#include <settingsinternalcrkeys.h> // for KSettingsScreenSaverPeriod
#include <e32property.h>
#include <aiutility.h>
#include <e32base.h>
#include <centralrepository.h>
#include <activeidle2domaincrkeys.h>
#include "hs_app_aifwpanic.h"
#include "hs_app_aipluginstatemanager.h"
#include "hs_app_aipluginstatemachineimpl.h"
#include "hs_app_ailightstatusobserver.h"
#include "hs_app_aicallstatusobserver.h"
#include "hs_app_aifocusobserver.h"
#include "hs_app_aikeylockobserver.h"
#include "hs_app_aibackuprestorestatusobserver.h"
#include "hs_app_aienvironmentchangeobserver.h"
#include "hs_app_aiuiframeworkobserverimpl.h"

#include <ScreensaverInternalPSKeys.h> // this include needs to be last

#include "debug.h"

const TInt KMinuteInSeconds( 60 );
const TInt KSecondInMikroSeconds( 1000*1000 );
const TInt KAIFadeOutEstimateMikroSeconds( 10 * KSecondInMikroSeconds );
const TInt KTwoMinutesInMikroSeconds( 2 * KMinuteInSeconds * KSecondInMikroSeconds );

#define AI2_OPTION_RESUME_AT_CREATION

// ======== MEMBER FUNCTIONS ========

// ----------------------------------------------------------------------------
// CAiPluginStateManager::NewL()
// ----------------------------------------------------------------------------
//
CAiPluginStateManager* CAiPluginStateManager::NewL()
    {
    CAiPluginStateManager* self = new (ELeave) CAiPluginStateManager;
    
    CleanupStack::PushL( self );
    self->ConstructL();
    CleanupStack::Pop( self );
    
    return self;
    }

// ----------------------------------------------------------------------------
// CAiPluginStateManager::~CAiPluginStateManager()
// ----------------------------------------------------------------------------
//
CAiPluginStateManager::~CAiPluginStateManager()
    {       
    iStateMachines.ResetAndDestroy();
    
    DestroySystemStateObservers();
    }

// ----------------------------------------------------------------------------
// CAiPluginStateManager::CAiPluginStateManager()
// ----------------------------------------------------------------------------
//
CAiPluginStateManager::CAiPluginStateManager()
    {
    }

// ----------------------------------------------------------------------------
// CAiPluginStateManager::ConstructL()
// ----------------------------------------------------------------------------
//
void CAiPluginStateManager::ConstructL()
    {
    TInt value( 0 );
    
    TInt err( RProperty::Get( KCRUidPersonalizationSettings, 
                              KSettingsScreenSaverPeriod, value ) );
                               
    if( err == KErrNone )
        {
        iT1Delay = ( value * KMinuteInSeconds * KSecondInMikroSeconds )
                   + KAIFadeOutEstimateMikroSeconds;
        }
    else
        {
        // default when error to 2 minutes
        iT1Delay = KTwoMinutesInMikroSeconds;
        }    
    }

// ----------------------------------------------------------------------------
// CAiPluginStateManager::CreateSystemStateObserversL()
// ----------------------------------------------------------------------------
//
void CAiPluginStateManager::CreateSystemStateObserversL()
    {
    iT1Timer = CPeriodic::NewL( CActive::EPriorityStandard );

    iBackupOperationObserver = CAiBackupRestoreStatusObserver::NewL( this );

    iCallStateObserver = CAiCallStatusObserver::NewL( this );

    iLightStateObserver = CAiLightStatusObserver::NewL( this );

    iFocusObserver = CAiFocusObserver::NewL( this );

    iKeylockObserver = CAiKeylockObserver::NewL( this );

    // Environment change observer notifies time/date/midnight/language
    // changes
    iEnvironmentObserver = CAiEnvironmentChangeObserver::NewL( this );

    // Ui framework observer notifies currently general theme changes
    iFrameworkObserver = CAiUiFrameworkObserverImpl::NewL( *this );
    }

// ----------------------------------------------------------------------------
// CAiPluginStateManager::DestroySystemStateObservers()
// ----------------------------------------------------------------------------
//
void CAiPluginStateManager::DestroySystemStateObservers()
    {
    if( iT1Timer )
        {
        iT1Timer->Cancel();
        
        delete iT1Timer;       
        iT1Timer = NULL;
        }

    delete iBackupOperationObserver;
    iBackupOperationObserver = NULL;

    delete iCallStateObserver;
    iCallStateObserver = NULL;

    delete iLightStateObserver;
    iLightStateObserver = NULL;

    delete iFocusObserver;
    iFocusObserver = NULL;

    delete iKeylockObserver;
    iKeylockObserver = NULL;

    delete iEnvironmentObserver;
    iEnvironmentObserver = NULL;

    delete iFrameworkObserver;
    iFrameworkObserver = NULL;
    }

// ----------------------------------------------------------------------------
// CAiPluginStateManager::UiFwObserver()
// ----------------------------------------------------------------------------
//
MAiUiFrameworkObserver* CAiPluginStateManager::UiFwObserver() const
    {
    return iFrameworkObserver;
    }
    
// ----------------------------------------------------------------------------
// CAiPluginStateManager::ReportStateChange()
// ----------------------------------------------------------------------------
//
void CAiPluginStateManager::ReportStateChange( TAiStateChanges aState )
    {
    if( !iIsDeviceStarted ) 
        {
        if( aState == ESMAISystemBoot )
            {
            iIsDeviceStarted = ETrue;
            }               
        }
    
    if( aState == ESMAISystemBoot )
        {
        return;
        }
        
    ProcessStateChangeForAll( aState );
    }

// ----------------------------------------------------------------------------
// CAiPluginStateManager::StateVariable()
// ----------------------------------------------------------------------------
//
TBool CAiPluginStateManager::StateVariable( TAiStateVariable aStateChange )
    {
    switch ( aStateChange )
        {
        case ESMAICallStatus:
            {
            return CallOngoing();
            }
        case ESMAILightStatus:
            {
            return LightsOn();
            }
        case ESMAIBackupRestoreStatus:
            {
            return BackupOngoing();
            }
        case ESMAIIdleFocusStatus:
            {
            return IdleFocused();
            }
        default:
            {
            return EFalse;
            }
        }
    }

// ----------------------------------------------------------------------------
// CAiPluginStateManager::RestartSuspendTimer()
// ----------------------------------------------------------------------------
//
void CAiPluginStateManager::RestartSuspendTimer()
    {
    if( iT1Timer )
        {
        iT1Timer->Cancel();
        
        iT1Timer->Start( iT1Delay, iT1Delay,                         
                         TCallBack( T1TimerCallback, this ) );
        }
    }

// ----------------------------------------------------------------------------
// CAiPluginStateManager::PluginCreatedL()
// ----------------------------------------------------------------------------
//
void CAiPluginStateManager::PluginCreatedL( CAiContentPublisher& aPlugin )
    {
    // Create a new state machine for the plugin from the heap
    // and append the machine to our local array.
    CAiPluginStateMachine* machine = 
        new ( ELeave ) CAiPluginStateMachine( *this, aPlugin );                                                                                                                                              

    // Important to append first so failure will be handled properly
    CleanupStack::PushL( machine );
    iStateMachines.AppendL( machine );
    CleanupStack::Pop( machine );
              
    // This will effectively resume the plugin NOW.
    ProcessStateChange( ESMAISystemBoot, *machine );
    }

// ----------------------------------------------------------------------------
// CAiPluginStateManager::PluginDestroyed()
// ----------------------------------------------------------------------------
//
void CAiPluginStateManager::PluginDestroyed( CAiContentPublisher& aPlugin )
    {
    // plugin has beed destroyed, remove the state machine also
    for( TInt i = 0; i < iStateMachines.Count(); i++ )
        {
        if( &iStateMachines[i]->Plugin() == &aPlugin )
            {
            iStateMachines[i]->HandleEvent( ESMAISystemShutdown );
            delete iStateMachines[i];
          
            iStateMachines.Remove( i );                                    
            break;
            }
        }
    }

// ----------------------------------------------------------------------------
// CAiPluginStateManager::AllPluginsCreated()
// ----------------------------------------------------------------------------
//
void CAiPluginStateManager::AllPluginsCreated()
    {
    // Currently we get this event via pluginmanager -> aifw -> EAISMSystemBoot event
    // so no implementation required. We might want to handle this locally in the 
    // future though. Current impl is such because RefreshUI for ui controller
    // needs to after the plugins are resumed -> If we handle this here it is not
    // garanteed.
    }

// ----------------------------------------------------------------------------
// CAiPluginStateManager::AllPluginsDestroyed()
// ----------------------------------------------------------------------------
//
void CAiPluginStateManager::AllPluginsDestroyed()
    {
    // Plugins have been destroyed, so destroy the state machines also.
    iStateMachines.ResetAndDestroy();
    }

// ----------------------------------------------------------------------------
// CAiPluginStateManager::TranslateReason()
// ----------------------------------------------------------------------------
//
TAiTransitionReason CAiPluginStateManager::TranslateReason( 
    TAiStateChanges aStateChange )
    {
    switch ( aStateChange )
        {
        case ESMAIBacklightOn:
            {
            return EAiBacklightOn;
            }
        case ESMAIBacklightOff:
            {
            return EAiBacklightOff;
            }
        case ESMAIBackupOn:
            {
            return EAiBackupRestoreStarted;
            }
        case ESMAIBackupOff:
            {
            return EAiBackupRestoreEnded;
            }
        case ESMAIInCall:
            {
            return EAiPhoneCallStarted;
            }
        case ESMAINoCall:
            {
            return EAiPhoneCallEnded;
            }
        case ESMAISystemBoot:
            {
            return EAiSystemStartup;
            }
        case ESMAILocaleChanged:
            {
            return EAiLanguageChanged;
            }
        case ESMAIIdleForeground:
            {
            return EAiIdleForeground;
            }
        case ESMAIIdleBackground:
            {
            return EAiIdleBackground;
            }
        case ESMAITimeChanged:
            {
            return EAiTimeChanged;
            }
        case ESMAIMidnightCrossover:
            {
            return EAiMidnightPassed;
            }
        case ESMAIRelayoutScreen:
            {
            return EAiScreenLayoutChanged;
            }
        case ESMAIReportThemeChangeStarted:
            {
            return EAiUiDefinitionChangeStarted;
            }
        case ESMAIReportThemeChangeReady:
            {
            return EAiUiDefinitionChangeEnded;
            }
        case ESMAIGeneralThemeChanged:
            {
            return EAiGeneralThemeChanged;
            }
        case ESMAISystemShutdown:
            {
            return EAiSystemShutdown;
            }
        case ESMAIT1Timeout:
            {
            return EAiSuspendPlugins;
            }
        case ESMAIKeylockEnabled:
            {
            return EAiKeylockEnabled;
            }
        case ESMAIKeylockDisabled:
            {
            return EAiKeylockDisabled;
            }
        case ESMAIOffLine:
            {
            return EAiIdleOffLine;
            }
        case ESMAIOnLine:
            {
            return EAiIdleOnLine;
            }   
        case ESMAIPageSwitch:
        	{
        	return EAiIdlePageSwitch;
        	}
        case ESMAIUnknownState: // fallthrough
        default:
            {
            return EAiUnknownTransitionReason;
            }
        }
    }

// ----------------------------------------------------------------------------
// CAiPluginStateManager::ProcessStateChange()
// ----------------------------------------------------------------------------
//
void CAiPluginStateManager::ProcessStateChange( TAiStateChanges aState,
    CAiPluginStateMachine& aMachine )
    {
    aMachine.HandleEvent( aState );    
    }

// ----------------------------------------------------------------------------
// CAiPluginStateManager::ProcessOnlineState()
// ----------------------------------------------------------------------------
//
void CAiPluginStateManager::ProcessOnlineState( CAiContentPublisher& aPlugin ) 
    {
    for ( TInt i = 0; i < iStateMachines.Count(); i++ )
        {
        if( &iStateMachines[i]->Plugin() == &aPlugin )
            {
            iStateMachines[i]->HandleEvent( ESMAIOnLine );            
            break;
            }
        }
    }

// ----------------------------------------------------------------------------
// CAiPluginStateManager::ProcessOfflineState()
// ----------------------------------------------------------------------------
//
void CAiPluginStateManager::ProcessOfflineState( CAiContentPublisher& aPlugin )
    {
    for ( TInt i = 0; i < iStateMachines.Count(); i++ )
        {
        if( &iStateMachines[i]->Plugin() == &aPlugin )
            {
            iStateMachines[i]->HandleEvent( ESMAIOffLine );            
            break;
            }        
        }
    }

// ----------------------------------------------------------------------------
// CAiPluginStateManager::ProcessStateChangeForAll()
// ----------------------------------------------------------------------------
//
void CAiPluginStateManager::ProcessStateChangeForAll( TAiStateChanges aState )
    {
    for ( TInt i = 0; i < iStateMachines.Count(); ++i )
        {
        iStateMachines[i]->HandleEvent( aState );
        }
    }

// ----------------------------------------------------------------------------
// CAiPluginStateManager::IdleFocused()
// ----------------------------------------------------------------------------
//
TBool CAiPluginStateManager::IdleFocused() const
    {
    if ( iFocusObserver )
        {
        return ( iFocusObserver->Status() == ESMAIIdleForeground );
        }

    return EFalse;
    }

// ----------------------------------------------------------------------------
// CAiPluginStateManager::BackupOngoing()
// ----------------------------------------------------------------------------
//
TBool CAiPluginStateManager::BackupOngoing() const
    {
    if ( iBackupOperationObserver )
        {
        return ( iBackupOperationObserver->Status() == ESMAIBackupOn );
        }
    
    return EFalse;    
    }

// ----------------------------------------------------------------------------
// CAiPluginStateManager::LightsOn()
// ----------------------------------------------------------------------------
//
TBool CAiPluginStateManager::LightsOn() const
    {
    if ( iLightStateObserver )
        {
        return ( iLightStateObserver->Status() == ESMAIBacklightOn );
        }
    
    return EFalse;
    }

// ----------------------------------------------------------------------------
// CAiPluginStateManager::CallOngoing()
// ----------------------------------------------------------------------------
//
TBool CAiPluginStateManager::CallOngoing() const
    {
    if ( iCallStateObserver )
        {
        return ( iCallStateObserver->Status() == ESMAIInCall );
        }
        
    return EFalse;    
    }

// ----------------------------------------------------------------------------
// CAiPluginStateManager::T1TimerCallback()
// ----------------------------------------------------------------------------
//
TInt CAiPluginStateManager::T1TimerCallback( TAny* aPtr )
    {
    CAiPluginStateManager* self =
        static_cast< CAiPluginStateManager* >( aPtr );

    __ASSERT_DEBUG( self, 
        AiFwPanic::Panic( AiFwPanic::EAiFwPanic_NullPointerReference ) );

    self->iT1Timer->Cancel();
    
//    self->ProcessStateChangeForAll( ESMAIT1Timeout );
    
    return KErrNone;
    }

// End of file

