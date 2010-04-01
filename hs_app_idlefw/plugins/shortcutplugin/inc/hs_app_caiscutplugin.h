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
* Description:  Plug-in main class
*
*/


#ifndef CAISCUTPLUGIN_H
#define CAISCUTPLUGIN_H

#include <aicontentpublisher.h>
#include <aipropertyextension.h>
#include <aieventhandlerextension.h>
#include <aicontentmodel.h>

class CAiScutEngine;
class MAiContentObserver;
class MAiContentItemIterator;
class CAiScutShortcut;

/**
 *  Plug-in main class
 *
 *  @since S60 v3.2
 */
class CAiScutPlugin : public CAiContentPublisher
                    , public MAiPropertyExtension
                    , public MAiEventHandlerExtension
{

public: // factory methods and destructor.

    static CAiScutPlugin* NewL();

    virtual ~CAiScutPlugin();

public: // new methods

    /**
     * Publishes the given shortcut
     *
     * @since S60 v3.2
     * @return Reference to a shortcut to publish
     */
    void PublishShortcutsL(RAiShortcutArray& aShortcuts);

    /**
     *
     */
    TBool IsAlive() const;

    /**
     * @return Resume reason
     */
    TAiTransitionReason ResumeReason() const;

public: // methods from base classes

    // from base class CAiContentPublisher

    void Resume(TAiTransitionReason aReason);

    void Suspend(TAiTransitionReason aReason);

    void Stop(TAiTransitionReason aReason);

    /**
     * From CAiContentPublisher
     * Adds the content observer / subscriber to plug-in. The plug-in MUST
     * maintain a registry of subscribers and send notification to all them
     * whenever the plug-in changes state or new content available.
     *
     * @since S60 3.2
     * @param aObserver content observer to register.
     */
    void SubscribeL(MAiContentObserver& aObserver);

    /**
     * From CAiContentPublisher
     * Configures the plug-in.
     * Plug-ins take ownership of the settings array, so it must either
     * store it in a member or free it. Framework has put the array in cleanup
     * stack so the plug-in shouldn't do that.
     * If this leaves, the plug-in will be destroyed by AI FW.
     * Plug-in must support LaunchByValue-event even if normal shortcuts don't
     * work. The only allowed serious enough leave is KErrNotFound from CenRep.
     *
     * @since S60 3.2
     * @param aSettings setting items defined in the UI definition.
     */
    void ConfigureL(RAiSettingsItemArray& aSettings);

    /**
     * From CAiContentPublisher
     * Returns interface extension. In Series 60 3.1 only event & property
     * extensions are supported. See MAiEventExtension & MAiPropertyExtension
     * interfaces.
     *
     * @since S60 3.2
     * @param  aUid - UID of the extension interface to access.
     * @return the extension interface. Actual type depends on the passed aUid
     *         argument.
     */
    TAny* Extension(TUid aUid);

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

    /**
	* Invoked by the framework for querying if plugin has menu item
	*
	* @return ETrue if plugin has specific menu item, EFalse otherwise
	*/   
	
// Online/Offline - web widgets	
    TBool HasMenuItem(const TDesC16& aMenuItem);
    
    // from base class MAiPropertyExtension

    /**
     * From MAiPropertyExtension.
     * Read property of publisher plug-in.
     *
     * @param aProperty - identification of property.
     * @return pointer to property value.
     * @since S60 3.2
     */
    TAny* GetPropertyL(TInt aProperty);

    /**
     * From MAiPropertyExtension.
     * Write property value.
     *
     * @param aProperty - identification of property.
     * @param aValue - contains pointer to property value.
     * @since S60 3.2
     */
    void SetPropertyL(TInt aProperty, TAny* aValue);

protected:

private:

    CAiScutPlugin();

    void ConstructL();

    void DoResumeL(TAiTransitionReason aReason);

    void FreeEngine();
    
    void DeleteDefaultShortcutsL();


private:  // data

    /**
     * Iterator for plug-in content.
     * Own.
     */
    MAiContentItemIterator*             iContent;

    /**
     * Iterator for plug-in resources.
     * Own.
     */
    MAiContentItemIterator*             iResources;

    /**
     * Iterator for plug-in events.
     * Own.
     */
    MAiContentItemIterator*             iEvents;

    /**
     * Plug-in engine
     * Own.
     */
    CAiScutEngine*                      iEngine;

    /**
     * Array of content observers
     * Not own.
     */
    RPointerArray<MAiContentObserver>   iObservers;

    /**
     * Information about the content publisher ( this plug-in ).
     */
    TAiPublisherInfo                    iInfo;

    /**
     * Plug-in state, suspended or alive.
     */
    TBool                               iAlive;

    /**
     * Resume reason
     */
    TAiTransitionReason                 iResumeReason;

    TBool                               iForcePublishAll;
};

#endif // CAISCUTPLUGIN_H

// End of File.
