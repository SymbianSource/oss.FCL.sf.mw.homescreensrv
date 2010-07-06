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
#ifndef AFASYNCREQUESTPRIVATE_H
#define AFASYNCREQUESTPRIVATE_H
#include <e32base.h>
#include <QVariant>
#include <QPixmap>
#include "afasyncrequestobserver.h"
#include "afstorageclientimp.h"
#include "afcmd.h"

class CAfAsyncRequestPrivate : public CActive

{
public:
    static void NewLD(MAfAsyncRequestObserver &observer, 
                      RAfStorageClientImplementation & session,
                      ActivityCmd cmd);

    ~CAfAsyncRequestPrivate();

protected:
    void DoCancel();

    void RunL();

    TInt RunError(TInt error);

private:
    CAfAsyncRequestPrivate(MAfAsyncRequestObserver &, 
                           RAfStorageClientImplementation &,
                           ActivityCmd);
    void ConstructL();

private:
    MAfAsyncRequestObserver &mObserver;
    RAfStorageClientImplementation &mSession;
    const ActivityCmd mCmd;
    TPckgBuf<TInt> mIds[4];
};
#endif // AFASYNCREQUESTPRIVATE_H
