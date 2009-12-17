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
* Description:  Interface for Active Idle content publisher plug-ins.
*
*/


#ifndef C_AICONTENTPUBLISHER_H
#define C_AICONTENTPUBLISHER_H

#include <ecom/ecom.h>
#include <aicontentpublisheruid.hrh>

/**
 * ECom plugin interface UID
 */
const TUid KInterfaceUidContentPlugin = { AI_UID_ECOM_INTERFACE_CONTENTPUBLISHER }; 

/**
 * API Extension UID for MAiPropertyExtension.
 *
 * @see CAiContentPublisher::Extension
 * @see MAiPropertyExtension
 */
const TUid KExtensionUidProperty      = { 0x1 };

/**
 * API Extension UID for MAiEventExtension.
 *
 * @see CAiContentPublisher::Extension
 * @see MAiEventExtension
 */
const TUid KExtensionUidEventHandler  = { 0x2 };

/**
 * Defines set of reasons for plug-in state change.
 */
enum TAiTransitionReason
    {
    
    /**
     * Unknown transition reason.
     */
    EAiUnknownTransitionReason = 0,
    
    /**
     * System has started up.
     */
    EAiSystemStartup,

    /**
     * System is shutting down.
     */
    EAiSystemShutdown,

    /**
     * Backlight on.
     */
    EAiBacklightOn,

    /**
     * Backlight off.
     */
    EAiBacklightOff,

    /**
     * Backup or restore has been initiated.
     */
    EAiBackupRestoreStarted,

    /**
     * Backup or restore has ended.
     */
    EAiBackupRestoreEnded,

    /**
     * Call started.
     */
    EAiPhoneCallStarted,

    /**
     * Call ended.
     */
    EAiPhoneCallEnded,
    
    /**
     * Active Idle UI Definition change has started.
     */
    EAiUiDefinitionChangeStarted,

    /**
     * Active Idle UI Definition change has ended.
     */
    EAiUiDefinitionChangeEnded,

    /**
     * Phone general theme has changed.
     */
    EAiGeneralThemeChanged,
    
    /**
     * Screen layout has changed.
     */
    EAiScreenLayoutChanged,

    /**
     * System clock crossed midnight.
     */
    EAiMidnightPassed,

    /**
     * Language has been changed.
     */
    EAiLanguageChanged,

    /**
     * System time has been changed by user.
     */
    EAiTimeChanged,

    /**
     * Idle changes to background.
     */
    EAiIdleBackground,

    /**
     * Idle changes to foreground.
     */
    EAiIdleForeground,

    /**
     * Suspending plugins.
     */
    EAiSuspendPlugins,

    /**
     * Keylock enabled.
     */
    EAiKeylockEnabled,

    /**
     * Keylock disabled.
     */
    EAiKeylockDisabled,
    
    /**
	 * Plugins can go online
	 */
	EAiIdleOnLine,
	
	/**
	 * Plugins must go offline 
	 */
	EAiIdleOffLine,
	
	/**
	 * Page changed    
	 */
	EAiIdlePageSwitch
    };

class MAiContentObserver;
class MAiPluginSettings;
typedef RPointerArray< MAiPluginSettings > RAiSettingsItemArray;

/**
 *  ECom plug-in interface that Active Idle plug-ins must implement.
 *  It is used to control plug-in life cycle: load/destroy plug-ins;
 *  suspend/resume plug-in execution.
 *
 *  @since S60 3.2
 */
