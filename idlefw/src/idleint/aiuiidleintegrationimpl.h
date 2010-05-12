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

// System includes
#include <e32property.h>

// User includes
#include "aiuiidleintegration.h"

// Forward declarations
class CActiveIdleState;
class CAknIncallBubble;
class MAiPSPropertyObserver;
class MAiFwEventHandler;

/**
 * CAiUiIdleIntegration implementation.
 */
NONSHARABLE_CLASS( CAiUiIdleIntegrationImpl ) : public CAiUiIdleIntegration
    {
public: 
    // constructor and destructor
    
    static CAiUiIdleIntegrationImpl* NewL( CEikonEnv& aEikEnv,         
                const TAiIdleKeySoundConfig& aKeySoundConfig,
                MAiFwEventHandler* aAiFwEventHandler );

    ~CAiUiIdleIntegrationImpl();

public: 
    // new functions
        
    void HandleWsEventL( const TWsEvent& aEvent, 
                         CCoeControl* aDestination );

private: 
    // constructors
    
    CAiUiIdleIntegrationImpl(CEikonEnv& aEikEnv,
                             MAiFwEventHandler* aAiFwEventHandler);
    void ConstructL(const TAiIdleKeySoundConfig& aKeySoundConfig);

private: 
    // new functions
    
    void ActivateUI();
    
    void SetCallBubbleIfNeededL();
    void ClearCallBubbleL();
           
    static TInt HandleCallEvent( TAny *aPtr );
    
    static TInt HandleUiStartupStateChange( TAny *aPtr );
    
private: 
    // data  

    /**
     * EikonEnv
     * Not owned
     */
    CEikonEnv& iEikEnv;  
    
    /**
     * Active idle state
     * Owned
     */
    CActiveIdleState* iActiveIdleState;
        
    /**
     * Incall bubble
     * Owned
     */
    CAknIncallBubble* iIncallBubble;

    /**
    * Observer telephony state
    * Owned
    */
    MAiPSPropertyObserver* iCallStatusObserver;
    
    /**
    * Ui startup state
    * Owned
    */
    MAiPSPropertyObserver* iUiStartupStateObserver;
    
    /**
     * Framework event handler. For notifying critical startup over.
     * Not owned.
     */
    MAiFwEventHandler* iAiFwEventHandler;
    
    TBool iForeground;
    TBool iIncallBubbleAllowed;  
    TBool iUiStartupPhaseOk;    
    };

#endif // C_AIUIIDLEINTEGRATION_H

// End of file
