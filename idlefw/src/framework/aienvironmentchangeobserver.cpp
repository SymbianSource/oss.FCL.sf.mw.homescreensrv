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
* Description:  Environment change observer for AI2
*
*/


#include <bacntf.h> // for CEnvironmentChangeNotifier
#include <coemain.h>
#include <AknDef.h> // for KEikDynamicLayoutVariantSwitch
#include "aienvironmentchangeobserver.h"
#include "aistatemanager.h"
#include "aifwpanic.h"
#include "debug.h"

CAiEnvironmentChangeObserver::CAiEnvironmentChangeObserver()
    {
    }
    
CAiEnvironmentChangeObserver::~CAiEnvironmentChangeObserver()
    {
    if( iEnvironmentChangeNotifier )
        {
        iEnvironmentChangeNotifier->Cancel();
        delete iEnvironmentChangeNotifier;
        }
    }

CAiEnvironmentChangeObserver* CAiEnvironmentChangeObserver::NewL( MAiStateManager* aStateManager )
    {
    CAiEnvironmentChangeObserver* self = new (ELeave) CAiEnvironmentChangeObserver();
    CleanupStack::PushL(self);
    self->ConstructL( aStateManager );
    CleanupStack::Pop(self);
    return self;
    }

void CAiEnvironmentChangeObserver::ConstructL( MAiStateManager* aStateManager )
    {
    iStateManager = aStateManager;
    iEnvironmentChangeNotifier = CEnvironmentChangeNotifier::NewL(
                                    EActivePriorityLogonA,
                                    TCallBack( EnvironmentChangeCallBack, this ) );
    iEnvironmentChangeNotifier->Start();
    }
    
TAiStateChanges CAiEnvironmentChangeObserver::Status()
    {
    // No statuses to report
    return ESMAIUnknownState;
    }

TInt CAiEnvironmentChangeObserver::EnvironmentChangeCallBack(TAny* aPtr)
    {
    CAiEnvironmentChangeObserver* self =
            static_cast<CAiEnvironmentChangeObserver*>( aPtr );
            
    __ASSERT_DEBUG( self, 
                    AiFwPanic::Panic( AiFwPanic::EAiFwPanic_NullPointerReference ) );
                    
    const TInt changes( self->iEnvironmentChangeNotifier->Change() );

    // report environment changes
    if( changes & EChangesMidnightCrossover )
        {
        __PRINTS("XAI: Event: Midnight crossover");
        self->iStateManager->ReportStateChange( ESMAIMidnightCrossover );
        }
    if( changes & EChangesSystemTime )
        {
        __PRINTS("XAI: Event: Time changed");
        self->iStateManager->ReportStateChange( ESMAITimeChanged );
        }
    if( changes & EChangesLocale )
        {
        __PRINTS("XAI: Event: Locale setting changed");
        self->iStateManager->ReportStateChange( ESMAILocaleChanged );
        }

    return EFalse;
    }

