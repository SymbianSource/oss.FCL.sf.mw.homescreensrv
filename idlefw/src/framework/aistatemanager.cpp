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
* Description:  State Manager
*
*/

// System includes

// User includes
#include <hscontentpublisher.h>
#include <aifwdefs.h>

#include "aipluginfactory.h"

#include "aistatemanager.h"

#include "debug.h"

// Constants
_LIT( KOnlineOffline, "online_offline" );

// ======== LOCAL FUNCTIONS ========
// ----------------------------------------------------------------------------
// StartReason
// 
// ----------------------------------------------------------------------------
//
static CHsContentPublisher::TStartReason StartReason( TAiFwLoadReason aReason )
    {
    CHsContentPublisher::TStartReason reason; 
             
    if ( aReason == EAiFwPageStartup )
        {
        reason = CHsContentPublisher::EPageStartup; 
        }
    else if ( aReason == EAiFwPluginStartup )
        {
        reason = CHsContentPublisher::EPluginStartup;
        }
    else
        {
        reason = CHsContentPublisher::ESystemStartup;
        }
    
    return reason;
    }

// ----------------------------------------------------------------------------
// StopReason
// 
// ----------------------------------------------------------------------------
//
static CHsContentPublisher::TStopReason StopReason( TAiFwDestroyReason aReason )
    {
    CHsContentPublisher::TStopReason reason; 
             
    if ( aReason == EAiFwPageShutdown )
        {
        reason = CHsContentPublisher::EPageShutdown; 
        }
    else if ( aReason == EAiFwPluginShutdown )
        {
        reason = CHsContentPublisher::EPluginShutdown;
        }
    else
        {
        reason = CHsContentPublisher::ESystemShutdown;
        }
    
    return reason;
    }

// ======== MEMBER FUNCTIONS ========
// ----------------------------------------------------------------------------
// CAiStateManager::NewL()
// Two-phased constructor.
// ----------------------------------------------------------------------------
//
CAiStateManager* CAiStateManager::NewL( CAiPluginFactory& aFactory )
    {
    CAiStateManager* self = CAiStateManager::NewLC( aFactory );
    CleanupStack::Pop( self );
    return self;
    }

// ----------------------------------------------------------------------------
// CAiStateManager::NewLC()
// Two-phased constructor.
// ----------------------------------------------------------------------------
//
CAiStateManager* CAiStateManager::NewLC( CAiPluginFactory& aFactory )
    {
    CAiStateManager* self = new ( ELeave ) CAiStateManager( aFactory );
    CleanupStack::PushL( self );
    self->ConstructL();
    return self;
    }

// ----------------------------------------------------------------------------
// CAiStateManager::~CAiStateManager()
// C++ default destructor.
// ----------------------------------------------------------------------------
//
CAiStateManager::~CAiStateManager()
    {  
    }

// ----------------------------------------------------------------------------
// CAiStateManager::CAiStateManager()
// C++ default constructor.
// ----------------------------------------------------------------------------
//
CAiStateManager::CAiStateManager( CAiPluginFactory& aFactory )
    : iFactory( aFactory )
    {
    }

// ----------------------------------------------------------------------------
// CAiStateManager::ConstructL()
// 2nd phase constructor
// ----------------------------------------------------------------------------
//
void CAiStateManager::ConstructL()
    {        
    }

