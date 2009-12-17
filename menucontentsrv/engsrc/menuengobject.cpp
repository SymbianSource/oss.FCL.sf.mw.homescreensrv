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
#include "mcsdef.h"
#include "menuengobject.h"
#include "mcsmenuitem.h"
#include "menueng.h"
#include "centralrepository.h"

// ================= LOCAL FUNCTIONS =======================

/**
* Check if this is a valid attribute name. Leave with KErrArgument if not.
* We don't accept whitespace or exotic characters anywhere, and also
* forbid setting reserved attribute "flags".
*/
LOCAL_C void CheckAttrNameL( const TDesC& aAttrName )
    {
    if ( !aAttrName.Length() )
        {
        User::Leave( KErrArgument );
        }
    if ( KMenuAttrFlags() == aAttrName || KMenuAttrId() == aAttrName )
        {
        // Internal attributes, disallow.
        User::Leave( KErrArgument );
        }
    for ( TInt i = 0; i < aAttrName.Length(); i++ )
        {
        TChar c =  aAttrName[i];
        if ( !c.IsAlphaDigit() &&
             '_' != c &&
             '-' != c &&
             ':' != c )
            {
            User::Leave( KErrArgument );
            }
        }
    }

// ================= MEMBER FUNCTIONS =======================

// ---------------------------------------------------------
// CMenuEngObject::~CMenuEngObject
// ---------------------------------------------------------
//
EXPORT_C CMenuEngObject::~CMenuEngObject()
    {
    iAttributes.ResetAndDestroy();
    iType.Close();
    }

// ---------------------------------------------------------
// CMenuEngObject::NewL
// ---------------------------------------------------------
//
CMenuEngObject* CMenuEngObject::NewL
( CMenuEng& aEng, const TDesC& aType )
    {
    CMenuEngObject* obj = new (ELeave) CMenuEngObject( aEng );
    CleanupStack::PushL( obj );
    obj->ConstructL( aType );
    CleanupStack::Pop( obj );
    return obj;
    }

// ---------------------------------------------------------
// CMenuEngObject::CMenuEngObject
// ---------------------------------------------------------
//
CMenuEngObject::CMenuEngObject( CMenuEng& aEng ): iEng( aEng ), iNative( ETrue )
    {
    }

// ---------------------------------------------------------
// CMenuEngObject::ConstructL
// ---------------------------------------------------------
//
void CMenuEngObject::ConstructL( const TDesC& aType )
    {
    CGECODefaultObject::BaseConstructL( aType );
    iType.CreateL( aType );
    }

// ---------------------------------------------------------
// CMenuEngObject::Id
// ---------------------------------------------------------
//
EXPORT_C TInt CMenuEngObject::Id() const
    {
    return iId;
    }

// ---------------------------------------------------------
// CMenuEngObject::Flags
// ---------------------------------------------------------
//
EXPORT_C TUint32 CMenuEngObject::Flags( TBool aFactoryCall ) const
    {
    TUint32 flags = iFlags;
    //if hidden flag is not set, we check it in central repository
    if( !aFactoryCall )
    	{
    	TRAP_IGNORE( UpdateCrHiddenFlagL( flags ) );
    	}
    return flags;
    }


// ---------------------------------------------------------
// CMenuEngObject::UpdateCrHiddenFlagL
// ---------------------------------------------------------
//
void CMenuEngObject::UpdateCrHiddenFlagL( TUint32& aFlags ) const
	{
    if ( iType == KMenuTypeFolder() && !( iFlags & TMenuItem::EHidden ) )
    	{
    	TBool localized;
    	TPtrC folderName(KNullDesC);
    	TBool exists = FindAttribute( KMenuAttrAppGroupName, folderName, localized );

    	if ( exists && IsInCrL( folderName ) )
    		{
			aFlags |= TMenuItem::EHidden;
    		}
    	}
	}

