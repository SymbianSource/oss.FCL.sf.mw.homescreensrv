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
* Description:  Interface class for dom list items.
*
*/



#ifndef Mhsps_DOM_LIST_ITEM_H
#define Mhsps_DOM_LIST_ITEM_H

//  INCLUDES

// FORWARD DECLARATIONS


// CLASS DECLARATION

/**
*  Class represents a list item interface. Items are stored into
*  a ChspsDomList as a instance of this class.
*
*  @lib hspsdomdocument.lib
*  @since S60 5.0
*  @ingroup group_hspsdom
*/
class MhspsDomListItem
    {
    public:  // Destructor
        
        /**
        * Destructor.
        */
        virtual ~MhspsDomListItem(){}

    public: // Serializing
                        
        /**
        * This list item's data size in bytes.
        * 
        * @since S60 5.0
        * @return Data size in bytes.
        */
        virtual TInt Size() const=0;
        
        /**
        * Externalize list item.
        * 
        * @since S60 5.0
        * @param aStream Output stream
        */
        virtual void ExternalizeL( RWriteStream& aStream )const=0;
        
        /**
        * Internalize list item.
        * 
        * @since S60 5.0
        * @param aStream Input stream
        */
        virtual void InternalizeL( RReadStream& aStream )=0;
        
        
        
    public: //Accessing     
        /**
        * Get the name of this item as UTF8 encoded, if any.
        *  
        * @since S60 5.0
        * @return Name buffer or KNullDesC8
        */
        virtual const TDesC8& Name()=0;     
 
    };

#endif      // Mhsps_DOM_LIST_ITEM_H 
            
// End of File
