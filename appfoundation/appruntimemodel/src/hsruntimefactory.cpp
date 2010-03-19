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


#include "hsruntimefactory.h"
#include "hsruntimefactory_p.h"
#include "hsruntimeprovidermanifest.h"
#include "hstest_global.h"

#include <QStringList>
#include <QDir>
#include <QPluginLoader>


HsRuntimeFactoryPrivate::HsRuntimeFactoryPrivate(HsRuntimeFactory* aPublic)
    : QObject(aPublic),
      mQ(aPublic)
{

}


HsRuntimeFactoryPrivate::~HsRuntimeFactoryPrivate()
{

}

QList<HsRuntimeToken> HsRuntimeFactoryPrivate::runtimes()
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

    QList<HsRuntimeToken> runtimes;

    for(int h=0; h < pluginPaths.count(); h++)
    {
        QString path = pluginPaths.at(h);
        QDir dir(path);
        QStringList filters("*.manifest");

        for(int i=0; i < dir.entryList(filters, QDir::Files).count(); ++i)
        {
            QString fileName = dir.entryList(filters, QDir::Files).at(i);

            HsRuntimeProviderManifest manifest;
            manifest.loadFromXml(dir.absoluteFilePath(fileName));

            if(manifest.loadOnQuery())
            {
                QList<HsRuntimeToken> tokens = manifest.runtimes();
                for(int j=0; j < tokens.count(); ++j)
                {
                    HsRuntimeToken token = tokens.at(j);
                    IHsRuntimeProvider* provider = loadProviderFromPlugin(token.mLibrary);
                    if(provider)
                    {
                        runtimes << provider->runtimes();
                        delete provider;
                    }
                }
            }
            else
            {
                runtimes << manifest.runtimes();
            }
        }
    }
    return runtimes;
}

HsRuntime* HsRuntimeFactoryPrivate::createRuntime(const HsRuntimeToken& aToken)
{
    IHsRuntimeProvider* provider = loadProviderFromPlugin(aToken.mLibrary);
    if(!provider)
    {
        HSDEBUG("Runtime creation failed - No provider.")
        return 0;
    }

    HsRuntime* runtime = provider->createRuntime(aToken);
    delete provider;
    if(!runtime)
    {
        HSDEBUG("Runtime creation failed.")
    }
    return runtime;
}

IHsRuntimeProvider* HsRuntimeFactoryPrivate::loadProviderFromPlugin(const QString& aPluginName)
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

    IHsRuntimeProvider* provider = 0;
    QPluginLoader loader;
    QObject* plugin = 0;

    for(int i=0; i < pluginPaths.count(); i++)
    {
        QString path = pluginPaths.at(i);
        QString fileName = QDir(path).absoluteFilePath(aPluginName);

        loader.setFileName(fileName);
        plugin = loader.instance();
        provider = qobject_cast<IHsRuntimeProvider*>(plugin);
        if(provider)
        {
            return provider;
        }

        //Don't leak memory if provider not IHsRuntimeProvider
        if(plugin)
        {
            HSDEBUG("Runtime provider load - !provider, deleting plugin.")
            delete plugin;
        }
    }

    HSDEBUG("Runtime provider load failed - Not found.")
    return 0;
}

/*!
    \class HsRuntimeFactory
    \ingroup group_hsruntimemodel
    \brief Finds and creates home screen runtimes.

    HsRuntime factory finds home screen runtimes from HsRuntime provider 
    plugins. The search is done based on given plugin manifest
    and plugin binary directories. Found runtimes are returned as
    a list of HsRuntime tokens. HsRuntime factory creates an instance of
    a HsRuntime base on a HsRuntime token that is given to it.
*/

/*!
    Constructor.
    
    \a aPluginManifestDirectory Directory that contains plugin manifests.
    \a aPluginDirectory Directory that contains plugin binaries.
    \a aParent Parent object.
*/
HsRuntimeFactory::HsRuntimeFactory(const QString& aPluginManifestDirectory,
                               const QString& aPluginDirectory,
                               QObject* aParent)
    : QObject(aParent)
{
    mD = new HsRuntimeFactoryPrivate(this);
    mD->mPluginManifestDirectory = aPluginManifestDirectory;
    mD->mPluginDirectory = aPluginDirectory;
}

/*!
    Destructor.    
*/
HsRuntimeFactory::~HsRuntimeFactory()
{

}

/*!
    Returns found runtimes as a list of HsRuntime tokens.    
*/
QList<HsRuntimeToken> HsRuntimeFactory::runtimes()
{
    return mD->runtimes();
}

/*!
    Creates and returns a HsRuntime based on the given token.    
    \a aToken Identifies the HsRuntime to be created.

    Return The created HsRuntime.
*/
HsRuntime* HsRuntimeFactory::createRuntime(const HsRuntimeToken& aToken)
{
    return mD->createRuntime(aToken);
}
