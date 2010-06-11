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
#include "tsscreenshotregistry.h"
#include "tsscreenshotregistry_p.h"

TsScreenshotRegistry::TsScreenshotRegistry() : d_ptr(new TsScreenshotRegistryPrivate())
{
}

TsScreenshotRegistry::~TsScreenshotRegistry()
{
    delete d_ptr;
}

bool TsScreenshotRegistry::registerScreenshot(const QPixmap &screenshot)
{
    return d_ptr->registerScreenshot(screenshot);
}

bool TsScreenshotRegistry::unregisterScreenshot()
{
    return d_ptr->unregisterScreenshot();
}
