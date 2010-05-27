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
#ifndef TSTASKMONITORCLIENTIMPL_H
#define TSTASKMONITORCLIENTIMPL_H

#include <e32base.h>

#include "tsfswentry.h"
#include "tstaskmonitorsession.h"

class MTsTaskMonitorObserver;

NONSHARABLE_CLASS( CTsTaskMonitorClientImpl ) : public CActive
{
public:
    static CTsTaskMonitorClientImpl* NewL();
    static CTsTaskMonitorClientImpl* NewLC();
    ~CTsTaskMonitorClientImpl();

public:
    void TaskListL(RTsFswArray& tasks);

    void Subscribe(MTsTaskMonitorObserver& aObserver);
    void CancelSubscribe();

private:
    void RunL();
    void DoCancel();

private:
    CTsTaskMonitorClientImpl();
    void ConstructL();

    RTsTaskMonitorSession iSession;
    MTsTaskMonitorObserver* iObserver; // not owned
};

#endif  // TSTASKMONITORCLIENTIMPL_H
