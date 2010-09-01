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

#ifndef __MENUENGDELETIONCHECKER_H__
#define __MENUENGDELETIONCHECKER_H__

// INCLUDES

#include <e32base.h>
#include "menuengvisitor.h"

// CLASS DECLARATION

/**
* Menu Engine deletion checker - recurse a directory to check if there are
* any read-only items (lock_delete). Leave with KErrAccessDenied if yes.
*/
NONSHARABLE_CLASS( TMenuEngDeletionChecker ): public MMenuEngVisitor
    {

public:     // new methods

    /**
    * Enter folder.
    * @param aFolder Entering this folder.
    * @return ETrue (visit children of this folder).
    */
    TBool VisitEnterL( CMenuEngObject& aFolder )
        { CheckL( aFolder ); return ETrue; }

    /**
    * Visit leaf node.
    * @param aLeaf Leaf node.
    * @return ETrue (visit next sibling of this leaf).
    */
    TBool VisitL( CMenuEngObject& aLeaf )
        { CheckL( aLeaf ); return ETrue; }

private:    // new methods

    /**
    * Check node. Leave with KErrAccessDenied if read-only.
    * @param aObject Object.
    */
    void CheckL( CMenuEngObject& aObject );

    };

#endif // __MENUENGDELETIONCHECKER_H__
