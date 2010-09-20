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
#ifndef TSSCREENSHOTPROVIDER_H
#define TSSCREENSHOTPROVIDER_H
#include "tsdatastorage.h"
#include "tswindowgroupsobserver.h"
#include <w32std.h>

class CTsScreenshotProvider: public CWsGraphic,
                             public MTsWindowGroupsObserver
{
public:
    static CTsScreenshotProvider* NewL(MTsDataStorage&, MTsWindowGroupsMonitor&);
    ~CTsScreenshotProvider();

public://from MTsWindowGroupsObserver
    void HandleWindowGroupChanged( MTsResourceManager &aResources, 
                                   const MTsRunningApplicationStorage& aStorage );

public://from CWsGraphic
    void HandleMessage(const TDesC8&);

private:
    CTsScreenshotProvider(MTsDataStorage&, MTsWindowGroupsMonitor&);
    void HandleMessageL(const TDesC8&);
    void HandleWindowGroupChangedL( MTsResourceManager &aResources, 
                                    const MTsRunningApplicationStorage& aStorage );
    void OnReplace();

private:
    MTsDataStorage& iStorage;
    MTsWindowGroupsMonitor& iMonitor;
};

#endif //TSSCREENSHOTTASK_H
