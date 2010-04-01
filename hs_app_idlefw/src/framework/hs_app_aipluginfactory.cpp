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
* Description:  Content plugin manager class for Active idle framework.
*
*/

// System includes
#include <ecom/ecom.h>
#include <ecom/implementationinformation.h>
#include <AknGlobalNote.h>
#include <StringLoader.h>
#include <e32property.h>                    // For RProperty

// User includes
#include <aicontentobserver.h>
#include <aiutility.h>
#include <aiplugintool.h>
#include <activeidle2domainpskeys.h>        // PubSub category
#include <activeidle2internalpskeys.h>      // PubSub category key and values

#include "hs_app_aipluginfactory.h"
#include "hs_app_aicontentpluginmanager.h"
#include "hs_app_aipluginlifecycleobserver.h"

#include "aiuicontroller.h"
#include "hs_app_aifwpanic.h"
#include "debug.h"


// ======== LOCAL FUNCTIONS ========
// ----------------------------------------------------------------------------
// CleanupResetAndDestroy()
// ----------------------------------------------------------------------------
//
template<class T>
static void CleanupResetAndDestroy( TAny* aObj )
    {
    if( aObj )
        {
        static_cast<T*>( aObj )->ResetAndDestroy();
        }
    }

// ----------------------------------------------------------------------------
// CleanupResetAndDestroyPushL()
// ----------------------------------------------------------------------------
//
template<class T>
static void CleanupResetAndDestroyPushL(T& aArray)
    {
    CleanupStack::PushL( TCleanupItem( &CleanupResetAndDestroy<T>, &aArray ) );
    }
    
// ======== MEMBER FUNCTIONS ========    

// ----------------------------------------------------------------------------
// CAiPluginFactory::CAiPluginFactory()
// ----------------------------------------------------------------------------
//
CAiPluginFactory::CAiPluginFactory( 
    RPointerArray<CAiContentPublisher>& aPlugins,
    CAiContentPluginManager& aManager )
      : iPlugins( aPlugins ), iManager( aManager )    
    {
    }

// ----------------------------------------------------------------------------
// CAiPluginFactory::~CAiPluginFactory()
// ----------------------------------------------------------------------------
//
CAiPluginFactory::~CAiPluginFactory()
    {    
    Release( iPluginTool );
           
    iEComPlugins.ResetAndDestroy();
           
    iLifecycleObservers.Reset();
    }
    
// ----------------------------------------------------------------------------
// CAiPluginFactory::NewL()
// ----------------------------------------------------------------------------
//
CAiPluginFactory* CAiPluginFactory::NewL( 
    RPointerArray<CAiContentPublisher>& aPlugins,
    CAiContentPluginManager& aManager )
    {
    CAiPluginFactory* self = 
        new ( ELeave ) CAiPluginFactory( aPlugins, aManager );
                                                                      
    CleanupStack::PushL( self );
    self->ConstructL();
    CleanupStack::Pop( self );
    return self;
    }

// ----------------------------------------------------------------------------
// CAiPluginFactory::ConstructL()
// ----------------------------------------------------------------------------
//
void CAiPluginFactory::ConstructL()
    {        
    iPluginTool = AiUtility::CreatePluginToolL();
    }
        
// ----------------------------------------------------------------------------
// CAiPluginFactory::AddLifecycleObserverL()
// ----------------------------------------------------------------------------
//
void CAiPluginFactory::AddLifecycleObserverL(
    MAiPluginLifecycleObserver& aObserver )
    {
    if( iLifecycleObservers.Find( &aObserver ) == KErrNotFound )
        {
        iLifecycleObservers.AppendL( &aObserver );
        }    
    }

