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
* Description:  List class to hold MhspsDomListItem objects.
*
*/



#ifndef hsps_DOM_LIST_H
#define hsps_DOM_LIST_H

//  INCLUDES
#include <e32base.h>
#include <s32strm.h>


// FORWARD DECLARATIONS
class MhspsDomListItem;
class ChspsDomStringPool;

// CLASS DECLARATION

/**
 *  Represents streamable list object, which holds MhspsDomListItem objects.
 *
 *  @lib hspsdomdocument.lib
 *  @since S60 5.0
 *  @ingroup group_hspsdom
 */
class ChspsDomList : public CBase
    {
    public:
        enum TListType
    		{
    		ENodeList=0,
    		EAttributeList    		
    		};
    public:  // Constructors and destructor
        
        /**
        * Two-phased constructor.
        */
        static ChspsDomList* NewL( TListType aListType, ChspsDomStringPool& aStringPool );
        
        /**
        * Two-phased stream constructor.
        */
        static ChspsDomList* NewL( 
            RReadStream& aStream, 
            ChspsDomStringPool& aStringPool );
        
        /**
        * Destructor.
        */
        virtual ~ChspsDomList();

    public: // Adding and removing
    	
    	/**
        * Frees memory allocated for all items in a list.
        * 
        * @since S60 5.0
        */
        IMPORT_C void Reset();
        
        /**
        * List owns items.
        * 
        * @since S60 5.0
        * @param aListItem to add.
        */
        IMPORT_C void AddItemL( MhspsDomListItem* aListItem );

        /**
        * List owns items.
        * 
        * @since S60 5.0
        * @param aListItem to add.
        * @param aIndex Index in list where the item is added.
        */
        IMPORT_C void AddItemL( MhspsDomListItem* aListItem, TInt aIndex );
        
        /**
        * Remove item from list.
        * 
        * @since S60 5.0
        * @param aIndex to aList item to remove.
        */
        IMPORT_C void RemoveItem( TInt aIndex );
        
        /**
        * Delete item from list.
        * 
        * @since S60 5.0
        * @param aIndex Item to delete.
        */
        IMPORT_C void DeleteItem( TInt aIndex );
        
        /**
        * Remove item from list.
        * 
        * @since S60 5.0
        * @param aItem Item to remove.
        */
        IMPORT_C void RemoveItem( MhspsDomListItem* aItem );
        
        /**
        * Delete item from list.
        * 
        * @since S60 5.0
        * @param aItem item to delete.
        */
        IMPORT_C void DeleteItem( MhspsDomListItem* aItem );
   
   public: // Accessing     
        /**
        * List items are accessed by index, i.e like arrays
        *  
        * @since S60 5.0
        * @param aIndex list item index.
        * @return Pointer to a a list item.
        */
        IMPORT_C MhspsDomListItem* Item( TInt aIndex )const;
        
        /**
        * Number of items in a list.
        * 
        * @since S60 5.0
        * @return Item count.
        */
        IMPORT_C TInt Length() const;
        
        /**
        * Get the first item in a list.
        * 
        * @since S60 5.0
        * @return Pointer to the first item in a list.
        */
        IMPORT_C MhspsDomListItem* First();
        
        /**
        * The last item in a list
        * 
        * @since S60 5.0
        * @return Pointer to the last item in a list.
        */
        IMPORT_C MhspsDomListItem* Last();
        
        /**
        * Find list item by item name. Searches list items 
        * which have name attribute.
        * 
        * @since S60 5.0
        * @param aName UTF8 encoded item's name to be searched
        * @return Pointer to the list item, NULL if not exist.
        */
        IMPORT_C MhspsDomListItem* FindByName( const TDesC8& aName );
        
        /**
        * Get item index
        * 
        * @since S60 5.0
        * @param aItem Item to be searched.
        * @return Item index or KErrNotFound if not found.
        */
        IMPORT_C TInt ItemIndex( const MhspsDomListItem& aItem )const;
        
        /**
        * Get reference to the dom's string pool.
        * 
        * @since S60 5.0
        * @return Reference to string pool.
        */
        IMPORT_C ChspsDomStringPool& StringPool() const;
        
        /**
        * Count items in a list.
        */
        TInt Count() const;
  public: // Serializing      
        
        /**
        * Get list's data size in bytes.
        * @since Series 60 3.1
        * @return Data size in bytes
        */
        TInt Size() const;
    
        /**
        * Externalize list's data.
        * @since Series 60 3.1
        * @param aStream Output stream
        */
        void ExternalizeL( RWriteStream& aStream ) const;
        
        /**
        * Internalize list's data.
        * @since Series 60 3.1
        * @param aStream Input stream
        */
        void InternalizeL( RReadStream& aStream );
        
   public: //Utility
   	   		
   		/**
        * Get list item type.
        * @since Series 60 3.1
        * @return List item type
        */
        TListType Type() const 
            {
            return iListType;
            }
            
        
   private:

        /**
        * C++ default constructor.
        */
        ChspsDomList( TListType aListType, ChspsDomStringPool& aStringPool );
        
        /**
        * By default Symbian 2nd phase constructor is private.
        */
        void ConstructL();
        
    protected:
        //List type    	
    	TListType                       iListType;
        
        //Items are stored into a segmented array
        RPointerArray<MhspsDomListItem>   iList;
    
    private:    // Data
        // StringPool, not owned
        ChspsDomStringPool&   iStringPool;
        
        
    };

#endif      // hsps_DOM_LIST_H  
            
// End of File
