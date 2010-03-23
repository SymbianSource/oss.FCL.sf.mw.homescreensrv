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
* Description:  State factory.
*
*/


#ifndef HSSTATEFACTORY_H
#define HSSTATEFACTORY_H

#include <QObject>

#include "hsstatemodel_global.h"
#include "hsistateprovider.h"

class QState;
class HsStateFactoryPrivate;

class HSSTATEMODEL_EXPORT HsStateFactory : public QObject
{
    Q_OBJECT

public:
    HsStateFactory(const QString& aPluginManifestDirectory,
                 const QString& aPluginDirectory,
                 QObject* aParent = 0);

    virtual ~HsStateFactory();

    QList<HsStateToken> states();
    QState* createState(const HsStateToken& aToken);

private:

    Q_DISABLE_COPY(HsStateFactory)

private:
    HsStateFactoryPrivate* mD;
    friend class HsStateFactoryPrivate;

};

#endif
