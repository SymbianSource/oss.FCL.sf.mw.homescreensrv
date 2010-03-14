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
* Description:  Numeric key forwarding handler for Active Idle WS Plug-in.
*
*/


#ifndef C_AIWSPLUGIN_NUMERICKEYHANDLER_H
#define C_AIWSPLUGIN_NUMERICKEYHANDLER_H

#include "eventhandler.h"
#include <cenrepnotifyhandler.h>
#include <PtiDefs.h>

class MAnimGeneralFunctionsWindowExtension;
class MAiPSPropertyObserver;

namespace AiWsPlugin {

class MUiState;

/**
 *  @ingroup group_wsplugin
 *
 * Numeric key forwarding handler for Active Idle WS Plug-in. 
 *
 * Monitors and forwards numeric key presses that should be handled by Phone 
 * application.
 *
 *  @since S60 3.2
 */
class CNumericKeyHandler : 
        public CEventHandler, 
        private MCenRepNotifyHandlerCallback
    {
public:    
    
    /**
     * Creates a new instance of this class.
     *
     * @param aTargetWgId Window Group id of the target application where 
     *                    numeric key events are forwarded.
     * @return A new object of this class. The returned object is left on the
     *         cleanup stack.
     */
    static CNumericKeyHandler* NewLC
        ( TInt aTargetWgId, MAnimGeneralFunctionsWindowExtension* aWindowExt );

    ~CNumericKeyHandler();

private:
// from base class CEventHandler
    void SetUiStateQuery( MUiState& aUiState );
    
    void FocusChanged( TBool aState );
    
    TBool OfferRawEvent(const TRawEvent& aRawEvent);

// new methods
    CNumericKeyHandler
        ( TInt aTargetWgId, MAnimGeneralFunctionsWindowExtension* aWindowExt );
        
    void ConstructL();

    TBool CheckPostToTarget( const TRawEvent& aRawEvent ) const;
    
    void LoadInputLanguageKeyBindings( TInt aLanguage );
    
    static TInt HandleQwertyModeChanged( TAny *aPtr );
    
    TInt HandleInputLanguageChanged( TInt aNewValue );
    
    void SetQwertyMode( TInt aValue );
    
    void SetInputLanguage( TInt aValue );

// from base class MCenRepNotifyHandlerCallback

    void HandleNotifyGeneric(TUint32 aKey);
    
    void HandleNotifyError(TUint32 aKey, TInt aError, CCenRepNotifyHandler* aHandler);
        
private: // data

    /// Target window group id
    TInt iTargetWgId;
    
    /// Anim extension. Not own.
    MAnimGeneralFunctionsWindowExtension* iWindowExt;
    
    /// Pointer ot state. Not own.
    MUiState* iUiState;
    
    /// Qwerty ps observer. Owned.
    MAiPSPropertyObserver* iQwertyObserver;

    /// Input language observer. Owned.    
	CCenRepNotifyHandler* iInputLanguageObserver;
	
	/// Input language repository. Owned.
    CRepository* iInputLanguageRepository; 
    
    /// Numeric keys array.
    RArray<TPtiNumericKeyBinding> iNumericKeys;
    
    /// Qwerty mode indicator.
    TInt iQwertyMode;
    
    /// Input language indicator.
    TInt iInputLanguage;
    
private:
    // friend classes
    
#ifdef _AIWSPLUGIN_UNIT_TEST    
    friend class UT_NumericKeyHandler;
#endif    
    };

} // namespace AiWsPlugin


#endif // C_AIWSPLUGIN_KEYLOCKHANDLER_H
