/*
* Copyright (c) 2007 Nokia Corporation and/or its subsidiary(-ies).
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


#include <apgtask.h>        //TApaTask
#include <apgcli.h>         //RApaLsSession

#include "hs_app_aiscuttargetshutter.h"
#include "aiscutdefs.h"
#include "debug.h"

#include <AknSgcc.h>

const TUid KMenuUID3 = { 0x101F4CD2 }; 

const TInt KTaskExistsDelay = 1500000; //1.5 second
const TInt KTaskNotExistsDelay = 500000; //0.5 second
const TInt KMaxNumberOfTries = 3;


// ======== MEMBER FUNCTIONS ========

CAiScutTargetShutter::CAiScutTargetShutter(CCoeEnv* aEnv, TUid aAppUid)
    : CActive(CActive::EPriorityLow)
    , iEnv(aEnv)
    , iAppUid(aAppUid)
{
    CActiveScheduler::Add(this);
}


// ---------------------------------------------------------------------------
//
// ---------------------------------------------------------------------------
//
CAiScutTargetShutter::~CAiScutTargetShutter()
{
    if (iPeriodic)
    {
        iPeriodic->Cancel();
        delete iPeriodic;
    }

    iWsSession.Close();
    Cancel();
}

// ---------------------------------------------------------------------------
//
// ---------------------------------------------------------------------------
//
void CAiScutTargetShutter::ConstructL(TBool aIsRunning, TBool aIsDialog)
{
    iIsRunning = aIsRunning;
    iIsDialog = aIsDialog;
    iTaskExists = EFalse;
    iTaskKilled = EFalse;
    iCounter = 0;
    iPeriodic = CPeriodic::NewL(CActive::EPriorityLow);
    User::LeaveIfError(iWsSession.Connect());
}

// ---------------------------------------------------------------------------
//
// ---------------------------------------------------------------------------
//
CAiScutTargetShutter* CAiScutTargetShutter::NewL(CCoeEnv* aEnv, TUid aAppUid, TBool aIsRunning, TBool aIsDialog)
{
    CAiScutTargetShutter* self = new (ELeave) CAiScutTargetShutter(aEnv, aAppUid);
    CleanupStack::PushL(self);
    self->ConstructL(aIsRunning, aIsDialog);
    CleanupStack::Pop(self);
    return self;
}

// ---------------------------------------------------------------------------
//
// ---------------------------------------------------------------------------
//
void CAiScutTargetShutter::StartL()
{
	__PRINT( __DBG_FORMAT( "XAI: CAiScutTargetShutter::Start() app uid=0x%x"), iAppUid.iUid);
	

    TApaTaskList taskList(iWsSession);
    TApaTask idleTask(taskList.FindApp(KScutActiveIdleUid));

    if ( idleTask.Exists() && iIsRunning)
    {
        if ( iIsDialog )
            {
            TKeyEvent keyEvent;
            keyEvent.iCode = EKeyEscape;
            keyEvent.iModifiers = 0;
            keyEvent.iRepeats = 0;
            iEnv->SimulateKeyEventL(keyEvent, EEventKey);            
            }
        //idleTask.BringToForeground();
        CAknSgcClient::MoveApp(idleTask.WgId(), ESgcMoveAppToForeground);
        iTaskExists = ETrue;               
		__PRINTS( "XAI:   exists and running");

    }
    else
    {
        TKeyEvent keyEvent;
        keyEvent.iCode = EKeyEscape;
        keyEvent.iModifiers = 0;
        keyEvent.iRepeats = 0;
        iEnv->SimulateKeyEventL(keyEvent, EEventKey);

        // bring active idle to foreground
        TApaTask idleTask(taskList.FindApp(KScutActiveIdleUid));
        if (idleTask.Exists())
        {
			__PRINTS("XAI:   idle to foreground");
            //idleTask.BringToForeground();
            CAknSgcClient::MoveApp(idleTask.WgId(), ESgcMoveAppToForeground);
        }
        TApaTask task(taskList.FindApp(iAppUid));
		if (task.Exists())
        {
			//task.SendToBackground();
			CAknSgcClient::MoveApp(task.WgId(), ESgcMoveAppToBackground);
        }
    }
	if( !iPeriodic->IsActive() )
	{
	    if (iTaskExists)
    	{
            iPeriodic->Start(KTaskExistsDelay, KTaskExistsDelay,
                TCallBack(TaskExistsCallback, this));
    	}
    	else
	    {
            iPeriodic->Start(KTaskNotExistsDelay, KTaskNotExistsDelay,
                TCallBack(TaskNotExistsCallback, this));
    	}
		
	}
}

// ---------------------------------------------------------------------------
// From CActive
// ---------------------------------------------------------------------------
//
void CAiScutTargetShutter::DoCancel()
{
    if (iPeriodic)
    {
        iPeriodic->Cancel();
    }
}

// ---------------------------------------------------------------------------
// From CActive.
// ---------------------------------------------------------------------------
//
void CAiScutTargetShutter::RunL()
{
	__PRINT( __DBG_FORMAT( "XAI: CAiScutTargetShutter::RunL() app uid=0x%x"), iAppUid.iUid);	

	TUid menuUid = KMenuUID3;
	
    TApaTaskList taskList(iWsSession);
    TApaTask task(taskList.FindApp(iAppUid));

    if (iTaskExists)
    {
        RWindowGroup windowGroup = iEnv->RootWin();

        if (windowGroup.OrdinalPosition() != 0)
        {
            TApaTask idleTask(taskList.FindApp(KScutActiveIdleUid));
            if (idleTask.Exists())
            {
			__PRINTS( "XAI:   idle to foreground");
	
                //idleTask.BringToForeground();
                CAknSgcClient::MoveApp(idleTask.WgId(), ESgcMoveAppToForeground);
            }
        }
        iPeriodic->Cancel();
    }
    else
    {
        if (iCounter >= KMaxNumberOfTries || iTaskKilled)
        {
            iPeriodic->Cancel();
        }
        else if (task.Exists() && iAppUid != menuUid)        
        {
			__PRINTS("XAI:   shutdown task");
			//task.SendToBackground();
			CAknSgcClient::MoveApp(task.WgId(), ESgcMoveAppToBackground);
            task.SendSystemEvent(EApaSystemEventShutdown);
            iTaskKilled = ETrue;
        }
        else
        {
            iCounter++;
        }
    }
}

// ---------------------------------------------------------------------------
// From CActive
// Handles an error situation.
// ---------------------------------------------------------------------------
//
TInt CAiScutTargetShutter::RunError(TInt aError)
{
    return aError;
}

// ---------------------------------------------------------------------------
//
//
// ---------------------------------------------------------------------------
//
void CAiScutTargetShutter::Run()
{
    SetActive();
    TRequestStatus *status = &iStatus;
    User::RequestComplete(status, KErrNone);
}

// ---------------------------------------------------------------------------
//
// ---------------------------------------------------------------------------
//
TInt CAiScutTargetShutter::TaskExistsCallback(TAny* aPtr)
{
    CAiScutTargetShutter* self = static_cast<CAiScutTargetShutter*>(aPtr);

    if (self)
    {
        self->Run();
    }

    return KErrNone;
}

// ---------------------------------------------------------------------------
//
// ---------------------------------------------------------------------------
//
TInt CAiScutTargetShutter::TaskNotExistsCallback(TAny* aPtr)
{
    CAiScutTargetShutter* self = static_cast<CAiScutTargetShutter*>(aPtr);

    if (self)
    {
        self->Run();
    }

    return KErrNone;
}

// End of File.
