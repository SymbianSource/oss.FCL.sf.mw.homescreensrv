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
* Description:  Date publisher
*
*/


#ifndef C_AIDATEPUBLISHER_H
#define C_AIDATEPUBLISHER_H

// System includes
#include <e32base.h>
#include <coemain.h>

// User includes
#include "aidevicestatuspublisher.h"
#include "aidevicestatuscontentmodel.h"

// Forward declarations
class MAiDeviceStatusContentObserver;
class CHsContentPublisher;
class CEnvironmentChangeNotifier;

/**
 *  @ingroup group_devicestatusplugin
 *
 *  Date publisher.
 *
 *  Publishes current date formatted according to current locale if profile
 *  if either general or offline.
 *
 *  @since S60 3.2
 */
NONSHARABLE_CLASS( CAiDatePublisher ) : public CBase, 
    public MAiDeviceStatusPublisher
    {
public:

    static CAiDatePublisher* NewL();

    virtual ~CAiDatePublisher();

protected:

// from base class MAiDeviceStatusPublisher

    void ResumeL();
    void Subscribe( MAiContentObserver& aObserver, 
                    CHsContentPublisher& aExtension,
                    MAiPublishPrioritizer& aPrioritizer,
                    MAiPublisherBroadcaster& aBroadcaster );
    void RefreshL( TBool aClean );
    TBool RefreshL( TInt aContentId, TBool aClean );

private:

    CAiDatePublisher();

    void ConstructL();

	/// Re-publish date
	void RefreshDateL();
	
	/// Create day name string
	HBufC* GetDayNameStringLC( TDay aDay, CCoeEnv& aCoeEnv );

	/// Create date string
    HBufC* ConstructDateStringL();
    
    /// Callback when enviroment changed.
    static TInt HandleCallBackL( TAny *aPtr );



protected: // data

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
	 * Content prioritizer.
	 * Not own.
	 */
	MAiPublishPrioritizer* iPrioritizer;
   
    /**
	 * Environment notifier.
	 * own.
	 */
	CEnvironmentChangeNotifier* iEnvNotifier;
	
	/**
	 * Constructed date text.
	 * Own.
	 */
	HBufC* iDateText;
	
    };


#endif // C_AIDATEPUBLISHER_H
