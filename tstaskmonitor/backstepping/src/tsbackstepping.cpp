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

#include <w32std.h>// key event
#include <apgwgnam.h>// for CApaWindowGroupName
#include <apgtask.h>
#include <coedef.h>
#include <f32file.h>
#include <flogger.h>
#include <e32property.h>
#include <startupdomainpskeys.h>

#include "tsbackstepping.h"

const TUid KHSUid = {0x20022f35};
const int KOrdinalPositionNoZOrder(-1);


/** 
 *  CTsBackstepping::NewL
 *  two phase constructor
 */
EXPORT_C CTsBackstepping* CTsBackstepping::NewL(RWsSession &session)
  {
  RDebug::Print(_L("CTsBackstepping::NewL"));
  CTsBackstepping* self = CTsBackstepping::NewLC(session);
  CleanupStack::Pop(self);
  return self;
  }

/** 
 *  CTsBackstepping::NewLC
 *  two phase constructor
 */
EXPORT_C CTsBackstepping* CTsBackstepping::NewLC(RWsSession &session)
{ 
  CTsBackstepping* self = new (ELeave) CTsBackstepping(session);
  CleanupStack::PushL(self);
  self->ConstructL();
  return self;
}

/** 
 * CTsBackstepping::CTsBackstepping
 * constructor
 */
CTsBackstepping::CTsBackstepping(RWsSession &session)
:CActive(EPriorityStandard),
mWsSession(session)
{
    CActiveScheduler::Add(this);
}


/** 
 *  CTsBackstepping::~CTsBackstepping
 *  deconstructor
 */
EXPORT_C CTsBackstepping::~CTsBackstepping()
{
    RDebug::Print(_L("CTsBackstepping::~CTsBackstepping"));
    // Cancel AO
    Cancel();
    // Close opened session
    mWg.Close();
    
}

/** 
 *  CTsBackstepping::ConstructL
 *  two phase constructor
 */
void CTsBackstepping::ConstructL ()
{
    RDebug::Print(_L("CTsBackstepping::ConstructL"));
    // Initial window group
    mWg = RWindowGroup (mWsSession);
    User::LeaveIfError (mWg.Construct ((TUint32)&mWg, EFalse));
    mWg.SetOrdinalPosition (KOrdinalPositionNoZOrder);
    mWg.EnableReceiptOfFocus (EFalse);

    // Hide window
    CApaWindowGroupName* wn = CApaWindowGroupName::NewLC (mWsSession);
    wn->SetHidden (ETrue);
    wn->SetWindowGroupName (mWg);
    CleanupStack::PopAndDestroy (wn);

    // Window group change event
    User::LeaveIfError (mWg.EnableGroupListChangeEvents());
    
    TRAP_IGNORE(AnalyseWindowStackL());//not critical operation
    Subscribe();
}

/** 
 *  CTsBackstepping::RunL
 *  called for handling events from window server
 */
void CTsBackstepping::RunL()
{
    User::LeaveIfError(iStatus.Int());
    TWsEvent wsEvent;
    mWsSession.GetEvent (wsEvent);
    if (EEventWindowGroupListChanged == wsEvent.Type()) {
        RDebug::Print(_L("CTsBackstepping::RunL : EEventWindowGroupListChanged"));
        AnalyseWindowStackL ();
    }
    Subscribe();
}

/** 
 *  CTsBackstepping::DoCancel
 *  Handling RunL errors.
 */
TInt CTsBackstepping::RunError(TInt error)
{
    if (!IsActive() && KErrCancel != error) {
        Subscribe();
    }
    return KErrNone;
}

/** 
 *  CTsBackstepping::DoCancel
 *  Stopping active object
 */
void CTsBackstepping::DoCancel ()
{
    if (IsActive()) {
        mWsSession.EventReadyCancel();
    }
}

/** 
 *  CTsBackstepping::ActivateListeningL
 *  Starts listening to Window session events
 */
void CTsBackstepping::Subscribe()
{
    RDebug::Print(_L("CTsBackstepping::Subscribe"));
    // and start listening
    iStatus = KRequestPending;
    mWsSession.EventReady( &iStatus );
    SetActive();
}

/** 
 * CTsBackstepping::AnalyseWindowStackL
 * Analyzes window stack and move homescreen to proper position
 */
void CTsBackstepping::AnalyseWindowStackL ()
{  
    RDebug::Print(_L("CTsBackstepping::GetWindowCaption"));
    
    RArray<RWsSession::TWindowGroupChainInfo> windowGroups;
    CleanupClosePushL(windowGroups);
    
    CApaWindowGroupName *windowGroupName = CApaWindowGroupName::NewLC(mWsSession );
    //update window group list
    mWsSession.WindowGroupList( &windowGroups );
    
    TInt count(windowGroups.Count());
    TInt pos(0);
    TInt whereToJump(1);
    for (TInt i=0; i<count; i++) {//iterate through list and give debug info
        const RWsSession::TWindowGroupChainInfo& info = windowGroups[i];
        windowGroupName->ConstructFromWgIdL(info.iId);
        if (windowGroupName->AppUid() != TUid::Null()) {
            // find the window group id and check that it has no parent
            if ( info.iParentId <= 0 ) {
                RDebug::Print( _L("CTsBackstepping::GetWindowCaption wgid:%d is standalone view"), info.iId);
                //check if it is homescreen 
                if (windowGroupName->AppUid() == KHSUid) {
                    RDebug::Print(_L("CTsBackstepping::GetWindowCaption Homescreen position = %d ; list:%d/%d"), pos, i, count);
                    if (pos <= 1) {// HS is in foreground or second in line - do nothing 
                        RDebug::Print(_L("CTsBackstepping::nothing to be done - it will stop checking here"));
                    } else {//we should move homescreen to be second (ommit embeded views) from top
                        RDebug::Print(_L("CTsBackstepping::moving homescreen to be second from top"));
                        RDebug::Print(_L("CTsBackstepping::whereToJump = %d"), whereToJump);
                        mWsSession.SetWindowGroupOrdinalPosition(info.iId, whereToJump);
                    }
                    // and break
                    break;
                }
                pos++;
            } else {
                // only embeded items for first standalone launch should be counted
                if (!pos) {
                    ++whereToJump;
                }
                RDebug::Print(_L("CTsBackstepping::GetWindowCaption wgid:%d is embedded view"), info.iId);
            }
        }
    }
    CleanupStack::PopAndDestroy(windowGroupName);
    CleanupStack::PopAndDestroy(&windowGroups);
}

// end of file
