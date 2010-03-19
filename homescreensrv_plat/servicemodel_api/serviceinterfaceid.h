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


#ifndef SERVICEINTERFACEID_H
#define SERVICEINTERFACEID_H

#include <QString>

/**
 * @ingroup group_hsservicemodel group_service_api_candidates
 * @brief Interface id
 *
 * @lib servicemodel.lib
 *
 * @since S60 ?S60_version
 *
 */
struct HsServiceInterfaceId
{
    /**
    * Interface id.
    */
    unsigned int mInterfaceId;
    
    /**
    * Implementation id.
    */
    QString  mImplementationId;
};

#endif
