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


#ifndef C_CPSTORAGE_H
#define C_CPSTORAGE_H

// FORWARD DECLARATIONS
class CLiwMap;
class CLiwGenericParamList;
class CLiwDefaultList;

/**
 *  Callback Interface for observing changes in Content Publisher Database  
 *
 *  @since S60 v5.0
 */
class MCPChangeNotifier
    {
public:

    /**
     * This method should be implemented by subscriber for changes in
     * database
     *
     * @param Map containing change information
     * @param Id of changed entry
     * @return 
     */
    virtual void HandleChangeL( CLiwDefaultList* aListOfMaps ) = 0;

    };

/**
 * Interface for CPS database implementations
 *
 */
class CCpStorage : public CBase
    {
public:

    /**
     * Fetches data from database
     *
     * @param aMap Filtering and sorting criteria
     * @param aList Target for results 
     */
    virtual void GetListL( const CLiwMap* aMap,
        CLiwGenericParamList& aList ) = 0;
    /**
     * Adds or Updates data & action to database
     * @param Map containing data item
     * @return id of added entry
     */
    virtual TInt32 AddL( const CLiwMap* aMap ) = 0;

    /**
     * Removes data item from database
     * @param Filtering and sorting criteria
     */
    virtual void RemoveL( const CLiwMap* aMap ) = 0;

    /**
     * Set database observer 
     * @param Callback pointer
     */
    virtual void SetCallback( MCPChangeNotifier* aCallback ) = 0;

    };

#endif //C_CPSTORAGE_H
