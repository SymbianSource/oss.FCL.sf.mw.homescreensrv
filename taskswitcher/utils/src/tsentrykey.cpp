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
* Description:  Task list entry
 *
*/


#include "tsentrykey.h"

// -----------------------------------------------------------------------------
/**
 * @return amount of memory ( number of bytes ) required to store key.
 */
TInt TTsEntryKey::Size()
    {
    return sizeof( TInt ) * 2;
    }
// -----------------------------------------------------------------------------
/**
 * Default constructor. Key members are initialized with 0
 */
TTsEntryKey::TTsEntryKey()
:
    iKey( 0 ),
    iRoot( 0 )
    {
    //No implementation required
    }

// -----------------------------------------------------------------------------
/**
 * Constructor. Initialize members with provided values.
 * @param aKey - key value
 * @param aRoot - root value
 */
TTsEntryKey::TTsEntryKey( TTsKey aKey, TInt aRoot )
:
    iKey( aKey ),
    iRoot( aRoot )
    {
    //No implementation required
    }

// -----------------------------------------------------------------------------
/**
 * Copy constructor
 * @param aKey - key value that need to be duplicated
 */
TTsEntryKey::TTsEntryKey( const TTsEntryKey& aKey )
:
    iKey( aKey.iKey ),
    iRoot( aKey.iRoot )
    {
    //No implementation required
    }

// -----------------------------------------------------------------------------
/**
 * Assignment operator. Initialize members with new values
 * @param aKey - key value that need to be duplicated
 * @return reference to key instance
 */
TTsEntryKey& TTsEntryKey::operator =( const TTsEntryKey& aKey )
    {
    iKey = aKey.iKey;
    iRoot = aKey.iRoot;
    return (*this);
    }

// -----------------------------------------------------------------------------
/**
 * Comparison operator
 * @param aKey - compared value
 * @return EFalse if values are not equal, other value if they match
 */
TBool TTsEntryKey::operator == ( const TTsEntryKey aKey ) const
    {
    return ( iKey == aKey.iKey && iRoot == aKey.iRoot );
    }

// -----------------------------------------------------------------------------
/**
 * Enable access to key value
 * @return key value
 */
TInt TTsEntryKey::Key() const
    {
    return iKey;
    }

// -----------------------------------------------------------------------------
/**
 * Serialize key into data stream
 * @param aStream - destination binary stream
 */
void TTsEntryKey::ExternalizeL( RWriteStream& aStream ) const
    {
    aStream.WriteInt32L( iKey );
    aStream.WriteInt32L( iRoot );
    }

// -----------------------------------------------------------------------------
/**
 * Deserialize key from data stream
 * @param aStream - source binary stream
 */
void TTsEntryKey::InternalizeL( RReadStream& aStream )
    {
    iKey = aStream.ReadInt32L();
    iRoot = aStream.ReadInt32L();
    }
