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

#ifndef __MENUENG_H__
#define __MENUENG_H__

#include <e32base.h>
#include <xcfwengine.h>
#include <xcfwtree.h> // for RNodeArray
#include "mcsmenunotifier.h" // for notification events

// FORWARD DECLARATION

class TMenuItem;
class CXCFWTree;
class MXCFWNode;
class CMenuEngObjectFactoryProxy;
class CMenuEngObject;
class CMenuEngIdManager;
class MMenuEngObserver;
class MMenuEngOperation;
class MMenuEngFilter;
class MMenuEngVisitor;

/**
* Event notification data.
*/
class TMenuEngNotify
    {

public:     // construction

    /**
    * Constructor.
    * @param aFolder Folder.
    * @param aEvents Events.
    */
    TMenuEngNotify( TInt aFolder, TInt aEvents )
        : iFolder( aFolder ), iEvents( aEvents ) {}

public:     // data

    TInt iFolder; ///< Folder.
    TInt iEvents; ///< Events.

    };

/**
* Menu Engine.
*/
NONSHARABLE_CLASS( CMenuEng ): public CActive, public MXCFWEngineObserver
    {

public:     // construction

    /**
    * Destructor.
    */
    IMPORT_C virtual ~CMenuEng();

    /**
    * Two-phased constructor.
    * @param aName Name.
    * @param aObserver Observer.
    * @return The created object.
    */
    IMPORT_C static CMenuEng* NewL(
        const TDesC& aName,
        MMenuEngObserver& aObserver );

protected:  // construction

    /**
    * Constructor.
    * @param aObserver Observer.
    */
    CMenuEng( MMenuEngObserver& aObserver );

    /**
    * Second phase constructor.
    * @param aName Name.
    */
    void ConstructL( const TDesC& aName );

public:     // new methods

    enum TState /// State machine states.
        {
        ENull = 0, ///< Initial state.
        ELoadRamFile, ///< Loading RAM file.
        ELoadRomFile, ///< Loading ROM file.
        ESaveFile, ///< Saving TEMP file and replacing RAM file.
        EExecuteOp, ///< Operation scheduled for execution or being executed.
        EReady, ///< Idle.
        EDead ///< Unrecoverable error; final state.
        };

    /**
    * Queue an operation for execution.
    * Queued operations are scheduled for execution by the Engine.
    * @param aOperation Operation.
    */
    IMPORT_C void QueueOperationL( MMenuEngOperation& aOperation );

    /**
    * Dequeue an operation (remove from the operation queue). Safe to call
    * if not queued.
    * @param aOperation Operation.
    */
    IMPORT_C void DequeueOperation( MMenuEngOperation& aOperation );

    /**
    * Traverse a subtree of the item tree.
    * @param aFolder traversal starts at this folder.
    * @param aVisitor Visitor visiting the nodes and controlling the traversal.
    */
    IMPORT_C void TraverseFolderL(
        TInt aFolder,
        MMenuEngVisitor& aVisitor ) const;

    /**
    * Create a new object. Owner is the caller. The object is not added to
    * the engine yet, but it has an ID already. It can be used to AddL() item.
    * @param aType Type.
    * @return Created object. Owner is the caller.
    */
    IMPORT_C CMenuEngObject* NewObjectL( const TDesC& aType );

    /**
    * Returns current engine state.
    * @return Current state.
    */
	inline TState State() const;
	
	/*
	* Gets legacy format
	* @return legacy format
	*/
	IMPORT_C TBool GetOnceLegacyFormat();

public:     // data query (read access)

    // Read-access methods can be called any time.
    // There is a short startup period during which the engine data is not
    // ready yet; calls made in that period leave with KErrNotReady.
    // When the engine becomes ready, an (0 ,EItemsAddedRemoved) event is
    // sent to the observer.

    /**
    * Get ID of root folder.
    * @param aId ID of root folder.
    */
    IMPORT_C void RootFolderL( TInt& aId ) const;

    /**
    * Get parent folder of item.
    * @param aId Item ID.
    * @param aParentId ID of parent folder.
    */
    IMPORT_C void ParentFolderL( TInt aId, TInt& aParentId ) const;

    /**
    * Get item header.
    * @param aId Item ID.
    * @param aItem Item header is returned here.
    */
    IMPORT_C void GetItemL( TInt aId, TMenuItem& aItem ) const;

    /**
    * Get list of items.
    * @param aItemArray Array receiving item list. Existing content not touched
    * (new items are appended).
    * @param aFolder Get items from this folder.
    * @param aFilter Filter criteria or NULL for unfiltered results.
    * @param aRecursive ETrue to recurse folders, EFalse for immediate
    * children only.
    */
    IMPORT_C void GetItemsL(
        RArray<TMenuItem>& aItemArray,
        TInt aFolder,
        const MMenuEngFilter* aFilter,
        TBool aRecursive ) const;

    /**
    * Get object.
    * @param aId Item ID.
    * @return Object.
    */
    IMPORT_C const CMenuEngObject& ObjectL( TInt aId ) const;

public:     // data manipulation (write access).

    // All write-access methods methods must be wrapped in MMenuEngOperation
    // objects (scheduled execution), otherwise they leave with KErrLocked.

    /**
    * Remove item.
    * @param aId ID of item to be removed.
    */
    IMPORT_C void RemoveL( TInt aId );

    /**
    * Move items to another folder.
    * @param aItems ID-s of items to be to be moved. All items must be in
    * the same folder.
    * @param aFolder Target folder.
    * @param aMoveBefore. In the target folder, items will be inserted before
    * this item (if found). If the target folder contains no item with that ID,
    * the moved items are appended to the end of existing items. Pass 0 to
    * append to the end.
    */
    IMPORT_C void MoveToFolderL(
        RArray<TInt>& aItems,
        TInt aFolder,
        TInt aMoveBefore );

    /**
    * Move item to a different position in its current folder.
    * @param aId ID of item to be to be reordered.
    * @param aMoveBefore. Move the item before this item (if found).
    * If aMoveBefore is not found, the item is moved to the end.
    */
    IMPORT_C void ReorderL( TInt aId, TInt aMoveBefore );

    /**
    * Add new object.
    * @param aObject Object to be added. Note, the object must have been
    * created with NewObjectL of THIS engine (or the call fails)!
    * Ownership taken.
    * @param aFolder Parent folder.
    * @param aMoveBefore. Move the item before this item (if found).
    * If aMoveBefore is not found, the item is added to the end.
    */
    IMPORT_C void AddL(
        CMenuEngObject& aObject,
        TInt aFolder,
        TInt aInsertBefore );

    /**
    * Get modifiable object. Note: when this method is called, the engine
    * marks the object as changed (without checking if there was really
    * a change or not). Therefore, it is advisable to use ObjectL() to
    * investigate an object, and only call ModifiableObjectL if changes are
    * really needed. This results in better performace, as it avoids
    * unnecessary saving.
    * @param aId Item ID.
    * @param aEvent event for engine to append.
    * @return Modifiable object. Already marked as changed.
    */
    IMPORT_C CMenuEngObject& ModifiableObjectL( TInt aId,
    		TInt aEvent = RMenuNotifier::EItemAttributeChanged );

    
    /**
    * Append new notification to the list of pending notifications.
    * @param aFolder Folder.
    * @param aEvents Events.
    */
    IMPORT_C void AppendNotifyL( TInt aFolder, TInt aEvents );
    
    
    IMPORT_C TBool Exist( TInt aId ) const;

private:    // from CActive

    /**
    * Outstanding request completed.
    */
    void RunL();

    /**
    * Cancel outstanding request.
    */
    void DoCancel();

    /**
    * Handle error.
    * @param aError Error code.
    * @return KErrNone.
    */
    TInt RunError( TInt aError );

private:    // from MXCFWEngineObserver

    /**
    * Called when Engine parsing / saving state changes.
    * @param aEvent Engine event
    */
    void HandleEngineEventL( TXCFWEngineEvent aEvent );

    /**
    * Called when there's an error during parsing / saving.
    * @param aErrorCode Error code
    */
    void HandleEngineErrorL( TInt aErrorCode );        

private:    // types

    enum TFile /// File selector.
        {
        ERomFile, ///< Factory file in ROM.
        ERamFile, ///< Working file in RAM.
        ETempFile, ///< Temp file for saving (RAM).
        };

private:    // new methods

    /**
    * Get object from node.
    * @param aNode Node.
    * @return Object.
    */
    inline CMenuEngObject& Object( MXCFWNode& aNode ) const;

    /**
    * Tree is up, perform structure check.
    * @param aTree Tree.
    * @return The root folder node.
    */
    MXCFWNode& CheckTreeL( MXCFWTree& aTree ) const;

    /**
    * Sanity ID check for all nodes in the tree.
    * @param aTree Tree.
    * @return ETrue if ID-s are OK.
    */
    TBool DebugSanityCheck( MXCFWTree& aTree ) const;

    /**
    * Wait for data xml file to load.
    */
    void ActiveWaitForFileLoadL();

    /**
    * Sanity ID check for one nodes in the tree.
    * @param aNode Node.
    * @return ETrue if ID is OK.
    */
    TBool DebugSanityCheck( MXCFWNode& aNode ) const;

    /**
    * Find node by ID, leave with KErrNotFound if not found.
    * @param aId ID.
    * @return Node.
    */
    MXCFWNode& NodeL( TInt aId ) const;

    /**
    * Find folder node by ID, leave with KErrNotFound if not found or not
    * folder.
    * @param aId ID.
    * @return Folder node.
    */
    MXCFWNode& FolderNodeL( TInt aId ) const;

    /**
    * Find child of aParent with given id, or NULL. Not recursive.
    * @param aParent Folder.
    * @param aId Child ID.
    * @return Child or NULL.
    */
    MXCFWNode* Child( MXCFWNode& aParent, TInt aId );

    /**
    * Cancel all operations in the queue.
    */
    void CancelAllOperations();

    /**
    * Validate content filename. Leave with KErrArgument if not valid.
    * @param aName Content filename (without extension).
    */
    void ValidateNameL( const TDesC& aName );

    /**
    * Generate filename of content file.
    * @param aFname Generated filename is returned here.
    * @param aSelector Filename selector.
    */
    void GetFileNameL( TFileName& aFname, TFile aSelector );

    /**
    * Traverse a node.
    * @param aNode Node.
    * @param aVisitor Visitor visiting the nodes and controlling the traversal.
    */
    TBool TraverseNodeL( MXCFWNode& aNode, MMenuEngVisitor& aVisitor ) const;

    /**
    * Tree is up; create and initialize ID manager and tree ID-s.
    * @param aRootNode Root node.
    */
    void InitIdManagerL( MXCFWNode& aRootNode );

    /**
    * Read id_seed from the tree.
    * @param aIdSeed ID seed.
    */
    void GetIdSeedL( TInt& aIdSeed );

    /**
    * Write the id_seed to the tree.
    * @param aSeed ID seed.
    */
    void SetIdSeedL( TInt aSeed );

private:    // state machine parts

    /**
    * Self-completion: go to RunL() asynchronously.
    * @param aError Error code.
    */
    void SelfComplete( TInt aError );

    /**
    * Initiate loading RAM file.
    */
    void LoadRamFileL();

    /**
    * Initiate loading ROM file.
    */
    void LoadRomFileL();

    /**
    * Initiate saving to TEMP file.
    */
    void SaveTempFileL();

    /**
    * Replace RAM file with TEMP file.
    */
    void ReplaceRamFileL();

    /**
    * Complete current operation, if any.
    * @param aError Completion code.
    */
    void CompleteCurrentOperation( TInt aError );

    /**
    * Start executing next operation, if any.
    */
    void ExecuteOperationL();

    /**
    * Save changes, if any.
    */
    void SaveChangesL();

private:    // data

    RBuf iName; ///< Own.
    RBuf iTempFileName; ///< Own.
    RBuf iRamFileName; ///< Own.
    TBool iChanged; ///< ETrue if there are unsaved changes.
    MMenuEngObserver& iObserver;
    TState iState; ///< State.
    CXCFWTree* iTree; ///< DOM tree. Own.
    CXCFWEngine* iEngine; ///< XML Engline. Own.
    CMenuEngObjectFactoryProxy* iObjectFactory; ///< Object factory. Own.
    TInt iRoot; ///< Cached root folder id.
    RPointerArray<MMenuEngOperation> iOperations; ///< Pending operations.
    MMenuEngOperation* iCurrentOperation; ///< Operation being served.
    RArray<TMenuEngNotify> iNotifyQueue; ///< Events for notifiers.
    RFs iFs; ///< File Server Session. Own.
    CMenuEngIdManager* iIdManager; ///< ID manager. Owned.
    CActiveSchedulerWait* iActiveWait;
    /*
    * Legacy format
    */
    TBool iLegacyFormat;
    /**
     * True if the last saving was unsuccessful
     */
    TBool iDiskWasFullAndRamFileWasNotCreated;
    };

// -----------------------------------------------------------------------------
// CMenuEng::State
// -----------------------------------------------------------------------------
// 
CMenuEng::TState CMenuEng::State() const
    {
    return iState;
    }

#endif // __MENUENG_H__
