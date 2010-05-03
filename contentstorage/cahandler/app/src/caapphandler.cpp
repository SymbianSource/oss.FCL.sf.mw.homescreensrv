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

#include "caapphandler.h"
#include "cainnerentry.h"
#include "cauninstalloperation.h"
#include "causifuninstalloperation.h"
#include "catasklist.h"

#include "cautils.h"
#include "cadef.h"

// ======== MEMBER FUNCTIONS ========
using namespace Usif;

// ---------------------------------------------------------------------------
//
// ---------------------------------------------------------------------------
//
CCaAppHandler::~CCaAppHandler()
{
    delete iUninstallOperation;
    delete iUsifUninstallOperation;
}

// ---------------------------------------------------------------------------
//
// ---------------------------------------------------------------------------
//
CCaAppHandler *CCaAppHandler::NewL()
{
    CCaAppHandler *handler = new(ELeave) CCaAppHandler();
    CleanupStack::PushL( handler );
    handler->ConstructL();
    CleanupStack::Pop( handler );
    return handler;
}

// ---------------------------------------------------------
// CCaAppHandler::CCaAppHandler
// ---------------------------------------------------------
//
CCaAppHandler::CCaAppHandler()
{
    iEikEnv = CEikonEnv::Static();
    iUninstallOperation = NULL;
    iUsifUninstallOperation = NULL;
}

// ---------------------------------------------------------------------------
//
// ---------------------------------------------------------------------------
//
void CCaAppHandler::ConstructL()
{
}

// ---------------------------------------------------------------------------
//
// ---------------------------------------------------------------------------
//
void CCaAppHandler::HandleCommandL(
        CCaInnerEntry &aEntry, const TDesC8 &aCommand )
{

    if( aCommand == KCaCmdOpen()
            && aEntry.GetEntryTypeName() == KCaTypeApp() )
        {
        TInt viewId(-1);
        TPtrC viewIdValue;
        if( aEntry.FindAttribute( KCaAttrView(), viewIdValue ) )
            {
            if( MenuUtils::GetTUint( viewIdValue, (TUint &) viewId )
                    != KErrNone )
                {
                User::Leave( KErrCorrupt );
                }
            }
        LaunchApplicationL(
                TUid::Uid( aEntry.GetUid() ), KNullDesC8(), viewId );
        }
    else if ( aCommand == KCaCmdClose()
            && aEntry.GetEntryTypeName() == KCaTypeApp() )
        {
        CloseApplicationL( aEntry );
        }
    else if ( aCommand == KCaCmdRemove() )
        {
        HandleRemoveL(aEntry);
        }
    else
        {
        User::Leave( KErrNotSupported );
        }
}

// ---------------------------------------------------------------------------
//
// ---------------------------------------------------------------------------
//
void CCaAppHandler::LaunchApplicationL(
        const TUid aUid, const TDesC8 &aParam, TInt aViewId )
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

            cmdLine->SetTailEndL( aParam );

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
void CCaAppHandler::CloseApplicationL( CCaInnerEntry &aEntry )
{
    RWsSession wsSession;
    User::LeaveIfError( wsSession.Connect() );
    CleanupClosePushL<RWsSession>( wsSession );

    if( ( aEntry.GetFlags() & ERunning )
            && !( aEntry.GetFlags() & ESystem ) )
        {
        RBuf value;
        CleanupClosePushL( value );
        value.CreateL( KCaMaxAttrValueLen );
        if( aEntry.FindAttribute( KCaAttrWindowGroupId, value ) )
            {
            TInt wgId( KErrNotFound );
            TLex16 parser( value );
            if( KErrNone == parser.Val( wgId ) && wgId > 0 )
                {
                TWsEvent event;
                event.SetTimeNow();
                event.SetType( KAknShutOrHideApp );
                wsSession.SendEventToWindowGroup( wgId, event );
                }
            }
        CleanupStack::PopAndDestroy( &value );
    }

    CleanupStack::PopAndDestroy( &wsSession );
}

// ---------------------------------------------------------------------------
//
// ---------------------------------------------------------------------------
//
void CCaAppHandler::HandleRemoveL( CCaInnerEntry &aEntry )
{
    if (!( aEntry.GetFlags() & ERemovable ) )
        {
        User::Leave( KErrAccessDenied );
        }
    if ( aEntry.GetEntryTypeName() == KCaTypeApp() )
        {
        TComponentId componentId( GetComponentIdL( aEntry,KSoftwareTypeJava ) );
        if ( componentId != KErrNotFound )
            {
            StartUsifUninstallL( componentId );
            }
        else
            {
            StartSwiUninstallL( aEntry );
            }
        }
    else if ( aEntry.GetEntryTypeName() == KCaTypeWidget() )
        {
        StartSwiUninstallL( aEntry );
        }
    else if( aEntry.GetEntryTypeName() == KCaTypePackage() )
        {
        TPtrC componentId;
        if ( aEntry.FindAttribute( KCaAttrComponentId, componentId ) )
            {
            TInt32 id ;
            TLex idDesc;
            idDesc.Assign( componentId );
            User::LeaveIfError( idDesc.Val( id ) );
            StartUsifUninstallL( id );
            }
        }
    else
        {
        User::Leave( KErrNotSupported );
        }
}

// ---------------------------------------------------------------------------
//
// ---------------------------------------------------------------------------
//
TInt CCaAppHandler::GetComponentIdL( const CCaInnerEntry &aEntry,
        const TDesC& aSoftwareType )
{
    TInt id(KErrNotFound);
    RSoftwareComponentRegistry scr;
    CleanupClosePushL(scr);
    User::LeaveIfError(scr.Connect());
    CComponentFilter* compFilter = CComponentFilter::NewLC();
    compFilter->AddPropertyL(_L("Uid"), aEntry.GetUid());
    compFilter->SetSoftwareTypeL(aSoftwareType);
    RArray<TComponentId> componentIdList;
    CleanupClosePushL(componentIdList);
    scr.GetComponentIdsL(componentIdList, compFilter);
    if (componentIdList.Count() > 0)
        {
        id = componentIdList[0];
        }
    CleanupStack::PopAndDestroy(&componentIdList);
    CleanupStack::PopAndDestroy(compFilter);
    CleanupStack::PopAndDestroy(&scr);
    return id;
}

// ---------------------------------------------------------------------------
//
// ---------------------------------------------------------------------------
//
void CCaAppHandler::StartUsifUninstallL( TInt aComponentId )
{
    if ( iUsifUninstallOperation && iUsifUninstallOperation->IsActive() )
        {
        User::Leave( KErrInUse );
        }
    delete iUsifUninstallOperation;
    iUsifUninstallOperation = NULL;
    iUsifUninstallOperation = CCaUsifUninstallOperation::NewL( aComponentId );
}

// ---------------------------------------------------------------------------
//
// ---------------------------------------------------------------------------
//
void CCaAppHandler::StartSwiUninstallL(CCaInnerEntry &aEntry )
{
    if ( iUninstallOperation && iUninstallOperation->IsActive() )
        {
        User::Leave( KErrInUse );
        }
    delete iUninstallOperation;
    iUninstallOperation = NULL;
    iUninstallOperation = CCaUninstallOperation::NewL( aEntry );
}
