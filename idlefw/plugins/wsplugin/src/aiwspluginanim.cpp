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


#include "aiwspluginanim.h"
#include "aiwspluginanimdef.h"
#include "keylockhandler.h"
#include "numerickeyhandler.h"
#include "logslaunchhandler.h"
#include "sindlaunchhandler.h"

#include <featmgr.h>

using namespace AiWsPlugin;

CAiWsPluginAnim::CAiWsPluginAnim()
    {
    }

CAiWsPluginAnim::~CAiWsPluginAnim()
    {
    iEventHandlers.ResetAndDestroy();
    if ( iFunctions )
        {
        iFunctions->GetRawEvents( EFalse );
        }
    FeatureManager::UnInitializeLib();
    }

TInt CAiWsPluginAnim::CommandReplyL( TInt /*aOpcode*/, TAny* /*aArgs*/ )
    {
    return KErrNone;
    }

void CAiWsPluginAnim::Command( TInt /*aOpcode*/, TAny* /*aArgs*/ )
    {
    }

void CAiWsPluginAnim::Animate( TDateTime* /*aDateTime*/ )
    {
    }

void CAiWsPluginAnim::AddEventHandlerAndPopL( CEventHandler* aEventHandler )
    {
    if( aEventHandler )
        {
        aEventHandler->SetUiStateQuery( *this );
	    iEventHandlers.AppendL( aEventHandler );
	    CleanupStack::Pop( aEventHandler );
        }
    }

void CAiWsPluginAnim::ConstructL( TAny* aArgs, TBool aHasFocus )
    {
    FeatureManager::InitializeLibL();
    
    iFunctions->GetRawEvents( ETrue );
    if( !aArgs )
        {
        User::Leave( KErrArgument );
        }
    iWgInfo = *( static_cast<TAiWsPluginAnimInitData*>(aArgs) );
    
    AddEventHandlerAndPopL( CKeyLockHandler::NewLC() );

    MAnimGeneralFunctionsWindowExtension* ext = reinterpret_cast<MAnimGeneralFunctionsWindowExtension*>
        ( iFunctions->ExtendedInterface( 
         MAnimGeneralFunctions::EWindowExtensionInterface ) );
    AddEventHandlerAndPopL( CNumericKeyHandler::NewLC( iWgInfo.iTargetWgId, ext ) );
	
	AddEventHandlerAndPopL( CLogsLaunchHandler::NewLC() );
	
/* Leave this commented code here for now.. 2.5.2007, unclear if needed still in some config.
	if ( FeatureManager::FeatureSupported( KFeatureIdKeypadNoVoiceKey ) )
	    {
	    AddEventHandlerAndPopL( CSINDLaunchHandler::NewLC() );       
	    }	
*/
    // Update initial focus status
    FocusChanged( aHasFocus );
    }

void CAiWsPluginAnim::Redraw()
    {
    }

void CAiWsPluginAnim::FocusChanged( TBool aState )
    {
    iAiFocused = aState;
    const TInt handlerCount = iEventHandlers.Count();
    for( TInt i = 0; i < handlerCount; ++i )
        {
        iEventHandlers[i]->FocusChanged( aState );
        }
    }

TBool CAiWsPluginAnim::OfferRawEvent( const TRawEvent& aRawEvent )
    {
    // Forward event to all event handlers
    const TInt handlerCount = iEventHandlers.Count();
    for( TInt i = 0; i < handlerCount; ++i )
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
	
TUint CAiWsPluginAnim::Modifiers() const
    {
    return iModifierTracker.Status();
    }
    
TBool CAiWsPluginAnim::HasFocus() const
    {
    return iAiFocused;
    }
	
