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

#ifndef __MENUSRVMMCHISTORY_H__
#define __MENUSRVMMCHISTORY_H__

#include <e32base.h>

// FORWARD DECLARATION

class CMenuEng;
class RFs;

/**
* MMC History.
* Stores a fixed number of MMC ids. When new ID-s are inserted,
* old ones are purged. Supports file persistence.
*/
NONSHARABLE_CLASS( CMenuSrvMmcHistory ): public CBase
    {

public:     // construction

    /**
    * Destructor.
    */
    virtual ~CMenuSrvMmcHistory() { iMmcList.Close(); }

    /**
    * Constructor.
    */
    CMenuSrvMmcHistory() {}

public:     // new methods

    /**
    * Load from file. Safe to call if file does not exist.
    * @param aFs File Server Session.
    * @param aFname File name.
    */
    void LoadL( RFs& aFs, const TDesC& aFname );

    /**
    * Save to file. Existing file overwritten.
    * @param aFs File Server Session.
    * @param aFname File name.
    */
    void SaveL( RFs& aFs, const TDesC& aFname );

    /**
    * Insert new ID to first position.
    * If ID is already in the history, now it is moved to first
    * position (->latest).
    * If history is full, oldest ID may be dropped from history.
    * @param aMmc MMC ID.
    */
    void InsertL( TUint aMmc );

    /**
    * Find ID in history.
    * @param aMmc MMC ID.
    * @return History index, or KErrNotFound.
    */
    TInt Find( TUint aMmc ); // Find mmc in history.

private:    // data

    RArray<TUint> iMmcList; ///< ID history list. Own.

    };

#endif // __MENUSRVMMCHISTORY_H__
