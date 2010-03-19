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
* Description:  State factory private implementation.
*
*/


#ifndef HSSTATEFACTORY_P_H
#define HSSTATEFACTORY_P_H

#include "hsistateprovider.h"

class QState;
class HsStateFactory;
class IHsStateProvider;

class HsStateFactoryPrivate : public QObject
{
    Q_OBJECT

public:

    HsStateFactoryPrivate(HsStateFactory* aPublic);
    ~HsStateFactoryPrivate();

private:

    Q_DISABLE_COPY(HsStateFactoryPrivate)

public:
    QList<HsStateToken> states();
    QState* createState(const HsStateToken& aToken);
    IHsStateProvider* loadProviderFromPlugin(const QString& aPluginName);

public:
    HsStateFactory* mQ;
    QString mPluginManifestDirectory;
    QString mPluginDirectory;

};

#endif
