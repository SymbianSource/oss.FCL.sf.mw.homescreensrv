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


#include <mmf/common/mmfcontrollerpluginresolver.h>

#include "cdatasourceinterface.h"
#include "cpdebug.h"
#include "cpglobals.h"
#include "cpclient.h"
#include "serviceerrno.h"
#include "cpclientiterable.h"

using namespace LIW;

// ---------------------------------------------------------------------------
//
// ---------------------------------------------------------------------------
//
CDataSourceInterface* CDataSourceInterface::NewL()
    {
    CDataSourceInterface* self = CDataSourceInterface::NewLC( );
    CleanupStack::Pop( self );
    return self;
    }

// ---------------------------------------------------------------------------
//
// ---------------------------------------------------------------------------
//
CDataSourceInterface* CDataSourceInterface::NewLC()
    {
    CDataSourceInterface* self = new( ELeave ) CDataSourceInterface;
    CleanupStack::PushL( self );
    self->ConstructL( );
    return self;
    }

// ---------------------------------------------------------------------------
// 
// ---------------------------------------------------------------------------
//
CDataSourceInterface::~CDataSourceInterface()
    {
    CP_DEBUG( _L8("CDataSourceInterface::~CDataSourceInterface") );
    delete iCPClient;
    }

// ---------------------------------------------------------------------------
//  Executes the SAPI as per params
// ---------------------------------------------------------------------------
//
void CDataSourceInterface::ExecuteCmdL( const TDesC8& aCmdName,
    const CLiwGenericParamList& aInParamList,
    CLiwGenericParamList& aOutParamList, TUint aCmdOptions,
    MLiwNotifyCallback* aCallback )
    {
    CP_DEBUG( _L8("CCPClientInterface::ExecuteCmdL") );
    TInt errCode(KErrNone);
    aOutParamList.AppendL( TLiwGenericParam( EGenericParamError,
        TLiwVariant( ErrCodeConversion( KErrNone ) ) ) );
    aOutParamList.AppendL( TLiwGenericParam( KErrorCode,
        TLiwVariant( ErrCodeConversion( KErrNone ) ) ) );
    // Check the command name
    TRAP( errCode , ProcessCommandL( aCmdName, aInParamList,
                    aOutParamList,
                    aCmdOptions,
                    aCallback ) );
    if ( errCode != KErrNone )
        {
       aOutParamList.Reset( );
        aOutParamList.AppendL( TLiwGenericParam( EGenericParamError,
            TLiwVariant( ErrCodeConversion( errCode ) ) ) );
        aOutParamList.AppendL( TLiwGenericParam( KErrorCode,
            TLiwVariant( ErrCodeConversion( errCode ) ) ) );
        }
    }

// ---------------------------------------------------------------------------
// Closes the interface
// ---------------------------------------------------------------------------
//
void CDataSourceInterface::Close()
    {
    delete this;
    }

// ---------------------------------------------------------------------------
// 
// ---------------------------------------------------------------------------
//
CDataSourceInterface::CDataSourceInterface()
    {

    }

// ---------------------------------------------------------------------------
// 
// ---------------------------------------------------------------------------
//
void CDataSourceInterface::ConstructL()
    {
    iCPClient = CCPClient::NewL( );
    CP_DEBUG( _L8("CDataSourceInterface::ConstructL") );
    }

// ---------------------------------------------------------------------------
//  
// ---------------------------------------------------------------------------
//
void CDataSourceInterface::ProcessCommandL( const TDesC8& aCmdName,
    const CLiwGenericParamList& aInParamList,
    CLiwGenericParamList& aOutParamList, TUint aCmdOptions,
    MLiwNotifyCallback* aCallback )
    {
    CP_DEBUG( _L8("CCPClientInterface::ProcessCommandL") );
    TInt32 transactionId( -1 );
    if ( aCmdName.CompareF( KGetList ) == 0 )
        {
        CLiwGenericParamList* list = CLiwGenericParamList::NewL( );
        CleanupStack::PushL( list );
        iCPClient->GetListL( aInParamList, *list );
        CCPClientIterable* iterable = CCPClientIterable::NewL( list );
        CleanupStack::Pop( list );
        iterable->PushL( );
        aOutParamList.AppendL( TLiwGenericParam( KResults,
            TLiwVariant( iterable ) ) );
        CleanupStack::PopAndDestroy( iterable );
        }
    else if ( aCmdName.CompareF( KAdd ) == 0 )
        {
        iCPClient->AddL( aInParamList, aOutParamList );
        }
    else if ( aCmdName.CompareF( KDelete ) == 0 )
        {
        iCPClient->DeleteL( aInParamList );
        }
    else if ( aCmdName.CompareF( KRequestNotification ) == 0 )
        {
        if ( !(aCmdOptions & KLiwOptCancel) )
            {
            if ( !aCallback )
            	{
            	User::Leave( KErrPathNotFound );
            	}
            transactionId = aCallback->GetTransactionID();
            iCPClient->RegisterObserverL( aCallback, aInParamList, transactionId );
            }
        else
            {
            iCPClient->UnregisterObserversL( aInParamList );
            }
        }
    else if ( aCmdName.CompareF( KCmdCancel ) == 0 )
        {
        if ( aCmdOptions & KLiwOptCancel )
            {
            iCPClient->UnregisterObserversL( aInParamList );
            }
        }
    else
        {
        User::Leave( KErrNotSupported );
        }
    if ( transactionId != -1 )
        {
        aOutParamList.AppendL( TLiwGenericParam( KTransactionID,
            TLiwVariant( TInt32( transactionId ) ) ) );
        }
    }

// ---------------------------------------------------------------------------
// ErrCode Conversion
// ---------------------------------------------------------------------------
//
TInt32 CDataSourceInterface::ErrCodeConversion( TInt code )
    {
    TInt32 err;
    switch ( code )
        {
        case KErrCancel:
            // Returning KErrNone incase of KErrCancel
        case KErrNone:
            err= SErrNone;
            break;

        case KErrNotFound:
            err= SErrNotFound;
            break;

        case KErrNoMemory:
            err = SErrNoMemory;
            break;

        case KErrInUse:
            err = SErrServiceInUse;
            break;

        case KErrNotSupported:
            err = SErrServiceNotSupported;
            break;

        case KErrBadName:
            err = SErrBadArgumentType;
            break;

        case KErrArgument:
            err = SErrInvalidServiceArgument;
            break;

        case KErrPermissionDenied:
            err = SErrAccessDenied;
            break;
            
        case KErrPathNotFound:
            err = SErrMissingArgument;
            break;

        default:
            err = SErrGeneralError;
            break;
        }
    return err;
    }
