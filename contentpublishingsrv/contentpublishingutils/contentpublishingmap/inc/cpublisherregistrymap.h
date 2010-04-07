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
* Description:  CPublisherRegistryMap - class used in Content Publisher
 *
*/

#ifndef C_CPUBLISHERREGISTRYMAP_H
#define C_CPUBLISHERREGISTRYMAP_H

#include "cpliwmap.h"

// CLASS DECLARATION
/**
 *  Map which contains Publisher Registry information
 *
 *  @lib cputils_hs_hs.dll
 *  @since S60 v5.0
 */
NONSHARABLE_CLASS( CPublisherRegistryMap ): public CCPLiwMap
    {
public:

    /**
     * Two-phased constructor.
     */
    IMPORT_C static CPublisherRegistryMap* NewL( );

    /**
     * Two-phased constructor.
     */
    IMPORT_C static CPublisherRegistryMap* NewLC( );

    /**
     * Destructor.
     */
    virtual ~CPublisherRegistryMap();

private:

    /**
     * Adds to database current content
     *  
     * @param aSqlDb handle to database
     * @param aNotificationList list of notifications to fill in when 
     *        something was actually changed in dataabse
     *
     * @return id of added entry
     */
	TInt32 AddL( RSqlDatabase aSqlDb,
            CLiwDefaultList* aNotificationList = 0 ) const;

    /**
     * Fetches entries from database
     *  
     * @param aSqlDb handle to database
     * @param aList target list for enntries
     * @param aItemsToDelete ids of entries for removal - 
     *        they were found to be expired
     *
     * @return number of fetched entries
     */
    TUint GetListL( RSqlDatabase aSqlDb,
            CLiwGenericParamList* aList,
            RArray<TInt32>& aItemsToDelete ) const;
    /**
     * 
     */
    void RemoveL( RSqlDatabase aSqlDb,
            CLiwDefaultList* aNotificationList = 0 ) const;
    
    /**
     * Removes entries from database
     *  
     * @param aSqlDb handle to database
     * @param aNotificationList list of notifications to fill in when 
     *        something was actually changed in dataabse
     */
    void SaveQueryResultsL( RSqlStatement& aStmt ,
            CLiwGenericParamList& aList,
            RArray<TInt32>& aItemsToDelete )const;
    
    /**
     * Performs actuall add operation
     *  
     * @param aSqlDb handle to database
     * @param aNotificationList list of notifications to fill in when 
     *        something was actually changed in dataabse
     *
     * @return id of added entry
     */
    TInt32 AddToDbL( RSqlDatabase aSqlDb,
            CLiwDefaultList* aNotificationList ) const;

    /**
     * Appends filter as strings to SQL query according to aFilter
     *  
     * @param aQuery SQL query
     * @param aFilter as enum
     *
     */
    void AppendFilterL( RBuf& aQuery, TUint& aFilter ) const;

    /**
     * Appends sorting criteria as strings to SQL query according to aSort
     *
     * @param aQuery SQL query
     * @param aSort sort filter as TInt
     */
    void AppendSortL( RBuf& aQuery, const TCPSortOrder& aSort ) const;

    /**
     * Prepares SQL query for Add operation
     *
     * @param aSqlDb handle to database  
     * @param aStmt handle to SQL statement
     * @param aQuery SQL query
     *
     */
    void PrepareStmtforAddL(RSqlDatabase aSqlDb, 
	                               RSqlStatement& aStmt , 
                                   const TDesC& aQuery ) const;

    /**
     * Prepares SQL query for Update operation
     *
     * @param aSqlDb handle to database  
     * @param aStmt handle to SQL statement
     * @param aId id of entry to update
     *
     */							   
    void PrepareStmtforUpdateL( RSqlDatabase aSqlDb,
            RSqlStatement& aStmt,
            TInt32 aId ) const;
			
    /**
     * Updates entry in database
     *  
     * @param aSqlDb handle to database
     * @param aId id entry to update
     * @param aNotificationList list of notifications to fill in when 
     *        something was actually changed in dataabse
     */
    void UpdateL( RSqlDatabase aSqlDb, TInt32 aId,
            CLiwDefaultList* aNotificationList = 0 ) const;
			
    /**
     * Check if IDs are valid for Delete operation
     *
     */
    void IsValidForDeleteL() const;
    
    /**
     * Check if IDs are valid for GetList operation
     */
    void IsValidForGetListL() const ;

    /**
     *
     * @return ETrue if supported, EFalse if not
     */
    TBool ActivateActionSupport( ) const;      
    
    /**
     *
     * @return ETrue if activate flag is set, EFalse if not
     */
    TBool GetActivateInfo( ) const;
    
    /**
      * Fills change info map with proper values
      *aStmt
      * @param aStmt handle to SQL statement
      * @param aChangeInfoMap map that will be filed
      *
      */
    void FillChangeInfoMapL( RSqlStatement& aStmt, 
			CLiwDefaultMap* aMap ) const;

private:

    /**
     * C++ default constructor.
     */     
    CPublisherRegistryMap();

    /**
     * Perform the second phase construction of a CPublisherRegistryMap object.
     */      
    void ConstructL();

    };

#endif // C_CPUBLISHERREGISTRYMAP_H
