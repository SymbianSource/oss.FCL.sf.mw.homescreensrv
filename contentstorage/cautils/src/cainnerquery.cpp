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
 * Description:  ?Description
 *
 */

#include <badesca.h>
#include <s32strm.H>
#include "cadef.h"
#include "cainnerquery.h"

// ======== MEMBER FUNCTIONS ========

// ---------------------------------------------------------------------------
//
// ---------------------------------------------------------------------------
//
CCaInnerQuery::~CCaInnerQuery()
    {
    delete iEntryTypeNames;
    iIds.Close();
    }

// ---------------------------------------------------------------------------
//
// ---------------------------------------------------------------------------
//
EXPORT_C CCaInnerQuery* CCaInnerQuery::NewL()
    {
    CCaInnerQuery* entry = CCaInnerQuery::NewLC();
    CleanupStack::Pop( entry );
    return entry;
    }

// ---------------------------------------------------------------------------
//
// ---------------------------------------------------------------------------
//
EXPORT_C CCaInnerQuery* CCaInnerQuery::NewLC()
    {
    CCaInnerQuery* self = new ( ELeave ) CCaInnerQuery();
    CleanupStack::PushL( self );
    self->ConstructL();
    return self;
    }

// ---------------------------------------------------------------------------
//
// ---------------------------------------------------------------------------
//
EXPORT_C void CCaInnerQuery::ExternalizeL( RWriteStream& aStream ) const
    {
    aStream.WriteInt32L( iParentId );
    aStream.WriteInt32L( iRole );
    aStream.WriteUint32L( iFlagsOn );
    aStream.WriteUint32L( iFlagsOff );
    aStream.WriteUint32L( iUid );
    aStream.WriteUint32L( iSort );
    aStream.WriteUint32L( iCount );
    aStream.WriteUint32L( iIds.Count() );
    for( TInt i = 0; i < iIds.Count(); i++ )
        {
        aStream.WriteInt32L( iIds[i] );
        }

    aStream.WriteUint32L( iEntryTypeNames->MdcaCount() );
    for( TInt i = 0; i < iEntryTypeNames->MdcaCount(); i++ )
        {
        aStream.WriteInt32L( iEntryTypeNames->MdcaPoint( i ).Length() );
        aStream.WriteL( iEntryTypeNames->MdcaPoint( i ),
                iEntryTypeNames->MdcaPoint( i ).Length() );
        }
    }

// ---------------------------------------------------------------------------
//
// ---------------------------------------------------------------------------
//
EXPORT_C void CCaInnerQuery::InternalizeL( RReadStream& aStream )
    {
    iParentId = aStream.ReadInt32L();
    iRole = aStream.ReadInt32L();
    iFlagsOn = aStream.ReadUint32L();
    iFlagsOff = aStream.ReadUint32L();
    iUid = aStream.ReadUint32L();
    iSort = aStream.ReadUint32L();
    iCount = aStream.ReadUint32L();
    TInt count = aStream.ReadUint32L();
    iIds.Close();
    for( TInt i = 0; i < count; i++ )
        {
        iIds.AppendL( aStream.ReadInt32L() );
        }
    iEntryTypeNames->Reset();
    count = aStream.ReadUint32L();
    for( TInt i = 0; i < count; i++ )
        {
        TInt length = aStream.ReadInt32L();
        RBuf16 buf;
        CleanupClosePushL( buf );
        buf.CreateL( length );
        aStream.ReadL( buf, length );
        iEntryTypeNames->AppendL( buf );
        CleanupStack::PopAndDestroy( &buf );
        }
    }

// ---------------------------------------------------------------------------
//
// ---------------------------------------------------------------------------
//
void CCaInnerQuery::ConstructL()
    {
    iEntryTypeNames = new ( ELeave ) CDesC16ArrayFlat( KDefaultGranularity );
    iParentId = -1;
    //    TODO Item | Group
    iRole = CCaInnerQuery::Item | CCaInnerQuery::Group;
    }

// ---------------------------------------------------------------------------
//
// ---------------------------------------------------------------------------
//
CCaInnerQuery::CCaInnerQuery()
    {

    }

