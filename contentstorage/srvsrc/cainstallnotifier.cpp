/*
 * Copyright (c) 2008 Nokia Corporation and/or its subsidiary(-ies).
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
 * Description:  ?Description
 *
 */

#include "cainstallnotifier.h"
#include "cainstallstrategy.h"

EXPORT_C CCaInstallNotifier* CCaInstallNotifier::NewL(
        MCaInstallListener& aListener, TNotificationType aNotificationType )
    {
    CCaInstallNotifier* self = new ( ELeave ) CCaInstallNotifier( aListener );
    CleanupStack::PushL( self );
    self->ConstructL( aNotificationType );
    CleanupStack::Pop( self );
    return self;
    }

CCaInstallNotifier::~CCaInstallNotifier()
    {
    Cancel();
    iProperty.Close();
    delete iNotifierStrategy;
    }

CCaInstallNotifier::CCaInstallNotifier( MCaInstallListener& aListener ) :
    CActive( EPriorityNormal ), iListener( aListener )
    {
    CActiveScheduler::Add( this );
    SetActive();
    }

void CCaInstallNotifier::ConstructL( TNotificationType aNotificationType )
    {
    switch( aNotificationType )
		{
        case ESisInstallNotification:
        	iNotifierStrategy = CCaSwiInstallStrategy::NewL( iProperty );
            break;
        case EUsifUninstallNotification:
        	iNotifierStrategy = CCaUsifUninstallStrategy::NewL( iProperty );
            break;
        case EJavaInstallNotification:
        	iNotifierStrategy = CCaJavaInstallStrategy::NewL( iProperty );
            break;
        default:
        	User::Leave( KErrNotSupported );
            break;
		}
    iProperty.Subscribe( iStatus );
    }

void CCaInstallNotifier::DoCancel()
    {
    iProperty.Cancel();
    }

void CCaInstallNotifier::RunL()
    {
    SetActive();
    iProperty.Subscribe( iStatus );
    iNotifierStrategy->NotifyListenerL( iProperty, iListener );
    }

TInt CCaInstallNotifier::RunError( TInt /*aError*/)
    {
    // No need to do anything
    return KErrNone;
    }

