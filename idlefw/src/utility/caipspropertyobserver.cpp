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
* Description:  Publish and subscribe parameter observer implementation.
*
*/


#include "caipspropertyobserver.h"

// ---------------------------------------------------------------------------
// void ConstructL( TCallBack aCallBack, TUid aCategory, TInt aKey )
// ---------------------------------------------------------------------------
//
void CPSPropertyObserver::ConstructL()
    {
    User::LeaveIfError( iProperty.Attach( iCategory, iKey ) );
    CActiveScheduler::Add( this );

    iProperty.Subscribe( iStatus );
    SetActive();
    }

// ---------------------------------------------------------------------------
// CPSPropertyObserver()
// ---------------------------------------------------------------------------
//
CPSPropertyObserver::CPSPropertyObserver( TCallBack aCallBack,
                                          TUid aCategory,
                                          TInt aKey )
  : CActive( EPriorityHigh ),
    iCallBack( aCallBack ),
    iCategory( aCategory ),
    iKey( aKey )
    {
    }
    
// ---------------------------------------------------------------------------
// CPSPropertyObserver* NewL( TCallBack aCallBack, TUid aCategory, TInt aKey )
// ---------------------------------------------------------------------------
//
CPSPropertyObserver* CPSPropertyObserver::NewL( TCallBack aCallBack,
                                                TUid aCategory,
                                                TInt aKey )
    { 
    CPSPropertyObserver* self = new ( ELeave ) CPSPropertyObserver( aCallBack,
                                                                    aCategory,
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
    if( IsAdded() )
        {
        Deque();
        }
    iProperty.Close();
    }
    
// ---------------------------------------------------------------------------
// void Release()
// ---------------------------------------------------------------------------
//
void CPSPropertyObserver::Release()
    {
    delete this;
    }
    
// ---------------------------------------------------------------------------
// void RunL()
// ---------------------------------------------------------------------------
//
void CPSPropertyObserver::RunL()
    {
    // resubscribe before processing new value to prevent missing updates.
    iProperty.Subscribe( iStatus );
    SetActive();

    // property updated, get new value.
    TInt propval;
    if ( iProperty.Get( iCategory, iKey, propval ) != KErrNotFound )
        {
        iCallBack.CallBack();
        }
    }

// ---------------------------------------------------------------------------
// void DoCancel()
// ---------------------------------------------------------------------------
//
void CPSPropertyObserver::DoCancel()
    {
    iProperty.Cancel();
    }

// ---------------------------------------------------------------------------
// TInt Get( TInt& aValue )
// ---------------------------------------------------------------------------
//
TInt CPSPropertyObserver::Get( TInt& aValue )
    { 
	TInt error = iProperty.Get( iCategory, iKey, aValue );

    return error;
    }
    
// ---------------------------------------------------------------------------
// TInt Get( TDes8& aString )
// ---------------------------------------------------------------------------
//
TInt CPSPropertyObserver::Get( TDes8& aString )
	{ 
	TInt error = iProperty.Get( iCategory, iKey, aString );
    return error;
    }

// ---------------------------------------------------------------------------
// TInt Get( TDes16& aString )
// ---------------------------------------------------------------------------
//
TInt CPSPropertyObserver::Get( TDes16& aString )
    { 
	TInt error = iProperty.Get( iCategory, iKey, aString );
    return error;
    }

// ========== OTHER EXPORTED FUNCTIONS ========================================

// End of File.