// ----------------------------------------------------------------------------
// CAiStateManager::NotifyStateChange()
// 
// ----------------------------------------------------------------------------
//
void CAiStateManager::NotifyStateChange( TAiFwState aState )    
    {       
    if ( aState == EAiFwUiShutdown )
        {     
        __PRINTS( "CAiStateManager::NotifyStateChange, aState: EAiFwUiShutdown" );
        
        iFlags.Set( EShutdown );
        
        DestroyPlugins();
        
        return;
        }    
    else if ( aState == EAiFwUiStartup )
        {
        __PRINTS( "CAiStateManager::NotifyStateChange, aState: EAiFwUiStartup" );
        
        iFlags.Clear( EShutdown );
        
        return;
        }
       
    TBitFlags32 flags( iFlags );
    
    if ( aState == EAiFwForeground )
        {
        __PRINTS( "CAiStateManager::NotifyStateChange, aState: EAiFwForeground" );
        
        iFlags.Set( EIsForeground );                                    
        }
    else if ( aState == EAiFwBackground )
        {
        __PRINTS( "CAiStateManager::NotifyStateChange, aState: EAiFwBackground" );
        
        iFlags.Clear( EIsForeground );
        }
    else if ( aState == EAiFwBacklightOn )
        {
        __PRINTS( "CAiStateManager::NotifyStateChange, aState: EAiFwBacklightOn" );
        
        iFlags.Set( EIsLightsOn );
        }
    else if ( aState == EAiFwBacklightOff )
        {
        __PRINTS( "CAiStateManager::NotifyStateChange, aState: EAiFwBacklightOff" );
        
        iFlags.Clear( EIsLightsOn );
        }
    else if ( aState == EAiFwOnline )
        {
        __PRINTS( "CAiStateManager::NotifyStateChange, aState: EAiFwOnline" );
        
        iFlags.Set( EIsOnline );
        
        ProcessOnlineStateChange();
        }
    else if ( aState == EAiFwOffline )
        {
        __PRINTS( "CAiStateManager::NotifyStateChange, aState: EAiFwOffline" );
        
        iFlags.Clear( EIsOnline );
        
        ProcessOnlineStateChange();
        }
    else if ( aState == EAiFwGeneralThemeChange )
        {
        __PRINTS( "CAiStateManager::NotifyStateChange, aState: EAiFwGeneralThemeChange" );
        
        ProcessGeneralThemeChange();
        }
    else if ( aState == EAiFwBackupRestoreStart || 
        aState == EAiFwBackupRestoreEnd )
        {
        __PRINTS( "CAiStateManager::NotifyStateChange, aState: EAiFwBackupRestoreStart/End" );
        
        ProcessBackupRestore( aState == EAiFwBackupRestoreStart );
        }
    else
        {
        __PRINTS( "CAiStateManager::NotifyStateChange, aState: Unknown" );
        }

    // State change evaluation and state change trial is done always here
    __PRINTS( "CAiStateManager::NotifyStateChange, Run state change" );    
    ProcessStateChange( EvaluateNextState() );            
    }

// ----------------------------------------------------------------------------
// CAiStateManager::NotifyLoadPlugin()
// 
// ----------------------------------------------------------------------------
//
TInt CAiStateManager::NotifyLoadPlugin( const THsPublisherInfo& aPublisherInfo,
    TAiFwLoadReason aReason )
    {                
    __PRINT( __DBG_FORMAT( 
        "CAiStateManager::NotifyLoadPlugin: name: %S, reason: %d " ), 
              &aPublisherInfo.Name(), (TInt) aReason );      
          
    __TIME_MARK( time );
      
    // Create plugin
    TInt retval( iFactory.CreatePlugin( aPublisherInfo ) );
    
    if ( retval == KErrNone )
        {
        CHsContentPublisher* plugin( iFactory.PluginByInfo( aPublisherInfo ) );

        // Do startup state transition    
        StartPlugin( *plugin, StartReason( aReason ) );            
        }
      
    __TIME_ENDMARK( "CAiStateManager::NotifyLoadPlugin, construction", time );
    
    return retval;
    }

// ----------------------------------------------------------------------------
// CAiStateManager::NotifyDestroyPlugin()
// 
// ----------------------------------------------------------------------------
//
void CAiStateManager::NotifyDestroyPlugin( 
    const THsPublisherInfo& aPublisherInfo, TAiFwDestroyReason aReason )    
    {
    __PRINT( __DBG_FORMAT( 
        "CAiStateManager::NotifyDestroyPlugin: name: %S, reason: %d " ), 
            &aPublisherInfo.Name(), (TInt)aReason );              
    
    __TIME_MARK( time );
    
    // Resolve plugin
    CHsContentPublisher* plugin( iFactory.PluginByInfo( aPublisherInfo ) );
            
    if ( plugin )
        {                  
        // Do shutdown state transition
        StopPlugin( *plugin, StopReason( aReason ) );
        
        // Destroy plugin
        iFactory.DestroyPlugin( aPublisherInfo );                                    
        }
           
    __TIME_ENDMARK( "CAiStateManager::DestroyPlugin, done", time );
    }