// ----------------------------------------------------------------------------
// CAiPluginFactory::CreatePluginL()
// ----------------------------------------------------------------------------
//
void CAiPluginFactory::CreatePluginL(
    const TAiPublisherInfo& aPublisherInfo,
    RPointerArray<CAiUiController>& aControllerArray )                       
    {           
    iEComPlugins.ResetAndDestroy();
    
    // Discover Plugin implementations.
    __TIME_MARK( ecomOverhead );
    
    REComSession::ListImplementationsL( 
            KInterfaceUidContentPlugin, iEComPlugins );
    
    __TIME_ENDMARK( "FW: ECom Discover plug-ins", ecomOverhead );    
                          
    iPlugins.ReserveL( iPlugins.Count() + 1 );
                       
    TBool implFound( EFalse );
    
    for( TInt i = 0; i < iEComPlugins.Count(); i++ )
        {
        CImplementationInformation* information( iEComPlugins[i] );
                                                                 
        if( information->ImplementationUid().iUid == aPublisherInfo.iUid.iUid )
            {
            implFound = ETrue;
            break;
            }
        }
    
    if( aPublisherInfo.iNamespace == KNullDesC8 || !implFound )
        {
        // No namespace available or no ecom implementation available                               
        User::Leave( KErrNotSupported );
        }
           
    CAiContentPublisher* plugin( PluginByInfoL( aPublisherInfo ) );
    
    if( plugin )
        {                             
        User::Leave( KErrAlreadyExists );
        }
           
    __PRINT( __DBG_FORMAT( "\t[I]\t Loading plug-in uid=%x name=%S"), 
                            aPublisherInfo.iUid, &(aPublisherInfo.iName) );
           
    __TIME( "FW: Create plug-in:",
        plugin = CreatePluginLC( aPublisherInfo );
        ) // __TIME
       
    __TIME( "FW: Subscribe content observers",    
        SubscribeContentObserversL( *plugin, 
            aPublisherInfo, aControllerArray );
        ) // __TIME
                                              
    // Plug-in settings
    __TIME( "FW: Configure Plugin",
        ConfigurePluginL( aControllerArray, *plugin, aPublisherInfo );
        ) // __TIME
                                  
    __PRINTS( "*** FW: Done - Load Plug-in ***" );                          
    
    // This might fail and the plugin ends up destroyed
    for( TInt i = 0; i < iLifecycleObservers.Count(); ++i )
        {
        iLifecycleObservers[i]->PluginCreatedL( *plugin );
        }
                
    for( TInt i = 0; i < iLifecycleObservers.Count(); ++i )
        {
        iLifecycleObservers[i]->AllPluginsCreated();
        }
                   
    // Move plugins to manager
    iPlugins.Append( plugin );
    CleanupStack::Pop( plugin );
    
    iEComPlugins.ResetAndDestroy();       
    }
        
// ----------------------------------------------------------------------------
// CAiPluginFactory::DestroyPluginL()
// ----------------------------------------------------------------------------
//
void CAiPluginFactory::DestroyPluginL(
    const TAiPublisherInfo& aPublisherInfo,
    RPointerArray< CAiUiController >& /*aControllerArray*/ )                       
    {
    // TODO: check is there need to call 
    // iUiControllerManager->RemovePluginFromUI( aPlugin );
    // it will clean the published content.     
    
    if( iPlugins.Count() == 0 )
        {
        return;
        }
               
    CAiContentPublisher* plugin( PluginByInfoL( aPublisherInfo ) );
    
    TInt index( iPlugins.Find( plugin ) );
    
    if( plugin && index != KErrNotFound )
        {               
        for( TInt i = 0; i < iLifecycleObservers.Count(); i++ )
            {                
            iLifecycleObservers[i]->PluginDestroyed( *plugin );
            }
        
        iPlugins.Remove( index );
        
        delete plugin;
        plugin = NULL;
        }
        
    if( iPlugins.Count() == 0 )
        {
        for( TInt i = 0; i < iLifecycleObservers.Count(); i++ )
            {                
            iLifecycleObservers[i]->AllPluginsDestroyed();
            }        
        }
    }

// ----------------------------------------------------------------------------
// CAiPluginFactory::DestroyPlugins()
// ----------------------------------------------------------------------------
//
void CAiPluginFactory::DestroyPlugins()
    {
    for( TInt i = 0; i < iPlugins.Count(); i++ )
        {
        CAiContentPublisher* plugin( iPlugins[i] );
        
        for( TInt i = 0; i < iLifecycleObservers.Count(); i++ )
            {                
            iLifecycleObservers[i]->PluginDestroyed( *plugin );
            }        
        }

    iPlugins.ResetAndDestroy();
    
    for( TInt i = 0; i < iLifecycleObservers.Count(); i++ )
        {                
        iLifecycleObservers[i]->AllPluginsDestroyed();
        }           
    }
        
