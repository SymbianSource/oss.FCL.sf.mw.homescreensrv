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
* Description:  Definition of Menu operation subsession
*
*/


#ifndef __MENUCLIENTOPERATION_H__
#define __MENUCLIENTOPERATION_H__

#include "mcsmenuoperation.h"
#include <e32base.h>

class RMenu;
class RMenuBuf;
class CMenuItem;

/**
 *  Menu operation subsession object.
 *  This is a "disposable" object (used only once):
 *  CreateXxx() -> Start() -> Close().
 *  CreateXxx() methods package and send operation data to server side; Start()
 *  invokes the operation asynchronously.
 *  @lib mcsmenu.lib
 *  @since S60 v5.0
 */
NONSHARABLE_CLASS( RMenuOperation ): public RSubSessionBase
    {

public:

    /**
    * Close the operation. Safe to call if not open.
    */
    void Close();

    /**
    * Create a Remove operation.
    * @param aMenu Menu.
    * @param aId Remove this item.
    * @return Error code.
    */
    TInt CreateRemove( RMenu& aMenu, TInt aId );

    /**
    * Create a MoveToFolder operation.
    * @param aMenu Menu.
    * @param aId ID-s of item to be to be moved have been sent in this
    * buffer.
    * @param aMoveBefore. Move the items before this item (if found).
    * If aMoveBefore is not found, the aItem is moved to the end.
    * @return Error code.
    */
    TInt CreateMoveToFolder(
        RMenu& aMenu,
        const RMenuBuf& aItems,
        TInt aFolder,
        TInt aMoveBefore );

    /**
    * Create Reorder operation.
    * @param aMenu Menu.
    * @param aId ID of item to be to be reordered.
    * @param aMoveBefore. Move the item before this item (if found).
    * If aMoveBefore is not found, the aItem is moved to the end.
    * @return Error code.
    */
    TInt CreateReorder( RMenu& aMenu, TInt aId, TInt aMoveBefore );

    /**
    * Create Add operation.
    * @param aMenu Menu.
    * @param aType Type. Maximum length is KMenuMaxTypeLen.
    * @param aChanges Operation data (attributes, etc.) have been sent in this
    * buffer.
    * @param aId ID of item to be created is returned here (ID is reserved,
    * but not yet valid).
    * @return Error code.
    */
    TInt CreateAdd( RMenu& aMenu, const TDesC& aType, const RMenuBuf& aChanges, TInt& aId );

    /**
    * Create Update operation.
    * @param aMenu Menu.
    * @param aId Id.
    * @param aChanges Operation data (attributes, etc.) have been sent in this
    * buffer.
    * @return Error code.
    */
    TInt CreateUpdate( RMenu& aMenu, TInt aId, const RMenuBuf& aChanges );

    /**
    * Cancel operation.
    * NOT safe to call on an unopened operation.
    */
    void Cancel();

    /**
    * Start operation.
    * @param aStatus Completes with error code when operation is done.
    */
    void Start( TRequestStatus &aStatus );

    };

/**
 *  RMenuOperation wrapped as CMenuOperation.
 *  @lib mcsmenu.lib
 *  @since S60 v5.0
 */
NONSHARABLE_CLASS( CMenuClientOperation ): public CMenuOperation
    {

public:

    /**
    * Destructor.
    */
    virtual ~CMenuClientOperation();

    /**
    * Constructor.
    * @param aMenu Menu.
    * @param Priority Active Object priority.
    * @param aStatus Completes with error code when operation is done.
    */
    CMenuClientOperation(
        RMenu &aMenu,
        TInt aPriority,
        TRequestStatus &aObserverStatus );

    /**
    * Construct and start Remove operation.
    * @param aMenu Menu.
    * @param aId Remove this item.
    */
    void RemoveL( TInt aId );

    /**
    * Construct and start a MoveToFolder operation.
    * @param aMenu Menu.
    * @param aId ID-s of item to be to be moved have been sent in this
    * buffer.
    * @param aMoveBefore. Move the items before this item (if found).
    * If aMoveBefore is not found, the aItem is moved to the end.
    */
    void MoveToFolderL( const RMenuBuf& aItems, TInt aFolder, TInt aMoveBefore );

    /**
    * Construct and start Reorder operation.
    * @param aMenu Menu.
    * @param aId ID of item to be to be reordered.
    * @param aMoveBefore. Move the item before this item (if found).
    * If aMoveBefore is not found, the aItem is moved to the end.
    */
    void ReorderL( TInt aId, TInt aMoveBefore );

    /**
    * Construct and start Add operation.
    * @param aMenu Menu.
    * @param aType Type. Maximum length is KMenuMaxTypeLen.
    * @param aChanges Operation data (attributes, etc.) have been sent in this
    * buffer.
    * @return ID of item to be created (ID is reserved, but not yet valid).
    */
    TInt AddL( const TDesC& aType, const RMenuBuf& aChanges );

    /**
    * Construct and start Update operation.
    * @param aMenu Menu.
    * @param aId Id.
    * @param aChanges Operation data (attributes, etc.) have been sent in this
    * buffer.
    */
    void UpdateL( TInt aId, const RMenuBuf& aChanges );

private:

    /**
    * Start operation.
    */
    void Start();

// from base class CActive

    /**
    * Operation completed.
    */
    void RunL();

    /**
    * Cancel operation.
    */
    void DoCancel();

    /**
    * Never called, RunL() is not leaving.
    */
    TInt RunError( TInt /*aError*/ ) { return KErrNone; }

private:    // data

    RMenuOperation iOperation; ///< The operation. Own.

    };

#endif // __MENUCLIENTOPERATION_H__
