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
* Description:  Content plugin manager class for Active idle framework.
*
*/



#ifndef C_AICONTENTPLUGINMANAGER_H
#define C_AICONTENTPLUGINMANAGER_H

#include <e32base.h>
#include "aicontentmodel.h"
#include "aicontentpublisher.h"
#include "aifwdefs.h"
#include "aipluginsettings.h"
#include "hs_app_aipluginfactory.h"

class MAiContentObserver;
class MAiEventHandlerExtension;
class MAiPluginTool;
class CAiContentPublisher;
class CAiPluginStateManager;
class CAiUiController;
class CImplementationInformation;

/**
 * @ingroup group_aifw
 * 
 *  Content plugin manager class for Active idle framework.
 *
 *  @lib aifw
 *  @since S60 3.2
 */
NONSHARABLE_CLASS( CAiContentPluginManager ) : public CBase
                                               
	{
public: // Constructor and destructor	        	
		
    static CAiContentPluginManager* NewL();
                                              
    virtual ~CAiContentPluginManager();
		    
public: // New functions
    
    /**
     * Forward plugin event to plugins.
     *
     * @since S60 3.2
     * @param aParam event string.
     */
    void HandlePluginEvent( const TDesC& aParam );

    /**
     * Forward plugin event to plugins.
     *
     * @since S60 5.0
     * @param aPublisherInfo publisher info.
     * @param aParam event string.
     */
    void HandlePluginEventL( const TAiPublisherInfo& aPublisherInfo, const TDesC& aParam ); 
            
    /**
     * Queries wheter a plugin has settigns or not.
     */
    TBool HasMenuItemL( const TAiPublisherInfo& aPublisherInfo, const TDesC& aMenuItem );

    /**
     * Requests a Content publisher plug-in to refresh a content item.
     */
    TBool RefreshContent( const TDesC& aContentCid );

    /**
     * Sets plugins to online/offline
     * 
     * @since S60 5.0
     * @param aOnline ETrue to set plugins online, EFalse to offline
     * @paran aPublishers List of publishers
     */    
    void ProcessOnlineState( TBool aOnline );
            
    /**
     * Gets plugin state manager.     
     *
     * @since S60 5.0
     * @return plugin state manager.
     */        
    CAiPluginStateManager& StateManager() const;

    /**
     * Gets plugin factory.     
     *
     * @since S60 5.0
     * @return plugin factory.
     */        
    CAiPluginFactory& PluginFactory() const;
    
    
private: // Constructors

    CAiContentPluginManager();
                             
    void ConstructL();

private: // New functions
                                        
    void GetIdL( CAiContentPublisher& aContentPublisher,
                 TAiPublisherProperty aProperty,            
                 const TDesC& aName, TInt& aId ); 
            
    TInt RefreshContentL( const TDesC& aContentCid );

private: // Data

    /**
     * Plugins array.
     * Owned. 
     */
    RPointerArray< CAiContentPublisher > iPlugins;

    /**
     * Plugin factory.
     * Owned.
     */
    CAiPluginFactory* iPluginFactory;
    
    /**
     * System state observer.
     * Owned.
     */    
    CAiPluginStateManager* iStateManager;
    
    /**
     * Plugin tool from utility lib.
     * Owned.
     */
    MAiPluginTool* iPluginTool;     
    };

#endif // C_AICONTENTPLUGINMANAGER_H

// End of File.
