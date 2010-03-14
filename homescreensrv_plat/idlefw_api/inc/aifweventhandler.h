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

// System includes
#include <e32cmn.h>

// User includes

// Forward declarations
class CAiUiController;
class CHsContentPublisher;
class THsPublisherInfo;

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
    // new functions
    
    /**
     * Application environment ready callback. Main UI controller calls this 
     * method when application framework is initialized.
     *
     * @since S60 3.2
     */
    virtual void AppEnvReadyL() = 0;

    /**
     * UI ready event handler.
     *
     * @since S60 3.2
     * @param aUiController Active Idle UI Controller which manages the UI 
     *                      that is ready.     
     */
    virtual void HandleUiReadyEventL( CAiUiController& aUiController ) = 0;

    /**
     * UI activation event handler.
     *
     * @since S60 3.2     
     */
    virtual void HandleActivateUI() = 0;
    
    /**
     * UI shutdown event handler.
     *
     * @since S60 3.2
     * @param aUiController Active Idle UI Controller which manages the UI 
     *                      that was shut down.     
     */
    virtual void HandleUiShutdown( CAiUiController& aUiController ) = 0;

    /**
     * Active Idle Framework Plug-in event handler. UI controllers forward
     * events that are targeted to Active Idle Content Publisher Plug-ins
     * to this method.
     *
     * @since S60 3.2
     * @param aParam event parameters from the UI model.          
     */
    virtual void HandlePluginEvent( const TDesC& aParam ) = 0;

    /**
     * Active Idle Framework Plug-in event handler. UI controllers forward
     * events that are targeted to Active Idle Content Publisher Plug-ins
     * to this method.
     *
     * @since S60 5.2
     * @param aPublisherInfo publisher info.
     * @param aParam event parameters from the UI model.
     */
    virtual void HandlePluginEventL( 
        const THsPublisherInfo& aPublisherInfo, 
        const TDesC& aParam ) = 0;

    /**
     * Queries if a Content Publiseher Plug-in has settings     
     *
     * @since S60 5.2
     * @param aPublisherInfo publisher info.
     * @param aMenuItem menuitem type.
     */          
    virtual TBool HasMenuItemL( 
        const THsPublisherInfo& aPublisherInfo, 
        const TDesC& aMenuItem ) = 0;

    /**
     * Refresh content request. UI controller can use this interface to request
     * a content publisher plug-in to refresh (re-publish) a specific content 
     * item.
     *
     * @since S60 3.2
     * @param aContentCid Textual identifier of the content to refresh.
     * @return True if the content publisher plug-in is found and the plugin 
     *         will refresh the content by calling its content observer. 
     *         False otherwise.
     */
    virtual TBool RefreshContent( const TDesC& aContentCid ) = 0;

    /**
     * Refresh content request. UI controller can use this interface to request
     * a content publisher plug-in to refresh (re-publish) a specific content 
     * item.
     *
     * @since S60 5.2
     * @param aPublisherInfo publisher info.
     * @param aContentCid Textual identifier of the content to refresh.
     * @return True if the content publisher plug-in is found and the plugin 
     *         will refresh the content by calling its content observer. 
     *         False otherwise.
     */
    virtual TBool RefreshContent( const THsPublisherInfo& aPublisherInfo,
        const TDesC& aContentCid ) = 0;

    /**
     * Suspend content request. UI controller can use this interface to request
     * a content publisher plug-in to suspend a specific content 
     * item.
     *
     * @since S60 5.2
     * @param aPublisherInfo publisher info.
     * @param aContentCid Textual identifier of the content to refresh.
     * @return True if the content publisher plug-in is found and the plugin 
     *         will refresh the content by calling its content observer. 
     *         False otherwise.
     */    
    virtual TBool SuspendContent( const THsPublisherInfo& aPublisherInfo,
        const TDesC& aContentCid ) = 0;
        
    /**
     * Service to check if menu is open.
     *
     * @since S60 3.2
     * @return ETrue if menu is open, EFalse otherwise
     */
    virtual TBool QueryIsMenuOpen() = 0;                  
    };

#endif // M_AIEVENTHANDLEREXTENSION_H

// End of file
