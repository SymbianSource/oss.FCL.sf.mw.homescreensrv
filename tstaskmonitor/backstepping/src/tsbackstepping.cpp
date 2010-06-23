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


/** 
 *  CTsBackstepping::NewL
 *  two phase constructor
 */
EXPORT_C CTsBackstepping* CTsBackstepping::NewL(MTsWindowGroupsMonitor &monitor)
  {
  RDebug::Print(_L("CTsBackstepping::NewL"));
  CTsBackstepping* self = CTsBackstepping::NewLC(monitor);
  CleanupStack::Pop(self);
  return self;
  }

/** 
 *  CTsBackstepping::NewLC
 *  two phase constructor
 */
EXPORT_C CTsBackstepping* CTsBackstepping::NewLC(MTsWindowGroupsMonitor &monitor)
{ 
  CTsBackstepping* self = new (ELeave) CTsBackstepping(monitor);
  CleanupStack::PushL(self);
  self->ConstructL();
  return self;
}

/** 
 * CTsBackstepping::CTsBackstepping
 * constructor
 */
CTsBackstepping::CTsBackstepping(MTsWindowGroupsMonitor &monitor)
:
CTsWindowGroupsObserver(monitor)
{
}


/** 
 *  CTsBackstepping::~CTsBackstepping
 *  deconstructor
 */
EXPORT_C CTsBackstepping::~CTsBackstepping()
{
}

/** 
 *  CTsBackstepping::ConstructL
 *  two phase constructor
 */
void CTsBackstepping::ConstructL ()
{
    RDebug::Print(_L("CTsBackstepping::ConstructL"));
    BaseConstructL();
}

/** 
 * CTsBackstepping::AnalyseWindowStackL
 * Analyzes window stack and move homescreen to proper position
 */
void CTsBackstepping::HandleWindowGroupChanged(MTsResourceManager &resource, const TArray<RWsSession::TWindowGroupChainInfo> &windowGroups)
{
    TRAP_IGNORE(HandleWindowGroupChangedL(resource, windowGroups));
}

/** 
 * CTsBackstepping::AnalyseWindowStackL
 * Analyzes window stack and move homescreen to proper position
 */
void CTsBackstepping::HandleWindowGroupChangedL(MTsResourceManager &resource,
                                                const TArray<RWsSession::TWindowGroupChainInfo> &windowGroups)
{  
    RDebug::Print(_L("CTsBackstepping::GetWindowCaption"));
    
    CApaWindowGroupName *windowGroupName = CApaWindowGroupName::NewLC(resource.WsSession());
    
    TInt count(windowGroups.Count());
    TInt pos(0);
    TInt whereToJump(1);
    for (TInt i=0; i<count; i++) {//iterate through list and give debug info
        windowGroupName->ConstructFromWgIdL(windowGroups[i].iId);
        if (windowGroupName->AppUid() != TUid::Null()) {
            // find the window group id and check that it has no parent
            if ( windowGroups[i].iParentId <= 0 ) {
                RDebug::Print( _L("CTsBackstepping::GetWindowCaption wgid:%d is standalone view"), windowGroups[i].iId);
                //check if it is homescreen 
                if (windowGroupName->AppUid() == KHSUid) {
                    RDebug::Print(_L("CTsBackstepping::GetWindowCaption Homescreen position = %d ; list:%d/%d"), pos, i, count);
                    if (pos <= 1) {// HS is in foreground or second in line - do nothing 
                        RDebug::Print(_L("CTsBackstepping::nothing to be done - it will stop checking here"));
                    } else {//we should move homescreen to be second (ommit embeded views) from top
                        RDebug::Print(_L("CTsBackstepping::moving homescreen to be second from top"));
                        RDebug::Print(_L("CTsBackstepping::whereToJump = %d"), whereToJump);
                        resource.WsSession().SetWindowGroupOrdinalPosition(windowGroups[i].iId, whereToJump);
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
                RDebug::Print(_L("CTsBackstepping::GetWindowCaption wgid:%d is embedded view"), windowGroups[i].iId);
            }
        }
    }
    CleanupStack::PopAndDestroy(windowGroupName);
}

// end of file
