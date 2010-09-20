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

class MTsRunningApplication
    {
public:
    virtual TUid UidL()const =0;
    virtual const TDesC& CaptionL() const =0;
    virtual TBool IsHiddenL() const =0;
    virtual TBool IsSystemL() const =0;
    virtual TInt WindowGroupId() const =0;
    virtual TInt ParentWindowGroupId() const =0;
    virtual TBool IsEmbeded() const =0;
    

    };

class MTsRunningApplicationStorage
    {
public:
    virtual const MTsRunningApplication& operator[] (TInt aOffset) const=0;
    virtual TInt Count() const =0;
    virtual TInt ParentIndex( const MTsRunningApplication& aRaunningApp ) const =0;
    virtual TArray<TInt> BlockedWindowGroups() const =0;

    };

/**
 * Interface declare mathods to notify about window server events
 */
class MTsWindowGroupsObserver
{
public:
    /**
     * Method notify about window group changes.
     * @param aResources - resource manager
     * @param aStorage - list of running applications
     */
    virtual void HandleWindowGroupChanged(MTsResourceManager &aResources, 
                                          const MTsRunningApplicationStorage& aStorage) =0;
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

#endif //TSWINDOWGROUPSOBSERVER_H
