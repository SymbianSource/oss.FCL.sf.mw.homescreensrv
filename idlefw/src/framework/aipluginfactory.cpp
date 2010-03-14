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

// User includes
#include <aisystemuids.hrh>
#include <hscontentpublisher.h>
#include <hspublisherinfo.h>
#include <aicontentobserver.h>
#include <aiuicontroller.h>

#include "aiuicontrollermanager.h"
#include "aipluginfactory.h"
#include "debug.h"

// Constants
const TUid KDeviceStatusPluginUid = 
    { AI_UID_ECOM_IMPLEMENTATION_CONTENTPUBLISHER_DEVSTAPLUGIN };

_LIT( KDeviceStatusPluginName, "DeviceStatus" );


// ======== LOCAL FUNCTIONS ========
// ----------------------------------------------------------------------------
// IsDeviceStatus()
//
// ----------------------------------------------------------------------------
//
TBool IsDeviceStatus( const THsPublisherInfo& aInfo )
    {
    return ( aInfo.Name() == KDeviceStatusPluginName && 
        aInfo.Uid() == KDeviceStatusPluginUid );
    }

// ----------------------------------------------------------------------------
// CleanupResetAndDestroy()
//
// ----------------------------------------------------------------------------
//
template< class T >
static void CleanupResetAndDestroy( TAny* aObj )
    {        
    static_cast< T* >( aObj )->ResetAndDestroy();    
    }

// ----------------------------------------------------------------------------
// CleanupResetAndDestroyPushL()
//
// ----------------------------------------------------------------------------
//
template< class T >
static void CleanupResetAndDestroyPushL( T& aArray )
    {
    CleanupStack::PushL( 
        TCleanupItem( &CleanupResetAndDestroy< T >, &aArray ) );
    }
    
// ======== MEMBER FUNCTIONS ========    

// ----------------------------------------------------------------------------
// CAiPluginFactory::CAiPluginFactory()
//
// ----------------------------------------------------------------------------
//
CAiPluginFactory::CAiPluginFactory( CAiUiControllerManager& aManager )
    : iUiControllerManager( aManager )
    {
    }

// ----------------------------------------------------------------------------
// CAiPluginFactory::~CAiPluginFactory()
//
// ----------------------------------------------------------------------------
//
CAiPluginFactory::~CAiPluginFactory()
    {
    // All publishers should be already deleted from CAiFw::HandleUiShutdown       
    iPublishers.ResetAndDestroy();
    
    iEComPlugins.ResetAndDestroy();        
    
    REComSession::FinalClose();
    }
    
// ----------------------------------------------------------------------------
// CAiPluginFactory::NewL()
//
// ----------------------------------------------------------------------------
//
CAiPluginFactory* CAiPluginFactory::NewL( CAiUiControllerManager& aManager )
    {
    CAiPluginFactory* self = 
        new ( ELeave ) CAiPluginFactory( aManager );
                                                                      
    CleanupStack::PushL( self );
    self->ConstructL();
    CleanupStack::Pop( self );
    return self;
    }

// ----------------------------------------------------------------------------
// CAiPluginFactory::ConstructL()
//
// ----------------------------------------------------------------------------
//
void CAiPluginFactory::ConstructL()
    {               
    REComSession::ListImplementationsL( 
        KInterfaceUidHsContentPlugin, iEComPlugins );    
    }
        
// ----------------------------------------------------------------------------
// CAiPluginFactory::CreatePluginL()
//
// ----------------------------------------------------------------------------
//
TInt CAiPluginFactory::CreatePlugin( 
    const THsPublisherInfo& aPublisherInfo )                             
    {                                            
    __PRINTS( "*** CAiPluginFactory::CreatePlugin: Start ***" );
                                    
    if ( IsDeviceStatus( aPublisherInfo ) )
        {
        CHsContentPublisher* plugin( PluginByUid( aPublisherInfo.Uid() ) );
        
        if ( plugin )
            {
            // Devicestatus plugin already exists, update its namespace
            THsPublisherInfo& info( 
                const_cast< THsPublisherInfo& >( plugin->PublisherInfo() ) );
            
            info.iNamespace.Copy( aPublisherInfo.Namespace() );
            
            __PRINTS( "*** CAiPluginFactory::CreatePlugin: Done - DeviceStatus plugin updated ***" );
            
            return KErrNone;
            }
        }
            
    TBool implFound( EFalse );
    
    for( TInt i = 0; i < iEComPlugins.Count(); i++ )
        {
        CImplementationInformation* information( iEComPlugins[i] );
                                                                 
        if( information->ImplementationUid().iUid == aPublisherInfo.Uid().iUid )
            {
            implFound = ETrue;
            break;
            }
        }
    
    if( aPublisherInfo.Namespace() == KNullDesC8 || !implFound )
        {
        // No namespace available or no ecom implementation available
        __PRINTS( "*** CAiPluginFactory::CreatePlugin: Done - Failed to Load Plug-in: KErrNotSupported ***" );
        
        return KErrNotSupported;
        }
           
    CHsContentPublisher* plugin( PluginByInfo( aPublisherInfo ) );
    
    if( plugin )
        {                             
        __PRINTS( "*** CAiPluginFactory::CreatePlugin: Done - Failed to Load Plug-in: KErrAlreadyExists ***" );
        
        return KErrAlreadyExists;
        }
    
    TInt err( KErrNone );
    
    TRAP( err, CreatePluginL( aPublisherInfo ) );
    
    __PRINTS( "*** CAiPluginFactory::CreatePlugin: Done - Load Plug-in ***" );
    
    return err;    
    }
        
