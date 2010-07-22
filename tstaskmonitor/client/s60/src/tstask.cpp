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

#include <XQConversions>

#include "tsentry.h"

/*!
    @class TsTask
    @ingroup publicApi
    @brief TsTask object represents a single task.
    
    This class is one of the Qt interfaces for Task Monitor server. It can be
    used to set custom screenshot of the task, or to change the task visibility.
*/

/*!
    @internal
    Constructor. 
    Takes ownership over @a entry.
*/
TsTask::TsTask(CTsEntry* entry) : mEntry(entry)
{
}

/*!
    Destructor.
*/
TsTask::~TsTask()
{
    delete mEntry;
}

/*!
    @return True if it's possible to close the task, false otherwise.
*/
bool TsTask::isClosable() const
{
    return mEntry->CloseableApp();
}

/*!
    @return Icon of the task.
*/
QPixmap TsTask::icon() const
{
    QPixmap pixmap = QPixmap::fromSymbianCFbsBitmap(mEntry->AppIconBitmap());
    QPixmap mask = QPixmap::fromSymbianCFbsBitmap(mEntry->AppIconMask());
    // @todo QPixmap::setAlphaChannel() is deprecated, change to code using QPainter
    pixmap.setAlphaChannel(mask);
    return pixmap;
}

/*!
    @return Screenshot of the task.
*/
QPixmap TsTask::screenshot() const
{
    return QPixmap::fromSymbianCFbsBitmap(mEntry->Screenshot());
}

/*!
    @return Name of the task.
*/
QString TsTask::name() const
{
    return XQConversions::s60DescToQString(mEntry->AppName());
}

/*!
    @fn TsTask::open()
    Start or bring the task to foreground.
*/

/*!
    @fn TsTask::close()
    Close the task.
*/
