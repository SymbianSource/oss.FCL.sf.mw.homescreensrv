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

#ifndef __CAINNERENTRY_H__
#define __CAINNERENTRY_H__

#include <e32base.h>

#include "caentryattribute.h"

/**
 * Class represents Content Arsenal entry
 */
NONSHARABLE_CLASS( CCaInnerEntry ): public CBase
    {
public:
    /*
     * Struct containing icon attributes
     */
    struct TIconAttributes
        {
        //TODO: maybe it should be C class with RBuf instead of TFileName
        TInt iId;
        TInt iBitmapId;
        TInt iMaskId;
        TInt iSkinMajorId;
        TInt iSkinMinorId;
        TFileName iFileName;
        };

public:

    /**
     * Destructor.
     */
    virtual ~CCaInnerEntry();

    /**
     * Two-phased constructor.
     * @return The created object.
     */
    IMPORT_C static CCaInnerEntry* NewL();

    /**
     * Two-phased constructor.
     * @return The created object.
     */
    IMPORT_C static CCaInnerEntry* NewLC();

    //    GETTERS
    /**
     * Gets entry id.
     * @return Entry id.
     */
    IMPORT_C TInt GetId() const;

    /**
     * Gets entry text.
     * @return Entry text.
     */
    IMPORT_C const RBuf& GetText() const;

    /**
     * Gets entry description.
     * @return Entry description.
     */
    IMPORT_C const RBuf& GetDescription() const;
    
    /**
     * Gets entry typename.
     * @return Entry typename.
     */
    IMPORT_C const RBuf& GetEntryTypeName() const;

    /**
     * Gets icon attributes.
     * @return Icon atrributes.
     */
    IMPORT_C const CCaInnerEntry::TIconAttributes& GetIcon() const;

    /**
     * Gets entry flags.
     * @return Entry flags.
     */
    IMPORT_C TUint GetFlags() const;

    /**
     * Gets entry attributes.
     * @return Entry attributes table.
     */
    IMPORT_C const RCaEntryAttrArray& GetAttributes() const;

    /**
     * Gets entry role.
     * @return Entry role.
     */
    IMPORT_C TUint GetRole() const;

    /**
     * Gets entry uid.
     * @return Entry uid.
     */
    IMPORT_C TInt32 GetUid() const;

    /**
     * Gets icon id.
     * @return Icon id.
     */
    IMPORT_C TInt GetIconId() const;

    //    SETTERS
    /**
     * Sets entry id.
     * @param aId Entry id.
     */
    IMPORT_C void SetId( TUint aId );

    /**
     * Sets the entry text.
     * @param aText Entry text.
     */
    IMPORT_C void SetTextL( const TDesC& aText );

    /**
     * Sets the entry description.
     * @param aText Entry description.
     */
    IMPORT_C void SetDescriptionL( const TDesC& aText );
    
    /**
     * Sets the entry typename.
     * @param aTypeName Entry typename.
     */
    IMPORT_C void SetEntryTypeNameL( const TDesC& aTypeName );

    /**
     * Sets icon data.
     * @param aBitmapId Bitmap id.
     * @param aMaskId Mask id.
     * @param aSkinMajorId Skin major id.
     * @param aSkinMinorId Skin minor id.
     * @param aFilename Filename.
     */
    IMPORT_C void SetIconDataL( TInt aBitmapId, TInt aMaskId,
            TInt aSkinMajorId, TInt aSkinMinorId, const TDesC& aFilename );

    /**
     * Adds attribute to entry.
     * @param aKey Attribute key.
     * @param aValue Attribute value.
     */
    IMPORT_C void AddAttributeL( const TDesC& aKey, const TDesC& aValue );

    /**
     * Sets flags for the entry
     * @param aFlags Flags.
     */
    IMPORT_C void SetFlags( TUint aFlags );

    /**
     * Sets the entry's role.
     * @param aRole Entry role.
     */
    IMPORT_C void SetRole( TUint aRole );

    /**
     * Remove attribute with given key.
     * @param aKey Key of attribute to be removed.
     */
    IMPORT_C void RemoveAttributeL( const TDesC& aKey );

    /**
     * Verifies if attribute exists, and returns its value for the given key.
     * @param aKey Attribute key.
     * @param aAttrVal Attribute value.
     * @return ETrue if attribute exists.
     */
    IMPORT_C TBool FindAttribute( const TDesC& aKey, TDes& aAttrVal );
    
    /**
     * Sets the entry uid.
     * @param aUid Entry uid.
     */
    IMPORT_C void SetUid( TInt32 aUid );

    /**
     * Sets the entry icon id (id from storage).
     * @param aIconId Icon id.
     */
    IMPORT_C void SetIconId( TInt aIconId );

    /**
     * Externalizes object to the stream
     * @param aStream a write stream
     */
    IMPORT_C void ExternalizeL( RWriteStream& aStream ) const;

    /**
     * Internalizes stream to the object
     * @param aStream a read stream
     */
    IMPORT_C void InternalizeL( RReadStream& aStream );

private:

    /**
     * Second phased constructor.
     */
    void ConstructL();

    /**
     * Constructor
     */
    CCaInnerEntry();

private:
    // data

    /*
     * Unique identifier of the entry.
     */
    TInt iId;

    /*
     * Name.
     * Own.
     */
    RBuf iText;

    /*
     * Description.
     * Own.
     */
    RBuf iDescription;
    
    /*
     * Entry type.
     * Own.
     */
    RBuf iEntryTypeName;

    /*
     * Own.
     */
    TIconAttributes iIcon;

    /*
     * Flags.
     */
    TUint iFlags;

    /*
     * Attributes array. Own
     */
    RCaEntryAttrArray iAttributes;

    /*
     * Entry role ( group, item )
     */
    TUint iRole;

    /*
     * Uid
     */
    TInt32 iUid;

    };

#endif // __CAINNERENTRY_H__
