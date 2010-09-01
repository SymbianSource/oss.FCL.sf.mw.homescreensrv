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
* Description:  AutoInstaller installs theme from given location. It is intended
*                to be used by hspsThemeServer directly. AutoInstaller is initiated
*                when FileChangeListener finds installable theme source on given
*                location. Theme source is set during the SISX-package
*                exctraction. AutoInstaller is deployed by calling its function 
*                InstallThemeL with manifest-file of the installable theme source set.
*                AutoInstaller deploys hspsInstallationHandler. hspsInstallationHandler
*                is asynchronous object, there for AutoInstaller is asynchronous object
*                as well capable to deploy asynchronous installation.
*                When installation ends, AutoInstaller calls its observer with 
*                ThspsServiceCompletedMessage-type parameter. Observer is given on
*                initialising of AutoInstaller and it is allways ChspsThemeServer-type.
*                AutoInstaller have functions to get ODT-header of successfully installed
*                theme and ChspsResult-typed object expressing errors in unsuccessful cases.   
*
*/


#ifndef __hspsAUTOINSTALLER_H__
#define __hspsAUTOINSTALLER_H__

#include <e32base.h>
#include "hspsthememanagement.h"

#ifdef HSPS_LOG_ACTIVE
class ChspsLogBus;
#endif
class ChspsODT;
class ChspsResult;
class ChspsThemeServer;
class ChspsMaintenanceHandler;
class ChspsInstallationHandler;

/**
*  @ingroup group_hspsserver
*  @lib hspsThemeServer.exe
*  @since S60 5.0
*/
class ChspsAutoInstaller : public CActive
    {
    public: // Constructors and destructors
        /**
        * NewL.
        * Two-phased constructor.
        * Creates a ChspsAutoInstaller object using two phase construction,
        * and return a pointer to the created object.
        * @param aObserver The object to be used to handle updates from the server.
        * @return A pointer to the created instance of ChspsAutoInstaller.
        */
        static ChspsAutoInstaller* NewL( ChspsThemeServer& aThemeServer );

        /**
        * NewLC.
        * Two-phased constructor.
        * Creates a ChspsClinet object using two phase construction,
        * and return a pointer to the created object.
        * @param aObserver The object to be used to handle updates from the server.
        * @return a pointer to the created instance of ChspsAutoInstaller.
        */
        static ChspsAutoInstaller* NewLC( ChspsThemeServer& aThemeServer );

        /**
        * ~ChspsAutoInstaller.
        * Destructor.
        * Destroys the object and release all memory objects.
        */
        virtual ~ChspsAutoInstaller();

    public: // New functions

        /**
        * UnInstallThemeL
        * @since S60 3.2
        * @param aPathFile a path_*.dat file which describes the theme to be uninstalled
        */
        void UnInstallThemeL( const TDesC& aPathFile );
        
        /**
        * InstallTheme
        * @since S60 3.1
        * @param aManifestFileName is a name of the manifest file of theme source to be installed.
        */
        TInt InstallThemeL( const TDesC& aManifestFileName ); 
                
        /**
        * GethspsResultL
        * @since S60 3.1
        * @param aResult is a ChspsResult object containing additional information of the result of query.
        */
        ChspsResult& hspsResult();
        
#ifdef HSPS_LOG_ACTIVE        
        /** 
        * Set log bus.
        */
        void SetLogBus( ChspsLogBus* aLogBus );
#endif        

    protected: // Functions from base classes

        /**
        * From CActive, RunL.
        * Callback function.
        * Invoked to handle responses from the server.
        */
        void RunL();

        /**
        * From CActive, DoCancel.
        * Cancels any outstanding operation.
        */
        void DoCancel();
        
        /**
        * From CActive Called when asynchronous request has failed
        * @since S60 3.1
        */
        TInt RunError( TInt aError );

     private: // Constructors and destructors

        /**
        * ChspsAutoInstaller.
        * Performs the first phase of two phase construction.
        * @param aObserver The object to be used to handle updates from the server.
        */
        ChspsAutoInstaller( ChspsThemeServer& aThemeServer );

        /**
        * ConstructL.
        * Performs the second phase construction of a ChspsAutoInstaller object.
        */
        void ConstructL();

   
    private: // Members
    
        
    private: // Data    
        
        TBuf8<KMaxHeaderDataLength8> iHeaderData;
        ChspsThemeServer& iThemeServer;
        ChspsResult* iResult;
        ChspsInstallationHandler* iInstallationHandler;
        ChspsMaintenanceHandler* iMaintenanceHandler;
#ifdef HSPS_LOG_ACTIVE        
        /**
         * Log bus. Not owned.
         */        
        ChspsLogBus* iLogBus;
#endif
        
     };


#endif //__hspsAUTOINSTALLER_H__

// End of File
