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

#ifndef __MENUENGIDCOLLECTOR_H__
#define __MENUENGIDCOLLECTOR_H__

// INCLUDES

#include <e32base.h>
#include "menuengvisitor.h"

// FORWARD DECLARATION

class CMenuEngIdManager;

// CLASS DECLARATION

/**
* Menu Engine ID collector - recurse the menu tree and collect all
* used ID-s, into an array.
*/
NONSHARABLE_CLASS( TMenuEngIdCollector ): public MMenuEngVisitor
    {

public:     // construction

    /**
    * Constructor.
    * @param aIds Ids are appended to this array.
    */
    TMenuEngIdCollector( RArray<TInt>& aIds ): iIds( aIds ) {}

public:     // new methods

    /**
    * Enter folder.
    * @param aFolder Entering this folder.
    * @return ETrue (visit children of this folder).
    */
    TBool VisitEnterL( CMenuEngObject& aFolder )
        { CollectIdL( aFolder ); return ETrue; }

    /**
    * Visit leaf node.
    * @param aLeaf Leaf node.
    * @return ETrue (visit next sibling of this leaf).
    */
    TBool VisitL( CMenuEngObject& aLeaf )
        { CollectIdL( aLeaf ); return ETrue; }

private:    // new methods

    /**
    * Collect ID from object.
    * @param aObject Object.
    */
    void CollectIdL( CMenuEngObject& aObject );

private:    // data

    RArray<TInt>& iIds; ///< Id array. Not owned.

    };

#endif // __MENUENGIDCOLLECTOR_H__
