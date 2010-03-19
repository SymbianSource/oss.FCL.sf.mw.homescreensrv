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
* Description:  Widget factory.
*
*/

#include <QDebug>
#include <QStringList>
#include <QDir>
#include <QPluginLoader>

#include "bagetfactory.h"
#include "bagetfactory_p.h"
#include "baget.h"
#include "ibagetprovider.h"

/*!
    \class BagetPluginUnloader
    \brief Unloads plugin and deletes plugin loader.
    
    Holds plugin loader instance and unloads and destroys plugin
    on it's destructor.
 */

/*!
    Constructs a new BagetPluginUnloader with \a pluginLoader and \a parent.
 */
BagetPluginUnloader::BagetPluginUnloader(QPluginLoader *pluginLoader, QObject *parent) :
    QObject(parent), mPluginLoader(pluginLoader)
{
}

/*!
    Destructs the class.
 */
BagetPluginUnloader::~BagetPluginUnloader()
{
    if (mPluginLoader) {
        mPluginLoader->unload();
        delete mPluginLoader;
    }
}

/*!
    \class BagetFactoryPrivate
    \brief Private implementation of the BagetFactory.
 */

/*!
    Constructs a new BagetFactoryPrivate with \a pluginDirectory and \a bagetFactoryPublic.
 */
BagetFactoryPrivate::BagetFactoryPrivate(const QString& pluginDirectory, 
                                         BagetFactory *bagetFactoryPublic) :
    m_q(bagetFactoryPublic), mPluginDirectory(pluginDirectory)
{
}

/*!
    Destructs the class.
 */
BagetFactoryPrivate::~BagetFactoryPrivate()
{
}

/*!
    Creates and returns a Baget based on the given token.
    \param token Identifies the Baget to be created.
    \return The created Baget.
 */
Baget *BagetFactoryPrivate::createBaget(const BagetToken &token)
{
    QStringList pluginPaths;

    // check plugin dirs from root of different drives
    QFileInfoList drives = QDir::drives();
    for(int i=0; i < drives.count(); i++) {
        QFileInfo drive = drives.at(i);
        QString driveLetter = drive.absolutePath();
        QString path = driveLetter + mPluginDirectory;
        if (QDir(path).exists()) {
            pluginPaths << path;
        }
    }

    // check plugin dir relative to current dir
    if (QDir(mPluginDirectory).exists() && 
        !pluginPaths.contains(QDir(mPluginDirectory).absolutePath())) {
        pluginPaths << mPluginDirectory;
    }

    IBagetProvider *provider(0);
    QPluginLoader *loader = new QPluginLoader();
    QObject *plugin(0);

    for(int i=0; i < pluginPaths.count(); i++) {
        QString path = pluginPaths.at(i);
        QString fileName = QDir(path).absoluteFilePath(token.mLibrary);

        loader->setFileName(fileName);
        plugin = loader->instance();
        provider = qobject_cast<IBagetProvider*>(plugin);
        if (provider) {
            break;
        }
    }

    Baget *baget(0);

    if (provider) {
        baget = provider->createBaget(token);
        if (!baget) {
            qWarning() << "Baget creation failed.";
            qWarning() << token.mLibrary << "cannot provide" << token.mUri;
            loader->unload();
            delete loader;
        } else {
            // unload plugin once baget gets deleted
            BagetPluginUnloader *unloader = new BagetPluginUnloader(loader);
            unloader->connect(baget, SIGNAL(destroyed()), SLOT(deleteLater()));
        }
    } else {
        qDebug() << "Baget creation failed.";
        qWarning() << token.mLibrary << "- provider not found";
        loader->unload();
        delete loader;
    }

    return baget;
}

/*!
    \class BagetFactory
    \brief Finds and creates baget widgets.

    Baget factory creates an instance of a Baget
    based on a BagetToken that is given to it.
 */

/*!
    Constructs a new BagetFactory with \a pluginDirectory and \a parent.
 */
BagetFactory::BagetFactory(const QString& pluginDirectory, QObject *parent) :
    QObject(parent), m_d(new BagetFactoryPrivate(pluginDirectory, this))
{
}

/*!
    Destructs the class.
 */
BagetFactory::~BagetFactory()
{
    delete m_d;
}

/*!
    Creates and returns a Baget based on the given token.
    \param token Identifies the Baget to be created.
    \return The created Baget.
 */
Baget* BagetFactory::createBaget(const BagetToken &token)
{
    return m_d->createBaget(token);
}
