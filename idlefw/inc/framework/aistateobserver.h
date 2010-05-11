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
class TAiFwPublisherInfo;

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
     */
    virtual void NotifyLoadPlugin( const TAiFwPublisherInfo& aInfo ) = 0;        

    /**
     * Content publisher destroy request.
     * 
     * @since S60 5.2
     * @param aInfo Publisher Info, which describes the plugin to be destroyed.
     */    
    virtual void NotifyDestroyPlugin( const TAiFwPublisherInfo& aInfo ) = 0;        
    
    /**
     * Notifies to reload previously released plugins
     * 
     * @since S60 5.2
     */    
    virtual void NotifyReloadPlugins() = 0;
    
    /**
     * Notifies that defined ECom plugins should be released to enable
     * plugin upgrade
     * 
     * @since S60 5.2
     */
    virtual void NotifyReleasePlugins( const RArray<TUid>& aUidList ) = 0;
    
    };
    
#endif // _AISTATEOBSERVER_H

// End of file
