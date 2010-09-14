/*
* Copyright (c) 2002-2005 Nokia Corporation and/or its subsidiary(-ies).
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
* Description:  Used for receive SIM Application name, icon or
*                visibility information.
*
*/



// INCLUDE FILES

#include "mcscachehandler.h"
#include "menueng.h"
#include "menuengobject.h"
#include "menusrvengutils.h"
#include "mcsmenunotifier.h"


// ============================ MEMBER FUNCTIONS ===============================
// -----------------------------------------------------------------------------
// CMcsCacheHandler::NewL
// Two-phased constructor.
// -----------------------------------------------------------------------------
//
EXPORT_C CMcsCacheHandler* CMcsCacheHandler::NewL(
    CMenuEng& aEng,
    CMenuSrvEngUtils& aUtils )
    {
    CMcsCacheHandler* self = new( ELeave ) CMcsCacheHandler( aEng, aUtils );
    CleanupStack::PushL( self );
    self->ConstructL();
    CleanupStack::Pop( self );
    return self;
    }

// -----------------------------------------------------------------------------
// Destructor
// -----------------------------------------------------------------------------
CMcsCacheHandler::~CMcsCacheHandler()
    {
    iAttrCache.ResetAndDestroy();
    }

// -----------------------------------------------------------------------------
// CMcsCacheHandler::CMcsCacheHandler
// C++ default constructor can NOT contain any code, that
// might leave.
// -----------------------------------------------------------------------------
//
CMcsCacheHandler::CMcsCacheHandler(
    CMenuEng& aEng,
    CMenuSrvEngUtils& aUtils ): iEng( aEng ), iUtils( aUtils )
    {
    }

// -----------------------------------------------------------------------------
// CMcsDrmHandler::ConstructL
// Symbian 2nd phase constructor can leave.
// -----------------------------------------------------------------------------
//
void CMcsCacheHandler::ConstructL()
    {
    }

// -----------------------------------------------------------------------------
// -----------------------------------------------------------------------------
//
void CMcsCacheHandler::HandleRemoveId( TInt aId )
    {
    iAttrCache.RemoveId( aId );
    }

// -----------------------------------------------------------------------------
// -----------------------------------------------------------------------------
//
void CMcsCacheHandler::HandleRemoveAttribute( TInt aId, const TDesC& aAttrName )
	{
	iAttrCache.RemoveAttribute(aId, aAttrName);
	}

// -----------------------------------------------------------------------------
// -----------------------------------------------------------------------------
//
void CMcsCacheHandler::EngineEvents( TInt aEvents )
    {
    if( aEvents & RMenuNotifier::EItemsAddedRemoved )
        {
        RemoveNotExistItems();
        }
    if( aEvents & RMenuNotifier::EItemAttributeChanged )
        {
        TRAP_IGNORE( RemoveItemsWithChangedAttributesL() );
        }
    }

// ---------------------------------------------------------
// RMenuSrvAttrArray::RemoveId
// ---------------------------------------------------------
//
void CMcsCacheHandler::RemoveItemsWithChangedAttributesL()
    {
    for ( TInt i = 0; i < iAttrCache.Count(); i++ )
        {
        TBool dummy;
        TBool exist;

        TInt id = iAttrCache[i]->Id();
        TPtrC name = iAttrCache[i]->Name();
        TPtrC valueCache = iAttrCache[i]->Value();

        TPtrC valueXml(KNullDesC);

        exist = iEng.ObjectL(id).FindAttribute( name, valueXml, dummy );
        if( exist && valueXml != valueCache )
            {
            iAttrCache.RemoveAttribute( id, name );
            i--;
            }
        }
    }

// ---------------------------------------------------------
// RMenuSrvAttrArray::RemoveId
// ---------------------------------------------------------
//
void CMcsCacheHandler::RemoveNotExistItems()
    {
    TInt id( KErrNone );
    for ( TInt i = 0; i < iAttrCache.Count(); i++ )
        {
        if( id != iAttrCache[i]->Id() )
            {
            id = iAttrCache[i]->Id();
            if( !iEng.Exist( id ) )
                {
                iAttrCache.RemoveId( id );
                i--;
                }
            }
        }
    }

//  End of File
