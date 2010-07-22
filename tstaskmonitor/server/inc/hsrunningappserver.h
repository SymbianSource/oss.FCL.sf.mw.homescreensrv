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
#ifndef RUNNINGAPPSERVER_H
#define RUNNINGAPPSERVER_H

// INCLUDES
#include <e32std.h>
#include <e32base.h>
#include <w32std.h>
#include "tsdataobserver.h"

class CRunningAppStorage;
class CTsResourceManager;
class CTsWindowGroupsMonitor;
/**
 *  CRunningAppServer
 * 
 */
class CRunningAppServer : public CServer2,
                          public MHsDataObserver,
                          public MHsDataObserverStorage
{
public:
    /**
     * Destructor.
     */
    ~CRunningAppServer();

    /**
     * Two-phased constructor.
     */
    static CRunningAppServer* NewLC();
    
public: //MHsDataObserver
    void DataChanged();
    
public: //MHsDataObserverStorage
    void PushL(CHsDataObserver*);
    void Pop(CHsDataObserver*);
    void Cancel(const RMessage2&);

private:
    /**
     * Constructor for performing 1st stage construction
     */
    CRunningAppServer();

    /**
     * Default constructor for performing 2nd stage construction
     */
    void ConstructL();

    /**
     * Interface implementation
     * @see CServer2::NewSessionL(const TVersion&, const RMessage2&)
     */
    CSession2* NewSessionL(const TVersion& version, const RMessage2& message) const;
private:
    RPointerArray<CHsDataObserver> mObservers;
    RWsSession mWsSession;
    CTsResourceManager* mResources;
    CTsWindowGroupsMonitor* mMonitor;
    CRunningAppStorage* mStorage;
    
    CBase *mScreenshotProviderStarter;
    CBase *mBacksteppingEngine;
};

#endif // RUNNINGAPPSERVER_H
