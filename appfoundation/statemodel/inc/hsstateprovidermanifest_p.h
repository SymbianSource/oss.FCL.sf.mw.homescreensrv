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
* Description:  State provider manifest private implementation.
*
*/


#ifndef HSSTATEPROVIDERMANIFEST_P_H
#define HSSTATEPROVIDERMANIFEST_P_H

#include <QDomElement>

#include "hsistateprovider.h"

class HsStateProviderManifest;
class HsStateProviderManifestPrivate : public QObject
{
    Q_OBJECT
    
public:
    HsStateProviderManifestPrivate(HsStateProviderManifest* aPublic);
    ~HsStateProviderManifestPrivate();

    bool loadFromXml(const QString& aFileName);
    QList<HsStateToken> states() const;
    
private:
    QString parseAttribute(QDomElement& aElement, const QString& aAttributeName, bool aIsRequired = true) const;
    Q_DISABLE_COPY(HsStateProviderManifestPrivate)

public:
    HsStateProviderManifest* mQ;
    QList<HsStateToken> mStateTokens;
    bool mLoadOnQuery;
};

#endif
