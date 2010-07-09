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
#ifndef AFSTORAGECLIENTIMPLEMENTATION_H
#define AFSTORAGECLIENTIMPLEMENTATION_H
#include <e32base.h>
#include "afasyncrequestobserver.h"

class CAfEntry;
class CFbsBitmap;

class RAfStorageClientImplementation: protected RSessionBase
{
public:
    RAfStorageClientImplementation(MAfAsyncRequestObserver &observer);

    ~RAfStorageClientImplementation();

    void Close();

    void executeL(int function);

    void executeL(int function, 
                  const CAfEntry &sourceEntry,
                  int userData =0);

    void executeL(int function, 
                  RPointerArray<CAfEntry> &resultsList, 
                  const CAfEntry& templateEntry);
    
    void getThumbnailL(TSize size, 
                      const TDesC& source,
                      TAny *userData );

    void connectL();

public:
    void PushL(CBase* asyncTask);

    void Pop(CBase* asyncTask);

    void sendAsync(int function, const TIpcArgs &args, TRequestStatus& status);

    void getDataL(int id, TDes8& buffer);

private:
    void startServerL();

private:
    RPointerArray<CBase> mAsyncTasks;
    MAfAsyncRequestObserver &mObserver;
};

#endif //AFSTORAGECLIENTIMPLEMENTATION_H