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

#ifndef __MENUENGIDMANAGERINIT_H__
#define __MENUENGIDMANAGERINIT_H__

// INCLUDES

#include <e32base.h>
#include "menuengvisitor.h"

// FORWARD DECLARATION

class CMenuEngIdManager;

// CLASS DECLARATION

/**
* Menu Engine ID manager initialization - recurse the menu tree and collect
* all used ID-s from objects, into the ID manager.
*/
NONSHARABLE_CLASS( TMenuEngIdManagerInit ): public MMenuEngVisitor
    {

public:     // construction

    /**
    * Constructor.
    * @param aIdManager Ids are collected into this manager.
    */
    TMenuEngIdManagerInit( CMenuEngIdManager& aIdManager )
        : iIdManager( aIdManager ) {}

public:     // new methods

    /**
    * Enter folder.
    * @param aFolder Entering this folder.
    * @return ETrue (visit children of this folder).
    */
    TBool VisitEnterL( CMenuEngObject& aFolder )
        { AddIdL( aFolder ); return ETrue; }

    /**
    * Visit leaf node.
    * @param aLeaf Leaf node.
    * @return ETrue (visit next sibling of this leaf).
    */
    TBool VisitL( CMenuEngObject& aLeaf )
        { AddIdL( aLeaf ); return ETrue; }

private:    // new methods

    /**
    * Add ID from object.
    * @param aObject Object.
    */
    void AddIdL( CMenuEngObject& aObject );

private:    // data

    CMenuEngIdManager& iIdManager; ///< Id manager.

    };

#endif // __MENUENGIDMANAGERINIT_H__
