/*
* Copyright (c) 2006-2006 Nokia Corporation and/or its subsidiary(-ies).
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
* Description:  Engine class for Device Status Plug-in
*
*/


#ifndef AIDEVICESTATUSPLUGINENGINE_H
#define AIDEVICESTATUSPLUGINENGINE_H

// System includes
#include <e32base.h>

// User includes
#include <aisystemuids.hrh>
#include "aipublisherbroadcaster.h"
#include "aidevicestatuspublisher.h"

// Forward declarations
class CHsContentPublisher;
class MAiContentObserver;
class MAiPublishPrioritizer;

// Constants
//device status plugin UI
const TInt KImplUidDevStaPlugin = AI_UID_ECOM_IMPLEMENTATION_CONTENTPUBLISHER_DEVSTAPLUGIN;


/**
 *  @ingroup group_devicestatusplugin
 *
 *  Engine class for Device Status Plug-in
 *
 *  This class works as an engine for Device Status Plug-in.
 *  It owns the publishers and communicates between them and
 *	the subscribers.
 *
 *  @since S60 v3.2
 */
NONSHARABLE_CLASS( CAiDeviceStatusPluginEngine ) : public CBase, 
    public MAiPublisherBroadcaster
    {
public:
    // constructor and destructor
    
    static CAiDeviceStatusPluginEngine* NewL( MAiContentObserver& aObserver, 
        CHsContentPublisher& aExtension, MAiPublishPrioritizer& aPrioritizer );
                                                
    virtual ~CAiDeviceStatusPluginEngine();

public:
    // new functions

    /**
     * Refreshes all active publishers.
     */
    void RefreshActivePublishersL( TBool aClean );    

    /**
     * Resumes all publishers.
     */
    void ResumePublishersL();
    
    /**
     * Refreshes all publishers.
     * @param aClean Indicates if publishers should clean their content before
     *                  re-publish.
     */    
    void RefreshPublishersL( TBool aClean );

    /**
     * Refreshes specific publishers.
     * @param aContentId Indicates which publishers should publish their 
     *                  content.
     * @param aClean Indicates if publishers should clean their content before
     *                  re-publish.
     * @return ETrue if publisher informed that publish was successful.
     */    
    TBool RefreshPublishersL( TInt aContentId, TBool aClean );

    /**
     * Suspends specific publishers.
     * @param aContentId Indicates which publishers should suspend their 
     *                  content.
     * @param aClean Indicates if publishers should clean their content before
     *                  suspend.
     * @return ETrue if publisher informed that publish was successful.
     */    
    TBool SuspendPublishersL( TInt aContentId, TBool aClean );
    

public: 
    // from MAiPublisherBroadcaster
    
    TBool RefreshPriorizedPublishersL( TInt aContentId, TInt aPriority );


private:
    // constructors

    CAiDeviceStatusPluginEngine( MAiContentObserver& aObserver, 
        CHsContentPublisher& aExtension, MAiPublishPrioritizer& aPrioritizer );
                                    
    void ConstructL();

private:
    // new functions
    
     /**
      * Add Device Status publisher.
      */
    void AddPublisherL( MAiDeviceStatusPublisher* aPublisher );

private: 
    // data

    /**
     * Array of publishers.
     * Own.
     */
    RPointerArray<MAiDeviceStatusPublisher> iPublishers;

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
    };

#endif // AIDEVICESTATUSPLUGINENGINE_H

// End of file
