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

class TsApplicationTask : public TsTask
{
    
public:
    TsApplicationTask(CTsFswEntry* entry);
    
    virtual void open();
    virtual void close();

};

#endif //TSAPPLICATIONTASK_H
