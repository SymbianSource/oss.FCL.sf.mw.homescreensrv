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
* Description:  Auto-installs configurations from ROM drive to the C drive.
*
*
*/


#ifndef __hspsROMINSTALLER_H__
#define __hspsROMINSTALLER_H__

#include <e32base.h>
#include <f32file.h>

#include "hspsthememanagement.h"

#ifdef HSPS_LOG_ACTIVE
class ChspsLogBus;
#endif
class ChspsThemeServer;
class ChspsInstallationHandler;


/**
 * @ingroup group_hspsserver 
 * ROM installer
 * @lib hspsThemeServer.exe
 * @since S60 5.0
 */
class ChspsRomInstaller : public CActive
    {
    public: // Constructors and destructors
        /**
        * NewL.
        * Two-phased constructor.
        * Creates a ChspsRomInstaller object using two phase construction,
        * and return a pointer to the created object.
        * @since S60 5.0
        * @param aThemeServer is a reference to theme server instance
        * @param aFsSession is a reference to file server instance
        * @return A pointer to the created instance of ChspsRomInstaller.
        */
        static ChspsRomInstaller* NewL( 
        		ChspsThemeServer& aThemeServer,
        		RFs& aFsSession );

        /**
        * NewLC.
        * Two-phased constructor.
        * Creates a ChspsClinet object using two phase construction,
        * and return a pointer to the created object.
        * @since S60 5.0
        * @param aObserver The object to be used to handle updates from the server.
        * @return a pointer to the created instance of ChspsRomInstaller.
        */
        static ChspsRomInstaller* NewLC( 
        		ChspsThemeServer& aThemeServer,
        		RFs& aFsSession );

        /**
        * ~ChspsRomInstaller.
        * Destructor.
        * Destroys the object and release all memory objects.
        * @since S60 5.0
        */
        virtual ~ChspsRomInstaller();
      
    public: // New functions
           
        /**         
         * Finds plugin_*.dat and app_*.dat files from Z\private\200159C0\install
         * File names are stored into iImportsArrayV1 member array
         * @since S60 5.0
         */
        void FindImportsV1L();                
                                                
        /**
         * Imports.
         * Returns a reference to the imports list, which is maintained by the class.
         * Contents of the list is set in FindImportsL() function.
         * @since S60 5.0
         * @return an array of *.DAT files which were found from the ROM drive.
         */
        const RPointerArray<HBufC>& ImportsV1();
        
        /**
         * InstallTheme
         * Synchronous service for installing configurations from provided manifest files.         
         * @since S60 5.0
         * @param aFileName is name of the manifest file to be installed.
         * @return ThspsServiceCompletedMessage error code 
         */
        ThspsServiceCompletedMessage InstallThemeL( 
                const TDesC& aFileName ); 
        
        /**
         * ReinstallThemeL.
         * Synchronous service for re-installing corrupted application configurations  
         * from an import (imports\app_*.dat file).
         * @since S60 5.0
         * @param aAppUid is UID of an application 
         * @param aConfigurationUid is UID of a HSPS configuration
         * @return ThspsServiceCompletedMessage error code 
         */
        ThspsServiceCompletedMessage ReinstallThemeL(
                const TInt aAppUid,
                const TInt aConfigurationUid );
        
        /**         
         * Gets names of the folders which should be installed from Z\private\200159C0\install
         * @since S60 5.0
         */
        void GetInstallationFoldersL(  
                RPointerArray<HBufC>& aFolders );
        
#ifdef HSPS_LOG_ACTIVE        
        /** 
        * Set log bus.
        */
        void SetLogBus( ChspsLogBus* aLogBus );
#endif  
        
    protected: // Functions from base classes
        
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
        
    protected: 
        
        /**
        * ChspsRomInstaller.
        * Performs the first phase of two phase construction.
        * @since S60 5.0
        * @param aObserver The object to be used to handle updates from the server.
        * @param aFsSession is a reference to file server instance
        */
        ChspsRomInstaller( 
                ChspsThemeServer& aThemeServer,
                RFs& aFsSession );

        /**
        * ConstructL.
        * Performs the second phase construction of a ChspsRomInstaller object.
        * @since S60 5.0
        */
        void ConstructL();           
        
        /**
         * SetImportsFilterL.
         * Finds specific imports ("plugin_*.dat"/"app_*.dat" files) from ROM drive's 
         * import folder. Search results are appended into iImportsArray member.
         * @since S60 5.0
         * @param aFileFilter is a filter for finding the imports
         * @param 
         * @return ETrue if files were found and added into the array
         */
        TBool SetImportsFilterL(      
                const TDesC& aFileFilter );
        
    private: // Data    
    	               
        ChspsThemeServer& iThemeServer;
        
        // Required by the file copying process
        RFs& iFsSession;
        
        // Installation handler
        ChspsInstallationHandler* iInstallationHandler;
        
        // Required by the installation process
        TBuf8<KMaxHeaderDataLength8> iHeaderData;
        
        // An array of found *.DAT files
        RPointerArray<HBufC> iImportsArrayV1;                              
        
        // Results of the previous installation 
        ThspsServiceCompletedMessage iRet;
        
#ifdef HSPS_LOG_ACTIVE        
        /**
         * Log bus. Not owned.
         */        
        ChspsLogBus* iLogBus;
#endif        
     };


#endif //__hspsROMINSTALLER_H__

// End of File
