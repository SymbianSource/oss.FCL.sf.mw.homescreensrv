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
#include <tstaskmonitorglobals.h>
#include "tsstorage.h"
#include "tsmodelitemkeymsg.h"

const TInt KTsDataLimit(10);
// -----------------------------------------------------------------------------
/**
 * Two phase construction. Create and initialize storage instance.
 * @param aDataProviders - list of data providers
 * @return storage instane
 */
CTsStorage* CTsStorage::NewL(const TArray<MTsModel*> &aDataProviders)
    {
    CTsStorage* self = new(ELeave) CTsStorage();
    CleanupStack::PushL( self );
    self->ConstructL( aDataProviders );
    CleanupStack::Pop( self );
    return self;
    }

// -----------------------------------------------------------------------------
/**
 * Destructor. Function cancel subscribtion for data change notyfications
 */
CTsStorage::~CTsStorage()
    {
    for( TInt iter(0); iter < iDataProviders.Count(); ++iter )
        {
        iDataProviders[iter]->SetObserver( 0 );
        }
    iData.Close();
    }

// -----------------------------------------------------------------------------
/**
 * First phase construction.
 */
CTsStorage::CTsStorage()
    {
    //No implementation required
    }

// -----------------------------------------------------------------------------
/**
 * Second phase construction. Function make subscribtion for data changes notifications
 * @param aDataProviders - list of data providers
 */
void CTsStorage::ConstructL( const TArray<MTsModel*> &aDataProviders )
    {
    for (TInt iter(0); iter < aDataProviders.Count(); ++iter)
        {
        iDataProviders.AppendL( aDataProviders[iter] );
        aDataProviders[iter]->SetObserver( this );
        }
    }

// -----------------------------------------------------------------------------
/**
 * Interface implementation
 * @see MTsDataObserver::DataChanged()
 */
void CTsStorage::DataChanged()
    {
    ResetModel();
    }

// -----------------------------------------------------------------------------
TBool CTsStorage::IsSupported( TInt aFunction ) const
    {
    return ( OpenTaskMessage == aFunction || CloseTaskMessage == aFunction );
    }

// -----------------------------------------------------------------------------
void CTsStorage::HandleDataL( TInt aFunction, RReadStream& aDataStream )
    {
    if( !IsSupported(aFunction) ) 
        {
        User::Leave( KErrCorrupt );
        }
    CTsModelItemKeyMsg* msg = CTsModelItemKeyMsg::NewLC( aDataStream );
    OpenTaskMessage == aFunction ? Launch( msg->Key() ) : Close( msg->Key() );
    CleanupStack::PopAndDestroy( msg );
    }

// -----------------------------------------------------------------------------
/**
 * Interface implementation
 * @see MTsModelObserver::dataChanged(MTsModel &)
 */
void CTsStorage::DataChanged(MTsModel &/*model*/)
    {
    ResetModel();
    }

// -----------------------------------------------------------------------------
/**
 * Interface implementation
 * @see MTsModel::count()
 */
TInt CTsStorage::Count() const
    {
    return iData.Count();
    }

// -----------------------------------------------------------------------------
/**
 * Interface implementation
 * @see MTsModel::SetObserver(MTsModelObserver *)
 */
void CTsStorage::SetObserver( MTsModelObserver* aObserver ) 
    {
    iDataObserver = aObserver;
    }

// -----------------------------------------------------------------------------
/**
 * Interface implementation
 * @see MTsModel::DisplayNameL(TInt)
 */
const TDesC& CTsStorage::DisplayName( TInt aOffset ) const 
    {
    return iData[aOffset].DisplayName();
    }

// -----------------------------------------------------------------------------
/**
 * Interface implementation
 * @see MTsModel::IconHandleL(TInt)
 */
TInt CTsStorage::IconHandle( TInt aOffset ) const 
    {
    return iData[aOffset].IconHandle();
    }

// -----------------------------------------------------------------------------
/**
 * Interface implementation
 * @see MTsModel::TimestampL(TInt)
 */
TTime CTsStorage::Timestamp( TInt aOffset ) const 
    {
    return iData[aOffset].Timestamp();
    }

// -----------------------------------------------------------------------------
/**
 * Interface implementation
 * @see MTsModel::TimestampUpdateL(TInt)
 */
TTime CTsStorage::TimestampUpdate( TInt offset ) const 
{
    return iData[offset].TimestampUpdate();
}

