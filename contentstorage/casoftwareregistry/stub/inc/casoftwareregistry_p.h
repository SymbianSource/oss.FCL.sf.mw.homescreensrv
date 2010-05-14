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
 * Description: casoftwareregistry_p.h
 *
 */

#ifndef CA_SOFTWARE_REGISTRY_PRIVATE_H
#define CA_SOFTWARE_REGISTRY_PRIVATE_H

#include <QHash>
#include <QString>

class CaSoftwareRegistry;

class CaSoftwareRegistryPrivate
{
public:
    typedef QHash<QString, QString> DetailMap;
    
    explicit CaSoftwareRegistryPrivate(
        CaSoftwareRegistry *softwareRegistryPublic);
    ~CaSoftwareRegistryPrivate();

    DetailMap entryDetails(int componentId) const;
private:

    CaSoftwareRegistry *const m_q;

};

#endif // CA_SOFTWARE_REGISTRY_PRIVATE_H
