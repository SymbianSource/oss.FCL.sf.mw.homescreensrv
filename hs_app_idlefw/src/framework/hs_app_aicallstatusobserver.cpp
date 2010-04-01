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
* Description:  Call status observer for AI2
*
*/


#include <aipspropertyobserver.h>
#include <ctsydomainpskeys.h>
#include "hs_app_aicallstatusobserver.h"
#include "hs_app_aistatemanager.h"
#include "hs_app_aifwpanic.h"
#include "debug.h"

CAiCallStatusObserver::CAiCallStatusObserver()
    {
    }
    
CAiCallStatusObserver::~CAiCallStatusObserver()
    {
    }

CAiCallStatusObserver* CAiCallStatusObserver::NewL( MAiStateManager* aStateManager )
    {
    CAiCallStatusObserver* self = new (ELeave) CAiCallStatusObserver();
    CleanupStack::PushL(self);
    self->ConstructL( aStateManager );
    CleanupStack::Pop(self);
    return self;
    }

void CAiCallStatusObserver::ConstructL( MAiStateManager* aStateManager )
    {
    BaseConstructL( TCallBack( HandleCallStateChange, this ),
                    KPSUidCtsyCallInformation,
                    KCTsyCallState,
                    aStateManager );
    }
    
TAiStateChanges CAiCallStatusObserver::Status()
    {
    TInt value = 0;
    TInt err = iObserver->Get( value );
    if( ( value > EPSCTsyCallStateNone ) &&
        ( err == KErrNone ) )
        {
        return ESMAIInCall;
        }
    else
        {
        return ESMAINoCall;
        } 
    }

TInt CAiCallStatusObserver::HandleCallStateChange( TAny* aPtr )
    {
    CAiCallStatusObserver* self = reinterpret_cast< CAiCallStatusObserver* >( aPtr );

    __ASSERT_DEBUG( self, 
                    AiFwPanic::Panic( AiFwPanic::EAiFwPanic_NullPointerReference ) );

    TInt value = 0;
    TInt err = self->iObserver->Get( value );
                 
    if( ( value > EPSCTsyCallStateNone ) &&
        ( err == KErrNone ) )
        {
        __PRINTS("XAI: Call = ON");
        self->iStateManager->ReportStateChange( ESMAIInCall );
        }
    else
        {
        __PRINTS("XAI: Call = OFF");
        self->iStateManager->ReportStateChange( ESMAINoCall );
        } 
    return KErrNone;
    }

