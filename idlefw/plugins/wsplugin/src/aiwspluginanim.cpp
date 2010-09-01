/*
* Copyright (c) 2005-2007 Nokia Corporation and/or its subsidiary(-ies).
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
* Description:  Active Idle Window Server Plug-in implementation.
*
*/


// System includes
#include <featmgr.h>

// User includes
#include "aiwspluginanim.h"
#include "aiwspluginanimdef.h"
#include "numerickeyhandler.h"


using namespace AiWsPlugin;

// ======== MEMBER FUNCTIONS ========

// ---------------------------------------------------------------------------
// CAiWsPluginAnim::CAiWsPluginAnim
//
// ---------------------------------------------------------------------------
//
CAiWsPluginAnim::CAiWsPluginAnim()
    {
    }

// ---------------------------------------------------------------------------
// CAiWsPluginAnim::~CAiWsPluginAnim
//
// ---------------------------------------------------------------------------
//
CAiWsPluginAnim::~CAiWsPluginAnim()
    {
    iEventHandlers.ResetAndDestroy();
    
    if ( iFunctions )
        {
        iFunctions->GetRawEvents( EFalse );
        }
    
    FeatureManager::UnInitializeLib();
    }

// ---------------------------------------------------------------------------
// CAiWsPluginAnim::CommandReplyL
//
// ---------------------------------------------------------------------------
//
TInt CAiWsPluginAnim::CommandReplyL( TInt /*aOpcode*/, TAny* /*aArgs*/ )
    {
    return KErrNone;
    }

// ---------------------------------------------------------------------------
// CAiWsPluginAnim::Command
//
// ---------------------------------------------------------------------------
//
void CAiWsPluginAnim::Command( TInt /*aOpcode*/, TAny* /*aArgs*/ )
    {
    }

// ---------------------------------------------------------------------------
// CAiWsPluginAnim::Animate
//
// ---------------------------------------------------------------------------
//
void CAiWsPluginAnim::Animate( TDateTime* /*aDateTime*/ )
    {
    }

// ---------------------------------------------------------------------------
// CAiWsPluginAnim::AddEventHandlerAndPopL
//
// ---------------------------------------------------------------------------
//
void CAiWsPluginAnim::AddEventHandlerAndPopL( CEventHandler* aEventHandler )
    {
    if ( aEventHandler )
        {
        aEventHandler->SetUiStateQuery( *this );
	    iEventHandlers.AppendL( aEventHandler );
	    CleanupStack::Pop( aEventHandler );
        }
    }

// ---------------------------------------------------------------------------
// CAiWsPluginAnim::ConstructL
//
// ---------------------------------------------------------------------------
//
void CAiWsPluginAnim::ConstructL( TAny* aArgs, TBool aHasFocus )
    {
    if ( !aArgs )
        {
        User::Leave( KErrArgument );
        }
    
    FeatureManager::InitializeLibL();
    
    iFunctions->GetRawEvents( ETrue );
       
    iWgInfo = *( static_cast< TAiWsPluginAnimInitData* >( aArgs ) );
        
    // AddEventHandlerAndPopL( CKeyLockHandler::NewLC() );
    
    MAnimGeneralFunctionsWindowExtension* ext = 
        reinterpret_cast<MAnimGeneralFunctionsWindowExtension*>
            ( iFunctions->ExtendedInterface( 
                MAnimGeneralFunctions::EWindowExtensionInterface ) ); 
         
    AddEventHandlerAndPopL( 
        CNumericKeyHandler::NewLC( iWgInfo.iTargetWgId, ext ) );
	
    // Update initial focus status
    FocusChanged( aHasFocus );
    }

// ---------------------------------------------------------------------------
// CAiWsPluginAnim::Redraw
//
// ---------------------------------------------------------------------------
//
void CAiWsPluginAnim::Redraw()
    {
    }

// ---------------------------------------------------------------------------
// CAiWsPluginAnim::FocusChanged
//
// ---------------------------------------------------------------------------
//
void CAiWsPluginAnim::FocusChanged( TBool aState )
    {
    iAiFocused = aState;
    
    const TInt count( iEventHandlers.Count() );
    
    for ( TInt i = 0; i < count; ++i )
        {
        iEventHandlers[i]->FocusChanged( aState );
        }
    }

// ---------------------------------------------------------------------------
// CAiWsPluginAnim::OfferRawEvent
//
// ---------------------------------------------------------------------------
//
TBool CAiWsPluginAnim::OfferRawEvent( const TRawEvent& aRawEvent )
    {
    // Forward event to all event handlers
    const TInt count( iEventHandlers.Count() );
    
    for ( TInt i = 0; i < count; ++i )
        {
        // All other are made to return EFalse
        // Except wait for 2ndkeyup and 2ndkeydown of keylock states.
        if( iEventHandlers[i]->OfferRawEvent( aRawEvent ) )
            {
            return ETrue;
            }
        }
    
    // Update modifier key status
    iModifierTracker.Update( aRawEvent );
    
    return EFalse;
    }

// ---------------------------------------------------------------------------
// CAiWsPluginAnim::Modifiers
//
// ---------------------------------------------------------------------------
//
TUint CAiWsPluginAnim::Modifiers() const
    {
    return iModifierTracker.Status();
    }
    
// ---------------------------------------------------------------------------
// CAiWsPluginAnim::HasFocus
//
// ---------------------------------------------------------------------------
//
TBool CAiWsPluginAnim::HasFocus() const
    {
    return iAiFocused;
    }
	
// End of file