// ----------------------------------------------------------------------------
// CAiStateManager::NotifyUpdatePlugins()
// 
// ----------------------------------------------------------------------------
//
void CAiStateManager::NotifyUpdatePlugins()
    {
    __PRINTS( "CAiStateManager::NotifyUpdatePlugins" );
    
    RArray< THsPublisherInfo > publishers;
        
    // Get plugins which has upgrade available
    TRAP_IGNORE( iFactory.ResolvePluginsToUpgradeL( publishers ) );
    
    for ( TInt i = 0; i < publishers.Count(); i++ )
        {        
        THsPublisherInfo info( publishers[i] );
        
        // Update by destroy - load sequence 
        NotifyDestroyPlugin( info, EAiFwSystemShutdown );
        NotifyLoadPlugin( info, EAiFwSystemStartup );         
        }
    
    publishers.Reset();
    
    __PRINTS( "CAiStateManager::NotifyUpdatePlugins, done" );
    }

// ----------------------------------------------------------------------------
// CAiStateManager::OnlineStateInUse()
// 
// ----------------------------------------------------------------------------
//
TBool CAiStateManager::OnlineStateInUse() const
    {
    __PRINTS( "CAiStateManager::OnlineStateInUse" );        
    
    RPointerArray< CHsContentPublisher >& plugins( iFactory.Publishers() );
    
    for( TInt i = 0; i < plugins.Count(); i++ )
        {
        CHsContentPublisher* plugin( plugins[i] );
        
        if ( plugin->HasMenuItem( KOnlineOffline() ) )
            {
            return ETrue;
            }
        }
    
    return EFalse;
    }

// ----------------------------------------------------------------------------
// CAiStateManager::EvaluateNextState()
// 
// ----------------------------------------------------------------------------
//
CAiStateManager::TState CAiStateManager::EvaluateNextState() const
    {
    TState nextState( iCurrentState );
        
    if ( iFlags.IsSet( EIsForeground ) && iFlags.IsSet( EIsLightsOn ) )
        {
        nextState = EAlive;
        }
    else 
        {
        nextState = ESuspended;
        } 

    __PRINT( __DBG_FORMAT( 
        "CAiStateManager::EvaluateNextState: current state: %d, next state: %d" ), 
            (TInt)iCurrentState, (TInt)nextState );     
    
    return nextState;
    }

// ----------------------------------------------------------------------------
// CAiStateManager::ProcessStateChange()
// 
// ----------------------------------------------------------------------------
//
void CAiStateManager::ProcessStateChange( TState aNextState )
    {
    __PRINT( __DBG_FORMAT( 
        "CAiStateManager::ProcessStateChange: current state: %d, next state: %d, halt: &d" ), 
            (TInt)iCurrentState, (TInt)aNextState, iHalt );     
        
    __TIME_MARK( time );
    
    if ( aNextState != iCurrentState )
        {
        // Update state
        iCurrentState = aNextState;
        
        RPointerArray< CHsContentPublisher >& plugins( iFactory.Publishers() );
                       
        // Process state for all
        for( TInt i = 0; !iHalt && i < plugins.Count(); i++ )
            {
            CHsContentPublisher* plugin( plugins[i] );
            
            const THsPublisherInfo& info( plugin->PublisherInfo() );
                
            __PRINT( __DBG_FORMAT( 
                "CAiStateManager::ProcessStateChange: name: %S" ), &info.Name() ); 
            
            if ( iCurrentState == EAlive )
                {
                __TIME( "CAiStateManager::ProcessStateChange, EAlive", 
                        
                plugin->Resume( CHsContentPublisher::EForeground ) );         
                }
            else if ( iCurrentState == ESuspended )
                {
                __TIME( "CAiStateManager::ProcessStateChange, ESuspended",
                        
                plugin->Suspend( CHsContentPublisher::EBackground ) );
                }            
            }
        }
    else
        {
        __PRINTS( "CAiStateManager::ProcessStateChange, no state change" );
        }
    
    __TIME_ENDMARK( "CAiStateManager::ProcessStateChange, done", time );
    }

// ----------------------------------------------------------------------------
// CAiStateManager::ProcessGeneralThemeChange()
// 
// ----------------------------------------------------------------------------
//
void CAiStateManager::ProcessGeneralThemeChange()
    {
    __PRINTS( "CAiStateManager::ProcessGeneralThemeChange" );
    __TIME_MARK( time );
    
    RPointerArray< CHsContentPublisher >& plugins( iFactory.Publishers() );
            
    for( TInt i = 0; i < plugins.Count(); i++ )
        {
        CHsContentPublisher* plugin( plugins[i] );
        
        plugin->Suspend( CHsContentPublisher::EGeneralThemeChange );
                
        // Resume publisher immediately if in alive state 
        if( !iHalt && iCurrentState == EAlive )
            {
            plugin->Resume( CHsContentPublisher::EForeground );
            }        
        } 
    
    __TIME_ENDMARK( "CAiStateManager::ProcessGeneralThemeChange, done", time );
    }

