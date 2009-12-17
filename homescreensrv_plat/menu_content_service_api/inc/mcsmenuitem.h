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
* Description:  Definition of menu item
*
*/



#ifndef __MCSMENUITEM_H__
#define __MCSMENUITEM_H__

#include "mcsdef.h"
#include <e32base.h>
class RMenu;
class CMenuOperation;
class RWriteStream;
class RReadStream;

/**
 *  Menu item header.
 *  Partial data from a menu item (the administrative part).
 *  @lib mcsmenu.lib
 *  @since S60 v5.0
 */
NONSHARABLE_CLASS( TMenuItem )
    {

public:

    enum TFlags ///< Item flags.
        {
        ELockDelete = 0x1, ///< Item cannot be deleted.
        ELockName = 0x2, ///< Item cannot be renamed.
        ELockIcon = 0x4, ///< Icon cannot be changed.
        ELockMoveInto = 0x8, ///< Cannot add items into this folder.
        ELockItem = 0x10, ///< Item cannot be moved to other folder, or reordered.
        EHidden = 0x20, ///< Hidden item (do not display).
        EMissing = 0x40 ///< Missing item (do not display).
        };

public:

    /**
    * Constructor.
    * @since S60 v5.0
    * @capability None.
    * @throws None.
    * @panic None.
    */
    TMenuItem(): iId( 0 ), iParent( 0 ), iFlags( 0 ) {}

// getters

    /**
    * Get item ID.
    * @since S60 v5.0
    * @return Item ID.
    * @capability None.
    * @throws None.
    * @panic None.
    */
    TInt Id() const { return iId; }

    /**
    * Get parent folder ID.
    * @since S60 v5.0
    * @return Parent Folder ID.
    * @capability None.
    * @throws None.
    * @panic None.
    */
    TInt Parent() const { return iParent; }

    /**
    * Get flags.
    * @since S60 v5.0
    * @return Flags.
    * @capability None.
    * @throws None.
    * @panic None.
    */
    TUint32 Flags() const { return iFlags; }

    /**
    * Get type.
    * @since S60 v5.0
    * @return Type.
    * @capability None.
    * @throws None.
    * @panic None.
    */
    TPtrC Type() const { return TPtrC( iType ); }
    

// setters

    /**
    * Set item ID.
    * @since S60 v5.0
    * @param aId Item ID.
    * @capability None.
    * @throws None.
    * @panic None.
    */
    void SetId( TInt aId ) { iId = aId; }

    /**
    * Set parent folder ID.
    * @since S60 v5.0
    * @param aParent Parent folder ID.
    * @capability None.
    * @throws None.
    * @panic None.
    */
    void SetParent( TInt aParent ) { iParent = aParent; }

    /**
    * Set flags.
    * @since S60 v5.0
    * @param aFlags Flags.
    * @capability None.
    * @throws None.
    * @panic None.
    */
    void SetFlags( TUint32 aFlags ) { iFlags = aFlags; }

    /**
    * Set type.
    * @since S60 v5.0
    * @param aType Type. Maximum length is KMenuMaxTypeLen.
    * @capability None.
    * @throws None.
    * @panic None.
    */
    void SetType( const TDesC& aType ) { iType = aType; }
    

// not exported public

    /**
    * Externalize object.
    * @param aStream Stream to externalize into.
    * @capability None.
    * @throws System-wide error codes if an error occurs.
    * @panic None.
    */
    void ExternalizeL( RWriteStream& aStream ) const;

    /**
    * Internalize object.
    * @param aStream Stream to externalize from.
    * @capability None.
    * @throws System-wide error codes if an error occurs.
    * @panic None.
    */
    void InternalizeL( RReadStream& aStream );

private:    // data

    TInt iId; ///< Item ID.
    TInt iParent; ///< Parent folder ID.
    TUint32 iFlags; ///< Flags.
    TBuf<KMenuMaxTypeLen> iType; ///< Type.

    };

