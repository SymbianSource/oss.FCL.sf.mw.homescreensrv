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

#include "tstaskmonitor.h"
#include "tstaskmonitor_p.h"

#include "tstask.h"

TsTaskMonitor::TsTaskMonitor(QObject *parent) : QObject(parent), d_ptr(0)
{
    d_ptr = new TsTaskMonitorPrivate(this);
}

TsTaskMonitor::~TsTaskMonitor()
{
    delete d_ptr;
}

QList< QSharedPointer<TsTask> > TsTaskMonitor::taskList()
{
    return d_ptr->taskList();
}
