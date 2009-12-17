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
* Description:  The API supports attributes not present in MCS from SAT Api
*
*/


#include "mcsdrmobserver.h"

// ============================ MEMBER FUNCTIONS =============================

// ---------------------------------------------------------------------------
// Creates an instance of CMcsDrmObserver
// ---------------------------------------------------------------------------
//
CMcsDrmObserver* CMcsDrmObserver::NewL( 
	MMcsDrmObserver& aObserver, 
    const TDesC& aDRMContentId,
    TUid aUid,
    TDRMEventType aDRMEventType )
	{
	CMcsDrmObserver* self = new (ELeave) 
		CMcsDrmObserver( aObserver, aUid, aDRMEventType );
    CleanupStack::PushL( self );
    self->ConstructL( aDRMContentId );
    CleanupStack::Pop( self );
    
    return self;
	}

// ---------------------------------------------------------------------------
// Destructor
// ---------------------------------------------------------------------------
//
CMcsDrmObserver::~CMcsDrmObserver()
	{
	UnRegisterEventObserver();
	delete iDRMNotifier;
	iContentId.Close();
	}


// ---------------------------------------------------------------------------
// 
// ---------------------------------------------------------------------------
//
CMcsDrmObserver::CMcsDrmObserver( 
	MMcsDrmObserver& aObserver,
	TUid aUid,
	TDRMEventType aDRMEventType ):
	iObserver( aObserver ),
	iAppUid( aUid ),
	iDRMEventType( aDRMEventType )
	{
	
	}

// -----------------------------------------------------------------------------
// CMcsDrmObserver::ConstructL
// S2nd phase constructor.
// -----------------------------------------------------------------------------
//
void CMcsDrmObserver::ConstructL( 
		const TDesC& aDRMContentId )
	{
	iContentId.CreateL( aDRMContentId.Length() );
	iContentId.Copy( aDRMContentId );
	iDRMNotifier = CDRMNotifier::NewL();
	RegisterEventObserverL();
	}
	

// ---------------------------------------------------------------------------
// 
// ---------------------------------------------------------------------------
//
void CMcsDrmObserver::RegisterEventObserverL()
	{
	iDRMNotifier->RegisterEventObserverL( *this, iDRMEventType, iContentId );
	}
// ---------------------------------------------------------------------------
// 
// ---------------------------------------------------------------------------
//
TUid CMcsDrmObserver::Uid()
	{
	return iAppUid;
	}

// ---------------------------------------------------------------------------
// 
// ---------------------------------------------------------------------------
//
void CMcsDrmObserver::UnRegisterEventObserver()
	{
	TRAP_IGNORE(iDRMNotifier->UnRegisterEventObserverL( *this, iDRMEventType, iContentId ));
	}

// ---------------------------------------------------------------------------
// 
// ---------------------------------------------------------------------------
//
void CMcsDrmObserver::HandleEventL( MDRMEvent* /*aEvent*/ )
	{
	iObserver.HandleDrmEvent( iAppUid );
	}

// End of File
