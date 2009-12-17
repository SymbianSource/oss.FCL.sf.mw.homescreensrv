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

// INCLUDE FILES

#include <s32strm.h>
#include "menusrvattr.h"
//#include "menuengobject.h"

// ================= LOCAL FUNCTIONS =======================

/**
* Cleanup support method. Call ResetAndDestroy() on the array.
* @param RMenuItemArray* as TAny*
*/
LOCAL_C void ResetAndDestroy( TAny* aArray )
    {
    ((RMenuSrvAttrArray*)aArray)->ResetAndDestroy();
    }

// ================= MEMBER FUNCTIONS =======================

// ---------------------------------------------------------
// CMenuSrvAttr::~CMenuItemAttr
// ---------------------------------------------------------
//
CMenuSrvAttr::~CMenuSrvAttr()
    {
    iName.Close();
    iValue.Close();
    }

// ---------------------------------------------------------
// CMenuSrvAttr::NewL
// ---------------------------------------------------------
//
CMenuSrvAttr* CMenuSrvAttr::NewL( const TDesC& aName )
    {
    CMenuSrvAttr* attr = NewLC( aName );
    CleanupStack::Pop( attr );
    return attr;
    }

// ---------------------------------------------------------
// CMenuSrvAttr::NewLC
// ---------------------------------------------------------
//
CMenuSrvAttr* CMenuSrvAttr::NewLC( const TDesC& aName )
    {
    CMenuSrvAttr* attr = new (ELeave) CMenuSrvAttr();
    CleanupStack::PushL( attr );
    attr->ConstructL( aName );
    return attr;
    }

// ---------------------------------------------------------
// CMenuSrvAttr::ConstructL
// ---------------------------------------------------------
//
void CMenuSrvAttr::ConstructL( const TDesC& aName )
    {
    iName.CreateL( aName );
    }

// ---------------------------------------------------------
// CMenuSrvAttr::ConstructL
// ---------------------------------------------------------
//
void CMenuSrvAttr::SetValueL( const TDesC& aName )
    {
    if( iValue.Length() > 0 )
        {
        iValue.Close();
        }
    iValue.CreateL( aName );
    }

// ---------------------------------------------------------
// RMenuSrvAttrArray::Find
// ---------------------------------------------------------
//
TBool RMenuSrvAttrArray::Find( TInt aId, 
    const TDesC& aName, TDes& aAttrVal  )
    {
    for ( TInt i = 0; i < Count(); i++ )
        {
        if( aId == operator[]( i )->Id() &&
            aName.Compare( operator[]( i )->Name() ) == KErrNone )
            {
            aAttrVal = operator[]( i )->Value();
            return ETrue;
            }
        }
    return EFalse;
    }

// ---------------------------------------------------------
// RMenuSrvAttrArray::Find
// ---------------------------------------------------------
//
TBool RMenuSrvAttrArray::Exist( TInt aId, const TDesC& aName )
    {
    for ( TInt i = 0; i < Count(); i++ )
        {
        if( aId == operator[]( i )->Id() &&
            aName.Compare( operator[]( i )->Name() ) == KErrNone )
            {
            return ETrue;
            }
        }
    return EFalse;
    }

// ---------------------------------------------------------
// RMenuSrvAttrArray::RemoveId
// ---------------------------------------------------------
//
void RMenuSrvAttrArray::RemoveId( TInt aId )
    {
    for ( TInt i = 0; i < Count(); i++ )
        {
        if( aId == operator[]( i )->Id() )
            {
            delete operator[]( i );
            Remove( i );
            i--;
            }
        }
    }

// ---------------------------------------------------------
// RMenuSrvAttrArray::RemoveId
// ---------------------------------------------------------
//
void RMenuSrvAttrArray::RemoveAttribute( TInt aId, const TDesC& aAttrName )
	{
	for ( TInt i = 0; i < Count(); i++ )
		{
		if( aId == operator[]( i )->Id() && aAttrName == operator[]( i )->Name()) 
			{
			delete operator[]( i );
			Remove( i );
			break;
			}
		}
	}

// ---------------------------------------------------------
// RMenuSrvAttrArray::RemoveId
// ---------------------------------------------------------
//
/*void RMenuSrvAttrArray::RemoveNotExistItems()
    {
    TInt id( KErrNone );
    TInt err( KErrNone );
    for ( TInt i = 0; i < Count(); i++ )
       {
       if( id != operator[]( i )->Id() )
           {
           //CMenuEngObject& obj = NULL;
           //id = operator[]( i )->Id();
           TRAP( err, CMenuEngObject& obj = iEng->ObjectL(11));
           if( err == KErrNotFound )
               {
               RemoveId( id );
               i--;
               }
           }
       }
    }*/

// ---------------------------------------------------------
// CleanupResetAndDestroyPushL
// ---------------------------------------------------------
//
void CleanupResetAndDestroyPushL( RMenuSrvAttrArray& aArray )
    {
    CleanupStack::PushL( TCleanupItem( ResetAndDestroy, &aArray ) );
    }

//  End of File  
