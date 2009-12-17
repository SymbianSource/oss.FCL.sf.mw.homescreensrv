/*
* Copyright (c) 2006 Nokia Corporation and/or its subsidiary(-ies).
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
* Description:  Network identity related prioritizer.
*
*/



#ifndef M_AIPUBLISHPRIORITIZER_H
#define M_AIPUBLISHPRIORITIZER_H

#include "aicontentobserver.h"

class MAiPublisherBroadcaster;


/**
 *  @ingroup group_devicestatusplugin
 *
 *  Network identity related prioritizer.
 *
 *  This class is used as proxy between publishers which publish data that
 *  needs to be priorised and content observers. It keeps record of published
 *  data and decides if new published data has high enough priority to go 
 *  through to content observer.
 *
 *  @since S60 3.2
 */
class MAiPublishPrioritizer
    {

public:

    /**
     * Invoked by publisher that publishes prioritized data.
     * Data is published only if aPriority is same or higher than
     * current priority.
     *
     * @param  aBroadcaster - Publisher broadcaster implementation.
     * @param  aContent - identification of content selector, MUST correspond
     *         single content selector supported by plug-in. The framework
     *         utilizes the selector id to match for cid and MIME type.
     * @param  aResource - identification of content reference, MUST correspond
     *         single content reference supported by plug-in. The framework
     *         utilizes the reference if to match for cid and MIME type of the
     *         content supplied with in UI definition.
     * @param  aPriority - priority of published data.
     */
    virtual void TryToPublishL( MAiPublisherBroadcaster& aBroadcaster, 
                                TInt aContent, 
                                TInt aResource, 
                                TInt aPriority ) = 0;

    /**
     * Invoked by publisher that publishes prioritized data.
     * Data is published only if aPriority is same or higher than
     * current priority.
     *
     * @param  aBroadcaster - Publisher broadcaster implementation.
     * @param  aContent - identification of content selector, MUST correspond
     *         single content selector supported by plug-in. The framework
     *         utilizes the selector id to match for cid and MIME type.
     * @param  aText - Textual content in UNICODE.
     * @param  aPriority - priority of published data.
     */
    virtual void TryToPublishL( MAiPublisherBroadcaster& aBroadcaster, 
                                TInt aContent, 
                                const TDesC16& aText, 
                                TInt aPriority ) = 0;

    /**
     * Invoked by publisher that publishes prioritized data.
     * Data is published only if aPriority is same or higher than
     * current priority.
     *
     * @param  aBroadcaster - Publisher broadcaster implementation.
     * @param  aContent - identification of content selector, MUST correspond
     *         single content selector supported by plug-in. The framework
     *         utilizes the selector id to match for cid and MIME type.
     * @param  aBuf - instance of content.
     * @param  aPriority - priority of published data.
     */ 
    virtual void TryToPublishL( MAiPublisherBroadcaster& aBroadcaster, 
                                TInt aContent, 
                                const TDesC8& aBuf, 
                                TInt aPriority ) = 0;
                                
    /**
     * Invoked by publisher that wants to clean previously published data.
     * Data is cleaned only if aPriority is same than current priority.
     *
     * @param  aBroadcaster - Publisher broadcaster implementation.
     * @param  aContent - identification of content selector, MUST correspond
     *         single content selector supported by plug-in. The framework
     *         utilizes the selector id to match for cid and MIME type.
     * @param  aPriority - priority of published data.
     */
    virtual void TryToCleanL( MAiPublisherBroadcaster& aBroadcaster, 
                                TInt aContent, 
                                TInt aPriority ) = 0;

    };


#endif // M_AIPUBLISHPRIORITIZER_H
