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
* Description:  Network status publisher
*
*/


#ifndef C_AINWSPUBLISHER_H
#define C_AINWSPUBLISHER_H


#include <e32base.h>
#include "aidevicestatuspublisher.h"
#include "aidevicestatuscontentmodel.h"
#include "ainetworkinfoobserver.h"


class MAiDeviceStatusContentObserver;
class MAiPropertyExtension;
class CAiNetworkInfoListener;
class CRepository;

/**
 *  @ingroup group_devicestatusplugin
 *
 *  Listens Network status and publishes data if network selection dialog is needed.
 *
 *  @since S60 3.2
 */
class CAiNwsPublisher : public CBase, public MAiDeviceStatusPublisher,
                                public MAiNetworkInfoObserver
    {
public:

    static CAiNwsPublisher* NewL();

    virtual ~CAiNwsPublisher();

// from base class MAiDeviceStatusPublisher

    void ResumeL();

    void Subscribe( MAiContentObserver& aObserver,
                    MAiPropertyExtension& aExtension,
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


    CAiNwsPublisher();

    void ConstructL();

    void HandleStateChange( const TNWInfo& aInfo );

    void HandleNetworkFound();

    void HandleNetworkLost( const TNWNetworkSelectionSetting &aSelectionSetting  );


    /**
     * Tests if offline mode is engaged.
     *
     * @return ETrue if offline mode is currently active
     */
    TBool IsOffLineMode() const;

    /**
     * Tests if Bluetooth SAP is in connected mode.
     *
     * @return ETrue if Bluetooth SAP is active.
     */
    TBool IsBluetoothSAPConnected() const;

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
    MAiPropertyExtension* iExtension;

    /**
     * Network info listener.
     * Own.
     */
    CAiNetworkInfoListener* iListener;

    /**
     * Profile API.
     */
    CRepository* iProfileApi;


    /**
     * A true value if registered to network.
     */
    TBool iRegistered;

    };


#endif // C_AINWSPUBLISHER_H
