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

// User includes
#include <debug.h>
#include <liwservicehandler.h>
#include <liwvariant.h>
#include <liwgenericparam.h>

#include "caicpscommandbuffer.h"
#include "aicpsexecuteparam.h"

// Constants
_LIT8( KCPSConfigurationIf, "IContentPublishing" );
_LIT8( KCPS, "Service.ContentPublishing" );
_LIT8( KExecuteAction, "ExecuteAction" );

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
    
    if ( iServiceHandler && iCpsService )
       {
       // Detach services from the handler
       RCriteriaArray interestList;
    
       TRAP_IGNORE( interestList.AppendL( iCpsService );
                    iServiceHandler->DetachL( interestList ); );
    
       interestList.Reset();
       }
    
    delete iCpsService;
    delete iServiceHandler;
    
    }

// ---------------------------------------------------------------------------
// CAiCpsCommandBuffer::GetCPSInterfaceL
//
// ---------------------------------------------------------------------------
//
void CAiCpsCommandBuffer::GetCPSInterfaceL()
    {
    iServiceHandler = CLiwServiceHandler::NewL();
    
    RCriteriaArray interestList;
    
    // Attach to CPS:
    iCpsService = CLiwCriteriaItem::NewL( 1, KCPSConfigurationIf, KCPS );
    iCpsService->SetServiceClass( TUid::Uid( KLiwClassBase ) );
    
    interestList.AppendL( iCpsService );
    iServiceHandler->AttachL( interestList );
    interestList.Reset();
    
    CLiwGenericParamList& inParamList( iServiceHandler->InParamListL() );
    CLiwGenericParamList& outParamList( iServiceHandler->OutParamListL() );
    
    iServiceHandler->ExecuteServiceCmdL(
          *iCpsService,
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
// CAiCpsCommandBuffer::DoAddCommandL
//
// ---------------------------------------------------------------------------
//
void CAiCpsCommandBuffer::DoAddCommandL( const TDesC& aPluginId,
        const TDesC& aType, CLiwDefaultMap* aFilter,
        const TDesC8& aAction )
    {
    TInt found = KErrNotFound;
    for (TInt i=0; i< iPlugins.Count(); i++)
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
    if ( !iCpsInterface   )
       {
       GetCPSInterfaceL();
       }
	   
    if(iCpsInterface)
        {
        TInt pluginCount = iPlugins.Count();
        for (TInt i=0; i < pluginCount; i++ )
            {
            CLiwGenericParamList* inParamList  = iPlugins[i]->InParamListLC();
            CLiwGenericParamList* outParamList  = CLiwGenericParamList::NewLC();
            __PRINTS( "CAiCpsCommandBuffer::DoFlush : Execute" );
            iCpsInterface->ExecuteCmdL( KExecuteAction, *inParamList, *outParamList);
            
            CleanupStack::PopAndDestroy( outParamList );
            CleanupStack::PopAndDestroy( inParamList );
            }
        }
    else
       {
       User::Leave( KErrNotSupported );
       }
    iPlugins.ResetAndDestroy();
    }

// End of file
