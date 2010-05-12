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
* Description:  Listens Bluetooth SIM Access Profile (BT SAP) and publishes
*					text when BT SAP is activated.
*
*/


#ifndef C_AIBTSAPPUBLISHER_H
#define C_AIBTSAPPUBLISHER_H

// System includes
#include <e32base.h>
#include <coemain.h>
#include <aidevicestatuscontentmodel.h>
#include <RSSSettings.h>
#include <MSSSettingsObserver.h>
#include <MProfileChangeObserver.h>

// User includes
#include "aidevicestatuspublisher.h"

// Forward declarations
class MAiDeviceStatusContentObserver;
class CHsContentPublisher;

/**
 *  @ingroup group_devicestatusplugin
 *
 *  Listens Bluetooth SIM Access Profile (BT SAP) and publishes text from
 *  resource file with EAiDeviceStatusContentBTSAP content id when
 *  BT SAP is activated.
 *
 *  @since S60 3.2
 */
NONSHARABLE_CLASS( CAiBTSAPPublisher ) : public CActive, 
    public MAiDeviceStatusPublisher
    {
public:

    static CAiBTSAPPublisher* NewL();

    virtual ~CAiBTSAPPublisher();

// from base class MAiDeviceStatusPublisher

    void ResumeL();
    void Subscribe( MAiContentObserver& aObserver, 
                    CHsContentPublisher& aExtension,
                    MAiPublishPrioritizer& aPrioritizer,
                    MAiPublisherBroadcaster& aBroadcaster );
    void RefreshL( TBool aClean );
    TBool RefreshL( TInt aContentId, TBool aClean );
    TBool SuspendL( TInt aContentId, TBool aClean );
    TBool RefreshContentWithPriorityL( TInt aContentId, TInt aPriority );
    
protected:

// from base class CActive

    void RunL();
    void DoCancel();
    TInt RunError( TInt aError );

private:

    CAiBTSAPPublisher();

    void ConstructL();

    void FreeEngine();

private: // data

    /** Property extension, not owned */
    CHsContentPublisher* iExtension;    
	/** Content prioritizer, not owned */
	MAiPublishPrioritizer* iPrioritizer;    
	/**  Publish broadcaster, not owned */
	MAiPublisherBroadcaster* iBroadcaster;	
    /** Publish-subscribe client used to observer BT SAP activation */
    RProperty iPubSub;
    /** Variable which tells if publisher has published previously or not */
    TBool iFirstPublish;
    /**  True if publish was successful */
    TBool iSuccess;    
    /** True if publisher content is suspended */
    TBool iSuspended;
    };

#endif // C_AIBTSAPPUBLISHER_H

// End of file
