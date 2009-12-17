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

#include <xcfwtree.h>
#include <xcfwengine.h>
#include <pathinfo.h>
#include <f32file.h>
#include <sysutil.h>
#include "menueng.h"
#include "menuengobjectfactoryproxy.h"
#include "menuengobject.h"
#include "menuengobserver.h"
#include "menuengoperation.h"
#include "menuengfilter.h"
#include "menuengvisitor.h"
#include "menuengitemlister.h"
#include "menuengdeletionchecker.h"
#include "menuengidmanagerinit.h"
#include "menuengidsetter.h"
#include "menuengidmanager.h"
#include "menuengidcollector.h"
#include "menuengflags.h"
#include "mcsmenuitem.h" // for flags

// CONSTANTS

LOCAL_D const TInt KDriveAndColon = 2; //drive letter and colon, e.g. "c:"
_LIT( KMenuTypeIdSeed, "menu:id_seed" );
_LIT( KMenuAttrIdSeed, "seed" );
_LIT( KMenuContentDirName, "content\\" );
_LIT( KMenuContentExtension, ".xml" );
_LIT( KMenuTempExtension, ".$$$" );

// ================= LOCAL FUNCTIONS =======================

/**
* Insert string into other string, checking vaailable space.
* Leaves with KErrOverflow if aSoure cannot be inserted to aTarget.
* @param aTarget Target descriptor.
* @param aPos Insertion point.
* @param aSource Source descriptor.
*/
LOCAL_C void InsertL( TDes& aTarget, TInt aPos, const TDesC& aSource )
    {
    if ( aTarget.MaxLength() < aTarget.Length() + aSource.Length() )
        {
        User::Leave( KErrOverflow );
        }
    aTarget.Insert( aPos, aSource );
    }

/**
* Append string to the end of other string, checking available space.
* Leaves with KErrOverflow if aSource cannot be appended to aTarget.
* @param aTarget Target descriptor.
* @param aSource Source descriptor.
*/
LOCAL_C void AppendL( TDes& aTarget, const TDesC& aSource )
    {
    if ( aTarget.MaxLength() < aTarget.Length() + aSource.Length() )
        {
        User::Leave( KErrOverflow );
        }
    aTarget.Append( aSource );
    }

// ================= MEMBER FUNCTIONS =======================

// ---------------------------------------------------------
// CMenuEng::Object
// ---------------------------------------------------------
//
CMenuEngObject& CMenuEng::Object( MXCFWNode& aNode ) const
    {
    return (CMenuEngObject&)(*aNode.Data());
    }

// ---------------------------------------------------------
// CMenuEng::~CMenuEng
// ---------------------------------------------------------
//
EXPORT_C CMenuEng::~CMenuEng()
    {
    Cancel();
    CancelAllOperations();
    iOperations.Close();
    iNotifyQueue.Close();
    delete iEngine;
    delete iTree;
    delete iObjectFactory;
    delete iIdManager;
    iFs.Close();
    iName.Close();
    iTempFileName.Close();
    iRamFileName.Close();
    delete iActiveWait;
    }

// ---------------------------------------------------------
// CMenuEng::NewL
// ---------------------------------------------------------
//
EXPORT_C CMenuEng* CMenuEng::NewL
( const TDesC& aName, MMenuEngObserver& aObserver )
    {
    CMenuEng* eng = new (ELeave) CMenuEng( aObserver );
    CleanupStack::PushL( eng );
    eng->ConstructL( aName );
    CleanupStack::Pop( eng );
    return eng;
    }

// ---------------------------------------------------------
// CMenuEng::CMenuEng
// ---------------------------------------------------------
//
CMenuEng::CMenuEng( MMenuEngObserver& aObserver )
: CActive( CActive::EPriorityStandard ), iObserver( aObserver )
    {
    CActiveScheduler::Add( this );
    iLegacyFormat = EFalse;
    }

// ---------------------------------------------------------
// CMenuEng::ConstructL
// ---------------------------------------------------------
//
void CMenuEng::ConstructL( const TDesC& aName )
    {
    ValidateNameL( aName );
    iName.CreateL( aName );
    User::LeaveIfError( iFs.Connect() );
    // Keep temp file and the RAM file name as members -
    // we use these all the time.
    TFileName fname;
    GetFileNameL( fname, ETempFile );
    iTempFileName.CreateL( fname );
    GetFileNameL( fname, ERamFile );
    iRamFileName.CreateL( fname );
    iObjectFactory = CMenuEngObjectFactoryProxy::NewL( *this );
    iEngine = CXCFWEngine::NewL( this );
    iEngine->RegisterObjectFactoryL( iObjectFactory );
    SelfComplete( KErrNone ); 	// Start processing asynchronously.
    ActiveWaitForFileLoadL();	//we need to wait until parsing is complete
    }

// ---------------------------------------------------------
// CMenuEng::QueueOperationL
// ---------------------------------------------------------
//
EXPORT_C void CMenuEng::QueueOperationL( MMenuEngOperation& aOperation )
    {
    __ASSERT_DEBUG( KErrNotFound == iOperations.Find( &aOperation ), \
                    User::Invariant() );
    iOperations.AppendL( &aOperation );
    // If idle, kick back into life.
    if ( EReady == iState )
        {
        iState = EExecuteOp;
        SelfComplete( KErrNone );
        }
    }

