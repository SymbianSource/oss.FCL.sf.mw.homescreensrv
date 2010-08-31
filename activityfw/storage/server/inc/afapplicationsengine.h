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
#ifndef AFAPPLICATIONSENGINE_H
#define AFAPPLICATIONSENGINE_H

#include <e32base.h>
#include "afstorage.h"

class RApaLsSession;
class CAfApplicationsCollection;
class CAfApplicationsStorage;
class CAfApplicationsMonitor;

class CAfApplicationsEngine: public CBase
{
public:
    static CAfApplicationsEngine* NewL(RApaLsSession& serviceProvider, CAfStorage& storage);
    ~CAfApplicationsEngine();

private:
    CAfApplicationsEngine();
    void ConstructL(CAfStorage& storage, RApaLsSession& serviceProvider);

private:
    CAfApplicationsCollection *mCollection;
    CAfApplicationsMonitor *mMonitor;
    CAfApplicationsStorage *mStorage;
};

#endif //AFAPPLICATIONSENGINE_H
