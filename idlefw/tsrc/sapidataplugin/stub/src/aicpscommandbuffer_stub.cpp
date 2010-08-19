/*
* Copyright (c) 2010 Nokia Corporation and/or its subsidiary(-ies).
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
* Description:  Cps command buffer
*
*/


// System includes
#include <liwservicehandler.h>
#include <liwvariant.h>
#include <liwgenericparam.h>

// User includes
#include "caicpscommandbuffer_stub.h"
#include "aicpsexecuteparam.h"
#include <debug.h>

// Constants
_LIT8( KCPSConfigurationIf, "IContentPublishing" );
_LIT8( KCPS, "Service.ContentPublishing" );
_LIT8( KExecuteAction, "ExecuteAction" );
_LIT8( KExecuteMultipleActions, "ExecuteMultipleActions" );
_LIT8( KFilters, "filters" );

// ======== LOCAL FUNCTIONS ========

// ======== MEMBER FUNCTIONS ========
// ---------------------------------------------------------------------------
// CAiCpsCommandBuffer::CAiCpsCommandBuffer
//
// ---------------------------------------------------------------------------
//
CAiCpsCommandBuffer::CAiCpsCommandBuffer()
    {
    }

// ---------------------------------------------------------------------------
// CAiCpsCommandBuffer::ConstructL
//
// ---------------------------------------------------------------------------
//
void CAiCpsCommandBuffer::ConstructL()
    {  
    }

// ---------------------------------------------------------------------------
// CAiCpsCommandBuffer::NewL
//
// ---------------------------------------------------------------------------
//
CAiCpsCommandBuffer* CAiCpsCommandBuffer::NewL()
    {
    CAiCpsCommandBuffer* self = CAiCpsCommandBuffer::NewLC();
    CleanupStack::Pop( self );
    return self;
    }

// ---------------------------------------------------------------------------
// CAiCpsCommandBuffer::NewLC
//
// ---------------------------------------------------------------------------
//
CAiCpsCommandBuffer* CAiCpsCommandBuffer::NewLC()
    {
    CAiCpsCommandBuffer* self = new ( ELeave ) CAiCpsCommandBuffer;
    CleanupStack::PushL( self );
    self->ConstructL();
    return self;
    }

// ---------------------------------------------------------------------------
// CAiCpsCommandBuffer::~CAiCpsCommandBuffer
//
// ---------------------------------------------------------------------------
//
CAiCpsCommandBuffer::~CAiCpsCommandBuffer()
    {
    // Flush any pending commands
    Flush();
    
    if ( iCpsInterface )
       {
       // Close interface
       iCpsInterface->Close();
       }
    
    TRAP_IGNORE( DetachL() );
    
    delete iCpsService;
    delete iServiceHandler;    
    }

// ---------------------------------------------------------------------------
// CAiCpsCommandBuffer::DetachL
//
// ---------------------------------------------------------------------------
//
void CAiCpsCommandBuffer::DetachL()
    {
    if ( iServiceHandler && iCpsService )
        {                   
        // Detach services from the handler       
        RCriteriaArray list;
        CleanupClosePushL( list );
     
        list.AppendL( iCpsService );
       
        iServiceHandler->DetachL( list );    
        
        CleanupStack::PopAndDestroy( &list );
        }
    }

// ---------------------------------------------------------------------------
// CAiCpsCommandBuffer::GetCPSInterfaceL
//
// ---------------------------------------------------------------------------
//
void CAiCpsCommandBuffer::GetCPSInterfaceL()
    {
    if ( iCpsInterface )
        {
        return;
        }
    
    RCriteriaArray interestList;
    CleanupClosePushL( interestList );
    
    CLiwServiceHandler* serviceHandler = CLiwServiceHandler::NewL();
    CleanupStack::PushL( serviceHandler );
          
    // Attach to CPS:    
    CLiwCriteriaItem* cpsService = CLiwCriteriaItem::NewL( 1, KCPSConfigurationIf, KCPS );
    CleanupStack::PushL( cpsService );
    
    cpsService->SetServiceClass( TUid::Uid( KLiwClassBase ) );
    
    interestList.AppendL( cpsService );
    serviceHandler->AttachL( interestList );

    CLiwGenericParamList& inParamList( serviceHandler->InParamListL() );
    CLiwGenericParamList& outParamList( serviceHandler->OutParamListL() );
    
    serviceHandler->ExecuteServiceCmdL(
          *cpsService,
          inParamList,
          outParamList );
    
    TInt pos( 0 );
    
    outParamList.FindFirst( pos, KCPSConfigurationIf );
    
    if ( pos != KErrNotFound )
        {
        iCpsInterface = (outParamList)[pos].Value().AsInterface();
        inParamList.Reset();
        outParamList.Reset();
        }
    else
        {
        inParamList.Reset();
        outParamList.Reset();
        User::Leave( KErrNotFound );
        }
    
    CleanupStack::Pop( cpsService );   
    iCpsService = cpsService;
    
    CleanupStack::Pop( serviceHandler );    
    iServiceHandler = serviceHandler;
    
    CleanupStack::PopAndDestroy( &interestList );
    }

