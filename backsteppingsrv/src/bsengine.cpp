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
* Description:  Engine for BS Engine
 *
*/


#include <apgtask.h>
#include <apgwgnam.h>
#include <eikenv.h>
#include <e32base.h>

#include "bsengineglobals.h"
#include "bsdebug.h"
#include "bsengine.h"
#include "bsapplicationinfo.h"
#include "bsconfiguration.h"

// CONSTANTS
const TInt KSecondTaskIndex( 1);

// ================= MEMBER FUNCTIONS =======================

// -----------------------------------------------------------------------------
// C++ default constructor can NOT contain any code, that might leave.
// -----------------------------------------------------------------------------
//
CBSEngine::CBSEngine()
    {
    iEnv = CEikonEnv::Static( );
    swap = EFalse;
    wasPressed = EFalse;
    }

// -----------------------------------------------------------------------------
// Symbian 2nd phase constructor can leave.
// -----------------------------------------------------------------------------
//
void CBSEngine::ConstructL()
    {
    DEBUG(("Create Engine"));
    iConfiguration = CBSConfiguration::NewL( );
    }

// -----------------------------------------------------------------------------
// Destructor.
// -----------------------------------------------------------------------------
//
CBSEngine::~CBSEngine()
    {
    DEBUG(("~CBSEngine"));
    iAppsStack.ResetAndDestroy( );
    iFocusHistory.Close( );

    delete iConfiguration;
    }

// -----------------------------------------------------------------------------
// Two-phased constructor.
// -----------------------------------------------------------------------------
//
CBSEngine* CBSEngine::NewL()
    {
    CBSEngine* self = CBSEngine::NewLC( );
    CleanupStack::Pop( self ) ;
    return self;
    }

// -----------------------------------------------------------------------------
// Two-phased constructor.
// -----------------------------------------------------------------------------
//
CBSEngine* CBSEngine::NewLC()
    {
    CBSEngine* self = new ( ELeave ) CBSEngine();
    CleanupStack::PushL( self );
    self->ConstructL( ) ;
    return self;
    }

// -----------------------------------------------------------------------------
// 
// -----------------------------------------------------------------------------
//
void CBSEngine::ApplicationKeyWasPressed()
    {
    wasPressed = ETrue;
    }

// -----------------------------------------------------------------------------
// 
// -----------------------------------------------------------------------------
//
void CBSEngine::HandleFocusChangeL( const TUid& aApp )
    {
    DEBUG(("-> HandleFocusChangeL(0x%X)", aApp.iUid ));
    // check if application is not in ignored list
    if ( iConfiguration->IsIgnored( aApp ) )
        {
        DEBUG(("\tIgnore the event - application in ignored list"));
        return;
        }

    TInt fsCount = iFocusHistory.Count( );
    if ( fsCount > 0 && iFocusHistory[fsCount - 1] == aApp.iUid )
        {
        DEBUG(("\tIgnore the event - application already on top"));
        swap = EFalse;
        wasPressed = EFalse;
        return;
        }

    // check if we should not reset back stepping stack
    if ( iConfiguration->IsReset( aApp ) )
        {
        swap = EFalse;
        wasPressed = EFalse;
        ResetHistory( );
        }

    // check if we should reset if application was started tru 
    RArray<TInt>& thuApps = iConfiguration->ResetIfThruList( );

    for ( TInt i = 0; i < thuApps.Count( ); i++ )
        {
        // position of application - i.e. fast swap
        if ( aApp.iUid == thuApps[i] )
            {
            //mark that there is a fast swap or dialog
            swap = ETrue;
            return;
            }
        else
            {
            if ( swap )
                {
                swap = EFalse;
                TInt currentApp = aApp.iUid;
                TInt pos = iFocusHistory.Count( ) - 1;
                if ( pos >= 0 )
                    {
                    TInt prevApp = iFocusHistory[pos];
                    if ( currentApp != prevApp && wasPressed )
                        {
                        //we are here as the result of the fast swap
                        ResetHistory( );
                        iFocusHistory.AppendL( thuApps[i] );
                        wasPressed = EFalse;
                        }
                    }
                }
            }
        }
    // add application to focus history list, 
    iFocusHistory.AppendL( aApp.iUid );
    }

// -----------------------------------------------------------------------------
// 
// -----------------------------------------------------------------------------
//
TInt CBSEngine::HandleActivationEventL( const TUid& aApp,
    const TDesC8& aState, TBool aItem )
    {
    DEBUG(("-> HandleActivationEventL(0x%X, %S, 0x%X)",
                    aApp.iUid, &aState, aItem));
    TInt result(EBSEngineCommandWasNotConsumed);

#ifdef _DEBUG    
    // check from which application it was started 
    for ( TInt i = 0; i< iFocusHistory.Count( ); i++ )
        {
        DEBUG(("\tiFocusHistory[%d] = %X)", i, iFocusHistory[i]));
        }
#endif        

    // is it the same application - i.e. internal state change - ignore       
    CBSApplicationInfo* last(NULL);
    if ( iAppsStack.Count( ) )
        last = iAppsStack[iAppsStack.Count() - 1];

    if ( last && last->AppUid( ) == aApp )
        {
        DEBUG(("\tIgnore the event, application is already on stack"));
        }
    else if ( aItem ) // store only item events
        {
        CBSApplicationInfo* newApp = CBSApplicationInfo::NewLC();
        newApp->SetPrevAppUid( FindPreviousApp( aApp ) );
        newApp->SetAppUid( aApp );
        newApp->SetItemStateL( aState );

        DEBUG(("\tAdd to BS stack app:0x%X, state:%S, prevapp:0x%X)",
                        newApp->AppUid().iUid,
                        &newApp->ItemState(),
                        newApp->PrevAppUid().iUid));
        iAppsStack.AppendL( newApp );
        CleanupStack::Pop( newApp );
        result = EBSEngineCommandWasConsumed;
        }

    return result;
    }

