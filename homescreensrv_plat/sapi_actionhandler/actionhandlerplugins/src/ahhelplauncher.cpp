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
* Description:  App for Action handler
 *
*/


#include <eikenv.h>
#include <HLPLCH.H> // help launcher
#include <LiwVariant.h>

#include "ahproperties.hrh"
#include "ahhelplauncher.h"

// ----------------------------------------------------------------------------
// Symbian 2nd phase constructor can leave.
// ----------------------------------------------------------------------------
//
void CAHHelpLauncher::ConstructL()
    {
    }

// ----------------------------------------------------------------------------
// Two-phased constructor.
// ----------------------------------------------------------------------------
//
CAHHelpLauncher* CAHHelpLauncher::NewL()
    {
    CAHHelpLauncher* self = CAHHelpLauncher::NewLC( );
    CleanupStack::Pop( self );
    return self;
    }

// ----------------------------------------------------------------------------
// Two-phased constructor.
// ----------------------------------------------------------------------------
//
CAHHelpLauncher* CAHHelpLauncher::NewLC()
    {
    CAHHelpLauncher* self = new( ELeave ) CAHHelpLauncher;
    CleanupStack::PushL( self );
    self->ConstructL( );
    return self;
    }

// ---------------------------------------------------------------------------
// destructor
// ---------------------------------------------------------------------------
CAHHelpLauncher::~CAHHelpLauncher()
    {

    }

TInt CAHHelpLauncher::ExecuteActionL( const CLiwMap* aMap )
    {
    TInt errCode(KErrArgument);
    TUid appUid= TUid::Null( );
    
    RBuf helpString;
    CleanupClosePushL( helpString );
    
    if ( !ExtractUidL( aMap, appUid, KHelpUid ) 
        && !ExtractDesL( aMap, helpString, KHelpTopic ) )
        {
        CArrayFixFlat<TCoeHelpContext>* array = 
            new (ELeave) CArrayFixFlat<TCoeHelpContext>(1);
        CleanupStack::PushL( array );
        array->AppendL( TCoeHelpContext( appUid, helpString ) );
        CleanupStack::Pop( array );
        HlpLauncher::LaunchHelpApplicationL( CCoeEnv::Static()->WsSession( ), array );
        errCode = KErrNone;
        }
    CleanupStack::PopAndDestroy( &helpString );    
        
    return errCode;
    }

// ---------------------------------------------------------------------------
// 
// ---------------------------------------------------------------------------
//
TInt CAHHelpLauncher::ExtractDesL( const CLiwMap* aMap,
    RBuf& aString, const TDesC8& aMapName )
    {
    TInt errCode(KErrNotFound);
    TLiwVariant variant;
    variant.PushL( );
    TPtrC tempString( KNullDesC );
    if ( aMap->FindL( aMapName, variant ) )
        {
        variant.Get( tempString );
        aString.ReAllocL( tempString.Length( ) );
        aString.Append( tempString );
        errCode = KErrNone;
        }
    CleanupStack::PopAndDestroy( &variant );    
    return errCode;
    }

// ---------------------------------------------------------------------------
// 
// ---------------------------------------------------------------------------
//
TInt CAHHelpLauncher::ExtractUidL( const CLiwMap* aMap, TUid& aUid,
    const TDesC8& aMapName )
    {
    TInt errCode(KErrNotFound);
    TInt32 temp( 0);
    TLiwVariant variant;
    variant.PushL( );
    if ( aMap->FindL( aMapName, variant ) )
        {
        variant.Get( temp );
        aUid = TUid::Uid( temp );
        errCode = KErrNone;
        }
    CleanupStack::PopAndDestroy( &variant );
    return errCode;
    }
// End of file   
