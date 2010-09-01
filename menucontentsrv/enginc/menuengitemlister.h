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

#ifndef __MENUENGITEMLISTER_H__
#define __MENUENGITEMLISTER_H__

#include "menuengvisitor.h"
#include "mcsmenuitem.h"

#include <e32base.h>

// FORWARD DECLARATION

class CMenuEng;
class MMenuEngFilter;

// CLASS DECLARATION

/**
* Menu Engine item lister.
* Creates a list of item headers.
*/
NONSHARABLE_CLASS( TMenuEngItemLister ): public MMenuEngVisitor
    {

public:     // construction

    /**
    * Constructor.
    * @param aEng Engine.
    * @param aItemArray Items are appended to this list.
    * @param aFolder Root folder of query (will be excluded from the listing).
    * @return aFilter Filter criteria or NULL.
    * @param aRecursive ETrue for recursive listing.
    */
    TMenuEngItemLister( 
        const CMenuEng& aEng,
        RArray<TMenuItem>& aItemArray,
        TInt aFolder,
        const MMenuEngFilter* aFilter,
        TBool aRecursive );

public:     // new methods

	// From MMenuEngVisitor:
    /**
    * Enter folder.
    * @param aFolder Entering this folder.
    * @return ETrue to visit children of this folder. VisitLeaveL() will
    *         be called after children of this folder are visited.
    *         EFalse to skip children of this folder. VisitLeaveL() will
    *         be called next.
    */
    TBool VisitEnterL( CMenuEngObject& aFolder );

    /**
    * Leave folder.
    * @param aFolder Leaving this folder.
    * @return ETrue to visit next sibling of this folder.
    *         EFalse to stop visiting items at this level.
    */
    TBool VisitLeaveL( CMenuEngObject& aFolder );

    /**
    * Visit leaf node.
    * @param aLeaf Leaf node.
    * @return ETrue to visit next sibling of this leaf.
    *         EFalse to stop visiting items at this level.
    */
    TBool VisitL( CMenuEngObject& aLeaf );

private:    // new methods

    void AddL( CMenuEngObject& aObject );

private:    // data

    const CMenuEng& iEng; ///< Engine. Not own.
    RArray<TMenuItem>& iItemArray; ///< Item array. Not own.
    TInt iFolder; ///< Folder of query (excluded from the listing).
    const MMenuEngFilter* iFilter; ///< Filter or NULL. Not own.
    TBool iRecursive; ///< ETrue for recursive listing.

    };

#endif // __MENUENGITEMLISTER_H__