// ----------------------------------------------------------------------------
// CAiPluginFactory::DestroyPlugin()
//
// ----------------------------------------------------------------------------
//
void CAiPluginFactory::DestroyPlugin( const THsPublisherInfo& aPublisherInfo )                             
    {
    __PRINTS( "*** CAiPluginFactory::DestroyPlugin: Start ***" );
    
    if ( IsDeviceStatus( aPublisherInfo ) )
        {
        // Don't destroy device status plugin
        __PRINTS( "*** CAiPluginFactory::DestroyPlugin: Done - Keepind DeviceStatus Plug-in ***" );
        
        return;
        }
        
    CHsContentPublisher* plugin( PluginByInfo( aPublisherInfo ) );
    
    if ( plugin )
        {
        iPublishers.Remove( iPublishers.Find( plugin ) );
        
        delete plugin;
        plugin = NULL;            
        }
    
    __PRINTS( "*** CAiPluginFactory::DestroyPlugin: Done ***" );
    }
        
// ----------------------------------------------------------------------------
// CAiPluginFactory::CreatePluginL()
//
// ----------------------------------------------------------------------------
//
void CAiPluginFactory::CreatePluginL(
    const THsPublisherInfo& aPublisherInfo )
    {       
    __PRINT( __DBG_FORMAT( "\t[I]\t Loading plug-in uid=%x name=%S"), 
    aPublisherInfo.Uid(), &(aPublisherInfo.Name() ) );

    __TIME( "FW: Create plug-in:",
            
    iPublishers.ReserveL( iPublishers.Count() + 1 );    
            
    CHsContentPublisher* plugin = 
        CHsContentPublisher::NewL( aPublisherInfo ) );            
    CleanupStack::PushL( plugin );
    
    __TIME( "FW: Subscribe content observers",    
    SubscribeContentObserversL( *plugin, aPublisherInfo ) );             
                                                      
    __TIME( "FW: Configure Plugin",
    ConfigurePluginL( *plugin, aPublisherInfo ) );
    
    // Take plugin's ownership
    iPublishers.Append( plugin );
    CleanupStack::Pop( plugin );
    }

// ----------------------------------------------------------------------------
// CAiPluginFactory::SubscribeContentObserversL()
//
// ----------------------------------------------------------------------------
//
void CAiPluginFactory::SubscribeContentObserversL(
    CHsContentPublisher& aContentPublisher,        
    const THsPublisherInfo& aPublisherInfo )    
    {
    RPointerArray< CAiUiController >& 
        controllers( iUiControllerManager.UiControllers() );
            
    for( TInt i = 0; i < controllers.Count(); i++ )
        {
        MAiContentObserver& observer( 
            controllers[i]->GetContentObserver() );
        
        if ( observer.RequiresSubscription( aPublisherInfo ) )
            {
			// Subscribe observer only if it understands the plugin
            aContentPublisher.SubscribeL( observer );
            }                   
        }
    }

