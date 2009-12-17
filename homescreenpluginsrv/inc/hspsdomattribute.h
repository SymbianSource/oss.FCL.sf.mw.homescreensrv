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
* Description:  Represent single xmluiml attribute
*
*/



#ifndef hsps_DOM_ATTRIBUTE_H
#define hsps_DOM_ATTRIBUTE_H

//  INCLUDES
#include    <e32base.h>
#include    <s32strm.h>
#include    "mhspsdomlistitem.h"

//CLASS FORWARD

class ChspsDomStringPool;

// CLASS DECLARATION

/**
 * Represent attribute name and value as strings. 
 *
 * @lib hspsdomdocument.lib
 * @since S60 5.0
 * @ingroup group_group_hspsdom
 */
class ChspsDomAttribute : public CBase, public MhspsDomListItem
    {
    public:  // Constructors and destructor
        
        /**
        * Two-phased constructor 8 bit.
        * 
        * @since S60 5.0
        * @param aName          Name of attribute.
        * @param aStringPool    Attached string pool.
        */
        IMPORT_C static ChspsDomAttribute* NewL( 
            const TDesC8& aName, 
            ChspsDomStringPool& aStringPool );
        
        /**
        * Two-phased stream constructor.
        * 
        * @since S60 5.0
        * @param aStream        Source stream.
        * @param aStringPool    Attached string pool.
        */
        static ChspsDomAttribute* NewL( 
            RReadStream& aStream, 
            ChspsDomStringPool& aStringPool );
        
        /**
        * Destructor.
        */
        IMPORT_C virtual ~ChspsDomAttribute();

    public: // New functions
        /**
        * Makes a copy from original attribute.
        * 
        * @since S60 5.0
        * @return Pointer to attribute. Attribute ownership is transferred to a caller.
        */
        IMPORT_C ChspsDomAttribute* CloneL();        
        
        /**
        * Makes a copy from original property.
        * 
        * @since S60 5.0
        * @param aStringPool Original string pool clone.
        * @return Pointer to an attribute. Ownership is transferred to a caller.
        */
        ChspsDomAttribute* CloneL( ChspsDomStringPool& aStringPool );        
        
        /**
        * Get the attribute value.
        * 
        * @since S60 5.0
        * @return Pointer to a attribute value string. If value is not set KNullDesC8 is returned. 
        */
        IMPORT_C const TDesC8& Value();
        
        /**
        * Set attribute value.
        * 
        * @since S60 5.0
        * @param aValue Attribute string value.
        */
        IMPORT_C void SetValueL( const TDesC8& aValue );
        
        /**
        * Get the attributes name string pool index.
        *  
        * @since S60 5.0
        * @return String pool index.
        */
        IMPORT_C TInt16 NameStringPoolIndex()const;
        
        /**
        * Get the attributes value string pool index.
        *  
        * @since S60 5.0
        * @return String pool index.
        */
        IMPORT_C TInt16 ValueStringPoolIndex()const;
         
    public: //From ChspsDomListItem
        
        /**
        * Documented in ChspsDomListItem::Size
        * 
        * @since S60 5.0
        * @return Name.
        */
        IMPORT_C const TDesC8& Name();
        
       
        /**
        * Documented in ChspsDomListItem::Size.
        */
        TInt Size() const;
        
        /**
        * Documented in ChspsDomListItem::Size.
        */
        void ExternalizeL( RWriteStream& aStream ) const;
       
        /**
        * Internalize property.
        * 
        * @since S60 5.0
        * @param aStream Input stream.
        */
        void InternalizeL( RReadStream& aStream );    
    private:
        
        /**
        * C++ default constructor.
        */
        ChspsDomAttribute( ChspsDomStringPool& aStringPool );
            
        /**
        * By default Symbian 2nd phase constructor is private.
        */
        void ConstructL( const TDesC8& aName );

     
    private:    // Data
        //String pool to get string for references, not owned.
        ChspsDomStringPool& iStringPool;
        
        //Attribute name reference.
        TInt16         iNameRef;
        
        //Attribute value reference.
        TInt16         iValueRef;

    };

#endif      // hsps_DOM_ATTRIBUTE_H   
            
// End of File
