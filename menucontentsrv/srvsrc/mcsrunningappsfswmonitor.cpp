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
* Description:  
*
*/


#include "mcsrunningappsfswmonitor.h"
#include "mcsrunningappshandler.h"
#include "menueng.h"

const TInt KMaxHiddenApps = 25;
const TUid KPSUidUikon = { 0x101F8773 };
const TUint32 KUikAppHiddenList  = 0x00000010;

// ---------------------------------------------------------
// CMcsRunningAppsFswMonitor::NewL
// ---------------------------------------------------------
//
CMcsRunningAppsFswMonitor* CMcsRunningAppsFswMonitor::NewL(
    CMenuEng& aEng, MMcsRunningAppsHandler* aRunningAppsHandler, 
    TInt aPriority )
	{
	CMcsRunningAppsFswMonitor* self = 
		new ( ELeave ) CMcsRunningAppsFswMonitor( aEng, aRunningAppsHandler, 
				aPriority );
	CleanupStack::PushL( self );
	self->ConstructL();
	CleanupStack::Pop( self );
	return self;
	}

// ---------------------------------------------------------
// CMcsRunningAppsFswMonitor::~CMcsRunningAppsFswMonitor
// ---------------------------------------------------------
//
CMcsRunningAppsFswMonitor::~CMcsRunningAppsFswMonitor()
    {
    Cancel();
    iEng.DequeueOperation( *this );
    iFSWStatusProperty.Close();
    iFswHiddenApps.Close();
    }

// ---------------------------------------------------------
// CMcsRunningAppsFswMonitor::CMcsRunningAppsFswMonitor
// ---------------------------------------------------------
//
CMcsRunningAppsFswMonitor::CMcsRunningAppsFswMonitor(
    CMenuEng& aEng, MMcsRunningAppsHandler* aRunningAppsHandler, TInt aPriority):
    CActive( aPriority ),
    iEng(aEng),
    iRunningAppsHandler(aRunningAppsHandler)
    {
    CActiveScheduler::Add( this );
    }

// ---------------------------------------------------------
// CMcsRunningAppsFswMonitor::ConstructL
// ---------------------------------------------------------
//
void CMcsRunningAppsFswMonitor::ConstructL()
	{
	// Create P&S watcher for FSW status changes and read initial apps list
	if ( KErrNone == iFSWStatusProperty.Attach( KPSUidUikon, KUikAppHiddenList ) )
		{
	    // read list
	    TRAP_IGNORE( UpdateFswHiddenApplicationsL() )
	    Start();
	    }	
	}

// ---------------------------------------------------------
// CMcsRunningAppsFswMonitor::IsFswHidden
// ---------------------------------------------------------
//
TBool CMcsRunningAppsFswMonitor::IsFswHidden( TUid aUid )  
	{
	TBool result(EFalse);
	if( KErrNotFound != iFswHiddenApps.Find( aUid ) )
		{
		result = ETrue;
		}
	return result;
    } 

// ---------------------------------------------------------
// CMcsRunningAppsFswMonitor::RunMenuEngOperationL
// ---------------------------------------------------------
//
void CMcsRunningAppsFswMonitor::RunMenuEngOperationL()
    {
    UpdateFswHiddenApplicationsL();
    } 
       
// ---------------------------------------------------------
// CMcsRunningAppsFswMonitor::CompletedMenuEngOperation
// ---------------------------------------------------------
//
void CMcsRunningAppsFswMonitor::CompletedMenuEngOperation( TInt /*aErr*/ )
    {
    // If there was error, ignore it (what else could we do?).
    // When next AppArc update occurs, we will run again.
    }

// ---------------------------------------------------------
// CMcsRunningAppsFswMonitor::UpdateFswHiddenApplicationsL
// ---------------------------------------------------------
//
void CMcsRunningAppsFswMonitor::UpdateFswHiddenApplicationsL()
    {
    // the maximum size of array (25 32-bit UIDs equal 100 bytes)
    // 1. read current data
    TBuf16 <2*KMaxHiddenApps> retrievedList;
    TInt err = iFSWStatusProperty.Get( retrievedList );
    
    // 2. convert the buffer to dynamic array with right uid values
    TInt i = 0;
    iFswHiddenApps.Reset();
    TUint32 listValue;
    while( i < KMaxHiddenApps && KErrNone == err )
        {
        // 32-bit uid values are retrieved in two 16-bit parts
        listValue = retrievedList[2*i] << 16;
        listValue += retrievedList[2*i+1];
        
        // the value NULL marks the end of array -> leave the loop
        if ( listValue )
            {
            TUid t ( KNullUid );
            t.iUid = listValue;
            iFswHiddenApps.AppendL( t );
            }
        else
            {
            err = KErrEof;                
            }
        i++;
        }
	
	iRunningAppsHandler->HandleListUpdateL( iFswHiddenApps, 
			MMcsRunningAppsHandler::EFswHiddenApps );
    }


// ---------------------------------------------------------
// CMcsRunningAppsFswMonitor::Start
// ---------------------------------------------------------
//
void CMcsRunningAppsFswMonitor::Start()
    {
    if (!IsActive())
        {
	    iFSWStatusProperty.Subscribe(iStatus);
        SetActive();
        }
    }
// ---------------------------------------------------------
// CMcsRunningAppsFswMonitor::DoCancel
// ---------------------------------------------------------
//
void CMcsRunningAppsFswMonitor::DoCancel()
    {
    iFSWStatusProperty.Cancel();
    }

// ---------------------------------------------------------
// CMcsRunningAppsFswMonitor::RunL
// ---------------------------------------------------------
//
void CMcsRunningAppsFswMonitor::RunL()
    {
    User::LeaveIfError( iStatus.Int() ); // Handle errors in RunL.
    Start();
    iEng.QueueOperationL( *this );
    }

// ---------------------------------------------------------
// CMcsRunningAppsFswMonitor::RunError()
// ---------------------------------------------------------
//
TInt CMcsRunningAppsFswMonitor::RunError( TInt /*aError*/ )
    {
    Start();
    return KErrNone;
    }

//  End of File  
