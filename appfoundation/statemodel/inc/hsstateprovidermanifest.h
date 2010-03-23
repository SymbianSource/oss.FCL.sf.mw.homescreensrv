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
* Description:  State provider manifest.
*
*/


#ifndef HSSTATEPROVIDERMANIFEST_H
#define HSSTATEPROVIDERMANIFEST_H

#include <QObject>

#include "hsstatemodel_global.h"
#include "hsistateprovider.h"

class HsStateProviderManifestPrivate;

class HSSTATEMODEL_EXPORT HsStateProviderManifest : public QObject
{
    Q_OBJECT

public:
    HsStateProviderManifest(QObject* aParent = 0);
    virtual ~HsStateProviderManifest();

    QList<HsStateToken> states() const;
    bool loadOnQuery() const;
    bool loadFromXml(const QString& aFileName);

private:
    Q_DISABLE_COPY(HsStateProviderManifest)

private:
    HsStateProviderManifestPrivate* mD;
    friend class HsStateProviderManifestPrivate;

};

#endif
