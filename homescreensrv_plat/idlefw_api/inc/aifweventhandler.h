/*
* Copyright (c) 2005-2006 Nokia Corporation and/or its subsidiary(-ies).
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
* Description:  Active Idle Framework internal event handling interface.
*
*/


#ifndef M_AIFWEVENTHANDLER_H
#define M_AIFWEVENTHANDLER_H

#include <e32cmn.h>
#include <aifwdefs.h>

class CAiUiController;
class CAiContentPublisher;

/**
 * Active Idle Framework internal event handling interface. 
 * Events from UI controllers are forwarded to this interface for processing 
 * on the Active Idle Framework side.
 *
 * @since S60 3.2
 */
class MAiFwEventHandler
{
public:
    /**
     * Application environment ready callback. Main UI controller calls this 
     * method when application framework is initialized.
     */
    virtual void AppEnvReadyL() = 0;

    /**
     * UI ready event handler.
     *
     * @param aUiController Active Idle UI Controller which manages the UI 
     *                      that is ready.
     */
    virtual void HandleUiReadyEventL( CAiUiController& aUiController ) = 0;

    /**
     * UI activation event handler.
     *     
     */
    virtual void HandleActivateUI() = 0;
    
    /**
     * UI shutdown event handler.
     *
     * @param aUiController Active Idle UI Controller which manages the UI 
     *                      that was shut down.
     */
    virtual void HandleUiShutdown( CAiUiController& aUiController ) = 0;

    /**
     * UI has changed, load data plugin.
     *
     * @param aPublisherInfo Publisher info describing plugin to load.
     */    
    virtual void HandleLoadPluginL( const TAiPublisherInfo& aPublisherInfo ) = 0;
    
    /**
     * UI has changed, destroy data plugin.
     *
     * @param aPublisherInfo Publisher info describing plugin to destroy.
     */    
    virtual void HandleDestroyPluginL( const TAiPublisherInfo& aPublisherInfo ) = 0;    

    /**
     * Active Idle Framework Plug-in event handler. UI controllers forward
     * events that are targeted to Active Idle Content Publisher Plug-ins
     * to this method.
     *
     * @param aParam event parameters from the UI model.     
     */
    virtual void HandlePluginEvent( const TDesC& aParam ) = 0;

    /**
     * Active Idle Framework Plug-in event handler. UI controllers forward
     * events that are targeted to Active Idle Content Publisher Plug-ins
     * to this method.
     *
     * @param aPublisherInfo publisher info.
     * @param aParam event parameters from the UI model.
     */
    virtual void HandlePluginEventL( const TAiPublisherInfo& aPublisherInfo, const TDesC& aParam ) = 0;

    /**
     * Queries if a Content Publiseher Plug-in has settings     
     *
     * @param aPublisherInfo publisher info.
     * @param aMenuItem menuitem type.
     */          
    virtual TBool HasMenuItemL( const TAiPublisherInfo& aPublisherInfo, const TDesC& aMenuItem ) = 0;

    /**
     * Refresh content request. UI controller can use this interface to request
     * a content publisher plug-in to refresh (re-publish) a specific content 
     * item.
     *
     * @param aContentCid Textual identifier of the content to refresh.
     * @return True if the content publisher plug-in is found and the plugin 
     *         will refresh the content by calling its content observer. 
     *         False otherwise.
     */
     virtual TBool RefreshContent( const TDesC& aContentCid ) = 0;

    /**
     * Service to check if menu is open.
     *
     * @return ETrue if menu is open, EFalse otherwise
     */
    virtual TBool QueryIsMenuOpen() = 0;

    /**
     * Service to process the state changes
     *
     * @param aState changed state
     */    
    virtual void ProcessStateChange( TAifwStates aState ) = 0;
                  
protected:
    /**
     * Protected destructor prevents deletion through this interface.
     */
    ~MAiFwEventHandler() { }
    };

#endif // M_AIEVENTHANDLEREXTENSION_H