// ---------------------------------------------------------------------------
//
// ---------------------------------------------------------------------------
//
EXPORT_C const RArray<TInt>& CCaInnerQuery::GetIds() const
    {
    return iIds;
    }
// ---------------------------------------------------------------------------
//
// ---------------------------------------------------------------------------
//
EXPORT_C void CCaInnerQuery::SetIdsL( const RArray<TInt>& aArray )
    {
    for( TInt i( 0 ); i < aArray.Count(); ++i )
        {
        iIds.AppendL( aArray[i] );
        }
    }

// ---------------------------------------------------------------------------
//
// ---------------------------------------------------------------------------
//
EXPORT_C TInt CCaInnerQuery::GetParentId() const
    {
    return iParentId;
    }

// ---------------------------------------------------------------------------
//
// ---------------------------------------------------------------------------
//
EXPORT_C void CCaInnerQuery::SetParentId( TInt aId )
    {
    iParentId = aId;
    }

// ---------------------------------------------------------------------------
//
// ---------------------------------------------------------------------------
//
EXPORT_C const CDesC16ArrayFlat* CCaInnerQuery::GetEntryTypeNames() const
    {
    return iEntryTypeNames;
    }

// ---------------------------------------------------------------------------
//
// ---------------------------------------------------------------------------
//
EXPORT_C void CCaInnerQuery::SetEntryTypeNames( CDesC16ArrayFlat* aArray )
    {
    //TODO: Quickt setter for now, probably sth more convinient needed
    ASSERT( aArray );
    delete iEntryTypeNames;
    iEntryTypeNames = aArray;
    }

// ---------------------------------------------------------------------------
//
// ---------------------------------------------------------------------------
//
EXPORT_C TInt CCaInnerQuery::GetRole() const
    {
    return iRole;
    }

// ---------------------------------------------------------------------------
//
// ---------------------------------------------------------------------------
//
EXPORT_C void CCaInnerQuery::SetRole( TInt aRole )
    {
    iRole = aRole;
    }

// ---------------------------------------------------------------------------
//
// ---------------------------------------------------------------------------
//
EXPORT_C TUint CCaInnerQuery::GetFlagsOn() const
    {
    return iFlagsOn;
    }

// ---------------------------------------------------------------------------
//
// ---------------------------------------------------------------------------
//
EXPORT_C void CCaInnerQuery::SetFlagsOn( TUint aFlagsOn )
    {
    iFlagsOn = aFlagsOn;
    }

// ---------------------------------------------------------------------------
//
// ---------------------------------------------------------------------------
//
EXPORT_C TUint CCaInnerQuery::GetFlagsOff() const
    {
    return iFlagsOff;
    }

// ---------------------------------------------------------------------------
//
// ---------------------------------------------------------------------------
//
EXPORT_C void CCaInnerQuery::SetFlagsOff( TUint aFlagsOff )
    {
    iFlagsOff = aFlagsOff;
    }
// ---------------------------------------------------------------------------
//
// ---------------------------------------------------------------------------
//
EXPORT_C TUint CCaInnerQuery::GetUid() const
    {
    return iUid;
    }

// ---------------------------------------------------------------------------
//
// ---------------------------------------------------------------------------
//
EXPORT_C void CCaInnerQuery::SetUid( TUint aUid )
    {
    iUid = aUid;
    }

// ---------------------------------------------------------------------------
//
// ---------------------------------------------------------------------------
//
EXPORT_C CCaInnerQuery::TSortAttribute CCaInnerQuery::GetSort() const
    {
    return (TSortAttribute ) iSort;
    }

// ---------------------------------------------------------------------------
//
// ---------------------------------------------------------------------------
//
EXPORT_C void CCaInnerQuery::SetSort( CCaInnerQuery::TSortAttribute aSort )
    {
    iSort = (TInt ) aSort;
    }

// ---------------------------------------------------------------------------
//
// ---------------------------------------------------------------------------
//
EXPORT_C void CCaInnerQuery::SetCount( TUint aCount )
    {
    iCount = aCount;
    }

// ---------------------------------------------------------------------------
//
// ---------------------------------------------------------------------------
//
EXPORT_C TUint CCaInnerQuery::GetCount() const
    {
    return iCount;
    }
