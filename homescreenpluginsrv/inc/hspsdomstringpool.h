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
* Description:  Flyweigth pattern implementation for dom strings.
*
*/

#ifndef HSPS_DOM_STRING_POOL_H
#define HSPS_DOM_STRING_POOL_H

//  INCLUDES
#include <e32base.h>
#include <s32strm.h>
#include "hspsdomstringpooloptimizer.h"

// CLASS DECLARATION

/**
*  @ingroup group_hspsdom
*  Class utilize flyweight pattern. Dom strings are stored once 
*  and referred with index. Class can be serialized.
*
*  @lib xndomdocument.lib
*  @since Series 60 3.1
*/
class ChspsDomStringPool : public CBase
    {
    public:  // Constructors and destructor
        
        /**
        * Two-phased constructor.
        * 
        * @param    aAllowDuplicates        ETrue if duplicates are to be allowed.
        *                                   Supported for legacy reasons. 
        */
        static ChspsDomStringPool* NewL( const TBool aAllowDuplicates = EFalse );

        /**
        * Two-phased stream constructor.
        * 
        * @param    aStream                 Stream where string pool is internalized.
        * @param    aAllowDuplicates        ETrue if duplicates are to be allowed.
        *                                   Supported for legacy reasons. 
        */
        static ChspsDomStringPool* NewL( RReadStream& aStream,
                const TBool aAllowDuplicates = EFalse );
        
        /**
        * Destructor.
        */
        virtual ~ChspsDomStringPool();
        
   public: 
        /**
        * Make a copy from original StringPool.
        * @since Series 60 3.1
        * @return Pointer to a string pool. Ownership is transferred to a caller.
        */
        ChspsDomStringPool* CloneL(); 
        
        /**
        * Reset string pool to be reused.
        * 
        * @since Series 60 5.2
        */
        void Reset();         
        
   public: //Adding
        
        /**
        * Set dom string into string pool.
        * 
        * @param aString String to add to string pool
        * @return Index (reference) to string pool
        */
        IMPORT_C TInt AddStringL( const TDesC8& aString ); 

        /**
        * Set dom string into string pool.
        * 
        * @param aString String to add to string pool. OWNERSHIP TRANSFERRED!
        * @return Index (reference) to string pool
        */
        TInt AddStringL( HBufC8* aString );         

        /**
        * Add all string from another string pool.
        * 
        * @param aStringPool    Source string pool.
        */
        void AddAllL( ChspsDomStringPool& aStringPool );        
        
   public: //Accessing     
        /**
        * Get reference to string.
        * 
        * @param aMap Map object which has index to name string
        * @return Pointer to the name
        */
        const TDesC8& String( const TInt aStringRef ); 
        
        /**
        * Get object's data size in bytes.
        * 
        * @return Data size in bytes
        */
        TInt Size() const;

        /**
        * Get amount of strings.
        */
        TInt Count() const;        
        
        /**
        * Externalize object.
        * 
        * @param aStream Output stream
        */
        void ExternalizeL( RWriteStream& aStream ) const;
       
        /**
        * Internalize object.
        * 
        * @param aStream Input stream
        */
        void InternalizeL( RReadStream& aStream );          
        
   private:

        /**
        * C++ default constructor.
        * 
        * @param    aAllowDuplicates        ETrue if duplicates are to be allowed.
        *                                   Supported for legacy reasons.
        */
        ChspsDomStringPool( const TBool aAllowDuplicates );

        /**
        * By default Symbian 2nd phase constructor is private. 
        */
        void ConstructL();    
        
        /**
        * Add string to string pool and to optimizer also.
        * 
        * @param aNewString     String to be added. OWNERSHIP TRANSFERRED.
        * @param TInt           Index to added string.
        */
        TInt DoAddStringL( HBufC8* aNewString ) ;
        
    private:            
        //String pool
        RPointerArray<HBufC8>       iStringPool;
        
        /**
         * String pool optimizer.
         */
        ThspsDomStringPoolOptimizer iStringPoolOptimizer;
        
        /**
         * ETrue if string pool can contain duplicate entries. Must
         * be supported for legacy reasons while loading xuikon odts.
         */
        TBool iAllowDuplicates;
    };

#endif      // HSPS_DOM_STRING_POOL_H  
            
// End of File
