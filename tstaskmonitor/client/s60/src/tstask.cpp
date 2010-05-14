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
#include "tstask.h"

#include <xqconversions>

#include "tsfswentry.h"

TsTask::TsTask(CTsFswEntry* entry) : mEntry(entry)
{
}

TsTask::~TsTask()
{
    delete mEntry;
}

bool TsTask::isClosable() const
{
    return mEntry->CloseableApp();
}

QPixmap TsTask::icon() const
{
    QPixmap pixmap = QPixmap::fromSymbianCFbsBitmap(mEntry->AppIconBitmap());
    QPixmap mask = QPixmap::fromSymbianCFbsBitmap(mEntry->AppIconMask());
    // @todo QPixmap::setAlphaChannel() is deprecated, change to code using QPainter
    pixmap.setAlphaChannel(mask);
    return pixmap;
}

QString TsTask::name() const
{
    return XQConversions::s60DescToQString(mEntry->AppName());
}
