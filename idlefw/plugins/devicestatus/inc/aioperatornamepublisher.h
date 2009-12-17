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
* Description:  Operator/Service provider name publisher.
*
*/


#ifndef C_AIOPERATORPROVIDERNAMEPUBLISHER_H
#define C_AIOPERATORPROVIDERNAMEPUBLISHER_H


#include <e32base.h>
#include <AknUtils.h>
#include "aidevicestatuspublisher.h"
#include "ainetworkinfoobserver.h"

class CAiNetworkInfoListener;
class MAiDeviceStatusContentObserver;
class MAiPropertyExtension;

/**
 *  @ingroup group_devicestatusplugin
 *
 *  Operator/Service provider name publisher.
 *
 *  Listens network info changes and publishes operator name
 *  if it is received.
 *
 *  @since S60 3.2
 */
class CAiOperatorNamePublisher : public CBase, public MAiDeviceStatusPublisher,
                               public MAiNetworkInfoObserver
    {
public:

    static CAiOperatorNamePublisher* NewL();

    virtual ~CAiOperatorNamePublisher();

//from base class MAiDeviceStatusPublisher

    void ResumeL();
    void Subscribe( MAiContentObserver& aObserver, 
                    MAiPropertyExtension& aExtension,
                    MAiPublishPrioritizer& aPrioritizer,
                    MAiPublisherBroadcaster& aBroadcaster );
    void RefreshL( TBool aClean );
    TBool RefreshL( TInt aContentId, TBool aClean );
    TBool RefreshContentWithPriorityL( TInt aContentId, TInt aPriority );


protected:

//from base class MAiNetworkInfoObserver

    /**
     * From MAiNetworkInfoObserver.
     * Called when network info changes.
     */
    void HandleNetworkInfoChange( const MNWMessageObserver::TNWMessages& aMessage, 
    							  const TNWInfo& aInfo, 
    							  const TBool aShowOpInd );


private:

    CAiOperatorNamePublisher();

    void ConstructL();

    /**
     * Check SPN status from PLMNField. Also check if current country and network
     * codes are in PLMNField.
     */
    void CAiOperatorNamePublisher::CheckServiceProviderDisplayListStatus(
        TInt aMCC,
        TInt aMNC,
        TBool& aShowSPN,
        TBool& aIsInSPDI ) const;

    /**
     * Check if current network is ready. Returns ETrue if all needed messages are
     * received from network info listener.
     */
    TBool CurrentNetworkOk() const;

    /**
     * Returns ETrue if phone is currently roaming.
     */
    TBool IsRoaming() const;

    /**
     * If operator indicator needs to be animated, start delayed clean operation.
     * Operator indicator will be replaced by plmn indicator after clean operation.
     */
    void StartDelayedPLMNOperation();

    /**
     * Called by CPeriodic when timer expires.
     */
    static TInt CleanAndShowPLMNOperationCallback( TAny* aPtr );

    /**
     * Do delayed clean operation.
     */
    void DoCleanOperationL();
    
    /**
     * Show network indicator
     * @param aTryToPublish 
     * ETrue: Sets operator name, priority and tries to publish operator name. 
     * EFalse: Sets operator name, priority and not to try to publish it.
     */
    void ShowNetworkIdentityNameL( TBool aTryToPublish );
    
    /**
     * Check priority of operator name
     */	
    TBool OperatorNamePriority( TInt& aPriority );
    
    /**
     * Construct PLMN and SPN indicators to same string
     */
    HBufC* ConstructOperatorNameStringL( const TDesC& aServiceProviderName );
    
    /**
     * Check status of keylock
     */
    TBool IsKeyLockEnabled();

private: // data

    /**
     * Network info listener.
     * Own.
     */
    CAiNetworkInfoListener* iListener;

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
     * Used to do delayed clean operation.
     * Own.
     */
    CPeriodic* iPeriodic;
    
    /**
     * True if publish was successful.
     */
    TBool iSuccess;
    
    /**
     * Operator name priority
     */
    TInt iPriority;
    
    /**
     * Show operator indicator.
     */
    TBool iShowOpInd;
    
    /**
     * Network identity name
     */
    TPtrC iNetworkIdentityName;
    };


#endif // C_AIOPERATORPROVIDERNAMEPUBLISHER_H
