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


#include <ecom/implementationproxy.h>
#include <liwcommon.h>
#include <liwserviceifbase.h>

#include "ahplugindefs.h"
#include "actionhandlerservice.h"
#include "serviceerrno.h"

#include "actionhandlerinterface.h"

const TInt implUid( 0x2000E539);

using namespace LIW;

// ---------------------------------------------------------------------------
// 
// ---------------------------------------------------------------------------
//
CActionHandlerService::CActionHandlerService()
    {

    }

// ---------------------------------------------------------------------------
// 
// ---------------------------------------------------------------------------
//
CActionHandlerService* CActionHandlerService::NewL()
    {
    return new(ELeave) CActionHandlerService();
    }

// ---------------------------------------------------------------------------
// 
// ---------------------------------------------------------------------------
//
CActionHandlerService::~CActionHandlerService()
    {

    }

// ---------------------------------------------------------------------------
// 
// ---------------------------------------------------------------------------
//
void CActionHandlerService::InitialiseL(
    MLiwNotifyCallback& /*aFrameworkCallback*/, const RCriteriaArray& /*aInterest*/)
    {

    }

// ---------------------------------------------------------------------------
// 
// ---------------------------------------------------------------------------
//
void CActionHandlerService::HandleServiceCmdL( const TInt& aCmdId,
    const CLiwGenericParamList& aInParamList,
    CLiwGenericParamList& aOutParamList, TUint aCmdOptions,
    const MLiwNotifyCallback* aCallback )

    {
    TInt errCode( KErrNotSupported );
    // Check the command name;Cmd name should be Execute
    if ( aCmdId == KLiwCmdAsStr && !aCallback && !aCmdOptions )
        {
        TInt pos = 0;
        TPtrC8 cmdName;
        const TLiwGenericParam* cmd= NULL;
        cmd = aInParamList.FindFirst( pos, KCommand );
        if ( cmd && pos != KErrNotFound )
            {
            cmdName.Set( cmd->Value().AsData( ) );
            if ( !cmdName.CompareF( KActionHandlerInterface ) )
                {
                //Create interface pointer and return the output param
                CActionHandlerInterface* ifp =
                        CActionHandlerInterface::NewLC( );
                aOutParamList.AppendL( TLiwGenericParam(
                    KActionHandlerInterface, TLiwVariant( ifp ) ) );
                CleanupStack::Pop( ifp );
                errCode = KErrNone;
                }
            }
        else
            {
            errCode = KErrArgument;
            }
        }
    if ( errCode != KErrNone )
        {
        aOutParamList.AppendL( TLiwGenericParam( EGenericParamError,
            TLiwVariant( ErrCodeConversion( errCode ) ) ) );
        }
    }

// ---------------------------------------------------------------------------
// ErrCode Conversion
// ---------------------------------------------------------------------------
//
TInt32 CActionHandlerService::ErrCodeConversion( TInt code )
    {
    TInt32 err;
    switch ( code )
        {

        case KErrNotFound:
            err= SErrNotFound;
            break;

        case KErrNoMemory:
            err = SErrNoMemory;
            break;

        case KErrNotSupported:
            err = SErrServiceNotSupported;
            break;

        case KErrArgument:
            err = SErrInvalidServiceArgument;
            break;

        default:
            err = SErrGeneralError;
            break;
        }

    return err;

    }

// ---------------------------------------------------------
// Map the interface UIDs to implementation factory functions
// ---------------------------------------------------------
//
const TImplementationProxy ImplementationTable[] =
    {
    IMPLEMENTATION_PROXY_ENTRY( implUid,CActionHandlerService::NewL)
    };

// ---------------------------------------------------------
// Exported proxy for instantiation method resolution
// ---------------------------------------------------------
//
EXPORT_C const TImplementationProxy* ImplementationGroupProxy(TInt& aTableCount)
    {
    aTableCount = sizeof(ImplementationTable) / sizeof(TImplementationProxy);
    return ImplementationTable;
    }
