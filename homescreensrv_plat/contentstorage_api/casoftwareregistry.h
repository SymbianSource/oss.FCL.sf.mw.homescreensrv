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

#ifndef CA_SOFTWARE_REGISTRY_H
#define CA_SOFTWARE_REGISTRY_H

#include <QHash>
#include <QObject>
#include <QSharedPointer>
#include <QString>

#include "caclient_global.h"

//FORWARD DECLARATIONS
class CaSoftwareRegistryPrivate;

/**
 * Class provides Usif specific services
 */
class CACLIENT_EXPORT CaSoftwareRegistry: public QObject
{
    
Q_OBJECT    

    explicit CaSoftwareRegistry(QObject *parent = 0);

    ~CaSoftwareRegistry();
    
public:
    
    static QSharedPointer<CaSoftwareRegistry> create();
    
    typedef QHash<QString, QString> DetailMap;
    
    DetailMap entryDetails(int componentId) const;
    
    static QString componentNameKey();
    static QString componentVersionKey();
    static QString componentVendorKey();
    static QString componentDriveInfoKey();
    static QString componentSizeKey();
    static QString componentTypeKey();
private:
    /**
     * Pointer to a private implementation.
     */
    CaSoftwareRegistryPrivate *const m_d;

    /**
     * The static pointer to the instance of CaSoftwareRegistry.
     */
    static QWeakPointer<CaSoftwareRegistry> m_instance;

    friend class CaSoftwareRegistryPrivate;
    friend class QtSharedPointer::ExternalRefCount<CaSoftwareRegistry>;

    Q_DISABLE_COPY(CaSoftwareRegistry)
    
};

#endif // CA_SOFTWARE_REGISTRY_H
