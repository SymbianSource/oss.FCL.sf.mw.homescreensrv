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
* Description:  Task monitor engine
 *
*/


#include "tsfswengine.h"

#include "tsfswdatalist.h"




#include <apgtask.h>
#include <AknDef.h>
#include <apgwgnam.h>
#include <mmf/common/mmfcontrollerpluginresolver.h> // for CleanupResetAndDestroyPushL
#include <featmgr.h>
#include <swi/swispubsubdefs.h>
#include <s32mem.h>
#include <bitmaptransforms.h>

// time to wait before refreshing content
const TInt KContentRefreshDelay = 50000; // 0.05 sec


// --------------------------------------------------------------------------
// CTsFswEngine::NewL
// --------------------------------------------------------------------------
//
CTsFswEngine* CTsFswEngine::NewL( MHsDataObserver& aObserver )
    {
    CTsFswEngine* self = NewLC( aObserver );
    CleanupStack::Pop( self );
    return self;
    }

// --------------------------------------------------------------------------
// CTsFswEngine::NewLC
// --------------------------------------------------------------------------
//
CTsFswEngine* CTsFswEngine::NewLC( MHsDataObserver& aObserver )
    {
    CTsFswEngine* self = new ( ELeave ) CTsFswEngine( aObserver );
    CleanupStack::PushL( self );
    self->ConstructL();
    return self;
    }

// --------------------------------------------------------------------------
// CTsFswEngine::CTsFswEngine
// --------------------------------------------------------------------------
//
CTsFswEngine::CTsFswEngine( MHsDataObserver& aObserver )
        :iObserver( aObserver )
    {
    }

// --------------------------------------------------------------------------
// CTsFswEngine::ConstructL
// --------------------------------------------------------------------------
//
void CTsFswEngine::ConstructL()
    {
    User::LeaveIfError( iWsSession.Connect() );
    User::LeaveIfError( iAppArcSession.Connect() );

    iDataList = CTsFswDataList::NewL(*this);    

    // get an initial list of tasks
    iDataList->SetAppDataRefreshNeeded();
    CollectTasksL();

    iUpdateStarter = CPeriodic::NewL( CActive::EPriorityStandard );   
    }

// --------------------------------------------------------------------------
// CTsFswEngine::~CTsFswEngine
// --------------------------------------------------------------------------
//
CTsFswEngine::~CTsFswEngine()
    {

    delete iUpdateStarter;

    iWgIds.Close();

    iAppArcSession.Close();
    iWsSession.Close();

    delete iDataList;
    }

// --------------------------------------------------------------------------
// CTsFswEngine::FswDataL
// --------------------------------------------------------------------------
//
const RTsFswArray& CTsFswEngine::FswDataL()
    {
    return iDataList->FswDataL();
    }

// --------------------------------------------------------------------------
// CTsFswEngine::UpdateTaskList
// Callback from appui
// --------------------------------------------------------------------------
//
void CTsFswEngine::UpdateTaskList()
    {
    // There can be many calls in a row, use a timer to prevent degrading
    // device performance.
    iDataList->SetDirty();
    if ( !iUpdateStarter->IsActive() )
        {
        iUpdateStarter->Start( KContentRefreshDelay, 0,
                TCallBack( UpdateStarterCallback, this ) );
        }    
    // get the foreground app uid and publish it to CFW if different than before
    TRAP_IGNORE( PublishFgAppUidL() );
    }

// --------------------------------------------------------------------------
// CTsFswEngine::UpdateStarterCallback
// Callback for the timer
// --------------------------------------------------------------------------
//
TInt CTsFswEngine::UpdateStarterCallback( TAny* aParam )
    {
    CTsFswEngine* self = static_cast<CTsFswEngine*>( aParam );
    self->iUpdateStarter->Cancel();
    // get list of tasks and notify observer if there is a change in the list
    TBool changed = EFalse;
    TRAPD( err, changed = self->CollectTasksL() );
    if ( err == KErrNone && changed )
        {
        self->iObserver.DataChanged();
        }
    return 0;
    }

// --------------------------------------------------------------------------
// CTsFswEngine::CollectTasksL
// --------------------------------------------------------------------------
//
TBool CTsFswEngine::CollectTasksL()
    {
    TBool changed = iDataList->CollectTasksL();
    return changed;
    }

// --------------------------------------------------------------------------
// CTsFswEngine::ForegroundAppUidL
// --------------------------------------------------------------------------
//
TUid CTsFswEngine::ForegroundAppUidL( TInt aType )
    {
    // Do not use GetFocusWindowGroup or others.
    // We want to see only "real" application uids in FgApp,
    // i.e. stuff that would also be shown in the task swapper
    // area of Taskswitcher.

    TUid result = KNullUid;
    RArray<RWsSession::TWindowGroupChainInfo> allWgIds;
    CleanupClosePushL( allWgIds );
    User::LeaveIfError( iWsSession.WindowGroupList( 0, &allWgIds ) );
    TInt count = allWgIds.Count();
    for ( TInt i = 0; i < count; ++i )
        {
        // Depending on aType we may not need to bother with iParentId here.
        // If aType == EUseEmbeddedUid => embedded apps are treated as others.
        // If aType == EUseStandaloneUid => embedded apps must be ignored.
        if ( allWgIds[i].iParentId > 0
                && aType == CTsFswEngine::EUseStandaloneUid )
            {
            continue;
            }

        CApaWindowGroupName* wgn = CApaWindowGroupName::NewLC(
            iWsSession, allWgIds[i].iId );
        TUid newUid = wgn->AppUid();
        TBool hidden = wgn->Hidden();
        CleanupStack::PopAndDestroy( wgn );

        if ( !hidden && newUid.iUid && !iDataList->IsHiddenUid(newUid) )
            {
            result = newUid;
            break;
            }
        }
    CleanupStack::PopAndDestroy( &allWgIds );
    
    return result;
    }

// --------------------------------------------------------------------------
// CTsFswEngine::PublishFgAppUidL
// --------------------------------------------------------------------------
//
void CTsFswEngine::PublishFgAppUidL()
    {
    TUid newUid = ForegroundAppUidL( CTsFswEngine::EUseEmbeddedUid );
    if ( iFgAppUid != newUid && newUid.iUid )
        {
        iFgAppUid = newUid;
        iDataList->MoveEntryAtStart(newUid.iUid, EFalse);
        }
    }

// end of file