// ---------------------------------------------------------
// CMenuEng::DequeueOperation
// ---------------------------------------------------------
//
EXPORT_C void CMenuEng::DequeueOperation( MMenuEngOperation& aOperation )
    {
    if ( iCurrentOperation == &aOperation )
        {
        iCurrentOperation = NULL;
        }
    TInt i = iOperations.Find( &aOperation );
    if ( KErrNotFound != i )
        {
        iOperations.Remove( i );
        }
    }

// ---------------------------------------------------------
// CMenuEng::TraverseFolderL
// ---------------------------------------------------------
//
EXPORT_C void CMenuEng::TraverseFolderL
( TInt aFolder, MMenuEngVisitor& aVisitor ) const
    {
    if ( !iTree )
        {
        User::Leave( KErrNotReady );
        }
    TraverseNodeL( FolderNodeL( aFolder ), aVisitor );
    }

// ---------------------------------------------------------
// CMenuEng::NewObjectL
// ---------------------------------------------------------
//
EXPORT_C CMenuEngObject* CMenuEng::NewObjectL( const TDesC& aType )
    {
    if ( !iIdManager )
        {
        User::Leave( KErrNotReady );
        }
    CMenuEngObject* object = CMenuEngObject::NewL( *this, aType );
    CleanupStack::PushL( object );
    TInt id;
    iIdManager->AllocL( id );
    object->SetId( id );
    CleanupStack::Pop( object );
    return object;
    }

// ---------------------------------------------------------
// CMenuEng::RootFolderL
// ---------------------------------------------------------
//
EXPORT_C void CMenuEng::RootFolderL( TInt& aId ) const
    {
    if ( !iTree )
        {
        User::Leave( KErrNotReady );
        }
    aId = iRoot;
    }

// ---------------------------------------------------------
// CMenuEng::ParentFolderL
// ---------------------------------------------------------
//
EXPORT_C void CMenuEng::ParentFolderL( TInt aId, TInt& aParentId ) const
    {
    if ( !iTree )
        {
        User::Leave( KErrNotReady );
        }
    TInt parentId = 0;
    if ( aId != iRoot )
        {
        // We may have nodes above the root, but do not allow access to those.
        // Say 0 to the root's parent.
        MXCFWNode* parentNode = NodeL( aId ).Parent();
        if ( parentNode )
            {
            parentId = Object( *parentNode ).Id();
            }
        }
    aParentId = parentId;
    }

// ---------------------------------------------------------
// CMenuEng::GetItemL
// ---------------------------------------------------------
//
EXPORT_C void CMenuEng::GetItemL( TInt aId, TMenuItem& aItem ) const
    {
    if ( !iTree )
        {
        User::Leave( KErrNotReady );
        }
    const CMenuEngObject& object = ObjectL( aId );
    aItem.SetId( object.Id() );
    aItem.SetFlags( object.Flags() );
    aItem.SetType( object.Type() );
    TInt parent;
    ParentFolderL( aId, parent );
    aItem.SetParent( parent );
    }

// ---------------------------------------------------------
// CMenuEng::GetItemsL
// ---------------------------------------------------------
//
EXPORT_C void CMenuEng::GetItemsL(
        RArray<TMenuItem>& aItemArray,
        TInt aFolder,
        const MMenuEngFilter* aFilter,
        TBool aRecursive ) const
    {
    if ( !iTree )
        {
        User::Leave( KErrNotReady );
        }
    TMenuEngItemLister lister
        ( *this, aItemArray, aFolder, aFilter, aRecursive );
    TraverseFolderL( aFolder, lister );
    }

// ---------------------------------------------------------
// CMenuEng::ObjectL
// ---------------------------------------------------------
//
EXPORT_C const CMenuEngObject& CMenuEng::ObjectL( TInt aId ) const
    {
    if ( !iTree )
        {
        User::Leave( KErrNotReady );
        }
    return Object( NodeL( aId ) );
    }

// ---------------------------------------------------------
// CMenuEng::GetLegacyFormat
// ---------------------------------------------------------
//
EXPORT_C TBool CMenuEng::GetOnceLegacyFormat()
    {
    TBool format = iLegacyFormat;
    iLegacyFormat = EFalse;
    return format;
    }

// ---------------------------------------------------------
// CMenuEng::RemoveL
// ---------------------------------------------------------
//
EXPORT_C void CMenuEng::RemoveL( TInt aId )
    {
	if ( SysUtil::DiskSpaceBelowCriticalLevelL( &iFs, 0, EDriveC) )
		{
		User::Leave( KErrDiskFull );
		}
    if ( iState != EExecuteOp )
        {
        User::Leave( KErrLocked );
        }
    // Check if aId exists.
    MXCFWNode& node = NodeL( aId );
    // Can't delete the root.
    if ( aId == iRoot )
        {
        User::Leave( KErrAccessDenied );
        }
    // Can't delete read-only items, or folders containing any read-only items.
    TMenuEngDeletionChecker checker;
    TraverseNodeL( node, checker );
    // Get the IDs of items to be deleted.
    RArray<TInt> deletedIds;
    CleanupClosePushL( deletedIds );
    TMenuEngIdCollector idCollector( deletedIds );
    TraverseNodeL( node, idCollector );
    // Add notification event.
    MXCFWNode* parent = node.Parent();
    __ASSERT_DEBUG( parent, User::Invariant() );
    AppendNotifyL( Object( *parent ).Id(),
                   RMenuNotifier::EItemsAddedRemoved );
    // Delete node.
    iTree->RemoveNodeL( &node );
    iChanged = ETrue;
    // Deletion successful, remove the IDs.
    for ( TInt i = 0; i < deletedIds.Count(); i++ )
        {
        iIdManager->Remove( deletedIds[i] );
        }
    CleanupStack::PopAndDestroy( &deletedIds );
    }

