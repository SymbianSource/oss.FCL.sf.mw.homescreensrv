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
* Description:  Plugin lifecycle observer
*
*/



#ifndef M_AIPLUGINLIFECYCLEOBSERVER_H
#define M_AIPLUGINLIFECYCLEOBSERVER_H

#include "aipropertyextension.h"
#include <aisystemuids.hrh>

class CAiContentPublisher;

// Unnamed namespace for local definitions
namespace
    {       
    const TInt KAIUidDevStaPlugin = AI_UID_ECOM_IMPLEMENTATION_CONTENTPUBLISHER_DEVSTAPLUGIN;
    
    const TInt KAIUidShortCutPlugin = AI_UID_ECOM_IMPLEMENTATION_CONTENTPUBLISHER_SHORTCUTPLUGIN;
    
    const TInt KAIUidProfilePlugin = AI_UID_ECOM_IMPLEMENTATION_CONTENTPUBLISHER_PROFILEPLUGIN;
    
    const TInt KAIUidSATPlugin = AI_UID_ECOM_IMPLEMENTATION_CONTENTPUBLISHER_SATPLUGIN;
    }

/**
 *  Plugin lifecycle observer base class.
 *
 *  @lib aifw_hs
 *  @since S60 3.2
 */
class MAiPluginLifecycleObserver
    {
public:
    
    /**
     * Report plugin created.
     * @param aPlugin reference to the created plugin.
     */
    virtual void PluginCreatedL( CAiContentPublisher& aPlugin ) = 0;

    /**
     * Report plugin destroyed.
     * @param aPlugin reference to the dtored plugin.
     */
    virtual void PluginDestroyed( CAiContentPublisher& aPlugin ) = 0;

    /**
     * Report all plugins created.
     */
    virtual void AllPluginsCreated() = 0;

    /**
     * Report all plugins destroyed.
     */
    virtual void AllPluginsDestroyed() = 0;
    	
protected:

    /**
     * Protected destructor prevents deletion through this interface.
     */
    ~MAiPluginLifecycleObserver() { };               
    };

#endif // M_AIPLUGINSTATEMANAGER_H

// End of File.
