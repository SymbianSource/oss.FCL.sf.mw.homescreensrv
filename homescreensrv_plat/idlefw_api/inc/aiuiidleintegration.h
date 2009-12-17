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
* Description:  Window server plug-in manager.
*
*/


#ifndef C_AIUIIDLEINTEGRATION_H
#define C_AIUIIDLEINTEGRATION_H

#include <e32base.h>

class CCoeControl;
class TWsEvent;
class CEikonEnv;
class TVwsViewId;
class CAknKeySoundSystem;
class MAiFwEventHandler;

struct TAiIdleKeySoundConfig
    {
    /**
     * Default constructor. Resets all config fields to null values.
     */
    TAiIdleKeySoundConfig() : iKeySounds(NULL), iContextResId(0) { }

    /// Pointer to AVKON Key sound system
    CAknKeySoundSystem* iKeySounds;

    /// Key sound context resource id
    TInt iContextResId;
    };

/**
 * Handles Idle integration related tasks and events for Active Idle main UI 
 * Controller.
 */
class CAiUiIdleIntegration : public CBase
    {

public:

    IMPORT_C static CAiUiIdleIntegration* NewL(
        CEikonEnv& aEikEnv,
    	const TAiIdleKeySoundConfig& aKeySoundConfig,
        MAiFwEventHandler* aAiFwEventHandler  );

    ~CAiUiIdleIntegration() { }
    
	/**
	 * Tries to bring Active Idle UI to the foreground.
	 */
    virtual void ActivateUI() = 0;

    /**
     * Call from entry to Main UI Controller AppUi HandleWsEventL.
     */
    virtual void HandleWsEventL( const TWsEvent& aEvent,
    								CCoeControl* aDestination ) = 0;
    };

#endif // C_AIUIIDLEINTEGRATION_H

