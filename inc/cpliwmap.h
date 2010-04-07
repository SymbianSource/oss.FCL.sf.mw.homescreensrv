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
* Description:   CCPLiwMap - class used in Content Publisher
 *
*/

#ifndef C_CPLIWIDSMAP_H
#define C_CPLIWIDSMAP_H

#include <liwvariant.h>
#include <sqldb.h>
#include "cpglobals.h"

class RDesReadStream;
class CCPSecurityPolicy;

// CLASS DECLARATION
/**
 *  Base Map class for Publisher Registry and Content
 *
 *  @lib cputils_hs_hs.dll
 *  @since S60 v5.0
 */
class CCPLiwMap : public CLiwMap
    {

protected:
    
    /**
     * Enum defining the properties of filter.
     */
    enum TCPProperties
        {
        EPublisher = 1,
        EContentType = 2,
        EContentId = 4,
        EFlag = 8,
        };

    /**
     * Enum defining the sort ordering.
     */    
	enum TCPSortOrder
        {
        ECPRecent = 1, // 0001
        ECPPublisher  = 2, // 0010
        ECPContent = 4, // 0100 
        ECPContentId = 8 // 1000 
        };   

public:

    friend class CCpStorageEngine;

    /**
     * Two-phased constructor.
     */    
    IMPORT_C static CCPLiwMap* NewL( RDesReadStream& aStream );

    /**
     * Two-phased constructor.
     */    
    IMPORT_C static CCPLiwMap* NewL( const CLiwGenericParamList& aList );

    /**
     * Used by client to serialize this object 
     *
     * @return object serialized to binary
     */
    IMPORT_C HBufC8* PackForServerLC() const;
    
    /**
     * Reset map
     */
    IMPORT_C void Reset( ) const;
    
    /**
     * Leaves when Add data operation is not possible due
     * to missing of some parameter
     */
    virtual void IsValidForAddL( ) const;

    /**
     * Leaves when Execute Action operation is not possible due
     * to missing of some parameter
     */
    IMPORT_C void IsValidForActionL( ) const;
    
    /**
     * Leaves when RequestNotification operation is not possible due
     * to missing of some parameter
     */
    IMPORT_C void IsValidForNotificationL( ) const;
    
    /**
     * Fetches value for property(key) from internal map
     *
     * @param aProperty name of the property ( key ) 
     * @param aResult target descriptor for fetching operation
     * @return ETrue is property found or EFalse if not
     */
    IMPORT_C TBool GetPropertyL( const TDesC8& aProperty,
             RBuf& aResult ) const;

    /**
     * Fetches value for property(key) from internal map
     *
     * @param aProperty name of the property ( key ) 
     * @param aResult target binary for fetching operation
     * @return ETrue is property found or EFalse if not
     */
    IMPORT_C TBool GetPropertyL( const TDesC8& aProperty,
            RBuf8& aResult ) const;
    
    /**
     * Fetches value for property(key) from internal map
     *
     * @param aProperty name of the property ( key ) 
     * @param aResult target integer for fetching operation
     * @return ETrue is property found or EFalse if not
     */
    IMPORT_C TBool GetProperty( const TDesC8& aProperty,
    		TInt32& aResult ) const;

    
    /**
     * Setter for security policy 
     *
     * @param aPolicy security policy
     */
    IMPORT_C void SetSecurityL( const RMessage2& aMessage );
    
    /**
     * Destructor.
     */
    virtual ~CCPLiwMap();

    /**
     * Check if IDs are valid for Delete operation
     */
    virtual void IsValidForDeleteL() const =0;
    
    /**
     * Check if IDs are valid for GetList operation
     */
    virtual void IsValidForGetListL() const =0;    

	/**
     *
     * @return ETrue if supported, EFalse if not
     */
	virtual TBool ActivateActionSupport( ) const = 0;

	/**
     *
     * @return ETrue if activate flag is set, EFalse if not
     */
	virtual TBool GetActivateInfo( ) const = 0;

	
public:

    // from base class CLiwMap
    /**
     * Inserts a key-value pair element to the map collection. If
     * the specified key already exists, it will be removed from
     * the collection and the new key-value pair will be added to the
     * map.
     *  
     * @param aKey the key to be stored
     * @param aValue the value associated with the key to be stored
     */
    void InsertL( const TDesC8& aKey, const TLiwVariant& aValue );

    /**
     * Finds a value stored in the map collection based on the key.
     *  
     * @param aKey the key to be searched
     * @param aFndValue the value associated with the found key
     * @return false if there is no key stored; true otherwise
     */
    TBool FindL( const TDesC8& aKey, TLiwVariant& aValue ) const;

    /**
     * Returns the number of key-value pair stored in the map collection.
     *  
     * @return the number of key-value pair stored in the map collection
     */
    TInt Count() const;

    /**
     * Returns the key stored at a specified index. 
     *  
     * @param aIndex the index of the key to be found
     * @param aFndKey the key found at the passed index
     *
     * @return true if a key-value entry is found at the passed index;
     * false otherwise
     */
    TBool AtL( TInt aIndex, TDes8& aFndKey ) const;

    /**
     * Removes a key from the map collection.
     *  
     * @param aKey the key to be removed from the map
     */
    void Remove( const TDesC8& aKey );

    /**
     * Returns the size of the collection. The size is 
     * calculated based on the total numer of key-value elements
     * and the size of each pair in the collection
     *  
     * @return the size of the collection
     */
    TInt Size() const;

    /**
     * The container managed data values can be serialized to an output
     * stream. 
     * 
     * @param aOutputStream the output stream to which the data will be streamed
     */
    void ExternalizeL( RWriteStream& aOutputStream ) const;

protected:

    /**
     * Adds to database current content
     *  
     * @param aSqlDb handle to database
     * @param aNotificationList list of notifications to fill in when 
     *        something was actually changed in dataabse
     *
     * @return id of added entry
     */
    virtual TInt32 AddL( RSqlDatabase aSqlDb,
        CLiwDefaultList* aNotificationList = 0 ) const= 0;

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
    virtual TUint GetListL( RSqlDatabase aSqlDb, CLiwGenericParamList* aList,
        RArray<TInt32>& aItemsToDelete ) const = 0;

    /**
     * Removes entries from database
     *  
     * @param aSqlDb handle to database
     * @param aNotificationList list of notifications to fill in when 
     *        something was actually changed in dataabse
     */
    virtual void RemoveL( RSqlDatabase aSqlDb,
        CLiwDefaultList* aNotificationList = 0 ) const = 0;

    /**
     * Saves results from query to SQLite Server
     *  
     * @param aStmt handle to SQL statement
     * @param aList target list for enntries
     * @param aItemsToDelete ids of entries for removal - 
     *        they were found to be expired
     */
    virtual void SaveQueryResultsL( RSqlStatement& aStmt,
        CLiwGenericParamList& aList, RArray<TInt32>& aItemsToDelete ) const = 0;

    /**
     * Updates entry in database
     *  
     * @param aSqlDb handle to database
     * @param aNotificationList list of notifications to fill in when 
     *        something was actually changed in dataabse
     */
    virtual void UpdateL( RSqlDatabase aSqlDb, TInt32 aId,
        CLiwDefaultList* aNotificationList = 0 ) const = 0;

    /**
     * Performs actuall add operation
     *  
     * @param aSqlDb handle to database
     * @param aNotificationList list of notifications to fill in when 
     *        something was actually changed in dataabse
     *
     * @return id of added entry
     */
    virtual TInt32 AddToDbL( RSqlDatabase aSqlDb,
        CLiwDefaultList* aNotificationList = 0 ) const = 0;

    /**
     * Checks if operation value is proper
     *  
     * @param aValue contains operation value
     */    
    void IsProperOperationL( const TLiwVariant& aValue ) const;
    
    
    /**
     * Appends GetList filter as strings to SQL query according to aFilter
     *  
     * @param aQuery SQL query
     * @param aFilter as enum
     *
     */
    void AppendGetListFilterL( RBuf& aQuery, TUint& aFilter ) const;
    
    /**
     * Appends Remove filter as strings to SQL query according to aFilter
     *  
     * @param aQuery SQL query
     * @param aFilter as enum
     *
     */
    void AppendRemoveFilterL( RBuf& aQuery, TUint& aFilter ) const;
    
    
    /**
     * Appends filter as strings to SQL query according to aFilter
     *  
     * @param aQuery SQL query
     * @param aFilter as enum
     *
     */
    virtual void AppendFilterL( RBuf& aQuery, TUint& aFilter ) const;

    /**
     * Prepares SQL query for Add operation
     *
     * @param aSqlDb handle to database  
     * @param aStmt handle to SQL statement
     * @param aQuery SQL query
     *
     */
    virtual void PrepareStmtforAddL( RSqlDatabase aSqlDb,
        RSqlStatement& aStmt, const TDesC& aQuery ) const;
    /**
     * Appends sorting criteria as strings to SQL query according to aSort
     *
     * @param aQuery SQL query
     * @param aSort sort filter as TInt
     *
     */
    virtual void AppendSortL( RBuf& aQuery, 
        const TCPSortOrder& aSort ) const = 0;

    /**
      * Fills change info map with proper values
      *aStmt
      * @param aStmt handle to SQL statement
      * @param aChangeInfoMap map that will be filed
      *
      */
    virtual void FillChangeInfoMapL( RSqlStatement& aStmt, 
			CLiwDefaultMap* aMap ) const;

protected:

    /**
     * Sets all parameters according to provided list
     * @param aItemMap map with parameters
     */
    void SetL( const CLiwGenericParamList& aInParamList );

    /**
     * Check Get properties
     *
     * @return logical sum of TCPProperties of the object
     */
    TUint GetFilterL() const;

    /**
     * Check and return sort order
     *
     * @return Sort Criteria
     */
    TCPSortOrder GetSortL() const;

    /**
     * Check if Publisher name is set
     *
     * @return ETrue if Publisher name was set, EFalse if not
     */
    TBool IsPublisherNameL() const;

    /**
     * Check if ContentType is set
     *
     * @return ETrue if ContentType was set, EFalse if not
     */
    TBool IsContentTypeL() const;

    /**
     * Check if ContentId is set
     *
     * @return ETrue if ContentId was set, EFalse if not
     */
    TBool IsContentIdL() const;

    /**
     * Check if ActionTrigger is set
     *
     * @return ETrue if ActionTrigger was set, EFalse if not
     */
    TBool IsTriggerL( ) const;

    /**
     * Check if Flag is present
     *
     * @return ETrue if ContentId was set, EFalse if not
     */
    TBool IsFlag( ) const;
    	
    /**
     * Checks if Id property is present in internal map
     *
     */
    TBool IsId() const;

    /**
     * Checks if property(key) is present in internal map 
     *
     */
    TBool PropertyExists( const TDesC8& aProperty ) const;

    
    /**
     * Fetches entries from database
     *  
     * @param aSqlDb handle to database
     * @param aQuery aQuery SQL query
     * @param aList target list for enntries
     * @param aItemsToDelete ids of entries for removal - 
     *        they were found to be expired
     *
     * @return number of fetched entries
     */
    TUint GetListL( RSqlDatabase aSqlDb, const TDesC& aQuery,
        CLiwGenericParamList* aList, RArray<TInt32>& aItemsToDelete ) const;

    /**
     * Removes entries from database
     *  
     * @param aSqlDb handle to database
     * @param aQuery SQL query
     * 
     */
    void RemoveL( RSqlDatabase aSqlDb, const TDesC& aQuery ) const;

    /**
     * Fetches Ids (Publisher, ContentType, ContentId) from database
     * 
     * @param aId id entry in database
     * @param aQuery SQL query
     * @param aMap map to insert Ids
     * @param aSqlDb handle to database  
     * 
     */
    void FetchIdsL( TInt32 aId, const TDesC& aQuery, CLiwDefaultMap* aMap,
        RSqlDatabase aSqlDb ) const;

    /**
     * Fetches Id from database
     * 
     * @param aSqlDb handle to database
     * @param aQuery SQL query
     * @param aId id entry from database
     * 
     */
    void FetchIdL( RSqlDatabase aSqlDb,
        const TDesC& aQuery, TInt32& aId ) const;

    /**
     * Gets Ids (Id, Publisher, ContentType, ContentId) 
     *      of removed entries from database
     *  
     * @param aSqlDb handle to database
     * @param aQuery SQL query
     * @param aList List with ids
     * 
     */
    void GetIdsOfRemovedEntriesL( RSqlDatabase aSqlDb, const TDesC& aQuery,
        CLiwDefaultList* aList ) const;

    /**
     * Saves Ids (Id, Publisher, ContentType, ContentId) from database
     * 
     * @param aSqlDb handle to database
     * @param aList List with ids
     * 
     */
    void SaveQueryResultsIdsL( RSqlStatement& aStmt,
        CLiwDefaultList* aList ) const;    

    /**
     * Contructs SQL query to select entry
     * @param aQuery SQL query
     * 
     */
    void ConstructSelectIdL( RBuf& aQuery ) const;

    /**
     * Contructs SQL query to select entries for GetList
     * @param aQuery SQL query
     * 
     */
    void ConstructGetListSelectL( RBuf& aQuery, TBool aGet = ETrue ) const;

    /**
     * Contructs SQL query to select entries for Remove
     * @param aQuery SQL query
     * 
     */
    void ConstructRemoveSelectL( RBuf& aQuery, TBool aGet = ETrue ) const;
    
    /**
     * Binds value to query
     * @param aQuery SQL query
     * @param aSqlDb handle to database
     * 
     */
    void BindValueL( const RBuf& aQuery, RSqlStatement& aStmt ) const;

    /**
     * Checks if GetList for all items should be done
     * @param aFilter flag describing which ids are set
     * @return ETrue if getlist for all items shold be done, EFalse if not
     */
    TBool IsGetListAllL( TUint aFilter ) const;
    
    /**
     * Removes attributes with "All" value from map
     * @param aFilter filter bit will be reset if any attribute == "all"
     */    
    void RemoveAllAttributeL( TUint& aFilter ) const;
    
    /**
     * Leaves if publisher, content type od content_id is not descriptor type. 
     */    
    void CheckIdentifiersL() const;
    
    /**
     * Leaves if aParameterIndex is negative-error value 
     * or in case of binding error. 
     */    
    void BindTextL(RSqlStatement& aStmt, TInt aParameterIndex,
        const TDesC& aParameterText) const;
    
    /**
     * Leaves if aParameterIndex is negative-error value 
     * or in case of binding error. 
     */ 
    void BindBinaryL(RSqlStatement& aStmt, TInt aParameterIndex,
        const TDesC8& aParameterText) const;
    
    /**
     * Leaves if aParameterIndex is negative-error value 
     * or in case of binding error. 
     */ 
    void BindInt64L(RSqlStatement& aStmt, TInt aParameterIndex,
        TInt64 aParameterValue) const;

    /**
     * Leaves if aParameterIndex is negative-error value 
     * or in case of binding error. 
     */ 
    void BindIntL(RSqlStatement& aStmt, TInt aParameterIndex,
        TInt aParameterValue) const;
    
    /**
     * Leaves if column index return value is negative.
     * @return result of RSqlStatement::ColumnIndex
     */ 
    TInt ColumnIndexL( RSqlStatement& aStmt, const TDesC& aColumnName )	const;
protected:

    /**
     * C++ default constructor.
     */     
    CCPLiwMap();

    /**
     * Perform the second phase construction of a CCPLiwMap object.
     */       
    void ConstructL();

protected:
    // data

    /*
     * The underlying generic parameter list
     * Own.
     */
    CLiwGenericParamList* iMap;
    
    /*
     * Class used for security 
     *Own.
     */
    const CCPSecurityPolicy* iPolicy;

    };

#endif // C_CPLIWIDSMAP_H
