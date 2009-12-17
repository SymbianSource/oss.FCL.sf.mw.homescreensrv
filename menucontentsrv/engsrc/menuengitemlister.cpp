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

#include "menuengitemlister.h"
#include "menuengfilter.h"
#include "menuengobject.h"
#include "menueng.h"

// ================= MEMBER FUNCTIONS =======================

// ---------------------------------------------------------
// TMenuEngItemLister::TMenuEngItemLister
// ---------------------------------------------------------
//
TMenuEngItemLister::TMenuEngItemLister( 
        const CMenuEng& aEng,
        RArray<TMenuItem>& aItemArray,
        TInt aFolder,
        const MMenuEngFilter* aFilter,
        TBool aRecursive )
: iEng( aEng ),
  iItemArray( aItemArray ),
  iFolder( aFolder ),
  iFilter( aFilter ),
  iRecursive( aRecursive )
    {
    }

// ---------------------------------------------------------
// TMenuEngItemLister::VisitEnterL
// ---------------------------------------------------------
//
TBool TMenuEngItemLister::VisitEnterL( CMenuEngObject& aFolder )
    {
    if ( iFolder == aFolder.Id() )
        {
        // We need content below this folder (but not the folder itself).
        return ETrue;
        }
    
    // A folder below iFolder - add and (optionally) recurse.
    AddL( aFolder );
    return iRecursive;
    }

// ---------------------------------------------------------
// TMenuEngItemLister::VisitLeaveL
// ---------------------------------------------------------
//
TBool TMenuEngItemLister::VisitLeaveL( CMenuEngObject& /*aFolder*/ )
    {
    return ETrue;
    }

// ---------------------------------------------------------
// TMenuEngItemLister::VisitL
// ---------------------------------------------------------
//
TBool TMenuEngItemLister::VisitL( CMenuEngObject& aLeaf )
    {
    AddL( aLeaf );
    return ETrue;
    }

// ---------------------------------------------------------
// TMenuEngItemLister::AddL
// ---------------------------------------------------------
//
void TMenuEngItemLister::AddL( CMenuEngObject& aObject )
    {
    if ( !iFilter || iFilter->MatchesObject( aObject ) )
        {
        TMenuItem hdr;
        hdr.SetId( aObject.Id() );
        hdr.SetFlags( aObject.Flags() );
        hdr.SetType( aObject.Type() );
        TInt parent;
        iEng.ParentFolderL( aObject.Id(), parent );
        hdr.SetParent( parent );
        iItemArray.AppendL( hdr );
        }
    }

//  End of File  
