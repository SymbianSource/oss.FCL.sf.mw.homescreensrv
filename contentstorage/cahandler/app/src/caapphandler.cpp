/*
 * Copyright (c) 2008 Nokia Corporation and/or its subsidiary(-ies).
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
 * Description:  ?Description
 *
 */

#include <QString>
#include <e32base.h>
#include <w32std.h>
#include <apgtask.h>
#include <apgcli.h>
#include <eikenv.h>
#include <eikappui.h>
#include <vwsdef.h>
#include <AknDef.h>
#include <AknTaskList.h>

#include <usif/usifcommon.h>
#include <usif/scr/scr.h>

#include <cadefs.h>
#include <caentry.h>

#include "caapphandler.h"
#include "cainnerentry.h"
#include "causifuninstalloperation.h"
#include "catasklist.h"
#include "cautils.h"
#include "caclient_defines.h"
#include "cadef.h"

using namespace Usif;

static const char caTypeApp[] = "application";
static const char caTypePackage[] = "package";
static const char caTypeWidget[] = "widget";
static const char caAttrView[] = "view";
static const char caCmdClose[] = "close";
static const char caAttrWindowGroupId[] = "window_group_id";
static const char caAttrComponentId[] = "component_id";
static const char caCmdRemove[] = "remove";

// ---------------------------------------------------------
//
// ---------------------------------------------------------
//
CaAppHandler::CaAppHandler(QObject *parent):
    iEikEnv(CEikonEnv::Static()),
    iUsifUninstallOperation(NULL)
{
    Q_UNUSED(parent);
}

// ---------------------------------------------------------------------------
//
// ---------------------------------------------------------------------------
//
CaAppHandler::~CaAppHandler()
{
    delete iUsifUninstallOperation;
}

int CaAppHandler::execute(const CaEntry &entry, const QString &command)
{
    int result(KErrGeneral);
    if (command == caCmdOpen && entry.entryTypeName() == caTypeApp) {
        QString viewIdValue = entry.attribute(caAttrView);
        bool viewIdIsCorrect(true);
        int viewId(-1);
        if (!viewIdValue.isNull()) {
            viewId = viewIdValue.toInt(&viewIdIsCorrect, 0);
        }
        if (viewIdIsCorrect) {
            QString uidValue =
                entry.attribute(APPLICATION_UID_ATTRIBUTE_NAME);
            int uid = uidValue.toInt();
            TRAP(result, launchApplicationL(TUid::Uid(uid), viewId));
        }
    } else if (command == caCmdClose && entry.entryTypeName() == caTypeApp ) {
        QString windowGroupId = entry.attribute(caAttrWindowGroupId);
        if (!windowGroupId.isNull()) {
            result = closeApplication(entry.flags(), windowGroupId.toInt());
        }
    } else if (command == caCmdRemove) {
        QString componentId(entry.attribute(caAttrComponentId));
        result = handleRemove(entry.flags(),
            entry.entryTypeName(),
            componentId);
    } else {
        result = KErrNotSupported;
    }

    return result;
}

// ---------------------------------------------------------------------------
//
// ---------------------------------------------------------------------------
//
void CaAppHandler::launchApplicationL(const TUid aUid, TInt aViewId)
{
    if( aViewId > 0 && iEikEnv )
        {
        TUid viewId = TUid::Uid( aViewId );
        TVwsViewId view( aUid, viewId );
        iEikEnv->EikAppUi()->ActivateViewL( view );
        }
    else
        {
        RWsSession wsSession;
        User::LeaveIfError( wsSession.Connect() );
        CleanupClosePushL<RWsSession>( wsSession );

        CCaTaskList* taskList = CCaTaskList::NewLC( wsSession );
        TApaTask task = taskList->FindRootApp( aUid );
        CleanupStack::PopAndDestroy( taskList );

        if( task.Exists() )
            {
            task.BringToForeground();
            }
        else
            {
            // TApaAppInfo size is greater then 1024 bytes
            // so its instances should not be created on the stack.
            TApaAppInfo* appInfo = new( ELeave ) TApaAppInfo;
            CleanupStack::PushL( appInfo );
            TApaAppCapabilityBuf capabilityBuf;
            RApaLsSession appArcSession;
            User::LeaveIfError( appArcSession.Connect() );
            CleanupClosePushL<RApaLsSession>( appArcSession );

            User::LeaveIfError( appArcSession.GetAppInfo( *appInfo, aUid ) );
            User::LeaveIfError( appArcSession.GetAppCapability(
                                   capabilityBuf, aUid ) );

            TApaAppCapability &caps = capabilityBuf();
            CApaCommandLine *cmdLine = CApaCommandLine::NewLC();
            cmdLine->SetExecutableNameL( appInfo->iFullName );

            if( caps.iLaunchInBackground )
                {
                cmdLine->SetCommandL( EApaCommandBackground );
                }
            else
                {
                cmdLine->SetCommandL( EApaCommandRun );
                }

            cmdLine->SetTailEndL( KNullDesC8 );

            User::LeaveIfError( appArcSession.StartApp( *cmdLine ) );

            CleanupStack::PopAndDestroy( cmdLine );
            CleanupStack::PopAndDestroy( &appArcSession );
            CleanupStack::PopAndDestroy( appInfo );
        }
        CleanupStack::PopAndDestroy( &wsSession );
    }
}

// ---------------------------------------------------------------------------
//
// ---------------------------------------------------------------------------
//
int CaAppHandler::closeApplication(const EntryFlags &flags, int windowGroupId)
{
    int result(KErrNone);
    if (flags.testFlag(RunningEntryFlag)
        && !( flags.testFlag(SystemEntryFlag))
        && windowGroupId > 0) {
        RWsSession wsSession;
        result = wsSession.Connect();
        if (result==KErrNone) {
            TWsEvent event;
            event.SetTimeNow();
            event.SetType( KAknShutOrHideApp );
            wsSession.SendEventToWindowGroup( windowGroupId, event );
        }
        wsSession.Close();
    }
    return result;
}

// ---------------------------------------------------------------------------
//
// ---------------------------------------------------------------------------
//
int CaAppHandler::handleRemove(const EntryFlags &flags,
    const QString &typeName,
    const QString &componentId)
{
    int result(KErrNone);
    if (flags.testFlag(RemovableEntryFlag)) {
        if (typeName == caTypeApp
            || typeName == caTypePackage
            || typeName == caTypeWidget) {
            if(componentId.isNull()) {
                result = KErrNotFound;
            } else {
                bool convertStatus(false);
                int id = componentId.toInt(&convertStatus);
                if (convertStatus) {
                    TRAP(result, startUsifUninstallL(id));
                } else {
                    result = KErrGeneral;
                }
            }
        }
        else {
            result = KErrNotSupported;
        }
    } else {
        result = KErrAccessDenied;
    }
    return result;
}

// ---------------------------------------------------------------------------
//
// ---------------------------------------------------------------------------
//
void CaAppHandler::startUsifUninstallL(TInt aComponentId)
{
    if (iUsifUninstallOperation && iUsifUninstallOperation->IsActive()) {
        User::Leave( KErrInUse );
    }
    delete iUsifUninstallOperation;
    iUsifUninstallOperation = NULL;
    iUsifUninstallOperation = CCaUsifUninstallOperation::NewL(aComponentId);
}

