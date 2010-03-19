/*
* Copyright (c) 2009 Nokia Corporation and/or its subsidiary(-ies).
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
* Description:  HsService provider interface.
*
*/


#ifndef HSISERVICEPROVIDER_H
#define HSISERVICEPROVIDER_H

#include <QtPlugin>
#include <QList>
#include <QString>

/**
 * @ingroup group_hsservicemodel group_service_api_candidates
 * @brief Represents a home screen service that can be created. 
 * 
 * Home screen services are located in service providers. Each provider
 * exposes its contained services as service tokens. A token has all
 * the needed information for service selection and creation.
 *
 * @lib ?library
 * @since S60 ?S60_version
 */
class HsServiceToken
{

public:

    /**
     * The name of the library/plugin that contains the service.
     */
    QString mLibrary;

    /**
     * Uniquely identifies the service.
     */
    QString mUri;

};


class IHsPlatformServiceAdapter;

/**
 * @ingroup group_hsservicemodel group_service_api_candidates
 * @brief Defines a mechanism for retrieving a home screen service.
 * 
 * Interface that defines a mechanism for retrieving a home screen
 * service. Each home screen service provider implements this interface. 
 *
 * @lib ?library
 * @since S60 ?S60_version
 */
class IHsServiceProvider
{

public:

    /**
     * Destructor.
     * @since S60 ?S60_version.
     */
    virtual ~IHsServiceProvider() {}

public:

    /**
     * Returns contained services as a list of service tokens.
     * @since S60 ?S60_version.
     * @return Contained services as tokens.
     */
    virtual QList<HsServiceToken> services() = 0;

    /**
     * Creates and returns a service based on the given token.
     * @since S60 ?S60_version.
     * @param aToken Identifies the service to be created.
     * @return The created service or null in failure cases.
     */
    virtual IHsPlatformServiceAdapter* createService(const HsServiceToken& aToken) = 0;

};

Q_DECLARE_INTERFACE(IHsServiceProvider, "com.nokia.homescreen.iserviceprovider/1.0")

#endif
