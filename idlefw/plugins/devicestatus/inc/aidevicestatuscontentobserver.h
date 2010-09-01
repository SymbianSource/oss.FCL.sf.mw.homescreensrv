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
* Description:  Device status content publisher
*
*/


#ifndef M_AIDEVICESTATUSCONTENTOBSERVER_H
#define M_AIDEVICESTATUSCONTENTOBSERVER_H


class MAiDeviceStatusPublisher;

/**
 *  @ingroup group_devicestatusplugin
 *
 *  Device status content publisher.
 *
 *  All of the publishers uses this interface to publish content.
 *
 *  @since S60 3.2
 */
class MAiDeviceStatusContentObserver
    {

protected:
    /**
     * Virtual destructor.
     *
     * This cannot be used to destroy observer object.
     */
     virtual ~MAiDeviceStatusContentObserver() {};

public:

    /**
     * Publish unicode text.
     *
     * @since S60 3.2
     * @param aPublisher is reference to publisher which is publishing.
     * @param aContent is content id.
     * @param aText is published text.
     * @param aPriority is priority of the content.
     * @return KErrNone if publish is successful, otherwise system wide error code.
     */
    virtual TInt Publish( MAiDeviceStatusPublisher& aPublisher, TInt aContent,
                          const TDesC16& aText, TInt aPriority ) = 0;

    /**
     * Publish data buffer.
     *
     * @since S60 3.2
     * @param aPublisher is reference to publisher which is publishing.
     * @param aContent is content id.
     * @param aBuf is data buffer.
     * @param aPriority is priority of the content.
     * @return KErrNone if publish is successful, otherwise system wide error code.
     */
    virtual TInt Publish( MAiDeviceStatusPublisher& aPublisher, TInt aContent,
                          const TDesC8& aBuf, TInt aPriority ) = 0;

    /**
     * Publish resource.
     *
     * @since S60 3.2
     * @param aPublisher is reference to publisher which is publishing.
     * @param aContent is content id.
     * @param aResource is resource id.
     * @param aPriority is priority of the content.
     * @return KErrNone if publish is successful, otherwise system wide error code.
     */
    virtual TInt Publish( MAiDeviceStatusPublisher& aPublisher, TInt aContent,
                          TInt aResource, TInt aPriority ) = 0;

    /**
     * Clean content.
     *
     * @since S60 3.2
     * @param aPublisher is reference to publisher which is publishing.
     * @param aContent is content id.
     * @param aPriority is priority of the content.
     * @return KErrNone if publish is successful, otherwise system wide error code.
     */
    virtual TInt Clean( MAiDeviceStatusPublisher& aPublisher, TInt aContent, TInt aPriority ) = 0;

    };


#endif // M_AIDEVICESTATUSCONTENTOBSERVER_H
