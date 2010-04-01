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
* Description:  Owner of publishers.
*
*/



#ifndef M_AIPUBLISHERBROADCASTER_H
#define M_AIPUBLISHERBROADCASTER_H

/**
 *  @ingroup group_devicestatusplugin
 *
 *  Broadcaster for Device Status Plug-in.
 *
 *  This class should be implemented by an instance that owns Device Status 
 *  Plug-in publishers. The purpose of this class is to offer a method for 
 *  single publisher to broadcast events to all other publishers.
 *
 *  @since S60 3.2
 */
class MAiPublisherBroadcaster
    {

public:

    /**
     *  Invoke a refresh call for publishers that publish specific content 
     *  with specific priority.
     *
     *  This has the same effect as the publisher would get content update from system.
     *  Publisher publishes content if it is available and has correct priority.
     *  @param aContentId Id of the content item that is requested to republish.
     *  @param aPriority Priority of the content that needs to be refreshed.
     *  @return true if content was republished.
     */
    virtual TBool RefreshPriorizedPublishersL( TInt aContentId, TInt aPriority ) = 0;
    };


#endif // M_AIPUBLISHERBROADCASTER_H
