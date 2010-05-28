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

class CTsScreenshotPlugin: public CWsGraphicDrawer, 
                           public MWsEventHandler
{
public:
    /**
     * Two phase constructor 
     */
    
    static CTsScreenshotPlugin* NewL();
    
    /**
     * Destructor
     */
    ~CTsScreenshotPlugin();

private:
    /**
     * From CWsGraphicDrawer.
     * @see CWsGraphicDrawer::DoDraw(MWsGc&, const TRect&, const TDesC8&) const
     */ 
    void DoDraw(MWsGc&, const TRect&, const TDesC8&) const;
    
    /**
     * From CWsGraphicDrawer
     * @see CWsGraphicDrawer::HandleMessage(const TDesC8&)
     */
    void HandleMessage(const TDesC8&);
    
    void HandleMessageL(const TDesC8&);
    
    /**
     * From CWsGraphicDrawer
     * @see CWsGraphicDrawer::ConstructL(MWsGraphicDrawerEnvironment&, const TGraphicDrawerId& , MWsClient&, const TDesC8&)
     */
    void ConstructL(MWsGraphicDrawerEnvironment& env, 
                    const TGraphicDrawerId& id, 
                    MWsClient& owner, 
                    const TDesC8& data);
    
    /**
     * From MWsEventHandler.
     * 
     * @see MWsEventHandler::DoHandleEvent(const TWservCrEvent&)
     */
    void DoHandleEvent(const TWservCrEvent& event);
    
    /**
     * Function take screenshot od current display
     * @param screenshot identyfier
     */
    void TakeScreenshot(TInt);
        
    /**
     * Function take screenshot od current display
     * @param screenshot identyfier
     */
    void TakeScreenshotL(TInt);

private:
    RPointerArray<CFbsBitmap> mCache;
    TInt mWindowGroupId;
};

#endif //TSSCREENSHOTPLUGIN_H
