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
* Description:  Baget factory.
*
*/

#ifndef BAGETFACTORY_H
#define BAGETFACTORY_H

#include <QObject>

#include "ibagetprovider.h"
#include "bagetmodel_global.h"

class QPluginLoader;

class BAGETMODEL_EXPORT BagetPluginUnloader : public QObject
{
    Q_OBJECT

public:

    BagetPluginUnloader(QPluginLoader *pluginLoader, QObject *parent = 0);
    ~BagetPluginUnloader();

private:

    QPluginLoader *mPluginLoader;

};

class BagetFactoryPrivate;

class BAGETMODEL_EXPORT BagetFactory : public QObject
{
    Q_OBJECT

public:

    BagetFactory(const QString& pluginDirectory, QObject *parent = 0);
    virtual ~BagetFactory();

    Baget *createBaget(const BagetToken &token);

private:

    Q_DISABLE_COPY(BagetFactory)

private:

    BagetFactoryPrivate *m_d;
    friend class BagetFactoryPrivate;

};

#endif // BAGETFACTORY_H
