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
* Description:  Software Installer plug-in usb handler
*
*/
 

// INCLUDE FILES

#include "chswiusbhandler.h"
#include "chswiplugin.h"
#include "chswimassmodeobserver.h"
#include <driveinfo.h>

// CONSTANTS
const TInt KCallBackDelay = 5000000;

// ----------------------------------------------------------------------------
// CCHSwiUsbHandler::NewL()
// two-phase constructor
// ----------------------------------------------------------------------------
//
CCHSwiUsbHandler* CCHSwiUsbHandler::NewL(
		MCHSwiMassModeObserver* aMassModeObserver,
		RFs& aFs )
    {
    CCHSwiUsbHandler* self =
        new(ELeave) CCHSwiUsbHandler( aMassModeObserver, aFs );
    CleanupStack::PushL( self );
    self->ConstructL();
    CleanupStack::Pop( self );
    return self;
    }

// ----------------------------------------------------------------------------
// CCHSwiUsbHandler::CCHSwiUsbHandler()
// C++ default constructor
// ----------------------------------------------------------------------------
//
CCHSwiUsbHandler::CCHSwiUsbHandler( MCHSwiMassModeObserver* aMassModeObserver,
        RFs& aFs )
    : CActive( CActive::EPriorityUserInput ),
      iLastDriveScanError( KErrNone ),
      iMassModeObserver( aMassModeObserver ),
      iFs( aFs ),
      iDriveFlags( 0 )
    {
    CActiveScheduler::Add( this );
    }

// ----------------------------------------------------------------------------
// CCHSwiUsbHandler::ConstructL()
// Symbian default constructor
// ----------------------------------------------------------------------------
//
void CCHSwiUsbHandler::ConstructL()
    {
    User::LeaveIfError( iTimer.CreateLocal() );
    
    iLastDriveScanError = ScanDrivesAndUpdateFlag( EFalse );

    if ( iLastDriveScanError != KErrNone )
    	{
    	HandleDriveError();
    	}
    }

// ----------------------------------------------------------------------------
// CCHSwiUsbHandler::~CCHSwiUsbHandler()
// destructor
// ----------------------------------------------------------------------------
//
CCHSwiUsbHandler::~CCHSwiUsbHandler()
    {
    Cancel();
    iTimer.Close();
    }

// ----------------------------------------------------------------------------
// CCHSwiUsbHandler::SynchronousDriveScan()
// Performs synchronous drive scan. Checks whether Mass Drive is available
// and updates the flag in observer.
// ----------------------------------------------------------------------------
//
void CCHSwiUsbHandler::SynchronousDriveScan()
	{
    iLastDriveScanError = ScanDrivesAndUpdateFlag( iLastDriveScanError == KErrNone );

    if ( iLastDriveScanError != KErrNone )
    	{
    	HandleDriveError();
    	}
	}

// ----------------------------------------------------------------------------
// CCHSwiUsbHandler::PostponedDriveScan()
// Starts postponed drive scan. 
// ----------------------------------------------------------------------------
//
void CCHSwiUsbHandler::PostponedDriveScan()
	{
	Cancel();
	iTimer.After( iStatus, KCallBackDelay ); 
	SetActive();
	}

// ----------------------------------------------------------------------------
// CCHSwiUsbHandler::RunL()
// Handles postponed drive scan. 
// Executed either after call to PostponedDriveScan or in case of drive reading
// errors in SynchronousDriveScan or ConstructL.
// ----------------------------------------------------------------------------
//
void CCHSwiUsbHandler::RunL()
    {
    TBool massModeBeforeScan = iMassModeObserver->IsMassStorageMode();
    TInt errorBeforeScan = iLastDriveScanError;
    
    SynchronousDriveScan();
    
    if ( iLastDriveScanError == KErrNone )
		{
		iMassModeObserver->HandleSuccessfulAsynchDriveScan();
		
		// CCHSwiUsbObserver can't detect app uninstall when app had been installed on removable 
		// memory card, the memory card was removed during mass mode, and then mass 
		// mode was deactivated.
	    // Additionaly, we need to update widgets when recovering from error.
		if ( !iMassModeObserver->IsMassStorageMode() && 
			( massModeBeforeScan || errorBeforeScan != KErrNone ) )
			{
			iMassModeObserver->HandleMassStorageModeEndEvent();
			}
		}   
    }

// ----------------------------------------------------------------------------
// CCHSwiUsbHandler::HandleDriveError()
// Handles drive access errors by launching new postponed scan.
// ----------------------------------------------------------------------------
void CCHSwiUsbHandler::HandleDriveError()
	{
	Cancel();
	iMassModeObserver->SetMassStorageMode( ETrue );
	
	iTimer.After( iStatus, KCallBackDelay ); 
	SetActive();
	}