// ---------------------------------------------------------
// CMenuEng::MoveToFolderL
// ---------------------------------------------------------
//
EXPORT_C void CMenuEng::MoveToFolderL
( RArray<TInt>& aItems, TInt aFolder, TInt aMoveBefore )
    {
	if ( SysUtil::DiskSpaceBelowCriticalLevelL( &iFs, 0, EDriveC) )
		{
		User::Leave( KErrDiskFull );
		}
    if ( iState != EExecuteOp )
        {
        User::Leave( KErrLocked );
        }
    TInt i;
    TInt id;
    MXCFWNode* fromFolder = NULL;
    // Check that toFolder exists.
    MXCFWNode& toFolder = FolderNodeL( aFolder );
    CMenuEngObject& toFolderObj = Object( toFolder );
    // Check that we can move items into target toFolder.
    if ( toFolderObj.Flags() & TMenuItem::ELockMoveInto )
        {
        User::Leave( KErrAccessDenied );
        }
    // Check that all items exist and they are not locked against moving.
    // Also check that and they are in the same toFolder.
    if ( aItems.Count() )
        {
        fromFolder = NodeL( aItems[0] ).Parent();
        for ( i = 0; i< aItems.Count(); i++ )
            {
            id = aItems[i];
            if ( id == iRoot )
                {
                User::Leave( KErrAccessDenied ); // Can't move the root.
                }
            MXCFWNode& node = NodeL( id );
            if ( Object( node ).Flags() & TMenuItem::ELockItem )
                {
                User::Leave( KErrAccessDenied ); // lock_item
                }
            if ( node.Parent() != fromFolder )
                {
                User::Leave( KErrArgument ); // Items from different folders.
                }
            }
        // Add notification events. Presume that there will be no recursion.
        // If there is recursion, then we make an unnecessary notify, that is
        // better than missing one.
        __ASSERT_DEBUG( fromFolder, User::Invariant() );
        AppendNotifyL( Object( *fromFolder ).Id(),
                       RMenuNotifier::EItemsAddedRemoved );
        AppendNotifyL( toFolderObj.Id(),
                       RMenuNotifier::EItemsAddedRemoved );
        // Do move the items.
        // Recursion check is done by the XML tree (->KErrArgument).
        MXCFWNode* moveBefore = Child( toFolder, aMoveBefore );
        for ( i = 0; i < aItems.Count(); i++ )
            {
            iTree->MoveNodeL( &NodeL( aItems[i] ), &toFolder, moveBefore );
            }
        iChanged = ETrue;
        }
    }

// ---------------------------------------------------------
// CMenuEng::ReorderL
// ---------------------------------------------------------
//
EXPORT_C void CMenuEng::ReorderL( TInt aId, TInt aMoveBefore )
    {
	if ( SysUtil::DiskSpaceBelowCriticalLevelL( &iFs, 0, EDriveC) )
		{
		User::Leave( KErrDiskFull );
		}
    if ( iState != EExecuteOp )
        {
        User::Leave( KErrLocked );
        }
    // Can't reorder the root.
    if ( aId == iRoot )
        {
        User::Leave( KErrArgument );
        }
    // Check if aId exists.
    MXCFWNode& node = NodeL( aId );
    // Check if item can be reordered.
    if ( Object( node ).Flags() & TMenuItem::ELockItem )
        {
        User::Leave( KErrAccessDenied ); // lock_item
        }
    MXCFWNode* parent = node.Parent();
    __ASSERT_DEBUG( parent, User::Invariant() );
    if ( parent )
        {
        // Add notification event.
        AppendNotifyL( Object( *parent ).Id(),
                       RMenuNotifier::EItemsReordered );
        // Move it.
        MXCFWNode* moveBefore = Child( *parent, aMoveBefore );
        iTree->MoveNodeL( &node, parent, moveBefore );
        iChanged = ETrue;
        }
    }

