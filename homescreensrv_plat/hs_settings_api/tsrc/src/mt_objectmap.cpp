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
* Description: EUnit test for homescreen settings API, object map tests
*
*/

// External includes
#include <digia/eunit/eunitmacros.h>

// Internal includes
#include "mt_objectmap.h"
#include "objectmap.h"

_LIT8(KPath,"\\path\\");
_LIT8(KMediaType,"mediaType");
_LIT8(KName, "Name");

// ======== MEMBER FUNCTIONS ========

//----------------------------------------------------------------------------
// MT_ObjectMap::NewL( )
//----------------------------------------------------------------------------
//
MT_ObjectMap* MT_ObjectMap::NewL()
    {
    MT_ObjectMap* self = MT_ObjectMap::NewLC();
    CleanupStack::Pop( self );
    return self;
    }

//----------------------------------------------------------------------------
// MT_ObjectMap::NewLC( )
//----------------------------------------------------------------------------
//
MT_ObjectMap* MT_ObjectMap::NewLC()
    {
    MT_ObjectMap* self = new( ELeave ) MT_ObjectMap();
    CleanupStack::PushL( self );
    self->ConstructL();
    return self;
    }

//----------------------------------------------------------------------------
// MT_ObjectMap::~MT_ObjectMap( )
//----------------------------------------------------------------------------
//
MT_ObjectMap::~MT_ObjectMap()
    {
    }

//----------------------------------------------------------------------------
// MT_ObjectMap::MT_ObjectMap( )
//----------------------------------------------------------------------------
//
MT_ObjectMap::MT_ObjectMap()
    {
    }

//----------------------------------------------------------------------------
// MT_ObjectMap::ConstructL( )
//----------------------------------------------------------------------------
//
void MT_ObjectMap::ConstructL()
    {
    // The ConstructL from the base class CEUnitTestSuiteClass must be called.
    // It generates the test case table.
    CEUnitTestSuiteClass::ConstructL();
    }

//----------------------------------------------------------------------------
// MT_ObjectMap::SetupL( )
//----------------------------------------------------------------------------
//
void MT_ObjectMap::SetupL(  )
    {
    iObjectMap = HSPluginSettingsIf::CObjectMap::NewL();
    }

//----------------------------------------------------------------------------
// MT_ObjectMap::Teardown( )
//----------------------------------------------------------------------------
//
void MT_ObjectMap::Teardown(  )
    {
    if ( iObjectMap )
        {
        delete iObjectMap;
        iObjectMap = NULL;
        }
    }

//----------------------------------------------------------------------------
// MT_ObjectMap::SetMediaTypeTestL( )
//----------------------------------------------------------------------------
//
void MT_ObjectMap::TestMediaTypeL(  )
    {
    EUNIT_ASSERT( iObjectMap->MediaType() == KNullDesC8 );
    
    iObjectMap->SetMediaTypeL( KMediaType );
    const TDesC8& mediaType = iObjectMap->MediaType();
    EUNIT_ASSERT( mediaType == KMediaType );
    }

//----------------------------------------------------------------------------
// MT_ObjectMap::SetNameTestL( )
//----------------------------------------------------------------------------
//
void MT_ObjectMap::TestNameL(  )
    {
    EUNIT_ASSERT( iObjectMap->NameL() == KNullDesC8 );
    
    iObjectMap->SetNameL( KName );
    const TDesC8& name = iObjectMap->NameL();
    EUNIT_ASSERT( name == KName );
    }

//----------------------------------------------------------------------------
// MT_ObjectMap::SetPathL( )
//----------------------------------------------------------------------------
//
void MT_ObjectMap::TestPathL(  )
    {
    EUNIT_ASSERT( iObjectMap->Path() == KNullDesC8 );
    
    iObjectMap->SetPathL( KPath );
    const TDesC8& mediaType = iObjectMap->Path();
    EUNIT_ASSERT( mediaType == KPath );
    }

//  EUnit test table

EUNIT_BEGIN_TEST_TABLE(
    MT_ObjectMap,
    "Test CObjectMap basic functionality",
    "MODULE")

EUNIT_TEST(
    "Test SetMediaTypeL, MediaType",
    "CObjectMap",
    "",
    "FUNCTIONALITY",
    SetupL, TestMediaTypeL, Teardown)

EUNIT_TEST(
    "Test SetNameL, Name",
    "CObjectMap",
    "",
    "FUNCTIONALITY",
    SetupL, TestNameL, Teardown)
    
EUNIT_TEST(
    "Test SetPathL, Path",
    "CObjectMap",
    "",
    "FUNCTIONALITY",
    SetupL, TestPathL, Teardown)

EUNIT_END_TEST_TABLE

//  END OF FILE