// ----------------------------------------------------------------------------
// CAiStateManager::ProcessBackupRestore()
// 
// ----------------------------------------------------------------------------
//
void CAiStateManager::ProcessBackupRestore( TBool aStart )
    {
    __PRINTS( "CAiStateManager::ProcessBackupRestore" );
    __TIME_MARK( time );
    
    iHalt = aStart;

    RPointerArray< CHsContentPublisher >& plugins( iFactory.Publishers() );
            
    for( TInt i = 0; i < plugins.Count(); i++ )
        {
        CHsContentPublisher* plugin( plugins[i] );
        
        if ( aStart )
            {
            plugin->Suspend( CHsContentPublisher::EBackupRestore );
            }
        else
            {
            // Resume publisher immediately if in alive state
            if ( !iHalt && iCurrentState == EAlive )
                {
                plugin->Resume( CHsContentPublisher::EForeground );
                }
            }
        }
            
    __TIME_ENDMARK( "CAiStateManager::ProcessBackupRestore, done", time );
    }

// ----------------------------------------------------------------------------
// CAiStateManager::ProcessOnlineStateChange()
// 
// ----------------------------------------------------------------------------
//
void CAiStateManager::ProcessOnlineStateChange()
    {
    __PRINTS( "CAiStateManager::ProcessOnlineStateChange" );
    
    RPointerArray< CHsContentPublisher >& plugins( iFactory.Publishers() );
    
    for( TInt i = 0; i < plugins.Count(); i++ )
        {
        CHsContentPublisher* plugin( plugins[i] );
        
        if ( iFlags.IsSet( EIsOnline ) )
            {
            plugin->SetOnline();
            }
        else
            {
            plugin->SetOffline();
            }
        }               
    }

// ----------------------------------------------------------------------------
// CAiStateManager::StartPlugin()
// 
// ----------------------------------------------------------------------------
//
void CAiStateManager::StartPlugin( CHsContentPublisher& aPlugin,
    CHsContentPublisher::TStartReason aReason )
    {
    __PRINTS( "CAiStateManager::StartPlugin" );
    
    aPlugin.Start( aReason );
    
    if ( iCurrentState == EAlive )
        {
        aPlugin.Resume( CHsContentPublisher::EForeground );
        }
    else if ( iCurrentState == ESuspended )
        {
        aPlugin.Suspend( CHsContentPublisher::EBackground );
        }    
    
    if ( iFlags.IsSet( EIsOnline ) )
        {
        aPlugin.SetOnline();
        }
    else
        {
        aPlugin.SetOffline();
        }
    }

// ----------------------------------------------------------------------------
// CAiStateManager::StopPlugin()
// 
// ----------------------------------------------------------------------------
//
void CAiStateManager::StopPlugin( CHsContentPublisher& aPlugin,
    CHsContentPublisher::TStopReason aReason )
    {
    __PRINTS( "CAiStateManager::StopPlugin" );
                
    if ( iCurrentState == EAlive )
        {
        aPlugin.Suspend( CHsContentPublisher::EBackground );
        }
    
    aPlugin.Stop( aReason );                    
    }

// ----------------------------------------------------------------------------
// CAiStateManager::DestroyPlugins()
// 
// ----------------------------------------------------------------------------
//
void CAiStateManager::DestroyPlugins()
    {
    __PRINTS( "CAiStateManager::DestroyPlugins, start" );    
    __TIME_MARK( time );
    
    RPointerArray< CHsContentPublisher >& plugins( iFactory.Publishers() );
    
    for ( TInt i = 0; i < plugins.Count(); i++ )
        {
        CHsContentPublisher* plugin( plugins[i] );

        // Do shutdown state transition
        StopPlugin( *plugin, CHsContentPublisher::ESystemShutdown );        
        }    
    
    // Finally get rid of all plugins
    plugins.ResetAndDestroy();
    
    __TIME_ENDMARK( "CAiStateManager::DestroyPlugins, done", time );
    }

// End of file
