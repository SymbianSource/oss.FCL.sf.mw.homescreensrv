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
* Description:  CPS Execute parameter object
*
*/


// System includes

// User includes
#include <debug.h>
#include <liwvariant.h>
#include <liwgenericparam.h>
#include "aicpsexecuteparam.h"

// Constants
_LIT8( KPublisherId, "publisher" );
_LIT8( KContentType,   "content_type" );
_LIT8( KContentId,   "content_id" );
_LIT8( KPluginId, "plugin_id");
_LIT8( KType,          "type");
_LIT8( KFilter, "filter" );
_LIT8( KActionTrigger, "action_trigger" );

// ======== LOCAL FUNCTIONS ========

// ======== MEMBER FUNCTIONS ========
// ---------------------------------------------------------------------------
// CAiCpsExecuteParam::CAiCpsExecuteParam
//
// ---------------------------------------------------------------------------
//  
CAiCpsExecuteParam::CAiCpsExecuteParam()
    {
    
    }

// ---------------------------------------------------------------------------
// CAiCpsExecuteParam::ConstructL
//
// ---------------------------------------------------------------------------
//
void CAiCpsExecuteParam::ConstructL( ) 
    { 
    }

// ---------------------------------------------------------------------------
// CAiCpsExecuteParam::NewL
//
// ---------------------------------------------------------------------------
//
CAiCpsExecuteParam* CAiCpsExecuteParam::NewL()
    {
    CAiCpsExecuteParam* self = CAiCpsExecuteParam::NewLC();
    CleanupStack::Pop( self );
    return self;
    }

// ---------------------------------------------------------------------------
// CAiCpsExecuteParam::NewLC
//
// ---------------------------------------------------------------------------
//
CAiCpsExecuteParam* CAiCpsExecuteParam::NewLC()
    {
    CAiCpsExecuteParam* self = new ( ELeave ) CAiCpsExecuteParam;
    CleanupStack::PushL( self );
    self->ConstructL();
    return self;
    }
// ---------------------------------------------------------------------------
// CAiCpsExecuteParam::~CAiCpsExecuteParam
//
// ---------------------------------------------------------------------------
//  
CAiCpsExecuteParam::~CAiCpsExecuteParam()
    {
    delete iPluginId;
    delete iRegistryType;
    delete iPublisher;
    delete iContentType;
    delete iContentId;
    iActions.ResetAndDestroy();
    }

// ---------------------------------------------------------------------------
// CAiCpsExecuteParam::PluginId
//
// ---------------------------------------------------------------------------
//
const TDesC& CAiCpsExecuteParam::PluginId() const
    {
    return *iPluginId;
    }

// ---------------------------------------------------------------------------
// CAiCpsExecuteParam::InParamListLC
//
// ---------------------------------------------------------------------------
//  
CLiwGenericParamList* CAiCpsExecuteParam::InParamListLC() 
    {
    CLiwGenericParamList* paramList = CLiwGenericParamList::NewLC();
    
    TLiwGenericParam pluginId( KPluginId, TLiwVariant( iPluginId ) );
    paramList->AppendL( pluginId );
    TLiwGenericParam type( KType, TLiwVariant( iRegistryType ) );
    paramList->AppendL( type );
    
    CLiwDefaultMap* filter = CLiwDefaultMap::NewLC();
    filter->InsertL( KPublisherId, TLiwVariant(iPublisher ));
    filter->InsertL( KContentId, TLiwVariant(iContentId ));
    filter->InsertL( KContentType, TLiwVariant(iContentType ));
    
    CLiwDefaultList* actionsToLaunch = CLiwDefaultList::NewLC();
    for ( TInt i=0; i< iActions.Count(); i++)
        {
        actionsToLaunch->AppendL( TLiwVariant( *iActions[i]));
        }
    filter->InsertL(KActionTrigger, TLiwVariant(actionsToLaunch) );
    
    TLiwGenericParam item( KFilter, TLiwVariant( filter ));
    paramList->AppendL( item );
    CleanupStack::PopAndDestroy( actionsToLaunch );
    CleanupStack::PopAndDestroy( filter );
    return paramList;
    }

// ---------------------------------------------------------------------------
// CAiCpsExecuteParam::SetFilterL
//
// ---------------------------------------------------------------------------
//  
void CAiCpsExecuteParam::SetFilterL(CLiwDefaultMap* aMap)
    {
    delete iPublisher;
    delete iContentType;
    delete iContentId;
    iPublisher = NULL;
    iContentType = NULL;
    iContentId = NULL;
    
    TLiwVariant variant;
    if ( aMap->FindL(KPublisherId, variant ))
        {
        iPublisher = variant.AsDes().AllocL();
        }
    if ( aMap->FindL(KContentType, variant ))
        {
        iContentType= variant.AsDes().AllocL();
        }
    if ( aMap->FindL(KContentId, variant ))
        {
        iContentId= variant.AsDes().AllocL();
        }
    }

// ---------------------------------------------------------------------------
// CAiCpsExecuteParam::SetRegistryTypeL
//
// ---------------------------------------------------------------------------
//  
void CAiCpsExecuteParam::SetRegistryTypeL(const TDesC& aRegistryType)
    {
    delete iRegistryType;
    iRegistryType = NULL;
    iRegistryType = aRegistryType.AllocL();
    }

// ---------------------------------------------------------------------------
// CAiCpsExecuteParam::SetPluginIdL
//
// ---------------------------------------------------------------------------
// 
void CAiCpsExecuteParam::SetPluginIdL(const TDesC& aPluginId)
    {
    delete iPluginId;
    iPluginId = NULL;
    iPluginId = aPluginId.AllocL();
    }
 
// ---------------------------------------------------------------------------
// CAiCpsExecuteParam::AddActionL
//
// ---------------------------------------------------------------------------
//
void CAiCpsExecuteParam::AddActionL(const TDesC8& aAction)
    {
    iActions.Append(aAction.AllocL());
    }

// End of file
