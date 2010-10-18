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
 * Description :
 *
 */
#ifndef TSWINDOWGROUPMONITORIMP_H
#define TSWINDOWGROUPMONITORIMP_H

#include "tswindowgroupsmonitor.h"
#include "tsdatastorage.h"

class CTsRunningAppStorage;

/**
 * Window server monitor implementation.
 */
class CTsWindowGroupsMonitor: public CActive, 
                              public MTsWindowGroupsMonitor,
                              public MTsDataStorage
{
public:
    /**
     * Two phase constructor
     */
    static CTsWindowGroupsMonitor* NewL(MTsResourceManager &);
    
    /**
     * Destructor
     */
    ~CTsWindowGroupsMonitor();
    
    /**
     * @see MTsWindowGroupsMonitor::SubscribeL
     */
    void SubscribeL(MTsWindowGroupsObserver &);
    
    /**
     * @see MTsWindowGroupsMonitor::Cancel
     */
    void Cancel(MTsWindowGroupsObserver &);
    const MTsRunningApplicationStorage& Storage() const;

public://from MTsDataStorage
    TBool IsSupported(TInt aFunction) const;
    void HandleDataL(TInt aFunction, RReadStream& aDataStream);

protected:
    /**
     * @see CActive::RunL
     */
    void RunL();
    
    /**
     * @see CActive::DoCancel
     */
    void DoCancel();
    
    /**
     * @see CActive::RunError
     */
    TInt RunError(TInt error);

private:
    /**
     * First phase constructor
     */
    CTsWindowGroupsMonitor(MTsResourceManager &);
    
    /**
     * Second phase constructor
     */
    void ConstructL();
    
    /**
     * Function subscribe for event to window server and activate object
     */
    void Subscribe();
    
    /**
     * Function provide window server event to observers
     */
    void ProvideEventL();
    
    void RefreshCacheL();

private:
    /**
     * Registry of subscribed observers
     */
    RPointerArray<MTsWindowGroupsObserver> iObservers;
    
    /**
     * Resources manager
     */
    MTsResourceManager& iResources;
    
    /**
     * Monitor window group
     */
    RWindowGroup iWg;
    
    /**
     * 
     */
    CTsRunningAppStorage* iCache;
};
#endif//TSWINDOWGROUPMONITORIMP_H