// ---------------------------------------------------------
// CMenuEng::AddL
// ---------------------------------------------------------
//
EXPORT_C void CMenuEng::AddL
( CMenuEngObject& aObject, TInt aFolder, TInt aInsertBefore )
    {
	if ( SysUtil::DiskSpaceBelowCriticalLevelL( &iFs, 0, EDriveC) )
		{
		User::Leave( KErrDiskFull );
		}
    if ( &aObject.Engine() != this )
        {
        // ID integrity requires that objects are bound to engines.
        User::Leave( KErrArgument );
        }
    if ( iState != EExecuteOp )
        {
        User::Leave( KErrLocked );
        }
    if ( KMenuTypeData() == aObject.Type() )
        {
        // Internal type, disallow.
        User::Leave( KErrArgument );
        }
    // Check if aFolder exists.
    MXCFWNode& folder = FolderNodeL( aFolder );
    // Check if folder is locked.
    if ( Object( folder ).Flags() & TMenuItem::ELockMoveInto )
        {
        User::Leave( KErrAccessDenied ); // lock_moveinto
        }
    // Add notification event.
    AppendNotifyL( aFolder, RMenuNotifier::EItemsAddedRemoved );
    // Add the node.
    MXCFWNode* insertBefore = Child( folder, aInsertBefore );
    if ( insertBefore )
        {
        iTree->AddNodeL( &aObject, &folder, insertBefore );
        // No leaving after this point - ownership of aObject taken by the
        // tree. (Well, almost -- CXCFWTree is buggy, but hopefully it will
        // be fixed.)
        }
    else
        {
        // CXCFWTree API design flaw:
        // AddNodeL( &aObject, &folder, NULL ) leaves with KErrArgument!
        iTree->AddNodeL( &aObject, &folder );
        // No leaving after this point - ownership of aObject taken by the
        // tree. (Well, almost -- CXCFWTree is buggy, but hopefully it will
        // be fixed.)
        }
    iChanged = ETrue;
    }

// ---------------------------------------------------------
// CMenuEng::ModifiableObjectL
// ---------------------------------------------------------
//
EXPORT_C CMenuEngObject& CMenuEng::ModifiableObjectL( TInt aId , TInt aEvent)
    {
    if ( !iTree )
        {
        User::Leave( KErrNotReady );
        }
    if ( iState != EExecuteOp )
        {
        User::Leave( KErrLocked );
        }
    MXCFWNode& node = NodeL( aId );
    CMenuEngObject& object = Object( node );
    // Add notification event.
    MXCFWNode* parent = node.Parent();
    __ASSERT_DEBUG( parent, User::Invariant() );
    if( aEvent != RMenuNotifier::EItemsNone )
    	{
    	AppendNotifyL( Object( *parent ).Id(), aEvent );
    	}
    iChanged = ETrue; // Might not be true, but how could we know?
    // Note1: We must presume that this object will actually be changed
    // because we haven't any information what is being done to it!
    // If the object had a back-pointer to the engine, this could
    // be solved, but that would be a BAD THING to do.
    // Note2: other possible solution is to drop this method and invent
    // something else to allow item data update.
    return object;
    }


	
// ---------------------------------------------------------
// CMenuEng::AppendNotifyL
// ---------------------------------------------------------
//
EXPORT_C void CMenuEng::AppendNotifyL( TInt aFolder, TInt aEvents )
    {
    for ( TInt i = 0; i < iNotifyQueue.Count(); i++ )
        {
        if ( iNotifyQueue[i].iFolder == aFolder )
            {
            iNotifyQueue[i].iEvents |= aEvents;
            return;
            }
        }
    iNotifyQueue.AppendL( TMenuEngNotify( aFolder, aEvents ) );
    }

// ---------------------------------------------------------
// CMenuEng::NodeL
// ---------------------------------------------------------
//
EXPORT_C TBool CMenuEng::Exist( TInt aId ) const
    {
    MXCFWNode* node = NULL;
    const RNodeArray& nodes = iTree->Nodes();
    for ( TInt i = 0; i < nodes.Count(); i++ )
        {
        node = nodes[i];
        if ( aId == Object( *node ).Id() )
            {
            return ETrue;
            }
        }
    return EFalse;
    }
// ---------------------------------------------------------
// CMenuEng::RunL
// ---------------------------------------------------------
//
void CMenuEng::RunL()
    {
    switch( iState )
        {
        case ENull:
            {
            // Self-completion in ConstructL(). Load RAM tree.
            __ASSERT_DEBUG( !iStatus.Int(), User::Invariant() );
            iState = ELoadRamFile;
            LoadRamFileL();
            break;
            }

        case ELoadRamFile:
            {
            if ( iStatus.Int() )
                {
                // Error loading RAM tree -> Try ROM tree.
                iObserver.EngineTreeReloaded();
                iState = ELoadRomFile;
                LoadRomFileL();
                }
            else
                {
                // Tree loaded OK. Start processing operations.
                iState = EExecuteOp;
                ExecuteOperationL();
                }
            break;
            }

        case ELoadRomFile:
            {
            // Error loading ROM tree is fatal. Nothing we can do.
            User::LeaveIfError( iStatus.Int() );
            iState = ESaveFile;
            SaveTempFileL();
            break;
            }

        case ESaveFile:
            {
            // Error saving tree is fatal. Nothing we can do.
            User::LeaveIfError( iStatus.Int() );
            // Tree saved to temp file OK. Replace content file with temp file.
            ReplaceRamFileL();
            // Saving completed succesfully.
            iChanged = EFalse;
            // If there is a current operation, this is the final result.
            CompleteCurrentOperation( iStatus.Int() );
            // Start next operation.
            iState = EExecuteOp;
            ExecuteOperationL();
            break;
            }

        case EExecuteOp:
            {
            // Start next operation.
            __ASSERT_DEBUG( !iStatus.Int(), User::Invariant() );
            ExecuteOperationL();
            break;
            }

        case EReady:
        case EDead:
        default:
            {
            User::Invariant();
            }
        }
    }

// ---------------------------------------------------------
// CMenuEng::DoCancel
// ---------------------------------------------------------
//
void CMenuEng::DoCancel()
    {
    // We don't have real requests, only self-completion; which is already
    // completed.
    }

