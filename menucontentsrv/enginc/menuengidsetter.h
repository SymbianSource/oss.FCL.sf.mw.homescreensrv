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

#ifndef __MENUENGIDSETTER_H__
#define __MENUENGIDSETTER_H__

// INCLUDES

#include <e32base.h>
#include "menuengvisitor.h"

// FORWARD DECLARATION

class CMenuEngIdManager;

// CLASS DECLARATION

/**
* Menu Engine ID setter - recurse the menu tree and, allocate and set ID to
* all objects that does not have one.
*/
NONSHARABLE_CLASS( TMenuEngIdSetter ): public MMenuEngVisitor
    {

public:     // construction

    /**
    * Constructor.
    * @param aIdManager Ids are allocated from this manager.
    */
    TMenuEngIdSetter( CMenuEngIdManager& aIdManager )
        : iIdManager( aIdManager ) {}

public:     // new methods

    /**
    * Enter folder.
    * @param aFolder Entering this folder.
    * @return ETrue (visit children of this folder).
    */
    TBool VisitEnterL( CMenuEngObject& aFolder )
        { SetIdL( aFolder ); return ETrue; }

    /**
    * Visit leaf node.
    * @param aLeaf Leaf node.
    * @return ETrue (visit next sibling of this leaf).
    */
    TBool VisitL( CMenuEngObject& aLeaf )
        { SetIdL( aLeaf ); return ETrue; }

private:    // new methods

    /**
    * Set ID from for this object.
    * @param aObject Object.
    */
    void SetIdL( CMenuEngObject& aObject );

private:    // data

    CMenuEngIdManager& iIdManager; ///< Id manager.

    };

#endif // __MENUENGIDSETTER_H__