// -----------------------------------------------------------------------------
// 
// -----------------------------------------------------------------------------
//
TInt CBSEngine::HandleBackEventL( const TUid& aApp, const TDesC8& aState,
    TBool aCheckOnly )
    {
    DEBUG(("-> HandleBackEventL(0x%X, %S, 0x%X )",
                    aApp.iUid, &aState, aCheckOnly ));
    TInt result(EBSEngineCommandWasNotConsumed);
    // get last application at stack
    CBSApplicationInfo* info(NULL);
    if ( iAppsStack.Count( ) )
        {
        info = iAppsStack[iAppsStack.Count() - 1];

        DEBUG(("\tcheck UIDs (0x%X-0x%X)", aApp.iUid, info->AppUid() ));
        DEBUG(("\tcheck states (%S-%S)", &aState, &info->ItemState()));
        if ( info && info->AppUid( ) == aApp && info->ItemState().Compare( aState ) == 0 )
            {
            DEBUG(("\tcheck prev app, if BS to be ignored = %d)",
                            iConfiguration->IsIgnoredIfStartedFrom( info->PrevAppUid() ) ));

            // check what application activated the application in question 
            if ( !iConfiguration->IsIgnoredIfStartedFrom( info->PrevAppUid( ) ) )
                {
                // find the task which is directly below the current one
                // in window group Z-order hierarchy
                TApaTaskList taskList(iEnv->WsSession( ) );
                // current task is in "0" position, next one is "1"
                // NOTE: FindByPos doesn't return hidden tasks
                TApaTask lowerTask = taskList.FindByPos( KSecondTaskIndex );
                CApaWindowGroupName* apaWGName = CApaWindowGroupName::NewLC(
                    iEnv->WsSession( ), lowerTask.WgId( ) );
                TUid lowerTaskUid = apaWGName->AppUid( );
                CleanupStack::PopAndDestroy( apaWGName );

                DEBUG(("\tcheck task below (0x%X-0x%X)",
                                lowerTaskUid.iUid, info->PrevAppUid().iUid));
                // if the lower task is the one that activated the current one
                // send the current one to background.
                // the condition is false if i.e. the previous active task
                // has been closed or sent to background (no explicit
                // activation of the current task occured)
                if ( lowerTaskUid == info->PrevAppUid( ) )
                    {
                    if ( !aCheckOnly )
                        {
                        SendToBackground( aApp );
                        // remove history for the application    
                        RemoveAppRecord( aApp );
                        }
                    result = EBSEngineCommandWasConsumed;
                    }
                }
            }
        }
    return result;
    }

// -----------------------------------------------------------------------------
// 
// -----------------------------------------------------------------------------
//
void CBSEngine::SendToBackground( const TUid& aApp )
    {
    DEBUG(("-> SendToBackgroundL(0x%X)", aApp.iUid ));

    TApaTaskList taskList(iEnv->WsSession( ));

    TApaTask task = taskList.FindApp( aApp );
    if ( task.Exists( ) )
        {
        // Request window server to send application to background
        task.SendToBackground( );
        }
    }

// -----------------------------------------------------------------------------
// 
// -----------------------------------------------------------------------------
//    
TUid CBSEngine::FindPreviousApp( const TUid& aApp )
    {
    TUid result(TUid::Null( ));

    for ( TInt i = iFocusHistory.Count( ) - 1; i >= 0; i-- )
        {
        if ( iFocusHistory[i] != aApp.iUid )
            {
            result = TUid::Uid( iFocusHistory[i] );
            break;
            }
        }

    return result;
    }

// -----------------------------------------------------------------------------
// 
// -----------------------------------------------------------------------------
//
void CBSEngine::RemoveAppRecord( const TUid& aApp )
    {
    DEBUG(("-> RemoveRecord(0x%X)", aApp.iUid ));

    for ( TInt i = iAppsStack.Count( ) - 1; i >= 0; i-- )
        {
        CBSApplicationInfo* info = iAppsStack[i];
        if ( info->AppUid( ) == aApp )
            {
            DEBUG(("\tRemove item - [%d]", i));
            iAppsStack.Remove( i );
            delete info;
            break;
            }
        }
    }

// -----------------------------------------------------------------------------
// 
// -----------------------------------------------------------------------------
//
void CBSEngine::ResetHistory()
    {
    DEBUG(("-> ResetHistory"));
    iAppsStack.ResetAndDestroy( );
    iFocusHistory.Reset( );
    }

// End of File