// ---------------------------------------------------------------------------
// CAiCpsCommandBuffer::AddCommand
//
// ---------------------------------------------------------------------------
//
void CAiCpsCommandBuffer::AddCommand(  const TDesC& aPluginId,
        const TDesC& aType, CLiwDefaultMap* aFilter,
        const TDesC8& aAction )
    {
    __PRINTS( "CAiCpsCommandBuffer::AddCommand, start" );
   
    TRAP_IGNORE( DoAddCommandL( aPluginId, aType, aFilter, aAction ) );

    __PRINTS( "CAiCpsCommandBuffer::AddCommand - done" );
    }

// ---------------------------------------------------------------------------
// CAiCpsCommandBuffer::ServiceHandler
//
// ---------------------------------------------------------------------------
//
CLiwServiceHandler* CAiCpsCommandBuffer::ServiceHandler() const
    {
    return iServiceHandler;
    }

// ---------------------------------------------------------------------------
// CAiCpsCommandBuffer::CpsInterface
//
// ---------------------------------------------------------------------------
//
MLiwInterface* CAiCpsCommandBuffer::CpsInterface() const
    {
    return iCpsInterface;
    }

// ---------------------------------------------------------------------------
// CAiCpsCommandBuffer::DoAddCommandL
//
// ---------------------------------------------------------------------------
//
void CAiCpsCommandBuffer::DoAddCommandL( const TDesC& aPluginId,
    const TDesC& aType, CLiwDefaultMap* aFilter,
    const TDesC8& aAction )
    {
    TInt found( KErrNotFound );
    
    for ( TInt i = 0; i < iPlugins.Count(); i++ )
        {
        if ( aPluginId == iPlugins[i]->PluginId() )
            {
            found = i;
            break;
            }
        }

    if ( found != KErrNotFound )
        {
        iPlugins[found]->AddActionL( aAction );
        }
    else
        {
        CAiCpsExecuteParam* param = CAiCpsExecuteParam::NewLC();
        param->SetPluginIdL( aPluginId );
        param->SetRegistryTypeL( aType );
        param->SetFilterL( aFilter );
        param->AddActionL( aAction );
        iPlugins.AppendL( param );
        CleanupStack::Pop( param );
        }    
    }

// ---------------------------------------------------------------------------
// CAiCpsCommandBuffer::Flush
//
// ---------------------------------------------------------------------------
//
void CAiCpsCommandBuffer::Flush()
    {
    __PRINTS( "CAiCpsCommandBuffer::Flush, start" );

    if ( iPlugins.Count() > 0 )
        {
        TRAP_IGNORE( DoFlushL() );
        }
    
    __PRINTS( "CAiCpsCommandBuffer::Flush - done" );
    }

// ---------------------------------------------------------------------------
// CAiCpsCommandBuffer::DoFlushL
//
// ---------------------------------------------------------------------------
//
void CAiCpsCommandBuffer::DoFlushL()
    {
    if ( !iCpsInterface )
       {
       GetCPSInterfaceL();
       }

    if ( iCpsInterface )
        {
    __PRINTS( "CAiCpsCommandBuffer::DoFlush : Execute" );
        
        TInt pluginCount( iPlugins.Count() );
        
        CLiwDefaultList* pluginCmdList = CLiwDefaultList::NewLC();
        
        for ( TInt i = 0; i < pluginCount; i++ )
            {
            CLiwDefaultMap* inParamMap = iPlugins[i]->InParamMapLC();
            pluginCmdList->AppendL( inParamMap );
            CleanupStack::PopAndDestroy( inParamMap );            
            }
        
        CLiwGenericParamList* inParamList  = CLiwGenericParamList::NewLC();
        CLiwGenericParamList* outParamList  = CLiwGenericParamList::NewLC();
        
        TLiwGenericParam item( KFilters, TLiwVariant ( pluginCmdList ) );
        inParamList->AppendL( item ); 
      
        iCpsInterface->ExecuteCmdL( KExecuteMultipleActions, *inParamList, *outParamList);
       
        CleanupStack::PopAndDestroy( 3, pluginCmdList ); // outparamList, inParamList
        
        iPlugins.ResetAndDestroy();
        }    
    }

// End of file
