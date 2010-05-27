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
#include <s32mem.h>

#include "tstaskmonitorsession.h"
#include "tstaskmonitorglobals.h"

void RTsTaskMonitorSession::TaskListL(RTsFswArray& tasks)
{
    for (;;) {
        TPckgBuf<TInt> dataSize;
        User::LeaveIfError(SendReceive(GetRunningAppInfo, TIpcArgs(&dataSize)));
        HBufC8* data = HBufC8::NewLC(dataSize());
        TPtr8 dataPointer(data->Des());
        TInt err = SendReceive(FlushData, TIpcArgs(&dataPointer, dataSize()));
        if (err == KErrNone) {
            RDesReadStream dataStream(dataPointer);
            CleanupClosePushL(dataStream);
            CTsFswEntry::InternalizeArrayL(dataStream, tasks);
            CleanupStack::PopAndDestroy(&dataStream);
        } else if (err != KErrCorrupt) {
            User::Leave(err);
        }
        CleanupStack::PopAndDestroy(data);
        if (err == KErrNone) {
            break;
        }
        // If result was KErrCorrupt then the size received from GetRunningAppInfo is
        // not valid anymore so restart the whole procedure.
    }
}

TInt RTsTaskMonitorSession::StartServer()
{
    RProcess server;
    if (TInt error = server.Create(KRunningAppServerName, KNullDesC, TUidType(KNullUid, KNullUid, KRunningAppServerUid))) {
        return error;
    }

    TRequestStatus status;
    server.Rendezvous(status);
    if (status != KRequestPending) {
        server.Kill(0);
    } else {
        server.Resume();
    }

    User::WaitForRequest(status);
    TInt error = (EExitPanic == server.ExitType()) ? KErrGeneral : status.Int();
    if (KErrCancel == error) {
        error = KErrNone;
    }
    server.Close();

    return error;
}

TInt RTsTaskMonitorSession::Connect()
{
    TInt retry = 4;
    TInt error(KErrNone);
    do {
        error = CreateSession(KRunningAppServerName, TVersion(0, 0, 0));
        if (KErrNotFound != error && KErrServerTerminated != error) {
            retry = 0;
        } else {
            error = StartServer();
            if (KErrNone != error && KErrAlreadyExists != error) {
                retry = 0;
                error = CreateSession(KRunningAppServerName, TVersion(0, 0, 0));
            }
        }
    } while (--retry > 0);
    return error;
}

void RTsTaskMonitorSession::Subscribe(TRequestStatus& aStatus)
{
    SendReceive(SubscribeRunningAppChanges, aStatus);
}

void RTsTaskMonitorSession::CancelSubscribe()
{
    SendReceive(CancelSubscribeRunningAppChanges);
}
