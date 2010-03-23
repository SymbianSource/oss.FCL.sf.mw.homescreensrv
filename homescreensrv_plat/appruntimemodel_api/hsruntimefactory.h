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
* Description:  HsRuntime factory.
*
*/


#ifndef HSRUNTIMEFACTORY_H
#define HSRUNTIMEFACTORY_H

#include <QObject>

#include "hsruntimemodel_global.h"
#include "hsiruntimeprovider.h"

class HsRuntimeFactoryPrivate;

class HSRUNTIMEMODEL_EXPORT HsRuntimeFactory : public QObject
{
    Q_OBJECT

public:
    HsRuntimeFactory(const QString& aPluginManifestDirectory,
                 const QString& aPluginDirectory,
                 QObject* aParent = 0);
    virtual ~HsRuntimeFactory();

    QList<HsRuntimeToken> runtimes();
    HsRuntime* createRuntime(const HsRuntimeToken& aToken);

private:
    Q_DISABLE_COPY(HsRuntimeFactory)

private:
    HsRuntimeFactoryPrivate* mD;
    friend class HsRuntimeFactoryPrivate;

};

#endif
