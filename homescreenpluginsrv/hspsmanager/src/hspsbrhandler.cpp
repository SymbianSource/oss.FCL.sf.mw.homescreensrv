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
* Description:  Implementation of CHSPSBRHandler class
*
*/


#include "hspsbrhandler.h"
#include "hspsthemeserver.h"


// ======== LOCAL FUNCTIONS ====================================================


// ======== MEMBER FUNCTIONS ===================================================


// -----------------------------------------------------------------------------
// CHSPSBRHandler::CHSPSBRHandler
// C++ default constructor can NOT contain any code, that
// might leave.
// -----------------------------------------------------------------------------
//
CHSPSBRHandler::CHSPSBRHandler( ChspsThemeServer& aServer ):
    iServer( aServer )
    {
    }

// -----------------------------------------------------------------------------
// CHSPSBRHandler::ConstructL
// Symbian 2nd phase constructor can leave.
// -----------------------------------------------------------------------------
//
void CHSPSBRHandler::ConstructL()
    {
    }
                    
// -----------------------------------------------------------------------------
// CHSPSBRHandler::NewL
// Two-phased constructor.
// -----------------------------------------------------------------------------
//
CHSPSBRHandler* CHSPSBRHandler::NewL( ChspsThemeServer& aServer )
    {
    CHSPSBRHandler* self = new( ELeave ) CHSPSBRHandler( aServer );
    
    CleanupStack::PushL( self );
    self->ConstructL();
    CleanupStack::Pop( self );

    return self;
    }

    
// Destructor
CHSPSBRHandler::~CHSPSBRHandler()
    {
    }

// ---------------------------------------------------------
// CHSPSBRHandler::AllSnapshotsSuppliedL
// ---------------------------------------------------------
// 
void CHSPSBRHandler::AllSnapshotsSuppliedL()
    {
    // No implementation needed. Must not leave
    }

// ---------------------------------------------------------
// CHSPSBRHandler::ReceiveSnapshotDataL
// ---------------------------------------------------------
// 
void CHSPSBRHandler::ReceiveSnapshotDataL(
    TDriveNumber /* aDrive */, 
    TDesC8& /* aBuffer */, 
    TBool /* aLastSection */ )
    {
    // No implementation needed
    User::Leave( KErrNotSupported );
    }

// ---------------------------------------------------------
// CHSPSBRHandler::GetExpectedDataSize
// ---------------------------------------------------------
// 
TUint CHSPSBRHandler::GetExpectedDataSize(
    TDriveNumber /* aDrive */)
    {
    // No implementation needed
    return 0;
    }

// ---------------------------------------------------------
// CHSPSBRHandler::GetSnapshotDataL
// ---------------------------------------------------------
// 
void CHSPSBRHandler::GetSnapshotDataL(
    TDriveNumber /* aDrive */, 
    TPtr8& /* aBuffer */, 
    TBool& /* aFinished */)
    {
    // No implementation needed
    User::Leave( KErrNotSupported );
    }

// ---------------------------------------------------------
// CHSPSBRHandler::InitialiseGetBackupDataL
// ---------------------------------------------------------
// 
void CHSPSBRHandler::InitialiseGetBackupDataL(
    TDriveNumber aDrive )
    {
    if( aDrive == EDriveC )
        {
        // Prepare backup data for passive backup 
        iServer.CreateBackupDataL();
        }
    }

// ---------------------------------------------------------
// CHSPSBRHandler::GetBackupDataSectionL
// ---------------------------------------------------------
// 
void CHSPSBRHandler::GetBackupDataSectionL(
    TPtr8& /*aBuffer*/, 
    TBool& aFinished )
    {
    // No data for active backup
    aFinished = ETrue;
    }

// ---------------------------------------------------------
// CHSPSBRHandler::InitialiseRestoreBaseDataL
// ---------------------------------------------------------
// 
void CHSPSBRHandler::InitialiseRestoreBaseDataL(
    TDriveNumber /* aDrive */ )
    {
    // No implementation needed
    User::Leave( KErrNotSupported );
    }

// ---------------------------------------------------------
// CHSPSBRHandler::RestoreBaseDataSectionL
// Create the files where the restore is performed from
// ---------------------------------------------------------
// 
void CHSPSBRHandler::RestoreBaseDataSectionL(
    TDesC8& /*aBuffer*/, 
    TBool /*aFinished*/ )
    {
    // No implementation needed
    }

// ---------------------------------------------------------
// CHSPSBRHandler::InitialiseRestoreIncrementDataL
// ---------------------------------------------------------
// 
void CHSPSBRHandler::InitialiseRestoreIncrementDataL(
    TDriveNumber /*aDrive*/ )
    {
    // No implementation needed
    User::Leave( KErrNotSupported );
    }

// ---------------------------------------------------------
// CHSPSBRHandler::RestoreIncrementDataSectionL
// ---------------------------------------------------------
//
void CHSPSBRHandler::RestoreIncrementDataSectionL(
    TDesC8& /*aBuffer*/, 
    TBool /*aFinished*/ )
    {
    // No implementation needed
    User::Leave( KErrNotSupported );
    }

// ---------------------------------------------------------
// CHSPSBRHandler::RestoreComplete
// ---------------------------------------------------------
// 
void CHSPSBRHandler::RestoreComplete(
    TDriveNumber /*aDrive*/ )
    {
    // No implementation needed
    }

// ---------------------------------------------------------
// CHSPSBRHandler::TerminateMultiStageOperation
// ---------------------------------------------------------
// 
void CHSPSBRHandler::TerminateMultiStageOperation()
    {
    // No implementation needed
    }

// ---------------------------------------------------------
// CHSPSBRHandler::GetDataChecksum
// ---------------------------------------------------------
// 
TUint CHSPSBRHandler::GetDataChecksum(
    TDriveNumber /* aDrive */)
    {
    // No implementation needed
    return 0;
    }

// End of file
