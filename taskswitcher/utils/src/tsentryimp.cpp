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

#include <s32strm.h>
#include <fbs.h>

#include "tsentryimp.h"

// --------------------------------------------------------------------------
CTsEntryImp::~CTsEntryImp()
    {
    delete iName;
    delete iThumbail;
    }

// --------------------------------------------------------------------------
void CTsEntryImp::BaseConstructL(RReadStream& aStream)
    {
    aStream >> ( *this );
    }

// --------------------------------------------------------------------------
TBool CTsEntryImp::IsActive() const
    {
    return iIsActive;
    }

// --------------------------------------------------------------------------
/**
 * Application name.
 */
const TDesC& CTsEntryImp::DisplayName() const
    {
    return iName ? *iName : KNullDesC();
    }

// --------------------------------------------------------------------------
/**
 * ETrue if the application is closeable
 */
TBool CTsEntryImp::IsClosable() const
    {
    return iCloseableApp;
    }

// --------------------------------------------------------------------------
/**
 * Application icon bitmap
 */
TInt CTsEntryImp::IconHandle() const
    {
    return iThumbail ? iThumbail->Handle() : KErrNotFound;
    }

// --------------------------------------------------------------------------
/**
 * Entry's key
 */
TTsEntryKey CTsEntryImp::Key() const
    {
    return iKey;
    }

// --------------------------------------------------------------------------
TTime CTsEntryImp::Timestamp() const
    {
    return iTimestamp;
    }

// --------------------------------------------------------------------------
/**
 * Timestamp of last entry change 
 */
TTime CTsEntryImp::TimestampUpdate() const
    {
    return iUpdateTimestamp;
    }

// --------------------------------------------------------------------------
TBool CTsEntryImp::IsMandatory() const
    {
    return ETrue;
    }

// --------------------------------------------------------------------------
void CTsEntryImp::ExternalizeL( RWriteStream& aDst, const MTsEntry& aSrc )
    {
    aDst.WriteInt32L( aSrc.DisplayName().Length() );
    if( 0 < aSrc.DisplayName().Length() )
        {
        aDst << aSrc.DisplayName();
        }
    TPckgBuf<TTime> timestamp(aSrc.TimestampUpdate()); 
    aDst.WriteL(timestamp);
    aDst.WriteInt32L( aSrc.IconHandle() );
    aDst.WriteInt32L( TTsEntryKey::Size() );
    aDst << aSrc.Key();
    aDst.WriteInt32L( aSrc.IsActive() );
    aDst.WriteInt32L( aSrc.IsClosable() );
    }

// --------------------------------------------------------------------------
void CTsEntryImp::InternalizeL( RReadStream& aSrc )
    {
    TInt lenght = aSrc.ReadInt32L();
    delete iName;
    iName = 0;
    if(0 < lenght)
        {
        iName = HBufC::NewL(aSrc, lenght);
        }
    
    TPckgBuf<TTime> timestamp;
    aSrc.ReadL(timestamp);
    iUpdateTimestamp = timestamp();
    
    delete iThumbail;
    iThumbail = 0;
    TInt handle = aSrc.ReadInt32L();
    if(KErrNotFound != handle)
        {
        iThumbail = new(ELeave)CFbsBitmap();
        User::LeaveIfError(iThumbail->Duplicate(handle));
        }
    aSrc.ReadInt32L();
    aSrc >> iKey;
    
    iIsActive = aSrc.ReadInt32L();
    iCloseableApp = aSrc.ReadInt32L();
    }
// end of file
