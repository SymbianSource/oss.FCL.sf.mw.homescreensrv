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
#include "applicationlauncher_p.h"

#include <apgcli.h>
#include <apacmdln.h>
#include <apgtask.h>
#include <eikenv.h>

#include <XQConversions>

bool ApplicationLauncherPrivate::isRunning(int applicationId)
{
    TApaTaskList taskList(CEikonEnv::Static()->WsSession());
    TApaTask task = taskList.FindApp(TUid::Uid(applicationId));
    return task.Exists();
}

void ApplicationLauncherPrivate::startApplication(int applicationId, const QString &activityId)
{
    QString actId = "\""+activityId+"\"";
    QString uri = QString("-activity appto://%1?activityname=%2").arg(uint(applicationId), 8, 16, QChar('0')).arg(actId);

    QT_TRAP_THROWING( {
        HBufC *uriAsDescriptor = XQConversions::qStringToS60Desc(uri);
        CleanupStack::PushL(uriAsDescriptor);

        RApaLsSession apaLsSession;
        User::LeaveIfError(apaLsSession.Connect());
        CleanupClosePushL(apaLsSession);

        TApaAppInfo appInfo;
        TInt retVal = apaLsSession.GetAppInfo(appInfo, TUid::Uid(applicationId));

        if (retVal == KErrNone) {
            RProcess application;
            User::LeaveIfError(application.Create(appInfo.iFullName, *uriAsDescriptor));
            application.Resume();
        } else {
            // @todo
        }

        CleanupStack::PopAndDestroy(&apaLsSession);
        CleanupStack::PopAndDestroy(uriAsDescriptor);
    }
                    );
}

void ApplicationLauncherPrivate::bringToForeground(int applicationId)
{
    TApaTaskList taskList(CEikonEnv::Static()->WsSession());
    TApaTask task = taskList.FindApp(TUid::Uid(applicationId));
    if (task.Exists()) {
        task.BringToForeground();
    } else {
        qCritical("Cannot bring to forward task %08x", applicationId);
    }
}