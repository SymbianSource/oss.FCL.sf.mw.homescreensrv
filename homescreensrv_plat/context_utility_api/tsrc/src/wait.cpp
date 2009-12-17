/*
* Copyright (c) 2006-2006 Nokia Corporation and/or its subsidiary(-ies).
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
* Description:  CWait class implementation
*
*/


#include "wait.h"

// CONSTANTS

_LIT( KPanicCat, "ActTimSche" );

enum TPanicReason
    {
    EAlreadyStarted
    };
    
LOCAL_C void Panic( TInt aCode )
    {
    User::Panic( KPanicCat, aCode );
    }

// MEMBER FUNCTIONS

CWait* CWait::NewL()
    {

    CWait* self =
        CWait::NewLC();
    CleanupStack::Pop( self );

    return self;
    }
  
CWait* CWait::NewLC()
   {

    CWait* self =
        new( ELeave ) CWait;
    CleanupStack::PushL( self );
    self->ConstructL();

    return self;
    }
  
CWait::CWait():
    CTimer( EPriorityStandard )
    {
    
    // Double check if base class adds active object into scheduler
    if( !IsAdded() )
        {
        CActiveScheduler::Add( this );
        }
    }

void CWait::ConstructL()
    {

    // Do base constructions
    CTimer::ConstructL();
    
    // Initialize active scheduler wait
    iWait = new( ELeave ) CActiveSchedulerWait;
    }

// Destructor
CWait::~CWait()
    {

    Cancel();
    delete iWait;
    }

// METHODS

//-----------------------------------------------------------------------------
// CWait::Start
//-----------------------------------------------------------------------------
//
void CWait::Start(
    const TTimeIntervalMicroSeconds32& aInterval )
    {

    __ASSERT_ALWAYS( !IsActive(), Panic( EAlreadyStarted ) );
    
    After( aInterval );
    iWait->Start();
    }

//-----------------------------------------------------------------------------
// CWait::Stop
//-----------------------------------------------------------------------------
//
void CWait::Stop()
    {

    Cancel();
    }

//-----------------------------------------------------------------------------
// CWait::RunL
//-----------------------------------------------------------------------------
//
void CWait::RunL()
    {

    // Double check that wait really started
    if( iWait->IsStarted() )
        {
        iWait->AsyncStop();
        }
    }

//-----------------------------------------------------------------------------
// CWait::DoCancel
//-----------------------------------------------------------------------------
//
void CWait::DoCancel()
    {

    // Double check that wait really started
    CTimer::DoCancel();
    if( iWait->IsStarted() )
        {
        iWait->AsyncStop();
        }
    }


// end of file
