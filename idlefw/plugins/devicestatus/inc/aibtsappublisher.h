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


#include <e32base.h>
#include <coemain.h>
#include <RSSSettings.h>
#include <MSSSettingsObserver.h>
#include <MProfileChangeObserver.h>
#include "aidevicestatuspublisher.h"
#include "aidevicestatuscontentmodel.h"

class MAiDeviceStatusContentObserver;
class MAiPropertyExtension;

/**
 *  @ingroup group_devicestatusplugin
 *
 *  Listens Bluetooth SIM Access Profile (BT SAP) and publishes text from
 *  resource file with EAiDeviceStatusContentBTSAP content id when
 *  BT SAP is activated.
 *
 *  @since S60 3.2
 */
class CAiBTSAPPublisher : public CActive, public MAiDeviceStatusPublisher
    {
public:

    static CAiBTSAPPublisher* NewL();

    virtual ~CAiBTSAPPublisher();

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

// from base class CActive

    void RunL();
    void DoCancel();
    TInt RunError( TInt aError );

private:

    CAiBTSAPPublisher();

    void ConstructL();

    void FreeEngine();

private: // data

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
	
    /**
     * Publish-subscribe client used to observer BT SAP activation.
     */
    RProperty iPubSub;

    /**
     * Variable which tells if publisher has published previously or not.
     */
    TBool iFirstPublish;

    /**
     * True if publish was successful.
     */
    TBool iSuccess;
    };


#endif // C_AIBTSAPPUBLISHER_H
