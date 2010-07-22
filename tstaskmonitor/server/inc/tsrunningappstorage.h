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
#ifndef RUNNINGAPPSTORAGE_H
#define RUNNINGAPPSTORAGE_H

// INCLUDES
#include <e32std.h>
#include <e32base.h>

#include "tsdataobserver.h"
#include "hsdataprovider.h"
#include "tsdatastorage.h"

class CTsDataList;
class MTsResourceManager;
class MTsWindowGroupsMonitor;

/**
 *  CRunningAppServer
 * 
 */
class CRunningAppStorage : public CBase,
                           public MHsDataProvider, 
                           public MHsDataObserver,
                           public MTsDataStorage 
{
public:
    /**
     * Destructor.
     */
    ~CRunningAppStorage();

    /**
     * Two-phased constructor.
     */
    static CRunningAppStorage* NewL(MHsDataObserver& observer,
                                    MTsResourceManager& resources,
                                    MTsWindowGroupsMonitor &wsMonitor);
    
public: //MHsDataObserver
    const TDesC8& Data() const; 
    
    //MHsDataChangeProvider
    void DataChanged();
    
    void UpdateL(TInt key,const CFbsBitmap& data, TInt param, TInt priority);
    
    void UpdateL(TInt key, const Visibility& data, TInt param);
    
    void RemoveL(TInt key, TInt param);

private:
    /**
     * Constructor for performing 1st stage construction
     */
    CRunningAppStorage(MHsDataObserver& observer);

    /**
     * Default constructor for performing 2nd stage construction
     */
    void ConstructL(MTsResourceManager& resources,
                    MTsWindowGroupsMonitor &wsMonitor);
    
    void DataChangedL();

private:
    MHsDataObserver& mObserver;
    RBuf8 mData;
    CTsDataList* mEngine;
};

#endif // RUNNINGAPPSERVER_H
