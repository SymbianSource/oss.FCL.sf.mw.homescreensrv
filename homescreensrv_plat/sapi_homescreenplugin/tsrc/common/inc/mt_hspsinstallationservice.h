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
* Description:  Theme handler class declaration
*
*/


#ifndef C_MT_HSPSINSTALLATIONSERVICE_H
#define C_MT_HSPSINSTALLATIONSERVICE_H


// SYSTEM INCLUDES

#include <e32base.h>
#include <hspsthememanagement.h>


// USER INCLUDES


// FORWARD DECLARATIONS
class ChspsDomNode;
class ChspsResult;
class ChspsODT;
class ChspsClient;

/**
  * This class provides HSPS installation services 
 */
class MT_CHspsInstallationService : public CBase, public MhspsThemeManagementServiceObserver
    {
    public: // Constructor and destructor

        /**
         * Two-phased constructor.
         * @return CHspsConfigurationService*
         */
        static MT_CHspsInstallationService* NewL();
    
        /**
         * Destructor.
         */
        virtual ~MT_CHspsInstallationService();

    private: // Constructors

        /**
         * Constructor
        */
        MT_CHspsInstallationService();
    
        /**
         * Symbian Constructor
        */
        void ConstructL();


    public: // From base class MhspsThemeManagementServiceObserver

       
        /**
         * Realisation of MhspsThemeManagementServiceObserver Interface
         * @param aMessage contains a service completion message
         */
        void HandlehspsClientMessage( ThspsServiceCompletedMessage aMessage );

    public: // New methods
       
        void InstallConfigurationL( const TDesC& aManifestFileName );
        void UninstallConfigurationL( TInt aAppUid, TInt aConfUid );
                
        
    private: // Methods
        void InstallThemeL(const TDesC& aManifestFileName);

    private: // Data        

        // Pointer to HSPS Theme server client object
        ChspsClient*                   iHspsClient;
        TBuf8<KMaxHeaderDataLength8>   iHeaderData;
        TRequestStatus                 iStatus;      
        ChspsResult*                   iResult;
        ChspsODT*                      iHeader;
        CActiveSchedulerWait*          iLoop;
    };

#endif // C_MT_HSPSINSTALLATIONSERVICE_H

// End of File.