// ---------------------------------------------------------
// CMenuEngObject::ParseHiddenFoldersL
// ---------------------------------------------------------
//
void CMenuEngObject::ParseHiddenFoldersL( 
            const TDesC& aHiddenFolders, 
            RArray<TPtrC>& aHiddenFoldersArray ) const
    {
    TLex input( aHiddenFolders );
    TLexMark startMark;
    input.Mark( startMark ); 
    TBool notEmpty = EFalse;
    while ( !input.Eos() )
        {
        if( input.Peek() == ',')
            {
            User::LeaveIfError( aHiddenFoldersArray.
                            Append( input.MarkedToken( startMark )  ) );            
            input.Inc();
            input.Mark( startMark );
            }
        input.Inc();     
        notEmpty = ETrue;       
        }
	if ( notEmpty )
		{
		User::LeaveIfError( aHiddenFoldersArray.
						Append( input.MarkedToken( startMark )  ) );            
		}
   }

// ---------------------------------------------------------
// CMenuEngObject::IsInCrL
// ---------------------------------------------------------
//
TBool CMenuEngObject::IsInCrL( const TDesC& aFolderName ) const
    {
	RBuf hiddenCRFolders;
	CleanupClosePushL( hiddenCRFolders );
	hiddenCRFolders.CreateL( KCenRepBufferSize );
    
    CRepository* cenRepSession = CRepository::NewLC( KCRUidMenu );
    cenRepSession->Get( KMenuHideCPFolder, hiddenCRFolders );
	CleanupStack::PopAndDestroy( cenRepSession );
	
	RArray<TPtrC> hiddenFoldersArray;
    CleanupClosePushL( hiddenFoldersArray );
	ParseHiddenFoldersL( hiddenCRFolders, hiddenFoldersArray );
    
    TBool result(EFalse);
    TInt id = hiddenFoldersArray.Find(aFolderName);
    if (KErrNotFound != id )
        {
        TPtrC folderName(KNullDesC);
        folderName.Set( hiddenFoldersArray[id] );
        if (folderName == aFolderName)
            {
            result = ETrue;
            }
        }
    
	CleanupStack::PopAndDestroy( &hiddenFoldersArray );
	CleanupStack::PopAndDestroy( &hiddenCRFolders );

    return result;
    }

// ---------------------------------------------------------
// CMenuEngObject::Type
// ---------------------------------------------------------
//
EXPORT_C TPtrC CMenuEngObject::Type() const
    {
    return iType;
    }

// ---------------------------------------------------------
// CMenuEngObject::NumAttributes
// ---------------------------------------------------------
//
TInt CMenuEngObject::NumAttributes() const
    {
    return iAttributes.Count();
    }

// ---------------------------------------------------------
// CMenuEngObject::GetAttribute
// ---------------------------------------------------------
//
EXPORT_C TBool CMenuEngObject::GetAttribute
( TInt aIndex, TPtrC& aName, TPtrC& aValue, TBool& aLocalized ) const
    {
    if( aIndex < 0 || aIndex >= iAttributes.Count() )
        {
        return EFalse;
        }
    CMenuEngObjectAttr* attr = iAttributes[aIndex];
    aName.Set( attr->Name() );
    aValue.Set( attr->Value() );
    aLocalized = attr->Localized();
    return ETrue;
    }

// ---------------------------------------------------------
// CMenuEngObject::FindAttribute
// ---------------------------------------------------------
//
EXPORT_C TBool CMenuEngObject::FindAttribute
( const TDesC& aName, TPtrC& aValue, TBool& aLocalized ) const
    {
    TInt i = iAttributes.Find( aName );
    if ( KErrNotFound != i )
        {
        CMenuEngObjectAttr* attr = iAttributes[i];
        aValue.Set( attr->Value() );
        aLocalized = attr->Localized();
        return ETrue;
        }
    return EFalse;
    }

