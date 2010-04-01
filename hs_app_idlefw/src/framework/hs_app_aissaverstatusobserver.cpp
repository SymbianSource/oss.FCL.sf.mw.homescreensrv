/*
* Copyright (c) 2005-2005 Nokia Corporation and/or its subsidiary(-ies).
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
* Description:  
*
*/


#include <aipspropertyobserver.h>
#include "hs_app_aissaverstatusobserver.h"
#include <screensaverinternalpskeys.h> // this include needs to be last
#include "hs_app_aifwpanic.h"
#include "debug.h"

CAiSSaverStatusObserver::CAiSSaverStatusObserver()
    {
    }
    
CAiSSaverStatusObserver::~CAiSSaverStatusObserver()
    {
    }

CAiSSaverStatusObserver* CAiSSaverStatusObserver::NewL( MAiStateManager* aStateManager )
    {
    CAiSSaverStatusObserver* self = new (ELeave) CAiSSaverStatusObserver();
    CleanupStack::PushL(self);
    self->ConstructL( aStateManager );
    CleanupStack::Pop(self);
    return self;
    }

void CAiSSaverStatusObserver::ConstructL( MAiStateManager* aStateManager )
    {
    BaseConstructL( TCallBack( HandleScreenSaverStateChanged, this ),
                    KPSUidScreenSaver,
                    KScreenSaverOn,
		            aStateManager );
    }
    
TAiStateChanges CAiSSaverStatusObserver::Status()
    {
    TInt value = 0;
    Tint err = iObserver->Get( value );
    if( ( value == 0 ) ||
        ( err != KErrNone ) )
    	{
    	return ESMAIScreensaverInactive;
    	}
    else
    	{
    	return ESMAIScreensaverActive;
    	} 
    }

TInt CAiSSaverStatusObserver::HandleScreenSaverStateChanged( TAny* aPtr )
    {
    CAiSSaverStatusObserver* self =
                static_cast<CAiSSaverStatusObserver*>( aPtr );

    __ASSERT_DEBUG( self, 
                    AiFwPanic::Panic( AiFwPanic::EAiFwPanic_NullPointerReference ) );

    TInt value = 0;
    TInt err = self->iObserver->Get( value );
	
    if ( ( value == 0 ) ||
         ( err != KErrNone ) )
		{
        // screensaver off
        __PRINTS("XAI: Screen saver = OFF");
        self->iStateManager->ReportStateChange( ESMAIScreensaverInactive );
		}
    else
		{
        // screensaver on
        __PRINTS("XAI: Screen saver = ON");
        self->iStateManager->ReportStateChange( ESMAIScreensaverActive );
		}
	
    return KErrNone;
    }

