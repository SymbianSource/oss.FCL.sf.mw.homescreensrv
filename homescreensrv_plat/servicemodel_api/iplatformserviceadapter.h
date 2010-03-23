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
* Description: 
*
*/


#ifndef IPLATFORMHSSERVICEMODEL_H
#define IPLATFORMHSSERVICEMODEL_H

#include <QString>
#include "serviceinterfaceid.h"

class IHsServiceBase;
class HsService;

/**
 * @ingroup group_hsservicemodel group_service_api_candidates
 * @brief Pure virtual class to create requested service.
 * All the service plugines need to implement this class.
 *
 */
class IHsPlatformServiceAdapter
{

public:

    /**
     * Destructor.
     * @since S60 ?S60_version.
     */
	virtual ~IHsPlatformServiceAdapter() {}

public:

    /**
     *
     * @param aService for example 'HsService.Messaging'
     * @param aInterface for example 'IMessaging'
     * @since S60 ?S60_version.
     * @return pointer to created service. NULL if service not found.
     */
    virtual HsService* getService(const QString &aService, const QString &aInterface) = 0;

    /**
     * @param aInterfaceId
     * @since S60 ?S60_version.
     * @return pointer to created service. NULL if service not found.
     */
    virtual IHsServiceBase* getService(const HsServiceInterfaceId &aInterfaceId) = 0;

};

#endif
