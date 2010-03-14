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
* Description:  CAiEcomObserver class implementation.
*
*/

// System includes

// User incldues
#include "aiecomobserver.h"


// Constants

// ======== LOCAL FUNCTIONS ========

// ======== MEMBER FUNCTIONS ========
//-----------------------------------------------------------------------------
// CAiEcomObserver::CAiEcomObserver
//
//-----------------------------------------------------------------------------
//
CAiEcomObserver::CAiEcomObserver()
    : CActive( EPriorityStandard )
	{
    CActiveScheduler::Add( this );
	}

//-----------------------------------------------------------------------------
// CAiEcomObserver::NewLC
//
//-----------------------------------------------------------------------------
//
CAiEcomObserver* CAiEcomObserver::NewLC()
	{
	CAiEcomObserver* self = new ( ELeave ) CAiEcomObserver();
	CleanupStack::PushL( self );
	self->ConstructL();
	return self;
	}
      
//-----------------------------------------------------------------------------
// CAiEcomObserver::NewL
//
//-----------------------------------------------------------------------------
//
CAiEcomObserver* CAiEcomObserver::NewL()
	{
	CAiEcomObserver* self = CAiEcomObserver::NewLC();
	CleanupStack::Pop( self ); 
	return self;
	}

//-----------------------------------------------------------------------------
// CAiEcomObserver::ConstructL
//
//-----------------------------------------------------------------------------
//
void CAiEcomObserver::ConstructL()
	{
	iEComSession = REComSession::OpenL();
	StartObserving();
	}

//-----------------------------------------------------------------------------
// CAiEcomObserver::~CAiEcomObserver
//
//-----------------------------------------------------------------------------
//
CAiEcomObserver::~CAiEcomObserver()
	{
    // Cancel any request, if outstanding
	Cancel();
    
    // Clean up memebers
    iObservers.Close();
         
    iEComSession.Close();   
	}

//-----------------------------------------------------------------------------
// CAiEcomObserver::DoCancel
//
//-----------------------------------------------------------------------------
//
void CAiEcomObserver::DoCancel()
	{
	iEComSession.CancelNotifyOnChange( iStatus );
	}

//-----------------------------------------------------------------------------
// CAiEcomObserver::StartObserving
//
//-----------------------------------------------------------------------------
//
void CAiEcomObserver::StartObserving()
	{       
	iEComSession.NotifyOnChange( iStatus );     
    SetActive();
	}

//-----------------------------------------------------------------------------
// CAiEcomObserver::AddObserverL
//
//-----------------------------------------------------------------------------
//
void CAiEcomObserver::AddObserverL( MAiEcomObserver* aObserver )
    {
    if ( aObserver )
        {
        iObservers.AppendL( aObserver );
        }    
    }

//-----------------------------------------------------------------------------
// CAiEcomObserver::RunL
//
//-----------------------------------------------------------------------------
//
void CAiEcomObserver::RunL()
	{   
    // Store completion status
    TInt status( iStatus.Int() );

    // Continue request
    StartObserving();

    // Notify observers
    if ( status == KErrNone )
        {
        NotifyObservers();
        }    
	}

//------------------------------------------------------------------------------
// CAiEcomObserver::NotifyObservers
//
//------------------------------------------------------------------------------
//
void CAiEcomObserver::NotifyObservers()
    {
    TInt count( iObservers.Count() );
    
    for ( TInt i = 0; i < count; i++ )
        {        
        iObservers[i]->NotifyEcomRegistryChanged();
        }
    }

// End of file
