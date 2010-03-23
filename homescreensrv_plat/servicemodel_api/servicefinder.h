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
* Description:  HsService Finder
*
*/


#ifndef SERVICEFINDER_H
#define SERVICEFINDER_H

#include <QObject>
#include "serviceinterfaceid.h"
#include "hsiserviceprovider.h"
#include "servicemodel_global.h"

class IHsServiceBase;
class HsServiceFinderPrivate;
class HsService;

/**
 * @ingroup group_hsservicemodel group_service_api_candidates
 * @brief Helper class to search and create services.
 * HsService adapter is the component to find different platform service implementations
 * and offer interface to use services.
 */
class HSSERVICEMODEL_EXPORT HsServiceFinder : public QObject
{
	Q_OBJECT

public:

    /**
     * Constructor.
     * @since S60 ?S60_version.
     * @param aPluginManifestDirectory Directory that contains plugin manifests.
     * @param aPluginDirectory Directory that contains plugin binaries.
     * @param aParent Parent object.
     */
    HsServiceFinder(const QString& aPluginManifestDirectory,
                  const QString& aPluginDirectory,
                  QObject* aParent = 0);

  
    /**
    * Destructor    
    */
    virtual ~HsServiceFinder();

public:

    /**
     * Get list of all services in the device implemented according the service architecture.
     * @return list of services (empty list if no services in the device).
     */
    QList<HsServiceToken> getServiceList();

    /**
     * Get list of all services in the device implemented according the service architecture.
     * @param aService for example 'BackupService'
     * @return list of matched services (empty list if no services in the device).
     * @code
     * if (!mServiceFinder)
     *     {
     *     mServiceFinder = new HsServiceFinder("c:/hsplugins","c:/hsplugins",this);
     *     }
     * IBackupService* service = 0;
     * QList<HsServiceToken> serviceList = mServiceFinder->getServiceList(IBackupService::type().mImplementationId );
     * if ( serviceList.count() )
     *     {
     *     // use first found service
     *     service = mServiceFinder->getService<IBackupService>(IBackupService::type(), serviceList.at(0));
     *     }
     *
     * @endcode
     */
    QList<HsServiceToken> getServiceList(const QString &aService);

    /**
     * Get service. This method returns the service as a QObject.
     * @param aService for example 'HsService.Messaging'
     * @param aInterface for example 'IMessaging'
     * @return pointer to HsService object. Ownership is NOT transferred to caller.
     * @see releaseService
     * @code
     * if (!mServiceFinder)
     *     {
     *     mServiceFinder = new HsServiceFinder("c:/hsplugins","c:/hsplugins",this);
     *     }
     * HsService* servicePtr = mServiceFinder->getService("HsService.Messaging", "IMessaging");
     * @endcode
     */
    HsService *getService(const QString &aService, const QString &aInterface);

    /**
     * Get service. This method returns the service as an abstract interface.
     * @param aInterfaceId service interface identifier
     * @param aPluginName which plugin implements this service, if empty, then first found interface is used.
     * @return pointer to service interface object. Ownership is NOT transferred to caller.
     *
     * Example in source code:
     * @code
     * if (!mServiceFinder)
     *     {
     *     mServiceFinder = new HsServiceFinder("c:/hsplugins","c:/hsplugins",this);
     *     }
     * IBackupService* service = 0;
     * service = mServiceFinder->getService<IBackupService>(IBackupService::type(), "backupservice");
     * if ( service )
     *     {
     *     service->backupPage();
     *     }
     * @endcode
     * where IBackupService is defined in header file:
     * @code
     * static const HsServiceInterfaceId Identifier=
     *     {
     *     0,"persistanceservice"
     *     };
     *
     * class IBackupService: public IHsServiceBase
     * {
     * public:
     * static inline const HsServiceInterfaceId& type()
     *     {
     *     return Identifier;
     *     }
     * public:
     * virtual void backupPage() = 0;
     * virtual IHsServiceBase* resolveService(const HsServiceInterfaceId& aType) = 0;
     * ... other possible API functions
     * @endcode
     * @see more details in example backupservice implementation, IBackupService, PersistanceService...
     */
    template<class T> T *getService(const HsServiceInterfaceId &aInterfaceId, const QString &aPluginName)
    {
        return static_cast<T*>(getService(aInterfaceId, aPluginName));
    }

    /**
     * Get service. This method returns the service as an abstract interface.
     * @param aInterfaceId service interface identifier
     * @param aToken which plugin implements this service
     * @return pointer to service interface object. Ownership is NOT transferred to caller.
     *
     * @code
     * if (!mServiceFinder)
     *     {
     *     mServiceFinder = new HsServiceFinder("c:/hsplugins","c:/hsplugins",this);
     *     }
     * IBackupService* service = 0;
     * QList<HsServiceToken> serviceList = mServiceFinder->getServiceList(IBackupService::type().mImplementationId);
     * if ( serviceList.count() )
     *     {
     *     // use first found service
     *     service = mServiceFinder->getService<IBackupService>(IBackupService::type(), serviceList.at(0));
     *     }
     * if ( service )
     *     {
     *     service->backupPage();
     *     }
     * @endcode
     */
    template<class T> T *getService(const HsServiceInterfaceId &aInterfaceId, const HsServiceToken &aToken)
    {
        return static_cast<T*>(getService(aInterfaceId, aToken));
    }

    /**
     * Get service. This method returns the service as an Base interface of the all services.
     * @param aInterfaceId service interface identifier
     * @param aPluginName which plugin implements this service
     * @return pointer to service interface object. Ownership is NOT transferred to caller.
     */
    IHsServiceBase *getService(const HsServiceInterfaceId &aInterfaceId, const QString &aPluginName);

    /**
     * Get service. This method returns the service as an Base interface of the all services.
     * @param aInterfaceId service interface identifier
     * @param aToken which plugin implements this service
     * @return pointer to service interface object. Ownership is NOT transferred to caller.
     */
    IHsServiceBase *getService(const HsServiceInterfaceId &aInterfaceId, const HsServiceToken &aToken);

    /**
     * Release service
     * @param aService service to be released and deleted. HsService is QObject based service.
     */
    void releaseService(HsService *aService);

    /**
     * Cancels asynchronous service requests.
     * @param aService QObject based service.
     */
    void cancel(HsService *aService);

private:

    /**
     * The private implementation.
     */
    HsServiceFinderPrivate *mServiceFinderPrivate;

};

#endif
