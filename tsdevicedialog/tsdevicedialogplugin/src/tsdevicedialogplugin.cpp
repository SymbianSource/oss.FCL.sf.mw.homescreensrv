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
* Description:
*
*/

#include "tsdevicedialogplugin.h"

#include <QTranslator>
#include <QCoreApplication>
#include <QLocale>
#include <QtPlugin>

#include <QValueSpacePublisher>
#include <QServiceManager>

#include <HbDeviceDialog>
#include <HbMainWindow>

#include <tspropertydefs.h>

#include "tsdevicedialogcontainer.h"
#include "tstasksgrid.h"
#include "tstasksgriditem.h"
#include "tsdocumentloader.h"
#include "tsmodel.h"

QTM_USE_NAMESPACE

/*!
    \class TsDeviceDialogPlugin
    \ingroup group_tsdevicedialogplugin
    \brief TaskSwitcher Device Dialog Plug-in.
 */

namespace
{
    const char KTranslationPath[] = "resource/qt/translations";
    const char KTsDialogType[] = "com.nokia.taskswitcher.tsdevicedialogplugin/1.0";
    const char KActivityManaged [] = "com.nokia.qt.activities.ActivityManager";
}

/*!
    Constructor.
 */
TsDeviceDialogPlugin::TsDeviceDialogPlugin()
    :
    mModel(0),
    mStorage(0),
    mTriedToLoadTranslation(false)
{
}

TsDeviceDialogPlugin::~TsDeviceDialogPlugin()
{
    delete mModel;
}
/*!
    \reimp
 */
bool TsDeviceDialogPlugin::accessAllowed(const QString &deviceDialogType,
                                         const QVariantMap &parameters,
                                         const QVariantMap &securityInfo) const
{
    Q_UNUSED(deviceDialogType)
    Q_UNUSED(parameters)
    Q_UNUSED(securityInfo)

    // This plugin doesn't perform operations that may compromise security.
    // All clients are allowed to use it.
    return true;
}

/*!
    \reimp
 */
HbDeviceDialogInterface *
TsDeviceDialogPlugin::createDeviceDialog(const QString &deviceDialogType,
                                         const QVariantMap &parameters)
{
    Q_UNUSED(parameters)
    HbDeviceDialogInterface *dialogInterface(0);
    if (deviceDialogType == QString(KTsDialogType)) {
        // lazy loading of translation
        if (!mTriedToLoadTranslation) {
            mTriedToLoadTranslation = true;

            QTranslator *translator = new QTranslator(this);
            QString translationFile =
                QString("taskswitcher_%1").arg(QLocale::system().name());

            bool translationLoaded(false);
#ifdef Q_OS_SYMBIAN
            translationLoaded =
                translator->load(translationFile,
                                 QString("z:/") + KTranslationPath);
            if (!translationLoaded) {
                translationLoaded =
                    translator->load(translationFile,
                                     QString("c:/") + KTranslationPath);
            }
#else
            translationLoaded =
                translator->load(translationFile,
                                 QString(KTranslationPath));
#endif //Q_OS_SYMBIAN

            Q_ASSERT(translationLoaded);
            qApp->installTranslator(translator);
        }

        // lazy loading of model
        if (!mModel) {
            mStorage = new TsTaskMonitor(this);

            QServiceManager serviceManager;
            QObject *activityManager(serviceManager.loadInterface(KActivityManaged));
            if (activityManager) {
                activityManager->setParent(this); //make it autodestructed
            } else {
                activityManager = this; //activity plugin is not present. provide invalid instance because its not critical functionality.
                //QMetaObject::invokeMethod is safe to use in such a case.
            }
            mModel = new TsModel(*mStorage, *activityManager);
        }

        // ensure the dismiss request property is set to false

        QValueSpacePublisher dismissRequestPublisher(TsProperty::KTsPath);
        dismissRequestPublisher.setValue(TsProperty::KDismissRequestPath, static_cast<int>(false));
        dismissRequestPublisher.sync();


        // create device dialog
        dialogInterface = new TsDeviceDialogContainer(mModel);
    }
    return dialogInterface;
}

/*!
    \reimp
 */
bool TsDeviceDialogPlugin::deviceDialogInfo(const QString &deviceDialogType,
                                            const QVariantMap &parameters,
                                            DeviceDialogInfo *info) const
{
    Q_UNUSED(parameters)
    Q_UNUSED(deviceDialogType)

    info->group = GenericDeviceDialogGroup;
    info->flags = SingleInstance;
    info->priority = DefaultPriority;

    return true;
}

/*!
    \reimp
 */
QStringList TsDeviceDialogPlugin::deviceDialogTypes() const
{
    return QStringList(QString(KTsDialogType));
}

/*!
    \reimp
 */
HbDeviceDialogPlugin::PluginFlags TsDeviceDialogPlugin::pluginFlags() const
{
    return PluginFlags(PreloadPlugin | KeepPluginLoaded);
}

/*!
    \reimp
 */
int TsDeviceDialogPlugin::error() const
{
    return 0;
}

#ifdef COVERAGE_MEASUREMENT
#pragma CTC SKIP
#endif //COVERAGE_MEASUREMENT

Q_EXPORT_PLUGIN2(tsdevicedialogplugin, TsDeviceDialogPlugin)

#ifdef COVERAGE_MEASUREMENT
#pragma CTC ENDSKIP
#endif //COVERAGE_MEASUREMENT
