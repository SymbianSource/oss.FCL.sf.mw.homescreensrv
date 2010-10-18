/*
* Copyright (c) 2009 Nokia Corporation and/or its subsidiary(-ies).
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
#include "tsmodel.h"
#include "tsentryimp.h"
// -----------------------------------------------------------------------------
/**
 * Constructor
 * @param aModel - data owner
 * @param aOffset - data index
 */
TTsModelItem::TTsModelItem( const MTsModel& aModel, TInt aOffset )
:
iModel( aModel ),
iIndex( aOffset )

    {
    //No implementation required
    }

// -----------------------------------------------------------------------------
/**
 * Copy constructor
 * @param aItem - template item
 */
TTsModelItem::TTsModelItem( const TTsModelItem& aItem )
:
iModel( aItem.iModel ),
iIndex( aItem.iIndex )
    {
    //No implementation required
    }

// -----------------------------------------------------------------------------
/**
 * Validate item instance and retrieve display name data
 * @return item display name
 */
const TDesC& TTsModelItem::DisplayName() const
    {
    return iModel.DisplayName( iIndex );
    }

// -----------------------------------------------------------------------------
/**
 * Validate item instance and retrieve icon handle ( CFbsBitmap handle )
 * @return item icon handle
 */
TInt TTsModelItem::IconHandle() const
    {
    return iModel.IconHandle( iIndex );
    }

// -----------------------------------------------------------------------------
/**
 * Validate item instance and retrieve entry key
 * @return item key
 */
TTsEntryKey TTsModelItem::Key() const
    {
    return iModel.Key( iIndex );
    }

// -----------------------------------------------------------------------------
/**
 * Validate item instance and retrieve entry timestamp
 * @return item timestamp
 */
TTime TTsModelItem::Timestamp() const
    {
    return iModel.Timestamp( iIndex );
    }

// -----------------------------------------------------------------------------
/**
 * Validate item instance and retrieve timestamp
 * with latest update time
 * @return item timestamp
 */
TTime TTsModelItem::TimestampUpdate() const 
    {
    return iModel.TimestampUpdate(iIndex);
    }

// -----------------------------------------------------------------------------
/**
 * Validate item instance and retrieve activity status 
 * @return activity status
 */
TBool TTsModelItem::IsActive() const
    {
    return iModel.IsActive( iIndex );
    }

// -----------------------------------------------------------------------------
/**
 * Validate item instance and retrieve closable status 
 * @return closable status
 */
TBool TTsModelItem::IsClosable() const
    {
    return iModel.IsClosable( iIndex );
    }

// -----------------------------------------------------------------------------
/**
 * Validate item instance and forward close request to its owner 
 * @return EFalse on failure
 */
TBool TTsModelItem::Close() const
    {
    return iModel.Close( Key() );
    }

// -----------------------------------------------------------------------------
/**
 * Validate item instance and forward launch request to its owner 
 * @return EFalse on failure
 */
TBool TTsModelItem::Launch() const
    {
    return iModel.Launch( Key() );
    }

// -----------------------------------------------------------------------------
/**
 * @return EFalse if entry is not mandatory, other value in other cases  
 */
TBool TTsModelItem::IsMandatory() const
    {
    return iModel.IsMandatory( iIndex );
    }

// -----------------------------------------------------------------------------
/**
 * Validate item instance 
 */
TBool TTsModelItem::IsValid() const
    {
    return iModel.Count() > iIndex;
    }

// -----------------------------------------------------------------------------
/**
 * Serialize item into destination stream
 * @param aStream - output stream 
 */

void TTsModelItem::ExternalizeL( RWriteStream& aStream ) const
    {
    CTsEntryImp::ExternalizeL(aStream, *this);
    }

