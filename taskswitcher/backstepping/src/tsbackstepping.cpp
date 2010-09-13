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

#include "tsbackstepping.h"

#include <w32std.h>// key event
#include <apgwgnam.h>// for CApaWindowGroupName
#include <apgtask.h>
#include <coedef.h>
#include <e32property.h>
#include <e32debug.h>

#include <homescreendomainpskeys.h>
#include <afactivitylauncher.h>

#include "tsbacksteppingfilter.h"

_LIT(KHsActivactionUri, "appto://20022F35?activityname=HsIdleView&activityinbackground=true");

// -----------------------------------------------------------------------------
/** 
 *  CTsBackstepping::NewL
 *  two phase constructor
 */
CTsBackstepping* CTsBackstepping::NewL( MTsWindowGroupsMonitor& aMonitor )
    {
    CTsBackstepping* self = CTsBackstepping::NewLC( aMonitor );
    CleanupStack::Pop( self );
    return self;
    }

// -----------------------------------------------------------------------------
/** 
 *  CTsBackstepping::NewLC
 *  two phase constructor
 */
CTsBackstepping* CTsBackstepping::NewLC( MTsWindowGroupsMonitor& aMonitor )
    { 
    CTsBackstepping* self = new (ELeave) CTsBackstepping( aMonitor );
    CleanupStack::PushL( self );
    self->ConstructL();
    return self;
    }

// -----------------------------------------------------------------------------
/** 
 * CTsBackstepping::CTsBackstepping
 * constructor
 */
CTsBackstepping::CTsBackstepping( MTsWindowGroupsMonitor &aMonitor )
:
CTsWindowGroupsObserver( aMonitor )
    {
    }

// -----------------------------------------------------------------------------
/** 
 *  CTsBackstepping::~CTsBackstepping
 *  deconstructor
 */
CTsBackstepping::~CTsBackstepping()
    {
    delete iFilter;
    }

// -----------------------------------------------------------------------------
/** 
 *  CTsBackstepping::ConstructL
 *  two phase constructor
 */
void CTsBackstepping::ConstructL ()
    {
    BaseConstructL();
    iFilter = CTsBacksteppingFilter::NewL();
    }

// -----------------------------------------------------------------------------
/** 
 * CTsBackstepping::AnalyseWindowStackL
 * Analyzes window stack and move homescreen to proper position
 */
void CTsBackstepping::HandleWindowGroupChanged( 
               MTsResourceManager &aResource, 
               const TArray<RWsSession::TWindowGroupChainInfo> &aWindowGroups )
    {
    RDebug::Print(_L("[Backstepping] started"));
    TRAP_IGNORE( HandleWindowGroupChangedL( aResource, aWindowGroups ) );
    RDebug::Print(_L("[Backstepping] finished"));
    }

// -----------------------------------------------------------------------------
/** 
 * CTsBackstepping::AnalyseWindowStackL
 * Analyzes window stack and move homescreen to proper position
 */
