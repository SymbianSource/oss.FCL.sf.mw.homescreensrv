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
#ifndef TSTASKMONITORCLIENT_H
#define TSTASKMONITORCLIENT_H

#include <e32base.h>

#include "tsfswentry.h"

class MTsTaskMonitorObserver;
class CTsTaskMonitorClientImpl;

NONSHARABLE_CLASS( CTsTaskMonitorClient ) : public CBase
{
public:
    IMPORT_C static CTsTaskMonitorClient* NewL();
    IMPORT_C static CTsTaskMonitorClient* NewLC();
    IMPORT_C ~CTsTaskMonitorClient();

public:
    void TaskListL(RTsFswArray& tasks);
    void Subscribe(MTsTaskMonitorObserver& aObserver);
    void CancelSubscribe();

private:
    CTsTaskMonitorClient();
    void ConstructL();

private:
    CTsTaskMonitorClientImpl* iImpl;
};

#endif  // TSTASKMONITORCLIENT_H
