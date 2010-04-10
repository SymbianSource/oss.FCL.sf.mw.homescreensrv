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
* Description:  State Observer
*
*/


#ifndef _AISTATEOBSERVER_H
#define _AISTATEOBSERVER_H

// System includes

// User includes
#include <aifwdefs.h>

// Forward declarations
class THsPublisherInfo;

/**
 * State Observer
 * 
 * @ingroup group_aifw
 * @lib aifw.lib
 * @since S60 5.2
 */
class MAiStateObserver
    {
public:
    /**
     * State change notification.
     * 
     * @since S60 5.2
     * @param aReason State change.
     */
    virtual void NotifyStateChange( TAiFwState aState ) = 0;
    
    /**
     * Content publisher load request.
     * 
     * @since S60 5.2
     * @param aInfo Publisher Info, which describes the plugin to be loaded.
     * @param aReason Startup reason, which will be forwarded to the plugin.
     * @return Error code, KErrNone if succesfully loaded. 
     */
    virtual TInt NotifyLoadPlugin( const THsPublisherInfo& aInfo,
        TAiFwLoadReason aReason ) = 0;

    /**
     * Content publisher destroy request.
     * 
     * @since S60 5.2
     * @param aInfo Publisher Info, which describes the plugin to be destroyed.
     * @param aReason Shutdown reason, which will be forwarded to the plugin.
     */    
    virtual void NotifyDestroyPlugin( const THsPublisherInfo& aInfo,
        TAiFwDestroyReason aReason ) = 0;
    
    /**
     * Notifies to update content publishers after Ecom registry change.
     * 
     * @since S60 5.2
     */    
    virtual void NotifyUpdatePlugins() = 0;
    
    /**
     * Queries whether online state is in use
     * by any of the currently loaded plugin.
     * 
     * @since S60 5.2
     * @return ETrue if online/offline state needed, EFalse otherwise
     */
    virtual TBool OnlineStateInUse() const = 0;    
    };
    
#endif // _AISTATEOBSERVER_H

// End of file