// ---------------------------------------------------------
// CMenuEngObject::SetFlags
// ---------------------------------------------------------
//
EXPORT_C void CMenuEngObject::SetFlags( TUint32 aMask, TBool aOn )
    {
    if ( aOn )
        {
        iFlags |= aMask;
        }
    else
        {
        iFlags &= ~aMask;
        }
    }

// ----------------------------------------------------------------------------
// CMenuEngObject::SetAttributeL
// Menu item lock flags are checked (e.g. can rename), but only when the 
// current SetAttributeL call is not made from the object factories.
// If it is called from there, no flags checking is made.
// This method is called from object factories when the engine is in 
// ELoadRamFile or ELoadRomFile states.
// ----------------------------------------------------------------------------
//
EXPORT_C void CMenuEngObject::SetAttributeL
( const TDesC& aAttrName, const TDesC& aAttrValue, TBool aLocalized )
    {
    SetInitialAttributeL( aAttrName, aAttrValue, aLocalized );
    CGECODefaultObject::RemoveAttribute( aAttrName );
    CGECODefaultObject::SetAttributeL( aAttrName, aAttrValue, aLocalized );
    }

void CMenuEngObject::SetInitialAttributeL
( const TDesC& aAttrName, const TDesC& aAttrValue, TBool aLocalized )
    {
    CheckAttrNameL( aAttrName );
    
    if ( iEng.State() != CMenuEng::ELoadRamFile && 
         iEng.State() != CMenuEng::ELoadRomFile &&
         Flags() & TMenuItem::ELockName && 
         ( 0 == aAttrName.Compare( KMenuAttrShortName ) || 
           0 == aAttrName.Compare( KMenuAttrLongName ) || 
           0 == aAttrName.Compare( KMenuAttrTitleName ) ) )
        {
        User::Leave( KErrAccessDenied );
        }

    CMenuEngObjectAttr* attr = NULL;
    TInt i = iAttributes.Find( aAttrName );
    if ( KErrNotFound == i )
        {
        attr = CMenuEngObjectAttr::NewLC( aAttrName );
        iAttributes.AppendL( attr );
        CleanupStack::Pop( attr );
        }
    else
        {
        attr = iAttributes[i];
        }
    __ASSERT_DEBUG( attr, User::Invariant() );
    attr->SetValueL( aAttrValue );
    attr->SetLocalized( aLocalized );
    }

// ---------------------------------------------------------
// CMenuEngObject::SetNative
// ---------------------------------------------------------
//
EXPORT_C void CMenuEngObject::SetNative( TBool aNative )
    {
    iNative = aNative;
    }

// ---------------------------------------------------------
// CMenuEngObject::GetNative
// ---------------------------------------------------------
//
EXPORT_C TBool CMenuEngObject::GetNative() const
    {
    return iNative;
    }

// ---------------------------------------------------------
// CMenuEngObject::RemoveAttribute
// ---------------------------------------------------------
//
EXPORT_C void CMenuEngObject::RemoveAttribute( const TDesC& aName )
    {
    TInt i = iAttributes.Find( aName );
    if ( KErrNotFound != i )
        {
        delete iAttributes[i];
        iAttributes.Remove( i );
        CGECODefaultObject::RemoveAttribute( aName );
        }
    }

// ---------------------------------------------------------
// CMenuEngObject::Reset
// ---------------------------------------------------------
//
EXPORT_C void CMenuEngObject::Reset()
    {
    iAttributes.ResetAndDestroy();
    iFlags = 0;
    }

// ---------------------------------------------------------
// CMenuEngObject::SetId
// ---------------------------------------------------------
//
void CMenuEngObject::SetId( TInt aId )
    {
    iId = aId;
    }

// ---------------------------------------------------------
// CMenuEngObject::Engine
// ---------------------------------------------------------
//
CMenuEng& CMenuEngObject::Engine()
    {
    return iEng;
    }

// ---------------------------------------------------------
// CMenuEngObject::TypeIdentifier
// ---------------------------------------------------------
//
const TDesC& CMenuEngObject::TypeIdentifier()
    {
    return iType;
    }

//  End of File  
