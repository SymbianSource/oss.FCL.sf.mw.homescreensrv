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
* Description:  Class used by session to handle asynchronous requests
 *
*/


#ifndef C_CPACTIONDATACACHE_H
#define C_CPACTIONDATACACHE_H

#include <e32base.h>
#include <e32hashtab.h> 

class CLiwDefaultList;

/**
 *  Action Data Cache holds a list containing data needed to 
 *  lauch an action fetched from SQL database
 *  @since S60 v5.0
 */
class CCPActionDataCache:public CBase
    {
public:

    /**
     * Creates new instance of CCPActionDataCache.
     * 
     * @return new instance of CCPActionDataCache
     */
    static CCPActionDataCache* NewL( );

    /**
     * Creates new instance of CCPActionDataCache.
     * 
     * @return new instance of CCPActionDataCache
     */
    static CCPActionDataCache* NewLC( );
    
    /**
     * Destructor.
     */
    virtual ~CCPActionDataCache();
    
    /**
     * Removes an item from the cache
     * 
     * @param aMap a map containing item identifiers 
     */
    void HandleCacheRemoveL( const CLiwMap* aMap );

    /**
     * Appends an item to the cache
     * 
     * @param aParamList a list to be added to the cache
     */
    void AppendL( const CLiwGenericParamList* aParamList); 
    
    /**
     * Appends empty item to the cache to indicate that the entry 
     * doesn't exist in a storage and it makes not sense to call GetList
     * operation on a storage
     * 
     * @param aIdsMap a map containing entry identifiers
     */
    void AppendEmptyL( const CLiwMap* aIdsMap );

    /**
     * Checks if it is possible to cache the item. 
     * Only items that are specified, which means that id or 
     * all parameters ( publisher, content_type, content_id ) are provided
     * 
     * @param aMap a map containing item identifiers 
     * @return ETrue if it is possible to cache an item, EFalse if not
     */
    TBool IsCacheableL( const CLiwMap* aMap );
    
    /**
     * Checks if it item exists in the cache.
     * 
     * @param aMap a map containing item identifiers 
     * @return ETrue if the item exists in the cache, EFalse if not
     */
    TBool ExistL( const CLiwMap* aMap );
    
    /**
     * Gets an item from the cache
     * 
     * @param aMap a map containing item identifiers 
     * @return aParamList item returned from the cache
     */
    void GetL( const CLiwMap* aMap,
            CLiwGenericParamList* aParamList );
    
private:
    
    /*
     * Constructor
     */
    CCPActionDataCache();
    
    /**
     * 2nd phase constructor.
     */
    void ConstructL( );
    
    /**
     * Checks if two items match
     * 
     * @param aCachedMap a map from the cache containing item identifiers 
     * @param aInputMap an input map containing item identifiers 
     * @return ETrue if items match , EFalse if not
     */
    TBool MatchL(const CLiwMap* aCachedMap, const CLiwMap* aInputMap);
    
    /**
     * Checks if types of two items match
     * 
     * @param aCachedMap a map from the cache containing item identifiers 
     * @param aInputMap an input map containing item identifiers 
     * @return ETrue if types match , EFalse if not
     */
    TBool TypesMatchL(const CLiwMap* aCachedMap, const CLiwMap* aInputMap);
    
    /**
     * Extracts RBuf from a variant
     * 
     * @param aVariant a variant
     * @param aBuf a result descriptor
     */
    void ExtractRBufL(const TLiwVariant& aVariant, RBuf& aBuf);
    
    /**
     * Checks if two strings contained in both maps match
     * 
     * @param aLeft first map
     * @param aRight second map 
     * @param aKey key 
     * @return ETrue if items match , EFalse if not
     */
    TBool MatchL(const CLiwMap* aLeft, 
            const CLiwMap* aRight, const TDesC8& aKey );
    
    /**
     * Checks if a string contained in map is specified
     * (it has value different than "all")
     * 
     * @param aMap map
     * @param aKey key 
     * @return ETrue if item is specified , EFalse if not
     */
    TBool IsSpecifiedL(const CLiwMap* aMap, 
            const TDesC8& aKey ); 

    /**
     * Finds the item in the cache
     *  
     * @param aKey map containing identifiers 
     * @return id of the item in a cache, KErrNotFound if does not 
     * exist in the cache
     */
    TInt FindL( const CLiwMap* aKey ); 
    
    /**
     * Copies variant from one map to another
     * 
     * @param aKey key
     * @param aInMap input map 
     * @param aOutMap output map 
     */
    void CopyVariantL(const TDesC8& aKey,
            const CLiwMap* aInMap, CLiwDefaultMap* aOutMap );
    
private:
    
    /*
     * Internal list. Own
     */
    CLiwDefaultList* iInternalList;
    };

#endif // C_CPACTIONDATACACHE_H
