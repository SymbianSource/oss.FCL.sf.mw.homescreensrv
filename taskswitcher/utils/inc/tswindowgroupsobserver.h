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
#ifndef TSWINDOWGROUPSOBSERVER_H
#define TSWINDOWGROUPSOBSERVER_H

#include "tsresourcemanager.h"

/**
 * Interface declare mathods to notify about window server events
 */
class MTsWindowGroupsObserver
{
public:
    /**
     * Method notidy about window group changes.
     * @param rsc - resource manager
     * @param wgs - list of window groups associated with running applications
     */
    virtual void HandleWindowGroupChanged(MTsResourceManager &rsc, 
                                          const TArray<RWsSession::TWindowGroupChainInfo> & wgs) =0;
};

/**
 * Interface declare methods to subscribe windo server events
 */
class MTsWindowGroupsMonitor
{
public:
    /**
     * Method make subscription for window server events
     * @param observer - events observer
     */
    virtual void SubscribeL(MTsWindowGroupsObserver & observer) =0;
    
    /**
     * Method cancel subscription for window server events
     * @param observer - events observer
     */
    virtual void Cancel(MTsWindowGroupsObserver &) =0;
};

/**
 * Window server observer implementation. Class automaticly subscribe / cancel subscription
 * at construction / destruction level.
 * 
 */
class CTsWindowGroupsObserver: public CBase,
                               public MTsWindowGroupsObserver
{
public:
    /**
     * Destructor
     * Function automaticly cancel subscrption to window server events
     */
    ~CTsWindowGroupsObserver();
    
protected:
    /**
     * First phase constructor
     */
    CTsWindowGroupsObserver(MTsWindowGroupsMonitor &);
    
    /**
     * Second phase constructor.
     * Function automaticly subscribe window server events
     */
    void BaseConstructL();

private:
    MTsWindowGroupsMonitor & mMonitor;
};

/**
 * Window server monitor implementation.
 */
class CTsWindowGroupsMonitor: public CActive, 
                              public MTsWindowGroupsMonitor
                              
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
    
    /**
     * Function provide window server event to observer
     */
    void ProvideEventL(TWsEvent, MTsWindowGroupsObserver &);

private:
    /**
     * Registry of subscribed observers
     */
    RPointerArray<MTsWindowGroupsObserver> mObservers;
    
    /**
     * Resources manager
     */
    MTsResourceManager &mResources;
    
    /**
     * Monitor window group
     */
    RWindowGroup mWg;
};
#endif //TSWINDOWGROUPSOBSERVER_H
