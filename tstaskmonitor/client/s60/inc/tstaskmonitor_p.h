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
#ifndef TSTASKMONITOR_P_H
#define TSTASKMONITOR_P_H

#include "tstaskmonitorobserver.h"

#include <w32std.h>

#include <QList>
#include <QSharedPointer>

class TsTask;
class TsTaskMonitor;
class CTsTaskMonitorClient;

class TsTaskMonitorPrivate : public MTsTaskMonitorObserver
{
public:
    TsTaskMonitorPrivate(TsTaskMonitor *q);
    virtual ~TsTaskMonitorPrivate();
    
public:
    QList< QSharedPointer<TsTask> > taskList();

public:
    virtual void HandleRunningAppChange();

private:
    TsTaskMonitor *q_ptr;
    CTsTaskMonitorClient *mClient;
    RWsSession &mWsSession;
};

#endif //TSTASKMONITOR_P_H
