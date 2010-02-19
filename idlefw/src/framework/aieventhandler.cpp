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
* Description:  Content plugin event handler
*
*/

// System includes

// User includes
#include <aiutility.h>
#include <aiplugintool.h>
#include <aicontentrequest.h>

#include "aipluginfactory.h"
#include "aiconsts.h"
#include "debug.h"

#include "aieventhandler.h"

// Constants
const TInt KAILenOfParenthesis( 2 );

// ======== MEMBER FUNCTIONS ========

// ----------------------------------------------------------------------------
// CAiEventHandler::NewL()
//
// ----------------------------------------------------------------------------
//
CAiEventHandler* CAiEventHandler::NewL( 
    CAiPluginFactory& aFactory )
    {
    CAiEventHandler* self = 
        new ( ELeave ) CAiEventHandler( aFactory );         
    
    CleanupStack::PushL( self );
    self->ConstructL();
    CleanupStack::Pop( self );
    return self;
    }
  
// ----------------------------------------------------------------------------
// CAiEventHandler::ConstructL()
//
// ----------------------------------------------------------------------------
//
void CAiEventHandler::ConstructL()
    {             
    iPluginTool = AiUtility::CreatePluginToolL();
    }

// ----------------------------------------------------------------------------
// CAiEventHandler::~CAiEventHandler()
//
// ----------------------------------------------------------------------------
//
CAiEventHandler::~CAiEventHandler()
    {                     
    Release( iPluginTool );
    }

// ----------------------------------------------------------------------------
// CAiEventHandler::CAiEventHandler()
//
// ----------------------------------------------------------------------------
//
CAiEventHandler::CAiEventHandler( CAiPluginFactory& aFactory ) 
    : iFactory( aFactory )
    {
    }

// ----------------------------------------------------------------------------
// CAiEventHandler::HandlePluginEvent()
//
// ----------------------------------------------------------------------------
//
void CAiEventHandler::HandlePluginEvent( const TDesC& aParam )    
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
    CHsContentPublisher* publisher( NULL );
    
    __TIME( "FW: Lookup plug-in by name",            
        publisher = iFactory.PluginByName( pluginName );      
    );

    __PRINT( __DBG_FORMAT( 
            "\t[I]\t Event: %S to plug-in by addr 0x%x" ), &aParam, publisher );
    
    if( publisher )
        {
        // Resolve plugin specific event id
        TInt eventId( KErrNotFound );
        
        TRAP_IGNORE( GetIdL( *publisher, 
            CHsContentPublisher::EPublisherEvents, eventName, eventId ) );
        
        if( eventId != KErrNotFound )
            {
            publisher->HandleEvent( eventId, param );
            }
        else
            {
            publisher->HandleEvent( eventName, param );
            }
        }
    }

// ----------------------------------------------------------------------------
// CAiEventHandler::HandlePluginEventL()
//
// ----------------------------------------------------------------------------
//
void CAiEventHandler::HandlePluginEventL( 
    const THsPublisherInfo& aPublisherInfo, const TDesC& aParam )
    {
    // Resolve plugin
    CHsContentPublisher* publisher( 
        iFactory.PluginByInfo( aPublisherInfo ) );
              
    if( publisher )    
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
        
        GetIdL( *publisher, 
            CHsContentPublisher::EPublisherEvents, eventName, eventId );
                
        if( eventId != KErrNotFound )
            {
            publisher->HandleEvent( eventId, param );
            }
        else
            {
            publisher->HandleEvent( eventName, param );
            }
        }
    }

// ----------------------------------------------------------------------------
// CAiEventHandler::HasMenuItemL()
//
// ----------------------------------------------------------------------------
//
TBool CAiEventHandler::HasMenuItemL( const THsPublisherInfo& aPublisherInfo,  
    const TDesC& aMenuItem )
    {          
    // Resolve plugin
    CHsContentPublisher* publisher( 
        iFactory.PluginByInfo( aPublisherInfo ) );
              
    if( publisher )    
        {
        return publisher->HasMenuItem( aMenuItem );            
        }

    return EFalse;
    }

