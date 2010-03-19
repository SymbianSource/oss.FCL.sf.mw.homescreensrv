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
* Description:  HsRuntime provider manifest.
*
*/


#ifndef HSRUNTIMEPROVIDERMANIFEST_H
#define HSRUNTIMEPROVIDERMANIFEST_H

#include <QObject>

#include "hsruntimemodel_global.h"
#include "hsiruntimeprovider.h"

class HsRuntimeProviderManifestPrivate;

class HSRUNTIMEMODEL_EXPORT HsRuntimeProviderManifest : public QObject
{
    Q_OBJECT

public:

    HsRuntimeProviderManifest(QObject* aParent = 0);   
    virtual ~HsRuntimeProviderManifest();

    bool loadFromXml(const QString& aFileName) const;
    QList<HsRuntimeToken> runtimes() const;
    bool loadOnQuery() const;

private:
    Q_DISABLE_COPY(HsRuntimeProviderManifest)

private:
    HsRuntimeProviderManifestPrivate* mD;
    friend class HsRuntimeProviderManifestPrivate;

};

#endif
