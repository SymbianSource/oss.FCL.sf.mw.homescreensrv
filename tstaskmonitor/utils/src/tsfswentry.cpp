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

#define __E32SVR_H__
#include "tsfswentry.h"
#include <s32strm.h>
#include <fbs.h>

// --------------------------------------------------------------------------
// CTsFswEntry::NewL
// --------------------------------------------------------------------------
//
CTsFswEntry* CTsFswEntry::NewL()
    {
    CTsFswEntry* self = NewLC();
    CleanupStack::Pop( self );
    return self;
    }

// --------------------------------------------------------------------------
// CTsFswEntry::NewLC
// --------------------------------------------------------------------------
//
CTsFswEntry* CTsFswEntry::NewLC()
    {
    CTsFswEntry* self = new ( ELeave ) CTsFswEntry;
    CleanupStack::PushL( self );
    return self;
    }

// --------------------------------------------------------------------------
// CTsFswEntry::~CTsFswEntry
// --------------------------------------------------------------------------
//
CTsFswEntry::~CTsFswEntry()
    {
    delete iAppName;
    delete iAppIconBitmap;
    delete iAppIconMask;
    }

// --------------------------------------------------------------------------
// CTsFswEntry::SetWgId
// --------------------------------------------------------------------------
//
void CTsFswEntry::SetWgId( TInt aWgId )
    {
    iWgId = aWgId;
    }

// --------------------------------------------------------------------------
// CTsFswEntry::SetAppUid
// --------------------------------------------------------------------------
//
void CTsFswEntry::SetAppUid( const TUid& aUid )
    {
    iAppUid = aUid;
    }

// --------------------------------------------------------------------------
// CTsFswEntry::SetAppNameL
// --------------------------------------------------------------------------
//
void CTsFswEntry::SetAppNameL( const TDesC& aAppName )
    {
    delete iAppName; iAppName = 0;
    iAppName = aAppName.AllocL();
    }

// --------------------------------------------------------------------------
// CTsFswEntry::SetSystemApp
// --------------------------------------------------------------------------
//
void CTsFswEntry::SetCloseableApp( TBool aValue )
    {
    iCloseableApp = aValue;
    }

// --------------------------------------------------------------------------
// CTsFswEntry::SetAppIconHandles
// --------------------------------------------------------------------------
//
void CTsFswEntry::SetAppIcon( CFbsBitmap* aBitmap, CFbsBitmap* aMask )
    {
    iAppIconBitmap = aBitmap;
    iAppIconMask = aMask;
    }

// --------------------------------------------------------------------------
// CTsFswEntry::WgId
// --------------------------------------------------------------------------
//
TInt CTsFswEntry::WgId() const
    {
    return iWgId;
    }

// --------------------------------------------------------------------------
// CTsFswEntry::AppUid
// --------------------------------------------------------------------------
//
TUid CTsFswEntry::AppUid() const
    {
    return iAppUid;
    }

// --------------------------------------------------------------------------
// CTsFswEntry::AppName
// --------------------------------------------------------------------------
//
const TDesC& CTsFswEntry::AppName() const
    {
    return iAppName ? *iAppName : KNullDesC();
    }

// --------------------------------------------------------------------------
// CTsFswEntry::SystemApp
// --------------------------------------------------------------------------
//
TBool CTsFswEntry::CloseableApp() const
    {
    return iCloseableApp;
    }

// --------------------------------------------------------------------------
// CTsFswEntry::AppIconBitmapHandle
// --------------------------------------------------------------------------
//
CFbsBitmap* CTsFswEntry::AppIconBitmap() const
    {
    return iAppIconBitmap;
    }

// --------------------------------------------------------------------------
// CTsFswEntry::AppIconMaskHandle
// --------------------------------------------------------------------------
//
CFbsBitmap* CTsFswEntry::AppIconMask() const
    {
    return iAppIconMask;
    }

// --------------------------------------------------------------------------
// CTsFswEntry::ExternalizeL
// --------------------------------------------------------------------------
//
void CTsFswEntry::ExternalizeL( RWriteStream& aStream )
    {
    aStream.WriteInt32L( iWgId );
    aStream.WriteInt32L( iAppUid.iUid );
    aStream << AppName();
    aStream.WriteInt32L( iCloseableApp );
    if ( iAppIconBitmap )
        {
        aStream.WriteInt32L( iAppIconBitmap->Handle() );
        }
    else
        {
        aStream.WriteInt32L( KErrNotFound );
        }
    
   if ( iAppIconMask )
        {
        aStream.WriteInt32L( iAppIconMask->Handle() );
        }
    else
        {
        aStream.WriteInt32L( KErrNotFound );
        }
    }

// --------------------------------------------------------------------------
// CTsFswEntry::InternalizeL
// --------------------------------------------------------------------------
//
void CTsFswEntry::InternalizeL( RReadStream& aStream )
    {
    iWgId = aStream.ReadInt32L();
    iAppUid = TUid::Uid( aStream.ReadInt32L() );
    delete iAppName; iAppName = NULL;
    iAppName = HBufC::NewL( aStream, KMaxTInt );
    iCloseableApp = aStream.ReadInt32L();
    TInt appIconBitmapHandle = aStream.ReadInt32L();
    TInt appIconMaskHandle = aStream.ReadInt32L();
    if ( appIconBitmapHandle != KErrNotFound )
        {
        iAppIconBitmap = new (ELeave) CFbsBitmap;
        iAppIconBitmap->Duplicate( appIconBitmapHandle );  
        }
    if ( appIconMaskHandle != KErrNotFound )    
        {
        iAppIconMask = new (ELeave) CFbsBitmap;
        iAppIconMask->Duplicate( appIconMaskHandle );  
        }
    }

// --------------------------------------------------------------------------
// CTsFswEntry::ExternalizeArrayL
// --------------------------------------------------------------------------
//
void CTsFswEntry::ExternalizeArrayL( RWriteStream& aStream,
        const RTsFswArray& aArray )
    {
    TInt count = aArray.Count();
    aStream.WriteInt32L( count );
    for ( TInt i = 0; i < count; ++i )
        {
        aArray[i]->ExternalizeL( aStream );
        }
    }

// --------------------------------------------------------------------------
// CTsFswEntry::InternalizeArrayL
// --------------------------------------------------------------------------
//
void CTsFswEntry::InternalizeArrayL( RReadStream& aStream,
        RTsFswArray& aArray )
    {
    aArray.ResetAndDestroy();
    TInt count = aStream.ReadInt32L();    for ( TInt i = 0; i < count; ++i )
        {
        CTsFswEntry* entry = CTsFswEntry::NewLC();
        entry->InternalizeL( aStream );
        aArray.AppendL( entry );
        CleanupStack::Pop( entry );
        }
    }

// --------------------------------------------------------------------------
// CTsFswEntry::Type
// --------------------------------------------------------------------------
//
TAppType CTsFswEntry::Type() const
    {
    return EApp;
    }
// end of file
