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
#ifndef TSAPPLICATIONTASK_H
#define TSAPPLICATIONTASK_H

#include "tstask.h"

#include <w32std.h>

class TsApplicationTask : public TsTask
{
    
public:
    TsApplicationTask(RWsSession &wsSession, CTsEntry* entry);
    
    virtual void open();
    virtual void close();

private:
    RWsSession &mWsSession;
    
};

#endif //TSAPPLICATIONTASK_H
