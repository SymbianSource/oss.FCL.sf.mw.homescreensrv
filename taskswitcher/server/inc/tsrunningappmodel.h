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
#ifndef TSRUNNINGAPPMODEL_H
#define TSRUNNINGAPPMODEL_H

#include "tsmodel.h"
#include "tsdatastorage.h"
#include "tsdataobserver.h"

class MTsResourceManager;
class CTsDataList;
class CTsScreenshotProvider;
class TsEnv;

NONSHARABLE_CLASS(CTsRunningAppModel) : public CBase, 
                                        public MTsModel, 
                                        public MTsDataStorage, 
                                        public MTsDataObserver
{

public:
    static CTsRunningAppModel *NewL( MTsResourceManager &aResources, 
                                     TsEnv& aEnv );
    static CTsRunningAppModel *NewLC( MTsResourceManager &aResources, 
                                      TsEnv& aEnv );
    ~CTsRunningAppModel();
    
private:
    CTsRunningAppModel( MTsResourceManager &aResources );
    void ConstructL( MTsResourceManager &aResources, 
                     TsEnv& aEnv);

public: // MHsDataObserver interface implementation
    virtual void DataChanged();

public: // MTsDataStorage interface implementation
    TBool IsSupported( TInt aFunction ) const;
    void HandleDataL( TInt aFunction, RReadStream& aDataStream );
    
public:
    virtual TInt Count() const;
    virtual void SetObserver( MTsModelObserver *aObserver );

public:
    virtual const TDesC& DisplayName( TInt aOffset ) const;
    virtual TInt IconHandle( TInt aOffset ) const;
    virtual TTime Timestamp( TInt aOffset ) const;
    virtual TTime TimestampUpdate(TInt offset) const; 
    virtual TTsEntryKey Key( TInt aOffset ) const;
    virtual TBool IsActive( TInt aOffset ) const;
    virtual TBool IsClosable( TInt aOffset ) const;
    virtual TBool IsMandatory( TInt aOffset ) const;

public:
    virtual TBool Close( TTsEntryKey aKey ) const;
    virtual TBool Launch( TTsEntryKey aKey ) const;

private: // owned
    CTsDataList *iDataList;
    
private: // not owned
    MTsModelObserver* iObserver;
    CTsScreenshotProvider* iScreenshotProvider;
    MTsResourceManager& iResources;

};

#endif //TSRUNNINGAPPMODEL_H
