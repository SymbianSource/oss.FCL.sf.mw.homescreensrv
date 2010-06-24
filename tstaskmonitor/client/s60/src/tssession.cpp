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
#include <fbs.h>

#include "tssession.h"

TInt RTsSession::Connect()
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

TInt RTsSession::StartServer()
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

void RTsSession::TaskListL(RTsFswArray& tasks)
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
            CTsEntry::InternalizeArrayL(dataStream, tasks);
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

void RTsSession::Subscribe(TRequestStatus& aStatus)
{
    SendReceive(SubscribeRunningAppChanges, aStatus);
}

void RTsSession::CancelSubscribe()
{
    SendReceive(CancelSubscribeRunningAppChanges);
}

TInt RTsSession::RegisterScreenshot(CFbsBitmap *screenshot, TInt wgId, UpdatePriority priority)
{
    TPckgBuf<TInt> screenshotPackage(screenshot->Handle());
    TPckgBuf<TInt> wgIdPackage(wgId);
    TPckgBuf<TInt> priorityPackage(priority);

    TIpcArgs arguments;
    arguments.Set(ScreenshotHandle, &screenshotPackage);
    arguments.Set(WindowsGroup, &wgIdPackage);
    arguments.Set(AdditionalParameters, &KNullDesC8);
    arguments.Set(Priority, &priorityPackage);

    return SendReceive(RegisterScreenshotMessage, arguments);
}

void RTsSession::RegisterScreenshot(TRequestStatus &status, CFbsBitmap *screenshot, TInt wgId, UpdatePriority priority)
{
    TPckgBuf<TInt> screenshotPackage(screenshot->Handle());
    TPckgBuf<TInt> wgIdPackage(wgId);
    TPckgBuf<TInt> priorityPackage(priority);

    TIpcArgs arguments;
    arguments.Set(ScreenshotHandle, &screenshotPackage);
    arguments.Set(WindowsGroup, &wgIdPackage);
    arguments.Set(AdditionalParameters, &KNullDesC8);
    arguments.Set(Priority, &priorityPackage);

    SendReceive(RegisterScreenshotMessage, arguments, status);
}

TInt RTsSession::UnregisterScreenshot(TInt wgId)
{
    TPckgBuf<TInt> wgIdPackage(wgId);
    
    TIpcArgs arguments;
    arguments.Set(WindowsGroup, &wgIdPackage);
    arguments.Set(AdditionalParameters, &KNullDesC8);
    
    return SendReceive(UnregisterScreenshotMessage, arguments);
}

TInt RTsSession::SetVisibility(TInt wgId, Visibility value)
{
    TPckgBuf<TInt> windowGroup(wgId), visibilityValue(value);
    
    TIpcArgs arguments(&windowGroup, &visibilityValue);
    return SendReceive(VisibilityChange, arguments);
}
