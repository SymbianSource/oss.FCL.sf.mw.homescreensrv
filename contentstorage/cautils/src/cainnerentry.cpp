/*
 * Copyright (c) 2008 Nokia Corporation and/or its subsidiary(-ies).
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
 * Description:  ?Description
 *
 */

#include <s32mem.h>
#include "cainnerentry.h"

// ======== MEMBER FUNCTIONS ========

// ---------------------------------------------------------------------------
//
// ---------------------------------------------------------------------------
//
CCaInnerEntry::~CCaInnerEntry()
    {
    iText.Close();
    iDescription.Close();
    iEntryTypeName.Close();
    iAttributes.ResetAndDestroy();
    }

// ---------------------------------------------------------------------------
//
// ---------------------------------------------------------------------------
//
EXPORT_C CCaInnerEntry* CCaInnerEntry::NewL()
    {
    CCaInnerEntry* entry = CCaInnerEntry::NewLC();
    CleanupStack::Pop( entry );
    return entry;
    }

// ---------------------------------------------------------------------------
//
// ---------------------------------------------------------------------------
//
EXPORT_C CCaInnerEntry* CCaInnerEntry::NewLC()
    {
    CCaInnerEntry* self = new ( ELeave ) CCaInnerEntry();
    CleanupStack::PushL( self );
    self->ConstructL();
    return self;
    }

// ---------------------------------------------------------------------------
//
// ---------------------------------------------------------------------------
//
EXPORT_C void CCaInnerEntry::ExternalizeL( RWriteStream& aStream ) const
    {
    aStream.WriteInt32L( iId );
    aStream.WriteInt32L( iUid );
    aStream.WriteInt32L( iIcon.iId );
    aStream.WriteUint32L( iIcon.iFileName.Length() );
    aStream.WriteL( iIcon.iFileName, iIcon.iFileName.Length() );
    aStream.WriteUint32L( iIcon.iSkinId.Length() );
    aStream.WriteL( iIcon.iSkinId, iIcon.iSkinId.Length() );
    aStream.WriteUint32L( iIcon.iApplicationId.Length() );
    aStream.WriteL( iIcon.iApplicationId, iIcon.iApplicationId.Length() );
    aStream.WriteUint32L( iFlags );
    aStream.WriteUint32L( iRole );
    aStream.WriteUint32L( iText.Length() );
    aStream.WriteL( iText, iText.Length() );
    aStream.WriteUint32L( iDescription.Length() );
    aStream.WriteL( iDescription, iDescription.Length() );
    aStream.WriteUint32L( iEntryTypeName.Length() );
    aStream.WriteL( iEntryTypeName, iEntryTypeName.Length() );
    iAttributes.ExternalizeL( aStream );
    aStream.CommitL();
    }

// ---------------------------------------------------------------------------
//
// ---------------------------------------------------------------------------
//
EXPORT_C void CCaInnerEntry::InternalizeL( RReadStream& aStream )
    {
    iId = aStream.ReadInt32L();
    iUid = aStream.ReadInt32L();
    iIcon.iId = aStream.ReadInt32L();
    TUint length = aStream.ReadUint32L();
    aStream.ReadL( iIcon.iFileName, length );
    length = aStream.ReadUint32L();
    aStream.ReadL( iIcon.iSkinId, length );
    length = aStream.ReadUint32L();
    aStream.ReadL( iIcon.iApplicationId, length );
    iFlags = aStream.ReadUint32L();
    iRole = aStream.ReadUint32L();
    length = aStream.ReadUint32L();
    iText.Close();
    iText.CreateL( length );
    aStream.ReadL( iText, length );
    length = aStream.ReadUint32L();
    iDescription.Close( );
    iDescription.CreateL( length );
    aStream.ReadL( iDescription, length);
    length = aStream.ReadUint32L( );
    iEntryTypeName.Close();
    iEntryTypeName.CreateL( length );
    aStream.ReadL( iEntryTypeName, length );
    iAttributes.InternalizeL( aStream );
    }

// ---------------------------------------------------------------------------
//
// ---------------------------------------------------------------------------
//
void CCaInnerEntry::ConstructL()
    {

    }

// ---------------------------------------------------------------------------
//
// ---------------------------------------------------------------------------
//
CCaInnerEntry::CCaInnerEntry()
    {

    }

// ---------------------------------------------------------------------------
//
// ---------------------------------------------------------------------------
//
EXPORT_C TInt CCaInnerEntry::GetId() const
    {
    return iId;
    }

// ---------------------------------------------------------------------------
//
// ---------------------------------------------------------------------------
//
EXPORT_C const RBuf& CCaInnerEntry::GetText() const
    {
    return iText;
    }

// ---------------------------------------------------------------------------
//
// ---------------------------------------------------------------------------
//
EXPORT_C const RBuf& CCaInnerEntry::GetDescription() const
    {
    return iDescription;
    }

