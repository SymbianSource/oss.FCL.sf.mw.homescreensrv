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
* Description:  Interface for Homescreen content publisher plug-ins.
*
*/

#ifndef _HSCONTENTPUBLISHER_H
#define _HSCONTENTPUBLISHER_H

// System includes
#include <e32base.h>
#include <ecom/ecom.h>

// User includes
#include <hscontentpublisheruid.hrh>
#include <hspublisherinfo.h>

// Constants
/**
 * ECom plugin interface UID
 *
 * @since S60 5.2 
 */
const TUid KInterfaceUidHsContentPlugin = { HS_UID_ECOM_INTERFACE_CONTENTPUBLISHER }; 

// Forward declarations
class MAiContentObserver;
class MAiPluginSettings;

// Type definitions
typedef RPointerArray< MAiPluginSettings > RAiSettingsItemArray;

/**
 *  ECom plug-in interface that Homescreen plug-ins must implement.
 *  It is used to control plug-in life cycle: load/destroy plug-ins;
 *  suspend/resume plug-in execution.
 *
 *  @since S60 5.2
 */
class CHsContentPublisher : public CBase
    {
public:
    // Data types

    /**
     * CHsContentPublisher start-up reason.
     * 
     * @since S60 5.2
     * @see CHsContentPublisher::Start()
     */
    enum TStartReason
        {
        ESystemStartup = 1,
        EPageStartup,
        EPluginStartup
        };
    
    /**
     * CHsContentPublisher shutdown reason.
     * 
     * @since S60 5.2
     * @see CHsContentPublisher::Stop()
     */
    enum TStopReason
        {
        ESystemShutdown = 1,
        EPageShutdown,
        EPluginShutdown
        };
    
    /**
     * CHsContentPublisher resume reason.
     * 
     * @since S60 5.2
     * @see CHsContentPublisher::Resume()
     */
    enum TResumeReason
        {
        /**
         * Homescreen is visible.
         */
        EForeground = 1
        };
    
    /**
     * CHsContentPublisher suspend reason.
     * 
     * @since S60 5.2
     * @see CHsContentPublisher::Suspend()
     */
    enum TSuspendReason
        {
        /**
         * Homescreen is invisible.
         */        
        EBackground = 1,
        /**
         * Backup/Restore is ongoing.
         *
         * The plug-in must realease all its resources 
         * which affects to backup/restore operation.
         */        
        EBackupRestore,        
        /**
         * General Theme is changed.
         *
         * The plug-in must re-create any data which is themeable.
         */
        EGeneralThemeChange
        };
    
    /**
     * CHsContentPublisher properties.
     * 
     * @since S60 5.2
     * @see CHsContentPublisher::SetProperty()
     * @see CHsContentPublisher::GetProperty() 
     */
    enum TProperty
        {
        /**
         * Enables read-only access to iterator of content selectors. GetProperty
         * must return instance of MAiContentItemIterator for content selectors.
         */        
        EPublisherContent = 1,
        
        /**
         * Enables read-only access to iterator of content references. GetProperty
         * must return instance of MAiContentItemIterator for content references.
         */    
        EPublisherResources,
        
        /**
         * Enables read-only access to iterator of events supported by plug-in.
         * GetProperty must return instance of MAiContentItemIterator for events.
         */    
        EPublisherEvents,
        
        /**
         * Provides access to MAiContentRequest interface for refreshing a content
         * item.
         * @see EPublisherContent
         */    
        EContentRequest,
        
        /**
         * Provides access to MAiContentRequest interface for refreshing a resource
         * item.
         * @see EPublisherResources
         */    
        EResourceRequest,
        
        /**
         * Provides access to localized plugin name if supported. HBufC*
         * @see EPublisherResources
         */    
        EPluginName
        };

public:
    // Constructor and destructor

    /**
     * Creates a new plug-in instance based on implementation UID.
     *     
     * @param aPublisherInfo This plug-in's publisher info.
     * @return pointer to the instantiated interface implementation.
     * @pre Interface implementation exists by uid aImpUid.
     */
    inline static CHsContentPublisher* NewL( 
        const THsPublisherInfo& aPublisherInfo );

    /**
     * Destroys instance of the plug-in. Called by the framework during plug-in
     * destruction phase.
     */
    inline ~CHsContentPublisher();

public:  
    // new functions
        
    /**
     * This method transits the plug-in into "Idle" state.
     * Parameter aReason explains the plug-in's startup reason.
     * Based on THsStartReason plug-in may decide its startup behavior.
     *
     * This method is called by the framework after plugin is constructed and configured.
     *
     * @since S60 5.2
     * @param aReason startup reason, see TStartReason.
     * @pre None
     * @post Plugin is in Idle state.     
     */
    virtual void Start( TStartReason aReason ) = 0;
    
    /**
     * This method transits the plug-in into its final state.
     * Parameter aReason explains the plug-in's shutdwon reason.
     * Based on THsStopReason plug-in may prepare itself for next startup.
     * 
     * This method is called by the framework before plugin is destroyed.
     *
     * @since S60 5.2
     * @param aReason reason for state change, see TStopReason.
     * @pre None
     * @post Plugin is ready to be destroyed.
     */
    virtual void Stop( TStopReason aReason ) = 0;

    /**
     * This method transits the plug-in into "Alive" state.
     *     
     * In this state the plug-in is allowed to actively publish data 
     * to its observers, and it can consume memory and CPU resources.
     *
     * @since S60 5.2
     * @param aReason reason for state change, see TResumeReason.
     * @pre None
     * @post Plugin is in "Alive" state and actively publishes its data.
     *
     * Short example what a typical resume implementation does.
     * @code
     * if( !MyEngineCreated() )
     *      {
     *      CreateEngine();
     *      StartEngine();
     *      }
     *  else
     *      {
     *      // Publish only changed data!
     *      RefreshData();
     *      }          
     * @endcode
     */    
    virtual void Resume( TResumeReason aReason ) = 0;
    
    /**
     * This method transits the plug-in into "Suspended" state.
     * 
     * In this state the plug-in is not allowed to publish data 
     * to its observers. CPU resource usage must be minimal, e.g.
     * timers must be stopped, outstanding asynchronous operations must
     * be canceled, etc.
     *
     * @since S60 5.2
     * @param aReason reason for state change, see TSuspendReason.
     * @pre None
     * @post Plugin suspends publishing data and free resources (timers etc).
     *
     * Short example what a typical suspend implementation does.
     * @code
     * SuspendEngine();
     * @endcode
     */    
    virtual void Suspend( TSuspendReason aReason ) = 0;
    
    /**
    * This method transits the plug-in into "Online" sub-state.
    * In this state plugin is allowed to use network connections.
    *
    * @since S60 5.2
    */            
    inline virtual void SetOnline();

    /**
    * This method transits the plug-in into "Offline" sub-state. 
    * In this state plugin is not allowed to use network connections.
    *
    * @since S60 5.2
    */                
    inline virtual void SetOffline();
    
    /**
     * Adds the content observer / subscriber to the plug-in. The plug-in must
     * maintain a registry of subscribers and publish data to all of them
     * when new content is available in "Alive" state.
     *
     * @since S60 5.2
     * @param aObserver content observer to register.
     * @pre None
     * @post Plugin publishes its data to the subscribed observer.
     *
     * Short example what a typical subscribe implementation does and
     * one alternative how observers are used.
     * @code
     * if( !ObserverAlreadyAdded( aObserver ) )
     *     {
     *     iMyContentObservers.AppendL( aObserver );
     *     }
     *
     * ...
     *
     * // Engine reports data changed in "Alive" state
     *
     * const TDesC& data = iEngine->LatestData();
     * for( TInt i = 0; i < iMyContentObservers.Count(); ++i )
     *     {
     *     iMyContentObservers[i].Publish( data );
     *     }
     * @endcode
     */    
    virtual void SubscribeL( MAiContentObserver& aObserver ) = 0;
    
    /**
     * Configures the plug-in.
     *
     * @since S60 5.2
     * @param aSettings setting items defined in the UI definition.
     *                  This plugin takes ownership of the
     *                  MAiPluginSettings objects in the array.
     *                  If this method leaves the caller will handle the cleanup.
     * @pre None
     * @post Plugin has set its state according to relevant settings.
     *
     * Short example how to read plugin settings.
     * @code
     * for( TInt i = 0; i < aSettings.Count(); ++i )
     *     {
     *     MAiPluginSettingsItem& item = (aSettings[i])->AiPluginSettingsItem();
     *     TInt32 value = 0;
     *     if( ParseInt( value, item.Value() ) != KErrNone )
     *         {
     *         continue;
     *         }
     *     if( value < 0 )
     *         {
     *         continue; // All our settings are counts, skip bad settings
     *         }
     *     if( item.Key() == EMySettingMaxUsers )
     *         {
     *         iEngine->SetMaxUsers( value );
     *         continue;
     *         }
     *     else if( item.Key() == EMySettingNumItems )
     *         {
     *         iNumItems = value;
     *         continue;
     *         }
     *     }
     * // aSettings ownership is passed to the plug-in, destroy array.
     * aSettings.ResetAndDestroy();
     * @endcode
     */    
    virtual void ConfigureL( RAiSettingsItemArray& aSettings ) = 0;
           
    /**
     * Sets property value.
     *
     * @since S60 5.2
     * @param aProperty - identification of property.
     * @param aValue - contains pointer to property value.
     * @see TProperty.
     *     
     * An example of setting a property
     * @code               
     * void CMyPlugin::SetPropertyL( TInt TProperty, TAny* aValue )
     *     {
     *     if( !aValue ) 
     *         {
     *         return;
     *         }
     *
     *     // Save the property here to a member variable
     *     }
     * @endcode
     */    
    inline virtual void SetProperty( TProperty aProperty, TAny* aAny );
    
    /**
     * Gets property value.
     *
     * @since S60 5.2
     * @param aProperty - identification of property.
     * @return Pointer to property value.
     * @see TProperty.
     *
     * An example of getting a property
     * @code          
     * void CMyPlugin::ConstructL()
     *     {     
     *     iContent = AiUtility::CreateContentItemArrayIteratorL( KMyPluginContent );
     *     iResources = AiUtility::CreateContentItemArrayIteratorL( KMyPluginResources );
     *     iEvents = AiUtility::CreateContentItemArrayIteratorL( KMyPluginEvents );
     *     }
     *
     * TAny* CMyPlugin::GetPropertyL( TProperty aProperty )
     *     {
     *     switch( aProperty )
     *         {
     *         case EPublisherContent:
     *             return iContent;
     * 
     *         case EPublisherResources:
     *             return iResources;
     * 
     *         case EPublisherEvents:
     *             return iEvents;
     *         }
     *     return NULL;
     *     }
     * @endcode
     */    
    inline virtual TAny* GetProperty( TProperty aProperty );
    
    /**
     * Invoked by the framework when the plug-in must handle an event.
     *
     * @since S60 5.2
     * @param aEvent - unique identifier of event from plug-in content model.
     * @param aParam - parameters associated with event. Each UI Definition
     *        declares events in the format: <event name>(<event params>),
     *        where <event name> is mapped by the framework to unique
     *        identifier supplied in aEvent, <event params> are provided to
     *        plug-in as-is in the descriptor.
     */    
    inline virtual void HandleEvent( TInt aEvent, const TDesC& aParam );
    
    /**
    * Invoked by the framework when the plug-in must handle an event.
    *
    * @since S60 5.2
    * @param aEventName - name of the event from plug-in content model.
    * @param aParam - parameters associated with event. Each UI Definition
    *        declares events in the format: <event name>(<event params>),
    *        where  <event name> mapping to unique identifier supplied by event 
    *        is failed by the frame work then the  <event name> and  
    *        <event params>  are provied to plug-in as-is in the descriptor.
    */    
    inline virtual void HandleEvent( const TDesC& aEventName, const TDesC& aParam );
    
    /**
    * Invoked by the framework to query whether the plug-in has a menu item.
    *
    * @since S60 5.2
    * @param aMenuItem menu item name
    * @return ETrue if plugin has specific menu item, EFalse otherwise
    */        
    inline virtual TBool HasMenuItem( const TDesC& aMenuItem );
            
    /**
     * Returns interface extension. In S60 5.2 extensions are not provided. 
     *
     * @param  aUid - UID of the extension interface to access.
     * @return the extension interface. Actual type depends on the passed aUid 
     *         argument.
     *
     * Example on how to properly return an extension.
     * @code
     * return NULL; // Requested extension not supported
     * @endcode
     */    
    inline virtual TAny* Extension( TUid aUid );
    
    /**
     * Gets plug-in's publisher info.
     *
     * @since S60 5.2
     * @return PublisherInfo 
     */        
    inline const THsPublisherInfo& PublisherInfo() const;
    
private:     
    // data
    
    /** An identifier used during destruction. */
    TUid iDestructKey;
    /** Publisher info. */
    mutable THsPublisherInfo iPublisherInfo;
    };

#include <hscontentpublisher.inl>

#endif // _HSCONTENTPUBLISHER_H

// End of file
