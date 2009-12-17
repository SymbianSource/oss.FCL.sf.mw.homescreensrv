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
* Description:  Database implementation
 *
*/


#ifndef C_CPSTORAGE_ENGINE_H
#define C_CPSTORAGE_ENGINE_H

#include <e32cons.h>
#include <sqldb.h>
#include "cpstorage.h"

// FORWARD DECLARATIONS
#ifdef CONTENT_PUBLISHER_DEBUG
class CCPDebug;
#endif
class MCPChangeNotifier;
class CRepository;

// CLASS DECLARATION
/**
 *  SQLite based databse for Content Publisher 
 *
 *  @lib cpstorage.dll
 *  @since S60 v5.0
 */
NONSHARABLE_CLASS( CCpStorageEngine ): public CCpStorage
    {
public:

    /**
     * Two-phased constructor.
     */
    static CCpStorageEngine* NewL();

    /**
     * Two-phased constructor.
     */
    static CCpStorageEngine* NewLC();

    /**
     * Destructor.
     */
    ~CCpStorageEngine();

    //from CCpStorage

    /**
     * Fetches data from database
     *
     * @param aMap Filtering and sorting criteria
     * @param aList Target for results 
     */
    void GetListL( const CLiwMap* aMap, CLiwGenericParamList& aList );

    /**
     * Adds or Updates data & action to database
     * @param Map containing data item
     * @return id of added entry
     */
    TInt32 AddL( const CLiwMap* aMap );

    /**
     * Remove data item from database
     * @param Filtering and sorting criteria
     */
    void RemoveL( const CLiwMap* aMap );

    /**
     * Set database observer 
     * @param Callback pointer
     */
    void SetCallback( MCPChangeNotifier* aCallback );

private:

    /**
     * Perform the second phase construction of a CCpStorageEngine object.
     */
    void ConstructL(); 

    /**
     * C++ default constructor.
     */
    CCpStorageEngine();

    /**
     * Creates database tables.
     */
    void CreateTablesL();

    /**
     * Creates new database.
     * If Db is created for the first time,
     * set repository db state into KSQLDBStateNormal  
     */
    void CreateNewDbL();

    /**
     * Deletes database.
     * Called when opening Db try return KSqlErrCorrupt error.
     * Removes KSQLDBStateNormal state and set KSQLDBStateRestored into
     * repository db state.  
     */
    void DeleteCorruptedDbL();

private:

    /**
     * Database.
     * Own. 
     */
    RSqlDatabase iSqlDb;

    /**
     * Callback pointer.
     * Not own.
     */
    MCPChangeNotifier* iCallback;
    
    /**
     * Repository keeps information about Db state.
     * Own.
     */
    CRepository* iRepository;

#ifdef CONTENT_PUBLISHER_DEBUG
    CCPDebug* iDebug;
#endif
    };

#endif //C_CPSTORAGE_ENGINE_H
