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
* Description:  Back Stepping Service Interface
 *
*/


#include "bsserviceinterface.h"
#include "bsserviceconstants.h"

using namespace LIW;

// ======== MEMBER FUNCTIONS ========

// ---------------------------------------------------------------------------
// 
// ---------------------------------------------------------------------------
//
CBSServiceInterface::CBSServiceInterface()
    {
    }

// ---------------------------------------------------------------------------
// 
// ---------------------------------------------------------------------------
//
void CBSServiceInterface::ConstructL()
    {
    User::LeaveIfError( iBSClient.Connect( ) );
    }

// ---------------------------------------------------------------------------
//
// ---------------------------------------------------------------------------
//
CBSServiceInterface* CBSServiceInterface::NewLC()
    {
    CBSServiceInterface* self = new( ELeave ) CBSServiceInterface;
    CleanupStack::PushL( self );
    self->ConstructL( );
    return self;
    }

// ---------------------------------------------------------------------------
// 
// ---------------------------------------------------------------------------
//
CBSServiceInterface::~CBSServiceInterface()
    {
    iBSClient.Close( );
    }

// ---------------------------------------------------------------------------
//  Executes the SAPI as per params
// ---------------------------------------------------------------------------
//
void CBSServiceInterface::ExecuteCmdL( const TDesC8& aCmdName,
    const CLiwGenericParamList& aInParamList,
    CLiwGenericParamList& aOutParamList, TUint aCmdOptions,
    MLiwNotifyCallback* aCallback )
    {
    if ( !aCallback && !aCmdOptions )
        {
        if ( !aCmdName.Compare( KBSCmdInitialize ) )
            {
            InitializeL( aInParamList, aOutParamList );
            }
        else if ( !aCmdName.Compare( KBSCmdForwardActivationEvent ) )
            {
            ForwardActivationEventL( aInParamList, aOutParamList );
            }
        else if ( !aCmdName.Compare( KBSCmdHandleBackCommand ) )
            {
            HandleBackCommandL( aInParamList, aOutParamList );
            }
        else
            {
            HandleErrorL( aOutParamList, KErrNotSupported );
            }
        }
    else
        {
        HandleErrorL( aOutParamList, KErrNotSupported );
        }
    }

// ---------------------------------------------------------------------------
// Closes the interface
// ---------------------------------------------------------------------------
//
void CBSServiceInterface::Close()
    {
    delete this;
    }

// ---------------------------------------------------------------------------
// 
// ---------------------------------------------------------------------------
//
void CBSServiceInterface::InitializeL(
    const CLiwGenericParamList& aInParamList,
    CLiwGenericParamList& aOutParamList )
    {
    TInt32 appUid;
    TInt err = GetParam( aInParamList, KBSInParamAppUid, appUid );
    if ( err == KErrNone )
        {
        TInt retVal = iBSClient.Initialize( TUid::Uid( appUid ) );
        HandleResultL( aOutParamList, retVal );
        return;
        }

    HandleErrorL( aOutParamList, err );
    }

// ---------------------------------------------------------------------------
// 
// ---------------------------------------------------------------------------
//
void CBSServiceInterface::ForwardActivationEventL(
    const CLiwGenericParamList& aInParamList,
    CLiwGenericParamList& aOutParamList )
    {
    TPtrC8 state;
    TInt err = GetParam( aInParamList, KBSInParamState, state );
    if ( err == KErrNone )
        {
        TBool enter;
        err = GetParam( aInParamList, KBSInParamEnter, enter );
        if ( err == KErrNone )
            {
            TInt retVal = iBSClient.ForwardActivationEvent( state, enter );
            HandleResultL( aOutParamList, retVal );
            return;
            }
        }

    HandleErrorL( aOutParamList, err );
    }

// ---------------------------------------------------------------------------
// 
// ---------------------------------------------------------------------------
//
void CBSServiceInterface::HandleBackCommandL(
    const CLiwGenericParamList& aInParamList,
    CLiwGenericParamList& aOutParamList )
    {
    TPtrC8 state;
    TInt err = GetParam( aInParamList, KBSInParamState, state );

    TBool checkOnly(EFalse);
    if ( GetParam( aInParamList, KBSInParamCheckOnly, checkOnly ) != KErrNone )
        {
        // use default value if parameter isn't correct
        checkOnly = EFalse;
        }

    if ( err == KErrNone )
        {
        TInt retVal = iBSClient.HandleBackCommand( state, checkOnly );
        HandleResultL( aOutParamList, retVal );
        return;
        }

    HandleErrorL( aOutParamList, err );
    }

// ---------------------------------------------------------------------------
// 
// ---------------------------------------------------------------------------
//
void CBSServiceInterface::HandleErrorL( CLiwGenericParamList& aOutParamList,
    const TInt aError )
    {
    aOutParamList.AppendL( TLiwGenericParam( EGenericParamError,
        TLiwVariant( aError ) ) );
    }

// ---------------------------------------------------------------------------
// 
// ---------------------------------------------------------------------------
//
void CBSServiceInterface::HandleResultL( CLiwGenericParamList& aOutParamList,
    const TInt aResult )
    {
    TLiwGenericParam statusInfo;
    statusInfo.SetNameAndValueL( KBSOutParamStatusInfo, TLiwVariant( aResult ) );
    aOutParamList.AppendL( statusInfo );
    statusInfo.Reset( );
    }

// ---------------------------------------------------------------------------
// 
// ---------------------------------------------------------------------------
//
TInt CBSServiceInterface::GetParam( const CLiwGenericParamList& aInParamList,
    const TDesC8& aParamName, TPtrC8& aPtr ) const
    {
    TInt errCode(KErrArgument);

    const TLiwGenericParam* param= NULL;
    TInt pos( 0);
    param = aInParamList.FindFirst( pos, aParamName );
    if ( param && (pos != KErrNotFound ) )
        {
        if ( param->Value().Get( aPtr ) )
            {
            errCode = KErrNone;
            }
        }

    return errCode;
    }

// ---------------------------------------------------------------------------
// 
// ---------------------------------------------------------------------------
//
TInt CBSServiceInterface::GetParam( const CLiwGenericParamList& aInParamList,
    const TDesC8& aParamName, TInt32& aNumber ) const
    {
    TInt errCode(KErrArgument);

    const TLiwGenericParam* param= NULL;
    TInt pos( 0);
    param = aInParamList.FindFirst( pos, aParamName );
    if ( param && (pos != KErrNotFound ) )
        {
        if ( param->Value().Get( aNumber ) )
            {
            errCode = KErrNone;
            }
        }

    return errCode;
    }

// ---------------------------------------------------------------------------
// 
// ---------------------------------------------------------------------------
//
TInt CBSServiceInterface::GetParam( const CLiwGenericParamList& aInParamList,
    const TDesC8& aParamName, TBool& aBool ) const
    {
    TInt errCode(KErrArgument);

    const TLiwGenericParam* param= NULL;
    TInt pos( 0);
    param = aInParamList.FindFirst( pos, aParamName );
    if ( param && (pos != KErrNotFound ) )
        {
        if ( param->Value().Get( aBool ) )
            {
            errCode = KErrNone;
            }
        }

    return errCode;
    }
