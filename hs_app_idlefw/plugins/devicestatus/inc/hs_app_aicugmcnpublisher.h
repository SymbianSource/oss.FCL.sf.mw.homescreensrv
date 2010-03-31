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
* Description:  Closed user group (CUG) publisher.
*
*/


#ifndef C_AICUGMCNPUBLISHER_H
#define C_AICUGMCNPUBLISHER_H


#include <e32base.h>
#include <RSSSettings.h>
#include <MSSSettingsObserver.h>
#include "hs_app_aidevicestatuspublisher.h"
#include "hs_app_aidevicestatuscontentmodel.h"
#include "hs_app_ainetworkinfoobserver.h"

class MAiDeviceStatusContentObserver;
class MAiPropertyExtension;
class CAiNetworkInfoListener;

const TInt KAnimDelay = 2000000;

/**
 *  @ingroup group_devicestatusplugin
 *
 *  Closed user group (CUG) and Micro Cellular Network (MCN) publisher.
 *
 *  @since S60 3.2
 */
class CAiCUGMCNPublisher : public CBase, public MAiDeviceStatusPublisher,
                        public MSSSettingsObserver, public MAiNetworkInfoObserver
    {
public:

    static CAiCUGMCNPublisher* NewL();

    virtual ~CAiCUGMCNPublisher();

protected:

// from base class MAiDeviceStatusPublisher

    void ResumeL();
    void Subscribe( MAiContentObserver& aObserver, 
                    MAiPropertyExtension& aExtension,
                    MAiPublishPrioritizer& aPrioritizer,
                    MAiPublisherBroadcaster& aBroadcaster );
    void RefreshL( TBool aClean );
    TBool RefreshL( TInt aContentId, TBool aClean );

// from base class MSSSettingsObserver

    void PhoneSettingChanged( TSSSettingsSetting aSetting, TInt aNewValue );

//from base class MAiNetworkInfoObserver

    void HandleNetworkInfoChange( const MNWMessageObserver::TNWMessages& aMessage, 
        						const TNWInfo& aInfo, 
        						const TBool aShowOpInd );

private:

    CAiCUGMCNPublisher();

    void ConstructL();

    /**
     * Updates CUGMCN indicator if needed.
     */
    void UpdateCUGMCNIndicatorL( TInt aValue );
    
    void Animate();
    
    void PublishCUG();
    
    void PublishMCN();
    
    /**
     * The call back function.
     * \param aAny A pointer to this class.
     */
    static TInt PeriodicTimerCallBack(TAny* aAny);
    
    void DoAnimation();


private: // data

    /**
     * Used for doing the animation.
     * Own.
     */
    CPeriodic* iPeriodic;
    
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
     * SS Settings client. Used to observer CUGMCN changes.
     */
    RSSSettings iSSSettings;
    
    /**
     * Network info listener.
     * Own.
     */
    CAiNetworkInfoListener* iListener;
    
    /**
     * Soft indicators.
     */
    HBufC* iCUG;
    HBufC* iMCN;
    
    TBool iAnimationDone;
    };

#endif // C_AICUGMCNPUBLISHER_H