// ----------------------------------------------------------------------------
// CCHSwiUsbHandler::ScanDrivesAndUpdateFlag()
// Scans drives and updates Mass Mode flag in the observer. 
// ----------------------------------------------------------------------------
TInt CCHSwiUsbHandler::ScanDrivesAndUpdateFlag( TBool aFlagsValid )
	{
	TInt error( KErrNone );
	
	if( aFlagsValid )
		{
		// iDriveFlags is valid, we can use deltaDriveFlags to avoid
		// unnecessary calls to DriveInfo::GetDriveStatus
		TInt driveFlags = 0;
		TInt deltaDriveFlags = 0;
		
		error = ScanDrives( driveFlags );
		
		if ( error == KErrNone )
			{
			deltaDriveFlags = iDriveFlags ^ driveFlags;
			iDriveFlags = driveFlags;
			}
		
		if ( deltaDriveFlags )
			{ 
			error = UpdateMassModeFlag();
			}
		}
	else
		{
		error = ScanDrives( iDriveFlags );

		if ( error == KErrNone )
			{
			error = UpdateMassModeFlag();
			}   
		}
	
	return error;
	}

// ----------------------------------------------------------------------------
// CCHSwiUsbHandler::UpdateMassModeFlag()
// Updates Mass Mode flag in the observer. 
// ----------------------------------------------------------------------------
TInt CCHSwiUsbHandler::UpdateMassModeFlag()
	{
    TInt flags = iDriveFlags;
    TBool massMemoryPresent = EFalse;
    
    for( TInt DriveNo = EDriveA + 1 ; DriveNo <= EDriveY; DriveNo++ )
    	{   
    	flags = flags >> 1;
        if( flags & 01 )
            {
            TUint status( 0 );
            TInt error = DriveInfo::GetDriveStatus( iFs, DriveNo, status );

            if( error != KErrNone )
            	{
            	return error;
            	}
            
            if( ( status & DriveInfo::EDriveExternallyMountable ) 
            		&& ( status & DriveInfo::EDriveInternal ) )
                {
                // Internal Memory
                massMemoryPresent = ETrue;
                break;
                }
            }            
        }
           
    iMassModeObserver->SetMassStorageMode( !massMemoryPresent );            
    return KErrNone;
	}

// ----------------------------------------------------------------------------
// CCHSwiUsbHandler::RunError()
// Ignore errors from RunL.
// ----------------------------------------------------------------------------
//
TInt CCHSwiUsbHandler::RunError( TInt /* aError */ )
    {
    return KErrNone; // indicates error was handled
    }

// ----------------------------------------------------------------------------
// CCHSwiUsbHandler::DoCancel()
// Cancel the MMC event handler
// ----------------------------------------------------------------------------
void CCHSwiUsbHandler::DoCancel()
    {
    iTimer.Cancel();
    }

// ----------------------------------------------------------------------------
// CCHSwiUsbHandler::ScanDrives( TInt& aDriveFlags )
// Scans drives and records a bit flag for those that exist and are
// suitable for installing widgets to.
// ----------------------------------------------------------------------------
//
TInt CCHSwiUsbHandler::ScanDrives( TInt& aDriveFlags )
    {
    // List all drives in the system
    TDriveList driveList;
    TInt error = iFs.DriveList( driveList );
    
    if ( KErrNone == error )
        {
        for ( TInt driveNumber = EDriveY;
              driveNumber >= EDriveA;
              driveNumber-- )
            {
            // The drives that will be filtered out are the same ones that
            // WidgetInstaller filters out in CWidgetUIHandler::SelectDriveL()
            if ( (EDriveD == driveNumber)
                 || !driveList[driveNumber] )
                {
                // EDriveD is a temporary drive usually a RAM disk
                continue;
                }

            TVolumeInfo volInfo;
            if ( iFs.Volume( volInfo, driveNumber ) != KErrNone )
                {
                // volume is not usable (e.g. no media card inserted)
                continue;
                }
            if ( (volInfo.iDrive.iType == EMediaNotPresent) ||
                 (volInfo.iDrive.iType == EMediaRom) ||
                 (volInfo.iDrive.iType == EMediaRemote) ||
                 (volInfo.iDrive.iDriveAtt & KDriveAttRom) ||
                 (volInfo.iDrive.iDriveAtt & KDriveAttSubsted) )
                {
                // not a suitable widget install drive
                continue;
                }

            // found a usable drive
            aDriveFlags |= (1 << driveNumber);
            }
        }
    
    return error;
    }

