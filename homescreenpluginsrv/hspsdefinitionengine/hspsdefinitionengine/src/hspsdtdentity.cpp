/*
* Copyright (c) 2005,2006 Nokia Corporation and/or its subsidiary(-ies).
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
* Description:  Entity reference data class 
*
*/



// INCLUDE FILES
#include    "hspsdtdentity.h"


// ============================ MEMBER FUNCTIONS ===============================

// -----------------------------------------------------------------------------
// ChspsDtdEntity::ChspsDtdEntity
// C++ default constructor can NOT contain any code, that
// might leave.
// -----------------------------------------------------------------------------
//
ChspsDtdEntity::ChspsDtdEntity()
    {
    }

// -----------------------------------------------------------------------------
// ChspsDtdEntity::ConstructL
// Symbian 2nd phase constructor can leave.
// -----------------------------------------------------------------------------
//
void ChspsDtdEntity::ConstructL()
    {
    }

// -----------------------------------------------------------------------------
// ChspsDtdEntity::NewL
// Two-phased constructor.
// -----------------------------------------------------------------------------
//
ChspsDtdEntity* ChspsDtdEntity::NewL()
    {
    ChspsDtdEntity* self = new( ELeave ) ChspsDtdEntity;
    
    CleanupStack::PushL( self );
    self->ConstructL();
    CleanupStack::Pop( self );

    return self;
    }

    
// Destructor
ChspsDtdEntity::~ChspsDtdEntity()
    {
    delete iEntity;
    delete iValue;
    }


// -----------------------------------------------------------------------------
// ChspsDtdEntity::SetEntityL
// (other items were commented in a header).
// -----------------------------------------------------------------------------
//
void ChspsDtdEntity::SetEntityL( const TDesC8& aEntity )
    {
    delete iEntity;
    iEntity = NULL;
    iEntity = aEntity.AllocL();
    }

// -----------------------------------------------------------------------------
// ChspsDtdEntity::Entity
// (other items were commented in a header).
// -----------------------------------------------------------------------------
//
const TDesC8& ChspsDtdEntity::Entity()
    {
    if ( iEntity )
        {
        return *iEntity;
        }
    else
        {
        return KNullDesC8;
        }    
    }
// -----------------------------------------------------------------------------
// ChspsDtdEntity::SetValueL
// (other items were commented in a header).
// -----------------------------------------------------------------------------
//
void ChspsDtdEntity::SetValueL( const TDesC8& aValue )
    {
    delete iValue;
    iValue = NULL;
    iValue = aValue.AllocL();
    }
// -----------------------------------------------------------------------------
// ChspsDtdEntity::Value
// (other items were commented in a header).
// -----------------------------------------------------------------------------
//
const TDesC8& ChspsDtdEntity::Value()
    {
    if ( iValue )
        {
        return *iValue;
        }
    else
        {
        return KNullDesC8;
        }    
    }

//  End of File  
