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
* Description:  Profile publisher
*
*/


#ifndef C_AIPROFILEPUBLISHER_H
#define C_AIPROFILEPUBLISHER_H

#include <e32base.h>
#include <RSSSettings.h>
#include <MSSSettingsObserver.h>
#include <MProfileChangeObserver.h>
#include "hs_app_aidevicestatuspublisher.h"
#include "hs_app_aidevicestatuscontentmodel.h"

class MProfileEngine;
class MAiDeviceStatusContentObserver;
class MAiPropertyExtension;
class CProfileChangeNotifyHandler;

/**
 *  @ingroup group_devicestatusplugin
 *
 *  Observers profile changes and publishes profile name when profile
 *  has been changed or if ALS status changes.
 *
 *  @since S60 3.2
 */
class CAiProfilePublisher : public CBase, public MAiDeviceStatusPublisher,
                            public MSSSettingsObserver, public MProfileChangeObserver
    {
public:

    static CAiProfilePublisher* NewL();

    virtual ~CAiProfilePublisher();

protected:

// from base class MAiDeviceStatusPublisher

    void ResumeL();
    void Subscribe( MAiContentObserver& aObserver, 
                    MAiPropertyExtension& aExtension,
                    MAiPublishPrioritizer& aPrioritizer,
                    MAiPublisherBroadcaster& aBroadcaster );
    void RefreshL( TBool aClean );
    TBool RefreshL( TInt aContentId, TBool aClean );
    TBool RefreshContentWithPriorityL( TInt aContentId, TInt aPriority );
    
    
protected:

// from base class MSSSettingsObserver

    /**
     * Called when SS Settings changes.
     */
    void PhoneSettingChanged( TSSSettingsSetting aSetting, TInt aNewValue );

// from base class MProfileChangeObserver

    /**
     * Called when active profile changes.
     */
    void HandleActiveProfileEventL( TProfileEvent aProfileEvent, TInt aProfileId );

private:

    CAiProfilePublisher();

    void ConstructL();

    /**
     * Clean last published profile.
     */
    void CleanLastProfileL();

    /**
     * Get active profile from profile engine and publish it.
     */
    void RefreshProfileL(TBool aClean);


protected: // data

    /**
     * Client to listen SS setting changes.
     */
    RSSSettings iSSSettings;

    /**
     * Profile engine.
     * Own.
     */
    MProfileEngine* iProfileEngine;

    /**
     * Profile change notifier.
     * Own.
     */
    CProfileChangeNotifyHandler* iProfileNotifier;

    /**
     * Content observer.
     * Not own.
     */
    MAiContentObserver* iContentObserver;

    /**
     * Property extension.
     * Not own.
     */
    MAiPropertyExtension* iExtension;

	/**
	 * Content prioritizer.
	 * Not own.
	 */
	MAiPublishPrioritizer* iPrioritizer;
	
	/**
	 * Publish broadcaster.
	 * Not own.
	 */
	MAiPublisherBroadcaster* iBroadcaster;
    
    enum TLastPublishedProfile
        {
        ENothingPublished = -1,        
        EGeneralProfilePublished = 0,
        EOfflineProfilePublished = 5,
        EOtherProfilePublished = 10
        };
    
    /**
     * Last published profile.
     */
    TLastPublishedProfile iLastPublishedProfileId;
    
    /**
     * True if last published profile was silent.
     */
    TBool iLastPublishedProfileSilent;
    
    /**
     * True if last published profile was timed.
     */    
    TBool iLastPublishedProfileTimed;

    /**
     * True if publish was successful.
     */
    TBool iSuccess;
    
    /**
     * True if active profile is changed or modified.
     */
    TBool iActiveProfilePublish;
    };


#endif // C_AIPROFILEPUBLISHER_H
