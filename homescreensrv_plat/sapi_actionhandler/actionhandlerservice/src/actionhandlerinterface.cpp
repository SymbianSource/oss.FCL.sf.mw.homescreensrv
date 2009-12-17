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


#include "ahplugin.h" 
#include "cpluginvalidator.h"
#include "actionhandlerinterface.h"
#include "ahplugindefs.h"
#include "serviceerrno.h"

using namespace LIW;

// ---------------------------------------------------------------------------
// 
// ---------------------------------------------------------------------------
//
CActionHandlerInterface::CActionHandlerInterface()
    {

    }

// ---------------------------------------------------------------------------
// 
// ---------------------------------------------------------------------------
//
void CActionHandlerInterface::ConstructL()
    {
    iPluginManager = CPluginValidator::NewL( 
                                          TUid::Uid( KAHPluginInterfaceUid ) );
    }

// ---------------------------------------------------------------------------
//
// ---------------------------------------------------------------------------
//
CActionHandlerInterface* CActionHandlerInterface::NewL()
    {
    CActionHandlerInterface* self = CActionHandlerInterface::NewLC( );
    CleanupStack::Pop( self );
    return self;
    }

// ---------------------------------------------------------------------------
//
// ---------------------------------------------------------------------------
//
CActionHandlerInterface* CActionHandlerInterface::NewLC()
    {
    CActionHandlerInterface* self = new( ELeave ) CActionHandlerInterface;
    CleanupStack::PushL( self );
    self->ConstructL( );
    return self;
    }

// ---------------------------------------------------------------------------
// 
// ---------------------------------------------------------------------------
//
CActionHandlerInterface::~CActionHandlerInterface()
    {
    delete iPluginManager;
    }

// ---------------------------------------------------------------------------
//  Executes the SAPI as per params
// ---------------------------------------------------------------------------
//
void CActionHandlerInterface::ExecuteCmdL( const TDesC8& aCmdName,
    const CLiwGenericParamList& aInParamList,
    CLiwGenericParamList& aOutParamList, TUint aCmdOptions,
    MLiwNotifyCallback* aCallback )
    {
    TInt errCode(KErrNotSupported);
    aOutParamList.AppendL( TLiwGenericParam( EGenericParamError,
        TLiwVariant( ErrCodeConversion( KErrNone ) ) ) );
    // Check the command name;Cmd name should be Execute
    if ( !aCmdName.Compare( KExecute ) && !aCallback && !aCmdOptions )
        {
        TUid pluginUid= TUid::Null( );
        CLiwDefaultMap* map = CLiwDefaultMap::NewLC( );
        //extract command params
        if ( !ExtractUid( aInParamList, pluginUid ) &&!ExtractMap(
            aInParamList, map ) )
            {
            TRAP(errCode, ExecuteActionL( pluginUid, map ));
            }
        else
            {
            errCode = KErrNotSupported;
            }
        CleanupStack::PopAndDestroy( map );
        }
    if ( errCode != KErrNone )
        {
        aOutParamList.Reset( );
        aOutParamList.AppendL( TLiwGenericParam( EGenericParamError,
            TLiwVariant( ErrCodeConversion( errCode ) ) ) );
        }
    }

// ---------------------------------------------------------------------------
// Closes the interface
// ---------------------------------------------------------------------------
//
void CActionHandlerInterface::Close()
    {
    delete this;
    }

// ---------------------------------------------------------------------------
// 
// ---------------------------------------------------------------------------
//
TInt CActionHandlerInterface::ExtractUid(
    const CLiwGenericParamList& aInParamList, TUid& aUid )

    {
    TInt errCode(KErrArgument);
    const TLiwGenericParam* param= NULL;
    TInt pos( 0);
    param = aInParamList.FindFirst( pos, KPluginUid );
    if ( param && pos !=KErrNotFound )
        {
        //get plugin uid
        if ( param->Value().Get( aUid ) )
            {
            errCode = KErrNone;
            }
        else
            {
            TUint plugin( 0 );
            if ( param->Value().Get( plugin ) )
                {
                aUid = TUid::Uid( plugin );
                errCode = KErrNone;
                }
            }
        }
    return errCode;
    }

// ---------------------------------------------------------------------------
// Extract Map
// ---------------------------------------------------------------------------
//
TInt CActionHandlerInterface::ExtractMap(
    const CLiwGenericParamList& aInParamList, CLiwMap* aMap )
    {
    TInt errCode(KErrArgument);
    const TLiwGenericParam* param= NULL;
    TInt pos( 0);
    param = aInParamList.FindFirst( pos, KMap );
    if ( param && pos !=KErrNotFound )
        {
        //get action type
        if ( param->Value().Get( *aMap ) )
            {
            errCode = KErrNone;
            }
        }
    return errCode;
    }

// ---------------------------------------------------------------------------
// ErrCode Conversion
// ---------------------------------------------------------------------------
//
TInt32 CActionHandlerInterface::ErrCodeConversion( TInt code )
    {
    TInt32 err;
    switch ( code )
        {
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

        default:
            err = SErrGeneralError;
            break;
        }

    return err;

    }

// ---------------------------------------------------------------------------
// CActionHandlerInterface::ExecuteActionL
// Gets required plugin  and triggers action execution
// ---------------------------------------------------------------------------
//
void CActionHandlerInterface::ExecuteActionL( const TUid aUid,
    const CLiwMap* aMap )
    {
    __ASSERT_DEBUG( aMap , User::Panic( _L("actionhandlerinterface"), 0 ) );
    CAHPlugin* pluginInstance = 
    static_cast<CAHPlugin*>( iPluginManager->GetImplementation( aUid ) );
    
    if ( pluginInstance )
        {
        User::LeaveIfError( pluginInstance->ExecuteActionL( aMap ) );
        }
    else
        {
        User::Leave( KErrNotFound );
        }
    }

