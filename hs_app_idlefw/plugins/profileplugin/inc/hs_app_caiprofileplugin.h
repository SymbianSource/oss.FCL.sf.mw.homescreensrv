/*
* Copyright (c) 2005-2007 Nokia Corporation and/or its subsidiary(-ies).
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
* Description:  Plug-in main class
*
*/


#ifndef CAIPROFILEPLUGIN_H
#define CAIPROFILEPLUGIN_H

#include <aicontentpublisher.h>
#include <aipropertyextension.h>
#include <aicontentmodel.h>
#include <aieventhandlerextension.h>
#include "hs_app_maiprofilepluginnotifier.h"

class MAiContentObserver;
class MAiContentItemIterator;
class CAiProfileEngine;

/**
 *  @ingroup group_profileplugin
 *
 *  Plug-in main class
 *
 *  @since S60 v3.2
 */
class CAiProfilePlugin : public CAiContentPublisher,
                     	 public MAiPropertyExtension,
                     	 public MAiProfilePluginNotifier,
                     	 public MAiEventHandlerExtension
                     	 
                    
    {

public:

    /**
    * Part of the two phased constuction
    *
    * @param none
    * @return none
    */
    static CAiProfilePlugin* NewL();
    
    /**
    * Constructor
    *
    * @param none
    * @return none
    */
    CAiProfilePlugin();
    
    /**
    * Destructor
    *
    * @param none
    * @return none
    */
    ~CAiProfilePlugin();
    
    /**
    * Publishes profile names
    *
    * @param void
    * @return void
    */
    void PublishL();

// from base class CAiContentPublisher
  
    /**
    * From CAiContentPublisher
    * The method is called by the framework to request the plug-in free all
    * memory and CPU resources and close all its open files, e.g. the plug-in 
    * should unload its engines due backup operation. The method transits the 
    * plug-in to "Idle" state.
    *
    * @param aReason reason for state change, see TAiTransitionChange.
    * @return void
    */
    void Stop( TAiTransitionReason aReason );

    /**
    * From CAiContentPublisher
    * The method is called by the framework to instruct plug-in that it is
    * allowed to consume CPU resources, e.g plug-in is able to run timers,
    * perform asynchronous operations, etc. The method transits the plug-in
    * to "Alive" state.
    *
    * @param aReason reason for state change, see TAiTransitionChange.
    * @return void
    */
    void Resume( TAiTransitionReason aReason );

    /**
    * From CAiContentPublisher
    * The method is called by the framework to instruct plug-in that it is
    * not allowed to consume CPU resources, e.g plug-in MUST stop each
    * timers, cancel outstanding asynchronous operations, etc. The method
    * transits the plug-in to "Suspendend" state.
    *
    * @param aReason reason for state change, see TAiTransitionChange.
    * @return void
    */
    void Suspend( TAiTransitionReason aReason );

    /**
    * From CAiContentPublisher
    * Adds the content observer / subscriber to plug-in. The plug-in MUST
    * maintain a registry of subscribers and send notification to all them
    * whenever the plug-in changes state or new content available.
    *
    * @param aObserver content observer to register.
    * @return void
    */
    void SubscribeL( MAiContentObserver& aObserver );
    
    /**
    * From CAiContentPublisher
    * Configures the plug-in.
    * Plug-ins take ownership of the settings array, so it must either
    * store it in a member or free it. Framework has put the array in cleanup
    * stack so the plugin shouldn't do that.
    * If this leaves, the plug-in will be destroyed by AI FW.
    * Plug-in must support LaunchByValue-event even if normal shortcuts don't
    * work. The only allowed serious enough leave is KErrNotFound from CenRep.
    *
    * @param aSettings setting items defined in the UI definition.
    * @return void
    */
    void ConfigureL( RAiSettingsItemArray& aSettings );

    /**
    * From CAiContentPublisher
    * Returns interface extension. In Series 60 3.1 only event & property
    * extensions are supported. See MAiEventExtension & MAiPropertyExtension
    * interfaces.
    *
    * @param  aUid - UID of the extension interface to access.
    * @return the extension interface. Actual type depends on the passed aUid 
    *         argument.
    */
    TAny* Extension( TUid aUid );  

// from base class MAiPropertyExtension

    /**
    * From MAiPropertyExtension.
    * Read property of publisher plug-in.
    *
    * @param aProperty - identification of property.
    * @return pointer to property value.
    */
    TAny* GetPropertyL( TInt aProperty );

    /**
    * From MAiPropertyExtension.
    * Write property value.
    *
    * @param aProperty - identification of property.
    * @param aValue - contains pointer to property value.
    */
    void SetPropertyL( TInt aProperty, TAny* aValue );
  
 // from base class MAiEventHandlerExtension
   
     /**
     * From MAiEventHandlerExtension
     * Invoked by the framework when plug-in must handle an event.
     *
     * @param aEvent - unique identifier of event from plug-in content model.
     * @param aParam - parameters associated with event. Each UI Definition
     *        declares events in the format: <event name>(<event params>),
     *        where <event name> is mapped by the framework to unique
     *        identifier supplied in aEvent, <event params> are provided to
     *        plug-in as-is in the descriptor.
     * @since S60 3.2
     */
    void HandleEvent(TInt aEvent, const TDesC& aParam);
 
// from base class MAiProfilePluginNotifier  
  
    /**
    * Receives a notification of the content update event
    *
    * @param void
    * @return void
    */
	void NotifyContentUpdate();
	
protected:

private:

    /**
    * Part of the two phased construction
    *
    * @param void
    * @return void
    */
    void ConstructL();
    
    /**
    * Resume the plug-in.
    *
    * @param aReason reason for state change, see TAiTransitionChange.
    * @return void
    */    
    void DoResumeL(TAiTransitionReason aReason);
    
    /**
    * Free the engine
    *
    * @param void
    * @return void
    */  
    void FreeEngine();

    /**
    * Clean profile names from published content
    *
    * @param void
    * @return void
    */  
    void CleanPublishedProfileNames();

private: // data

    // Iterator for plugin content
    // Own
    MAiContentItemIterator* iContent;

    // Iterator for plugin events
    // Own
    MAiContentItemIterator* iEvents;
    
    // Iterator for plug-in resources.
    // Own
    MAiContentItemIterator* iResources;
     
    // Plugin engine
    // Own
    CAiProfileEngine* iEngine;

    // Array of content observers
    // Own
    RPointerArray<MAiContentObserver> iObservers;
    
    // Information about the content publisher (this plug-in)
    TAiPublisherInfo iInfo;
    
    // Current profile count
    TInt iCurrentCount;

    // Profile count that is used as reference when profile list is updated
    TInt iPreviousCount;
	
	//Plug-in state, suspended or alive. 
    TBool iAlive;
	
	// Boolean, which expresses whether the content has been updated
	TBool iIsUpdated;
	
	HBufC *iActiveProfileAndChar;
	
	HBufC *iPreviousProfileNameAndChar;
    };

#endif // CAIPROFILEPLUGIN_H