// ---------------------------------------------------------
// CMenuEng::RunError
// ---------------------------------------------------------
//
TInt CMenuEng::RunError( TInt aError )
    {
    switch( iState )
        {
        case ENull:
            {
            User::Invariant(); // Self-completion cannot fail.
            break;
            }

        case ELoadRamFile:
            {
            // Error loading RAM tree -> keep going (try ROM tree).
            SelfComplete( aError );
            break;
            }

        case ELoadRomFile:
            {
            // Error loading ROM tree is fatal. Nothing we can do.
            // stop nested active scheduler loop if error in parsing
            if ( iActiveWait->IsStarted() )
                {
                iActiveWait->AsyncStop();
                }  
                
            iState = EDead;
            iObserver.EngineError( aError );
            // Can't delete the tree now, XCFW Engine keeps a pointer to it
            // and still uses it even after reporting the error. :(
         
            break;
            }

        case ESaveFile:
            {
            // File is too big and cannot be saved, delete it
            // and try reading from rom
            if (iStatus.Int() == KErrNoMemory)
            	{
            	iState = ELoadRamFile;
            	SelfComplete( aError );
            	}
            else {
				// Error saving tree is fatal. Nothing we can do.
				// If there is a current operation, this is the final result.
				CompleteCurrentOperation( aError );
				iState = EDead;
				iObserver.EngineError( aError );
            }
            // Can't delete the tree now, XCFW Engine keeps a pointer to it
            // and still uses it even after reporting the error. :(
            break;
            }

        case EExecuteOp:
            {
            // Current operation failed.
            CompleteCurrentOperation( aError );
            // Continue with next operation.
            SelfComplete( KErrNone );
            break;
            }

        case EReady:
        case EDead:
        default:
            {
            User::Invariant();
            }
        }
    return KErrNone;
    }

// ---------------------------------------------------------
// CMenuEng::HandleEngineEventL
// ---------------------------------------------------------
//
void CMenuEng::HandleEngineEventL( TXCFWEngineEvent aEvent )
    {
    switch ( aEvent )
        {
        case EEvtParsingComplete:
            {
            __ASSERT_DEBUG( iTree, User::Invariant() );

            // parsing operation complete
            // we can stop nested active scheduler loop
            if ( iActiveWait->IsStarted() )
                {
                iActiveWait->AsyncStop();
                }         
            
            TBool legacyFormat = iObjectFactory->IsLegacyFormat();                        
            // Reset object factory in all cases.
            iObjectFactory->Reset();
            // Tree is up (maybe unsaved yet).
            // Check the structure and get the root folder node.
            MXCFWNode& rootNode = CheckTreeL( *iTree );
            // Init ID manager, check and fix ID-s.
            InitIdManagerL( rootNode );
            // Now we have ID for all (including the root).
            iRoot = Object( rootNode ).Id();
            // This is the earliest point to do an ID sanity check.
            __ASSERT_DEBUG( DebugSanityCheck( *iTree ), User::Invariant() );
            // Send a "wildcard" notification so everybody reloads.
            iObserver.EngineEvents( 0, RMenuNotifier::EItemsAddedRemoved );
            iLegacyFormat = legacyFormat;
            SelfComplete( KErrNone ); // Go to RunL().
            break;
            }

        case EEvtSavingComplete:
            {
            //iLegacyFormat = EFalse;
            SelfComplete( KErrNone ); // Go to RunL().
            break;
            }

        case EEvtParsingCanceled:
        case EEvtSavingCanceled:
            {
            // Reset object factory in all cases.
            iObjectFactory->Reset();
            SelfComplete( KErrCancel ); // Go to RunL().
            break;
            }

        case EEvtParsingStarted:
        case EEvtSavingStarted:
        case EEvtNull:
        default:
            {
            break;
            }
        }
    }

// ---------------------------------------------------------
// CMenuEng::HandleEngineErrorL
// ---------------------------------------------------------
//
void CMenuEng::HandleEngineErrorL( TInt aErrorCode )
    {
    // Reset object factory in all cases.
    iObjectFactory->Reset();

    SelfComplete( aErrorCode ); // Go to RunL().
    }

// ---------------------------------------------------------
// CMenuEng::CheckTreeL
// ---------------------------------------------------------
//
MXCFWNode& CMenuEng::CheckTreeL( MXCFWTree& aTree ) const
    {
    MXCFWNode* rootFolder = NULL;
    MXCFWNode* treeRoot = aTree.Root();
    if ( treeRoot )
        {
        // The tree has nodes.
        if ( KMenuTypeData() == Object( *treeRoot ).Type() )
            {
            // Root node is "menu:data"
            RNodeArray nodes;
            CleanupClosePushL( nodes );
            aTree.GetNodesOfTypeL( KMenuTypeFolder(), nodes,
                                    treeRoot, EFalse );
            if ( 1 == nodes.Count() )
                {
                // Exactly 1 "menu:folder" in the root.
                // This is the root folder.
                rootFolder = nodes[0];
                }
            CleanupStack::PopAndDestroy( &nodes );
            }
        }
    if ( !rootFolder )
        {
        User::Leave( KErrCorrupt );
        }
    return *rootFolder;
    }

