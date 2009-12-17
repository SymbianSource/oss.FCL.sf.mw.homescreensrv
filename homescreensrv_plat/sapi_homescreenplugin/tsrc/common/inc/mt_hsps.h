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
* Description:  Defination of class MT_CHSPS
*
*/


#ifndef C_MT_HSPS_H
#define C_MT_HSPS_H

#include <ceunittestsuiteclass.h>
#include <eunitdecorators.h>
#include <liwservicehandler.h>
#include <hspsthememanagement.h>

// Service API interface literals
_LIT8( KHSPS, "Service.HSPS" );
_LIT8( KHSPSConfigurationIf, "IConfiguration" );
_LIT8( KHSPSTestAppUid, "536874929");

// Configuration installation files
_LIT( KHSPSInstallMinimalRootConf, "c:\\data\\mt_hsps\\minimalconf\\root\\manifest.dat");
_LIT( KHSPSInstallMinimalViewConf, "c:\\data\\mt_hsps\\minimalconf\\view\\manifest.dat");
_LIT( KHSPSInstallMinimalWidgetConf, "c:\\data\\mt_hsps\\minimalconf\\widget\\manifest.dat");
_LIT( KHSPSInstallTypicalRootConf, "c:\\data\\mt_hsps\\typicalconf\\root\\manifest.dat");
_LIT( KHSPSInstallTypicalView1Conf, "c:\\data\\mt_hsps\\typicalconf\\view1\\manifest.dat");
_LIT( KHSPSInstallTypicalView2Conf, "c:\\data\\mt_hsps\\typicalconf\\view2\\manifest.dat");
_LIT( KHSPSInstallTypicalWidgetConf, "c:\\data\\mt_hsps\\typicalconf\\widget\\manifest.dat");
_LIT( KHSPSInstallOperatorRootConf, "c:\\data\\mt_hsps\\operatorconf\\root\\manifest.dat");
_LIT( KHSPSInstallOperatorViewConf, "c:\\data\\mt_hsps\\operatorconf\\view\\manifest.dat");
_LIT( KHSPSInstallFinnishWidgetConf, "c:\\data\\mt_hsps\\finnish_widget\\manifest.dat");
_LIT( KHSPSInstallInstalledWidgetConf, "c:\\data\\mt_hsps\\installed_widget\\manifest.dat");
_LIT( KHSPSInstallInstalledWidgetConfV2, "c:\\data\\mt_hsps\\installed_widget_v2\\manifest.dat");

// Triggers for notifications
enum MT_THSPSTrigger
    {
    EHspsTriggerRunNothing = 0, 
    EHspsTriggerRunRemovePluginCase,
    EHspsTriggerRunAddPluginCase,            
    EHspsTriggerRunInstallationCase,
    EHspsTriggerRunSetActivePluginCase,
    EHspsTriggerRunReplacePluginCase,
    EHspsTriggerRunUpdateInstallationCase,
    EHspsTriggerRunUninstallationCase
    };

// Active configuration ids
const TInt KHSPSMTAppUid( 0x20000FB1 );
const TInt KHSPSMTInterfaceUid( 0x998 );
const TInt KHSPSActiveConfMinimal( 0x2000B100 );
const TInt KHSPSActiveConfTypical( 0x2000B110 );
const TInt KHSPSActiveConfOperator( 0x2000B130 );
const TInt KHSPSMinimalViewConf( 0x2000B101 );
const TInt KHSPSMinimalWidgetConf( 0x2000B102 );
const TInt KHSPSTypicalView1Conf( 0x2000B111 );
const TInt KHSPSTypicalView2Conf( 0x2000B112 );
const TInt KHSPSTypicalWidgetConf( 0x2000B120 );
const TInt KHSPSOperatorViewConf( 0x2000B131 );
const TInt KHSPSFinnishWidgetConf( 0x2000B132 );
const TInt KHSPSInstalledWidgetConf( 0x2000B133 );

class MT_CHSPSResource
    : public CBase
    {
    public:
        MT_CHSPSResource(
            const TDesC& aPath, 
            TInt aExpectedSize );
        
        virtual ~MT_CHSPSResource();
    public:
        TFileName iPath;
        TInt iSize;    
    };
    
/**
 * This class is an abstract class of HSPS SAPI tester classes 
*/
NONSHARABLE_CLASS( MT_CHSPS )
	: public CEUnitTestSuiteClass, public MhspsThemeManagementServiceObserver
    {
    protected:     // Constructors and destructors

        MT_CHSPS();

        virtual ~MT_CHSPS();

    protected:    // New methods
         
        void RunTestStepSyncL(
            TUint8* aMethod,
            TUint8* aInParams,
            TUint8* aExpOutParams,
            TBool aMeasureExecTime = EFalse );

        void RunTestStepAsyncL(
            TUint8* aMethod,
            TUint8* aInParams,
            TUint8* aExpOutParams,
            TInt aTriggerCase );

        void SetActiveConfigurationL(
            const TDesC8& aAppUid,
            TInt aConfUid );

        void ResetResources();
        
        void AddResourceL(             
            const TDesC& aFileName,
            const TInt aExpectedSize );
        
        void CheckResourcesL();
        
        // Request Notification testing purposes
        void RunNotificationTriggerL( TInt aTriggerCase );
                
        void RunRemovePluginCase();
                
        void RunAddPluginCase();
                
        void RunInstallationCase();
        
        void RunUpdateInstallationCase();
        
        void RunUninstallationCase();        

        void RunSetActivePluginCase();

        void RunReplacePluginCase();

        void RestoreTestDataL( TInt aTriggerCase );
        
        void RestoreRemovedPluginCase();

        void RestoreAddedPluginCase();

        void StartTiming(
            TDesC8& aMethod );

        void StopTiming();

        void SetCentralRepositoryKeyL(
            TInt aKey,
            TInt aValue );
        
        void RemoveResourceFilesL( RFs& aFileSession );

    public: // From base class MhspsThemeManagementServiceObserver

        /**
         * Realisation of MhspsThemeManagementServiceObserver Interface
         * @param aMessage contains a service completion message
         */
        void HandlehspsClientMessage( ThspsServiceCompletedMessage aMessage );

    public:
        /**
         * Attachs SAPI client to a service.
         * @param aSapi SAPI designation.
         * @param aInterface Interface name.
         * @param aAppUid Application Identifier.
         */
        void AttachServiceL( 
            const TDesC8& aSapi, 
            const TDesC8& aInterface,
            const TDesC8& aAppUid,
            const TBool aUseTiming = EFalse );

        void DetachServiceL();

        MLiwInterface* Interface();
    protected:    // Data

        CLiwServiceHandler* iServiceHandler;
        MLiwInterface* iInterface;
        CLiwCriteriaItem* iService;
        // This is only needed in request notification testing when restoring test data
        TInt iAddedPluginId;
        TTime iStartTime;
        RPointerArray<MT_CHSPSResource> iResourceArray;

    };

#endif      //  C_MT_HSPS_H

// End of file
