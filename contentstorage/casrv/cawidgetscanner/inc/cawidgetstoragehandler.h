/*
* Copyright (c) 2008 Nokia Corporation and/or its subsidiary(-ies).
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


#ifndef C_WIDGETSTORAGEHANDLER_H
#define C_WIDGETSTORAGEHANDLER_H

class CCaStorageProxy;
class CCaWidgetDescription;

//  INCLUDES
#include <e32base.h>
#include "cawidgetdescription.h"
#include "castorage_global.h"
// CONSTANTS

// FORWARD DECLARATIONS

/**
 *  Widget Parser
 *
 *  @since Series Series60 5.x
 */
class CCaWidgetStorageHandler : public CBase
    {
public:
    CA_STORAGE_TEST_FRIEND_CLASS    (T_CaWidgetScanner)

public:
    // Constructors and destructor

    /**
     * Two-phased constructor.
     */
    static CCaWidgetStorageHandler* NewL( CCaStorageProxy* aStorage,
            RFs& aFs );
    static CCaWidgetStorageHandler* NewLC( CCaStorageProxy* aStorage,
            RFs& aFs );

    /**
     * Destructor.
     */
    virtual ~CCaWidgetStorageHandler();

    /**
     * Synchronizes widgets
     * @param aWidgets widgets to synchronize
     */
    void SynchronizeL( const RWidgetArray& aWidgets );


private:

    /**
     * Adds widget to storage.
     * @param aWidget widget to be added
     */
    void AddL( const CCaWidgetDescription* aWidget );

    /**
     * Updates widget in storage.
     * @param aWidget widget to be updated
     */
    void UpdateL( const CCaWidgetDescription* aWidget, TUint aEntryId );

    /**
     * Add/Updates widgets in DB
     * @param aWidgets widgets to add
     */
    void AddWidgetsL( const RWidgetArray& aWidgets );

    /**
     * Removes widgets from DB
     */
    void RemoveWidgetsL( ) ;

    /**
     * Fetch widgets from content storage
     */
    void FetchWidgetsL( );

    /**
     * Add widget to download collection
     * @param aEntry entry containing widget information
     */
    void AddWidgetToDownloadCollectionL( const CCaInnerEntry* aEntry );

    /**
     * Set missing flag for an entry.
     * @param aWidget widget.
     */
    void SetMissingFlagL( const CCaWidgetDescription * aWidget );

    /**
     * Clear missing flag for an entry.
     * @param aWidget widget.
     */
    void ClearVisibleFlagL( const CCaWidgetDescription* aWidget );

private:

    /**
     * C++ default constructor.
     */
    CCaWidgetStorageHandler( CCaStorageProxy* aStorage, RFs& aFs );

    /**
     * By default Symbian 2nd phase constructor is private.
     */
    void ConstructL( );

    /**
     * Check if mass storage is not in use.
     * @return true if mass storage is not in use.
     */
    TBool MassStorageNotInUse();

private:
    // Data

    /*
     * Content Storage. Not Own
     */
    CCaStorageProxy* iStorage;

    /*
     * File server. Not own
     */
    RFs iFs;

    /*
     * Widgets. Own.
     */
    RWidgetArray iWidgets;

    /*
     * Indexes updated during last add operation. Own.
     */
    RArray<TInt> iUpdatedIndexes;


    };

#endif      // C_WIDGETSTORAGEHANDLER_H
// End of File
