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
#ifndef DATAOBSERVERTASK_H
#define DATAOBSERVERTASK_H
#include "hsdataobserver.h"
#include <e32base.h>
class CHsDataObserverTask: public CHsDataObserver
{
public:
    ~CHsDataObserverTask();
    static void ExecuteLD(MHsDataObserverStorage& storage,const RMessage2& msg);

private:
    CHsDataObserverTask(MHsDataObserverStorage& storage, const RMessage2& msg);
    void DataChanged();
    void Cancel(const RMessage2& reason);

private:
    MHsDataObserverStorage& mStorage;
    const RMessage2 mMsg;
    TBool mDataChanged;
};
#endif //DATAOBSERVERTASK_H
