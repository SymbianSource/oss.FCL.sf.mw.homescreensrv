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

#ifndef __MENUENGIDMANAGER_H__
#define __MENUENGIDMANAGER_H__

// INCLUDES

#include <e32base.h>

// CLASS DECLARATION

/**
* Menu Engine ID manager - providing unique ID-s.
*/
NONSHARABLE_CLASS( CMenuEngIdManager ): public CBase
    {

public:     // construction

    /**
    * Destructor.
    */
    virtual ~CMenuEngIdManager();

    /**
    * Constructor.
    */
    CMenuEngIdManager();

public:     // new methods

    /**
    * Set the seed.
    * @param aSeed ID seed. Any value is suitable (does not effect
    * ID uniqueness). However, the seed is persisted between runs,
    * it provides better ID distribution (returned ID-s are reused later).
    */
    void SetSeed( TInt aSeed ) { iSeed = aSeed; }

    /**
    * Get the seed.
    * @return Seed.
    */
    TInt Seed() const { return iSeed; }

public:     // new methods

    /**
    * Generate and allocate new ID.
    * @param aId ID
    */
    void AllocL( TInt& aId );

    /**
    * Store existing ID (mark it as used).
    * No sanity check - ID-s can be added more than once!
    * @param aId ID.
    */
    void AddL( TInt aId ); // Store existing ID (==mark as used)

    /**
    * Free ID (mark it as unused). Safe to call if aId is not used.
    * @param aId ID.
    */
    void Remove( TInt aId ); // Free ID (==mark as unused)

private:    // data

    RArray<TInt> iIds; ///< Id array. Own.
    TInt iSeed; ///< Seed.

    };

#endif // __MENUENGIDMANAGER_H__
