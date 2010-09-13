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
#ifndef TSSCREENSHOTPLUGIN_H
#define TSSCREENSHOTPLUGIN_H

#include <w32std.h>
#include <graphics/wsgraphicdrawerinterface.h>
#include <graphics/wsgraphicdrawer.h>

#include "tsorientationobserver.h"

class CTsOrientationMonitor;

class CTsScreenshotPlugin: public CWsGraphicDrawer, 
                           public MWsEventHandler,
                           public MTsOrientationObserver
{
public:
    static CTsScreenshotPlugin* NewL();
    ~CTsScreenshotPlugin();

private:
    void DoDraw(MWsGc&, const TRect&, const TDesC8&) const;
    void HandleMessage(const TDesC8&);
    void HandleMessageL(const TDesC8&);
    void ConstructL(MWsGraphicDrawerEnvironment& aEnv, 
                    const TGraphicDrawerId& aId, 
                    MWsClient& aOwner, 
                    const TDesC8& aData);
    void ConstructL();
    void DoHandleEvent(const TWservCrEvent& aEvent);
    void TakeScreenshot(TInt);
    void TakeScreenshotL(TInt);
    void NotifyWindowGroupToBackground(TInt aWindowGroupId);
    void NotifyWindowGroupToBackgroundL(TInt aWindowGroupId);
    TInt OrientationToAngle();
    
private://from MTsOrientationObserver
    void OrientationChanged( TInt aAngle );
    
private:
    RPointerArray<CFbsBitmap> iCache;
    CTsOrientationMonitor* iMonitor;
    TInt iWindowGroupId;
    TInt iAngle;
};

#endif //TSSCREENSHOTPLUGIN_H