// ---------------------------------------------------------
// CMenuEng::DebugSanityCheck
// ---------------------------------------------------------
//
TBool CMenuEng::DebugSanityCheck( MXCFWTree& aTree ) const
    {
    const RNodeArray& nodes = aTree.Nodes();
    for ( TInt i = 0; i < nodes.Count(); i++ )
        {
        if ( !DebugSanityCheck( *nodes[i] ) )
            {
            return EFalse;
            }
        }
    return ETrue;
    }

// ---------------------------------------------------------
// CMenuEng::ActiveWaitForLoadFileL
// ---------------------------------------------------------
//
void CMenuEng::ActiveWaitForFileLoadL()
    {
    iActiveWait = new( ELeave ) CActiveSchedulerWait();
    if ( !iActiveWait->IsStarted() )
        {
        iActiveWait->Start();
        }
    //now we check if file was properly loaded
    TInt root(0);
    RootFolderL( root );
    if( !root && ( iState != EDead ) )
    	{
    	//there was problem with Ram file(probably file was empty), lets load Rom file
    	iState = ELoadRamFile;
        if ( !iActiveWait->IsStarted() )
            {
            iActiveWait->Start();
            }
    	}
    }

// ---------------------------------------------------------
// CMenuEng::DebugSanityCheck
// ---------------------------------------------------------
//
TBool CMenuEng::DebugSanityCheck( MXCFWNode& aNode ) const
    {
    __ASSERT_DEBUG( iRoot, User::Invariant() );
    // Sanity check, for node location and ID.
    // - Nodes under the root folder (==items) should have non-0 ID.
    // - Nodes not under the root folder (==other stuff like id_seed) should
    //   always have 0 id.
    // This is crucial for the ID space integrity. XCFW node lookup uses
    // a flat list of nodes; the only way we can differentiate items from
    // non-items is the ID.
    MXCFWNode* node;
    for ( node = &aNode; node; node = node->Parent() )
        {
        if ( Object( *node ).Id() == iRoot )
            {
            // The node is, or descendant of, the root folder. Must have an ID.
            return 0 != Object( aNode ).Id();
            }
        }
    // Node is not under the root folder. Must not have an ID.
    return 0 == Object( aNode ).Id();
    }

// ---------------------------------------------------------
// CMenuEng::NodeL
// ---------------------------------------------------------
//
MXCFWNode& CMenuEng::NodeL( TInt aId ) const
    {
    __ASSERT_DEBUG( iTree, User::Invariant() );
    if ( !aId )
        {
        // We can actually have 0 id node (outside the item tree), but
        // those are not items.
        User::Leave( KErrNotFound );
        }
    MXCFWNode* node = NULL;
    const RNodeArray& nodes = iTree->Nodes();
    for ( TInt i = 0; i < nodes.Count(); i++ )
        {
        node = nodes[i];
        if ( aId == Object( *node ).Id() )
            {
            __ASSERT_DEBUG( DebugSanityCheck( *node ), User::Invariant() );
            return *node;
            }
        }
    User::Leave( KErrNotFound );
    /* NOTREACHED */
    return *node;
    }

// ---------------------------------------------------------
// CMenuEng::FolderNodeL
// ---------------------------------------------------------
//
MXCFWNode& CMenuEng::FolderNodeL( TInt aId ) const
    {
    __ASSERT_DEBUG( iTree, User::Invariant() );
    MXCFWNode& node = NodeL( aId );
    if ( KMenuTypeFolder() == Object( node ).Type() )
        {
        return node;
        }
    User::Leave( KErrNotFound );
    /* NOTREACHED */
    return node;
    }

// ---------------------------------------------------------
// CMenuEng::Child
// ---------------------------------------------------------
//

MXCFWNode* CMenuEng::Child( MXCFWNode& aParent, TInt aId )
    {
    MXCFWNode* node;
    for ( node = aParent.FirstChild(); node; node = node->NextSibling() )
        {
        if ( Object( *node ).Id() == aId )
            {
            break;
            }
        }
    return node;
    }

// ---------------------------------------------------------
// CMenuEng::CancelAllOperations
// ---------------------------------------------------------
//
void CMenuEng::CancelAllOperations()
    {
    // For safety, we remove pointers before completion.
    // (OperationCompleted might want to remove operation too, avoid
    // conflict.)
    MMenuEngOperation* operation;
    if ( iCurrentOperation )
        {
        operation = iCurrentOperation;
        iCurrentOperation = NULL;
        operation->CompletedMenuEngOperation( KErrCancel );
        }
    while ( iOperations.Count() )
        {
        operation = iOperations[0];
        iOperations.Remove( 0 );
        operation->CompletedMenuEngOperation( KErrCancel );
        }
    }

// ---------------------------------------------------------
// CMenuEng::ValidateNameL
// ---------------------------------------------------------
//
void CMenuEng::ValidateNameL( const TDesC& aName )
    {
    // We only accept alphanumeric characters and underscore as content
    // file name. This is stricter than the (current) file system
    // restriction, for future-proofing reasons.
    if ( !aName.Length() )
        {
        User::Leave( KErrArgument );
        }
    for ( TInt i = 0; i < aName.Length(); i++ )
        {
        TChar c =  aName[i];
        if ( !c.IsAlphaDigit() && '_' != c )
            {
            User::Leave( KErrArgument );
            }
        }
    }