class CAiContentPublisher : public CBase
    {
public:   // Constructors and destructor

    /**
     * Creates a new plug-in instance based on implementation UID.
     *
     * @param aImpUid implementation UID of plug-in to instantiate.
     * @return pointer to the instantiated interface implementation.
     * @pre Interface implementation exists by uid aImpUid.
     */
    inline static CAiContentPublisher* NewL(TUid aImpUid);

    /**
     * Creates a new plug-in instance based on mime type.
     *
     * @param aMime MIME type of plug-in to instantiate.
     * @return pointer to the instantiated interface implementation.
     */
    inline static CAiContentPublisher* NewL(const TDesC8& aMime);

    /**
     * Destroys instance of the plug-in. Called by the framework during plug-in
     * unloading phase.
     */
    inline virtual ~CAiContentPublisher();

public:  // New functions
    /**
     * This method transit the plugin to "Alive" state.
     * The method is called by the framework to instruct plug-in that it is
     * allowed to actively publish its data to its observers. This means the plugin
     * is allowed to consume memory and CPU resources, e.g plug-in is able load 
     * engines, run timers, perform asynchronous operations, etc. The method 
     * transits the plug-in to "Alive" state. There can be many concurrent
     * calls to resume, with different or the same reason code, this allows
     * the plugin to properly respond to enviroment change that raise the
     * need to re-publish content (changes like date/time change etc).
     *
     * @param aReason reason for state change, see TAiTransitionChange.
     * @pre None
     * @post Plugin is in resumed state and actively publishes its data.
	 *
	 * Short example what a typical resume implementation does.
	 * @code
	 * if( !MyEngineCreated() )
	 *     {
     *     CreateEngine();
	 *     }
     * StartEngine();
	 * @endcode
     */
    virtual void Resume(TAiTransitionReason aReason) = 0;

    /**
     * This method transits the plug-in to "Suspendend" state.
     * The method is called by the framework to instruct plug-in that it is
     * not allowed to consume CPU resources, e.g plug-in MUST stop each
     * timer, cancel outstanding asynchronous operations, etc. 
     *
     * @param aReason reason for state change, see TAiTransitionChange.
     * @pre None
     * @post Plugin suspends publishing data and free resources (timers etc).
     *
	 * Short example what a typical suspend implementation does.
	 * @code
     * DisableEngine();
	 * @endcode
     */
    virtual void Suspend(TAiTransitionReason aReason) = 0;

    /**
     * This method transits the plug-in to "Idle" state.
     * The method is called by the framework to request the plug-in free all
     * memory and CPU resources and close all its open files, the plug-in 
     * should unload its engines during backup operation.
     *
     * @param aReason reason for state change, see TAiTransitionChange.
     * @pre None
     * @post Plugin stops publishing data and frees all possible resources.
     *
	 * Short example what a typical stop implementation does.
	 * @code
     * DestroyEngine();
	 * @endcode
     */
    virtual void Stop(TAiTransitionReason aReason) = 0;

    /**
     * Adds the content observer / subscriber to plug-in. The plug-in MUST
     * maintain a registry of subscribers and send notification to all them
     * whenever the plug-in changes state or new content available.
     *
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
     * // Engine reports data changed
     *
     * const TDesC& data = iEngine->LatestData();
     * for( TInt i = 0; i < iMyContentObservers.Count(); ++i )
     *     {
     *     iMyContentObservers[i].Publish( data );
     *     }
	 * @endcode
     */
    virtual void SubscribeL(MAiContentObserver& aObserver) = 0;

    /**
     * Configures the plug-in.
     *
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
     * // We own the array so destroy it
     * aSettings.ResetAndDestroy();
	 * @endcode
     */
    virtual void ConfigureL( RAiSettingsItemArray& aSettings) = 0;

    /**
     * Returns interface extension. In S60 3.2 only event & property
     * extensions are supported. See MAiEventHandlerExtension & MAiPropertyExtension
     * interfaces.
     *
     * @param  aUid - UID of the extension interface to access.
     * @see MAiEventExtension
     * @see MAiPropertyExtension
     * @return the extension interface. Actual type depends on the passed aUid 
     *         argument.
     *
     * Example on how to properly return an extension.
     * @code
     * if (aUid == KExtensionUidProperty)
     *     {
     *     return static_cast<MAiPropertyExtension*>(this);
     *     }
     * else if (aUid == KExtensionUidEventHandler)
     *     {
     *     return static_cast<MAiEventHandlerExtension*>(this);
     *     }
     * return NULL; // Requested extension not supported
     * @endcode
     */
    virtual TAny* Extension(TUid aUid) = 0;

private:     // data
    /** An identifier used during destruction. */
    TUid iDestructKey; 
    };

inline CAiContentPublisher* CAiContentPublisher::NewL(TUid aImplUid)
    {
    TAny* ptr = REComSession::CreateImplementationL(aImplUid,
        _FOFF(CAiContentPublisher, iDestructKey));

    return reinterpret_cast<CAiContentPublisher*> (ptr);
    }

inline CAiContentPublisher* CAiContentPublisher::NewL(const TDesC8& aMime)
    {
    TEComResolverParams params;
    params.SetDataType(aMime);

    TAny* ptr = REComSession::CreateImplementationL(KInterfaceUidContentPlugin,
        _FOFF(CAiContentPublisher, iDestructKey), params);

    return reinterpret_cast<CAiContentPublisher*> (ptr);
    }

inline CAiContentPublisher::~CAiContentPublisher()
    {
    REComSession::DestroyedImplementation(iDestructKey);
    }

#endif // C_AICONTENTPUBLISHER_H
