/*
* Copyright (c) 2010 Nokia Corporation and/or its subsidiary(-ies).
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
* Description:  Publish and subscribe parameter observer implementation.
*
*/


#include <e32property.h>

//#include <TelephonyInternalPSKeys.h>
#include <StartupDomainPSKeys.h>

#include "caipspropertyobserver.h"



//const TInt KNotifyTimeShort( 100000 );
//const TInt KNotifyTimeLong( 10000000 );
// ---------------------------------------------------------------------------
// void ConstructL( TCallBack aCallBack, TUid aCategory, TInt aKey )
// ---------------------------------------------------------------------------
//
void CPSPropertyObserver::ConstructL()
    {

    //User::LeaveIfError( iProperty.Attach( iCategory, iKey ) );
    User::LeaveIfError( iTimer.CreateLocal() );
    CActiveScheduler::Add( this );
    
    //iProperty.Subscribe( iStatus );
    //SetActive();
    }

// ---------------------------------------------------------------------------
// CPSPropertyObserver()
// ---------------------------------------------------------------------------
//
CPSPropertyObserver::CPSPropertyObserver( TUid aCategory,
                                          TInt aKey )
    : 
    CActive( CActive::EPriorityStandard ),
    iCategory( aCategory ),
    iKey( aKey ),
    iCounter( 0 )
    {
    }
    
// ---------------------------------------------------------------------------
// CPSPropertyObserver* NewL( TCallBack aCallBack, TUid aCategory, TInt aKey )
// ---------------------------------------------------------------------------
//
CPSPropertyObserver* CPSPropertyObserver::NewL( TUid aCategory,
                                                TInt aKey )
    { 
    CPSPropertyObserver* self = new ( ELeave ) CPSPropertyObserver( aCategory,
                                                                    aKey );
    CleanupStack::PushL( self );
    self->ConstructL();
    CleanupStack::Pop( self );
    return self;
    }
    
// ---------------------------------------------------------------------------
// ~CPSPropertyObserver()
// ---------------------------------------------------------------------------
//
CPSPropertyObserver::~CPSPropertyObserver()
    {
    //Deque();
    //iProperty.Close();
    iTimer.Close();
    }
    
// ---------------------------------------------------------------------------
// void Release()
// ---------------------------------------------------------------------------
//
void CPSPropertyObserver::Release()
    {
    iTimer.Close();
    delete this;    
    }
    
// ---------------------------------------------------------------------------
// void RunL()
// ---------------------------------------------------------------------------
//
void CPSPropertyObserver::RunL()
    {
    iTimer.Close();
    iOtherObserver->DoEvent();
    //iCallBack.CallBack();
    }

// ---------------------------------------------------------------------------
// void DoCancel()
// ---------------------------------------------------------------------------
//
void CPSPropertyObserver::DoCancel()
    {
    iTimer.Cancel();
    }

// ---------------------------------------------------------------------------
// TInt Get()
// ---------------------------------------------------------------------------
//
void CPSPropertyObserver::SetCallBack( TCallBack aCallBack )
    {
    iCallBack = aCallBack;
    }

void CPSPropertyObserver::DoEvent()
    {
    iCallBack.CallBack();
    }

void CPSPropertyObserver::SetOtherObserver( CPSPropertyObserver* aOther )
    {
    iOtherObserver = aOther;
    }

TInt CPSPropertyObserver::Get()
    {
    TInt value = 0;

    // iProperty.Get( iCategory, iKey, value );

    return value;
    }

// ---------------------------------------------------------------------------
// TInt Get( TInt& aValue )
// ---------------------------------------------------------------------------
//
TInt CPSPropertyObserver::Get( TInt& aValue )
    { 	
	//TInt error = iProperty.Get( iCategory, iKey, aValue );
    TInt error = KErrNone;
    
   	aValue = ESwStateCriticalPhaseOK;

    return error;
    }
    
// ---------------------------------------------------------------------------
// TInt Get( TDes8& aString )
// ---------------------------------------------------------------------------
//
TInt CPSPropertyObserver::Get( TDes8& aString )
	{ 
	_LIT8( txt, "TEST");
	aString.Copy( txt );
	//TInt error = iProperty.Get( iCategory, iKey, aString );
    TInt error = KErrNone;
    return error;
    }

// ---------------------------------------------------------------------------
// TInt Get( TDes16& aString )
// ---------------------------------------------------------------------------
//
TInt CPSPropertyObserver::Get( TDes16& /*aString*/ )
    { 
	//TInt error = iProperty.Get( iCategory, iKey, aString );
    return KErrNone;
    }

// ========== OTHER EXPORTED FUNCTIONS ========================================

// End of File.
