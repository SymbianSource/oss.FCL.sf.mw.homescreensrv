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

#include <S32MEM.H>

#include "cpclientiterable.h"
#include "cpglobals.h"
#include "cpdebug.h"

using namespace LIW;

// ---------------------------------------------------------------------------
// Two-Phase constructor
// ---------------------------------------------------------------------------
//
CCPClientIterable* CCPClientIterable::NewL( CLiwGenericParamList* aList )
    {
    return new(ELeave) CCPClientIterable( aList );
    }

// ---------------------------------------------------------------------------
// Constructor
// ---------------------------------------------------------------------------
//
CCPClientIterable::CCPClientIterable( CLiwGenericParamList* aList ) :
    iList(aList)
    {
    }

// ---------------------------------------------------------------------------
// Destructor
// ---------------------------------------------------------------------------
//
CCPClientIterable::~CCPClientIterable()
    {
    delete iList;
    }

// ---------------------------------------------------------------------------
// Reset the list
// ---------------------------------------------------------------------------
//
void CCPClientIterable::Reset()
    {
    iPos = 0;
    }

// ---------------------------------------------------------------------------
// Gets next element in list
// ---------------------------------------------------------------------------
//
TBool CCPClientIterable::NextL( TLiwVariant& aNext )
    {
    CP_DEBUG( _L8("CCPClientIterable::NextL()") );
    TBool retValue = EFalse;
    const TLiwGenericParam* param= NULL;
    param = iList->FindFirst( iPos, KListMap, EVariantTypeMap );

    if ( param && iPos !=KErrNotFound )
        {
        CLiwDefaultMap* map = CLiwDefaultMap::NewLC( );
        //get map
        if ( param->Value().Get( *map ) )
            {
            ExctractBinariesL( *map );
            aNext.SetL( map );
            retValue = ETrue;
            }
        CleanupStack::PopAndDestroy( map );
        iPos++;
        }
    return retValue;
    }

// ---------------------------------------------------------------------------
// 
// ---------------------------------------------------------------------------
//
void CCPClientIterable::ExctractBinariesL( CLiwDefaultMap& aMap )
    {
    CP_DEBUG( _L8("CCPClientIterable::ExctractBinariesL()") );
    TLiwVariant buffer;
    buffer.PushL( );
    if ( aMap.FindL( KDataMap, buffer ) )
        {
        TPtrC8 packedData( KNullDesC8 );
        if ( buffer.Get( packedData ) )
            {
            RDesReadStream str(packedData);
            CleanupClosePushL( str );
            CLiwDefaultMap* datamap = CLiwDefaultMap::NewLC( str );
            aMap.Remove( KDataMap );
            aMap.InsertL( KDataMap, datamap );
            CleanupStack::PopAndDestroy( datamap );
            CleanupStack::PopAndDestroy( &str );
            }
        }
    if ( aMap.FindL( KActionMap, buffer ) )
        {
        TPtrC8 packedAction( KNullDesC8 );
        if ( buffer.Get( packedAction ) )
            {
            RDesReadStream str(packedAction);
            CleanupClosePushL( str );
            CLiwDefaultMap* actionmap = CLiwDefaultMap::NewLC( str );
            aMap.Remove( KActionMap );
            aMap.InsertL( KActionMap, actionmap );
            CleanupStack::PopAndDestroy( actionmap );
            CleanupStack::PopAndDestroy( &str );
            }
        }
    CleanupStack::PopAndDestroy( &buffer );
    }
