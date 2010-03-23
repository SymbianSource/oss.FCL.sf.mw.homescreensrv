/*
* Copyright (c) 2007 Nokia Corporation and/or its subsidiary(-ies).
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


#include <ecom/ImplementationProxy.h>
#include <LiwCommon.h>
#include <LiwServiceIfBase.h>

#include "cpclientservice.h"
#include "cpdebug.h"
#include "cpglobals.h"
#include "cdatasourceinterface.h"
#include "ccontentpublishinginterface.h"

const TInt implUid( 0x20016B7E );
const TInt implContentUid( 0x2001955F );

using namespace LIW;

// ---------------------------------------------------------------------------
// 
// ---------------------------------------------------------------------------
//
CCPClientService::CCPClientService()
    {

    }

// ---------------------------------------------------------------------------
// 
// ---------------------------------------------------------------------------
//
CCPClientService* CCPClientService::NewL()
    {
    return new(ELeave) CCPClientService();
    }

// ---------------------------------------------------------------------------
// 
// ---------------------------------------------------------------------------
//
CCPClientService::~CCPClientService()
    {

    }

// ---------------------------------------------------------------------------
// 
// ---------------------------------------------------------------------------
//
void CCPClientService::InitialiseL(
    MLiwNotifyCallback& /*aFrameworkCallback*/, const RCriteriaArray& /*aInterest*/)
    {

    }

// ---------------------------------------------------------------------------
// 
// ---------------------------------------------------------------------------
//
void CCPClientService::HandleServiceCmdL( const TInt& aCmdId,
    const CLiwGenericParamList& aInParamList,
    CLiwGenericParamList& aOutParamList, TUint aCmdOptions,
    const MLiwNotifyCallback* aCallback )
    {
    CP_DEBUG( _L8("CCPClientService::HandleServiceCmdL()") );
    if ( (aCallback ) || (aCmdOptions ) )
        {
        // report error if request is anything other than synchronous
        //or user sends a callback parameter
        aOutParamList.AppendL( TLiwGenericParam( EGenericParamError,
            TLiwVariant( KErrNotSupported ) ) );
        aOutParamList.AppendL( TLiwGenericParam( KErrorCode,
            TLiwVariant( KErrNotSupported ) ) );
        return;
        }

    TPtrC8 cmdName;
    const TLiwGenericParam* cmd= NULL;

    if ( aCmdId == KLiwCmdAsStr )
        {
        TInt pos = 0;
        cmd = aInParamList.FindFirst( pos, KCommand );
        if ( cmd )
            {
            cmdName.Set( cmd->Value().AsData( ) );
            }
        else
            {
            aOutParamList.AppendL( TLiwGenericParam( EGenericParamError,
                TLiwVariant( KErrArgument ) ) );
            aOutParamList.AppendL( TLiwGenericParam( KErrorCode,
                TLiwVariant( KErrArgument ) ) );
            }
        }
    else
        {
        aOutParamList.AppendL( TLiwGenericParam( EGenericParamError,
            TLiwVariant( KErrNotSupported ) ) );
        aOutParamList.AppendL( TLiwGenericParam( KErrorCode,
            TLiwVariant( KErrNotSupported ) ) );
        }
    if ( !cmdName.CompareF( KCPInterface ) )
        {
        //Create interface pointer and return the output param
        CDataSourceInterface* ifp =CDataSourceInterface::NewLC( );
        aOutParamList.AppendL( TLiwGenericParam( KCPInterface,
            TLiwVariant( ifp ) ) );
        CleanupStack::Pop( ifp );
        return;
        }
    else if ( !cmdName.CompareF( KCPContentInterface ) )
        {
        //Create interface pointer and return the output param
        CContentPublishingInterface* ifp = 
            CContentPublishingInterface::NewLC( );
        aOutParamList.AppendL( TLiwGenericParam( KCPContentInterface,
            TLiwVariant( ifp ) ) );
        CleanupStack::Pop( ifp );
        return;
        }
    else
        {
        aOutParamList.AppendL( TLiwGenericParam( EGenericParamError,
            TLiwVariant( KErrNotSupported ) ) );
        aOutParamList.AppendL( TLiwGenericParam( KErrorCode,
            TLiwVariant( KErrNotSupported ) ) );
        }
    }

// ---------------------------------------------------------
// Map the interface UIDs to implementation factory functions
// ---------------------------------------------------------
//
const TImplementationProxy ImplementationTable[] =
    {
    IMPLEMENTATION_PROXY_ENTRY( implUid, CCPClientService::NewL ),
    IMPLEMENTATION_PROXY_ENTRY( implContentUid, CCPClientService::NewL )
    };

// ---------------------------------------------------------
// Exported proxy for instantiation method resolution
// ---------------------------------------------------------
//
EXPORT_C const TImplementationProxy* ImplementationGroupProxy(
        TInt& aTableCount)
    {
    aTableCount = sizeof(ImplementationTable) / sizeof(TImplementationProxy);
    return ImplementationTable;
    }
