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
         * Installs all plugin configurations which can be found from ROM and C drives (UDA).         
         * @since S60 5.2         
         */
        void InstallL();
        
        /**
         * Installs a plugin configuration from the provided manifest file (synchronous).         
         * @since S60 5.0
         * @param aFileName is name of the manifest file to be installed.
         * @return ThspsServiceCompletedMessage error code 
         */
        ThspsServiceCompletedMessage InstallThemeL( 
                const TDesC& aFileName ); 
        
        /**
         * Re-installs an application plugin configuration (synchronous).         
         * @since S60 5.0
         * @param aAppUid is UID of an application 
         * @param aConfigurationUid is UID of an application plugin configuration
         * @return ThspsServiceCompletedMessage error code 
         */
        ThspsServiceCompletedMessage ReinstallThemeL(
                const TInt aAppUid,
                const TInt aConfigurationUid );
                        
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
         * Retrieves manifest files from both Z and C drives located in
         * \\private\200159C0\install\ paths.
         * @since S60 5.0
         */
        void FindInstallationFilesL(  
                RPointerArray<HBufC>& aFolders );
        
        void DoFindInstallationFilesL(  
                RPointerArray<HBufC>& aFolders,
                const TDesC& aPath );
        
        /**
         * Finds an installation file from a directory structure 
         * which has UID in hex format as folder name.
         * @since S60 5.2
         * @param aConfigurationUid Configuration to be found
         * @param aManifest Found manifest file
         */
        void FindInstallationFileL(  
                const TInt aConfigurationUid,
                TFileName& aManifest );
        
    private: // Data    
    	               
        ChspsThemeServer& iThemeServer;
        
        // Required by the file copying process
        RFs& iFsSession;
        
        // Installation handler
        ChspsInstallationHandler* iInstallationHandler;
        
        // Required by the installation process
        TBuf8<KMaxHeaderDataLength8> iHeaderData;                                    
        
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
