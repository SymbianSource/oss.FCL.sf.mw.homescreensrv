/*
 * Copyright (c) 2010 Nokia Corporation and/or its subsidiary(-ies).
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
 * Description:  ?Description
 *
 */
#include <QtGlobal>
#include <QMetaType>
#include <QStringList>


#include "casoftwareregistry.h"
#include "casoftwareregistry_p.h"


/*
 Constructor
 \param servicePublic Pointer to object of the public class.
 */
CaSoftwareRegistryPrivate::CaSoftwareRegistryPrivate(CaSoftwareRegistry *servicePublic) :
    m_q(servicePublic)
{
}

/*
 Destructor.
 */
CaSoftwareRegistryPrivate::~CaSoftwareRegistryPrivate()
{
}

/*!
 Provides details needed for uninstalling process of Java applications
 (windows stub).
 \param[in] componentId component id of an application to be uninstalled.
 \param[out] componentName a name of the component.
 \param[out] applicationsUids a list of uids of applications in the package
      of the given component id.
 \param[out] confirmationMessage optional deletion confirmation message,
      null string means the lack of the message.
 \retval true if there is no error.
 */
bool CaSoftwareRegistryPrivate::getUninstallDetails(int componentId,
    QString &componentName,
    QStringList &applicationsUids,
    QString &confirmationMessage)
{
    Q_UNUSED(componentId);
    componentName.clear();
    applicationsUids.clear();
    confirmationMessage.clear();
    return true;
}

/*!
 Provides a list of uids of applications installed by the given package
 (windows stub).
 \param[in] componentId component id of an application to be uninstalled.
 \param[out] applicationsUids a list of uids of applications in the package
      of the given component id.
 \retval true if there is no error.
 */
bool CaSoftwareRegistryPrivate::getApplicationsUids(int componentId,
    QStringList &applicationsUids)
{
    Q_UNUSED(componentId);
    applicationsUids.clear();
    return true;
}

/*
 Stub for windows development.
 \param Component id.
 \return Map with dummy entries when component id is greater than 0, otherwise
 empty map.
 */
CaSoftwareRegistryPrivate::DetailMap CaSoftwareRegistryPrivate::entryDetails(
    int componentId) const
{
    CaSoftwareRegistry::DetailMap detailMap;
    
    if (componentId >=1) {
        detailMap[CaSoftwareRegistry::componentNameKey()] = "no data";
            
        detailMap[CaSoftwareRegistry::componentVersionKey()] = "no data";
        
        detailMap[CaSoftwareRegistry::componentVendorKey()] = "no data";
                
        detailMap[CaSoftwareRegistry::componentDriveInfoKey()] = "no data";
        
        detailMap[CaSoftwareRegistry::componentSizeKey()] = "no data";
        
        detailMap[CaSoftwareRegistry::componentTypeKey()] = "no data";
    }
    return detailMap;    
}