void CTsBackstepping::HandleWindowGroupChangedL( 
                MTsResourceManager& aResource,
                const TArray<RWsSession::TWindowGroupChainInfo>& aWindowGroups )
    {
    // calculate the desired position of Homescreen
    const TInt currentHsOffset( HomescreenOffsetL( aResource, aWindowGroups ) );
    TInt optimalOffset(1);
    TInt targetHsOffset(currentHsOffset);
    for( TInt offset(0); offset < currentHsOffset; ++offset ) 
        {
        TUid uid = GetUidFromWindowGroupL( aResource, 
                                           aWindowGroups[offset].iId );
        RDebug::Print(_L("[Backstepping] application: %d"), uid.iUid);
                                           
        if( TUid::Null() != uid )
            {
            if( IsEmbededApp( aWindowGroups[offset] ) )
                {
                targetHsOffset = ParentOffsetL( offset, aWindowGroups ) + 1;
                } 
            else if (!iFilter->isBlocked(uid))
                {
                if(offset + 1 < currentHsOffset && 
                   GetUidFromWindowGroupL( aResource, aWindowGroups[offset+1].iId) == TUid::Null() ) 
                    {
                    ++optimalOffset;
                    }
                targetHsOffset = optimalOffset;
                }
            break;
            } 
        else
            {
            ++optimalOffset;
            }
        }

    // switch Homescreen to Idle state if Homescreen is not in foreground and is in different state
    if( 0 != targetHsOffset)
        {
        TInt hsState( EHomeScreenIdleState );
        User::LeaveIfError(RProperty::Get( KHsCategoryUid, 
                                           KHsCategoryStateKey, 
                                           hsState ) );
        if(!(hsState & EHomeScreenWidgetViewActive ) )
            {
            RDebug::Print(_L("[Backstepping] about to switch HS activity"));
            
            CAfActivityLauncher *activityEnabler = 
                CAfActivityLauncher::NewLC( aResource.ApaSession(), 
                                            aResource.WsSession() );
            activityEnabler->launchActivityL( KHsActivactionUri );
            CleanupStack::PopAndDestroy( activityEnabler );
            }
        }
    // change windows order if necessary
    if(targetHsOffset != currentHsOffset)
        {
        RDebug::Print(_L("[Backstepping] about to change window groups order"));
        
        const TInt hsWindowGroup( aWindowGroups[currentHsOffset].iId );
        aResource.WsSession().SetWindowGroupOrdinalPosition( hsWindowGroup, 
                                                             targetHsOffset );
        }
    }

// -----------------------------------------------------------------------------
/** 
 * CTsBackstepping::HomescreenOffsetL
 */
TInt CTsBackstepping::HomescreenOffsetL( 
        MTsResourceManager& aResource,
        const TArray<RWsSession::TWindowGroupChainInfo>& aWindowGroups ) const 
    {
    TInt offset( KErrNotFound );
    for( TInt iter(0); KErrNotFound == offset && iter < aWindowGroups.Count(); ++iter )
        {
        if( KHsCategoryUid == GetUidFromWindowGroupL( aResource, aWindowGroups[iter].iId ) )
            {
            offset = iter;
            }
        }
    User::LeaveIfError( offset );
    return offset;
    }

// -----------------------------------------------------------------------------
/** 
 * CTsBackstepping::ParentOffsetL
 */
TInt CTsBackstepping::ParentOffsetL( 
        TInt aOffset,
        const TArray<RWsSession::TWindowGroupChainInfo>& aWindowGroups )const 
    {
    for( TInt iter(aOffset + 1); iter < aWindowGroups.Count(); ++iter )
        {
        if(aWindowGroups[iter].iId == aWindowGroups[aOffset].iParentId)
            {
            return IsEmbededApp( aWindowGroups[iter] ) ? 
                   ParentOffsetL( iter, aWindowGroups ) : iter; 
            }
        }
    User::Leave( KErrNotFound) ;
    return KErrNotFound;
    }

// -----------------------------------------------------------------------------
/** 
 * CTsBackstepping::IsEmbededApp
 */
TBool CTsBackstepping::IsEmbededApp( 
            const RWsSession::TWindowGroupChainInfo &aWindowGroupInfo ) const 
    {
    return 0 < aWindowGroupInfo.iParentId;
    }

// -----------------------------------------------------------------------------
/** 
 * CTsBackstepping::GetUidFromWindowGroupL
 */
TUid CTsBackstepping::GetUidFromWindowGroupL( MTsResourceManager &aResource, 
                                              TInt aWindowGroupId ) const
    {
    TUid retVal(TUid::Null());
    CApaWindowGroupName *windowGroupName = 
            CApaWindowGroupName::NewLC( aResource.WsSession() );
    windowGroupName->ConstructFromWgIdL( aWindowGroupId );
    retVal = windowGroupName->AppUid();
    CleanupStack::PopAndDestroy( windowGroupName );
    return retVal;
    }

// end of file
