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
#include <fbs.h>
#include "tsdatatask.h"

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
//
void TsDataTask::ExecuteL(MTsDataStorage& dataStorage, const RMessage2& msg)
{
    switch(msg.Function()) {
    case RegisterScreenshotMessage:
        TsDataTask::RegisterScreenshotL(dataStorage, msg); break;
    case UnregisterScreenshotMessage:
        TsDataTask::UnregisterScreenshotL(dataStorage, msg); break;
    }
    
    msg.Complete(KErrNone);
}

void TsDataTask::RegisterScreenshotL(MTsDataStorage& dataStorage, 
                                           const RMessage2& msg)
{
    TPckgBuf<TInt> wgId, fbsHdl, param, priority;
    
    msg.ReadL(WindowsGroup, wgId);
    msg.ReadL(ScreenshotHandle, fbsHdl);
    msg.ReadL(AdditionalParameters, param);
    msg.Read(Priority, priority);
    
    //convert and provide data
    CFbsBitmap* bitmap = new(ELeave) CFbsBitmap();
    CleanupStack::PushL(bitmap);
    User::LeaveIfError(bitmap->Duplicate(fbsHdl()));
    dataStorage.UpdateL(wgId(), *bitmap, param(), priority());
    CleanupStack::PopAndDestroy(bitmap);
}
    
void TsDataTask::UnregisterScreenshotL(MTsDataStorage& dataStorage, 
                                       const RMessage2& msg)
{
    TPckgBuf<TInt> wgId, param;
    
    msg.ReadL(WindowsGroup, wgId);
    msg.ReadL(AdditionalParameters, param);
    
    dataStorage.RemoveL(wgId(), param());
}
