/*
* Copyright (c) 2005-2006 Nokia Corporation and/or its subsidiary(-ies).
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
* Description:  UI Framework observer for Active Idle
*
*/


#ifndef C_AIUIFRAMEWORKOBSERVERIMPL_H
#define C_AIUIFRAMEWORKOBSERVERIMPL_H


#include <e32base.h>
#include "aistatemanager.h"
#include "aiuiframeworkobserver.h"
#include "aidevicestatusobserver.h"

/**
 * CAiUiFrameworkObserverImpl implements MAiUiFrameworkObserver for
 * Active Idle framework. It is also a bridge between MAiUiFrameworkObserver
 * and Active Idle system state manager.
 *
 *  @lib aifw_hs.lib
 *  @since S60 3.2
 */
NONSHARABLE_CLASS( CAiUiFrameworkObserverImpl )
    : public CBase,
      public MAiUiFrameworkObserver,
      public MAiDeviceStatusObserver
    {
public:

    static CAiUiFrameworkObserverImpl* NewL( MAiStateManager& aManager );

    virtual ~CAiUiFrameworkObserverImpl();

// from base class MAiDeviceStatusObserver

    TAiStateChanges Status();

private:

    CAiUiFrameworkObserverImpl( MAiStateManager& aManager );

    void ConstructL();

// from base class MAiUiFrameworkObserver
    
    void HandleResourceChange( TInt aType );

    void HandleForegroundEvent( TBool aForeground );
    
private: // data

    /**
     * State manager.
     */
    MAiStateManager& iManager;

    };

#endif // C_AIUIFRAMEWORKOBSERVERIMPL_H
