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
#ifndef TSRESOURCEMANAGERIMP_H
#define TSRESOURCEMANAGERIMP_H

#include <e32base.h>
#include <w32std.h>
#include <apgcli.h>

#include "tsresourcemanager.h"
#include "tsdatastorage.h"

class CTsWindowGroupsMonitor;
class CTsIconProvider;
class CTsIdList;
/**
 * Resource manager implemetatioin
 */
class CTsResourceManager: public CBase,
                          public MTsResourceManager,
                          public MTsDataStorage
    {
public:
    /**
     * Two phase constructor
     */
    static CTsResourceManager* NewL();
    
    /**
     * Destructor
     */
    ~CTsResourceManager();
    
    /**
     * @see MTsResourceManager::WsSession
     */
    RWsSession& WsSession();
    
    
    RApaLsSession& ApaSession();
    
    MTsWindowGroupsMonitor& WsMonitor();
    
    MTsIconProvider& IconProvider();
    
    const CTsIdList& ApplicationsBlackList() const;
    
    TBool IsSupported(TInt aFunction) const;
    void HandleDataL(TInt aFunction, RReadStream& aDataStream);

private:
    /**
     * Second phase constructor
     */
    void ConstructL();

private:
    RWsSession mWsSession;
    RApaLsSession mApaSeesion;
    CTsWindowGroupsMonitor* iMonitor;
    CTsIconProvider* iIconProvider;
    CTsIdList* iBlackList;
    };

#endif // TSRESOURCEMANAGER_H
