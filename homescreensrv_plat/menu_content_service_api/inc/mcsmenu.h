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
* Description:  Definition of the session to the Menu
*
*/


#ifndef __MCSMENU_H__
#define __MCSMENU_H__

#include <e32base.h>
#include "mcsinternaltypes.h"
class TMenuItem;
class CMenuItem;
class CMenuOperation;
class CMenuFilter;

/**
 *  Menu session.
 *  The menu is an ID-based tree structure of menu folders and items.
 *  @lib mcsmenu.lib
 *  @since S60 v5.0
 */
NONSHARABLE_CLASS( RMenu ): public RSessionBase
    {

    /** 
    * RMenu acts on behalf on CMenuItem (CMenuItem is not a subsession object).
    * Data returned by the methods below usually exists in the transfer
    * buffer -> use it immediately or copy, because the buffer will be
    * overwritten very soon!
    */
    friend class CMenuItem;

public:

    /**
    * Constructor.
    * @since S60 v5.0
    * @capability None.
    * @throws None.
    * @panic None.
    */
    RMenu() { iData = NULL; }

    /**
    * Close the session. Safe to call if not open.
    * @since S60 v5.0
    * @capability None.
    * @throws None.
    * @panic None.
    */
    IMPORT_C void Close();

    /**
    * Open the session.
    * @since S60 v5.0
    * @param aName Name of the menu content to open.
    * @capability ECapabilityReadDeviceData.
    * @throws System-wide error codes if an error occurs.
    * @panic None.
    */
    IMPORT_C void OpenL( const TDesC& aName );

    /**
    * Get ID of the root folder.
    * @since S60 v5.0
    * @return ID of the root folder.
    * @capability ECapabilityReadDeviceData.
    * @throws System-wide error codes if an error occurs.
    * @panic None.
    */
    IMPORT_C TInt RootFolderL();

    /**
    * Get list of items.
    * @since S60 v5.0
    * @param aItemArray Array receiving item list. Existing content not touched
    * (new items are appended).
    * @param aFolder Get items from this folder.
    * @param aFilter Filter criteria or NULL for unfiltered results.
    * @param aRecursive ETrue to recurse folders, EFalse for immediate
    * children only.
    * @capability ECapabilityReadDeviceData.
    * @throws System-wide error codes if an error occurs.
    * @panic None.
    */
    IMPORT_C void GetItemsL(
        RArray<TMenuItem>& aItemArray,
        TInt aFolder,
        const CMenuFilter* aFilter = NULL,
        TBool aRecursive = EFalse );

    /**
     * Get array of running applications
     */
    IMPORT_C void GetRunningAppsL( RArray<TUid>& aArray );    
    
    /**
    * Remove item.
    * @since S60 v5.0
    * @param aId ID of item to be removed.
    * @param aStatus Observer request status. When the operation completes,
    * this status will be completed with the resulting error code.
    * @return Asynchronous operation. Owned by the caller.
    * @capability ECapabilityWriteDeviceData.
    * @throws System-wide error codes if an error occurs.
    * @panic None.
    */
    IMPORT_C CMenuOperation* RemoveL( TInt aId, TRequestStatus& aStatus );

    /**
    * Move items to another folder.
    * @since S60 v5.0
    * @param aItems ID-s of items to be to be moved. All items must be in
    * the same folder.
    * @param aFolder Target folder.
    * @param aMoveBefore. In the target folder, items will be inserted before
    * this item (if found). If the target folder contains no item with that ID,
    * the moved items are appended to the end of existing items. Pass 0 to
    * append to the end.
    * @param aStatus Observer request status. When the operation completes,
    * this status will be completed with the resulting error code.
    * @return Asynchronous operation. Owned by the caller.
    * @capability ECapabilityWriteDeviceData.
    * @throws System-wide error codes if an error occurs.
    * @panic None.
    */
    IMPORT_C CMenuOperation* MoveToFolderL(
        const RArray<TInt>& aItems,
        TInt aFolder,
        TInt aMoveBefore,
        TRequestStatus& aStatus );

    /**
    * Move item to a different position in its current folder.
    * @since S60 v5.0
    * @param aId ID-s of item to be to be reordered.
    * @param aMoveBefore. Move the item before this item (if found).
    * If aMoveBefore is not found, the aItem is moved to the end.
    * @param aStatus Observer request status. When the operation completes,
    * this status will be completed with the resulting error code.
    * @return Asynchronous operation. Owned by the caller.
    * @capability ECapabilityWriteDeviceData.
    * @throws System-wide error codes if an error occurs.
    * @panic None.
    */
    IMPORT_C CMenuOperation* ReorderL(
        TInt aId,
        TInt aMoveBefore,
        TRequestStatus& aStatus );

    /**
    * Start checking the number of allocated object through this session.
    * @since S60 v5.0
    * @capability ECapabilityAllFiles.
    * @throws System-wide error codes if an error occurs.
    * @panic None.
    */
    IMPORT_C void ResourceMark();

    /**
    * Check that the number of allocated object through this session
    * matches the same of when the last call to ResourceMark was made.
    * @since S60 v5.0
    * @capability ECapabilityAllFiles.
    * @throws System-wide error codes if an error occurs.
    * @panic None.
    */
    IMPORT_C void ResourceCheck();

    /**
    * Get number of allocated object through this session.
    * @since S60 v5.0
    * @return Number of allocated object through this session.
    * @capability ECapabilityAllFiles.
    * @throws System-wide error codes if an error occurs.
    * @panic None.
    */
    IMPORT_C TInt ResourceCount();

    /**
    * Simulate a heap allocation failure for server heap. Has empty
    * implementation for the UDEB server.
    * @since S60 v5.0
    * @param aType Type of heap failure simulation.
    * @param aRate Rate of failure.
    * @capability ECapabilityAllFiles.
    * @throws System-wide error codes if an error occurs.
    * @panic None.
    */
    IMPORT_C void __DbgSetAllocFail
        ( RAllocator::TAllocFail aType, TInt aRate );

private:

    /**
    * Connect session.
    * @return Error code.
    */
    TInt ConnectSession();

    /**
    * Get item attribute value.
    * @param aId Item ID.
    * @param aAttrName. Attribute name.
    * @return Attribute value, or NULL if attribute is not defined.
    * Owner is the caller.
    */
    HBufC* GetAttributeL( TInt aId, const TDesC& aAttrName );

    
    /**
    * Get item attribute name list.
    * @param aId Item ID.
    * @param aList Attribute item list.
    */
    void GetAttributeListL( TInt aId, RArray<TAttributeName>& aList );

    /**
    * Get item header.
    * @param aId Item ID.
    * @return Item header. The header is in the transfer buffer, copy it.
    */
    const TMenuItem& GetHdrL( TInt aId );

    /**
    * Handle command.
    * @param aItem Item.
    * @param aCommand Command.
    * @param aParams. Command parameters.
    * @param aStatus Observer request status. When the operation completes,
    * this status will be completed with the resulting error code.
    * @return Asynchronous operation. Owned by the caller.
    */
    CMenuOperation* HandleCommandL(
        CMenuItem& aItem,
        const TDesC8& aCommand,
        const TDesC8& aParams,
        TRequestStatus& aStatus );

private:    // data
    
    class TData;
    TData* iData; ///< Private data. Own.

    };

#endif // __MCSMENU_H__
