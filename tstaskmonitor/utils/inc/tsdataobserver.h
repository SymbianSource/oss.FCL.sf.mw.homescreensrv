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
#ifndef DATAOBSERVER_H
#define DATAOBSERVER_H

#include <e32base.h>

class MHsDataObserver {
public:
    virtual void DataChanged() =0;
};

class CHsDataObserver: public CBase,
                       public MHsDataObserver
{
public:
    virtual void Cancel(const RMessage2& reason) =0;
};

class MHsDataObserverStorage
{
public:
    virtual void PushL(CHsDataObserver*) =0;
    virtual void Pop(CHsDataObserver*) =0;
    virtual void Cancel(const RMessage2& reason) =0;
};

#endif //DATAOBSERVER_H
