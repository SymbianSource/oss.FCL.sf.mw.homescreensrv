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

EXPORT_C CCaInstallNotifier* CCaInstallNotifier::NewL(
        MCaInstallListener& aListener, TNotificationType aNotificationType )
    {
    CCaInstallNotifier* self = new ( ELeave ) CCaInstallNotifier(
            aListener, aNotificationType );
    CleanupStack::PushL( self );
    self->ConstructL();
    CleanupStack::Pop( self );
    return self;
    }

CCaInstallNotifier::~CCaInstallNotifier()
    {
    Cancel();
    iProperty.Close();
    }

CCaInstallNotifier::CCaInstallNotifier( MCaInstallListener& aListener,
        TNotificationType aNotificationType ) :
    CActive( EPriorityNormal ), iListener( aListener )
    {
    iRPropertyKey = aNotificationType;
    iProperty.Attach( KUidSystemCategory, iRPropertyKey );
    CActiveScheduler::Add( this );
    iProperty.Subscribe( iStatus );
    SetActive();
    }

void CCaInstallNotifier::ConstructL()
    {
    }

void CCaInstallNotifier::DoCancel()
    {
    iProperty.Cancel();
    }

void CCaInstallNotifier::RunL()
    {
    SetActive();
    iProperty.Subscribe( iStatus );
    TInt appUid;
    User::LeaveIfError( iProperty.Get( KUidSystemCategory, iRPropertyKey,
            appUid ) );
    if( appUid )
        {
        iListener.HandleInstallNotifyL( appUid );
        }
    }

TInt CCaInstallNotifier::RunError( TInt /*aError*/)
    {
    // No need to do anything
    return KErrNone;
    }
