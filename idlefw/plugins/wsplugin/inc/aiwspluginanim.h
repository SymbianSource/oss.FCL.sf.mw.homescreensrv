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
* Description:  Active Idle Window Server Plug-in.
*
*/

#ifndef C_AIWSPLUGINANIM_H
#define C_AIWSPLUGINANIM_H

#include <w32adll.h>
#include "aiwspluginanimdef.h"
#include "uistate.h"
#include "modifierkeytracker.h"

namespace AiWsPlugin
    {
    class CEventHandler;
    }

typedef MAnimGeneralFunctionsWindowExtension::TWindowGroupInfo TIdPlgWindowGroupInfo;

/**
 *  @ingroup group_wsplugin
 *
 * Active Idle Window Server Plug-in.
 *
 * @since 3.2
 */
class CAiWsPluginAnim : 
        public CWindowAnim, public AiWsPlugin::MUiState
    {
    public: // Constructors and destructors
        
        /** 
        * Constructor. 
        */
        CAiWsPluginAnim();
        
        /** 
        * Destructor.
        */
        ~CAiWsPluginAnim();
        
// From base class CWindowAnim

        TInt CommandReplyL( TInt aOpcode, TAny* aArgs );
        
        void Command( TInt aOpcode, TAny* aArgs );
        
        void Animate( TDateTime* aDateTime );
        
        void ConstructL( TAny* aArgs, TBool aHasFocus );
        
        void Redraw();
        
        void FocusChanged( TBool aState );
        
        TBool OfferRawEvent( const TRawEvent& aRawEvent );

// From base class MUiState

        TUint Modifiers() const;
        
        TBool HasFocus() const;
    
    private:  // Implementation
    
        void AddEventHandlerAndPopL( AiWsPlugin::CEventHandler* aEventHandler );
    
    private:  // Data

        /// Event handlers. Own.
        RPointerArray< AiWsPlugin::CEventHandler > iEventHandlers;

        /// Window group ids passed as construction parameters
        TAiWsPluginAnimInitData iWgInfo;
        
        /// Active Idle Focus status
        TBool iAiFocused;
        
        /// Modifier key tracker
        AiWsPlugin::TModifierKeyTracker iModifierTracker;
    };

#endif // C_AIWSPLUGINANIM_H

