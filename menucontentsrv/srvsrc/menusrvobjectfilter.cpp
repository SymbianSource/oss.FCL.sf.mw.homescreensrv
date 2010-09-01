/*
* Copyright (c) 2009 Nokia Corporation and/or its subsidiary(-ies).
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
#include "menusrvobjectfilter.h"
#include "mcsmenufilter.h"
#include "menuengobject.h"

// ================= MEMBER FUNCTIONS =======================

// ---------------------------------------------------------
// TMenuSrvObjectFilter::MatchesObject()
// ---------------------------------------------------------
//
TBool TMenuSrvObjectFilter::MatchesObject( const CMenuEngObject& aObject ) const
    {
    if ( ( iFilter.FlagsOn() & aObject.Flags() ) != iFilter.FlagsOn() )
        {
        return EFalse;
        }
    if ( ( iFilter.FlagsOff() & ~aObject.Flags() ) != iFilter.FlagsOff() )
        {
        return EFalse;
        }
    if ( iFilter.Type().Length() && iFilter.Type() != aObject.Type() )
        {
        return EFalse;
        }
    TBool needAttr;
    TBool haveAttr;
    TPtrC name;
    TPtrC needVal;
    TPtrC haveVal;
    TBool dummy;
    for ( TInt i = 0; i < iFilter.NumAttributes(); i++ )
        {
        needAttr = iFilter.GetAttribute( i, name, needVal );
        haveAttr = aObject.FindAttribute( name, haveVal, dummy );
        if( needAttr != haveAttr )
            {
            return EFalse;
            }
        if ( haveAttr )
            {
            __ASSERT_DEBUG( needAttr, User::Invariant() ); // Checked above.
            //we want co compare uid case onsensitive
            if( !name.Compare( KMenuAttrUid() ) )
                {
                if ( needVal.CompareF( haveVal ) )
                    {
                    return EFalse;
                    }
                }
            else
                {
                if ( needVal.Compare( haveVal ) )
                    {
                    return EFalse;
                    }
                }
            }
        }
    return ETrue;
    }

//  End of File  
