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
* Description:  Light status observer
*
*/


#ifndef C_AILIGHTSTATUSOBSERVER_H
#define C_AILIGHTSTATUSOBSERVER_H

#include <e32base.h>
#include <e32property.h>
#include <hwrmlight.h>
#include "aidevicestatusobserver.h"

class MAiStateManager;

/**
 * @ingroup group_aifw
 * 
 *  Publish and subscribe key observer
 * 
 *  @lib aifw_hs
 *  @since S60 3.2
 */
 
NONSHARABLE_CLASS( CAiLightStatusObserver ) : public CBase,
                                              public MHWRMLightObserver,
                                              public MAiDeviceStatusObserver
    {
public:

// Construction

    static CAiLightStatusObserver* NewL( MAiStateManager* aStateManager );

    ~CAiLightStatusObserver();

// functions from base class MAiDeviceStatusObserver

    TAiStateChanges Status();

private:

// Construction
    
    CAiLightStatusObserver();
    
    void ConstructL( MAiStateManager* aStateManager );
    
// from MHWRMLightObserver

    void LightStatusChanged( TInt aTarget, CHWRMLight::TLightStatus aStatus );
    
protected: // Data

    /**
     * Light client.
     * Own.
     */
    CHWRMLight* iLight;

    /**
     * State manager.
     * Not own.
     */
    MAiStateManager* iStateManager;

    };

#endif // C_AILIGHTSTATUSOBSERVER_H

// End of File.
