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


#ifndef WRTDATAPLUGIN_H
#define WRTDATAPLUGIN_H

#include <aicontentpublisher.h>
#include <aipropertyextension.h>
#include <aicontentmodel.h>
#include <aieventhandlerextension.h>

// FORWARD DECLARATIONS
class MAiContentObserver;
class MAiContentItemIterator;
class MAiPSPropertyObserver;
class CWrtData;
class CDesC16Array;
class CGulIcon;
class CLiwDefaultMap;


// CLASS DECLARATION
/**
 *  @ingroup group_wrtdataplugin
 *
 *  Plug-in main class
 *
 *  @since S60 v3.2
 */
class CWrtDataPlugin : public CAiContentPublisher,
                     	public MAiPropertyExtension,
                     	public MAiEventHandlerExtension
                    
    {
        
    public :
        /**
        * Plugin's network state.
        */
        enum TPluginNetworkStatus 
            {
            EUnknown, 
            EOffline,
            EOnline
            };
        
        /**
         * Plugin's state.
         */
        enum TPluginStates
            {
            ENone,
            EResume,
            ESuspend,
            EInActive,
            };
        
        /**
        * Content Items
        */
        enum TContentItem
         {
         EDefaultImage,
         EDefaultText,
         EImage1
         };

    public:
    
        /**
        * Part of the two phased constuction
        *
        * @param none
        * @return none
        */
        static CWrtDataPlugin* NewL();
        
        /**
        * Destructor
        *
        * @param none
        * @return none
        */
        ~CWrtDataPlugin();

    public: // from base class CAiContentPublisher
        
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
         * @param aEvent - unique identifier of event from plug-in content model.
         * @param aParam - parameters associated with event. Each UI Definition
         *        declares events in the format: <event name>(<event params>),
         *        where <event name> is mapped by the framework to unique
         *        identifier supplied in aEvent, <event params> are provided to
         *        plug-in as-is in the descriptor.
         * @since S60 3.2
         */
         void HandleEvent(TInt aEvent, const TDesC& aParam);
        
         /**
         * From MAiEventHandlerExtension
         * Invoked by the framework when plug-in must handle an event.
         *
         * @param aEventName - name of the event from plug-in content model.
         * @param aParam - parameters associated with event. Each UI Definition
         *        declares events in the format: <event name>(<event params>),
         *        where  <event name> mapping to unique identifier supplied by event 
         *        is failed by the frame work then the  <event name> and  
         *        <event params>  are provided to plug-in as-is in the descriptor.
         */
         void HandleEvent(const TDesC& aEventName, const TDesC& aParam);
    
        /**
        * Invoked by the framework for querying if plugin has menu item
        *
        * @param aMenuItem  menu item name.
        * @return ETrue if plugin has specific menu item, EFalse otherwise 
        */
        TBool HasMenuItem(const TDesC16& aMenuItem);
         
    public : // New functions
    
        /**
        * Gets the id of a content  
        *
        * @param aObjectId image or text id
        * @return id of the content
        */
        TInt GetIdL(TDesC16& aObjectId);
        
        /**
        * Gets the type of a specific content
        *
        * @param aObjectId image or text id
        * @param aType type
        * @return void
        */
        void GetTypeL( TDesC16& aObjectId, TDes16& aType );
        
        /**
        * RefereshL a specific image of text in the widget
        *
        * @param aOperation operation performed
        * @param aDataMap data map
        * @return void
        */
        void RefreshL(TDesC16& aOperation, CLiwDefaultMap* aDataMap );
        
        /**
        * Is plugin active to publish the data 
        *
        * @param void 
        * @return boolean (ETrue/EFalse)
        */
        TBool IsActive();
        
        /**
        * Publish a specific text of the widget  
        *
        * @param aObserver observer
        * @param aContentId content model id
        * @param aContentValue content value
        * @return void
        */
        void PublishTextL(MAiContentObserver* aObserver, 
                TInt aContentId, const TDesC16& aContentValue);
    
        /**
        * Publish a specific image of the widget  
        *
        * @param aObserver observer
        * @param aContentId content model id
        * @param aHandle image handle 
        * @param aMaskHandle handle of the mask image
        * @return void
        */
        void PublishImageL(MAiContentObserver* aObserver, 
                TContentItem aContentId, TInt aHandle, TInt aMaskHandle);
    
        /**
        * Publish a specific image of the widget  
        *
        * @param aObserver observer
        * @param aContentId content model id
        * @param aPath image path / skin id pattern / mif id Pattern 
        * @return void
        */
        void PublishImageL(MAiContentObserver* aObserver,
                TContentItem aContentId, const TDesC16& aPath );
        
        /**
        * Cleans a data from the widget
        *
        * @param aObserver observer
        * @param aContentId content model id
        * @return void
        */
        void Clean(MAiContentObserver* aObserver, 
                TInt aContentId );
        
        /**
        * Shows the loading icon animation 
        *
        * @param aObserver observer
        * @return void
        */
        void ShowLoadingIcon(MAiContentObserver* aObserver);

        /**
        * Hides the loading icon animation 
        *
        * @param aObserver observer
        * @return void
        */
        void HideLoadingIcon(MAiContentObserver* aObserver);

        /**
        * CWrtData getter
        * @return Pointer to CWrtData
        */
        inline CWrtData* Data() const
            {
            return iData;
            }
    
        /*
        * Plugin's network status getter
        * @return Pointer to Harvester status observer
        */
        inline TPluginNetworkStatus NetworkStatus() const
            {
            return iNetworkStatus;
            }
    
    private:
        
        /**
        * Constructor
        *
        * @param none
        * @return none
        */
        CWrtDataPlugin();
        
        /**
        * Part of the two phased construction
        *
        * @param void
        * @return void
        */
        void ConstructL();
        
        /**
        * Publishes widget's texts and images
        *
        * @param void
        * @return void
        */
        void PublishL();

        /**
        * Resume the plug-in.
        *
        * @param aReason reason for state change, see TAiTransitionChange.
        * @return void
        */    
        void DoResumeL(TAiTransitionReason aReason);
        
        /**
        * Resolves skin item id and Mif id from pattern 
        * skin( <majorId> <minorId> (<colourGroupId>) 
        * mif(<MifFileName.mif> <bitmapId> <maskId>)
        * 
        * @param aPath  skin pattern / mif pattern value
        * @param aItemId skin item id  
        * @param aMifId  mif id 
        * @param aMaskId  mask id 
        * @param aFilename mif file name
        * @return boolean (ETrue/EFalse)  
        */
        TBool ResolveSkinIdAndMifId( const TDesC& aPath, TAknsItemID& aItemId,
                TInt& aMifId, TInt& aMaskId, TDes& aFilename );
        
    private: // data
    
        // Iterator for plugin content
        // Own
        MAiContentItemIterator* iContent;
    
        // Array of content observers
        // Own
        RPointerArray<MAiContentObserver> iObservers;
        
        // Information about the content publisher (this plug-in)
        TAiPublisherInfo iInfo;
        
        // Number of data in the content model.
        TInt iDataCount;
         
        // Dynamic content model
        // Own
        TAiContentItem* iContentModel;
        
        // Reference array for Published text
        // Own
        RPointerArray<HBufC> iDataArray;
        
        // Service API Data Subscriber.
        // Own
        CWrtData* iData;
        
        // References array for published images 
        // Own
        RArray<CGulIcon*> iIconArray;
        
        // Plugin's network status
        TPluginNetworkStatus iNetworkStatus;
        
        // Is Homescreen foreground.
        TBool iHSForeGround;
        
        // Is KeyLockON.
        TBool iKeyLockOn;
    
        // Plugin state    
        TPluginStates iPluginState;
    };

#endif // WRTDATAPLUGIN_H


