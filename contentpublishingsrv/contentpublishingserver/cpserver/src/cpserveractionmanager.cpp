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
* Description:  
*
*/

#include <liwcommon.h>
#include <liwservicehandler.h>
#include <s32mem.h>

#include "cpserveractionmanager.h"
#include "cpdebug.h"
#include "cpglobals.h"

using namespace LIW;

// ---------------------------------------------------------------------------
// C++ destructor
// ---------------------------------------------------------------------------
//
CCPActionManager::~CCPActionManager()
    {
    if ( iInterface )
        {
        iInterface->Close( );
        }
    delete iServiceHandler;
    }

// ---------------------------------------------------------------------------
// 
// ---------------------------------------------------------------------------
//
CCPActionManager* CCPActionManager::NewL()
    {
    CCPActionManager* self = CCPActionManager::NewLC( );
    CleanupStack::Pop( self );
    return self;
    }

// ---------------------------------------------------------------------------
// 
// ---------------------------------------------------------------------------
//
CCPActionManager* CCPActionManager::NewLC()
    {
    CCPActionManager* self = new( ELeave ) CCPActionManager;
    CleanupStack::PushL( self );
    self->ConstructL( );
    return self;
    }

// ---------------------------------------------------------------------------
// C++ constructor.
// ---------------------------------------------------------------------------
//
CCPActionManager::CCPActionManager()
    {

    }

// ---------------------------------------------------------------------------
// Standard 2nd phase constructor.
// ---------------------------------------------------------------------------
//
void CCPActionManager::ConstructL()
    {
    CP_DEBUG( _L8("CCPActionManager::ConstructL()") );
    iServiceHandler = CLiwServiceHandler::NewL( );
    }

// -----------------------------------------------------------------------------
// CCPActionManager::ExecuteL
// --------------- --------------------------------------------------------------
//  
void CCPActionManager::ExecuteL( const CLiwGenericParamList& aList )
    {
    CP_DEBUG( _L8("CCPActionManager::ExecuteL()") );
    if ( !iInterface )
        {
        PrepareServiceL( iInterface );
        }
    CLiwGenericParamList* inparam = CLiwGenericParamList::NewLC( );
    CLiwGenericParamList* outparam = CLiwGenericParamList::NewLC( );
    PrepareInputListL( aList, *inparam );

    iInterface->ExecuteCmdL( KCmdExecute, *inparam, *outparam ) ;
    TInt pos(0);
    outparam->FindFirst( pos, EGenericParamError );
    if ( pos != KErrNotFound )
        {
        User::LeaveIfError( ( *outparam )[pos].Value().AsTInt32( ) );
        }
    else
        {
        User::Leave( KErrNotFound );
        }
    CleanupStack::PopAndDestroy( outparam );
    CleanupStack::PopAndDestroy( inparam );
    }

// -----------------------------------------------------------------------------
// CCPActionManager::ExecuteL
// --------------- --------------------------------------------------------------
//  
void CCPActionManager::PrepareServiceL( MLiwInterface*& aInterface )
    {
    CP_DEBUG( _L8("CCPActionManager::PrepareServiceL()") );
    CLiwGenericParamList* inparam = &(iServiceHandler->InParamListL( ) );
    CLiwGenericParamList* outparam = &(iServiceHandler->OutParamListL( ) );
    CLiwCriteriaItem* crit = CLiwCriteriaItem::NewL( 1,
        KActionHandlerInterface, KActionHandlerService );
    CleanupStack::PushL( crit );
    crit->SetServiceClass( TUid::Uid( KLiwClassBase ) );
    RCriteriaArray array;
    CleanupClosePushL( array );
    array.AppendL( crit );
    iServiceHandler->AttachL( array );
    iServiceHandler->ExecuteServiceCmdL( *crit, *inparam, *outparam );
    CleanupStack::PopAndDestroy( &array );
    CleanupStack::PopAndDestroy( crit );
    TInt pos = 0;
    outparam->FindFirst( pos, KActionHandlerInterface );
    if ( pos != KErrNotFound )
        {
        aInterface = ( *outparam )[pos].Value().AsInterface( );
        }
    else
        {
        User::Leave( KErrNotFound );
        }
    }

// -----------------------------------------------------------------------------
// CCPActionManager::PrepareInputListL
// --------------- --------------------------------------------------------------
//
void CCPActionManager::PrepareInputListL( const CLiwGenericParamList& aList,
    CLiwGenericParamList& aTarget )
    {
    CP_DEBUG( _L8("CCPActionManager::PrepareInputListL()") );
    const TLiwGenericParam* param= NULL;
    TInt pos( 0);
    param = aList.FindFirst( pos, KListMap, EVariantTypeMap );
    if ( param && pos !=KErrNotFound )
        {
        CLiwDefaultMap* map = CLiwDefaultMap::NewLC( );
        param->Value().Get( *map );
        ExtractUidAndMapL( *map, aTarget );
        CleanupStack::PopAndDestroy( map );
        }
    else
        {
        User::Leave( KErrNotFound );
        }
    }

// -----------------------------------------------------------------------------
// CCPActionManager::ExtractUidAndMapL
// --------------- --------------------------------------------------------------
//
void CCPActionManager::ExtractUidAndMapL( const CLiwDefaultMap& aMap,
    CLiwGenericParamList& aTarget )
    {
    CP_DEBUG( _L8("CCPActionManager::ExtractUidAndMapL()") );
    TLiwVariant variant;
    variant.PushL( );
    if ( aMap.FindL( KActionMap, variant ) )
        {
        CLiwDefaultMap* map = CLiwDefaultMap::NewLC( );
        variant.Get( *map );
        if ( map->FindL( KDataForActionHandler, variant ) )
            {
            TLiwGenericParam param( KDataForActionHandler, variant);
            aTarget.AppendL( param );
            }
        if ( map->FindL( KPluginUid, variant ) )
            {
            TLiwGenericParam param( KPluginUid, variant);
            aTarget.AppendL( param );
            }
        CleanupStack::PopAndDestroy( map );
        }

    CleanupStack::PopAndDestroy( &variant );
    }

