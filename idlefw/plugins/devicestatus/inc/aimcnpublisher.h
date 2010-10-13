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
* Description:  Micro Cellular Network (MCN) publisher.
*
*/


#ifndef C_AIMCNPUBLISHER_H
#define C_AIMCNPUBLISHER_H

// System includes
#include <e32base.h>
#include <aidevicestatuscontentmodel.h>

// User includes
#include "aidevicestatuspublisher.h"
#include "ainetworkinfoobserver.h"

// Forward declarations
class MAiDeviceStatusContentObserver;
class CHsContentPublisher;
class CAiNetworkInfoListener;

/**
 *  @ingroup group_devicestatusplugin
 *
 *  Micro Cellular Network (MCN) publisher.
 *
 *  This publisher listens network info changes and publishes
 *  received MCN messages.
 *
 *  @since S60 3.2
 */
NONSHARABLE_CLASS( CAiMCNPublisher ) : public CBase, 
    public MAiDeviceStatusPublisher,
    public MAiNetworkInfoObserver
    {
public:

    static CAiMCNPublisher* NewL();

    virtual ~CAiMCNPublisher();

protected:

// from base class MAiDeviceStatusPublisher

    void ResumeL();
    void Subscribe( MAiContentObserver& aObserver, 
                    CHsContentPublisher& aExtension,
                    MAiPublishPrioritizer& aPrioritizer,
                    MAiPublisherBroadcaster& aBroadcaster );
    void RefreshL( TBool aClean );
    TBool RefreshL( TInt aContentId, TBool aClean );


//from base class MAiNetworkInfoObserver

    void HandleNetworkInfoChange( const MNWMessageObserver::TNWMessages& aMessage, 
    							  const TNWInfo& aInfo, 
    							  const TBool aShowOpInd );


private:

    CAiMCNPublisher();

    void ConstructL();

private: // data


    /**
     * Content observer.
     * Not own.
     */
    MAiContentObserver* iContentObserver;

    /**
     * Property extension.
     * Not own.
     */
    CHsContentPublisher* iExtension;

    /**
     * Network info listener.
     * Own.
     */
    CAiNetworkInfoListener* iListener;
    };


#endif // C_AIMCNPUBLISHER_H
