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
#ifndef TSSCREENSHOTREGISTRY_P_H
#define TSSCREENSHOTREGISTRY_P_H

#include <QPixmap>

class CTsScreenshotClient;

class TsScreenshotRegistryPrivate
{
public:
    TsScreenshotRegistryPrivate();
    ~TsScreenshotRegistryPrivate();

public:
    bool registerScreenshot(const QPixmap &screenshot);
    bool unregisterScreenshot();
    
private:
    int wgId();
    
private:
    CTsScreenshotClient *mClient;   
    
};

#endif //TSSCREENSHOTREGISTRY_P_H
