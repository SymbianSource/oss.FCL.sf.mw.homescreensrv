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
* Description:  Defination of class CHSPSBRHandler
*
*/


#ifndef C_CHSPSBRHANDLER_H
#define C_CHSPSBRHANDLER_H

#include <e32base.h>
#include <sbdefs.h>
#include <abclient.h>

class ChspsThemeServer;

/**
* @ingroup group_hspsserver
* Provides implementation to HSPS backup handler
*
**/
NONSHARABLE_CLASS( CHSPSBRHandler ) : public CBase, public conn::MActiveBackupDataClient
    {

    public:  // Constructors and destructor
        
        /**
        * Two-phased constructor.
        */
        static CHSPSBRHandler* NewL( ChspsThemeServer& aServer );
        
        /**
        * Destructor.
        */
        virtual ~CHSPSBRHandler();


    public: // New functions
 
    public: // // Functions from base classes
        
        /**
        From MActiveBackupDataClient
        */
        void AllSnapshotsSuppliedL();

        /**
        From MActiveBackupDataClient
        */
        void ReceiveSnapshotDataL( TDriveNumber aDrive, TDesC8& aBuffer, TBool aLastSection );

        /**
        From MActiveBackupDataClient
        */
        TUint GetExpectedDataSize( TDriveNumber aDrive );

        /**
        From MActiveBackupDataClient
        */
        void GetSnapshotDataL( TDriveNumber aDrive, TPtr8& aBuffer, TBool& aFinished );

        /**
        From MActiveBackupDataClient
        */
        void InitialiseGetBackupDataL( TDriveNumber aDrive );

        /**
        From MActiveBackupDataClient
        */
        void GetBackupDataSectionL( TPtr8& aBuffer, TBool& aFinished );

        /**
        From MActiveBackupDataClient
        */
        void InitialiseRestoreBaseDataL( TDriveNumber aDrive );

        /**
        From MActiveBackupDataClient
        */
        void RestoreBaseDataSectionL( TDesC8& aBuffer, TBool aFinished );

        /**
        From MActiveBackupDataClient
        */
        void InitialiseRestoreIncrementDataL( TDriveNumber aDrive );

        /**
        From MActiveBackupDataClient
        */
        void RestoreIncrementDataSectionL( TDesC8& aBuffer, TBool aFinished );

        /**
        From MActiveBackupDataClient
        */
        void RestoreComplete( TDriveNumber aDrive );
        
        /**
        From MActiveBackupDataClient
        */
        void TerminateMultiStageOperation();
        
        /**
        From MActiveBackupDataClient
        */
        TUint GetDataChecksum( TDriveNumber aDrive );

    private:

        /**
        * C++ default constructor.
        */
        CHSPSBRHandler( ChspsThemeServer& aServer );

        /**
        * By default Symbian 2nd phase constructor is private.
        */
        void ConstructL();


    private:    // Data

        // Listening server reference
        ChspsThemeServer& iServer;

    };

#endif // C_CHSPSBRHANDLER_H
