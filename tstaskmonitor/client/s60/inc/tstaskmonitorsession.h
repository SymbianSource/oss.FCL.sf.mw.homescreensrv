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
#ifndef TSTASKMONITORSESSION_H
#define TSTASKMONITORSESSION_H

#include <e32base.h>

#include "tsfswentry.h"

NONSHARABLE_CLASS( RTsTaskMonitorSession ) : public RSessionBase
{
public:
    TInt Connect();

    void TaskListL(RTsFswArray& tasks);
    void Subscribe(TRequestStatus& aStatus);
    void CancelSubscribe();

private:
    TInt StartServer();
};

#endif  // TSTASKMONITORSESSION_H
