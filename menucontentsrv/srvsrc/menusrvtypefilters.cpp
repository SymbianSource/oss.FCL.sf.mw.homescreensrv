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
#include"menusrvtypefilters.h"
#include "mcsdef.h"
#include "mcsmenuitem.h"

// ---------------------------------------------------------
// TMenuSrvTypeFilter::MatchesObject
// ---------------------------------------------------------
//
TBool TMenuSrvTypeFilter::MatchesObject( const CMenuEngObject& aObject ) const
    {
    return iType == aObject.Type();
    }

// ==================== MEMBER FUNCTIONS ====================

// ---------------------------------------------------------
// TMenuSrvTypeAttrFilter::MatchesObject
// ---------------------------------------------------------
//
TBool TMenuSrvTypeAttrFilter::MatchesObject(
        const CMenuEngObject& aObject ) const
    {
    if ( TMenuSrvTypeFilter::MatchesObject( aObject ) )
        {
        TPtrC val;
        TBool dummy;
        if( aObject.FindAttribute( iAttrName, val, dummy ) )
            {
            if( !val.CompareF( iAttrValue ) )
                {
                return ETrue; // Match.
                }
            }
        }
    return EFalse;
    }

// ---------------------------------------------------------
// TMenuSrvAttrFilter::MatchesObject
// ---------------------------------------------------------
//
TBool TMenuSrvAttrFilter::MatchesObject( const CMenuEngObject& aObject ) const
    {
    TPtrC val;
    TBool dummy;
    if( aObject.FindAttribute( iAttrName, val, dummy ) )
        {
        if( !val.CompareF( iAttrValue ) )
            {
            return ETrue; // Match.
            }
        }
    return EFalse;
    }

// ==================== MEMBER FUNCTIONS ====================

// ---------------------------------------------------------
// TMenuSrvTypeAttrFilter::MatchesObject
// ---------------------------------------------------------
//
TBool TMenuSrvTypeAttrExistsFilter::MatchesObject(
        const CMenuEngObject& aObject ) const
    {
    if ( TMenuSrvTypeFilter::MatchesObject( aObject ) )
        {
        TPtrC val;
        TBool dummy;
        if( aObject.FindAttribute( iAttrName, val, dummy ) )
            {
            return ETrue; // Match.
            }
        }
    return EFalse;
    }

// ---------------------------------------------------------
// TMenuSrvTypeAttrFilter::MatchesObject
// ---------------------------------------------------------
//
TBool TMenuSrvAttrExistsFilter::MatchesObject(
        const CMenuEngObject& aObject ) const
    {
    TPtrC val;
    TBool dummy;
    if( aObject.FindAttribute( iAttrName, val, dummy ) )
        {
        return ETrue; // Match.
        }
    return EFalse;
    }

// ---------------------------------------------------------
// TMenuSrvHiddenAppFilter::MatchesObject
// ---------------------------------------------------------
//
TBool TMenuSrvHiddenAppFilter::MatchesObject(
        const CMenuEngObject& aObject ) const
    {
    if (KMenuTypeApp() == aObject.Type())
        {
        if( aObject.Flags() & TMenuItem::EHidden )
            {
            return ETrue; // Match.
            }
        }
    return EFalse;
    }
