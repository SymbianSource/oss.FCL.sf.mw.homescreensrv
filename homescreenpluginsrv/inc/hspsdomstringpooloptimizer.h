/*
* Copyright (c) 2005,2006 Nokia Corporation and/or its subsidiary(-ies).
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
* Description:  Optimizer module for ChspsDomStringPool.
*
*/

#ifndef HSPS_DOM_STRING_POOL_OPTIMIZER_H
#define HSPS_DOM_STRING_POOL_OPTIMIZER_H

//  INCLUDES
#include <e32base.h>

// CLASS DECLARATION

/**
*  @ingroup group_hspsdom
*  Optimizer module entry for ChspsDomStringPool.
*
*  @lib hspsdomdocument.lib
*  @since Series 60 5.2
*/
class ThspsDomStringPoolOptimizerEntry
    {
    public: // Construction.
        /**
         * Constructor.
         * 
         * @param aIndex    Index.
         * @param aString   String.
         */
        ThspsDomStringPoolOptimizerEntry( TInt aIndex, const TDesC8& aString ); 

    public: // Data.
        /**
         * Index of string in actual string pool.
         */
        TInt iIndex;
        
        /**
         * Reference to string in string pool.
         */
        const TDesC8& iString;                    
    };        

/**
*  @ingroup group_hspsdom
*  Optimizer module for ChspsDomStringPool.
*
*  @lib hspsdomdocument.lib
*  @since Series 60 5.2
*/
class ThspsDomStringPoolOptimizer
    {
    public:
        /**
         * Add entry to optimizer list.
         * 
         * @param aEntry    Entry to be added.
         */
        void AddEntryL( ThspsDomStringPoolOptimizerEntry& aEntry );
        
        /**
         * Get index for string.
         * 
         * @param   aString     Reference to given string.
         * @return  TInt        Index to actual string pool for string if found.
         *                      If string is not found will return KErrNotFound. 
         */
        TInt GetIndex( const TDesC8& aString );

        /**
         * Reset.
         */
        void Reset();        
        
        /**
         * Close allocated resources.
         */
        void Close();

        /**
         * Get item count.
         */
        TInt Count();        

        /**
         * Get entry.
         * 
         * @param aIndex    Index to Entry.
         */
        ThspsDomStringPoolOptimizerEntry& Entry( const TInt aIndex );        
        
    private:
        /**
         * Find entry from alphabetic list.
         * Uses binary search.
         * 
         * @param       aString Reference to string to be searched for.
         * @param       aLeft   Left limit for binary search
         * @param       aRight  Right limit for binary search.
         * 
         * @return      Index to OPTIMIZER ARRAY. KErrNotFound if
         *              given string is not found.
         */
        TInt FindEntry( const TDesC8& aString,
                        const TInt aLeft,
                        const TInt aRight );

        /**
         * Find a position clue for given string.
         * 
         * Will return index that can be used to initiate linear
         * search. Uses binary search to limit required comparisons
         * when string pools starts to fill.
         * 
         * Note: Returned index is not absolute! it must be
         * only used as a start index for linear searching.
         * 
         * Returned index will be quite close to actual insertion position.
         * it will be 0 - 2 steps backward from actual position.
         * 
         * @param       aString Reference to string.
         * @param       aLeft   Left limit for binary search
         * @param       aRight  Right limit for binary search.
         * 
         * @return      Index to start searching for position
         *              for given string.
         */
        TInt FindInsertionIndexEstimate( const TDesC8& aString,
                                         const TInt aLeft,
                                         const TInt aRight );
        
    private: // Data.
        /**
         * Array of optimizer entries.
         */
        RArray<ThspsDomStringPoolOptimizerEntry> iEntries;
    }; 

#endif      // HSPS_DOM_STRING_POOL_OPTIMIZER_H  
            
// End of File
