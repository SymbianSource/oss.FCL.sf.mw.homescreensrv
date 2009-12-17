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


#include "mcschildrenscanner.h"
#include "menueng.h"
#include "menusrveng.h"
#include "menuengobject.h"

// ============================ MEMBER FUNCTIONS =============================

// ---------------------------------------------------------------------------
// Creates an instance of CMcsChildrenScanner implementation
// ---------------------------------------------------------------------------
//
CMcsChildrenScanner* CMcsChildrenScanner::NewL( 
	MMcsChildrenScanner& aObserver, 
	CMenuSrvEng& aSrvEng)
    {
    CMcsChildrenScanner* self = new (ELeave) CMcsChildrenScanner( 
        aObserver, aSrvEng );
    CleanupStack::PushL( self );
    self->ConstructL();
    CleanupStack::Pop( self );
    
    return self;
    }


// ---------------------------------------------------------------------------
// Destructor
// ---------------------------------------------------------------------------
//
CMcsChildrenScanner::~CMcsChildrenScanner()
    {
    Cancel();
    iSrvEng.Engine().DequeueOperation( *this );
    iIdBuff.Close();
    }
    
// -----------------------------------------------------------------------------
// CMcsChildrenScanner::CMcsChildrenScanner
// C++ default constructor
// -----------------------------------------------------------------------------
//
CMcsChildrenScanner::CMcsChildrenScanner( 
	MMcsChildrenScanner& aObserver, 
    CMenuSrvEng& aSrvEng ): 
    CActive( EPriorityNormal ), iObserver( aObserver ), 
    iSrvEng(aSrvEng)
    {    
    CActiveScheduler::Add( this );
    iOpStatus = EFalse;
    }

// -----------------------------------------------------------------------------
// CMcsChildrenScanner::ConstructL
// S2nd phase constructor.
// -----------------------------------------------------------------------------
//
void CMcsChildrenScanner::ConstructL()
    {
    }

// ---------------------------------------------------------------------------
// CMcsChildrenScanner::DoCancel
// ---------------------------------------------------------------------------
//
void CMcsChildrenScanner::DoCancel()
    {
    }

// ---------------------------------------------------------------------------
// CMcsChildrenScanner::RunL
// ---------------------------------------------------------------------------
//
void CMcsChildrenScanner::RunL()
    {
    User::LeaveIfError( iStatus.Int() ); // Handle errors in RunL.
    iSrvEng.Engine().QueueOperationL( *this );    
    }    

// ---------------------------------------------------------
// CMcsChildrenScanner::RunError()
// ---------------------------------------------------------
//
TInt CMcsChildrenScanner::RunError( TInt /*aError*/ )
    {
    // Ignore the error (what else could we do?).
    return KErrNone;
    }

// ---------------------------------------------------------
// CMenuSrvAppScanner::RunMenuEngOperationL
// ---------------------------------------------------------
//
void CMcsChildrenScanner::RunMenuEngOperationL()
    {
    ScanFolderL();
    iOpStatus = EFalse;
    }  
    
// ---------------------------------------------------------
// CMenuSrvAppScanner::CompletedMenuEngOperation
// ---------------------------------------------------------
//
void CMcsChildrenScanner::CompletedMenuEngOperation( TInt /*aErr*/ )
    {
    }          

    
// ---------------------------------------------------------
// CMcsChildrenScanner::ScanFolderL
// ---------------------------------------------------------
//
void CMcsChildrenScanner::ScanFolderL()
    {    
    
    while ( iIdBuff.Count( ) != 0 ) 
    	{
    	TInt currentFolder = iIdBuff[0];
    	
		// Get current children_count
		TUint32 newCount =  iSrvEng.GetChildrenCountL(currentFolder);
		
		// Chceck if children_count has changed
		// if children_count wasn't in cash it will be added
		// when GetAttributeL is executed
		TBool attrExists;
		RBuf attrVal;
		attrVal.CleanupClosePushL();
		attrVal.CreateL(KMenuMaxAttrValueLen);
		
		iSrvEng.GetAttributeL(currentFolder, KChildrenCount, attrExists, attrVal);
		
		if (attrExists)
			{
			TLex lex(attrVal);
			TUint oldCount;
			TInt err = lex.Val(oldCount);
			if (err == KErrNone)
				{
				iObserver.HandleChildrenEvent(currentFolder, oldCount, newCount);
				}
			}
		
		CleanupStack::PopAndDestroy( &attrVal );
		RemoveId(currentFolder);
    	}
    iOpStatus = EFalse;
    }


// -----------------------------------------------------------------------------
// 
// -----------------------------------------------------------------------------
//
void CMcsChildrenScanner::Scan(TInt aId)
    {
    AddId(aId);
    if ( !IsActive() && !iOpStatus )
        {
        iOpStatus = ETrue;
        TRequestStatus* ownStatus = &iStatus;
        *ownStatus = KRequestPending;
        SetActive();
        User::RequestComplete( ownStatus, KErrNone );
        } 
    }  
    

void CMcsChildrenScanner::AddId(TInt aId)
	{
	if (iIdBuff.Find(aId) == KErrNotFound)
		{
		iIdBuff.Append(aId);
		}
	}

void CMcsChildrenScanner::RemoveId(TInt aId)
	{
	TInt index = iIdBuff.Find(aId);
	if (index != KErrNotFound)
		{
		iIdBuff.Remove(index);
		}
	}
// End of File
