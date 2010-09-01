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

#ifndef __MENUENGOBJECT_H__
#define __MENUENGOBJECT_H__

#include <e32base.h>
#include <gecoobjectbase.h>
#include <gecodefaultobject.h>
#include <gecoattributeprovider.h>
#include "menuengobjectattr.h"

// FORWARD DECLARATION

class CMenuEng;

/**
* Menu Engine Object.
* Menu Engine Object represent menu items in the Engine. Object are normally
* owned by the Engine; exception are the items that are created but not yet
* added to the Engine (these are temporarily owned by the client.)
* The primary identity of objects is the Id(), this is persistent and unique.
* 0 is not a valid ID.
*/
NONSHARABLE_CLASS( CMenuEngObject ): public CGECODefaultObject
    {

public:     // construction

    /**
    * Enum indicating type of application.
    */
    enum TAppType
        {
        ENoTypeApp,
        ENativeApp,
        EJavaApp,
        EWidgetApp
        };

    /**
    * Destructor.
    */
    IMPORT_C virtual ~CMenuEngObject();

protected:  // construction

    /**
    * Constructor.
    * @param aEng Engine.
    */
    CMenuEngObject( CMenuEng& aEng );

    /**
    * Second phase constructor.
    * @param aType Type.
    */
    void ConstructL( const TDesC& aType );

public:     // new methods

    /**
    * Get ID. The object always has a valid unique ID (even if the object is
    * not added to the Engine yet).
    * @return ID.
    */
    IMPORT_C TInt Id() const;

    /**
    * Get flags.
    * @param aFactoryCall for call from factory object(used to skip central repository check).
    * @return Flags.
    */
    IMPORT_C TUint32 Flags( TBool aFactoryCall = EFalse ) const;

    /**
    * Get type.
    * @return Type.
    */
    IMPORT_C TPtrC Type() const;

    /**
    * Get number of attributes.
    * @return Number of attributes.
    */
    IMPORT_C TInt NumAttributes() const;

    /**
    * Get attribute by index.
    * @param aIndex Attribute index.
    * @param aName Name is returned here.
    * @param aValue Value is returned here.
    * @param aLocalized ETrue if attribute is localized.
    * @return ETrue (unless aIndex is not valid).
    */
    IMPORT_C TBool GetAttribute(
        TInt aIndex,
        TPtrC& aName,
        TPtrC& aValue,
        TBool& aLocalized ) const;

    /**
    * Find attribute by name.
    * @param aName Name.
    * @param aValue Value is returned here.
    * @param aLocalized Etrue if attribute is localized.
    * @return ETrue if found.
    */
    IMPORT_C TBool FindAttribute(
        const TDesC& aName,
        TPtrC& aValue,
        TBool& aLocalized ) const;

    /**
    * Set or clear flags.
    * @param aMask Flags to set or clear.
    * @param aOn ETrue to set, EFalse to clear.
    */
    IMPORT_C void SetFlags( TUint32 aMask, TBool aOn );

    /**
    * Set attribute value. Old value, if any, is replaced.
    * @param aAttrName Name.
    * @param aAttrValue Value.
    * @param aLocalized ETrue if attribute is localized.
    */
    IMPORT_C void SetAttributeL(
        const TDesC& aAttrName,
        const TDesC& aAttrValue,
        TBool aLocalized ); // Set attribute value.

    /**
    * Set attribute value. Old value, if any, is replaced.
    * For use when reading xml file with initial data
    * and as a helper method for SetAttributeL.
    * @param aAttrName Name.
    * @param aAttrValue Value.
    * @param aLocalized ETrue if attribute is localized.
    */
    void SetInitialAttributeL(
        const TDesC& aAttrName,
        const TDesC& aAttrValue,
        TBool aLocalized ); // Set attribute value.

    /**
    * Method to set iNative for apps.
    */
    IMPORT_C void SetAppType( TAppType aNative );

    /**
    * Method to get iNative for apps.
    * @return ETrue if app is native.
    */
    IMPORT_C TAppType GetAppType() const;


    /**
    * Remove attribute.
    * @param aAttrName Name.
    */
    IMPORT_C void RemoveAttribute( const TDesC& aAttrName );

    /**
    * Remove all attributes and flags.
    */
    IMPORT_C void Reset(); // Remove all attributes and clear all flags.

public:     // but not imported: for the engine's use only.

    /**
    * Two-phased constructor.
    * @param aEng Engine.
    * @param aType Type.
    * @return The created object.
    */
    static CMenuEngObject* NewL( CMenuEng& aEng, const TDesC& aType );

    /**
    * Set ID.
    * @param aId ID.
    */
    void SetId( TInt aId );

    /**
    * Get engine.
    * @return Engine.
    */
    CMenuEng& Engine();

private:    // from CGECOObjectBase

    /**
    * Get type identifier.
    * @return Type identifier.
    */
    const TDesC& TypeIdentifier();

    /**
     * Updates flags with hidden flag from central repository if nececerry.
     * @param aFlags flags.
     */
    void UpdateCrHiddenFlagL( TUint32& aFlags ) const;

    /**
     * Parses descriptor containing folders hidden in central repository.
     * @param aHiddenFolders descriptor containing folders hidden in central repository.
     * @param aHiddenFoldersArray array to fill with names of folders hidden in central repository.
     */
    void ParseHiddenFoldersL(
                const TDesC& aHiddenFolders,
                RArray<TPtrC>& aHiddenFoldersArray ) const;

    /**
     * Checks wether folder name is in Central Repository.
     * @param aFolderName descriptor containing name of a folder.
     * @return ETrue if folder is in array, EFalse otherwise.
     */
    TBool IsInCrL( const TDesC& aFolderName ) const;



private:    // data

    TInt iId; ///< ID.
    CMenuEng& iEng; ///< Engine. DO NOT USE! Only needed to check ID-generator.
    RBuf iType; /// Type. Own.
    RMenuEngObjectAttrArray iAttributes; ///< Attributes. Own.
    TUint32 iFlags; ///< Flags.
    TAppType iAppType;	///< app type

    };

#endif // __MENUENGOBJECT_H__
