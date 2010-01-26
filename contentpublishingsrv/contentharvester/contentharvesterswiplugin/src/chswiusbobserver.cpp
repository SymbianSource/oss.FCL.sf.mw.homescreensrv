/*
* Copyright (c) 2007 Nokia Corporation and/or its subsidiary(-ies).
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

// INCLUDE FILES

#include "chswiusbobserver.h"
#include "chswiusbhandler.h"

// ----------------------------------------------------------------------------
// CCHSwiUsbObserver::NewL()
// two-phase constructor
// ----------------------------------------------------------------------------
//
CCHSwiUsbObserver* CCHSwiUsbObserver::NewL( CCHSwiUsbHandler* aHandler, RFs& aFs )
    {
    CCHSwiUsbObserver* self =
        new(ELeave) CCHSwiUsbObserver( aHandler, aFs );
    CleanupStack::PushL( self );
    self->ConstructL();
    CleanupStack::Pop( self );
    return self;
    }

// ----------------------------------------------------------------------------
// CCHSwiUsbObserver::CCHSwiUsbObserver()
// C++ default constructor
// ----------------------------------------------------------------------------
//
CCHSwiUsbObserver::CCHSwiUsbObserver( CCHSwiUsbHandler* aHandler, RFs& aFs )
    : CActive( CActive::EPriorityUserInput ),
      iFs( aFs ),
      iUsbHandler( aHandler )
    {
    CActiveScheduler::Add( this );
    }

// ----------------------------------------------------------------------------
// CCHSwiUsbObserver::ConstructL()
// Symbian default constructor
// ----------------------------------------------------------------------------
//
void CCHSwiUsbObserver::ConstructL()
    {
    }

// ----------------------------------------------------------------------------
// CCHSwiUsbObserver::~CCHSwiUsbObserver()
// destructor
// ----------------------------------------------------------------------------
//
CCHSwiUsbObserver::~CCHSwiUsbObserver()
    {
    Cancel();
    }

// ----------------------------------------------------------------------------
// CCHSwiUsbObserver::DoCancel()
// 
// ----------------------------------------------------------------------------
//
void CCHSwiUsbObserver::DoCancel()
	{
	iFs.NotifyChangeCancel( iStatus );
	}

// ----------------------------------------------------------------------------
// CCHSwiUsbObserver::Start()
// Start monitoring.
// ----------------------------------------------------------------------------
//
void CCHSwiUsbObserver::Start()
    {
    Cancel();
    iFs.NotifyChange( ENotifyDisk, iStatus );
    SetActive();
    }

// ----------------------------------------------------------------------------
// CCHSwiUsbObserver::RunL()
// Handle notifications of MMC events.
// ----------------------------------------------------------------------------
//
void CCHSwiUsbObserver::RunL()
    {
    iUsbHandler->PostponedDriveScan();
    }

// ----------------------------------------------------------------------------
// CCHSwiUsbObserver::RunError()
//
// ----------------------------------------------------------------------------
//
TInt CCHSwiUsbObserver::RunError( TInt /*aError*/)
    {
    return KErrNone;
    }

