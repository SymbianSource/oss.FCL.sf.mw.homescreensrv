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
* Description:  Content plugin error manager class
*
*/


#include <ecom/ecom.h>
#include <ecom/implementationinformation.h>
#include <aiutility.h>
#include <aiplugintool.h>

#include <centralrepository.h>
#include <activeidle2domaincrkeys.h>

#include "hs_app_aicontentpluginmanager.h"
#include "hs_app_aipluginstatemanager.h"
#include "aifweventhandler.h"
#include "aicontentpublisher.h"
#include "aipropertyextension.h"
#include <aicontentrequest.h>
#include "aieventhandlerextension.h"
#include "aiuicontroller.h"
#include "aiconsts.h"
#include "debug.h"

// CONSTANTS
const TInt KAILenOfParenthesis( 2 );

// ======== MEMBER FUNCTIONS ========

// ----------------------------------------------------------------------------
// CAiContentPluginManager::NewL()
// ----------------------------------------------------------------------------
//
CAiContentPluginManager* CAiContentPluginManager::NewL()
    {
    CAiContentPluginManager* self = new ( ELeave ) CAiContentPluginManager();         
    CleanupStack::PushL( self );
    self->ConstructL();
    CleanupStack::Pop( self );
    return self;
    }
  
// ----------------------------------------------------------------------------
// CAiContentPluginManager::ConstructL()
// ----------------------------------------------------------------------------
//
void CAiContentPluginManager::ConstructL()
    {
    iPluginFactory = CAiPluginFactory::NewL( iPlugins, *this );

    iStateManager = CAiPluginStateManager::NewL();
    
    iPluginFactory->AddLifecycleObserverL( *iStateManager );
        
    iPluginTool = AiUtility::CreatePluginToolL();
    }

// ----------------------------------------------------------------------------
// CAiContentPluginManager::~CAiContentPluginManager()
// ----------------------------------------------------------------------------
//
CAiContentPluginManager::~CAiContentPluginManager()
    {               
    iPlugins.ResetAndDestroy();
    
    delete iPluginFactory;
    delete iStateManager;
    
    Release( iPluginTool );
    }

// ----------------------------------------------------------------------------
// CAiContentPluginManager::CAiContentPluginManager()
// ----------------------------------------------------------------------------
//
CAiContentPluginManager::CAiContentPluginManager() 
    {
    }

// ----------------------------------------------------------------------------
// CAiContentPluginManager::HandlePluginEvent()
// ----------------------------------------------------------------------------
//
void CAiContentPluginManager::HandlePluginEvent( const TDesC& aParam )    
    {
    const TInt separatorPos( aParam.Locate( KPluginEventSeparator ) );
    
    if( separatorPos == KErrNotFound )
        {
        return;
        }

    // Extract plugin name
    TPtrC pluginName( aParam.Left( separatorPos ) );
    
    // Extract event and parameter string
    TPtrC eventNameAndParams( aParam.Mid( separatorPos + 1 ) );

    // Find parameter string position
    const TInt paramsPos( 
            eventNameAndParams.Locate( KEventParameterSeparator  ) );
    
    // Extract event name
    TPtrC eventName( paramsPos < 0 ? eventNameAndParams : 
                                     eventNameAndParams.Left( paramsPos ) );
    
    // Calculate actual parameter string length by ignoring parenthesis 
    TInt paramsLength( 
            eventNameAndParams.Length() - paramsPos - KAILenOfParenthesis );
    
    // Extract paramenters
    TPtrC param( paramsPos < 0 ? KNullDesC()  :             
            eventNameAndParams.Mid( paramsPos + 1, Max( 0, paramsLength ) ) );
    
    // Resolve plugin
    CAiContentPublisher* target = NULL;
    
    __TIME( "FW: Lookup plug-in by name",
            
        TRAP_IGNORE( target = iPluginFactory->PluginByNameL( pluginName ) );  
    
    );

    __PRINT( __DBG_FORMAT( 
            "\t[I]\t Event: %S to plug-in by addr 0x%x" ), &aParam, target );
    
    if( target )
        {
        // Resolve plugin specific event id
        TInt eventId( KErrNotFound );
        
        TRAP_IGNORE( GetIdL( *target, EAiPublisherEvents, eventName, eventId ) );
        
        // Forward event to plugin
        MAiEventHandlerExtension* ext( 
                iPluginTool->EventHandlerExt( *target ) );
        
        if( ext )
            {
            if( eventId != KErrNotFound )
                {
                ext->HandleEvent( eventId, param );
                }
            else
                {
                ext->HandleEvent( eventName, param );
                }
            }
        }
    }

// ----------------------------------------------------------------------------
// CAiContentPluginManager::HandlePluginEventL()
// ----------------------------------------------------------------------------
//
void CAiContentPluginManager::HandlePluginEventL( 
    const TAiPublisherInfo& aPublisherInfo, const TDesC& aParam )
    {
    // Resolve plugin
    CAiContentPublisher* target( 
            iPluginFactory->PluginByInfoL( aPublisherInfo ) );
              
    if( target )    
        {
        const TInt separatorPos( aParam.Locate( KPluginEventSeparator ) );
        
        // Extract event and parameter string
        TPtrC eventNameAndParams( aParam.Mid( separatorPos + 1 ) );

        // Find parameter string position
        const TInt paramsPos( 
                eventNameAndParams.Locate( KEventParameterSeparator  ) );
        
        // Extract event name
        TPtrC eventName( paramsPos < 0 ? 
            eventNameAndParams : eventNameAndParams.Left( paramsPos ) );
        
        // Calculate actual parameter string length by ignoring parenthesis 
        TInt paramsLength( 
                eventNameAndParams.Length() - paramsPos - KAILenOfParenthesis );
        
        // Extract paramenters
        TPtrC param( paramsPos < 0 ? KNullDesC() :
            eventNameAndParams.Mid( paramsPos + 1, Max( 0, paramsLength ) ) );
        
        // Resolve plugin specific event id
        TInt eventId( KErrNotFound );
        
        GetIdL( *target, EAiPublisherEvents, eventName, eventId );
        
        // Forward event to plugin
        MAiEventHandlerExtension* ext( 
                iPluginTool->EventHandlerExt( *target ) );
        
        if( ext )
            {
            if( eventId != KErrNotFound )
                {
                ext->HandleEvent( eventId, param );
                }
            else
                {
                ext->HandleEvent( eventName, param );
                }
            }        
        }
    }

