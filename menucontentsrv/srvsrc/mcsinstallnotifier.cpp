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
#include <sacls.h>

CMcsInstallNotifier* CMcsInstallNotifier::NewL(MMcsInstallListener& aListener, TInt aKey)
	{
	CMcsInstallNotifier* self = new (ELeave) CMcsInstallNotifier( aListener, aKey );
	CleanupStack::PushL( self );
	self->ConstructL( );
	CleanupStack::Pop( self );

	return self;
	}

CMcsInstallNotifier::~CMcsInstallNotifier()
	{
    Cancel( );
    iProperty.Close( );
	}

CMcsInstallNotifier::CMcsInstallNotifier( MMcsInstallListener& aListener, TInt aKey ) : 
    CActive(EPriorityNormal), iListener(aListener)
	{
	iKey = aKey;
	// Prepare automatically
	iProperty.Attach( KUidSystemCategory, iKey );
	CActiveScheduler::Add( this );
	iProperty.Subscribe( iStatus );
	SetActive( );
	}


void CMcsInstallNotifier::ConstructL()
	{

	}


void CMcsInstallNotifier::DoCancel()
	{
    iProperty.Cancel( );
	}


void CMcsInstallNotifier::RunL()
	{
	SetActive( );
	iProperty.Subscribe( iStatus );
	TInt status;
	User::LeaveIfError( iProperty.Get( KUidSystemCategory,
			iKey, status ) );

	if (( iKey == KPSUidJavaLatestInstallation ) || 
			((status & EInstOpInstall )||(status & EInstOpUninstall )) && 
			  (status & EInstOpStatusSuccess) )
		{
		iListener.HandleInstallNotifyL(status);
		}

	}


TInt CMcsInstallNotifier::RunError( TInt /*aError*/ )
	{
    // No need to do anything      
    return KErrNone;
	}
