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

#include <QStringList>
#include <QDir>
#include <QPluginLoader>
#include <QState>

#include "hsstatefactory.h"
#include "hsstatefactory_p.h"
#include "hsstateprovidermanifest.h"
#include "hstest_global.h"

HsStateFactoryPrivate::HsStateFactoryPrivate(HsStateFactory* aPublic)
    : QObject(aPublic),
      mQ(aPublic)
{

}

HsStateFactoryPrivate::~HsStateFactoryPrivate()
{

}

QList<HsStateToken> HsStateFactoryPrivate::states()
{
    QStringList pluginPaths;

    //Check plugin dirs from root of different drives
    QFileInfoList drives = QDir::drives();
    for(int i=0; i < drives.count(); i++)
    {
        QFileInfo drive = drives.at(i);
        QString driveLetter = drive.absolutePath();
        QString path = driveLetter + mPluginManifestDirectory;
        if(QDir(path).exists())
        {
            pluginPaths << path;
        }
    }

    //Check plugin dir relative to current dir
    if(QDir(mPluginManifestDirectory).exists() && !pluginPaths.contains(QDir(mPluginManifestDirectory).absolutePath()))
    {
        pluginPaths << mPluginManifestDirectory;
    }

    QList<HsStateToken> states;

    for(int h=0; h < pluginPaths.count(); h++)
    {
        QString path = pluginPaths.at(h);
        QDir dir(path);
        QStringList filters("*.manifest");

        for(int i=0; i < dir.entryList(filters, QDir::Files).count(); ++i)
        {
            QString fileName = dir.entryList(filters, QDir::Files).at(i);

            HsStateProviderManifest manifest;
            manifest.loadFromXml(dir.absoluteFilePath(fileName));

            if(manifest.loadOnQuery())
            {
                QList<HsStateToken> tokens = manifest.states();
                for(int j=0; j < tokens.count(); ++j)
                {
                    HsStateToken token = tokens.at(j);
                    IHsStateProvider* provider = loadProviderFromPlugin(token.mLibrary);
                    if(provider)
                    {
                        states << provider->states();
                        delete provider;
                    }
                }
            }
            else
            {
                states << manifest.states();
            }
        }
    }
    return states;
}

QState* HsStateFactoryPrivate::createState(const HsStateToken& aToken)
{
    IHsStateProvider* provider = loadProviderFromPlugin(aToken.mLibrary);
    if(!provider)
    {
        HSDEBUG("Widget creation failed - No provider.")
        return 0;
    }

    QState* state = provider->createState(aToken);
    delete provider;
    if(!state)
    {
        HSDEBUG("State creation failed.")
    }
    return state;
}

IHsStateProvider* HsStateFactoryPrivate::loadProviderFromPlugin(const QString& aPluginName)
{
    QStringList pluginPaths;

    //Check plugin dirs from root of different drives
    QFileInfoList drives = QDir::drives();
    for(int i=0; i < drives.count(); i++)
    {
        QFileInfo drive = drives.at(i);
        QString driveLetter = drive.absolutePath();
        QString path = driveLetter + mPluginDirectory;
        if(QDir(path).exists())
        {
            pluginPaths << path;
        }
    }


    //Check plugin dir relative to current dir
    if(QDir(mPluginManifestDirectory).exists() && !pluginPaths.contains(QDir(mPluginDirectory).absolutePath()))
    {
        pluginPaths << mPluginDirectory;
    }

    IHsStateProvider* provider = 0;
    QPluginLoader loader;
    QObject* plugin = 0;

    for(int i=0; i < pluginPaths.count(); i++)
    {
        QString path = pluginPaths.at(i);
        QString fileName = QDir(path).absoluteFilePath(aPluginName);

        loader.setFileName(fileName);
        plugin = loader.instance();
        provider = qobject_cast<IHsStateProvider*>(plugin);
        if(provider)
        {
            return provider;
        }

        //Don't leak memory if provider not IHsStateProvider
        if(plugin)
        {
            HSDEBUG("State provider load - !provider, deleting plugin.")
            delete plugin;
        }
    }

    HSDEBUG("State provider load failed - Not found.")
    return 0;
}

/*!
    \class HsStateFactory
    \ingroup group_hsstatemodel
    \brief Finds and creates home screen states.
    State factory finds home screen states from state provider
    plugins. The search is done based on given plugin manifest
    and plugin binary directories. Found states are returned as
    a list of state tokens. State factory creates an instance of
    a state base on a state token that is given to it.
*/

/*!
    Constructor.
    \a aPluginManifestDirectory Directory that contains plugin manifests.
    \a aPluginDirectory Directory that contains plugin binaries.
    \a aParent Parent object.
*/
HsStateFactory::HsStateFactory(const QString& aPluginManifestDirectory,
                           const QString& aPluginDirectory,
                           QObject* aParent)
    : QObject(aParent)
{
    mD = new HsStateFactoryPrivate(this);
    mD->mPluginManifestDirectory = aPluginManifestDirectory;
    mD->mPluginDirectory = aPluginDirectory;
}


/*!
    Destructor.
*/
HsStateFactory::~HsStateFactory()
{

}

/*!
    Returns found states as a list of state tokens.
*/
QList<HsStateToken> HsStateFactory::states()
{
    return mD->states();
}

/*!
    Creates and returns a state based on the given token.
    \a aToken Identifies the state to be created.

    Returns The created state.
*/
QState* HsStateFactory::createState(const HsStateToken& aToken)
{
    return mD->createState(aToken);
}