/**
 *  Menu item.
 *  This class holds client-side local copy of requested item attributes.
 *  CMenuItem and the underlying storage are very losely coupled:
 *  - Changes to this class are not automatically comitted to menu. Call SaveL()
 *    to commit changes.
 *  - Changes (made by others) to the underyling item are not reflected back
 *    in this CMenuItem object. ==> CMenuItem users are expected to open
 *    notifiers on the menu and watch for changes. If change is reported for
 *    the folder which contains any open CMenuItems, users should discard the
 *    objects and OpenL() them anew.
 *  @lib mcsmenu.lib
 *  @since S60 v5.0
 */
NONSHARABLE_CLASS( CMenuItem ): public CBase
    {

public:

    /**
    * Destructor.
    * @since S60 v5.0
    * @capability None.
    * @throws None.
    * @panic None.
    */
    IMPORT_C virtual ~CMenuItem();

    /**
    * Create new menu item. The item is not created until are committed with a
    * call to SaveL().
    * @since S60 v5.0.
    * @param aMenu Menu session.
    * @param aType Type. Maximum length is KMenuMaxTypeLen.
    * @param aFolder Parent folder.
    * @param aMoveBefore. In the target folder, the new item will be inserted
    * before this item (if found). If the target folder contains no item with
    * that ID, the new item is appended to the end of existing items. Pass 0 to
    * append to the end.
    * @return New unsaved menu item.
    * @capability None.
    * @throws KErrArgument if aType is KMenuTypeData. 
    * System-wide error codes if an error occurs.
    * @panic None.
    */
    IMPORT_C static CMenuItem* CreateL(
        RMenu& aMenu,
        const TDesC& aType,
        TInt aFolder,
        TInt aInsertBefore );

    /**
    * Open existing menu item.
    * @since S60 v5.0
    * @param aMenu Menu session.
    * @param aId Item ID.
    * @return Local copy of menu item.
    * @capability ECapabilityReadDeviceData.
    * @throws System-wide error codes if an error occurs.
    * @panic None.
    */
    IMPORT_C static CMenuItem* OpenL( RMenu& aMenu, TInt aId );

    /**
    * Open existing menu item.
    * @since S60 v5.0
    * @param aMenu Menu session.
    * @param aHdr Item header. Validity is not checked - it is assumed that it
    * was retrieved by a call to RMenu::GetItemsL().
    * (It is no use in modifying an existing header, because the changes are
    * not reflected back in the server side data).
    * @return Local copy of menu item.
    * @capability None.
    * @throws System-wide error codes if an error occurs.
    * @panic None.
    */
    IMPORT_C static CMenuItem* OpenL( RMenu& aMenu, const TMenuItem& aHdr );

    /**
    * Get item ID.
    * @since S60 v5.0
    * @return Item ID. (0 for unsaved new items.)
    * @capability None.
    * @throws None.
    * @panic None.
    */
    IMPORT_C TInt Id() const;

    /**
    * Get parent folder ID.
    * @since S60 v5.0
    * @return Parent folder ID
    * @capability None.
    * @throws None.
    * @panic None.
    */
    IMPORT_C TInt Parent() const;

    /**
    * Get flags.
    * @since S60 v5.0
    * @return Flags.
    * @capability None.
    * @throws None.
    * @panic None.
    */
    IMPORT_C TUint32 Flags() const;

    /**
    * Set or clear flags.
    * @since S60 v5.0
    * @param aMask Flags to set or clear.
    * @param aOn ETrue to set, EFalse to clear flags.
    * @capability None.
    * @throws None.
    * @panic None.
    */
    IMPORT_C void SetFlags( TUint32 aMask, TBool aOn );

    /**
    * Get type.
    * @since S60 v5.0
    * @return Type.
    * @capability None.
    * @throws None.
    * @panic None.
    */
    IMPORT_C TPtrC Type() const;
    
    /**
    * Get attribute value.
    * @since S60 v5.0
    * @param aAttrName Attribute name.
    * @param aAttrExists Will be set to indicate whether attribute exists
    * or not.
    * @return Attribute value. Empty string if attribute does not exist.
    * @capability ECapabilityReadDeviceData.
    * @throws KErrArgument in case of invalid attribute name. 
    * System-wide error codes if an error occurs.
    * @panic None.
    */
    IMPORT_C TPtrC GetAttributeL( const TDesC& aAttrName, TBool& aAttrExists );
    
    
    /**
    * Get attribute name list.
    * @since S60 v5.0
    * @param aList Attribute name list.
    * @capability ECapabilityReadDeviceData.
    * @throws System-wide error codes if an error occurs.
    * @panic None.
    */
    IMPORT_C void GetAttributeListL( RArray<TAttributeName>& aList );

    /**
    * Get running status folders and applications.
    * @return TBool.
    * @capability None.
    * @throws System-wide error codes if an error occurs.
    * @panic None.
    */
    IMPORT_C TBool RunningStatusL();
    
    /**
    * Set attribute value.
    * @since S60 v5.0
    * @param aAttrName Attribute name.
    * @param aAttrValue Attribute value.
    * @capability ECapabilityWriteDeviceData.
    * @throws KErrArgument in case of invalid attribute name or value. 
    * System-wide error codes if an error occurs.
    * @panic None.
    */
    IMPORT_C void SetAttributeL( const TDesC& aAttrName, const TDesC& aAttrValue );

    /**
    * Remove attribute.
    * @since S60 v5.0
    * @param aAttrName Attribute name.
    * @capability ECapabilityWriteDeviceData.
    * @throws KErrArgument in case of invalid attribute name. 
    * System-wide error codes if an error occurs.
    * @panic None.
    */
    IMPORT_C void RemoveAttributeL( const TDesC& aAttrName );

    /**
    * Save item (commit changes).
    * If the item is a new item (created with CreateL()), its ID is generated
    * now. However, the item with that ID does not exist until the operation
    * successfully completes!
    * @since S60 v5.0
    * @param aStatus Observer request status. When the operation completes,
    * this status will be completed with the resulting error code.
    * @return Asynchronous operation. Owned by the caller.
    * @capability ECapabilityWriteDeviceData.
    * @throws System-wide error codes if an error occurs.
    * @panic None.
    */
    IMPORT_C CMenuOperation* SaveL( TRequestStatus& aStatus );

    /**
    * Handle command.
    * @since S60 v5.0
    * @param aCommand Command.
    * @param aParams. Command parameters.
    * @param aStatus Observer request status. When the operation completes,
    * this status will be completed with the resulting error code.
    * @return Asynchronous operation. Owned by the caller.
    * @capability None.
    * @throws KErrNotSupported if no handler was found for the given type of 
    * item. System-wide error codes if an error occurs.
    * @panic None.
    */
    IMPORT_C CMenuOperation* HandleCommandL(
        const TDesC8& aCommand,
        const TDesC8& aParams,
        TRequestStatus& aStatus );

private:

    /**
    * Constructor.
    */
    CMenuItem();

    /**
    * Factory method.
    * @param aMenu Menu.
    * @return Menu item.
    */
    static CMenuItem* NewLC( RMenu& aMenu );

    /**
    * Add new item.
    * @param aStatus Observer request status. When the operation completes,
    * this status will be completed with the resulting error code.
    * @return Asynchronous operation. Owned by the caller.
    */
    CMenuOperation* AddL( TRequestStatus& aStatus );

    /**
    * Update item.
    * @param aStatus Observer request status. When the operation completes,
    * this status will be completed with the resulting error code.
    * @return Asynchronous operation. Owned by the caller.
    */
    CMenuOperation* UpdateL( TRequestStatus& aStatus );

    /**
    * Check if there are unsaved changes.
    * @return ETrue if there are unsaved changes.
    */
    TBool Changed() const;

    /**
    * Mark the item as unchanged.
    */
    void ClearChanged();
    
private:    // data
    
    class TData;
    TData* iData; ///< Private data. Own.

    };

#endif // __MCSMENUITEM_H__
