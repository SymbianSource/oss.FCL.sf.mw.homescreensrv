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
#include "menusrvoperation.h"
#include "menuitemattr.h"
#include "menuutil.h"
#include "menueng.h"
#include "menuengobject.h"

// ================= MEMBER FUNCTIONS =======================

// ---------------------------------------------------------
// CMenuSrvOperation::CMenuSrvOperation()
// ---------------------------------------------------------
//
CMenuSrvOperation::CMenuSrvOperation( CMenuEng& aEng )
: iEng( aEng )
    {
    }

// ---------------------------------------------------------
// CMenuSrvOperation::StartL()
// ---------------------------------------------------------
//
void CMenuSrvOperation::StartL( const RMessage2& aMessage )
    {
    PanicIfPendingL( aMessage );
    iEng.QueueOperationL( *this );
    SetPending( aMessage ); // No leaving after this point!
    }

// ---------------------------------------------------------
// CMenuSrvOperation::Cancel()
// ---------------------------------------------------------
//
void CMenuSrvOperation::Cancel()
    {
    iEng.DequeueOperation( *this );
    Complete( KErrCancel );
    }

// ---------------------------------------------------------
// CMenuSrvOperation::CompletedMenuEngOperation()
// ---------------------------------------------------------
//
void CMenuSrvOperation::CompletedMenuEngOperation( TInt aErr )
    {
    iEng.DequeueOperation( *this );
    Complete( aErr );
    }

// ================= MEMBER FUNCTIONS =======================

// ---------------------------------------------------------
// CMenuSrvRemoveOperation::CMenuSrvRemoveOperation
// ---------------------------------------------------------
//
CMenuSrvRemoveOperation::CMenuSrvRemoveOperation( CMenuEng& aEng, TInt aId )
: CMenuSrvOperation( aEng ), iId( aId )
    {
    }

// ---------------------------------------------------------
// CMenuSrvRemoveOperation::RunMenuEngOperationL
// ---------------------------------------------------------
//
void CMenuSrvRemoveOperation::RunMenuEngOperationL()
    {
    iEng.RemoveL( iId );
    }

// ================= MEMBER FUNCTIONS =======================

// ---------------------------------------------------------
// CMenuSrvMoveToFolderOperation::~CMenuSrvMoveToFolderOperation
// ---------------------------------------------------------
//
CMenuSrvMoveToFolderOperation::~CMenuSrvMoveToFolderOperation()
    {
    iItems.Close();
    }

// ---------------------------------------------------------
// CMenuSrvMoveToFolderOperation::NewL()
// ---------------------------------------------------------
//
CMenuSrvMoveToFolderOperation* CMenuSrvMoveToFolderOperation::NewL(
        CMenuEng& aEng,
        RReadStream& aItems,
        TInt aFolder,
        TInt aMoveBefore )
    {
    CMenuSrvMoveToFolderOperation* op =
        new (ELeave) CMenuSrvMoveToFolderOperation
        ( aEng, aFolder, aMoveBefore );
    CleanupStack::PushL( op );
    op->ConstructL( aItems );
    CleanupStack::Pop( op );
    return op;
    }

// ---------------------------------------------------------
// CMenuSrvMoveToFolderOperation::CMenuSrvMoveToFolderOperation
// ---------------------------------------------------------
//
CMenuSrvMoveToFolderOperation::CMenuSrvMoveToFolderOperation
( CMenuEng& aEng, TInt aFolder, TInt aMoveBefore )
: CMenuSrvOperation( aEng ), iFolder( aFolder ), iMoveBefore( aMoveBefore )
    {
    }

// ---------------------------------------------------------
// CMenuSrvMoveToFolderOperation::ConstructL()
// ---------------------------------------------------------
//
void CMenuSrvMoveToFolderOperation::ConstructL( RReadStream& aItems )
    {
    MenuUtil::InternalizeL( iItems, aItems );
    }

// ---------------------------------------------------------
// CMenuSrvMoveToFolderOperation::RunMenuEngOperationL
// ---------------------------------------------------------
//
void CMenuSrvMoveToFolderOperation::RunMenuEngOperationL()
    {
    iEng.MoveToFolderL( iItems, iFolder, iMoveBefore );
    }

// ================= MEMBER FUNCTIONS =======================

// ---------------------------------------------------------
// CMenuSrvReorderOperation::CMenuSrvReorderOperation
// ---------------------------------------------------------
//
CMenuSrvReorderOperation::CMenuSrvReorderOperation
( CMenuEng& aEng, TInt aId, TInt aMoveBefore )
: CMenuSrvOperation( aEng ), iId( aId ), iMoveBefore( aMoveBefore )
    {
    }

// ---------------------------------------------------------
// CMenuSrvReorderOperation::RunMenuEngOperationL
// ---------------------------------------------------------
//
void CMenuSrvReorderOperation::RunMenuEngOperationL()
    {
    iEng.ReorderL( iId, iMoveBefore );
    }

// ================= MEMBER FUNCTIONS =======================

// ---------------------------------------------------------
// CMenuSrvAddOperation::~CMenuSrvAddOperation
// ---------------------------------------------------------
//
CMenuSrvAddOperation::~CMenuSrvAddOperation()
    {
    delete iObject;
    }

