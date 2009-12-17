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


#include "aipublisherfactory.h"

// Publishers
#include "aiprofilepublisher.h"
#include "aidatepublisher.h"
#include "aioperatorlogopublisher.h"
#include "aioperatornamepublisher.h"
#include "aibtsappublisher.h"
#include "aimcnpublisher.h"
#include "aicugpublisher.h"
#include "aivhzpublisher.h"
#include "ainwspublisher.h"
#include "aisimregpublisher.h"
#include "aicugmcnpublisher.h"

template<class T>
class TPublisherFactory
    {
    public:
    MAiDeviceStatusPublisher* CreatePublisherL()
        {
		//Create publisher..
        MAiDeviceStatusPublisher* publisher = NULL;
        TRAPD( err, publisher = T::NewL() );

		//If publisher creation fails with error code KErrNotSupported,
		//return NULL.
        if( err == KErrNotSupported )
            {
            err = KErrNone;
            }

        User::LeaveIfError( err );

        return publisher;
        }
    };


MAiDeviceStatusPublisher* AiPublisherFactory::CreateProfilePublisherL()
    {
    TPublisherFactory<CAiProfilePublisher> factory;
    return factory.CreatePublisherL();
    }


MAiDeviceStatusPublisher* AiPublisherFactory::CreateDatePublisherL()
    {
    TPublisherFactory<CAiDatePublisher> factory;
    return factory.CreatePublisherL();
    }


MAiDeviceStatusPublisher* AiPublisherFactory::CreateOperatorLogoPublisherL()
    {
    TPublisherFactory<CAiOperatorLogoPublisher> factory;
    return factory.CreatePublisherL();
    }


MAiDeviceStatusPublisher* AiPublisherFactory::CreateOperatorNamePublisherL()
    {
    TPublisherFactory<CAiOperatorNamePublisher> factory;
    return factory.CreatePublisherL();
    }


MAiDeviceStatusPublisher* AiPublisherFactory::CreateBTSAPPublisherL()
    {
    TPublisherFactory<CAiBTSAPPublisher> factory;
    return factory.CreatePublisherL();
    }


MAiDeviceStatusPublisher* AiPublisherFactory::CreateMCNPublisherL()
    {
    TPublisherFactory<CAiMCNPublisher> factory;
    return factory.CreatePublisherL();
    }


MAiDeviceStatusPublisher* AiPublisherFactory::CreateCUGPublisherL()
    {
    TPublisherFactory<CAiCUGPublisher> factory;
    return factory.CreatePublisherL();
    }


MAiDeviceStatusPublisher* AiPublisherFactory::CreateSimRegPublisherL()
    {
    TPublisherFactory<CAiSimRegPublisher> factory;
    return factory.CreatePublisherL();
    }


MAiDeviceStatusPublisher* AiPublisherFactory::CreateNWSPublisherL()
    {
    TPublisherFactory<CAiNwsPublisher> factory;
    return factory.CreatePublisherL();
    }


MAiDeviceStatusPublisher* AiPublisherFactory::CreateVHZPublisherL()
    {
    TPublisherFactory<CAiVHZPublisher> factory;
    return factory.CreatePublisherL();
    }

MAiDeviceStatusPublisher* AiPublisherFactory::CreateCUGMCNPublisherL()
    {
    TPublisherFactory<CAiCUGMCNPublisher> factory;
    return factory.CreatePublisherL();
    }
