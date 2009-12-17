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
* Description:  Flyweigth pattern implementation for dom strings.
*
*/



#ifndef hsps_DOM_STRING_POOL_H
#define hsps_DOM_STRING_POOL_H

//  INCLUDES
#include <e32base.h>
#include <s32strm.h>

// CLASS DECLARATION

/**
 * Class utilize flyweight pattern. Dom strings are stored once 
 * and referred with index. Class can be serialized.
 *
 * @lib hspsdomdocument.lib
 * @since S60 5.0
 * @ingroup group_hspsdom
 */
class ChspsDomStringPool : public CBase
    {
    public:  // Constructors and destructor
        
        /**
        * Two-phased constructor.
        * 
        * @since S60 5.0
        */
        static ChspsDomStringPool* NewL();
        
        /**
        * Two-phased stream constructor.
        * 
        * @since S60 5.0
        * @param aStream Source stream.
        */        
        static ChspsDomStringPool* NewL( RReadStream& aStream );
        
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
   public: //Adding
        
        /**
        * Set dom string into string pool.
        * 
        * @since S60 5.0
        * @param aString String to add to string pool
        * @return Index (reference) to string pool
        */
        IMPORT_C TInt AddStringL( const TDesC8& aString ); 
   
   public: //Accessing     
        /**
        * Get pointer to the node element name.
        * @param aStringRef StringRef.
        * @return Pointer to the name
        */
        const TDesC8& String( const TInt aStringRef ); 
        
        /**
        * Get object's data size in bytes.
        * @return Data size in bytes
        */
        TInt Size() const;
        
        /**
        * Externalize object
        * @param aStream Output stream
        */
        void ExternalizeL( RWriteStream& aStream ) const;
       
        /**
        * Internalize object
        * @param aStream Input stream
        */
        void InternalizeL( RReadStream& aStream );          
        
   private:

        /**
        * C++ default constructor.
        */
        ChspsDomStringPool();

        /**
        * By default Symbian 2nd phase constructor is private.
        */
        void ConstructL();
     
    private:    
        //String pool
        RPointerArray<HBufC8>       iStringPool;
       
        
    };

#endif      // hsps_DOM_STRING_POOL_H  
            
// End of File
