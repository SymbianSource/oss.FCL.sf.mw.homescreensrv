/*
* Copyright (c) 2008 Nokia Corporation and/or its subsidiary(-ies).
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
#include "tsscreenshotplugin.h"
#include "tstaskmonitorglobals.h"
#include <graphics/wsscreendevice.h>
#include <s32mem.h>

const TInt KMsgSize(5);
const TInt KInvalidGroupId(~0);
// -----------------------------------------------------------------------------
// 
// -----------------------------------------------------------------------------
//
CTsScreenshotPlugin* CTsScreenshotPlugin::NewL()
{
    //no second step construction is required here
    //window server will initialize plugin later
    return new(ELeave)CTsScreenshotPlugin();
}
// -----------------------------------------------------------------------------
// 
// -----------------------------------------------------------------------------
//
void CTsScreenshotPlugin::ConstructL(MWsGraphicDrawerEnvironment& env, 
                                     const TGraphicDrawerId& id, 
                                     MWsClient& owner, 
                                     const TDesC8& /*data*/ )
{
    BaseConstructL(env, id, owner);
    env.RegisterEventHandler(this, 
                             this, 
                             TWservCrEvent::EWindowGroupChanged |
                             TWservCrEvent::EDeviceOrientationChanged);
    mWindowGroupId = KInvalidGroupId;
}
// -----------------------------------------------------------------------------
// 
// -----------------------------------------------------------------------------
//
CTsScreenshotPlugin::~CTsScreenshotPlugin()
{
    Env().UnregisterEventHandler(this);
    mCache.ResetAndDestroy();
}

// -----------------------------------------------------------------------------
// 
// -----------------------------------------------------------------------------
//
void CTsScreenshotPlugin::DoDraw(MWsGc&, const TRect&, const TDesC8&) const
{
    //plugin is not a real drawer
    //no implementation required
}
// -----------------------------------------------------------------------------
// 
// -----------------------------------------------------------------------------
//
void CTsScreenshotPlugin::HandleMessage(const TDesC8& msg)
{
    TRAP_IGNORE(HandleMessageL(msg));
}

// -----------------------------------------------------------------------------
// 
// -----------------------------------------------------------------------------
//
void CTsScreenshotPlugin::HandleMessageL(const TDesC8& msg)
{
    TInt parsedMsg[KMsgSize];
    RDesReadStream msgStream(msg);
    for (TInt iter(0); iter < KMsgSize; ++iter) {
        parsedMsg[iter] = msgStream.ReadInt32L(); 
    }
    
    if (RegisterScreenshotMessage == parsedMsg[0]) { //Screenshot registeration ACK 
        for(TInt iter(0); iter < mCache.Count(); ++iter) {
            if (mCache[iter]->Handle() == parsedMsg[ScreenshotHandle + 1]) {
                //bitmap is not needed no more
                delete mCache[iter];
                mCache.Remove(iter);
                break;
            }
        }
    }
}

// -----------------------------------------------------------------------------
// 
// -----------------------------------------------------------------------------
//
void CTsScreenshotPlugin::DoHandleEvent(const TWservCrEvent& event)
{
    switch (event.Type()) {
    case TWservCrEvent::EWindowGroupChanged:
        if (KInvalidGroupId != mWindowGroupId) {
            TakeScreenshot(mWindowGroupId);
        }
        mWindowGroupId = event.WindowGroupIdentifier();
        break;
    case TWservCrEvent::EDeviceOrientationChanged:
        TakeScreenshot(mWindowGroupId);
        break;
    }
}

// -----------------------------------------------------------------------------
// 
// -----------------------------------------------------------------------------
//
void CTsScreenshotPlugin::TakeScreenshot(TInt id)
{
    TRAP_IGNORE(TakeScreenshotL(id);)
}

// -----------------------------------------------------------------------------
// 
// -----------------------------------------------------------------------------
//
void CTsScreenshotPlugin::TakeScreenshotL(TInt id)
{
    if(0 >= Env().ScreenCount()) {
        User::Leave(KErrCorrupt);
    }
    const TInt screenId(0); //use local variable in case of changing screen selection policy
    const MWsScreenConfig* const screenConfig = 
        Env().Screen(screenId)->ObjectInterface<MWsScreenConfig>();
    const MWsScreenDevice* const screenDevice = 
        static_cast<MWsScreenDevice*>(Env().Screen(screenId)->ResolveObjectInterface(MWsScreenDevice::EWsObjectInterfaceId));
    
    User::LeaveIfNull(screenConfig);
    User::LeaveIfNull(screenDevice);
    
    //prepare destination bitmap
    CFbsBitmap *bitmap = new (ELeave) CFbsBitmap();
    CleanupStack::PushL(bitmap);
    
    
    User::LeaveIfError(bitmap->Create(screenConfig->SizeInPixels(), 
                                screenConfig->DisplayMode()));
    
    screenDevice->CopyScreenToBitmapL(bitmap, 
                                      screenConfig->SizeInPixels());
    
    
    //prepare and send message
    TInt msg[KMsgSize] = {RegisterScreenshotMessage};
    
    msg[WindowsGroup + 1] = id;
    msg[ScreenshotHandle + 1] = bitmap->Handle();
    msg[Priority + 1] = Low;
    msg[AdditionalParameters + 1] = 0; //unused
    
    const TPckgC<TInt[sizeof(msg) / sizeof(TInt)]> buf(msg);
    
    User::LeaveIfError(SendMessage(buf));
    
    mCache.AppendL(bitmap);
    CleanupStack::Pop(bitmap);
    
}

