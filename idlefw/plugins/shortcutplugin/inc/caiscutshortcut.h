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
* Description:  Implementation for a shortcut.
*
*/


#ifndef CAISCUTSHORTCUT_H
#define CAISCUTSHORTCUT_H

#include <e32base.h>

#include "taiscutparser.h"

class CAiScutEngine;
class CAiScutTarget;
class MAiContentObserver;
class MAiPropertyExtension;

/**
 *  Implementation for a shortcut
 *
 *  Handles parsing and launching an application shortcut
 *
 *  @since S60 v3.2
 */
class CAiScutShortcut : public CTimer
{

public:

    /**
     * First phase constructor.
     * Leaves the object on cleanup stack
     *
     * @since S60 v3.2
     * @param aId Shortcut id
     * @param aTarget The shortcut definition string.
     * @param aEngine Reference to shortcut plug-in engine
     */
    static CAiScutShortcut* NewLC(TInt aId, const TDesC& aTarget,
        CAiScutEngine& aEngine);

    /**
     * First phase constructor
     *
     * @since S60 v3.2
     * @param aId Shortcut id
     * @param aTarget The shortcut definition string
     * @param aEngine Reference to shortcut plug-in engine
     */
    static CAiScutShortcut* NewL(TInt aId, const TDesC& aTarget,
        CAiScutEngine& aEngine);

    virtual ~CAiScutShortcut();

    /**
     * Returns the shortcut id
     *
     * @since S60 v3.2
     * @return Shortcut id
     */
    TInt32 Id() const;

    /**
     * Publishes the shortcut content, non leaving version
     *
     * @since S60 v3.2
     * @param aPlugin Reference to plug-in
     * @param aObserver Content observer to publish to
     */
    void Publish(MAiPropertyExtension& aPlugin, MAiContentObserver& aObserver);

    /**
     * Checks if the application pointed to can be launched and reverts to the
     * default setting if not. If no valid target can be found, the shortcut
     * becomes empty. May only leave with KErrNotReady to signify that application
     * architecture server or message server is not ready and access check should
     * be retried later.
     *
     * @since S60 v3.2
     * @param aCheckType Specifies which kind of shortcuts should check its access
     * @return ETrue if the shortcut target is accessible
     */
    TBool CheckAccessL(TInt aCheckType);

    /**
     *
     */
    TBool IsTargetChanged() const;

    /**
     * Changes the shortcut target to the user defined setting
     *
     * @since S60 v3.2
     * @param aNewTarget The new user defined shortcut target
     */
    void SetUserTarget(const TDesC& aNewTarget);

    /**
     * Deletes the user target.
     *
     * @since S60 v3.2
     */
    void DeleteUserTarget();

    /**
     * Launches the shortcut
     *
     * @since S60 v3.2
     */
    void LaunchL();

    /**
     * Launches the shortcut
     *
     * @since S60 v5.1
     * @param aMessage A message to pass
     */
    void LaunchL(const TDesC8& aMessage);
    
    /**
     * Set call state
     *
     * @since S60 v3.2
     */
    void SetCallState(TBool aStatus);

    /**
     *
     * @since S60 v3.2
     */
    void SetToBePublished(TBool aFlag);

    /**
     *
     * @since S60 v3.2
     */
    TBool NeedsToBePublished();

    /**
     * Return shortcut type.
     *
     * @since S60 v3.2
     */
    TShortcutType Type() const;

    /**
     * Return application uid of this shortcut.
     *
     * @since S60 v3.2
     */
    TUid AppUid() const;

    /**
     * Return an additional id this target may use when 
     * launching.
     *
     * @since S60 v3.2
     */
    TUid AdditionalUid() const;
    
    /**
     * Return current valid definition.
     *
     * @since S60 v3.2
     */
    TPtrC ActiveDefinition();
    
    /**
     * Set the icon to publish to softkeys,
     * icon area or to touch toolbar. Uses
     * aIcon.iDestination to determine which icon
     * is in question
     *
     * @since S60 v5.0
     */
    void SetIcon(TAiScutIcon aIcon);


protected:


    CAiScutShortcut(TInt aId, CAiScutEngine& aEngine);

    void ConstructL(const TDesC& aTarget);

    /**
     * Creates a shortcut target object
     *
     * @since S60 v3.2
     * @param aDefinition Shortcut target definition
     * @return Pointer to shortcut target object
     */
    CAiScutTarget* CreateTargetL(
        const TDesC& aDefinition, TBool aCreateUserTarget);

    /**
     * Publishes the shortcut content, leaving version
     *
     * @since S60 v3.2
     * @param aPlugin Reference to plug-in
     * @param aObserver Content observer to publish to
     */
    virtual void PublishL(MAiPropertyExtension& aPlugin, MAiContentObserver& aObserver);

    /**
     * Publishes shortcut caption
     *
     * @since S60 v3.2
     * @param aPlugin Reference to plug-in
     * @param aObserver Content observer to publish to
     */
    virtual TInt PublishCaption(
        MAiPropertyExtension& aPlugin, MAiContentObserver& aObserver,
        TInt aCaptionContentId) const;

    /**
     * Publishes shortcut icon
     *
     * @since S60 v3.2
     * @param aPlugin Reference to plug-in
     * @param aObserver Content observer to publish to
     */
    virtual TInt PublishIcon(
        MAiPropertyExtension& aPlugin, MAiContentObserver& aObserver, TInt aIconContentId);

    /**
     * Set the icon to use instead of default icon
     *
     * @since S60 v3.2
     */
    void SetOverrideIcon(TAiScutIcon aIcon);

    /**
     * Set the icon to publish to softkeys
     *
     * @since S60 v3.2
     */
    void SetSoftkeyIcon(TAiScutIcon aIcon);

    /**
     * Set the icon to publish to touch toolbar
     *
     * @since S60 v5.0
     */
    void SetToolbarIcon(TAiScutIcon aIcon);
    
// from CActive/CTimer
		
	TInt RunError(TInt aError);
		
	void DoCancel();
		
	void RunL();

protected:  // data

    /**
     * Shortcut id.
     */
    TInt32          iId;

    /**
     * Default shortcut target.
     * Own.
     */
    CAiScutTarget*  iDefaultTarget;

    /**
     * User defined shortcut target.
     * Own.
     */
    CAiScutTarget*  iUserTarget;

    /**
     * Stores the old target data while the new one is being published.
     * During publishing the framework might still dereference pointers in the old
     * target so it must be kept alive until the new target is successfully published.
     * If publishing fails, this is taken back to use and the new target is deleted.
     * Own.
     */
    CAiScutTarget*  iRetiredTarget;

    /**
     * Pointer to the last published target.
     * DO NOT DELETE THROUGH THIS!
     * Not own.
     */
    CAiScutTarget*  iLastPublishedTarget;

    /**
     * Pointer to the active target.
     * Can point either to iDefaultTarget, iUserTarget or NULL
     * DO NOT DELETE THROUGH THIS!
     * Not own.
     */
    CAiScutTarget*  iActiveTarget;

    /**
     * Reference to the shortcut plug-in engine.
     */
    CAiScutEngine&  iEngine;

    /**
     * Boolean indicating if this shortcut needs to be published.
     */
    TBool           iNeedsToBePublished;

    /**
     * Call state
     */
    TBool           iActiveCall;
    
    /**
     * Force the publish of this shortcut. For example
     * after call state has changed the actual shortcut
     * isn't changed but it still needs to be published.
     */
    TBool           iForcePublish;

};

#endif // CAISCUTSHORTCUT_H

// End of File.
