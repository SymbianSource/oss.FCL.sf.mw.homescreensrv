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
* Description:  Light status observer for AI2
*
*/


#include <aipspropertyobserver.h>
#include "hs_app_ailightstatusobserver.h"
#include "hs_app_aistatemanager.h"
#include "debug.h"

CAiLightStatusObserver::CAiLightStatusObserver()
    {
    }
    
CAiLightStatusObserver::~CAiLightStatusObserver()
    {
    delete iLight;
    }

CAiLightStatusObserver* CAiLightStatusObserver::NewL( MAiStateManager* aStateManager )
    {
    CAiLightStatusObserver* self = new (ELeave) CAiLightStatusObserver();
    CleanupStack::PushL(self);
    self->ConstructL( aStateManager );
    CleanupStack::Pop(self);
    return self;
    }

void CAiLightStatusObserver::ConstructL( MAiStateManager* aStateManager )
    {
    iStateManager = aStateManager;
    iLight = CHWRMLight::NewL( this );
    }
    
TAiStateChanges CAiLightStatusObserver::Status()
    {
    // In future handle other screen lights here also..
    CHWRMLight::TLightStatus status = iLight->LightStatus( CHWRMLight::EPrimaryDisplay );
    if( status == CHWRMLight::ELightOn )
        {
        return ESMAIBacklightOn;
        }
    else if( status == CHWRMLight::ELightOff )
        {
        return ESMAIBacklightOff;
        }
    return ESMAIBacklightOn;
    }

void CAiLightStatusObserver::LightStatusChanged( TInt aTarget, CHWRMLight::TLightStatus aStatus )
    {
    if( aTarget == CHWRMLight::EPrimaryDisplay ||
        aTarget == CHWRMLight::EPrimaryDisplayAndKeyboard )
        {
        if( aStatus == CHWRMLight::ELightOn )
            {
            __PRINTS("XAI: Light = ON");
            iStateManager->ReportStateChange( ESMAIBacklightOn );
            }
        else if( aStatus == CHWRMLight::ELightOff )
            {
            __PRINTS("XAI: Light = OFF");
            iStateManager->ReportStateChange( ESMAIBacklightOff );
            }
        }
    }

