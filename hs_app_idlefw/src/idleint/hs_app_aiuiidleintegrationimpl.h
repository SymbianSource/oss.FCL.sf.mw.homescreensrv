/*
* Copyright (c) 2006 Nokia Corporation and/or its subsidiary(-ies).
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
* Description:  Active Idle main UI Controller idle integration.
*
*/


#ifndef C_AIUIIDLEINTEGRATIONIMPL_H
#define C_AIUIIDLEINTEGRATIONIMPL_H

#include "aiuiidleintegration.h"
#include <startupdomainpskeys.h>

class CActiveIdleState;
class CAknIncallBubble;
class MAiPSPropertyObserver;
class MAiFwEventHandler;

/**
 * CAiUiIdleIntegration implementation.
 */
NONSHARABLE_CLASS( CAiUiIdleIntegrationImpl ) : public CAiUiIdleIntegration
    {
public: // constructor and destructor
    
    static CAiUiIdleIntegrationImpl* NewL( CEikonEnv& aEikEnv,         
                const TAiIdleKeySoundConfig& aKeySoundConfig,
                MAiFwEventHandler* aAiFwEventHandler );

    ~CAiUiIdleIntegrationImpl();

public: // new functions
        
    void HandleWsEventL( const TWsEvent& aEvent, 
                         CCoeControl* aDestination );

private: // constructors
    
    CAiUiIdleIntegrationImpl(CEikonEnv& aEikEnv,
                             MAiFwEventHandler* aAiFwEventHandler);
    void ConstructL(const TAiIdleKeySoundConfig& aKeySoundConfig);

private: // new functions
    
    void ActivateUI();
    
    void SetCallBubbleIfNeededL();
    void ClearCallBubbleL();
           
    static TInt HandleSystemStateChange( TAny *aPtr );
    static TInt HandleUiStartupStateChange( TAny *aPtr );
    static TInt HandleCallEvent( TAny *aPtr );
    
private: // data  

    CEikonEnv& iEikEnv;  
    CActiveIdleState* iActiveIdleState;
    TBool iThisApplicationForeground;
    TBool iIncallBubbleAllowed;
    TInt iThisApplicationWgId;
    TInt iEikonServerWgId;
    CAknIncallBubble* iIncallBubble;

    /**
     * Observer for system state Publish&Subscribe key.
     * Owned.
     */
    MAiPSPropertyObserver* iSystemStateObserver;

    /**
     * Observer for system state Publish&Subscribe key.
     * Owned.
     */
    MAiPSPropertyObserver* iUiStartupStateObserver;

    /**
    * Observer telephony state
    * Owned
    */
    MAiPSPropertyObserver* iCallStatusObserver;
    /**
     * Framework event handler. For notifying critical startup over.
     * Not owned.
     */
    MAiFwEventHandler* iAiFwEventHandler;
    
    TBool iSystemStateOk;
    TBool iUiStartupPhaseOk;
    };


#endif // C_AIUIIDLEINTEGRATION_H

