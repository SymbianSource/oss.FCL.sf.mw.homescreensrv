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
* Description:  Installs HSPS configurations.
*
*
*/


#ifndef C_CHSPSINSTALLER_H
#define C_CHSPSINSTALLER_H

#include <e32base.h>
#include <f32file.h>
#include "hspsthememanagement.h"

class ChspsThemeServer;
class ChspsInstallationHandler;

/**
 * @ingroup group_hspsserver
 * HSPS Installer
 * @lib hspsThemeServer.exe
 * @since S60 5.0
 */
class CHSPSInstaller : public CActive
    {
    public: // Constructors and destructors
        /**
        * NewL.
        * Two-phased constructor.
        * Creates a CHSPSInstaller object using two phase construction,
        * and return a pointer to the created object.
        * @since S60 5.0
        * @param aServer is a reference to HSPS Theme Server
        * @return A pointer to the created instance of CHSPSInstaller.
        */
        static CHSPSInstaller* NewL( 
            ChspsThemeServer& aServer );

        /**
        * ~CHSPSInstaller.
        * Destructor.
        * Destroys the object and release all memory objects.
        * @since S60 5.0
        */
        virtual ~CHSPSInstaller();
      
    public: // New functions
                   
        /**
         * InstallConfigurationL
         * Synchronous service for installing configurations from provided manifest files.         
         * @since S60 5.0
         * @param aFileName is name of the manifest file (full path) to be installed.
         * @return ThspsServiceCompletedMessage error code 
         */
        ThspsServiceCompletedMessage InstallConfigurationL( 
            const TDesC& aFileName ); 
        
        /**
         * Get installation handler.
         */
        ChspsInstallationHandler& InstallationHandler();
        
    public: // Functions from base classes
        
        /**
         * RunL.
         * From CActive, callback function.
         * Invoked to handle responses from the server.
         * @since S60 5.0
         */
        void RunL();

        /**
         * DoCancel.         
         * From CActive, cancels any outstanding operation.
         * @since S60 5.0
         */
        void DoCancel();
        
        /**
         * RunError.
         * From CActive Called when asynchronous request has failed
         * @since S60 5.0
         */
        TInt RunError( 
            TInt aError );
        
    private: 
        
        /**
        * CHSPSInstaller.
        * Performs the first phase of two phase construction.
        * @since S60 5.0
        * @param aServer is a reference to the HSPS Theme Server
        */
        CHSPSInstaller(
            ChspsThemeServer& aServer );

        /**
        * ConstructL.
        * Performs the second phase construction of a CHSPSInstaller object.
        * @since S60 5.0
        */
        void ConstructL();           
        
    private: // Data    

        // Reference to HSPS Theme Server
        ChspsThemeServer& iServer;
        
        // Installation handler
        ChspsInstallationHandler* iInstallationHandler;
        
        // Installation status
        ThspsServiceCompletedMessage iRet;

        // Required by the installation process
        TBuf8<KMaxHeaderDataLength8> iHeaderData;
        
        // Active scheduler nested wait loop
        CActiveSchedulerWait* iLoop;

    };


#endif // C_CHSPSINSTALLER_H

// End of File