// ----------------------------------------------------------------------------
// CAiPluginFactory::ConfigurePluginL()
//
// ----------------------------------------------------------------------------
//
void CAiPluginFactory::ConfigurePluginL(     
    CHsContentPublisher& aContentPublisher,
    const THsPublisherInfo& aPublisherInfo )
    {    
    RAiSettingsItemArray settings;
    CleanupResetAndDestroyPushL( settings );
    
    RPointerArray< CAiUiController >& 
        controllers( iUiControllerManager.UiControllers() );
    
    for( TInt i = 0; i < controllers.Count(); i++ )
        {
        // Get settings for plug-in        
        controllers[i]->GetSettingsL( aPublisherInfo, settings );
        }
    
    // Configure plug-in with its settings
    aContentPublisher.ConfigureL( settings );
    
    CleanupStack::PopAndDestroy( &settings );    
    }
        
// ----------------------------------------------------------------------------
// CAiPluginFactory::PluginByInfo()
// Gets plugin by publisher info. Only this overload returns the exact match
// ----------------------------------------------------------------------------
//
CHsContentPublisher* CAiPluginFactory::PluginByInfo( 
    const THsPublisherInfo& aPublisherInfo ) const
    {       
    for( TInt i = 0; i < iPublishers.Count(); i++ )
        {         
        const THsPublisherInfo& info( iPublishers[i]->PublisherInfo() ); 
                                                                             
        if( aPublisherInfo == info ) 
            {
            return iPublishers[i];
            }
        }
    
    return NULL;
    }

// ----------------------------------------------------------------------------
// CAiPluginFactory::PluginByUid()
// Gets plugin by UID
// ----------------------------------------------------------------------------
//
CHsContentPublisher* CAiPluginFactory::PluginByUid( const TUid& aUid ) const
    {
    for( TInt i = 0; i < iPublishers.Count(); i++ )
        {
        const THsPublisherInfo& info( iPublishers[i]->PublisherInfo() );
                                                               
        if( info.Uid() == aUid )
            {
            return iPublishers[i];            
            }
        }
    
    return NULL;        
    }
    
// ----------------------------------------------------------------------------
// CAiPluginFactory::PluginByName()
// Gets plugin by name
// ----------------------------------------------------------------------------
//
CHsContentPublisher* CAiPluginFactory::PluginByName( 
    const TDesC& aName ) const
    {
    for( TInt i = 0; i < iPublishers.Count(); i++ )
        {
        const THsPublisherInfo& info( iPublishers[i]->PublisherInfo() );
                                                               
        if( info.Name() == aName )
            {
            return iPublishers[i];            
            }
        }
    
    return NULL;    
    }

// ----------------------------------------------------------------------------
// CAiPluginFactory::Publishers()
//
// ----------------------------------------------------------------------------
//
RPointerArray< CHsContentPublisher >& CAiPluginFactory::Publishers() const
    {
    return iPublishers;
    }

// ----------------------------------------------------------------------------
// CAiPluginFactory::ResolvePluginsToUpgradeL()
//
// ----------------------------------------------------------------------------
//
void CAiPluginFactory::ResolvePluginsToUpgradeL( 
    RArray< THsPublisherInfo >& aArray )
    {
    RImplInfoPtrArray ecomPlugins;
    CleanupResetAndDestroyPushL( ecomPlugins );
        
    REComSession::ListImplementationsL( 
        KInterfaceUidHsContentPlugin, ecomPlugins );
    
    for ( TInt i = 0; i < ecomPlugins.Count(); i++ )
        {
        CImplementationInformation* newInformation( ecomPlugins[i] );
                 
        for( TInt j = 0; j < iEComPlugins.Count(); j++ )
            {            
            CImplementationInformation* oldInformation( iEComPlugins[j] );
                                                                 
            if( newInformation->ImplementationUid() == oldInformation->ImplementationUid() )
                {
                if( newInformation->Version() != oldInformation->Version() )
                    {                        
                    for ( TInt k = 0; k < iPublishers.Count(); k++ )
                        {
                        const THsPublisherInfo& info( 
                            iPublishers[k]->PublisherInfo() );
                                                
                        if ( info.Uid() == newInformation->ImplementationUid() )
                            {                            
                            __PRINT( __DBG_FORMAT( "\t[I]\t Plug-in to update uid=%x name=%S namespace=%S, version update %d to %d"), 
                                info.Uid(), &(info.Name()), &(info.Namespace()), oldInformation->Version(), newInformation->Version()  );
                            
                            aArray.Append( info );
                            }
                        }
                                                         
                    break;
                    }
                }                
            }
        }
    
    CleanupStack::PopAndDestroy( &ecomPlugins );
           
    // Update ecom plugin array
    iEComPlugins.ResetAndDestroy();
    
    REComSession::ListImplementationsL( 
        KInterfaceUidHsContentPlugin, iEComPlugins );
    }

// End of file
