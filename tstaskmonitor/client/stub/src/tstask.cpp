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
#include "tsentry.h"

TsTask::TsTask(CTsEntry* entry) : mEntry(entry)
{
}

TsTask::~TsTask()
{
    delete mEntry;
}

bool TsTask::isClosable() const
{
    return false;
}

QPixmap TsTask::icon() const
{
    return QPixmap();
}

QPixmap TsTask::screenshot() const
{
    return QPixmap();
}

QString TsTask::name() const
{
    return QString();
}
