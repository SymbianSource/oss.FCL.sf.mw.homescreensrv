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
#include "tsscreenshotprovider.h"
#include "tstaskmonitorglobals.h"
#include <s32mem.h>

const TUid KPluginUID = {0x200267AE};
// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
//
CTsScreenshotProvider* CTsScreenshotProvider::NewL(MTsDataStorage& storage)
{
    CTsScreenshotProvider *self = new (ELeave)CTsScreenshotProvider(storage);
    CleanupStack::PushL(self);
    self->BaseConstructL(KPluginUID, KNullDesC8);
    CleanupStack::Pop(self);
    return self;
}

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
//
CTsScreenshotProvider::CTsScreenshotProvider(MTsDataStorage& storage)
:
mStorage(storage)
{}

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
//
void CTsScreenshotProvider::HandleMessage(const TDesC8& msg)
{
    TRAP_IGNORE(HandleMessageL(msg));
}

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
//
void CTsScreenshotProvider::HandleMessageL(const TDesC8& msg)
{
    const TInt KMsgSize(5); 
    TInt parsedMsg[KMsgSize];
    
    RDesReadStream msgStream(msg);
    for (TInt iter(0); iter < KMsgSize; ++iter) {
        parsedMsg[iter] = msgStream.ReadInt32L();
    }
    
    if (RegisterScreenshotMessage == parsedMsg[0]) {
        CFbsBitmap *bitmap = new CFbsBitmap;
        TInt errNo(KErrNoMemory);
        if (0 != bitmap) {
            CleanupStack::PushL(bitmap);
            errNo = bitmap->Duplicate(parsedMsg[ScreenshotHandle + 1]);
        }
        const TPckgC<TInt[sizeof(parsedMsg) / sizeof(TInt)]> buf(parsedMsg);//ACK bitmap duplication
        SendMessage(msg);
        User::LeaveIfError(errNo);
        
        mStorage.UpdateL(parsedMsg[WindowsGroup +1 ], 
                         *bitmap, 
                         parsedMsg[AdditionalParameters + 1], 
                         parsedMsg[Priority + 1]);
       
        CleanupStack::PopAndDestroy(bitmap);
    }
}

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
//
void CTsScreenshotProvider::OnReplace()
{
    //no implementation required
}
