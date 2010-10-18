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
#ifndef TSSTORAGE_H
#define TSSTORAGE_H
#include <e32base.h>

#include "tsmodel.h"
#include "tsmodelobserver.h"
#include "tsdataobserver.h"
#include "tsdatastorage.h"

class CTsStorage: public CBase,
                  public MTsModel,
                  public MTsModelObserver,
                  public MTsDataObserver,
                  public MTsDataStorage
{
public:
    static CTsStorage* NewL(const TArray<MTsModel*> &aDataProviders);
    ~CTsStorage();
    
private:
    CTsStorage();
    void ConstructL(const TArray<MTsModel*> &aDataProviders);
    TInt ResetModel();
    void ResetModelL();
    void PullDataL(const MTsModel& aSrc);
    void ReorderDataL();
    void TrimDataL();

public://from MTsModelObserver
    void DataChanged(MTsModel& aModel);

public://from MTsModel
    TInt Count() const;
    void SetObserver(MTsModelObserver* aObserver);
    
private://from MTsModel
    const TDesC& DisplayName( TInt aOffset ) const;
    TInt IconHandle( TInt aOffset ) const;
    TTime Timestamp( TInt aOffset ) const;
    TTime TimestampUpdate(TInt offset) const;
    TTsEntryKey Key( TInt aOffset ) const;
    TBool IsActive( TInt aOffset ) const;
    TBool IsClosable( TInt aOffset ) const;
    TBool IsMandatory( TInt aOffset ) const;
    TBool Close( TTsEntryKey aKey ) const;
    TBool Launch( TTsEntryKey aKey ) const;
    TTsModelItem FindL( TTsEntryKey aKey ) const;

public://from MTsDataObserver
    void DataChanged();

public://from MTsDataStorage
    TBool IsSupported(TInt aFunction) const;
    void HandleDataL(TInt aFunction, RReadStream& aDataStream);

private:
    RPointerArray<MTsModel> iDataProviders;
    RArray<TTsModelItem> iData;
    MTsModelObserver* iDataObserver;

};

#endif //TSSTORAGE_H