// ---------------------------------------------------------
// CMenuSrvAddOperation::NewL()
// ---------------------------------------------------------
//
CMenuSrvAddOperation* CMenuSrvAddOperation::NewL
( CMenuEng& aEng, const TDesC& aType, RReadStream& aChanges )
    {
    CMenuSrvAddOperation* op = new (ELeave) CMenuSrvAddOperation( aEng );
    CleanupStack::PushL( op );
    op->ConstructL( aType, aChanges );
    CleanupStack::Pop( op );
    return op;
    }

// ---------------------------------------------------------
// CMenuSrvAddOperation::CMenuSrvAddOperation
// ---------------------------------------------------------
//
CMenuSrvAddOperation::CMenuSrvAddOperation( CMenuEng& aEng )
: CMenuSrvOperation( aEng )
    {
    }

// ---------------------------------------------------------
// CMenuSrvAddOperation::ConstructL()
// ---------------------------------------------------------
//
void CMenuSrvAddOperation::ConstructL( const TDesC& aType, RReadStream& aChanges )
    {
    iObject = iEng.NewObjectL( aType );
    // Flags.
    const TUint32 flags = aChanges.ReadUint32L();
    // Attributes.
    RMenuItemAttrArray attrChanges;
    CleanupResetAndDestroyPushL( attrChanges );
    attrChanges.InternalizeL( aChanges );
    // Parent folder and insertion point.
    iParent = aChanges.ReadInt32L();
    iInsertBefore = aChanges.ReadInt32L();
    // Flag and attribute data is stored inside the object.
    iObject->SetFlags( flags, ETrue );
    for ( TInt i = 0; i < attrChanges.Count(); i++ )
        {
        const CMenuItemAttr* attr = attrChanges[i];
        if ( attr->Value() )
            {
            iObject->SetAttributeL( attr->Name(), *attr->Value(), EFalse );
            }
        else
            {
            ; // No use trying to remove attribute: this is a new object.
            }
        }
    CleanupStack::PopAndDestroy( &attrChanges ); // Ownership NOT passed.        
    }

// ---------------------------------------------------------
// CMenuSrvAddOperation::ObjectId
// ---------------------------------------------------------
//
TInt CMenuSrvAddOperation::ObjectId()
    {
    __ASSERT_DEBUG( iObject, User::Invariant() );
    return iObject->Id();
    }

// ---------------------------------------------------------
// CMenuSrvAddOperation::RunMenuEngOperationL
// ---------------------------------------------------------
//
void CMenuSrvAddOperation::RunMenuEngOperationL()
    {
    __ASSERT_DEBUG( iObject, User::Invariant() );
    iEng.AddL( *iObject, iParent, iInsertBefore );
    iObject = NULL; // Ownership passed to engine.
    }

// ================= MEMBER FUNCTIONS =======================

// ---------------------------------------------------------
// CMenuSrvUpdateOperation::~CMenuSrvUpdateOperation
// ---------------------------------------------------------
//
CMenuSrvUpdateOperation::~CMenuSrvUpdateOperation()
    {
    iAttrChanges.ResetAndDestroy();
    }

// ---------------------------------------------------------
// CMenuSrvUpdateOperation::NewL()
// ---------------------------------------------------------
//
CMenuSrvUpdateOperation* CMenuSrvUpdateOperation::NewL
( CMenuEng& aEng, TInt aId, RReadStream& aChanges )
    {
    CMenuSrvUpdateOperation* op =
        new (ELeave) CMenuSrvUpdateOperation( aEng, aId );
    CleanupStack::PushL( op );
    op->ConstructL( aChanges );
    CleanupStack::Pop( op );
    return op;
    }

// ---------------------------------------------------------
// CMenuSrvUpdateOperation::CMenuSrvUpdateOperation
// ---------------------------------------------------------
//
CMenuSrvUpdateOperation::CMenuSrvUpdateOperation( CMenuEng& aEng, TInt aId )
: CMenuSrvOperation( aEng ), iId( aId )
    {
    }

// ---------------------------------------------------------
// CMenuSrvUpdateOperation::ConstructL()
// ---------------------------------------------------------
//
void CMenuSrvUpdateOperation::ConstructL( RReadStream& aChanges )
    {
    iFlags = aChanges.ReadUint32L();
    iChangedFlags = aChanges.ReadUint32L();
    iAttrChanges.InternalizeL( aChanges );
    }

// ---------------------------------------------------------
// CMenuSrvUpdateOperation::RunMenuEngOperationL
// ---------------------------------------------------------
//
void CMenuSrvUpdateOperation::RunMenuEngOperationL()
    {
    CMenuEngObject& object = iEng.ModifiableObjectL( iId );
    object.SetFlags( iFlags & iChangedFlags, ETrue );
    object.SetFlags( ~iFlags & iChangedFlags, EFalse );
    for ( TInt i = 0; i < iAttrChanges.Count(); i++ )
        {
        const CMenuItemAttr* attr = iAttrChanges[i];
        if ( attr->Value() )
            {
            object.SetAttributeL( attr->Name(), *attr->Value(), EFalse );
            }
        else
            {
            object.RemoveAttribute( attr->Name() );
            }
        }
    }

//  End of File  
