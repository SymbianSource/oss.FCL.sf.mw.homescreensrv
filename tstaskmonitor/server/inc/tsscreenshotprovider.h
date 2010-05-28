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
#include <w32std.h>

class CTsScreenshotProvider: public CWsGraphic
{
public:
    static CTsScreenshotProvider* NewL(MTsDataStorage&);
private:
    CTsScreenshotProvider(MTsDataStorage&);
    void HandleMessage(const TDesC8&);
    void HandleMessageL(const TDesC8&);
    void OnReplace();
private:
    MTsDataStorage& mStorage;
};

#endif //TSSCREENSHOTTASK_H
