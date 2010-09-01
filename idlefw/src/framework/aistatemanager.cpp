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
#include <aifwpublisherinfo.h>
#include <debug.h>

#include "caicpscommandbuffer.h"
#include "aipluginfactory.h"

#include "aistatemanager.h"

// ======== LOCAL FUNCTIONS ========
// ----------------------------------------------------------------------------
// StartReason
// 
// ----------------------------------------------------------------------------
//
static CHsContentPublisher::TStartReason StartReason( TInt aReason )
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
static CHsContentPublisher::TStopReason StopReason( TInt aReason )
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
    iReloadPlugins.Reset();
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
    iFactory.SetStateManager( this );    
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
                
        iFactory.DestroyAllPlugins();
        
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
void CAiStateManager::NotifyLoadPlugin( const TAiFwPublisherInfo& aInfo )    
    {                
    __PRINT( __DBG_FORMAT( 
        "CAiStateManager::NotifyLoadPlugin: name: %S, reason: %d " ), 
              &aInfo.Info().Name(), aInfo.Reason() );      
          
    __TIME_MARK( time );
       
    iFactory.LoadPlugin( aInfo );
    
    __TIME_ENDMARK( "CAiStateManager::NotifyLoadPlugin - done", time );       
    }

// ----------------------------------------------------------------------------
// CAiStateManager::NotifyDestroyPlugin()
// 
// ----------------------------------------------------------------------------
//
void CAiStateManager::NotifyDestroyPlugin( const TAiFwPublisherInfo& aInfo )           
    {
    __PRINT( __DBG_FORMAT( 
        "CAiStateManager::NotifyDestroyPlugin: name: %S, reason: %d " ), 
            &aInfo.Info().Name(), aInfo.Reason() );              
    
    __TIME_MARK( time );
    
    iFactory.DestroyPlugin( aInfo );
                                 
    __TIME_ENDMARK( "CAiStateManager::DestroyPlugin - done", time );
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
        "CAiStateManager::ProcessStateChange: current state: %d, next state: %d, halt: %d" ), 
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
        
        iFactory.FlushCommandBuffer();
        }
    else
        {
        __PRINTS( "CAiStateManager::ProcessStateChange, no state change" );
        }
    
    __TIME_ENDMARK( "CAiStateManager::ProcessStateChange - done", time );
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
    
    iFactory.FlushCommandBuffer();
    
    __TIME_ENDMARK( "CAiStateManager::ProcessGeneralThemeChange - done", time );
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
    
    iFactory.FlushCommandBuffer();
            
    __TIME_ENDMARK( "CAiStateManager::ProcessBackupRestore - done", time );
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
    
    iFactory.FlushCommandBuffer();
    
    __PRINTS( "CAiStateManager::ProcessOnlineStateChange - done" );
    }

// ----------------------------------------------------------------------------
// CAiStateManager::StartPlugin()
// 
// ----------------------------------------------------------------------------
//
void CAiStateManager::StartPlugin( CHsContentPublisher& aPlugin, TInt aReason )    
    {    
    const THsPublisherInfo& info( aPlugin.PublisherInfo() );
    
    __PRINT( __DBG_FORMAT( 
        "CAiStateManager::StartPlugin: name: %S, reason: %d" ), &info.Name(), (TInt)aReason ); 
    
    aPlugin.Start( StartReason( aReason ) );
    
    if ( iCurrentState == EAlive )
        {
        __TIME( "CAiStateManager::StartPlugin, enter EAlive",
                
        aPlugin.Resume( CHsContentPublisher::EForeground ) );
        }
    else if ( iCurrentState == ESuspended )
        {
        __TIME( "CAiStateManager::StartPlugin, enter ESuspended",
                
        aPlugin.Suspend( CHsContentPublisher::EBackground ) );
        }    
    
    if ( iFlags.IsSet( EIsOnline ) )
        {
        __TIME( "CAiStateManager::StartPlugin, Set Online",
            
        aPlugin.SetOnline() );
        }
    else
        {
        __TIME( "CAiStateManager::StartPlugin, Set Offline",
            
        aPlugin.SetOffline() );
        }
           
    __PRINTS( "CAiStateManager::StartPlugin - done" );
    }

// ----------------------------------------------------------------------------
// CAiStateManager::StopPlugin()
// 
// ----------------------------------------------------------------------------
//
void CAiStateManager::StopPlugin( CHsContentPublisher& aPlugin, TInt aReason )    
    {
    const THsPublisherInfo& info( aPlugin.PublisherInfo() );
    
    __PRINT( __DBG_FORMAT( 
        "CAiStateManager::StopPlugin: name: %S, reason: %d" ), &info.Name(), (TInt)aReason ); 
                      
    if ( iCurrentState == EAlive )
        {
        __TIME( "CAiStateManager::StopPlugin, enter ESuspended", 
                
        aPlugin.Suspend( CHsContentPublisher::EBackground ) );
        }
    
    aPlugin.Stop( StopReason( aReason ) );   
           
    __PRINTS( "CAiStateManager::StopPlugin - done" );
    }

// ----------------------------------------------------------------------------
// CAiStateManager::NotifyReloadPlugins()
// 
// ----------------------------------------------------------------------------
//
void CAiStateManager::NotifyReloadPlugins()
    {
    __PRINTS( "CAiStateManager::NotifyReloadPlugins" );

    // Factory needs to update its ecom plugins list
    TRAP_IGNORE( iFactory.ListImplementationsL() );
    
    for ( TInt i = 0; i < iReloadPlugins.Count(); i++ )
        {
        TAiFwPublisherInfo info( iReloadPlugins[i], 
            TAiFwCallback(), EAiFwSystemStartup );
        
        // Reload plugin
        NotifyLoadPlugin( info );             
        }
    
    iReloadPlugins.Reset();
    
    __PRINTS( "CAiStateManager::NotifyReloadPlugins - done" );
    }

// ----------------------------------------------------------------------------
// CAiStateManager::NotifyReleasePlugins()
// 
// ----------------------------------------------------------------------------
//
void CAiStateManager::NotifyReleasePlugins( const RArray<TUid>& aUidList )
    {
    __PRINTS( "CAiStateManager::NotifyReleasePlugins" );    

    iReloadPlugins.Reset();
    
    TBool flush( EFalse );
    
    for ( TInt i = 0; i < aUidList.Count(); i++ )
        {
        const TUid& uid( aUidList[ i ] );
        
        CHsContentPublisher* plugin( iFactory.PluginByUid( uid ) );
                
        while ( plugin )               
            {
            StopPlugin( *plugin, CHsContentPublisher::ESystemShutdown );
            
            const THsPublisherInfo& info( plugin->PublisherInfo() );            
            iReloadPlugins.Append( info );
            
            iFactory.DestroyPlugin( uid );
            
            flush = ETrue;
            
            // Get next plugin with same uid
            plugin = iFactory.PluginByUid( uid );                        
            }
        }     
    
    if ( flush )
        {
        iFactory.FlushCommandBuffer();
        }
        
    __PRINTS( "CAiStateManager::NotifyReleasePlugins - done" );    
    }
    
// End of file
