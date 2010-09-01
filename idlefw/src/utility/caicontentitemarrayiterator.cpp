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


#include "caicontentitemarrayiterator.h"

CAiContentItemArrayIterator::CAiContentItemArrayIterator(
                                        const TAiContentItem* aArray,
                                        TInt aCount )
  : iArray( aArray ),
    iCount( aCount ),
    iIndex( 0 )
    {
    }

CAiContentItemArrayIterator* CAiContentItemArrayIterator::NewL(
                                        const TAiContentItem* aArray,
                                        TInt aCount )
	{
    CAiContentItemArrayIterator* self =
                        new( ELeave ) CAiContentItemArrayIterator( aArray, aCount );
    return self;
    }

CAiContentItemArrayIterator::~CAiContentItemArrayIterator()
    {
    }

void CAiContentItemArrayIterator::Release()
	{
	delete this;
	}

TBool CAiContentItemArrayIterator::HasNext() const
    {
    return ( iIndex < iCount );
    }

const TAiContentItem& CAiContentItemArrayIterator::NextL()
    {
    if ( iIndex >= iCount )
        {
        User::Leave( KErrOverflow );
        }
        
    return iArray[iIndex++];      
    }


const TAiContentItem& CAiContentItemArrayIterator::ItemL( TInt aId ) const
    {
    TInt i;
    
    for ( i = 0; i < iCount; ++i )
        {
        if ( iArray[i].id == aId )
            {
            break;
            }
        }

    if ( i >= iCount )
    	{
    	User::Leave( KErrNotFound );
    	}
    
    return iArray[i];
    }

const TAiContentItem& CAiContentItemArrayIterator::ItemL( const TDesC& aCid ) const
    {
    TInt i;
    
    for ( i = 0; i < iCount; ++i )
        {
        if ( ContentCid( iArray[i] ) == aCid )
            {
            break;
            }
        }
	
	if ( i >= iCount )
    	{
    	User::Leave( KErrNotFound );
    	}

	return iArray[i];
    }


void CAiContentItemArrayIterator::Reset()
    {
    iIndex = 0;
    }

