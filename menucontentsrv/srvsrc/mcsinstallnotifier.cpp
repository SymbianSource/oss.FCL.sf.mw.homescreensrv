/*
* Copyright (c) 2009 Nokia Corporation and/or its subsidiary(-ies).
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

#include <javadomainpskeys.h>
#include "mcsinstallnotifier.h"
#include "mcsinstallstrategy.h"


CMcsInstallNotifier* CMcsInstallNotifier::NewL(
		MMcsInstallListener& aListener, TNotificationType aNotificationType )
    {
    CMcsInstallNotifier* self = new ( ELeave ) CMcsInstallNotifier( aListener );
    CleanupStack::PushL( self );
    self->ConstructL( aNotificationType );
    CleanupStack::Pop( self );
    return self;
    }

CMcsInstallNotifier::~CMcsInstallNotifier()
    {
    Cancel();
    iProperty.Close();
    delete iNotifierStrategy;
    }

CMcsInstallNotifier::CMcsInstallNotifier( MMcsInstallListener& aListener ) :
    CActive( EPriorityNormal ), iListener( aListener )
    {
    CActiveScheduler::Add( this );
    SetActive();
    }

void CMcsInstallNotifier::ConstructL( TNotificationType aNotificationType )
    {
    switch( aNotificationType )
		{
        case ESisInstallNotification:
        	iNotifierStrategy = CMcsSwiInstallStrategy::NewL(
        	        iProperty, iListener );
            break;
        case EJavaInstallNotification:
        	iNotifierStrategy = CMcsJavaInstallStrategy::NewL(
        	        iProperty, iListener );
            break;
        default:
        	User::Leave( KErrNotSupported );
            break;
		}
    iProperty.Subscribe( iStatus );
    }

void CMcsInstallNotifier::DoCancel()
    {
    iProperty.Cancel();
    }

void CMcsInstallNotifier::RunL()
    {
    SetActive();
    iProperty.Subscribe( iStatus );
    iNotifierStrategy->NotifyListenerL();
    }

TInt CMcsInstallNotifier::RunError( TInt /*aError*/)
    {
    // No need to do anything
    return KErrNone;
    }

