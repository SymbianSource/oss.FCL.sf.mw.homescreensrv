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

#include <AknDef.h>
#include <apgwgnam.h>
#include <apgtask.h>

#include "tsrunningappmodel.h"
#include "tsrunningappstorageimp.h"
#include "tsdatalist.h"
#include "tsmodelobserver.h"
#include "tsscreenshotprovider.h"
#include "tsresourcemanager.h"

CTsRunningAppModel *CTsRunningAppModel::NewL( MTsResourceManager& aResources, 
                                              TsEnv& aEnv )
    {
    CTsRunningAppModel *self = CTsRunningAppModel::NewLC( aResources, aEnv );
    CleanupStack::Pop( self );
    return self;
    }

CTsRunningAppModel *CTsRunningAppModel::NewLC( MTsResourceManager& aResources, 
                                               TsEnv& aEnv )
    {
    CTsRunningAppModel *self = new (ELeave) CTsRunningAppModel( aResources );
    CleanupStack::PushL( self );
    self->ConstructL( aResources, aEnv );
    return self;
    }

CTsRunningAppModel::~CTsRunningAppModel()
    {
    delete iDataList;
    delete iScreenshotProvider;
    }
    
CTsRunningAppModel::CTsRunningAppModel( MTsResourceManager& aResources ) 
: 
    iResources( aResources )
    {
    }

void CTsRunningAppModel::ConstructL( MTsResourceManager& aResources, 
                                     TsEnv& aEnv)
    {
    iDataList = CTsDataList::NewL( aResources, *this, aEnv );
    iScreenshotProvider = 
        CTsScreenshotProvider::NewL(*iDataList, aResources.WsMonitor());
    
    RArray<RWsSession::TWindowGroupChainInfo> wgList;
    CleanupClosePushL( wgList );
    User::LeaveIfError( aResources.WsSession().WindowGroupList( 0, &wgList ) );
    CTsRunningAppStorage* storage = CTsRunningAppStorage::NewLC();
    storage->HandleWindowGroupChanged(aResources, wgList.Array());
    iDataList->HandleWindowGroupChanged( aResources, *storage );
    CleanupStack::PopAndDestroy(storage);
    CleanupStack::PopAndDestroy( &wgList );
    }

// --------------------------------------------------------------------------
// MHsDataObserver interface implementation
// --------------------------------------------------------------------------
//
void CTsRunningAppModel::DataChanged()
    {
    if (iObserver)
        {
        iObserver->DataChanged( *this );
        }
    }

// --------------------------------------------------------------------------
// MTsTaskSettings interface implementation
// --------------------------------------------------------------------------
//
TBool CTsRunningAppModel::IsSupported( TInt aFunction ) const
    {
    return iDataList->IsSupported( aFunction );
    }

void CTsRunningAppModel::HandleDataL( TInt aFunction, RReadStream& aDataStream )
    {
    iDataList->HandleDataL( aFunction, aDataStream );
    }

// --------------------------------------------------------------------------
// MTsModel interface implementation
// --------------------------------------------------------------------------
//
TInt CTsRunningAppModel::Count() const
    {
    return iDataList->Data().Count();
    }

void CTsRunningAppModel::SetObserver( MTsModelObserver* aObserver )
    {
    iObserver = aObserver;
    }

const TDesC& CTsRunningAppModel::DisplayName( TInt aOffset ) const
    {
    return iDataList->Data()[aOffset]->DisplayName();
    }

TInt CTsRunningAppModel::IconHandle( TInt aOffset ) const
    {
    return iDataList->Data()[aOffset]->IconHandle();
    
    }

TTime CTsRunningAppModel::Timestamp( TInt aOffset ) const
    {
    return iDataList->Data()[aOffset]->Timestamp();
    }

TTime CTsRunningAppModel::TimestampUpdate(TInt offset) const
{
    return iDataList->Data()[offset]->TimestampUpdate();
}

TTsEntryKey CTsRunningAppModel::Key( TInt aOffset ) const
    {
    return TTsEntryKey(iDataList->Data()[aOffset]->Key().Key(), 
                       reinterpret_cast<TInt>(this));
    }

TBool CTsRunningAppModel::IsActive( TInt /*aOffset*/ ) const
    {
    return ETrue;
    }

TBool CTsRunningAppModel::IsClosable( TInt aOffset ) const
    {
    return iDataList->Data()[aOffset]->IsClosable();
    }

TBool CTsRunningAppModel::IsMandatory( TInt /*aOffset*/ ) const
    {
    return ETrue;
    }

TBool CTsRunningAppModel::Close( TTsEntryKey aKey ) const
    {
    TBool retVal(EFalse);
    TRAP_IGNORE(retVal = iDataList->FindL(aKey).Close());
    return retVal;
    }

TBool CTsRunningAppModel::Launch( TTsEntryKey aKey ) const 
    {
    TBool retVal(EFalse);
    TRAP_IGNORE(retVal = iDataList->FindL(aKey).Launch());
    return retVal;
    }
