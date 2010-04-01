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
* Description:  Interface for device status publisher.
*
*/


#ifndef M_AIDEVICESTATUSPUBLISHER_H
#define M_AIDEVICESTATUSPUBLISHER_H

#include <e32std.h>
#include "aicontentobserver.h"

class MAiPublishPrioritizer;
class MAiPublisherBroadcaster;

/**
 *  @ingroup group_devicestatusplugin
 *
 *  Interface for device status publisher.
 *
 *  @since S60 3.2
 */
class MAiDeviceStatusPublisher
    {

public:
    /**
     * Virtual destructor.
     *
     * Publisher can be destroyed through this interface.
     */
    virtual ~MAiDeviceStatusPublisher() {};

    /**
     * Subscribe content observer.
     *
     * @param aObserver is reference to content observer.
     * @param aExtension is reference to property extension.
     * @param aPrioritizer is reference to publish prioritizer.
     * @param aBroadcaster is reference for publisher broadcaster.
     */
    virtual void Subscribe( MAiContentObserver& aObserver, 
                            MAiPropertyExtension& aExtension,
                            MAiPublishPrioritizer& aPrioritizer,
                            MAiPublisherBroadcaster& aBroadcaster ) = 0;

    /**
     * Resume publisher.
     *
     * Publisher publishes content if it is available.
     */
    virtual void ResumeL() = 0;

    /**
     * Refresh publisher.
     *
     * This has the same effect as the publisher would get content update from system.
     * Publisher publishes content if it is available.
     * @param aClean ETrue if current content needs to be cleaned.
     */
    virtual void RefreshL( TBool aClean ) = 0;
    
    /**
     * Refresh specific content.
     *
     * This has the same effect as the publisher would get content update from system.
     * Publisher publishes content if it is available.
     * @param aContentId Id of the content item that is requested to republish.
     * @param aClean ETrue if current content needs to be cleaned.
     * @return true if content was republished.
     */
    virtual TBool RefreshL( TInt /*aContentId*/, TBool /*aClean*/ ) { return EFalse; }

    /**
     * Refresh specific content with specific priority.
     *
     * This has the same effect as the publisher would get content update from system.
     * Publisher publishes content if it is available and has correct priority.
     * @param aContentId Id of the content item that is requested to republish.
     * @param aPriority Priority of the content that needs to be refreshed.
     * @return true if content was republished.
     */
    virtual TBool RefreshContentWithPriorityL( TInt /*aContentId*/, TInt /*aPriority*/ ) { return EFalse; }
    };


#endif // M_AIDEVICESTATUSPUBLISHER_H