// ----------------------------------------------------------------------------
// CAiEventHandler::RefreshContent()
//
// ----------------------------------------------------------------------------
//
TBool CAiEventHandler::RefreshContent( const TDesC& aContentCid )
    {
    TRAPD( error, RefreshContentL( aContentCid ) );
    
    return ( error == KErrNone );
    }

// ----------------------------------------------------------------------------
// CAiEventHandler::RefreshContent()
//
// ----------------------------------------------------------------------------
//
TBool CAiEventHandler::RefreshContent( const THsPublisherInfo& aPublisherInfo, 
    const TDesC& aContentCid )
    {
    // Resolve plugin
    CHsContentPublisher* publisher( 
        iFactory.PluginByInfo( aPublisherInfo ) );
    
    if( publisher )    
        {
        TInt id( KErrNotFound );
        
        TRAP_IGNORE( GetIdL( *publisher, 
            CHsContentPublisher::EPublisherContent, aContentCid, id ) ); 
            
        if ( id != KErrNotFound )
            {
            MAiContentRequest* handler = static_cast< MAiContentRequest* >(
                publisher->GetProperty( CHsContentPublisher::EContentRequest ) );
            
            if ( handler )
                {
                return handler->RefreshContent( id );
                }            
            }
        }
    
    return EFalse;
    }

// ----------------------------------------------------------------------------
// CAiEventHandler::SuspendContent()
//
// ----------------------------------------------------------------------------
//
TBool CAiEventHandler::SuspendContent( const THsPublisherInfo& aPublisherInfo,  
    const TDesC& aContentCid )
    {
    // Resolve plugin
    CHsContentPublisher* publisher( 
        iFactory.PluginByInfo( aPublisherInfo ) );
    
    if( publisher )    
        {
        TInt id( KErrNotFound );
        
        TRAP_IGNORE( GetIdL( *publisher, 
            CHsContentPublisher::EPublisherContent, aContentCid, id ) ); 

        if ( id != KErrNotFound )
            {
            MAiContentRequest* handler = static_cast< MAiContentRequest* >(
                publisher->GetProperty( CHsContentPublisher::EContentRequest ) );
            
            if ( handler )
                {
                return handler->SuspendContent( id );
                }            
            }
        }
    
    return EFalse;
    }

// ----------------------------------------------------------------------------
// CAiEventHandler::GetIdL()
//
// ----------------------------------------------------------------------------
//
void CAiEventHandler::GetIdL( CHsContentPublisher& aContentPublisher,         
    CHsContentPublisher::TProperty aProperty, const TDesC& aName, TInt& aId )                    
    {
    MAiContentItemIterator* iterator = 
        iPluginTool->ContentItemIterator( aContentPublisher, aProperty );

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
// CAiEventHandler::RefreshContentL()
//
// ----------------------------------------------------------------------------
//
TInt CAiEventHandler::RefreshContentL( const TDesC& aContentCid )
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
           
    CHsContentPublisher* publisher( iFactory.PluginByName( pluginName ) );
    
    if( !publisher )
        {
        return retval;
        }
            
    // Extract content id
    TPtrC cid( aContentCid.Mid( ++pos ) );
    TInt id( 0 );
    
    MAiContentRequest* handler( NULL );
    
    TRAPD( error, GetIdL( *publisher, 
        CHsContentPublisher::EPublisherContent, cid, id ) );
    
    if ( !error )
        {
        handler = static_cast< MAiContentRequest* >( 
            publisher->GetProperty( CHsContentPublisher::EContentRequest ) );            
        }
    else
        {
        GetIdL( *publisher, 
            CHsContentPublisher::EPublisherResources, cid, id );
        
        handler = static_cast< MAiContentRequest* >( 
            publisher->GetProperty( CHsContentPublisher::EResourceRequest ) );           
        }

    // Forward event to plugin
    if( handler && handler->RefreshContent( id ) )
        {               
        retval = KErrNone;        
        }
    
    return retval;
    }

// End of File.
