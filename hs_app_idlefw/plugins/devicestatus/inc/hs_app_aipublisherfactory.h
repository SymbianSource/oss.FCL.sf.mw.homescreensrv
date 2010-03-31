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
* Description:  Static factory class used to create publishers.
*
*/


#ifndef AIPUBLISHERFACTORY_H
#define AIPUBLISHERFACTORY_H

class MAiDeviceStatusPublisher;

/**
 *  @ingroup group_devicestatusplugin
 *
 *  Static factory class used to create publishers.
 *
 *  @since S60 3.2
 */
class AiPublisherFactory
    {

public:

    /**
     * Create profile publisher.
     *
     * @since S60 3.2
     * @return pointer to publisher or NULL if publisher is not supported
     *         by platform.
     */
    static MAiDeviceStatusPublisher* CreateProfilePublisherL();

    /**
     * Create date profile publisher.
     *
     * @since S60 3.2
     * @return pointer to publisher or NULL if publisher is not supported
     *         by platform.
     */
    static MAiDeviceStatusPublisher* CreateDatePublisherL();

    /**
     * Create operator logo publisher.
     *
     * @since S60 3.2
     * @return pointer to publisher or NULL if publisher is not supported
     *         by platform.
     */
    static MAiDeviceStatusPublisher* CreateOperatorLogoPublisherL();

    /**
     * Create operator name publisher.
     *
     * @since S60 3.2
     * @return pointer to publisher or NULL if publisher is not supported
     *         by platform.
     */
    static MAiDeviceStatusPublisher* CreateOperatorNamePublisherL();

    /**
     * Create BT SAP publisher.
     *
     * @since S60 3.2
     * @return pointer to publisher or NULL if publisher is not supported
     *         by platform.
     */
    static MAiDeviceStatusPublisher* CreateBTSAPPublisherL();

    /**
     * Create MCN publisher.
     *
     * @since S60 3.2
     * @return pointer to publisher or NULL if publisher is not supported
     *         by platform.
     */
    static MAiDeviceStatusPublisher* CreateMCNPublisherL();

    /**
     * Create CUG publisher.
     *
     * @since S60 3.2
     * @return pointer to publisher or NULL if publisher is not supported
     *         by platform.
     */
    static MAiDeviceStatusPublisher* CreateCUGPublisherL();

    /**
     * Create SIM registration status publisher.
     *
     * @since S60 3.2
     * @return pointer to publisher or NULL if publisher is not supported
     *         by platform.
     */
    static MAiDeviceStatusPublisher* CreateSimRegPublisherL();

    /**
     * Create Network status publisher.
     *
     * @since S60 3.2
     * @return pointer to publisher or NULL if publisher is not supported
     *         by platform.
     */
    static MAiDeviceStatusPublisher* CreateNWSPublisherL();

    /**
     * Create VHZ publisher.
     *
     * @since S60 3.2
     * @return pointer to publisher or NULL if publisher is not supported
     *         by platform.
     */
    static MAiDeviceStatusPublisher* CreateVHZPublisherL();
    
    /**
     * Create CUGMCN publisher.
     *
     * @since S60 3.2
     * @return pointer to publisher or NULL if publisher is not supported
     *         by platform.
     */
    static MAiDeviceStatusPublisher* CreateCUGMCNPublisherL();
    };


#endif // AIPUBLISHERFACTORY_H