// ----------------------------------------------------------------------------
// CAiPluginFactory::CreatePluginLC()
// ----------------------------------------------------------------------------
//
CAiContentPublisher* CAiPluginFactory::CreatePluginLC(
    const TAiPublisherInfo& aPluginInfo )
    {
    CAiContentPublisher* plugin = 
            CAiContentPublisher::NewL( aPluginInfo.iUid );
    
    CleanupStack::PushL( plugin );
        
    MAiPropertyExtension* ext( iPluginTool->PropertyExt( *plugin ) );
    
    if( !ext )
        {
        User::Leave( KErrNotFound );
        }
    
    ext->SetPropertyL( EAiPublisherInfo, (TAny*)&aPluginInfo );
    
    const TAiPublisherInfo* info( ext->PublisherInfoL() );
    
    if( info->iNamespace != aPluginInfo.iNamespace )
        {
        // SetPropertyL is not implemented correctly
        User::Leave( KErrNotSupported );
        }
    
    return plugin;
    }

// ----------------------------------------------------------------------------
// CAiPluginFactory::SubscribeContentObserversL()
// ----------------------------------------------------------------------------
//
void CAiPluginFactory::SubscribeContentObserversL(
    CAiContentPublisher& aContentPublisher,        
    const TAiPublisherInfo& aPublisherInfo,
    RPointerArray<CAiUiController>& aControllerArray )
    {       
    
    for( TInt i = 0; i < aControllerArray.Count(); i++ )
        {
        MAiContentObserver& observer( 
            aControllerArray[i]->GetContentObserver() );
        
        if ( observer.RequiresSubscription( aPublisherInfo ) )
            {
			// Subscribe observer only if it understands the plugin
            aContentPublisher.SubscribeL( observer );
            }                   
        }
    }

// ----------------------------------------------------------------------------
// CAiPluginFactory::ConfigurePluginL()
// ----------------------------------------------------------------------------
//
void CAiPluginFactory::ConfigurePluginL( 
    RPointerArray<CAiUiController>& aControllerArray,
    CAiContentPublisher& aContentPublisher,
    const TAiPublisherInfo& aPubInfo )
    {    
    RAiSettingsItemArray pluginSettings;
    CleanupResetAndDestroyPushL( pluginSettings );
    
    for( TInt i = 0; i < aControllerArray.Count(); i++ )
        {
        // Get settings for plug-in        
        aControllerArray[i]->GetSettingsL( aPubInfo, pluginSettings );
        }
    
    // Configure plug-in with its settings
    aContentPublisher.ConfigureL( pluginSettings );
    
    CleanupStack::PopAndDestroy( &pluginSettings );    
    }
        
// ----------------------------------------------------------------------------
// CAiPluginFactory::PluginByInfoL()
// ----------------------------------------------------------------------------
//
CAiContentPublisher* CAiPluginFactory::PluginByInfoL( 
    const TAiPublisherInfo& aInfo ) const
    {       
    for( TInt i = 0; i < iPlugins.Count(); i++ )
        {         
        const TAiPublisherInfo* info( NULL );
                                                  
        info = iPluginTool->PublisherInfoL( *iPlugins[i] );
        
        if( info && ( aInfo == *info ) ) 
            {
            return iPlugins[i];
            }
        }
    
    return NULL;
    }

// ----------------------------------------------------------------------------
// CAiPluginFactory::PluginByNameL()
// ----------------------------------------------------------------------------
//
CAiContentPublisher* CAiPluginFactory::PluginByNameL( 
    const TDesC& aName ) const
    {
    for( TInt i = 0; i < iPlugins.Count(); i++ )
        {
        const TAiPublisherInfo* info( NULL );
        
        TRAP_IGNORE( info = iPluginTool->PublisherInfoL( *iPlugins[i] ) );
                        
        if( info && info->iName == aName )
            {
            return iPlugins[i];            
            }
        }
    
    return NULL;    
    }

// End of file
