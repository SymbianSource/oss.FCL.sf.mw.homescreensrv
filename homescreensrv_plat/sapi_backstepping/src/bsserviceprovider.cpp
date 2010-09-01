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
* Description:  Back Stepping Service Provider
 *
*/


#include <ecom/implementationproxy.h>
#include <liwcommon.h>

#include "bsserviceprovider.h"
#include "bsserviceconstants.h"
#include "bsserviceinterface.h"

using namespace LIW;

// ======== LOCAL FUNCTIONS =========

// ---------------------------------------------------------------------------
// Map the interface UIDs to implementation factory functions
// ---------------------------------------------------------------------------
//
const TImplementationProxy ImplementationTable[] =
    {
    IMPLEMENTATION_PROXY_ENTRY( KBSServiceImplUid, CBSServiceProvider::NewL )
    };

// ---------------------------------------------------------------------------
// Exported proxy for instantiation method resolution
// ---------------------------------------------------------------------------
//
EXPORT_C const TImplementationProxy* ImplementationGroupProxy(TInt& aTableCount)
    {
    aTableCount = sizeof(ImplementationTable) / sizeof(TImplementationProxy);
    return ImplementationTable;
    }

// ======== MEMBER FUNCTIONS ========

// ---------------------------------------------------------------------------
// 
// ---------------------------------------------------------------------------
//
CBSServiceProvider::CBSServiceProvider()
    {
    }

// ---------------------------------------------------------------------------
// 
// ---------------------------------------------------------------------------
//
CBSServiceProvider* CBSServiceProvider::NewL()
    {
    return new (ELeave) CBSServiceProvider();
    }

// ---------------------------------------------------------------------------
// 
// ---------------------------------------------------------------------------
//
CBSServiceProvider::~CBSServiceProvider()
    {
    }

// ---------------------------------------------------------------------------
// 
// ---------------------------------------------------------------------------
//
void CBSServiceProvider::InitialiseL(
    MLiwNotifyCallback& /*aFrameworkCallback*/, const RCriteriaArray& /*aInterest*/)
    {
    }

// ---------------------------------------------------------------------------
// 
// ---------------------------------------------------------------------------
//
void CBSServiceProvider::HandleServiceCmdL( const TInt& aCmdId,
    const CLiwGenericParamList& aInParamList,
    CLiwGenericParamList& aOutParamList, TUint aCmdOptions,
    const MLiwNotifyCallback* aCallback )

    {
    if ( (aCallback ) || (aCmdOptions ) )
        {
        // report error if request is anything other than synchronous
        // or user sends a callback parameter
        HandleErrorL( aOutParamList, KErrNotSupported );
        return;
        }

    TPtrC8 cmdName;
    const TLiwGenericParam* cmd= NULL;
    if ( aCmdId == KLiwCmdAsStr )
        {
        TInt pos( 0);
        cmd = aInParamList.FindFirst( pos, KGenericParamServiceCmdIDStr );
        if ( cmd )
            {
            cmdName.Set( cmd->Value().AsData( ) );
            }
        else
            {
            HandleErrorL( aOutParamList, KErrArgument );
            }
        }
    else
        {
        HandleErrorL( aOutParamList, KErrNotSupported );
        }

    if ( !cmdName.CompareF( KBSInterface ) )
        {
        // create interface pointer and return the output param
        CBSServiceInterface* ifp = CBSServiceInterface::NewLC( );
        aOutParamList.AppendL( TLiwGenericParam( KBSInterface,
            TLiwVariant( ifp ) ) );
        CleanupStack::Pop( ifp );
        return;
        }
    else
        {
        HandleErrorL( aOutParamList, KErrNotSupported );
        }
    }

// ---------------------------------------------------------------------------
// 
// ---------------------------------------------------------------------------
//
void CBSServiceProvider::HandleErrorL( CLiwGenericParamList& aOutParamList,
    const TInt aError )
    {
    aOutParamList.AppendL( TLiwGenericParam( EGenericParamError,
        TLiwVariant( aError ) ) );
    }