// ----------------------------------------------------------------------------
// CAiContentPluginManager::HasMenuItemL()
// ----------------------------------------------------------------------------
//
TBool CAiContentPluginManager::HasMenuItemL( 
    const TAiPublisherInfo& aPublisherInfo, const TDesC& aMenuItem )
    {          
    // Resolve plugin
    CAiContentPublisher* target( 
            iPluginFactory->PluginByInfoL( aPublisherInfo ) );
              
    if( target )    
        {
        // Forward query to plugin
        MAiEventHandlerExtension* ext( 
                iPluginTool->EventHandlerExt( *target ) );                 
        
        if ( ext )
            {
            return ext->HasMenuItem( aMenuItem );
            }
        }

    return EFalse;
    }

// ----------------------------------------------------------------------------
// CAiContentPluginManager::RefreshContentL()
// ----------------------------------------------------------------------------
//
TBool CAiContentPluginManager::RefreshContent( const TDesC& aContentCid )
    {
    TRAPD( error, RefreshContentL( aContentCid ) );
    
    return ( error == KErrNone );
    }

// ----------------------------------------------------------------------------
// CAiFw::ProcessOnlineState()
// ----------------------------------------------------------------------------
//
void CAiContentPluginManager::ProcessOnlineState( TBool aOnline )    
    {  
    _LIT( KOnlineOffline, "online_offline" );
    
    for( TInt i = 0; i < iPlugins.Count(); i++ )
        {               
        MAiEventHandlerExtension* ext( 
                iPluginTool->EventHandlerExt( *iPlugins[i] ) );
        
        // If plugin understands online/offline run state change
        if( ext && ext->HasMenuItem( KOnlineOffline ) )
            {            
            if( aOnline )
                {
                iStateManager->ProcessOnlineState( *iPlugins[i] );
                }
            else
                {
                iStateManager->ProcessOfflineState( *iPlugins[i] );                     
                }                                                           
            }
        }       
    }

// ----------------------------------------------------------------------------
// CAiContentPluginManager::StateManager()
// ----------------------------------------------------------------------------
//
CAiPluginStateManager& CAiContentPluginManager::StateManager() const
    {
    return *iStateManager;
    }

// ----------------------------------------------------------------------------
// CAiContentPluginManager::PluginFactory()
// ----------------------------------------------------------------------------
//
CAiPluginFactory& CAiContentPluginManager::PluginFactory() const
    {
    return *iPluginFactory;
    }

// ----------------------------------------------------------------------------
// CAiContentPluginManager::GetIdL()
// ----------------------------------------------------------------------------
//
void CAiContentPluginManager::GetIdL( CAiContentPublisher& aContentPublisher,         
    TAiPublisherProperty aProperty, const TDesC& aName, TInt& aId )                 
    {
    MAiContentItemIterator* iterator = 
        iPluginTool->ContentItemIteratorL( aContentPublisher, aProperty );

    if( iterator )
        {
        const TAiContentItem& ci( iterator->ItemL( aName ) );
        aId = ci.id;
        }
    else
        {
        aId = KErrNotFound;
        }
    }

// ----------------------------------------------------------------------------
// CAiContentPluginManager::RefreshContentL()
// ----------------------------------------------------------------------------
//
TInt CAiContentPluginManager::RefreshContentL( const TDesC& aContentCid )
    {
    TInt retval( KErrNotFound );
    
    // Look up plug-in and content item and delegate to plug-in's
    // MAiContentRequest implementation.

    // Find plugin name
    TInt pos( aContentCid.Locate( KPluginEventSeparator ) );

    if( pos == KErrNotFound )
        {
        return retval;        
        }
    
    TPtrC pluginName( aContentCid.Left( pos ) );
           
    CAiContentPublisher* plugin( iPluginFactory->PluginByNameL( pluginName ) );
    
    if( !plugin )
        {
        return retval;
        }
    
    MAiPropertyExtension* ext( iPluginTool->PropertyExt( *plugin ) );
    
    if( !ext )
        {
        return retval;
        }
        
    // Extract content id
    TPtrC cid( aContentCid.Mid( ++pos ) );
    TInt id( 0 );
    
    MAiContentRequest* handler( NULL );
    
    TRAPD( error, GetIdL( *plugin, EAiPublisherContent, cid, id ) );
    
    if ( !error )
        {
        handler = static_cast< MAiContentRequest* >( 
                    ext->GetPropertyL( EAiContentRequest ) );            
        }
    else
        {
        GetIdL( *plugin, EAiPublisherResources, cid, id );
        
        handler = static_cast< MAiContentRequest* >( 
                    ext->GetPropertyL( EAiResourceRequest ) );           
        }

    // Forward event to plugin
    if( handler && handler->RefreshContent( id ) )
        {               
        retval = KErrNone;        
        }
    
    return retval;
    }

// End of File.
