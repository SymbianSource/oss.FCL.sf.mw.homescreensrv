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
* Description:  HsRuntime provider manifest private implementation.
*
*/


#ifndef HSRUNTIMEPROVIDERMANIFEST_P_H
#define HSRUNTIMEPROVIDERMANIFEST_P_H

#include <QDomElement>

#include "hsiruntimeprovider.h"

class HsRuntimeProviderManifest;

class HsRuntimeProviderManifestPrivate : public QObject
{
    Q_OBJECT
    
public:
    HsRuntimeProviderManifestPrivate(HsRuntimeProviderManifest* aPublic);
    ~HsRuntimeProviderManifestPrivate();

    QList<HsRuntimeToken> runtimes() const;
    bool loadFromXml(const QString& aFileName);

private:
    QString parseAttribute(QDomElement& aElement, const QString& aAttributeName, bool aIsRequired = true) const;
    Q_DISABLE_COPY(HsRuntimeProviderManifestPrivate)

public:

    HsRuntimeProviderManifest* mQ;
    QList<HsRuntimeToken> mRuntimeTokens;
    bool mLoadOnQuery;

};

#endif
