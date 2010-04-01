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
* Description:  Modifier key status tracker for Active Idle WS Plug-in.
*
*/


#include "hs_app_modifierkeytracker.h"
#include <e32keys.h>
#include <e32event.h>

//#define AI_ENABLE_RD_LOGGING
#define AI_RD_LOG_TO_DEBUG_OUTPUT
#include "debug.h"

namespace AiWsPlugin {


// LOCAL FUNCTIONS

TUint ModifierKeyToBit( TInt aScanCode )
    {
    switch( aScanCode )
        {
        case EStdKeyLeftShift:
            return EModifierLeftShift;
            
        case EStdKeyRightShift:
            return EModifierRightShift;
            
        case EStdKeyLeftAlt:
            return EModifierLeftAlt;

        case EStdKeyRightAlt:
            return EModifierRightAlt;

        case EStdKeyLeftCtrl:
            return EModifierLeftCtrl;

        case EStdKeyRightCtrl:
            return EModifierRightCtrl;

        case EStdKeyLeftFunc:
            return EModifierLeftFunc;

        case EStdKeyRightFunc:
            return EModifierRightFunc;
            
        default:
            return 0;
        }
    }

TUint UpdateCombinedIndicators(TUint aModifiers)
    {
    TUint result = aModifiers;
    struct TModifierCombinations { TUint iModifiers; TUint iCombinedIndicator; };
    static const TModifierCombinations KModifierCombinations[] =
        {
            { EModifierLeftShift | EModifierRightShift, EModifierShift },
            { EModifierLeftAlt   | EModifierRightAlt,   EModifierAlt   },
            { EModifierLeftCtrl  | EModifierRightCtrl,  EModifierCtrl  },
            { EModifierLeftFunc  | EModifierRightFunc,  EModifierFunc  }
        };
    const TModifierCombinations* KModifierCombinationsEnd = 
        KModifierCombinations + sizeof( KModifierCombinations ) / sizeof( KModifierCombinations[0] );

    for ( const TModifierCombinations* comb = KModifierCombinations;
          comb != KModifierCombinationsEnd;
          ++comb )
        {
        if ( aModifiers & comb->iModifiers )
            {
            result |= comb->iCombinedIndicator;
            }
        else
            {
            result &= ~comb->iCombinedIndicator;
            }
        }
    
    return result;
    }

// MEMBER FUNCTIONS

TModifierKeyTracker::TModifierKeyTracker() :
    iModifiers( 0 )
    {
    }

TBool TModifierKeyTracker::Update(const TRawEvent& aRawEvent)
    {
    TUint modifiers = iModifiers;
    
    switch( aRawEvent.Type() )
        {
        case TRawEvent::EKeyDown:
            {
            modifiers |= ModifierKeyToBit( aRawEvent.ScanCode() );
            modifiers = UpdateCombinedIndicators( modifiers );
            break;
            }
            
        case TRawEvent::EKeyUp:
            {
            modifiers &= ~ModifierKeyToBit( aRawEvent.ScanCode() );
            modifiers = UpdateCombinedIndicators( modifiers );
            break;
            }

        // DEBUG: Compare to what modifer update event would report
        case TRawEvent::EUpdateModifiers:
            {
            __PRINT( 
                __DBG_FORMAT("AiWsPlugin: TModifierKeyTracker::Update(), iModifers=%d, TRawEvent::Modifiers()=%d"), 
                iModifiers, aRawEvent.Modifiers() );
            break;
            }
            
        default:
            {
            // No operations..
            break;
            }
        }

    if( modifiers != iModifiers )
        {
        iModifiers = modifiers;
        return ETrue;
        }
    else
        {
        return EFalse;
        }
    }
    
} // namespace AiWsPlugin
