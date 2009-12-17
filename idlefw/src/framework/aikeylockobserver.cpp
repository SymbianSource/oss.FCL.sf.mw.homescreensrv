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
* Description:  Focus observer for Active idle 2
*
*/


#include <aipspropertyobserver.h>
#include <activeidle2domainpskeys.h>
#include <avkondomainpskeys.h>      // KPSUidAvkonDomain, KAknKeyguardStatus, TAknKeyguardStatus
#include "aistatemanager.h"
#include "aikeylockobserver.h"
#include "aifwpanic.h"
#include "debug.h"

CAiKeylockObserver::CAiKeylockObserver()
    {
    }
    
CAiKeylockObserver::~CAiKeylockObserver()
    {
    }

CAiKeylockObserver* CAiKeylockObserver::NewL(
                                        MAiStateManager* aStateManager )
    {
    CAiKeylockObserver* self = new (ELeave) CAiKeylockObserver();
    CleanupStack::PushL(self);
    self->ConstructL( aStateManager );
    CleanupStack::Pop(self);
    return self;
    }

void CAiKeylockObserver::ConstructL( MAiStateManager* aStateManager )
    {
    BaseConstructL( TCallBack( HandleKeylockStatusEvent, this ),
                    KPSUidAvkonDomain, 
                    KAknKeyguardStatus, 
                    aStateManager );
    }
    
TAiStateChanges CAiKeylockObserver::Status()
    {
	TInt value;
	TInt err = iObserver->Get( value );
	if( err != KErrNone ) 
	    {
	    return ESMAIKeylockDisabled;
	    }
	    
	switch( value ) 
	    {
	    case EKeyguardLocked:
	    case EKeyguardAutolockEmulation: // fallthorugh
	        {
	        return ESMAIKeylockEnabled;
	        }
	    case EKeyguardNotActive:
	    default: // fallthorugh
	        {
	        return ESMAIKeylockDisabled;
	        }
	    }
    }

TInt CAiKeylockObserver::HandleKeylockStatusEvent( TAny* aPtr )
    {
    CAiKeylockObserver* self =
                static_cast<CAiKeylockObserver*>( aPtr );
    
    __ASSERT_DEBUG( self, 
                    AiFwPanic::Panic( AiFwPanic::EAiFwPanic_NullPointerReference ) );
    
    TAiStateChanges stateChange = self->Status();
    self->iStateManager->ReportStateChange( stateChange );
    return KErrNone;
    }