// -----------------------------------------------------------------------------
/**
 * Interface implementation
 * @see MTsModel::Key(TInt)
 */
TTsEntryKey CTsStorage::Key( TInt aoffset ) const 
    {
    return iData[aoffset].Key();
    }

// -----------------------------------------------------------------------------
/**
 * Interface implementation
 * @see MTsModel::IsActive(TInt)
 */
TBool CTsStorage::IsActive( TInt aOffset ) const 
    {
    return iData[aOffset].IsActive();
    }

// -----------------------------------------------------------------------------
/**
 * Interface implementation
 * @see MTsModel::IsClosableL(TInt)
 */
TBool CTsStorage::IsClosable( TInt aOffset ) const 
    {
    return iData[aOffset].IsClosable();
    }

// -----------------------------------------------------------------------------
/**
 * Interface implementation
 * @see MTsModel::IsMandatoryL(TInt) const
 */
TBool CTsStorage::IsMandatory( TInt aOffset ) const
    {
    return iData[aOffset].IsMandatory();
    }

// -----------------------------------------------------------------------------
/**
 * Interface implementation
 * @see MTsModel::Close(TTsModelItemKey)
 */
TBool CTsStorage::Close( TTsEntryKey aKey ) const 
    {
    TBool retVal(EFalse);
    TRAP_IGNORE(retVal = FindL(aKey).Close())
    return retVal;
    }

// -----------------------------------------------------------------------------
/**
 * Interface implementation
 * @see MTsModel::Launch(TTsModelItemKey)
 */
TBool CTsStorage::Launch(TTsEntryKey aKey) const 
    {
    TBool retVal(EFalse);
    TRAP_IGNORE(retVal = FindL(aKey).Launch())
    return retVal;
    }

// -----------------------------------------------------------------------------
TTsModelItem CTsStorage::FindL( TTsEntryKey aKey ) const
    {
    for( TInt offset(0); offset < iData.Count(); ++offset ) 
        {
        if( iData[offset].Key() == aKey )
            {
            return iData[offset];
            }
        }
    User::Leave(KErrNotFound);
    return ItemL(0);//just avoid compilation warnings
    }

// -----------------------------------------------------------------------------
/**
 * Retrieve shallow copy of data from known data providers and sort entries
 */
TInt CTsStorage::ResetModel()
    {
    TRAPD(errNo, ResetModelL());
    return errNo;
    }

// -----------------------------------------------------------------------------
/**
 * Retrieve shallow copy of data from known data providers and sort entries
 */
void CTsStorage::ResetModelL() 
    {
    iData.Reset();
    for(TInt iter(0); iter < iDataProviders.Count(); ++iter )
        {
        PullDataL(*(iDataProviders[iter]));
        }
    ReorderDataL();
    if (0 != iDataObserver)
        {
        iDataObserver->DataChanged(*this);
        }
    }

// -----------------------------------------------------------------------------
/**
 * Retrieve shallow copy of data from source model
 * @param aSrc - source model 
 */
void CTsStorage::PullDataL( const MTsModel& aSrc )
    {
    for( TInt iter(0); iter < aSrc.Count(); ++iter )
        {
        iData.AppendL(aSrc.ItemL(iter));
        }
    }

// -----------------------------------------------------------------------------
/**
 * Sort internal data model 
 */
void CTsStorage::ReorderDataL()
    {
    for( TInt prev(0); prev < iData.Count(); ++prev ) 
        {
        for( TInt next(prev + 1); next < iData.Count(); ++next )
            {
            const TTsModelItem prevItem(iData[prev]), nextItem(iData[next]);
            if( ( !prevItem.IsMandatory() && nextItem.IsMandatory() ) ||
                ( prevItem.Timestamp() < nextItem.Timestamp() && prevItem.IsMandatory() == nextItem.IsMandatory() ) )
                {
                iData.Remove(prev);
                iData.InsertL(nextItem, prev);
                
                iData.Remove(next);
                iData.InsertL(prevItem, next);
                }
            }
        }
    TrimDataL();
    }

// -----------------------------------------------------------------------------
void CTsStorage::TrimDataL()
    {
    const TInt lastItemOffset(iData.Count() -1);
    if(KTsDataLimit <= lastItemOffset && !iData[lastItemOffset].IsMandatory())
        {
        iData.Remove(lastItemOffset);
        TrimDataL();
        }
    }