// ---------------------------------------------------------
// CMenuEng::GetFileNameL
// ---------------------------------------------------------
//
void CMenuEng::GetFileNameL( TFileName& aFname, TFile aSelector )
    {
    User::LeaveIfError( iFs.PrivatePath( aFname ) );
    if ( ERomFile == aSelector )
        {
        InsertL(
            aFname,
            0,
            PathInfo::RomRootPath().Left( KDriveAndColon ) );
        }
    else
        {
        InsertL(
            aFname,
            0,
            PathInfo::PhoneMemoryRootPath().Left( KDriveAndColon ) );
        }
    AppendL( aFname, KMenuContentDirName );
    AppendL( aFname, iName );
    AppendL( aFname, ETempFile == aSelector ?
        KMenuTempExtension : KMenuContentExtension );
    }

// ---------------------------------------------------------
// CMenuEng::TraverseNodeL
// ---------------------------------------------------------
//
TBool CMenuEng::TraverseNodeL
( MXCFWNode& aNode, MMenuEngVisitor& aVisitor ) const
    {
    CMenuEngObject& object = Object( aNode );
    if ( KMenuTypeFolder() == object.Type() )
        {
        if ( aVisitor.VisitEnterL( object ) )
            {
            // Recursion is used - menu data tree is not very deep.
            MXCFWNode* node = aNode.FirstChild();
            while( node && TraverseNodeL( *node, aVisitor ) )
                {
                node = node->NextSibling();
                }
            }
        // countChildren( object );
        return aVisitor.VisitLeaveL( object );
        }
    else
        {
        return aVisitor.VisitL( object );
        }
    }

// ---------------------------------------------------------
// CMenuEng::InitIdManagerL
// ---------------------------------------------------------
//
void CMenuEng::InitIdManagerL( MXCFWNode& aRootNode )
    {
    __ASSERT_DEBUG( !iIdManager, User::Invariant() );
    __ASSERT_DEBUG( iTree, User::Invariant() );
    iIdManager = new (ELeave) CMenuEngIdManager();
    TInt idSeed;
    GetIdSeedL( idSeed );
    iIdManager->SetSeed( idSeed );
    // Read ID-s to ID manager.
    TMenuEngIdManagerInit idManagerInit( *iIdManager );
    TraverseNodeL( aRootNode, idManagerInit );
    // Make sure all nodes have ID.
    TMenuEngIdSetter idSetter( *iIdManager );
    TraverseNodeL( aRootNode, idSetter ); 
    }

// ---------------------------------------------------------
// CMenuEng::GetIdSeedL
// ---------------------------------------------------------
//
void CMenuEng::GetIdSeedL( TInt& aIdSeed )
    {
    TInt seed = 0;
    __ASSERT_DEBUG( iTree, User::Invariant() );
    MXCFWNode* root = iTree->Root();
    if ( root )
        {
        RNodeArray nodes;
        CleanupClosePushL( nodes );
        iTree->GetNodesOfTypeL
            ( KMenuTypeIdSeed(), nodes, root, EFalse );
        if ( nodes.Count() )
            {
            const CMenuEngObject& object = Object( *nodes[0] );
            TPtrC val;
            TBool dummy;
            if ( object.FindAttribute( KMenuAttrIdSeed(), val, dummy ) )
                {
                seed = MenuEngId::AsInt( val );
                }
            }
        CleanupStack::PopAndDestroy( &nodes );
        }
    aIdSeed = seed;
    }

// ---------------------------------------------------------
// CMenuEng::SetIdSeedL
// ---------------------------------------------------------
//
void CMenuEng::SetIdSeedL( TInt aSeed )
    {
    __ASSERT_DEBUG( iTree, User::Invariant() );
    MXCFWNode* root = iTree->Root();
    if ( root )
        {
        CMenuEngObject* object = NULL;
        RNodeArray nodes;
        CleanupClosePushL( nodes );
        iTree->GetNodesOfTypeL
            ( KMenuTypeIdSeed(), nodes, root, EFalse );
        if ( nodes.Count() )
            {
            object = &Object( *nodes[0] );
            }
        else
            {
            // Outside of the item tree -> no ID set.
            object = CMenuEngObject::NewL( *this, KMenuTypeIdSeed() );
            CleanupStack::PushL( object );
            iTree->AddNodeL( object, root );
            CleanupStack::Pop( object );
            }
        __ASSERT_DEBUG( object, User::Invariant() );
        TBuf<KMenuMaxAttrValueLen> buf;
        MenuEngId::AsString( aSeed, buf );
        object->SetAttributeL( KMenuAttrIdSeed(), buf, EFalse );
        CleanupStack::PopAndDestroy( &nodes );
        }
    }

// ---------------------------------------------------------
// CMenuEng::SelfComplete
// ---------------------------------------------------------
//
void CMenuEng::SelfComplete( TInt aError )
    {
    TRequestStatus* ownStatus = &iStatus;
    *ownStatus = KRequestPending;
    SetActive();
    User::RequestComplete( ownStatus, aError );
    }

// ---------------------------------------------------------
// CMenuEng::LoadRamFileL
// ---------------------------------------------------------
//
void CMenuEng::LoadRamFileL()
    {
    __ASSERT_DEBUG( ELoadRamFile == iState, User::Invariant() );
    __ASSERT_DEBUG( !iTree, User::Invariant() );
    iTree = CXCFWTree::NewL();
    // Legacy xml format supported only if the xml is from rom:
    iObjectFactory->SupportLegacyFormat( EFalse ); 
    iEngine->LoadL( *iTree, iRamFileName );
    }