// ---------------------------------------------------------------------------
//
// ---------------------------------------------------------------------------
//
EXPORT_C const RBuf& CCaInnerEntry::GetEntryTypeName() const
    {
    return iEntryTypeName;
    }

// ---------------------------------------------------------------------------
//
// ---------------------------------------------------------------------------
//
EXPORT_C const CCaInnerEntry::TIconAttributes& CCaInnerEntry::GetIcon() const
    {
    return iIcon;
    }

// ---------------------------------------------------------------------------
//
// ---------------------------------------------------------------------------
//
EXPORT_C TUint CCaInnerEntry::GetFlags() const
    {
    return iFlags;
    }

// ---------------------------------------------------------------------------
//
// ---------------------------------------------------------------------------
//
EXPORT_C const RCaEntryAttrArray& CCaInnerEntry::GetAttributes() const
    {
    return iAttributes;
    }

// ---------------------------------------------------------------------------
//
// ---------------------------------------------------------------------------
//
EXPORT_C TUint CCaInnerEntry::GetRole() const
    {
    return iRole;
    }

// ---------------------------------------------------------------------------
//
// ---------------------------------------------------------------------------
//
EXPORT_C TInt32 CCaInnerEntry::GetUid() const
    {
    return iUid;
    }

// ---------------------------------------------------------------------------
//
// ---------------------------------------------------------------------------
//
EXPORT_C TInt CCaInnerEntry::GetIconId() const
    {
    return iIcon.iId;
    }
//    SETTERS

// ---------------------------------------------------------------------------
//
// ---------------------------------------------------------------------------
//
EXPORT_C void CCaInnerEntry::SetId( TUint aId )
    {
    iId = aId;
    }

// ---------------------------------------------------------------------------
//
// ---------------------------------------------------------------------------
//
EXPORT_C void CCaInnerEntry::SetTextL( const TDesC& aText )
    {
    iText.Close();
    iText.CreateL( aText );
    }

// ---------------------------------------------------------------------------
//
// ---------------------------------------------------------------------------
//
EXPORT_C void CCaInnerEntry::SetDescriptionL( const TDesC& aText )
    {
    iDescription.Close();
    iDescription.CreateL( aText );
    }

// ---------------------------------------------------------------------------
//
// ---------------------------------------------------------------------------
//
EXPORT_C void CCaInnerEntry::SetEntryTypeNameL( const TDesC& aTypeName )
    {
    iEntryTypeName.Close();
    iEntryTypeName.CreateL( aTypeName );
    }

// ---------------------------------------------------------------------------
//
// ---------------------------------------------------------------------------
//
EXPORT_C void CCaInnerEntry::SetIconDataL(
        const TDesC& aFilename, const TDesC& aSkinId, const TDesC& aApplicationId )
    {
    iIcon.iFileName.Copy( aFilename );
    iIcon.iSkinId.Copy( aSkinId );
    iIcon.iApplicationId.Copy(aApplicationId);
    }

// ---------------------------------------------------------------------------
//
// ---------------------------------------------------------------------------
//
EXPORT_C void CCaInnerEntry::AddAttributeL( const TDesC& aKey,
        const TDesC& aValue )
    {
    // to avoid duplicated attribute
    if( iAttributes.Exist( aKey ) )
        {
        iAttributes.RemoveAttribute( aKey );
        }
    CCaEntryAttribute* attr = CCaEntryAttribute::NewLC( aKey );
    attr->SetValueL( aValue );
    iAttributes.AppendL( attr );
    CleanupStack::Pop( attr );
    }

// ---------------------------------------------------------------------------
//
// ---------------------------------------------------------------------------
//
EXPORT_C void CCaInnerEntry::SetFlags( TUint aFlags )
    {
    iFlags = aFlags;
    }

// ---------------------------------------------------------------------------
//
// ---------------------------------------------------------------------------
//
EXPORT_C void CCaInnerEntry::SetRole( TUint aRole )
    {
    iRole = aRole;
    }

// ---------------------------------------------------------------------------
//
// ---------------------------------------------------------------------------
//
EXPORT_C void CCaInnerEntry::RemoveAttributeL( const TDesC& aKey )
    {
    iAttributes.RemoveAttribute( aKey );
    }

// ---------------------------------------------------------------------------
//
// ---------------------------------------------------------------------------
//
EXPORT_C TBool CCaInnerEntry::FindAttribute( const TDesC& aKey,
        TDes& aAttrVal )
    {
    return iAttributes.Find( aKey, aAttrVal );
    }

// ---------------------------------------------------------------------------
//
// ---------------------------------------------------------------------------
//
EXPORT_C void CCaInnerEntry::SetUid( TInt32 aUid )
    {
    iUid = aUid;
    }

// ---------------------------------------------------------------------------
//
// ---------------------------------------------------------------------------
//
EXPORT_C void CCaInnerEntry::SetIconId( TInt aIconId )
    {
    iIcon.iId = aIconId;
    }
