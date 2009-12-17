/*
* Copyright (c) 2005-2006 Nokia Corporation and/or its subsidiary(-ies).
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



#ifndef C_AIARRAYITERATOR_H
#define C_AIARRAYITERATOR_H


#include <e32base.h>
#include <aicontentmodel.h>

/**
 *  @ingroup group_aiutils
 * 
 *  Content item array iterator.
 *
 *  Iterator implementation for content item arrays
 *  Usage example:
 *
 *      const TAiContentItem KExPluginContent[] =
 *          {
 *              { 1, "Counter", "text/plain" },
 *              { 2, "Status",  "text/plain" }
 *          };
 *
 *      CAiContentItemArrayIterator* iContent =
 *          CreateIteratorL( KExPluginContent );
 *
 *      while( iContent->HasNext() )
 *          {
 *          TAiContentItem& myItem = iContent->NextL();
 *          // My ops using myItem
 *          }
 *
 *  @since S60 3.1
 */
NONSHARABLE_CLASS(CAiContentItemArrayIterator) : 
    public CBase, public MAiContentItemIterator
    {
public:
    
    /**
     * Creates a new iterator instance from a TAiContentItem array.
     *
     * @param aArray content item array.
     * @param aCount  number of content items in aArray.
     * @return a new iterator object for aArray.
     * @see CreateIteratorL
     * @since S60 3.2
     */
    static CAiContentItemArrayIterator* NewL
        ( const TAiContentItem* aArray, TInt aCount );
        
    virtual ~CAiContentItemArrayIterator();
    
    void Release();

// from base class MAiContentItemIterator

    TBool HasNext() const;

    const TAiContentItem& NextL();

    const TAiContentItem& ItemL(TInt aId) const;
    
	const TAiContentItem& ItemL( const TDesC& aCid ) const;

    void Reset();

private:

     CAiContentItemArrayIterator( const TAiContentItem* aArray, TInt aCount );

private: // data

    /**
     * Array of items
     * Not Own.
     */
    const TAiContentItem*   iArray;

    /**
     * Total number of items
     */
    TInt                    iCount;

    /**
     * Current iterator index
     */
    TInt                    iIndex;
    };


#endif // C_CAiContentItemArrayIterator_H
