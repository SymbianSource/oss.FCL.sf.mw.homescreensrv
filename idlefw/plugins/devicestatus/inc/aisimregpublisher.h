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
* Description:  SIM registration status publisher
*
*/


#ifndef C_AISIMREGFAILPUBLISHER_H
#define C_AISIMREGFAILPUBLISHER_H

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
 *  Listens SIM Registration status and publishes data if SIM registration failed.
 *
 *  @since S60 3.2
 */
NONSHARABLE_CLASS( CAiSimRegPublisher ) : public CBase, 
    public MAiDeviceStatusPublisher,
    public MAiNetworkInfoObserver
    {
public:

    static CAiSimRegPublisher* NewL();

    virtual ~CAiSimRegPublisher();

// from base class MAiDeviceStatusPublisher

    void ResumeL();
    void Subscribe( MAiContentObserver& aObserver, 
                    CHsContentPublisher& aExtension,
                    MAiPublishPrioritizer& aPrioritizer,
                    MAiPublisherBroadcaster& aBroadcaster );
    void RefreshL( TBool aClean );
    TBool RefreshL( TInt aContentId, TBool aClean );

protected:

//from base class MAiNetworkInfoObserver

    void HandleNetworkInfoChange( const MNWMessageObserver::TNWMessages& aMessage, 
    							  const TNWInfo& aInfo, 
    							  const TBool aShowOpInd );


private:

    CAiSimRegPublisher();

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


#endif // C_AISIMREGFAILPUBLISHER_H
