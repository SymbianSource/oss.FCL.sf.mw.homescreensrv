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
#include <QString>


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