// ---------------------------------------------------------
// CMenuEng::LoadRomFileL
// ---------------------------------------------------------
//
void CMenuEng::LoadRomFileL()
    {
    __ASSERT_DEBUG( ELoadRomFile == iState, User::Invariant() );
    // ROM file name is not kept as member - it is used only once.
    TFileName fname;
    GetFileNameL( fname, ERomFile );
	delete iIdManager; iIdManager = NULL;
    delete iTree; iTree = NULL;
    iTree = CXCFWTree::NewL();
    // Legacy xml format supported only if the xml is from rom:
    iObjectFactory->SupportLegacyFormat( ETrue ); 
    iEngine->LoadL( *iTree, fname );
    }

// ---------------------------------------------------------
// CMenuEng::SaveTempFileL
// ---------------------------------------------------------
//
void CMenuEng::SaveTempFileL()
    {
    __ASSERT_DEBUG( ESaveFile == iState, User::Invariant() );
    __ASSERT_DEBUG( iTree, User::Invariant() );
    __ASSERT_DEBUG( iIdManager, User::Invariant() );
    // Write back ID seed.
    SetIdSeedL( iIdManager->Seed() );
    // Save to temp file then replace content file with temp file.
    // This avoids having incomplete content file, if something goes wrong.
    __ASSERT_DEBUG( DebugSanityCheck( *iTree ), User::Invariant() );
    TRAPD( err, iEngine->SaveL( *iTree, iTempFileName ) );
    if( err==KErrDiskFull )
        {
        iDiskWasFullAndRamFileWasNotCreated = ETrue;
        SelfComplete( KErrNone );
        }
    else
        {
        iDiskWasFullAndRamFileWasNotCreated = EFalse;
        User::LeaveIfError( err );
        }

    }

// ---------------------------------------------------------
// CMenuEng::ReplaceRamFileL
// ---------------------------------------------------------
//
void CMenuEng::ReplaceRamFileL()
    {
    __ASSERT_DEBUG( ESaveFile == iState, User::Invariant() );
    // RFs::Replace() copies the file data ->
    // Delete() + Rename() is used instead.
    iFs.Delete( iRamFileName );
    TInt err = iFs.Rename( iTempFileName, iRamFileName );
    if( err==KErrDiskFull )
        {
        iDiskWasFullAndRamFileWasNotCreated = ETrue;
        }
    else
        {
        iDiskWasFullAndRamFileWasNotCreated = EFalse;
        User::LeaveIfError( err );
        }
    }

// ---------------------------------------------------------
// CMenuEng::CompleteCurrentOperation
// ---------------------------------------------------------
//
void CMenuEng::CompleteCurrentOperation( TInt aError )
    {
    // Operation completion reported two ways:
    // - "No change" operations report completion immediately;
    // - "Change" operations trigger saving and wait the result.
    __ASSERT_DEBUG( EExecuteOp == iState || ESaveFile == iState, \
        User::Invariant() );
    if ( iCurrentOperation )
        {
        iCurrentOperation->CompletedMenuEngOperation( aError );
        iCurrentOperation = NULL;
        }
    // Flush notifications in the success case.
    // (Otherwise, the changes did not happen.)
    if ( !aError )
        {
        for ( TInt i = 0; i < iNotifyQueue.Count(); i++ )
            {
            const TMenuEngNotify& notify = iNotifyQueue[i];
            iObserver.EngineEvents( notify.iFolder, notify.iEvents );
            }
        }
    iNotifyQueue.Reset();
    }

// ---------------------------------------------------------
// CMenuEng::ExecuteOperationL
// ---------------------------------------------------------
//
void CMenuEng::ExecuteOperationL()
    {
    __ASSERT_DEBUG( EExecuteOp == iState, User::Invariant() );
    __ASSERT_DEBUG( !iCurrentOperation, User::Invariant() );
    if ( iOperations.Count() )
        {
        iCurrentOperation = iOperations[0];
        iOperations.Remove( 0 );
        iCurrentOperation->RunMenuEngOperationL();
        SaveChangesL();
        }
    else
        {
        // No pending operations, we stop here.
        // Next QueueOperationL will kick us back to operation.
        iState = EReady;
        }
    }

// ---------------------------------------------------------
// CMenuEng::SaveChangesL
// ---------------------------------------------------------
//
void CMenuEng::SaveChangesL()
    {
    // This method is part of ExecuteOperationL.
    // (Post-operation processing.)
    __ASSERT_DEBUG( EExecuteOp == iState, User::Invariant() );
    if ( iChanged || iDiskWasFullAndRamFileWasNotCreated )
        {
        // Tree changed, async save.
        iState = ESaveFile;
        SaveTempFileL();
        }
    else
        {
        // Tree not changed, we are done.
        CompleteCurrentOperation( KErrNone );
        if ( iOperations.Count() )
            {
            // Go for the next operation (async).
            iState = EExecuteOp; // Same as current value, code clarity.
            SelfComplete( KErrNone );
            }
        else
            {
            // No pending operations, we stop here.
            // Next QueueOperationL will kick us back to operation.
            iState = EReady;
            }
        }
    }

//  End of File  
