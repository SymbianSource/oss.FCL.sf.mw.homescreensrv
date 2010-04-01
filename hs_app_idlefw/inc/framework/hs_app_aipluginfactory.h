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



#ifndef C_AIPLUGINASYNCFACTORY_H
#define C_AIPLUGINASYNCFACTORY_H

#include "aicontentpublisher.h"
#include "aipropertyextension.h"
#include "aifwdefs.h"
#include <e32base.h>

class MAiContentObserver;
class MAiEventHandlerExtension;
class MAiContentItemIterator;
class CAiContentPublisher;
class CAiUiController;
class CAiPluginActivityRegistry;
class CImplementationInformation;
class CAiContentPluginManager;
class MAiPluginTool;
class MAiPluginLifecycleObserver;

/**
 * @ingroup group_aifw
 * 
 *  Content plugin factory class for Active idle framework.
 *
 *  @lib aifw
 *  @since S60 3.2
 */
NONSHARABLE_CLASS( CAiPluginFactory ) : public CBase
	{
	public:
	
// Constructor and destructor
		
		static CAiPluginFactory* NewL( RPointerArray<CAiContentPublisher>& aPlugins,
		                                    CAiContentPluginManager& aManager );
		
		virtual ~CAiPluginFactory();

// New functions
		
        /**
         * Create plugin
         *
         * @since S60 5.0
         * @param aPublisherInfo plugin to load.
         * @param aControllerArray array of active UI controllers.
         */
		void CreatePluginL( const TAiPublisherInfo& aPublisherInfo,						
						     RPointerArray<CAiUiController>& aControllerArray );						

        /**
         * Destroy plugin
         *
         * @since S60 5.0
         * @param aPublisherInfo plugin to destroy.
         * @param aControllerArray array of active UI controllers.
         */
		void DestroyPluginL( const TAiPublisherInfo& aPublisherInfo,		    
                             RPointerArray<CAiUiController>& aControllerArray );

        /**
         * Destroys all plugins
         * 
         * @since S60 5.0         
         */
		void DestroyPlugins();
		
		
        void AddLifecycleObserverL( MAiPluginLifecycleObserver& aObserver );

        /**
         * Finds plugin by publisher info.
         *
         * @since S60 5.0
         * @param aInfo publisher info.
         * @return Pointer to plugin, NULL if not found.
         */        
        CAiContentPublisher* PluginByInfoL( const TAiPublisherInfo& aInfo ) const;

        /**
         * Finds plugin by name.
         *
         * @since S60 5.0
         * @param aInfo publisher info.
         * @return Pointer to plugin, NULL if not found.
         */                
        CAiContentPublisher* PluginByNameL( const TDesC& aName ) const;
						
	private:	

// Constructors
		
		CAiPluginFactory( RPointerArray<CAiContentPublisher>& aPlugins,
		                       CAiContentPluginManager& aManager );
		
		void ConstructL();
										
// New functions
		
		CAiContentPublisher* CreatePluginLC( const TAiPublisherInfo& aPluginInfo );
								
		void SubscribeContentObserversL( CAiContentPublisher& aContentPublisher,		    
            const TAiPublisherInfo& aPublisherInfo,
		    RPointerArray<CAiUiController>& aControllerArray );
		                              
        void ConfigurePluginL( RPointerArray<CAiUiController>& aControllerArray,
                               CAiContentPublisher& aContentPublisher,
                               const TAiPublisherInfo& aPubInfo );		                           
        
	private:     // Data	
	    // Array of loaded data plugins, Not owned
	    RPointerArray<CAiContentPublisher>& iPlugins;
	    // Content plugin manager, Not owned	    		
        CAiContentPluginManager& iManager;
        // Ecom implementation info, Owned
		RImplInfoPtrArray iEComPlugins;		
		// Plugin tool from utility lib, Owned
		MAiPluginTool* iPluginTool;      
        // Life cycle observers, Owned
		RPointerArray<MAiPluginLifecycleObserver> iLifecycleObservers;		
    };

#endif // C_AIPLUGINASYNCFACTORY_H

// End of File.
