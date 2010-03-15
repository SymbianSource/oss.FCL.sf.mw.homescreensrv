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
* Description:  Implementation of MT_CHSPSConfigurationIf class
*
*/

#include <s32mem.h>
#include <eunitmacros.h>
#include <eunitdecorators.h>
#include <eunituids.h>
#include <apgtask.h>
#include <eikenv.h>
#include <centralrepository.h>
#include <bautils.h>
#include "mt_hspsconfigurationif.h"
#include "mt_hspsliwlistprinter.h"
#include "mt_hspsinstallationservice.h"

#include "liwgenericparam.hrh"

// get active app conf
#include "mt_hsps_getactiveappconf_1.h"
#include "mt_hsps_getactiveappconf_2.h"
#include "mt_hsps_getactiveappconf_3.h"
// get plugin conf
#include "mt_hsps_getpluginconf_1.h"
#include "mt_hsps_getpluginconf_2.h"
#include "mt_hsps_getpluginconf_3.h"
#include "mt_hsps_getpluginconf_4.h"
#include "mt_hsps_getpluginconf_5.h"
// get plugins
#include "mt_hsps_getplugins_1.h"
#include "mt_hsps_getplugins_2.h"
#include "mt_hsps_getplugins_3.h"
#include "mt_hsps_getplugins_4.h"
#include "mt_hsps_getplugins_5.h"
#include "mt_hsps_getplugins_6.h"
#include "mt_hsps_getplugins_7.h"
// get plugin list
#include "mt_hsps_getpluginlist_1.h"
#include "mt_hsps_getpluginlist_2.h"
#include "mt_hsps_getpluginlist_3.h"
#include "mt_hsps_getpluginlist_4.h"
#include "mt_hsps_getpluginlist_5.h"
#include "mt_hsps_getpluginlist_6.h"
#include "mt_hsps_getpluginlist_7.h"
#include "mt_hsps_getpluginlist_8.h"
// add plugin
#include "mt_hsps_addplugin_1.h"
#include "mt_hsps_addplugin_2.h"
#include "mt_hsps_addplugin_3.h"
#include "mt_hsps_addplugin_4.h"
#include "mt_hsps_addplugin_5.h"
#include "mt_hsps_addplugin_6.h"
#include "mt_hsps_addplugin_7.h"
#include "mt_hsps_addplugin_8.h"
#include "mt_hsps_addplugin_9.h"
#include "mt_hsps_addplugin_10.h"
#include "mt_hsps_addplugin_11.h"
// remove plugin
#include "mt_hsps_removeplugin_1.h"
#include "mt_hsps_removeplugin_2.h"
#include "mt_hsps_removeplugin_3.h"
#include "mt_hsps_removeplugin_4.h"
// set plugin settings
#include "mt_hsps_setpluginsettings_1.h"
#include "mt_hsps_setpluginsettings_2.h"
#include "mt_hsps_setpluginsettings_3.h"
#include "mt_hsps_setpluginsettings_4.h"
#include "mt_hsps_setpluginsettings_5.h"
#include "mt_hsps_setpluginsettings_6.h"
// get plugin setting
#include "mt_hsps_getpluginsettings_1.h"
#include "mt_hsps_getpluginsettings_2.h"
#include "mt_hsps_getpluginsettings_3.h"
// mvoe plugins
#include "mt_hsps_moveplugins_1.h"
#include "mt_hsps_moveplugins_2.h"
#include "mt_hsps_moveplugins_3.h"
#include "mt_hsps_moveplugins_4.h"
#include "mt_hsps_moveplugins_5.h"
#include "mt_hsps_moveplugins_6.h"
// replace plugin
#include "mt_hsps_replaceplugin_1.h"
// get app confs
#include "mt_hsps_getappconfs_1.h"
// set app confs
#include "mt_hsps_setactiveappconf_1.h"
#include "mt_hsps_setactiveappconf_2.h"
#include "mt_hsps_setactiveappconf_3.h"
#include "mt_hsps_setactiveappconf_4.h"
// request notification
#include "mt_hsps_requestnotify_1.h"
#include "mt_hsps_requestnotify_2.h"
#include "mt_hsps_requestnotify_3.h"
#include "mt_hsps_requestnotify_4.h"
#include "mt_hsps_requestnotify_5.h"
#include "mt_hsps_requestnotify_6.h"
// set active plugin
#include "mt_hsps_setactiveplugin_1.h"
#include "mt_hsps_setactiveplugin_2.h"
#include "mt_hsps_setactiveplugin_3.h"
#include "mt_hsps_setactiveplugin_4.h"
#include "mt_hsps_setactiveplugin_5.h"
#include "mt_hsps_setactiveplugin_6.h"
// restore configurations
#include "mt_hsps_restoreconfigurations_1.h"
// customizations
#include "mt_hsps_customization_1.h"
// restoractiveeappconf
#include "mt_hsps_restoreactiveappconf_1.h"
#include "mt_hsps_restoreactiveappconf_2.h"

// ======== LOCAL CONSTANTS ====================================================

// Resource files copied by minimal configuration.
_LIT( KMinimalResourceFile1, "c:\\private\\20000fb1\\2456\\270513751\\536916225\\1.0\\sources\\main.xml"  );
_LIT( KMinimalResourceFile2, "c:\\private\\20000fb1\\2456\\270513751\\536916225\\1.0\\sources\\viewnavigationrules.xml"  );
_LIT( KMinimalResourceFile3, "c:\\private\\20000fb1\\2456\\270513751\\536916225\\1.0\\sources\\resource.file"  );
_LIT( KMinimalResourceFile4, "c:\\private\\20000fb1\\2456\\270513751\\536916225\\1.0\\sources\\picture.jpeg"  );

// ======== LOCAL FUNCTIONS ====================================================

// ======== MEMBER FUNCTIONS ===================================================

//------------------------------------------------------------------------------
// Two-phased constructor.
//------------------------------------------------------------------------------
MT_CHSPSConfigurationIf* MT_CHSPSConfigurationIf::NewL()
    {
    MT_CHSPSConfigurationIf* self = new( ELeave ) MT_CHSPSConfigurationIf();
    CleanupStack::PushL( self );
    self->ConstructL();
    CleanupStack::Pop( self );

    return self;
    }

//------------------------------------------------------------------------------
// Destructor.
//------------------------------------------------------------------------------   
MT_CHSPSConfigurationIf::~MT_CHSPSConfigurationIf()
    {
    iFileserver.Close();
    }

//------------------------------------------------------------------------------
// Constructor
//------------------------------------------------------------------------------
MT_CHSPSConfigurationIf::MT_CHSPSConfigurationIf()
    {
    }

//------------------------------------------------------------------------------
// Symbian Constructor
//------------------------------------------------------------------------------   
void MT_CHSPSConfigurationIf::ConstructL()
    {
    // The ConstructL from the base class CEUnitTestSuiteClass must be called.
    // It generates the test case table.
    CEUnitTestSuiteClass::ConstructL();

    User::LeaveIfError( iFileserver.Connect() );            
    }

//------------------------------------------------------------------------------
// Remove resources from client's private folder
//------------------------------------------------------------------------------
void MT_CHSPSConfigurationIf::ResetClientFolderL()
    {
    // Remove all resource files from the test client's private folder (files are 
    // copied back when an application configuration is fecthed)
    _LIT( KClientFolder, "c:\\private\\102750f0\\");
    TFileName folderPath;
    folderPath.Copy( KClientFolder );    
    if( BaflUtils::FolderExists( iFileserver, folderPath ) )
        {        
        CFileMan* fileMan = CFileMan::NewL( iFileserver );
        CleanupStack::PushL( fileMan );
        
        folderPath.Append( _L("2456\\") );
        if( BaflUtils::FolderExists( iFileserver, folderPath ) )
            {
            User::LeaveIfError( fileMan->RmDir( folderPath ) );
            }
        
        folderPath.Copy( KClientFolder );
        folderPath.Append( _L("271012080\\") );
        if( BaflUtils::FolderExists( iFileserver, folderPath ) )
            {
            User::LeaveIfError( fileMan->RmDir( folderPath ) );
            }
        
        CleanupStack::PopAndDestroy( fileMan );
        }    
    }

//------------------------------------------------------------------------------
// Initialize test case
//------------------------------------------------------------------------------    
void MT_CHSPSConfigurationIf::SetupL()
    {
    MT_CHspsInstallationService* installationService = MT_CHspsInstallationService::NewL();
    CleanupStack::PushL( installationService );

    // Uninstall possible test configurations
    installationService->UninstallConfigurationL( KHSPSMTAppUid, KHSPSActiveConfMinimal );
    installationService->UninstallConfigurationL( KHSPSMTInterfaceUid, KHSPSMinimalViewConf );
    installationService->UninstallConfigurationL( KHSPSMTInterfaceUid, KHSPSMinimalWidgetConf );
    installationService->UninstallConfigurationL( KHSPSMTAppUid, KHSPSActiveConfTypical );
    installationService->UninstallConfigurationL( KHSPSMTInterfaceUid, KHSPSTypicalView1Conf );
    installationService->UninstallConfigurationL( KHSPSMTInterfaceUid, KHSPSTypicalView2Conf );
    installationService->UninstallConfigurationL( KHSPSMTInterfaceUid, KHSPSTypicalWidgetConf );
    installationService->UninstallConfigurationL( KHSPSMTAppUid, KHSPSActiveConfOperator );
    installationService->UninstallConfigurationL( KHSPSMTInterfaceUid, KHSPSOperatorViewConf );
    installationService->UninstallConfigurationL( KHSPSMTInterfaceUid, KHSPSFinnishWidgetConf );
    installationService->UninstallConfigurationL( KHSPSMTInterfaceUid, KHSPSInstalledWidgetConf );

    // Remove test configurations from import & install folder at C drive
    CFileMan* fileManager = CFileMan::NewL( iFileserver );
    CleanupStack::PushL( fileManager );
    fileManager->Delete( _L( "c:\\private\\200159c0\\import\\plugin_0998_101FB657_2000B133.dat" ) );     
    fileManager->RmDir( _L( "c:\\private\\200159c0\\import\\0998\\" ) );
    fileManager->RmDir( _L( "d:\\data\\mt_hsps\\installed_widget\\widgetconfiguration.xml" ) );    
    CleanupStack::PopAndDestroy( fileManager );
    
    // Install test configurations    
    installationService->InstallConfigurationL( KHSPSInstallMinimalRootConf );
    installationService->InstallConfigurationL( KHSPSInstallMinimalViewConf );
    installationService->InstallConfigurationL( KHSPSInstallMinimalWidgetConf );
    installationService->InstallConfigurationL( KHSPSInstallTypicalRootConf );
    installationService->InstallConfigurationL( KHSPSInstallTypicalView1Conf );
    installationService->InstallConfigurationL( KHSPSInstallTypicalView2Conf );
    installationService->InstallConfigurationL( KHSPSInstallTypicalWidgetConf );
    installationService->InstallConfigurationL( KHSPSInstallOperatorRootConf );
    installationService->InstallConfigurationL( KHSPSInstallOperatorViewConf );
    installationService->InstallConfigurationL( KHSPSInstallFinnishWidgetConf );

    CleanupStack::PopAndDestroy( installationService );
    ResetResources();
    }

//------------------------------------------------------------------------------
// Deinitialize test case
//------------------------------------------------------------------------------   
void MT_CHSPSConfigurationIf::Teardown()
    {
    // Detach from HSPS 
    TRAPD( err, DetachServiceL() );
    // Remove test configuration resource files
    TRAP( err, RemoveResourceFilesL( iFileserver ) );
    if ( err != KErrNone )
    	{    	
    	EUNIT_FAIL_TEST( "Teardown failed." );
    	}
    }

//------------------------------------------------------------------------------
// Prints liw list to a file.
//------------------------------------------------------------------------------
void MT_CHSPSConfigurationIf::PrintReturnValuesL(  
    CLiwGenericParamList& aOutParamList )
    {
    CHspsLiwListPrinter* printer = CHspsLiwListPrinter::NewLC();
    printer->PrintLiwListL( aOutParamList );
    CleanupStack::PopAndDestroy( printer );
    }

//------------------------------------------------------------------------------
// Test case: GetActiveAppConf(1)
//------------------------------------------------------------------------------
void MT_CHSPSConfigurationIf::GetActiveAppConf_1_L()
    {    
    // Pre conditions
    //----------------------------------------------------------------
        
    // Remove old resources
    ResetClientFolderL();
    // Set active configuration to Minimal configuration
    EUNIT_PRINT( _L8( "Pre conditions: Set Active configuration Minimal" ) );
    SetActiveConfigurationL( KHSPSTestAppUid, KHSPSActiveConfMinimal );
    // Attach to HSPS 
    EUNIT_PRINT( _L8( "Pre conditions: Attach to HSPS service IConfiguration interface" ) );
    AttachServiceL( KHSPS, KHSPSConfigurationIf, KHSPSTestAppUid );
    
    // Test step 1
    //----------------------------------------------------------------
    EUNIT_PRINT( _L8( "Test step 1" ) );
    RunTestStepSyncL(
        ( TUint8* )getactiveappconf_1_ts_1_method,
        ( TUint8* )getactiveappconf_1_ts_1_input,
        ( TUint8* )getactiveappconf_1_ts_1_output,
        ETrue );
    EUNIT_PRINT( _L8( "Test step passed" ) );    

    // Test step 2
    EUNIT_PRINT( _L8( "Test step 2" ) );
    RunTestStepSyncL(
        ( TUint8* )getactiveappconf_1_ts_2_method,
        ( TUint8* )getactiveappconf_1_ts_2_input,
        ( TUint8* )getactiveappconf_1_ts_2_output,
        ETrue );
    EUNIT_PRINT( _L8( "Test step passed" ) );    

    // Test step 3
    EUNIT_PRINT( _L8( "Test step 3" ) );
    RunTestStepSyncL(
        ( TUint8* )getactiveappconf_1_ts_3_method,
        ( TUint8* )getactiveappconf_1_ts_3_input,
        ( TUint8* )getactiveappconf_1_ts_3_output );
    EUNIT_PRINT( _L8( "Test step passed" ) );    

    // Test step 4
    EUNIT_PRINT( _L8( "Test step 4" ) );
    EUNIT_PRINT( _L8( "DetachServiceL" ) );
    DetachServiceL();
    EUNIT_PRINT( _L8( "Set KCenrepKeyCheckConf key" ) );
    SetCentralRepositoryKeyL( 100000003, 1 );
    EUNIT_PRINT( _L8( "AttachServiceL" ) );
    AttachServiceL( KHSPS, KHSPSConfigurationIf, KHSPSTestAppUid );
    EUNIT_PRINT( _L8( "Test step passed" ) );    

    // Test step 5
    EUNIT_PRINT( _L8( "Test step 5" ) );
    RunTestStepSyncL(
        ( TUint8* )getactiveappconf_1_ts_5_method,
        ( TUint8* )getactiveappconf_1_ts_5_input,
        ( TUint8* )getactiveappconf_1_ts_5_output );
    EUNIT_PRINT( _L8( "Test step passed" ) );    

    // post condition check.
    //----------------------------------------------------------------
    EUNIT_PRINT( _L8( "post condition check for resource file copy" ) );
    ResetResources();
    AddResourceL( KMinimalResourceFile1, 136 );        
    AddResourceL( KMinimalResourceFile2, 17 ); 
    AddResourceL( KMinimalResourceFile3, 17 );
    AddResourceL( KMinimalResourceFile4, 18 );    
    CheckResourcesL();     
    EUNIT_PRINT( _L8( "post condition check for resource file copy passed" ) );
    
    }

//------------------------------------------------------------------------------
// Test case: GetActiveAppConf(2)
//------------------------------------------------------------------------------
void MT_CHSPSConfigurationIf::GetActiveAppConf_2_L()
    {
    
    // Pre conditions
    // Set active configuration to Minimal configuration
    EUNIT_PRINT( _L8( "Pre conditions: Set Active configuration Minimal" ) );
    SetActiveConfigurationL( KHSPSTestAppUid, KHSPSActiveConfMinimal );
    // Attach to HSPS 
    EUNIT_PRINT( _L8( "Pre conditions: Attach to HSPS service IConfiguration interface" ) );
    AttachServiceL( KHSPS, KHSPSConfigurationIf, KHSPSTestAppUid );
    
    // Test step 1
    EUNIT_PRINT( _L8( "Test step 1" ) );
    RunTestStepSyncL(
        ( TUint8* )getactiveappconf_2_ts_1_method,
        ( TUint8* )getactiveappconf_2_ts_1_input,
        ( TUint8* )getactiveappconf_2_ts_1_output );
    EUNIT_PRINT( _L8( "Test step passed" ) );    

    // Test step 2
    EUNIT_PRINT( _L8( "Test step 2" ) );
    RunTestStepSyncL(
        ( TUint8* )getactiveappconf_2_ts_2_method,
        ( TUint8* )getactiveappconf_2_ts_2_input,
        ( TUint8* )getactiveappconf_2_ts_2_output );
    EUNIT_PRINT( _L8( "Test step passed" ) );    

    // Test step 3
    EUNIT_PRINT( _L8( "Test step 3" ) );
    EUNIT_PRINT( _L8( "DetachServiceL" ) );
    DetachServiceL();
    EUNIT_PRINT( _L8( "Set KCenrepKeyCheckConf key" ) );
    SetCentralRepositoryKeyL( 100000003, 1 );
    EUNIT_PRINT( _L8( "AttachServiceL" ) );
    AttachServiceL( KHSPS, KHSPSConfigurationIf, KHSPSTestAppUid );
    EUNIT_PRINT( _L8( "Test step passed" ) );    
    
    // Test step 3
    EUNIT_PRINT( _L8( "Test step 4" ) );
    RunTestStepSyncL(
        ( TUint8* )getactiveappconf_2_ts_4_method,
        ( TUint8* )getactiveappconf_2_ts_4_input,
        ( TUint8* )getactiveappconf_2_ts_4_output );
    EUNIT_PRINT( _L8( "Test step passed" ) );    
    
    }

//------------------------------------------------------------------------------
// Test case: GetActiveAppConf(3)
//------------------------------------------------------------------------------
void MT_CHSPSConfigurationIf::GetActiveAppConf_3_L()
    {
    
    // Pre conditions
    // Set active configuration to Typical configuration
    EUNIT_PRINT( _L8( "Pre conditions: Set Active configuration Typical" ) );
    SetActiveConfigurationL( KHSPSTestAppUid, KHSPSActiveConfTypical );
    // Attach to HSPS 
    EUNIT_PRINT( _L8( "Pre conditions: Attach to HSPS service IConfiguration interface" ) );
    AttachServiceL( KHSPS, KHSPSConfigurationIf, KHSPSTestAppUid );
    
    // Test step 1
    EUNIT_PRINT( _L8( "Test step 1" ) );
    RunTestStepSyncL(
        ( TUint8* )getactiveappconf_3_ts_1_method,
        ( TUint8* )getactiveappconf_3_ts_1_input,
        ( TUint8* )getactiveappconf_3_ts_1_output );
    EUNIT_PRINT( _L8( "Test step passed" ) );    

    // Test step 2
    EUNIT_PRINT( _L8( "Test step 2" ) );
    RunTestStepSyncL(
        ( TUint8* )getactiveappconf_3_ts_2_method,
        ( TUint8* )getactiveappconf_3_ts_2_input,
        ( TUint8* )getactiveappconf_3_ts_2_output );
    EUNIT_PRINT( _L8( "Test step passed" ) );    

    // Test step 3
    EUNIT_PRINT( _L8( "Test step 3" ) );
    EUNIT_PRINT( _L8( "DetachServiceL" ) );
    DetachServiceL();
    EUNIT_PRINT( _L8( "Set KCenrepKeyCheckConf key" ) );
    SetCentralRepositoryKeyL( 100000003, 1 );
    EUNIT_PRINT( _L8( "AttachServiceL" ) );
    AttachServiceL( KHSPS, KHSPSConfigurationIf, KHSPSTestAppUid );
    EUNIT_PRINT( _L8( "Test step passed" ) );    

    // Test step 4
    EUNIT_PRINT( _L8( "Test step 3" ) );
    RunTestStepSyncL(
        ( TUint8* )getactiveappconf_3_ts_4_method,
        ( TUint8* )getactiveappconf_3_ts_4_input,
        ( TUint8* )getactiveappconf_3_ts_4_output );
    EUNIT_PRINT( _L8( "Test step passed" ) );    
    
    }

//------------------------------------------------------------------------------
// Test case: GetPluginConf(1)
//------------------------------------------------------------------------------
void MT_CHSPSConfigurationIf::GetPluginConf_1_L()
    {
    // Pre conditions
    // Set active configuration to Minimal configuration
    EUNIT_PRINT( _L8( "Pre conditions: Set Active configuration Minimal" ) );
    SetActiveConfigurationL( KHSPSTestAppUid, KHSPSActiveConfMinimal );
    // Attach to HSPS 
    EUNIT_PRINT( _L8( "Pre conditions: Attach to HSPS service IConfiguration interface" ) );
    AttachServiceL( KHSPS, KHSPSConfigurationIf, KHSPSTestAppUid );
    
    // Test step 1
    EUNIT_PRINT( _L8( "Test step 1" ) );
    RunTestStepSyncL(
        ( TUint8* )getpluginconf_1_ts_1_method,
        ( TUint8* )getpluginconf_1_ts_1_input,
        ( TUint8* )getpluginconf_1_ts_1_output,
        ETrue );
    EUNIT_PRINT( _L8( "Test step passed" ) );    

    // Test step 2
    EUNIT_PRINT( _L8( "Test step 2" ) );
    RunTestStepSyncL(
        ( TUint8* )getpluginconf_1_ts_2_method,
        ( TUint8* )getpluginconf_1_ts_2_input,
        ( TUint8* )getpluginconf_1_ts_2_output );
    EUNIT_PRINT( _L8( "Test step passed" ) );    

    // Test step 3
    EUNIT_PRINT( _L8( "Test step 3" ) );
    RunTestStepSyncL(
        ( TUint8* )getpluginconf_1_ts_3_method,
        ( TUint8* )getpluginconf_1_ts_3_input,
        ( TUint8* )getpluginconf_1_ts_3_output );
    EUNIT_PRINT( _L8( "Test step passed" ) );    

    // Test step 4
    EUNIT_PRINT( _L8( "Test step 4" ) );
    EUNIT_PRINT( _L8( "DetachServiceL" ) );
    DetachServiceL();
    EUNIT_PRINT( _L8( "Set KCenrepKeyCheckConf key" ) );
    SetCentralRepositoryKeyL( 100000003, 1 );
    EUNIT_PRINT( _L8( "AttachServiceL" ) );
    AttachServiceL( KHSPS, KHSPSConfigurationIf, KHSPSTestAppUid );
    EUNIT_PRINT( _L8( "Test step passed" ) );    

    // Test step 5
    EUNIT_PRINT( _L8( "Test step 5" ) );
    RunTestStepSyncL(
        ( TUint8* )getpluginconf_1_ts_5_method,
        ( TUint8* )getpluginconf_1_ts_5_input,
        ( TUint8* )getpluginconf_1_ts_5_output );
    EUNIT_PRINT( _L8( "Test step passed" ) );    

    }

//------------------------------------------------------------------------------
// Test case: GetPluginConf(2)
//------------------------------------------------------------------------------
void MT_CHSPSConfigurationIf::GetPluginConf_2_L()
    {
    // Pre conditions
    // Set active configuration to Minimal configuration
    EUNIT_PRINT( _L8( "Pre conditions: Set Active configuration Minimal" ) );
    SetActiveConfigurationL( KHSPSTestAppUid, KHSPSActiveConfMinimal );
    // Attach to HSPS 
    EUNIT_PRINT( _L8( "Pre conditions: Attach to HSPS service IConfiguration interface" ) );
    AttachServiceL( KHSPS, KHSPSConfigurationIf, KHSPSTestAppUid );
    
    // Test step 1
    EUNIT_PRINT( _L8( "Test step 1" ) );
    RunTestStepSyncL(
        ( TUint8* )getpluginconf_2_ts_1_method,
        ( TUint8* )getpluginconf_2_ts_1_input,
        ( TUint8* )getpluginconf_2_ts_1_output );
    EUNIT_PRINT( _L8( "Test step passed" ) );    
    
    }

//------------------------------------------------------------------------------
// Test case: GetPluginConf(3)
//------------------------------------------------------------------------------
void MT_CHSPSConfigurationIf::GetPluginConf_3_L()
    {
    // Pre conditions
    // Set active configuration to Minimal configuration
    EUNIT_PRINT( _L8( "Pre conditions: Set Active configuration Minimal" ) );
    SetActiveConfigurationL( KHSPSTestAppUid, KHSPSActiveConfMinimal );
    // Attach to HSPS 
    EUNIT_PRINT( _L8( "Pre conditions: Attach to HSPS service IConfiguration interface" ) );
    AttachServiceL( KHSPS, KHSPSConfigurationIf, KHSPSTestAppUid );
    
    // Test step 1
    EUNIT_PRINT( _L8( "Test step 1" ) );
    RunTestStepSyncL(
        ( TUint8* )getpluginconf_3_ts_1_method,
        ( TUint8* )getpluginconf_3_ts_1_input,
        ( TUint8* )getpluginconf_3_ts_1_output );
    EUNIT_PRINT( _L8( "Test step passed" ) );    
    
    }

//------------------------------------------------------------------------------
// Test case: GetPluginConf(4)
//------------------------------------------------------------------------------
void MT_CHSPSConfigurationIf::GetPluginConf_4_L()
    {
    // Pre conditions
    // Set active configuration to Minimal configuration
    EUNIT_PRINT( _L8( "Pre conditions: Set Active configuration Minimal" ) );
    SetActiveConfigurationL( KHSPSTestAppUid, KHSPSActiveConfMinimal );
    // Attach to HSPS 
    EUNIT_PRINT( _L8( "Pre conditions: Attach to HSPS service IConfiguration interface" ) );
    AttachServiceL( KHSPS, KHSPSConfigurationIf, KHSPSTestAppUid );
    
    // Test step 1
    EUNIT_PRINT( _L8( "Test step 1" ) );
    RunTestStepSyncL(
        ( TUint8* )getpluginconf_4_ts_1_method,
        ( TUint8* )getpluginconf_4_ts_1_input,
        ( TUint8* )getpluginconf_4_ts_1_output );
    EUNIT_PRINT( _L8( "Test step passed" ) );    

    // Test step 2
    EUNIT_PRINT( _L8( "Test step 2" ) );
    RunTestStepSyncL(
        ( TUint8* )getpluginconf_4_ts_2_method,
        ( TUint8* )getpluginconf_4_ts_2_input,
        ( TUint8* )getpluginconf_4_ts_2_output );
    EUNIT_PRINT( _L8( "Test step passed" ) );    

    // Test step 3
    EUNIT_PRINT( _L8( "Test step 3" ) );
    EUNIT_PRINT( _L8( "DetachServiceL" ) );
    DetachServiceL();
    EUNIT_PRINT( _L8( "Set KCenrepKeyCheckConf key" ) );
    SetCentralRepositoryKeyL( 100000003, 1 );
    EUNIT_PRINT( _L8( "AttachServiceL" ) );
    AttachServiceL( KHSPS, KHSPSConfigurationIf, KHSPSTestAppUid );
    EUNIT_PRINT( _L8( "Test step passed" ) );    

    // Test step 4
    EUNIT_PRINT( _L8( "Test step 4" ) );
    RunTestStepSyncL(
        ( TUint8* )getpluginconf_4_ts_4_method,
        ( TUint8* )getpluginconf_4_ts_4_input,
        ( TUint8* )getpluginconf_4_ts_4_output );
    EUNIT_PRINT( _L8( "Test step passed" ) );    

    }

//------------------------------------------------------------------------------
// Test case: GetPluginConf(5)
//------------------------------------------------------------------------------
void MT_CHSPSConfigurationIf::GetPluginConf_5_L()
    {
    // Pre conditions
    // Set active configuration to Minimal configuration
    EUNIT_PRINT( _L8( "Pre conditions: Set Active configuration Minimal" ) );
    SetActiveConfigurationL( KHSPSTestAppUid, KHSPSActiveConfMinimal );
    // Attach to HSPS 
    EUNIT_PRINT( _L8( "Pre conditions: Attach to HSPS service IConfiguration interface" ) );
    AttachServiceL( KHSPS, KHSPSConfigurationIf, KHSPSTestAppUid );
    
    // Test step 1
    EUNIT_PRINT( _L8( "Test step 1" ) );
    RunTestStepSyncL(
        ( TUint8* )getpluginconf_5_ts_1_method,
        ( TUint8* )getpluginconf_5_ts_1_input,
        ( TUint8* )getpluginconf_5_ts_1_output );
    EUNIT_PRINT( _L8( "Test step passed" ) );    

    // Test step 2
    EUNIT_PRINT( _L8( "Test step 2" ) );
    RunTestStepSyncL(
        ( TUint8* )getpluginconf_5_ts_2_method,
        ( TUint8* )getpluginconf_5_ts_2_input,
        ( TUint8* )getpluginconf_5_ts_2_output );
    EUNIT_PRINT( _L8( "Test step passed" ) );    

    // Test step 3
    EUNIT_PRINT( _L8( "Test step 3" ) );
    RunTestStepSyncL(
        ( TUint8* )getpluginconf_5_ts_3_method,
        ( TUint8* )getpluginconf_5_ts_3_input,
        ( TUint8* )getpluginconf_5_ts_3_output );
    EUNIT_PRINT( _L8( "Test step passed" ) );    

    // Test step 4
    EUNIT_PRINT( _L8( "Test step 4" ) );
    EUNIT_PRINT( _L8( "DetachServiceL" ) );
    DetachServiceL();
    EUNIT_PRINT( _L8( "Set KCenrepKeyCheckConf key" ) );
    SetCentralRepositoryKeyL( 100000003, 1 );
    EUNIT_PRINT( _L8( "AttachServiceL" ) );
    AttachServiceL( KHSPS, KHSPSConfigurationIf, KHSPSTestAppUid );
    EUNIT_PRINT( _L8( "Test step passed" ) );    

    // Test step 5
    EUNIT_PRINT( _L8( "Test step 5" ) );
    RunTestStepSyncL(
        ( TUint8* )getpluginconf_5_ts_5_method,
        ( TUint8* )getpluginconf_5_ts_5_input,
        ( TUint8* )getpluginconf_5_ts_5_output );
    EUNIT_PRINT( _L8( "Test step passed" ) );    

    }

//------------------------------------------------------------------------------
// Test case: GetPlugins(1)
//------------------------------------------------------------------------------
void MT_CHSPSConfigurationIf::GetPlugins_1_L()
    {
    
    // Pre conditions
    // Set active configuration to Minimal configuration
    EUNIT_PRINT( _L8( "Pre conditions: Set Active configuration Minimal" ) );
    SetActiveConfigurationL( KHSPSTestAppUid, KHSPSActiveConfMinimal );
    // Attach to HSPS 
    EUNIT_PRINT( _L8( "Pre conditions: Attach to HSPS service IConfiguration interface" ) );
    AttachServiceL( KHSPS, KHSPSConfigurationIf, KHSPSTestAppUid );
    
    // Test step 1
    EUNIT_PRINT( _L8( "Test step 1" ) );
    RunTestStepSyncL(
        ( TUint8* )getplugins_1_ts_1_method,
        ( TUint8* )getplugins_1_ts_1_input,
        ( TUint8* )getplugins_1_ts_1_output,
        ETrue );
    EUNIT_PRINT( _L8( "Test step passed" ) );    
    }

//------------------------------------------------------------------------------
// Test case: GetPlugins(2)
//------------------------------------------------------------------------------
void MT_CHSPSConfigurationIf::GetPlugins_2_L()
    {
    // Pre conditions
    // Set active configuration to Minimal configuration
    EUNIT_PRINT( _L8( "Pre conditions: Set Active configuration Minimal" ) );
    SetActiveConfigurationL( KHSPSTestAppUid, KHSPSActiveConfMinimal );
    // Attach to HSPS 
    EUNIT_PRINT( _L8( "Pre conditions: Attach to HSPS service IConfiguration interface" ) );
    AttachServiceL( KHSPS, KHSPSConfigurationIf, KHSPSTestAppUid );
    
    // Test step 1
    EUNIT_PRINT( _L8( "Test step 1" ) );
    RunTestStepSyncL(
        ( TUint8* )getplugins_2_ts_1_method,
        ( TUint8* )getplugins_2_ts_1_input,
        ( TUint8* )getplugins_2_ts_1_output );
    EUNIT_PRINT( _L8( "Test step passed" ) );    
    }

//------------------------------------------------------------------------------
// Test case: GetPlugins(3)
//------------------------------------------------------------------------------
void MT_CHSPSConfigurationIf::GetPlugins_3_L()
    {
    // Pre conditions
    // Set active configuration to Minimal configuration
    EUNIT_PRINT( _L8( "Pre conditions: Set Active configuration Minimal" ) );
    SetActiveConfigurationL( KHSPSTestAppUid, KHSPSActiveConfMinimal );
    // Attach to HSPS 
    EUNIT_PRINT( _L8( "Pre conditions: Attach to HSPS service IConfiguration interface" ) );
    AttachServiceL( KHSPS, KHSPSConfigurationIf, KHSPSTestAppUid );
    
    // Test step 1
    EUNIT_PRINT( _L8( "Test step 1" ) );
    RunTestStepSyncL(
        ( TUint8* )getplugins_3_ts_1_method,
        ( TUint8* )getplugins_3_ts_1_input,
        ( TUint8* )getplugins_3_ts_1_output );
    EUNIT_PRINT( _L8( "Test step passed" ) );    
    }

//------------------------------------------------------------------------------
// Test case: GetPlugins(4)
//------------------------------------------------------------------------------
void MT_CHSPSConfigurationIf::GetPlugins_4_L()
    {
    // Pre conditions
    // Set active configuration to Minimal configuration
    EUNIT_PRINT( _L8( "Pre conditions: Set Active configuration Minimal" ) );
    SetActiveConfigurationL( KHSPSTestAppUid, KHSPSActiveConfMinimal );
    // Attach to HSPS 
    EUNIT_PRINT( _L8( "Pre conditions: Attach to HSPS service IConfiguration interface" ) );
    AttachServiceL( KHSPS, KHSPSConfigurationIf, KHSPSTestAppUid );
    
    // Test step 1
    EUNIT_PRINT( _L8( "Test step 1" ) );
    RunTestStepSyncL(
        ( TUint8* )getplugins_4_ts_1_method,
        ( TUint8* )getplugins_4_ts_1_input,
        ( TUint8* )getplugins_4_ts_1_output );
    EUNIT_PRINT( _L8( "Test step passed" ) );    
    }

//------------------------------------------------------------------------------
// Test case: GetPlugins(5)
//------------------------------------------------------------------------------
void MT_CHSPSConfigurationIf::GetPlugins_5_L()
    {
    // Pre conditions
    // Set active configuration to Minimal configuration
    EUNIT_PRINT( _L8( "Pre conditions: Set Active configuration Minimal" ) );
    SetActiveConfigurationL( KHSPSTestAppUid, KHSPSActiveConfMinimal );
    // Attach to HSPS 
    EUNIT_PRINT( _L8( "Pre conditions: Attach to HSPS service IConfiguration interface" ) );
    AttachServiceL( KHSPS, KHSPSConfigurationIf, KHSPSTestAppUid );
    
    // Test step 1
    EUNIT_PRINT( _L8( "Test step 1" ) );
    RunTestStepSyncL(
        ( TUint8* )getplugins_5_ts_1_method,
        ( TUint8* )getplugins_5_ts_1_input,
        ( TUint8* )getplugins_5_ts_1_output );
    EUNIT_PRINT( _L8( "Test step passed" ) );    
    }

//------------------------------------------------------------------------------
// Test case: GetPlugins(6)
//------------------------------------------------------------------------------
void MT_CHSPSConfigurationIf::GetPlugins_6_L()
    {
    // Pre conditions
    // Set active configuration to Minimal configuration
    EUNIT_PRINT( _L8( "Pre conditions: Set Active configuration Minimal" ) );
    SetActiveConfigurationL( KHSPSTestAppUid, KHSPSActiveConfMinimal );
    // Attach to HSPS 
    EUNIT_PRINT( _L8( "Pre conditions: Attach to HSPS service IConfiguration interface" ) );
    AttachServiceL( KHSPS, KHSPSConfigurationIf, KHSPSTestAppUid );
    
    // Test step 1
    EUNIT_PRINT( _L8( "Test step 1" ) );
    RunTestStepSyncL(
        ( TUint8* )getplugins_6_ts_1_method,
        ( TUint8* )getplugins_6_ts_1_input,
        ( TUint8* )getplugins_6_ts_1_output );
    EUNIT_PRINT( _L8( "Test step passed" ) );    
    }

//------------------------------------------------------------------------------
// Test case: GetPlugins(7)
//------------------------------------------------------------------------------
void MT_CHSPSConfigurationIf::GetPlugins_7_L()
    {
    // Pre conditions
    // Set active configuration to Minimal configuration
    EUNIT_PRINT( _L8( "Pre conditions: Set Active configuration Minimal" ) );
    SetActiveConfigurationL( KHSPSTestAppUid, KHSPSActiveConfMinimal );
    // Attach to HSPS 
    EUNIT_PRINT( _L8( "Pre conditions: Attach to HSPS service IConfiguration interface" ) );
    AttachServiceL( KHSPS, KHSPSConfigurationIf, KHSPSTestAppUid );
    
    // Test step 1
    EUNIT_PRINT( _L8( "Test step 1" ) );
    RunTestStepSyncL(
        ( TUint8* )getplugins_7_ts_1_method,
        ( TUint8* )getplugins_7_ts_1_input,
        ( TUint8* )getplugins_7_ts_1_output );
    EUNIT_PRINT( _L8( "Test step passed" ) );    
    }

//------------------------------------------------------------------------------
// Test case: GetPluginList(1)
//------------------------------------------------------------------------------
void MT_CHSPSConfigurationIf::GetPluginList_1_L()
    {
    
    // Pre conditions
    // Set active configuration to Minimal configuration
    EUNIT_PRINT( _L8( "Pre conditions: Set Active configuration Typical" ) );
    SetActiveConfigurationL( KHSPSTestAppUid, KHSPSActiveConfTypical );
    // Attach to HSPS 
    EUNIT_PRINT( _L8( "Pre conditions: Attach to HSPS service IConfiguration interface" ) );
    AttachServiceL( KHSPS, KHSPSConfigurationIf, KHSPSTestAppUid );
    
    // Test step 1
    EUNIT_PRINT( _L8( "Test step 1" ) );
    RunTestStepSyncL(
        ( TUint8* )getpluginlist_1_ts_1_method,
        ( TUint8* )getpluginlist_1_ts_1_input,
        ( TUint8* )getpluginlist_1_ts_1_output,
        ETrue );
    EUNIT_PRINT( _L8( "Test step passed" ) );    
    }

//------------------------------------------------------------------------------
// Test case: GetPluginList(2)
//------------------------------------------------------------------------------
void MT_CHSPSConfigurationIf::GetPluginList_2_L()
    {
    
    // Pre conditions
    // Set active configuration to Minimal configuration
    EUNIT_PRINT( _L8( "Pre conditions: Set Active configuration Typical" ) );
    SetActiveConfigurationL( KHSPSTestAppUid, KHSPSActiveConfTypical );
    // Attach to HSPS 
    EUNIT_PRINT( _L8( "Pre conditions: Attach to HSPS service IConfiguration interface" ) );
    AttachServiceL( KHSPS, KHSPSConfigurationIf, KHSPSTestAppUid );
    
    // Test step 1
    EUNIT_PRINT( _L8( "Test step 1" ) );
    RunTestStepSyncL(
        ( TUint8* )getpluginlist_2_ts_1_method,
        ( TUint8* )getpluginlist_2_ts_1_input,
        ( TUint8* )getpluginlist_2_ts_1_output );
    EUNIT_PRINT( _L8( "Test step passed" ) );    
    }

//------------------------------------------------------------------------------
// Test case: GetPluginList(3)
//------------------------------------------------------------------------------
void MT_CHSPSConfigurationIf::GetPluginList_3_L()
    {
    
    // Pre conditions
    // Set active configuration to Minimal configuration
    EUNIT_PRINT( _L8( "Pre conditions: Set Active configuration Typical" ) );
    SetActiveConfigurationL( KHSPSTestAppUid, KHSPSActiveConfTypical );
    // Attach to HSPS 
    EUNIT_PRINT( _L8( "Pre conditions: Attach to HSPS service IConfiguration interface" ) );
    AttachServiceL( KHSPS, KHSPSConfigurationIf, KHSPSTestAppUid );
    
    // Test step 1
    EUNIT_PRINT( _L8( "Test step 1" ) );
    RunTestStepSyncL(
        ( TUint8* )getpluginlist_3_ts_1_method,
        ( TUint8* )getpluginlist_3_ts_1_input,
        ( TUint8* )getpluginlist_3_ts_1_output );
    EUNIT_PRINT( _L8( "Test step passed" ) );    
    }

//------------------------------------------------------------------------------
// Test case: GetPluginList(4)
//------------------------------------------------------------------------------
void MT_CHSPSConfigurationIf::GetPluginList_4_L()
    {
    
    // Pre conditions
    // Set active configuration to Minimal configuration
    EUNIT_PRINT( _L8( "Pre conditions: Set Active configuration Typical" ) );
    SetActiveConfigurationL( KHSPSTestAppUid, KHSPSActiveConfTypical );
    // Attach to HSPS 
    EUNIT_PRINT( _L8( "Pre conditions: Attach to HSPS service IConfiguration interface" ) );
    AttachServiceL( KHSPS, KHSPSConfigurationIf, KHSPSTestAppUid );
    
    // Test step 1
    EUNIT_PRINT( _L8( "Test step 1" ) );
    RunTestStepSyncL(
        ( TUint8* )getpluginlist_4_ts_1_method,
        ( TUint8* )getpluginlist_4_ts_1_input,
        ( TUint8* )getpluginlist_4_ts_1_output );
    EUNIT_PRINT( _L8( "Test step passed" ) );    
    }

//------------------------------------------------------------------------------
// Test case: GetPluginList(5)
//------------------------------------------------------------------------------
void MT_CHSPSConfigurationIf::GetPluginList_5_L()
    {
    
    // Pre conditions
    // Set active configuration to Minimal configuration
    EUNIT_PRINT( _L8( "Pre conditions: Set Active configuration Typical" ) );
    SetActiveConfigurationL( KHSPSTestAppUid, KHSPSActiveConfTypical );
    // Attach to HSPS 
    EUNIT_PRINT( _L8( "Pre conditions: Attach to HSPS service IConfiguration interface" ) );
    AttachServiceL( KHSPS, KHSPSConfigurationIf, KHSPSTestAppUid );
    
    // Test step 1
    EUNIT_PRINT( _L8( "Test step 1" ) );
    RunTestStepSyncL(
        ( TUint8* )getpluginlist_5_ts_1_method,
        ( TUint8* )getpluginlist_5_ts_1_input,
        ( TUint8* )getpluginlist_5_ts_1_output );
    EUNIT_PRINT( _L8( "Test step passed" ) );    
    }

//------------------------------------------------------------------------------
// Test case: GetPluginList(6)
//------------------------------------------------------------------------------
void MT_CHSPSConfigurationIf::GetPluginList_6_L()
    {
    
    // Pre conditions
    // Set active configuration to Minimal configuration
    EUNIT_PRINT( _L8( "Pre conditions: Set Active configuration Typical" ) );
    SetActiveConfigurationL( KHSPSTestAppUid, KHSPSActiveConfTypical );
    // Attach to HSPS 
    EUNIT_PRINT( _L8( "Pre conditions: Attach to HSPS service IConfiguration interface" ) );
    AttachServiceL( KHSPS, KHSPSConfigurationIf, KHSPSTestAppUid );
    
    // Test step 1
    EUNIT_PRINT( _L8( "Test step 1" ) );
    RunTestStepSyncL(
        ( TUint8* )getpluginlist_6_ts_1_method,
        ( TUint8* )getpluginlist_6_ts_1_input,
        ( TUint8* )getpluginlist_6_ts_1_output );
    EUNIT_PRINT( _L8( "Test step passed" ) );    
    }

//------------------------------------------------------------------------------
// Test case: GetPluginList(7)
//------------------------------------------------------------------------------
void MT_CHSPSConfigurationIf::GetPluginList_7_L()
    {
    
    // Pre conditions
    // Set active configuration to Minimal configuration
    EUNIT_PRINT( _L8( "Pre conditions: Set Active configuration Operator" ) );
    SetActiveConfigurationL( KHSPSTestAppUid, KHSPSActiveConfOperator );
    // Attach to HSPS 
    EUNIT_PRINT( _L8( "Pre conditions: Attach to HSPS service IConfiguration interface" ) );
    AttachServiceL( KHSPS, KHSPSConfigurationIf, KHSPSTestAppUid );
    
    // Test step 1
    EUNIT_PRINT( _L8( "Test step 1" ) );
    RunTestStepSyncL(
        ( TUint8* )getpluginlist_7_ts_1_method,
        ( TUint8* )getpluginlist_7_ts_1_input,
        ( TUint8* )getpluginlist_7_ts_1_output );
    EUNIT_PRINT( _L8( "Test step passed" ) );    
    }

//------------------------------------------------------------------------------
// Test case: GetPluginList(8)
//------------------------------------------------------------------------------
void MT_CHSPSConfigurationIf::GetPluginList_8_L()
    {
    
    // Pre conditions
    // Set active configuration to Minimal configuration
    EUNIT_PRINT( _L8( "Pre conditions: Set Active configuration Operator" ) );
    SetActiveConfigurationL( KHSPSTestAppUid, KHSPSActiveConfOperator );
    // Attach to HSPS 
    EUNIT_PRINT( _L8( "Pre conditions: Attach to HSPS service IConfiguration interface" ) );
    AttachServiceL( KHSPS, KHSPSConfigurationIf, KHSPSTestAppUid );
    
    // Test step 1
    EUNIT_PRINT( _L8( "Test step 1" ) );
    RunTestStepSyncL(
        ( TUint8* )getpluginlist_8_ts_1_method,
        ( TUint8* )getpluginlist_8_ts_1_input,
        ( TUint8* )getpluginlist_8_ts_1_output );
    EUNIT_PRINT( _L8( "Test step passed" ) );    
    }

//------------------------------------------------------------------------------
// Test case: AddPlugin(1)
//------------------------------------------------------------------------------
void MT_CHSPSConfigurationIf::AddPlugin_1_L()
    {
    // Pre conditions
    // Set active configuration to Typical configuration
    EUNIT_PRINT( _L8( "Pre conditions: Set Active configuration Typical" ) );
    SetActiveConfigurationL( KHSPSTestAppUid, KHSPSActiveConfTypical );    
    // Attach to HSPS 
    EUNIT_PRINT( _L8( "Pre conditions: Attach to HSPS service IConfiguration interface" ) );
    AttachServiceL( KHSPS, KHSPSConfigurationIf, KHSPSTestAppUid );
    
    // Test step 1
    EUNIT_PRINT( _L8( "Test step 1" ) );
    RunTestStepSyncL(
        ( TUint8* )addplugin_1_ts_1_method,
        ( TUint8* )addplugin_1_ts_1_input,
        ( TUint8* )addplugin_1_ts_1_output,
        ETrue );
    EUNIT_PRINT( _L8( "Test step passed" ) );    
    
    // Test step 2
    EUNIT_PRINT( _L8( "Test step 2" ) );
    RunTestStepSyncL(
        ( TUint8* )addplugin_1_ts_2_method,
        ( TUint8* )addplugin_1_ts_2_input,
        ( TUint8* )addplugin_1_ts_2_output );
    EUNIT_PRINT( _L8( "Test step passed" ) );    

    // Test step 3
    EUNIT_PRINT( _L8( "Test step 3" ) );
    RunTestStepSyncL(
        ( TUint8* )addplugin_1_ts_3_method,
        ( TUint8* )addplugin_1_ts_3_input,
        ( TUint8* )addplugin_1_ts_3_output );
    EUNIT_PRINT( _L8( "Test step passed" ) );    

    // Test step 4
    EUNIT_PRINT( _L8( "Test step 4" ) );
    RunTestStepSyncL(
        ( TUint8* )addplugin_1_ts_4_method,
        ( TUint8* )addplugin_1_ts_4_input,
        ( TUint8* )addplugin_1_ts_4_output,
        ETrue );
    EUNIT_PRINT( _L8( "Test step passed" ) );    

    // Test step 5
    EUNIT_PRINT( _L8( "Test step 5" ) );
    RunTestStepSyncL(
        ( TUint8* )addplugin_1_ts_5_method,
        ( TUint8* )addplugin_1_ts_5_input,
        ( TUint8* )addplugin_1_ts_5_output );
    EUNIT_PRINT( _L8( "Test step passed" ) );    
    }

//------------------------------------------------------------------------------
// Test case: AddPlugin(2)
//------------------------------------------------------------------------------
void MT_CHSPSConfigurationIf::AddPlugin_2_L()
    {
    // Pre conditions 
    //----------------------------------------------------------------
    ResetClientFolderL(); // affects performance testing  

    // Set active configuration to Typical configuration
    EUNIT_PRINT( _L8( "Pre conditions: Set Active configuration Typical" ) );
    SetActiveConfigurationL( KHSPSTestAppUid, KHSPSActiveConfTypical );    
    // Attach to HSPS 
    EUNIT_PRINT( _L8( "Pre conditions: Attach to HSPS service IConfiguration interface" ) );
    AttachServiceL( KHSPS, KHSPSConfigurationIf, KHSPSTestAppUid );

    // Test step 1
    //----------------------------------------------------------------
    EUNIT_PRINT( _L8( "Test step 1" ) );
    RunTestStepSyncL(
        ( TUint8* )addplugin_2_ts_1_method,
        ( TUint8* )addplugin_2_ts_1_input,
        ( TUint8* )addplugin_2_ts_1_output );
    EUNIT_PRINT( _L8( "Test step passed" ) );    
    
    // post condition check: verify that resources of the application configuration have been copied
    EUNIT_PRINT( _L8( "post condition check for resource file copy" ) );
    ResetResources();
    // Typical - Root               
    AddResourceL( _L("c:\\private\\20000fb1\\536874929\\270513751\\536916240\\1.0\\sources\\root.bmp"), 165450 );
    // Typical - View1
    AddResourceL( _L("c:\\private\\20000fb1\\2456\\270513751\\536916241\\1.0\\sources\\view1.bmp"), 9354 );      
    AddResourceL( _L("c:\\private\\20000fb1\\2456\\270513751\\536916241\\1.0\\sources\\hs_logoz.jpg"), 19519 );
    // Typical - View2
    AddResourceL( _L("c:\\private\\20000fb1\\2456\\270513751\\536916242\\1.0\\sources\\view2.bmp"), 183654 );          
    // Typical - Widget
    AddResourceL( _L("c:\\private\\20000fb1\\2456\\270513751\\536916256\\1.0\\sources\\hs_logo.jpg"), 18179 );    
    AddResourceL( _L("c:\\private\\20000fb1\\2456\\270513751\\536916256\\1.0\\sources\\widget.bmp"), 6446 );       
    CheckResourcesL( );        
    // Minimal - Widget       
    EUNIT_PRINT( _L8( "post condition check for resource copy passed" ) );
    
    // Test step 2
    //----------------------------------------------------------------
    EUNIT_PRINT( _L8( "Test step 2" ) );
    RunTestStepSyncL(
        ( TUint8* )addplugin_2_ts_2_method,
        ( TUint8* )addplugin_2_ts_2_input,
        ( TUint8* )addplugin_2_ts_2_output );
    EUNIT_PRINT( _L8( "Test step passed" ) );    

    // Test step 3
    //----------------------------------------------------------------
    EUNIT_PRINT( _L8( "Test step 3" ) );
    RunTestStepSyncL(
        ( TUint8* )addplugin_2_ts_3_method,
        ( TUint8* )addplugin_2_ts_3_input,
        ( TUint8* )addplugin_2_ts_3_output );
    EUNIT_PRINT( _L8( "Test step passed" ) );    

    // Test step 4
    //----------------------------------------------------------------
    EUNIT_PRINT( _L8( "Test step 4" ) );
    RunTestStepSyncL(
        ( TUint8* )addplugin_2_ts_4_method,
        ( TUint8* )addplugin_2_ts_4_input,
        ( TUint8* )addplugin_2_ts_4_output );
    EUNIT_PRINT( _L8( "Test step passed" ) );    

    // Test step 5
    //----------------------------------------------------------------
    EUNIT_PRINT( _L8( "Test step 5" ) );
    RunTestStepSyncL(
        ( TUint8* )addplugin_2_ts_5_method,
        ( TUint8* )addplugin_2_ts_5_input,
        ( TUint8* )addplugin_2_ts_5_output );
    EUNIT_PRINT( _L8( "Test step passed" ) );    
    }

//------------------------------------------------------------------------------
// Test case: AddPlugin(3)
//------------------------------------------------------------------------------
void MT_CHSPSConfigurationIf::AddPlugin_3_L()
    {
    // Pre conditions
    // Set active configuration to Minimal configuration
    EUNIT_PRINT( _L8( "Pre conditions: Set Active configuration Minimal" ) );
    SetActiveConfigurationL( KHSPSTestAppUid, KHSPSActiveConfMinimal );
    // Attach to HSPS 
    EUNIT_PRINT( _L8( "Pre conditions: Attach to HSPS service IConfiguration interface" ) );
    AttachServiceL( KHSPS, KHSPSConfigurationIf, KHSPSTestAppUid );
    
    // Test step 1
    EUNIT_PRINT( _L8( "Test step 1" ) );
    RunTestStepSyncL(
        ( TUint8* )addplugin_3_ts_1_method,
        ( TUint8* )addplugin_3_ts_1_input,
        ( TUint8* )addplugin_3_ts_1_output );
    EUNIT_PRINT( _L8( "Test step passed" ) );    
    
    // Test step 2
    EUNIT_PRINT( _L8( "Test step 2" ) );
    RunTestStepSyncL(
        ( TUint8* )addplugin_3_ts_2_method,
        ( TUint8* )addplugin_3_ts_2_input,
        ( TUint8* )addplugin_3_ts_2_output );
    EUNIT_PRINT( _L8( "Test step passed" ) );    
    }

//------------------------------------------------------------------------------
// Test case: AddPlugin(4)
//------------------------------------------------------------------------------
void MT_CHSPSConfigurationIf::AddPlugin_4_L()
    {
    // Pre conditions
    // Set active configuration to Minimal configuration
    EUNIT_PRINT( _L8( "Pre conditions: Set Active configuration Minimal" ) );
    SetActiveConfigurationL( KHSPSTestAppUid, KHSPSActiveConfMinimal );
    // Attach to HSPS 
    EUNIT_PRINT( _L8( "Pre conditions: Attach to HSPS service IConfiguration interface" ) );
    AttachServiceL( KHSPS, KHSPSConfigurationIf, KHSPSTestAppUid );
    
    // Test step 1
    EUNIT_PRINT( _L8( "Test step 1" ) );
    RunTestStepSyncL(
        ( TUint8* )addplugin_4_ts_1_method,
        ( TUint8* )addplugin_4_ts_1_input,
        ( TUint8* )addplugin_4_ts_1_output );
    EUNIT_PRINT( _L8( "Test step passed" ) );    
    }

//------------------------------------------------------------------------------
// Test case: AddPlugin(5)
//------------------------------------------------------------------------------
void MT_CHSPSConfigurationIf::AddPlugin_5_L()
    {
    // Pre conditions
    // Set active configuration to Minimal configuration
    EUNIT_PRINT( _L8( "Pre conditions: Set Active configuration Minimal" ) );
    SetActiveConfigurationL( KHSPSTestAppUid, KHSPSActiveConfMinimal );
    // Attach to HSPS 
    EUNIT_PRINT( _L8( "Pre conditions: Attach to HSPS service IConfiguration interface" ) );
    AttachServiceL( KHSPS, KHSPSConfigurationIf, KHSPSTestAppUid );
    
    // Test step 1
    EUNIT_PRINT( _L8( "Test step 1" ) );
    RunTestStepSyncL(
        ( TUint8* )addplugin_5_ts_1_method,
        ( TUint8* )addplugin_5_ts_1_input,
        ( TUint8* )addplugin_5_ts_1_output );
    EUNIT_PRINT( _L8( "Test step passed" ) );    
    }

//------------------------------------------------------------------------------
// Test case: AddPlugin(6)
//------------------------------------------------------------------------------
void MT_CHSPSConfigurationIf::AddPlugin_6_L()
    {
    // Pre conditions
    // Set active configuration to Operator configuration
    EUNIT_PRINT( _L8( "Pre conditions: Set Active configuration Operator" ) );
    SetActiveConfigurationL( KHSPSTestAppUid, KHSPSActiveConfOperator );    
    // Attach to HSPS 
    EUNIT_PRINT( _L8( "Pre conditions: Attach to HSPS service IConfiguration interface" ) );
    AttachServiceL( KHSPS, KHSPSConfigurationIf, KHSPSTestAppUid );

    // Test step 1
    EUNIT_PRINT( _L8( "Test step 1" ) );
    RunTestStepSyncL(
        ( TUint8* )addplugin_6_ts_1_method,
        ( TUint8* )addplugin_6_ts_1_input,
        ( TUint8* )addplugin_6_ts_1_output );
    EUNIT_PRINT( _L8( "Test step passed" ) );    
    
    // Test step 2
    EUNIT_PRINT( _L8( "Test step 2" ) );
    RunTestStepSyncL(
        ( TUint8* )addplugin_6_ts_2_method,
        ( TUint8* )addplugin_6_ts_2_input,
        ( TUint8* )addplugin_6_ts_2_output );
    ResetResources();  
    AddResourceL( _L("c:\\private\\20000fb1\\2456\\270513751\\536916273\\1.0\\sources\\dummy.mif"), 4608 );        
    CheckResourcesL( );    
    EUNIT_PRINT( _L8( "Test step passed" ) );    

    // Test step 3
    EUNIT_PRINT( _L8( "Test step 3" ) );
    RunTestStepSyncL(
        ( TUint8* )addplugin_6_ts_3_method,
        ( TUint8* )addplugin_6_ts_3_input,
        ( TUint8* )addplugin_6_ts_3_output );
    EUNIT_PRINT( _L8( "Test step passed" ) );    

    // Test step 4
    EUNIT_PRINT( _L8( "Test step 4" ) );
    RunTestStepSyncL(
        ( TUint8* )addplugin_6_ts_4_method,
        ( TUint8* )addplugin_6_ts_4_input,
        ( TUint8* )addplugin_6_ts_4_output );
    EUNIT_PRINT( _L8( "Test step passed" ) );    

    // Test step 5
    EUNIT_PRINT( _L8( "Test step 5" ) );
    RunTestStepSyncL(
        ( TUint8* )addplugin_6_ts_5_method,
        ( TUint8* )addplugin_6_ts_5_input,
        ( TUint8* )addplugin_6_ts_5_output );
    EUNIT_PRINT( _L8( "Test step passed" ) );    
    }

//------------------------------------------------------------------------------
// Test case: AddPlugin(7)
//------------------------------------------------------------------------------
void MT_CHSPSConfigurationIf::AddPlugin_7_L()
    {
    // Pre conditions
    // Set active configuration to Operator configuration
    EUNIT_PRINT( _L8( "Pre conditions: Set Active configuration Typical" ) );
    SetActiveConfigurationL( KHSPSTestAppUid, KHSPSActiveConfTypical );    
    // Attach to HSPS 
    EUNIT_PRINT( _L8( "Pre conditions: Attach to HSPS service IConfiguration interface" ) );
    AttachServiceL( KHSPS, KHSPSConfigurationIf, KHSPSTestAppUid );

    // Test step 1
    EUNIT_PRINT( _L8( "Test step 1" ) );
    RunTestStepSyncL(
        ( TUint8* )addplugin_7_ts_1_method,
        ( TUint8* )addplugin_7_ts_1_input,
        ( TUint8* )addplugin_7_ts_1_output );
    EUNIT_PRINT( _L8( "Test step passed" ) );    
    
    // Test step 2
    EUNIT_PRINT( _L8( "Test step 2" ) );
    RunTestStepSyncL(
        ( TUint8* )addplugin_7_ts_2_method,
        ( TUint8* )addplugin_7_ts_2_input,
        ( TUint8* )addplugin_7_ts_2_output );
    EUNIT_PRINT( _L8( "Test step passed" ) );    

    // Test step 3
    EUNIT_PRINT( _L8( "Test step 3" ) );
    RunTestStepSyncL(
        ( TUint8* )addplugin_7_ts_3_method,
        ( TUint8* )addplugin_7_ts_3_input,
        ( TUint8* )addplugin_7_ts_3_output );
    EUNIT_PRINT( _L8( "Test step passed" ) );    

    // Test step 4
    EUNIT_PRINT( _L8( "Test step 4" ) );
    RunTestStepSyncL(
        ( TUint8* )addplugin_7_ts_4_method,
        ( TUint8* )addplugin_7_ts_4_input,
        ( TUint8* )addplugin_7_ts_4_output );
    EUNIT_PRINT( _L8( "Test step passed" ) );    
    }

//------------------------------------------------------------------------------
// Test case: AddPlugin(8)
//------------------------------------------------------------------------------
void MT_CHSPSConfigurationIf::AddPlugin_8_L()
    {
    // Pre conditions
    //----------------------------------------------------------------
    ResetClientFolderL(); // affects performance testing 
    
    // Active device language is ELangEnglish(=1) 
    // in \epoc32\RELEASE\WINSCW\udeb\Z\resource\BootData\Languages.txt
    
    // Set active configuration to Minimal configuration    
    EUNIT_PRINT( _L8( "Pre conditions: Set Active configuration Minimal" ) );
    SetActiveConfigurationL( KHSPSTestAppUid, KHSPSActiveConfMinimal );    
    
    // Attach to HSPS 
    EUNIT_PRINT( _L8( "Pre conditions: Attach to HSPS service IConfiguration interface" ) );
    AttachServiceL( KHSPS, KHSPSConfigurationIf, KHSPSTestAppUid );

    // Test step 1: add widget with english locale to unlocalized configuration
    //----------------------------------------------------------------
    EUNIT_PRINT( _L8( "Test step 1" ) );
    RunTestStepSyncL(
        ( TUint8* )addplugin_8_ts_1_method,
        ( TUint8* )addplugin_8_ts_1_input,
        ( TUint8* )addplugin_8_ts_1_output );
    EUNIT_PRINT( _L8( "Test step passed" ) );
    
    // Test step 2: get added plugin configuration
    //----------------------------------------------------------------
    EUNIT_PRINT( _L8( "Test step 2" ) );
    RunTestStepSyncL(
        ( TUint8* )addplugin_8_ts_2_method,
        ( TUint8* )addplugin_8_ts_2_input,
        ( TUint8* )addplugin_8_ts_2_output );
    EUNIT_PRINT( _L8( "Test step passed" ) );
    
    // post condition check: verify that resources of the application configuration have been copied
    //----------------------------------------------------------------
    EUNIT_PRINT( _L8( "post condition check for resource file copy" ) );    
    // Minimal - Root           
    // Minimal - View
    ResetResources();
    AddResourceL( _L("c:\\private\\20000fb1\\2456\\270513751\\536916225\\1.0\\sources\\main.xml"), 136 );    
    AddResourceL( _L("c:\\private\\20000fb1\\2456\\270513751\\536916225\\1.0\\sources\\picture.jpeg"), 18 );
    AddResourceL( _L("c:\\private\\20000fb1\\2456\\270513751\\536916225\\1.0\\sources\\resource.file"), 17 );
    AddResourceL( _L("c:\\private\\20000fb1\\2456\\270513751\\536916225\\1.0\\sources\\viewnavigationrules.xml"), 17 );
    // Minimal - Widget
    // Typical - View1 
    AddResourceL( _L("c:\\private\\20000fb1\\2456\\270513751\\536916241\\1.0\\sources\\view1.bmp"), 9354 );      
    AddResourceL( _L("c:\\private\\20000fb1\\2456\\270513751\\536916241\\1.0\\sources\\hs_logoz.jpg"), 19519 );
    // Typical - Widget
    AddResourceL( _L("c:\\private\\20000fb1\\2456\\270513751\\536916256\\1.0\\sources\\hs_logo.jpg"), 18179 );
    AddResourceL( _L("c:\\private\\20000fb1\\2456\\270513751\\536916256\\1.0\\sources\\widget.bmp"), 6446 );
    AddResourceL( _L("c:\\private\\20000fb1\\2456\\270513751\\536916256\\1.0\\sources\\widget.bmp"), 6446 );
    CheckResourcesL();            
    EUNIT_PRINT( _L8( "post condition check for resource copy passed" ) ); 
    
    // Test step 3: restore configuration by removing the plugin
    //----------------------------------------------------------------
    EUNIT_PRINT( _L8( "Test step 3" ) );
    RunTestStepSyncL(
        ( TUint8* )addplugin_8_ts_3_method,
        ( TUint8* )addplugin_8_ts_3_input,
        ( TUint8* )addplugin_8_ts_3_output );
    EUNIT_PRINT( _L8( "Test step passed" ) );           
    }

//------------------------------------------------------------------------------
// Test case: AddPlugin(9)
//------------------------------------------------------------------------------
void MT_CHSPSConfigurationIf::AddPlugin_9_L()
    {
    // Pre conditions
    //----------------------------------------------------------------
    ResetClientFolderL(); // affects performance testing         
    EUNIT_PRINT( _L8( "Pre conditions: Set Active configuration Typical" ) );
    SetActiveConfigurationL( KHSPSTestAppUid, KHSPSActiveConfTypical );    
    
    // Attach to HSPS 
    EUNIT_PRINT( _L8( "Pre conditions: Attach to HSPS service IConfiguration interface" ) );
    AttachServiceL( KHSPS, KHSPSConfigurationIf, KHSPSTestAppUid );
    
    // Test step 1: add widget with a finnish locale only 
    //----------------------------------------------------------------
    EUNIT_PRINT( _L8( "Test step 1" ) );
    RunTestStepSyncL(
        ( TUint8* )addplugin_9_ts_1_method,
        ( TUint8* )addplugin_9_ts_1_input,
        ( TUint8* )addplugin_9_ts_1_output );
    EUNIT_PRINT( _L8( "Test step passed" ) );
    
    // Test step 2: get added plugin configuration
    //----------------------------------------------------------------
    EUNIT_PRINT( _L8( "Test step 2" ) );
    RunTestStepSyncL(
        ( TUint8* )addplugin_9_ts_2_method,
        ( TUint8* )addplugin_9_ts_2_input,
        ( TUint8* )addplugin_9_ts_2_output );
    EUNIT_PRINT( _L8( "Test step passed" ) );
    
    // post condition check: verify that resources of the application configuration have been copied
    //----------------------------------------------------------------
    EUNIT_PRINT( _L8( "post condition check for resource file copy" ) );
    ResetResources();
    // Typical - Root           
    // Typical - View1    
    AddResourceL( _L("c:\\private\\20000fb1\\2456\\270513751\\536916241\\1.0\\sources\\view1.bmp"), 9354 );      
    AddResourceL( _L("c:\\private\\20000fb1\\2456\\270513751\\536916241\\1.0\\sources\\hs_logoz.jpg"), 19519 );    
    // Typical - View2     
    AddResourceL( _L("c:\\private\\20000fb1\\2456\\270513751\\536916242\\1.0\\sources\\view2.bmp"), 183654 );
    // Typical - Widget
    AddResourceL( _L("c:\\private\\20000fb1\\2456\\270513751\\536916256\\1.0\\sources\\hs_logo.jpg"), 18179 );    
    AddResourceL( _L("c:\\private\\20000fb1\\2456\\270513751\\536916256\\1.0\\sources\\widget.bmp"), 6446 );
    // Typical - Widget
    AddResourceL( _L("c:\\private\\20000fb1\\2456\\270513751\\536916256\\1.0\\sources\\hs_logo.jpg"), 18179 );    
    AddResourceL( _L("c:\\private\\20000fb1\\2456\\270513751\\536916256\\1.0\\sources\\widget.bmp"), 6446 );               
    // Typical - FinnishWidget
    AddResourceL( _L("c:\\private\\20000fb1\\2456\\270513751\\536916274\\1.0\\sources\\common.jpg"), 19465 );
    AddResourceL( _L("c:\\private\\20000fb1\\2456\\270513751\\536916274\\1.0\\sources\\localizedbg.jpg"), 1214 );    
    CheckResourcesL();            
    EUNIT_PRINT( _L8( "post condition check for resource copy passed" ) ); 
    
    // Test step 3: restore configuration by removing the plugin
    //----------------------------------------------------------------
    EUNIT_PRINT( _L8( "Test step 3" ) );
    RunTestStepSyncL(
        ( TUint8* )addplugin_9_ts_3_method,
        ( TUint8* )addplugin_9_ts_3_input,
        ( TUint8* )addplugin_9_ts_3_output );
    EUNIT_PRINT( _L8( "Test step passed" ) );
    }

//------------------------------------------------------------------------------
// Test case: AddPlugin(10)
//------------------------------------------------------------------------------
void MT_CHSPSConfigurationIf::AddPlugin_10_L()
    {
    // Pre conditions    
    
    // Attach to HSPS 
    EUNIT_PRINT( _L8( "Pre conditions: Attach to HSPS service IConfiguration interface" ) );
    AttachServiceL( KHSPS, KHSPSConfigurationIf, KHSPSTestAppUid );
          
    EUNIT_PRINT( _L8( "Test step 1" ) );
    RunTestStepSyncL(
        ( TUint8* )addplugin_10_ts_1_method,
        ( TUint8* )addplugin_10_ts_1_input,
        ( TUint8* )addplugin_10_ts_1_output );
    EUNIT_PRINT( _L8( "Test step passed" ) );
        
    EUNIT_PRINT( _L8( "Test step 2" ) );
    RunTestStepSyncL(
        ( TUint8* )addplugin_10_ts_2_method,
        ( TUint8* )addplugin_10_ts_2_input,
        ( TUint8* )addplugin_10_ts_2_output );
    EUNIT_PRINT( _L8( "Test step passed" ) );
    
    EUNIT_PRINT( _L8( "Test step 3" ) );
    RunTestStepSyncL(
        ( TUint8* )addplugin_10_ts_3_method,
        ( TUint8* )addplugin_10_ts_3_input,
        ( TUint8* )addplugin_10_ts_3_output );
    EUNIT_PRINT( _L8( "Test step passed" ) );       
    
    EUNIT_PRINT( _L8( "Test step 4" ) );
    RunTestStepSyncL(
        ( TUint8* )addplugin_10_ts_4_method,
        ( TUint8* )addplugin_10_ts_4_input,
        ( TUint8* )addplugin_10_ts_4_output );
    EUNIT_PRINT( _L8( "Test step passed" ) );
    
    EUNIT_PRINT( _L8( "Test step 5" ) );
    RunTestStepSyncL(
        ( TUint8* )addplugin_10_ts_5_method,
        ( TUint8* )addplugin_10_ts_5_input,
        ( TUint8* )addplugin_10_ts_5_output );
    EUNIT_PRINT( _L8( "Test step passed" ) );
    
    EUNIT_PRINT( _L8( "Test step 6" ) );
    RunTestStepSyncL(
        ( TUint8* )addplugin_10_ts_6_method,
        ( TUint8* )addplugin_10_ts_6_input,
        ( TUint8* )addplugin_10_ts_6_output );
    EUNIT_PRINT( _L8( "Test step passed" ) );
    
    EUNIT_PRINT( _L8( "Test step 7" ) );
    RunTestStepSyncL(
        ( TUint8* )addplugin_10_ts_7_method,
        ( TUint8* )addplugin_10_ts_7_input,
        ( TUint8* )addplugin_10_ts_7_output );
    EUNIT_PRINT( _L8( "Test step passed" ) );

    EUNIT_PRINT( _L8( "Test step 8" ) );
    RunTestStepSyncL(
        ( TUint8* )addplugin_10_ts_8_method,
        ( TUint8* )addplugin_10_ts_8_input,
        ( TUint8* )addplugin_10_ts_8_output );
    EUNIT_PRINT( _L8( "Test step passed" ) );
    }

//------------------------------------------------------------------------------
// Test case: AddPlugin(11)
//------------------------------------------------------------------------------
void MT_CHSPSConfigurationIf::AddPlugin_11_L()
    {
    // Pre conditions
    // Set active configuration to Typical configuration
    EUNIT_PRINT( _L8( "Pre conditions: Set Active configuration Typical" ) );
    SetActiveConfigurationL( KHSPSTestAppUid, KHSPSActiveConfTypical );
    
    // Uninstall Widget Conf #1
    MT_CHspsInstallationService* installationService = MT_CHspsInstallationService::NewL();
    CleanupStack::PushL( installationService );
    installationService->UninstallConfigurationL( KHSPSMTInterfaceUid, KHSPSMinimalWidgetConf );
    CleanupStack::PopAndDestroy( installationService );
    
    // Attach to HSPS 
    EUNIT_PRINT( _L8( "Pre conditions: Attach to HSPS service IConfiguration interface" ) );
    AttachServiceL( KHSPS, KHSPSConfigurationIf, KHSPSTestAppUid );
    
    // Test step 1
    EUNIT_PRINT( _L8( "Test step 1" ) );
    RunTestStepSyncL(
        ( TUint8* )addplugin_11_ts_1_method,
        ( TUint8* )addplugin_11_ts_1_input,
        ( TUint8* )addplugin_11_ts_1_output,
        ETrue );
    EUNIT_PRINT( _L8( "Test step passed" ) );    
    
    // Test step 2
    EUNIT_PRINT( _L8( "Test step 2" ) );
    RunTestStepSyncL(
        ( TUint8* )addplugin_11_ts_2_method,
        ( TUint8* )addplugin_11_ts_2_input,
        ( TUint8* )addplugin_11_ts_2_output );
    EUNIT_PRINT( _L8( "Test step passed" ) );    
    }

//------------------------------------------------------------------------------
// Test case: RemovePlugin(1)
//------------------------------------------------------------------------------
void MT_CHSPSConfigurationIf::RemovePlugin_1_L()
    {
    // Pre conditions
    // Set active configuration to Typical configuration
    EUNIT_PRINT( _L8( "Pre conditions: Set Active configuration Typical" ) );
    SetActiveConfigurationL( KHSPSTestAppUid, KHSPSActiveConfTypical );    
    // Attach to HSPS 
    EUNIT_PRINT( _L8( "Pre conditions: Attach to HSPS service IConfiguration interface" ) );
    AttachServiceL( KHSPS, KHSPSConfigurationIf, KHSPSTestAppUid );
    
    // Test step 1
    EUNIT_PRINT( _L8( "Test step 1" ) );
    RunTestStepSyncL(
        ( TUint8* )removeplugin_1_ts_1_method,
        ( TUint8* )removeplugin_1_ts_1_input,
        ( TUint8* )removeplugin_1_ts_1_output );
    EUNIT_PRINT( _L8( "Test step passed" ) );    
    
    // Test step 2
    EUNIT_PRINT( _L8( "Test step 2" ) );
    RunTestStepSyncL(
        ( TUint8* )removeplugin_1_ts_2_method,
        ( TUint8* )removeplugin_1_ts_2_input,
        ( TUint8* )removeplugin_1_ts_2_output );
    EUNIT_PRINT( _L8( "Test step passed" ) );
    }

//------------------------------------------------------------------------------
// Test case: RemovePlugin(2)
//------------------------------------------------------------------------------
void MT_CHSPSConfigurationIf::RemovePlugin_2_L()
    {
    // Pre conditions
            
    // Attach to HSPS 
    EUNIT_PRINT( _L8( "Pre conditions: Attach to HSPS service IConfiguration interface" ) );
    AttachServiceL( KHSPS, KHSPSConfigurationIf, KHSPSTestAppUid );
    
    // Test step 1
    EUNIT_PRINT( _L8( "Test step 1" ) );
    RunTestStepSyncL(
        ( TUint8* )removeplugin_2_ts_1_method,
        ( TUint8* )removeplugin_2_ts_1_input,
        ( TUint8* )removeplugin_2_ts_1_output );
    EUNIT_PRINT( _L8( "Test step passed" ) );    
    
    // Test step 2
    EUNIT_PRINT( _L8( "Test step 2" ) );
    RunTestStepSyncL(
        ( TUint8* )removeplugin_2_ts_2_method,
        ( TUint8* )removeplugin_2_ts_2_input,
        ( TUint8* )removeplugin_2_ts_2_output );
    EUNIT_PRINT( _L8( "Test step passed" ) );
    
    // Test step 3
    EUNIT_PRINT( _L8( "Test step 3" ) );
    RunTestStepSyncL(
        ( TUint8* )removeplugin_2_ts_3_method,
        ( TUint8* )removeplugin_2_ts_3_input,
        ( TUint8* )removeplugin_2_ts_3_output );
    EUNIT_PRINT( _L8( "Test step passed" ) );
    
    // Test step 4
    EUNIT_PRINT( _L8( "Test step 4" ) );
    RunTestStepSyncL(
        ( TUint8* )removeplugin_2_ts_4_method,
        ( TUint8* )removeplugin_2_ts_4_input,
        ( TUint8* )removeplugin_2_ts_4_output );
    EUNIT_PRINT( _L8( "Test step passed" ) );
    
    // Test step 5
    EUNIT_PRINT( _L8( "Test step 5" ) );
    RunTestStepSyncL(
        ( TUint8* )removeplugin_2_ts_5_method,
        ( TUint8* )removeplugin_2_ts_5_input,
        ( TUint8* )removeplugin_2_ts_5_output );
    EUNIT_PRINT( _L8( "Test step passed" ) );
    }

//------------------------------------------------------------------------------
// Test case: RemovePlugin(3)
//------------------------------------------------------------------------------
void MT_CHSPSConfigurationIf::RemovePlugin_3_L()
    {
    // Pre conditions
    // Set active configuration to Typical configuration
    EUNIT_PRINT( _L8( "Pre conditions: Set Active configuration Typical" ) );
    SetActiveConfigurationL( KHSPSTestAppUid, KHSPSActiveConfTypical );    
    // Attach to HSPS 
    EUNIT_PRINT( _L8( "Pre conditions: Attach to HSPS service IConfiguration interface" ) );
    AttachServiceL( KHSPS, KHSPSConfigurationIf, KHSPSTestAppUid );
    
    // Test step 1
    EUNIT_PRINT( _L8( "Test step 1" ) );
    RunTestStepSyncL(
        ( TUint8* )removeplugin_3_ts_1_method,
        ( TUint8* )removeplugin_3_ts_1_input,
        ( TUint8* )removeplugin_3_ts_1_output );
    EUNIT_PRINT( _L8( "Test step passed" ) );    
    
    // Test step 2
    EUNIT_PRINT( _L8( "Test step 2" ) );
    RunTestStepSyncL(
        ( TUint8* )removeplugin_3_ts_2_method,
        ( TUint8* )removeplugin_3_ts_2_input,
        ( TUint8* )removeplugin_3_ts_2_output );
    EUNIT_PRINT( _L8( "Test step passed" ) );
    }

//------------------------------------------------------------------------------
// Test case: RemovePlugin(4)
//------------------------------------------------------------------------------
void MT_CHSPSConfigurationIf::RemovePlugin_4_L()
    {
    // Pre conditions
    // Set active configuration to Typical configuration
    EUNIT_PRINT( _L8( "Pre conditions: Set Active configuration Typical" ) );
    SetActiveConfigurationL( KHSPSTestAppUid, KHSPSActiveConfTypical );    
    // Attach to HSPS 
    EUNIT_PRINT( _L8( "Pre conditions: Attach to HSPS service IConfiguration interface" ) );
    AttachServiceL( KHSPS, KHSPSConfigurationIf, KHSPSTestAppUid );
    
    // Test step 1
    EUNIT_PRINT( _L8( "Test step 1" ) );
    RunTestStepSyncL(
        ( TUint8* )removeplugin_4_ts_1_method,
        ( TUint8* )removeplugin_4_ts_1_input,
        ( TUint8* )removeplugin_4_ts_1_output );
    EUNIT_PRINT( _L8( "Test step passed" ) );    
    
    // Test step 2
    EUNIT_PRINT( _L8( "Test step 2" ) );
    RunTestStepSyncL(
        ( TUint8* )removeplugin_4_ts_2_method,
        ( TUint8* )removeplugin_4_ts_2_input,
        ( TUint8* )removeplugin_4_ts_2_output );
    EUNIT_PRINT( _L8( "Test step passed" ) );
    }

//------------------------------------------------------------------------------
// Test case: SetPluginSettings(1)
//------------------------------------------------------------------------------
void MT_CHSPSConfigurationIf::SetPluginSettings_1_L()
    {
    // Pre conditions
    // Set active configuration to Typical configuration
    EUNIT_PRINT( _L8( "Pre conditions: Set Active configuration Minimal" ) );
    SetActiveConfigurationL( KHSPSTestAppUid, KHSPSActiveConfMinimal );    
    // Attach to HSPS 
    EUNIT_PRINT( _L8( "Pre conditions: Attach to HSPS service IConfiguration interface" ) );
    AttachServiceL( KHSPS, KHSPSConfigurationIf, KHSPSTestAppUid );
    
    // Test step 1
    EUNIT_PRINT( _L8( "Test step 1" ) );
    RunTestStepSyncL(
        ( TUint8* )setpluginsettings_1_ts_1_method,
        ( TUint8* )setpluginsettings_1_ts_1_input,
        ( TUint8* )setpluginsettings_1_ts_1_output,
        ETrue );
    EUNIT_PRINT( _L8( "Test step passed" ) );  
    
    // Test step 2
    EUNIT_PRINT( _L8( "Test step 2" ) );
    RunTestStepSyncL(
        ( TUint8* )setpluginsettings_1_ts_2_method,
        ( TUint8* )setpluginsettings_1_ts_2_input,
        ( TUint8* )setpluginsettings_1_ts_2_output );
    EUNIT_PRINT( _L8( "Test step passed" ) );  
    
    // Test step 3
    EUNIT_PRINT( _L8( "Test step 3" ) );
    RunTestStepSyncL(
        ( TUint8* )setpluginsettings_1_ts_3_method,
        ( TUint8* )setpluginsettings_1_ts_3_input,
        ( TUint8* )setpluginsettings_1_ts_3_output );
    EUNIT_PRINT( _L8( "Test step passed" ) );  
    }

//------------------------------------------------------------------------------
// Test case: SetPluginSettings(2)
//------------------------------------------------------------------------------
void MT_CHSPSConfigurationIf::SetPluginSettings_2_L()
    {
    // Pre conditions
    // Set active configuration to Typical configuration
    EUNIT_PRINT( _L8( "Pre conditions: Set Active configuration Typical" ) );
    SetActiveConfigurationL( KHSPSTestAppUid, KHSPSActiveConfTypical );    
    // Attach to HSPS 
    EUNIT_PRINT( _L8( "Pre conditions: Attach to HSPS service IConfiguration interface" ) );
    AttachServiceL( KHSPS, KHSPSConfigurationIf, KHSPSTestAppUid );
    
    // Test step 1
    EUNIT_PRINT( _L8( "Test step 1" ) );
    RunTestStepSyncL(
        ( TUint8* )setpluginsettings_2_ts_1_method,
        ( TUint8* )setpluginsettings_2_ts_1_input,
        ( TUint8* )setpluginsettings_2_ts_1_output );
    EUNIT_PRINT( _L8( "Test step passed" ) );  
    
    // Test step 2
    EUNIT_PRINT( _L8( "Test step 2" ) );
    RunTestStepSyncL(
        ( TUint8* )setpluginsettings_2_ts_2_method,
        ( TUint8* )setpluginsettings_2_ts_2_input,
        ( TUint8* )setpluginsettings_2_ts_2_output );
    EUNIT_PRINT( _L8( "Test step passed" ) );  
    
    // Test step 2
    EUNIT_PRINT( _L8( "Test step 3" ) );
    RunTestStepSyncL(
        ( TUint8* )setpluginsettings_2_ts_3_method,
        ( TUint8* )setpluginsettings_2_ts_3_input,
        ( TUint8* )setpluginsettings_2_ts_3_output );
    EUNIT_PRINT( _L8( "Test step passed" ) );  
    }

//------------------------------------------------------------------------------
// Test case: SetPluginSettings(3)
//------------------------------------------------------------------------------
void MT_CHSPSConfigurationIf::SetPluginSettings_3_L()
    {
    // Pre conditions
    // Set active configuration to Typical configuration
    EUNIT_PRINT( _L8( "Pre conditions: Set Active configuration Typical" ) );
    SetActiveConfigurationL( KHSPSTestAppUid, KHSPSActiveConfTypical );    
    // Attach to HSPS 
    EUNIT_PRINT( _L8( "Pre conditions: Attach to HSPS service IConfiguration interface" ) );
    AttachServiceL( KHSPS, KHSPSConfigurationIf, KHSPSTestAppUid );

    // Test step 1
    EUNIT_PRINT( _L8( "Test step 1" ) );
    RunTestStepSyncL(
        ( TUint8* )setpluginsettings_3_ts_1_method,
        ( TUint8* )setpluginsettings_3_ts_1_input,
        ( TUint8* )setpluginsettings_3_ts_1_output );
    EUNIT_PRINT( _L8( "Test step passed" ) );  
    }

//------------------------------------------------------------------------------
// Test case: SetPluginSettings(4)
//------------------------------------------------------------------------------
void MT_CHSPSConfigurationIf::SetPluginSettings_4_L()
    {
    // Pre conditions
    // Set active configuration to Typical configuration
    EUNIT_PRINT( _L8( "Pre conditions: Set Active configuration Typical" ) );
    SetActiveConfigurationL( KHSPSTestAppUid, KHSPSActiveConfTypical );    
    // Attach to HSPS 
    EUNIT_PRINT( _L8( "Pre conditions: Attach to HSPS service IConfiguration interface" ) );
    AttachServiceL( KHSPS, KHSPSConfigurationIf, KHSPSTestAppUid );

    // Test step 1
    EUNIT_PRINT( _L8( "Test step 1" ) );
    RunTestStepSyncL(
        ( TUint8* )setpluginsettings_4_ts_1_method,
        ( TUint8* )setpluginsettings_4_ts_1_input,
        ( TUint8* )setpluginsettings_4_ts_1_output );
    EUNIT_PRINT( _L8( "Test step passed" ) );  
    }

//------------------------------------------------------------------------------
// Test case: SetPluginSettings(5)
//------------------------------------------------------------------------------
void MT_CHSPSConfigurationIf::SetPluginSettings_5_L()
    {
    // Pre conditions
    // Set active configuration to Typical configuration
    EUNIT_PRINT( _L8( "Pre conditions: Set Active configuration Typical" ) );
    SetActiveConfigurationL( KHSPSTestAppUid, KHSPSActiveConfTypical );    
    // Attach to HSPS 
    EUNIT_PRINT( _L8( "Pre conditions: Attach to HSPS service IConfiguration interface" ) );
    AttachServiceL( KHSPS, KHSPSConfigurationIf, KHSPSTestAppUid );

    // Test step 1
    EUNIT_PRINT( _L8( "Test step 1" ) );
    RunTestStepSyncL(
        ( TUint8* )setpluginsettings_5_ts_1_method,
        ( TUint8* )setpluginsettings_5_ts_1_input,
        ( TUint8* )setpluginsettings_5_ts_1_output );
    EUNIT_PRINT( _L8( "Test step passed" ) );  
    }

//------------------------------------------------------------------------------
// Test case: SetPluginSettings(6)
//------------------------------------------------------------------------------
void MT_CHSPSConfigurationIf::SetPluginSettings_6_L()
    {
    // Pre conditions
    // Set active configuration to Typical configuration
    EUNIT_PRINT( _L8( "Pre conditions: Set Active configuration Typical" ) );
    SetActiveConfigurationL( KHSPSTestAppUid, KHSPSActiveConfTypical );    
    // Attach to HSPS 
    EUNIT_PRINT( _L8( "Pre conditions: Attach to HSPS service IConfiguration interface" ) );
    AttachServiceL( KHSPS, KHSPSConfigurationIf, KHSPSTestAppUid );

    // Test step 1
    EUNIT_PRINT( _L8( "Test step 1" ) );
    RunTestStepSyncL(
        ( TUint8* )setpluginsettings_6_ts_1_method,
        ( TUint8* )setpluginsettings_6_ts_1_input,
        ( TUint8* )setpluginsettings_6_ts_1_output );
    EUNIT_PRINT( _L8( "Test step passed" ) );
    }

//------------------------------------------------------------------------------
// Test case: GetPluginSettings(1)
//------------------------------------------------------------------------------
void MT_CHSPSConfigurationIf::GetPluginSettings_1_L()
    {
    // Pre conditions
    // Set active configuration to Typical configuration
    EUNIT_PRINT( _L8( "Pre conditions: Set Active configuration Typical" ) );
    SetActiveConfigurationL( KHSPSTestAppUid, KHSPSActiveConfTypical );    
    // Attach to HSPS 
    EUNIT_PRINT( _L8( "Pre conditions: Attach to HSPS service IConfiguration interface" ) );
    AttachServiceL( KHSPS, KHSPSConfigurationIf, KHSPSTestAppUid );
    
    // Test step 1
    EUNIT_PRINT( _L8( "Test step 1" ) );
    RunTestStepSyncL(
        ( TUint8* )getpluginsettings_1_ts_1_method,
        ( TUint8* )getpluginsettings_1_ts_1_input,
        ( TUint8* )getpluginsettings_1_ts_1_output,
        ETrue );
    EUNIT_PRINT( _L8( "Test step passed" ) );  
    }  

//------------------------------------------------------------------------------
// Test case: GetPluginSettings(2)
//------------------------------------------------------------------------------
void MT_CHSPSConfigurationIf::GetPluginSettings_2_L()
    {
    // Pre conditions
    // Set active configuration to Typical configuration
    EUNIT_PRINT( _L8( "Pre conditions: Set Active configuration Typical" ) );
    SetActiveConfigurationL( KHSPSTestAppUid, KHSPSActiveConfTypical );    
    // Attach to HSPS 
    EUNIT_PRINT( _L8( "Pre conditions: Attach to HSPS service IConfiguration interface" ) );
    AttachServiceL( KHSPS, KHSPSConfigurationIf, KHSPSTestAppUid );
    
    // Test step 1
    EUNIT_PRINT( _L8( "Test step 1" ) );
    RunTestStepSyncL(
        ( TUint8* )getpluginsettings_2_ts_1_method,
        ( TUint8* )getpluginsettings_2_ts_1_input,
        ( TUint8* )getpluginsettings_2_ts_1_output );
    EUNIT_PRINT( _L8( "Test step passed" ) );
    }  

//------------------------------------------------------------------------------
// Test case: GetPluginSettings(3)
//------------------------------------------------------------------------------
void MT_CHSPSConfigurationIf::GetPluginSettings_3_L()
    {
    // Pre conditions
    // Set active configuration to Typical configuration
    EUNIT_PRINT( _L8( "Pre conditions: Set Active configuration Typical" ) );
    SetActiveConfigurationL( KHSPSTestAppUid, KHSPSActiveConfTypical );    
    // Attach to HSPS 
    EUNIT_PRINT( _L8( "Pre conditions: Attach to HSPS service IConfiguration interface" ) );
    AttachServiceL( KHSPS, KHSPSConfigurationIf, KHSPSTestAppUid );
    
    // Test step 1
    EUNIT_PRINT( _L8( "Test step 1" ) );
    RunTestStepSyncL(
        ( TUint8* )getpluginsettings_3_ts_1_method,
        ( TUint8* )getpluginsettings_3_ts_1_input,
        ( TUint8* )getpluginsettings_3_ts_1_output );
    EUNIT_PRINT( _L8( "Test step passed" ) );
    }  

//------------------------------------------------------------------------------
// Test case: MovePlugins(1)
//------------------------------------------------------------------------------
void MT_CHSPSConfigurationIf::MovePlugins_1_L()
    {
    // Pre conditions
    // Set active configuration to Typical configuration
    EUNIT_PRINT( _L8( "Pre conditions: Set Active configuration Typical" ) );
    SetActiveConfigurationL( KHSPSTestAppUid, KHSPSActiveConfTypical );
    
    // Attach to HSPS 
    EUNIT_PRINT( _L8( "Pre conditions: Attach to HSPS service IConfiguration interface" ) );
    AttachServiceL( KHSPS, KHSPSConfigurationIf, KHSPSTestAppUid );
    
    // Test step 1
    EUNIT_PRINT( _L8( "Test step 1" ) );
    RunTestStepSyncL(
        ( TUint8* )moveplugins_1_ts_1_method,
        ( TUint8* )moveplugins_1_ts_1_input,
        ( TUint8* )moveplugins_1_ts_1_output,
        ETrue );
    EUNIT_PRINT( _L8( "Test step passed" ) );          

    // Test step 2
    EUNIT_PRINT( _L8( "Test step 2" ) );
    RunTestStepSyncL(
        ( TUint8* )moveplugins_1_ts_2_method,
        ( TUint8* )moveplugins_1_ts_2_input,
        ( TUint8* )moveplugins_1_ts_2_output );
    EUNIT_PRINT( _L8( "Test step passed" ) );          

    // Test step 3
    EUNIT_PRINT( _L8( "Test step 3" ) );
    RunTestStepSyncL(
        ( TUint8* )moveplugins_1_ts_3_method,
        ( TUint8* )moveplugins_1_ts_3_input,
        ( TUint8* )moveplugins_1_ts_3_output );
    EUNIT_PRINT( _L8( "Test step passed" ) );          

    // Test step 4
    EUNIT_PRINT( _L8( "Test step 4" ) );
    RunTestStepSyncL(
        ( TUint8* )moveplugins_1_ts_4_method,
        ( TUint8* )moveplugins_1_ts_4_input,
        ( TUint8* )moveplugins_1_ts_4_output );
    EUNIT_PRINT( _L8( "Test step passed" ) );          
    }

//------------------------------------------------------------------------------
// Test case: MovePlugins(2)
//------------------------------------------------------------------------------
void MT_CHSPSConfigurationIf::MovePlugins_2_L()
    {
    // Pre conditions
    // Set active configuration to Typical configuration
    EUNIT_PRINT( _L8( "Pre conditions: Set Active configuration Typical" ) );
    SetActiveConfigurationL( KHSPSTestAppUid, KHSPSActiveConfTypical );
    
    // Attach to HSPS 
    EUNIT_PRINT( _L8( "Pre conditions: Attach to HSPS service IConfiguration interface" ) );
    AttachServiceL( KHSPS, KHSPSConfigurationIf, KHSPSTestAppUid );
    
    // Test step 1
    EUNIT_PRINT( _L8( "Test step 1" ) );
    RunTestStepSyncL(
        ( TUint8* )moveplugins_2_ts_1_method,
        ( TUint8* )moveplugins_2_ts_1_input,
        ( TUint8* )moveplugins_2_ts_1_output );
    EUNIT_PRINT( _L8( "Test step passed" ) );          
    }

//------------------------------------------------------------------------------
// Test case: MovePlugins(3)
//------------------------------------------------------------------------------
void MT_CHSPSConfigurationIf::MovePlugins_3_L()
    {
    // Pre conditions
    // Set active configuration to Typical configuration
    EUNIT_PRINT( _L8( "Pre conditions: Set Active configuration Typical" ) );
    SetActiveConfigurationL( KHSPSTestAppUid, KHSPSActiveConfTypical );
    
    // Attach to HSPS 
    EUNIT_PRINT( _L8( "Pre conditions: Attach to HSPS service IConfiguration interface" ) );
    AttachServiceL( KHSPS, KHSPSConfigurationIf, KHSPSTestAppUid );
    
    // Test step 1
    EUNIT_PRINT( _L8( "Test step 1" ) );
    RunTestStepSyncL(
        ( TUint8* )moveplugins_3_ts_1_method,
        ( TUint8* )moveplugins_3_ts_1_input,
        ( TUint8* )moveplugins_3_ts_1_output );
    EUNIT_PRINT( _L8( "Test step passed" ) );          
    }

//------------------------------------------------------------------------------
// Test case: MovePlugins(4)
//------------------------------------------------------------------------------
void MT_CHSPSConfigurationIf::MovePlugins_4_L()
    {
    // Pre conditions
    // Set active configuration to Typical configuration
    EUNIT_PRINT( _L8( "Pre conditions: Set Active configuration Typical" ) );
    SetActiveConfigurationL( KHSPSTestAppUid, KHSPSActiveConfTypical );
    
    // Attach to HSPS 
    EUNIT_PRINT( _L8( "Pre conditions: Attach to HSPS service IConfiguration interface" ) );
    AttachServiceL( KHSPS, KHSPSConfigurationIf, KHSPSTestAppUid );
    
    // Test step 1
    EUNIT_PRINT( _L8( "Test step 1" ) );
    RunTestStepSyncL(
        ( TUint8* )moveplugins_4_ts_1_method,
        ( TUint8* )moveplugins_4_ts_1_input,
        ( TUint8* )moveplugins_4_ts_1_output );
    EUNIT_PRINT( _L8( "Test step passed" ) );          
    }

//------------------------------------------------------------------------------
// Test case: MovePlugins(5)
//------------------------------------------------------------------------------
void MT_CHSPSConfigurationIf::MovePlugins_5_L()
    {
    // Pre conditions
    // Set active configuration to Typical configuration
    EUNIT_PRINT( _L8( "Pre conditions: Set Active configuration Typical" ) );
    SetActiveConfigurationL( KHSPSTestAppUid, KHSPSActiveConfTypical );
    
    // Attach to HSPS 
    EUNIT_PRINT( _L8( "Pre conditions: Attach to HSPS service IConfiguration interface" ) );
    AttachServiceL( KHSPS, KHSPSConfigurationIf, KHSPSTestAppUid );
    
    // Test step 1
    EUNIT_PRINT( _L8( "Test step 1" ) );
    RunTestStepSyncL(
        ( TUint8* )moveplugins_5_ts_1_method,
        ( TUint8* )moveplugins_5_ts_1_input,
        ( TUint8* )moveplugins_5_ts_1_output );
    EUNIT_PRINT( _L8( "Test step passed" ) );          
    }

//------------------------------------------------------------------------------
// Test case: MovePlugins(6)
//------------------------------------------------------------------------------
void MT_CHSPSConfigurationIf::MovePlugins_6_L()
    {
    // Pre conditions
        
    // Attach to HSPS 
    EUNIT_PRINT( _L8( "Pre conditions: Attach to HSPS service IConfiguration interface" ) );
    AttachServiceL( KHSPS, KHSPSConfigurationIf, KHSPSTestAppUid );
    
    // Test step 1
    EUNIT_PRINT( _L8( "Test step 1" ) );
    RunTestStepSyncL(
        ( TUint8* )moveplugins_6_ts_1_method,
        ( TUint8* )moveplugins_6_ts_1_input,
        ( TUint8* )moveplugins_6_ts_1_output );
    EUNIT_PRINT( _L8( "Test step passed" ) );
    
    // Test step 2
    EUNIT_PRINT( _L8( "Test step 2" ) );
    RunTestStepSyncL(
        ( TUint8* )moveplugins_6_ts_2_method,
        ( TUint8* )moveplugins_6_ts_2_input,
        ( TUint8* )moveplugins_6_ts_2_output );
    EUNIT_PRINT( _L8( "Test step passed" ) );          

    // Test step 3
    EUNIT_PRINT( _L8( "Test step 3" ) );
    RunTestStepSyncL(
        ( TUint8* )moveplugins_6_ts_3_method,
        ( TUint8* )moveplugins_6_ts_3_input,
        ( TUint8* )moveplugins_6_ts_3_output );
    EUNIT_PRINT( _L8( "Test step passed" ) );          
    
    // Test step 4
    EUNIT_PRINT( _L8( "Test step 4" ) );
    RunTestStepSyncL(
        ( TUint8* )moveplugins_6_ts_4_method,
        ( TUint8* )moveplugins_6_ts_4_input,
        ( TUint8* )moveplugins_6_ts_4_output );
    EUNIT_PRINT( _L8( "Test step passed" ) );                  
    
    // Test step 5
    EUNIT_PRINT( _L8( "Test step 5" ) );
    RunTestStepSyncL(
        ( TUint8* )moveplugins_6_ts_5_method,
        ( TUint8* )moveplugins_6_ts_5_input,
        ( TUint8* )moveplugins_6_ts_5_output );
    EUNIT_PRINT( _L8( "Test step passed" ) );
    }

//------------------------------------------------------------------------------
// Test case: ReplacePlugin_1_L(1)
//------------------------------------------------------------------------------
void MT_CHSPSConfigurationIf::ReplacePlugin_1_L()
    {
    // Pre conditions
    
    // Set active configuration to Typical configuration
    EUNIT_PRINT( _L8( "Pre conditions: active configuration is Typical" ) );
    SetActiveConfigurationL( KHSPSTestAppUid, KHSPSActiveConfTypical );
           
    // Attach to HSPS 
    EUNIT_PRINT( _L8( "Pre conditions: Attach to HSPS service IConfiguration interface" ) );
    AttachServiceL( KHSPS, KHSPSConfigurationIf, KHSPSTestAppUid );
     
    // Test step 1
    EUNIT_PRINT( _L8( "Test step 1" ) );
    RunTestStepSyncL(
        ( TUint8* )replaceplugin_1_ts_1_method,
        ( TUint8* )replaceplugin_1_ts_1_input,
        ( TUint8* )replaceplugin_1_ts_1_output );
    EUNIT_PRINT( _L8( "Test step passed" ) );
    
    // Test step 2
    EUNIT_PRINT( _L8( "Test step 2" ) );
    RunTestStepSyncL(
        ( TUint8* )replaceplugin_1_ts_2_method,
        ( TUint8* )replaceplugin_1_ts_2_input,
        ( TUint8* )replaceplugin_1_ts_2_output );
    EUNIT_PRINT( _L8( "Test step passed" ) );
                  
    // Test step 3
    EUNIT_PRINT( _L8( "Test step 3" ) );
    RunTestStepSyncL(
        ( TUint8* )replaceplugin_1_ts_3_method,
        ( TUint8* )replaceplugin_1_ts_3_input,
        ( TUint8* )replaceplugin_1_ts_3_output );
    EUNIT_PRINT( _L8( "Test step passed" ) );
    
    // Test step 4
    EUNIT_PRINT( _L8( "Test step 4" ) );
    RunTestStepSyncL(
        ( TUint8* )replaceplugin_1_ts_4_method,
        ( TUint8* )replaceplugin_1_ts_4_input,
        ( TUint8* )replaceplugin_1_ts_4_output );
    EUNIT_PRINT( _L8( "Test step passed" ) );
        
    // Test step 5
    EUNIT_PRINT( _L8( "Test step 5" ) );
    RunTestStepSyncL(
        ( TUint8* )replaceplugin_1_ts_5_method,
        ( TUint8* )replaceplugin_1_ts_5_input,
        ( TUint8* )replaceplugin_1_ts_5_output );
    EUNIT_PRINT( _L8( "Test step passed" ) );
    
    // Test step 6
    EUNIT_PRINT( _L8( "Test step 6" ) );
    RunTestStepSyncL(
        ( TUint8* )replaceplugin_1_ts_6_method,
        ( TUint8* )replaceplugin_1_ts_6_input,
        ( TUint8* )replaceplugin_1_ts_6_output );
    EUNIT_PRINT( _L8( "Test step passed" ) );
    
    // Test step 7
    EUNIT_PRINT( _L8( "Test step 7" ) );
    RunTestStepSyncL(
        ( TUint8* )replaceplugin_1_ts_7_method,
        ( TUint8* )replaceplugin_1_ts_7_input,
        ( TUint8* )replaceplugin_1_ts_7_output );
    EUNIT_PRINT( _L8( "Test step passed" ) );
    
    // Test step 8
    EUNIT_PRINT( _L8( "Test step 8" ) );
    RunTestStepSyncL(
        ( TUint8* )replaceplugin_1_ts_8_method,
        ( TUint8* )replaceplugin_1_ts_8_input,
        ( TUint8* )replaceplugin_1_ts_8_output );
    EUNIT_PRINT( _L8( "Test step passed" ) );
    
    // Test step 9
    EUNIT_PRINT( _L8( "Test step 9" ) );
    RunTestStepSyncL(
        ( TUint8* )replaceplugin_1_ts_9_method,
        ( TUint8* )replaceplugin_1_ts_9_input,
        ( TUint8* )replaceplugin_1_ts_9_output );
    EUNIT_PRINT( _L8( "Test step passed" ) );    
        
    // Make sure resources of the typical-widget are removed 
    // from client's private folder, should be restored in next step    
    if ( BaflUtils::FileExists( iFileserver, _L( "c:\\private\\20000fb1\\2456\\270513751\\536916256\\1.0\\sources\\hs_logo.jpg" ) )
        || BaflUtils::FileExists( iFileserver, _L( "c:\\private\\20000fb1\\2456\\270513751\\536916256\\1.0\\sources\\widget.bmp" ) ) )
        {
        CFileMan* fileManager = CFileMan::NewL( iFileserver );
        CleanupStack::PushL( fileManager );        
        User::LeaveIfError( fileManager->Delete( _L( "c:\\private\\20000fb1\\2456\\270513751\\536916256\\1.0\\sources\\hs_logo.jpg" ) ) );     
        User::LeaveIfError( fileManager->Delete( _L( "c:\\private\\20000fb1\\2456\\270513751\\536916256\\1.0\\sources\\widget.bmp" ) ) );
        CleanupStack::PopAndDestroy( fileManager );
        if ( BaflUtils::FileExists( iFileserver, _L( "c:\\private\\20000fb1\\2456\\270513751\\536916256\\1.0\\sources\\hs_logo.jpg" ))
                || BaflUtils::FileExists( iFileserver, _L( "c:\\private\\20000fb1\\2456\\270513751\\536916256\\1.0\\sources\\widget.bmp" ) ) )
            {
            User::Leave( KErrGeneral );
            }        
        }
    
    // Test step 10
    EUNIT_PRINT( _L8( "Test step 10" ) );
    RunTestStepSyncL(
        ( TUint8* )replaceplugin_1_ts_10_method,
        ( TUint8* )replaceplugin_1_ts_10_input,
        ( TUint8* )replaceplugin_1_ts_10_output );
    EUNIT_PRINT( _L8( "Test step passed" ) );
    
    // Test step 11
    EUNIT_PRINT( _L8( "Test step 11" ) );
    RunTestStepSyncL(
        ( TUint8* )replaceplugin_1_ts_11_method,
        ( TUint8* )replaceplugin_1_ts_11_input,
        ( TUint8* )replaceplugin_1_ts_11_output );
    EUNIT_PRINT( _L8( "Test step passed" ) );
    
    EUNIT_PRINT( _L8( "post condition check for resource file copy" ) );
    ResetResources();
    AddResourceL( _L( "c:\\private\\20000fb1\\2456\\270513751\\536916256\\1.0\\sources\\hs_logo.jpg" ), 18179 );        
    AddResourceL( _L( "c:\\private\\20000fb1\\2456\\270513751\\536916256\\1.0\\sources\\widget.bmp" ), 6446 );        
    CheckResourcesL();     
    EUNIT_PRINT( _L8( "post condition check for resource file copy passed" ) );
    }

//------------------------------------------------------------------------------
// Test case: GetAppConfs(1)
//------------------------------------------------------------------------------
void MT_CHSPSConfigurationIf::GetAppConfs_1_L()
    {
    // Pre conditions
    // Set active configuration to Typical configuration
    EUNIT_PRINT( _L8( "Pre conditions: Set Active configuration Minimal" ) );
    SetActiveConfigurationL( KHSPSTestAppUid, KHSPSActiveConfMinimal );
    
    // Attach to HSPS 
    EUNIT_PRINT( _L8( "Pre conditions: Attach to HSPS service IConfiguration interface" ) );
    AttachServiceL( KHSPS, KHSPSConfigurationIf, KHSPSTestAppUid );
    
    // Test step 1
    EUNIT_PRINT( _L8( "Test step 1" ) );
    RunTestStepSyncL(
        ( TUint8* )getappconfs_1_ts_1_method,
        ( TUint8* )getappconfs_1_ts_1_input,
        ( TUint8* )getappconfs_1_ts_1_output,
        ETrue );
    EUNIT_PRINT( _L8( "Test step passed" ) );
    }

//------------------------------------------------------------------------------
// Test case: SetActiveAppConf(1)
//------------------------------------------------------------------------------
void MT_CHSPSConfigurationIf::SetActiveAppConf_1_L()
    {
    // Pre conditions
    // Set active configuration to Typical configuration
    EUNIT_PRINT( _L8( "Pre conditions: Set Active configuration Minimal" ) );
    SetActiveConfigurationL( KHSPSTestAppUid, KHSPSActiveConfMinimal );
    
    // Attach to HSPS 
    EUNIT_PRINT( _L8( "Pre conditions: Attach to HSPS service IConfiguration interface" ) );
    AttachServiceL( KHSPS, KHSPSConfigurationIf, KHSPSTestAppUid );
    
    // Test step 1
    EUNIT_PRINT( _L8( "Test step 1" ) );
    RunTestStepSyncL(
        ( TUint8* )setactiveappconf_1_ts_1_method,
        ( TUint8* )setactiveappconf_1_ts_1_input,
        ( TUint8* )setactiveappconf_1_ts_1_output,
        ETrue );
    EUNIT_PRINT( _L8( "Test step passed" ) );          

    // Test step 2
    EUNIT_PRINT( _L8( "Test step 2" ) );
    RunTestStepSyncL(
        ( TUint8* )setactiveappconf_1_ts_2_method,
        ( TUint8* )setactiveappconf_1_ts_2_input,
        ( TUint8* )setactiveappconf_1_ts_2_output );
    EUNIT_PRINT( _L8( "Test step passed" ) );          
    }

//------------------------------------------------------------------------------
// Test case: SetActiveAppConf(2)
//------------------------------------------------------------------------------
void MT_CHSPSConfigurationIf::SetActiveAppConf_2_L()
    {
    // Pre conditions
    // Set active configuration to Typical configuration
    EUNIT_PRINT( _L8( "Pre conditions: Set Active configuration Minimal" ) );
    SetActiveConfigurationL( KHSPSTestAppUid, KHSPSActiveConfMinimal );
    
    // Attach to HSPS 
    EUNIT_PRINT( _L8( "Pre conditions: Attach to HSPS service IConfiguration interface" ) );
    AttachServiceL( KHSPS, KHSPSConfigurationIf, KHSPSTestAppUid );
    
    // Test step 1
    EUNIT_PRINT( _L8( "Test step 1" ) );
    RunTestStepSyncL(
        ( TUint8* )setactiveappconf_2_ts_1_method,
        ( TUint8* )setactiveappconf_2_ts_1_input,
        ( TUint8* )setactiveappconf_2_ts_1_output );

    EUNIT_PRINT( _L8( "Test step passed" ) );          

    // Test step 2
    EUNIT_PRINT( _L8( "Test step 2" ) );

    RunTestStepSyncL(
        ( TUint8* )setactiveappconf_2_ts_2_method,
        ( TUint8* )setactiveappconf_2_ts_2_input,
        ( TUint8* )setactiveappconf_2_ts_2_output );
    
    EUNIT_PRINT( _L8( "Test step passed" ) );
    }

//------------------------------------------------------------------------------
// Test case: SetActiveAppConf(3)
//------------------------------------------------------------------------------
void MT_CHSPSConfigurationIf::SetActiveAppConf_3_L()
    {
    // Pre conditions
    // Set active configuration to Typical configuration
    EUNIT_PRINT( _L8( "Pre conditions: Set Active configuration Minimal" ) );
    SetActiveConfigurationL( KHSPSTestAppUid, KHSPSActiveConfMinimal );
    
    // Attach to HSPS 
    EUNIT_PRINT( _L8( "Pre conditions: Attach to HSPS service IConfiguration interface" ) );
    AttachServiceL( KHSPS, KHSPSConfigurationIf, KHSPSTestAppUid );
    
    // Test step 1
    EUNIT_PRINT( _L8( "Test step 1" ) );
    RunTestStepSyncL(
        ( TUint8* )setactiveappconf_3_ts_1_method,
        ( TUint8* )setactiveappconf_3_ts_1_input,
        ( TUint8* )setactiveappconf_3_ts_1_output );

    EUNIT_PRINT( _L8( "Test step passed" ) );          

    // Test step 2
    EUNIT_PRINT( _L8( "Test step 2" ) );

    RunTestStepSyncL(
        ( TUint8* )setactiveappconf_3_ts_2_method,
        ( TUint8* )setactiveappconf_3_ts_2_input,
        ( TUint8* )setactiveappconf_3_ts_2_output );
    
    EUNIT_PRINT( _L8( "Test step passed" ) );          
    }

//------------------------------------------------------------------------------
// Test case: SetActiveAppConf(4)
//------------------------------------------------------------------------------
void MT_CHSPSConfigurationIf::SetActiveAppConf_4_L()
    {
    // Pre conditions
    // Set active configuration to Typical configuration
    EUNIT_PRINT( _L8( "Pre conditions: Set Active configuration Minimal" ) );
    SetActiveConfigurationL( KHSPSTestAppUid, KHSPSActiveConfMinimal );
    
    // Attach to HSPS 
    EUNIT_PRINT( _L8( "Pre conditions: Attach to HSPS service IConfiguration interface" ) );
    AttachServiceL( KHSPS, KHSPSConfigurationIf, KHSPSTestAppUid );
    
    // Test step 1
    EUNIT_PRINT( _L8( "Test step 1" ) );
    RunTestStepSyncL(
        ( TUint8* )setactiveappconf_4_ts_1_method,
        ( TUint8* )setactiveappconf_4_ts_1_input,
        ( TUint8* )setactiveappconf_4_ts_1_output );

    EUNIT_PRINT( _L8( "Test step passed" ) );          

    // Test step 2
    EUNIT_PRINT( _L8( "Test step 2" ) );

    RunTestStepSyncL(
        ( TUint8* )setactiveappconf_4_ts_2_method,
        ( TUint8* )setactiveappconf_4_ts_2_input,
        ( TUint8* )setactiveappconf_4_ts_2_output );
    
    EUNIT_PRINT( _L8( "Test step passed" ) );          
    }

//------------------------------------------------------------------------------
// Test case: RequestNotify(1)
//------------------------------------------------------------------------------
void MT_CHSPSConfigurationIf::RequestNotify_1_L()
    {
    // Pre conditions
    // Set active configuration to Typical configuration
    EUNIT_PRINT( _L8( "Pre conditions: Set Active configuration Typical" ) );
    SetActiveConfigurationL( KHSPSTestAppUid, KHSPSActiveConfTypical );    
    
    // Test step 1
    EUNIT_PRINT( _L8( "Test step 1" ) );
    RunTestStepAsyncL(
        ( TUint8* )requestnotify_1_ts_1_method,
        ( TUint8* )requestnotify_1_ts_1_input,
        ( TUint8* )requestnotify_1_ts_1_output,
        ( TInt ) requestnotify_1_ts_1_trigger );
        
    EUNIT_PRINT( _L8( "Test step passed" ) );  
    }

//------------------------------------------------------------------------------
// Test case: RequestNotify(2)
//------------------------------------------------------------------------------
void MT_CHSPSConfigurationIf::RequestNotify_2_L()
    {
    // Pre conditions
    // Set active configuration to Typical configuration
    EUNIT_PRINT( _L8( "Pre conditions: Set Active configuration Typical" ) );
    SetActiveConfigurationL( KHSPSTestAppUid, KHSPSActiveConfTypical );    
    
    // Test step 1
    EUNIT_PRINT( _L8( "Test step 1" ) );
    RunTestStepAsyncL(
        ( TUint8* )requestnotify_2_ts_1_method,
        ( TUint8* )requestnotify_2_ts_1_input,
        ( TUint8* )requestnotify_2_ts_1_output,
        ( TInt ) requestnotify_2_ts_1_trigger );
        
    EUNIT_PRINT( _L8( "Test step passed" ) );  
    }

//------------------------------------------------------------------------------
// Test case: RequestNotify(3)
//------------------------------------------------------------------------------
void MT_CHSPSConfigurationIf::RequestNotify_3_L()
    {
    // Pre conditions
    // Set active configuration to Typical configuration
    EUNIT_PRINT( _L8( "Pre conditions: Set Active configuration Typical" ) );
    SetActiveConfigurationL( KHSPSTestAppUid, KHSPSActiveConfTypical );    
    
    // Test step 1
    EUNIT_PRINT( _L8( "Test step 1" ) );
    RunTestStepAsyncL(
        ( TUint8* )requestnotify_3_ts_1_method,
        ( TUint8* )requestnotify_3_ts_1_input,
        ( TUint8* )requestnotify_3_ts_1_output,
        ( TInt ) requestnotify_3_ts_1_trigger );
    
    EUNIT_PRINT( _L8( "Test step passed" ) );  
    }

//------------------------------------------------------------------------------
// Test case: RequestNotify(4)
//------------------------------------------------------------------------------
void MT_CHSPSConfigurationIf::RequestNotify_4_L()
    {
    // Pre conditions
    // Set active configuration to Typical configuration
    EUNIT_PRINT( _L8( "Pre conditions: Set Active configuration Typical" ) );
    SetActiveConfigurationL( KHSPSTestAppUid, KHSPSActiveConfTypical );    
    
    // Test step 1
    EUNIT_PRINT( _L8( "Test step 1" ) );
    RunTestStepAsyncL(
        ( TUint8* )requestnotify_4_ts_1_method,
        ( TUint8* )requestnotify_4_ts_1_input,
        ( TUint8* )requestnotify_4_ts_1_output,
        ( TInt ) requestnotify_4_ts_1_trigger );
    
    EUNIT_PRINT( _L8( "Test step passed" ) );  

    // Test step 2
    EUNIT_PRINT( _L8( "Test step 2" ) );
    RunTestStepAsyncL(
        ( TUint8* )requestnotify_4_ts_2_method,
        ( TUint8* )requestnotify_4_ts_2_input,
        ( TUint8* )requestnotify_4_ts_2_output,
        ( TInt ) requestnotify_4_ts_2_trigger );
    
    EUNIT_PRINT( _L8( "Test step passed" ) );  
    }

//------------------------------------------------------------------------------
// Test case: RequestNotify(5)
//------------------------------------------------------------------------------
void MT_CHSPSConfigurationIf::RequestNotify_5_L()
    {
    // Pre conditions
    // Set active configuration to Typical configuration
    EUNIT_PRINT( _L8( "Pre conditions: Set Active configuration Typical" ) );
    SetActiveConfigurationL( KHSPSTestAppUid, KHSPSActiveConfTypical );    
    
    // Test step 1
    EUNIT_PRINT( _L8( "Test step 1" ) );
    RunTestStepAsyncL(
        ( TUint8* )requestnotify_5_ts_1_method,
        ( TUint8* )requestnotify_5_ts_1_input,
        ( TUint8* )requestnotify_5_ts_1_output,
        ( TInt ) requestnotify_5_ts_1_trigger );
    
    EUNIT_PRINT( _L8( "Test step passed" ) );  
    }

//------------------------------------------------------------------------------
// Test case: RequestNotify(6)
//------------------------------------------------------------------------------
void MT_CHSPSConfigurationIf::RequestNotify_6_L()
    {
    // Pre conditions
    // Set active configuration to Minimal configuration
    EUNIT_PRINT( _L8( "Pre conditions: Set Active configuration Minimal" ) );
    SetActiveConfigurationL( KHSPSTestAppUid, KHSPSActiveConfMinimal );    
                        
    // Test step 1: install a SISX package and check output from received notifiction
    EUNIT_PRINT( _L8( "Test step 1" ) );
    RunTestStepAsyncL(
        ( TUint8* )requestnotify_6_ts_1_method,
        ( TUint8* )requestnotify_6_ts_1_input,
        ( TUint8* )requestnotify_6_ts_1_output,
        ( TInt ) requestnotify_6_ts_1_trigger );    
    EUNIT_PRINT( _L8( "Test step passed" ) );  
            
    // Test step 2: update the SISX package and check output from received notifiction
    EUNIT_PRINT( _L8( "Test step 2" ) );
    RunTestStepAsyncL(
        ( TUint8* )requestnotify_6_ts_2_method,
        ( TUint8* )requestnotify_6_ts_2_input,
        ( TUint8* )requestnotify_6_ts_2_output,
        ( TInt ) requestnotify_6_ts_2_trigger );    
    EUNIT_PRINT( _L8( "Test step passed" ) );
    
    // Attach to HSPS, get active app configuration which generates full application configuration with IDs 
    EUNIT_PRINT( _L8( "Attach to HSPS service IConfiguration interface" ) );
    AttachServiceL( KHSPS, KHSPSConfigurationIf, KHSPSTestAppUid );
                    
    // Test step 3: add the installed v2 plugin to 1st view in the Minimal root configuration
    EUNIT_PRINT( _L8( "Test step 3" ) );
    RunTestStepSyncL(
        ( TUint8* )requestnotify_6_ts_3_method,
        ( TUint8* )requestnotify_6_ts_3_input,
        ( TUint8* )requestnotify_6_ts_3_output );        
    EUNIT_PRINT( _L8( "Test step passed" ) );

    // Detach so that the next Typical app configuration gets read
    TRAP_IGNORE( DetachServiceL() );
    
    // Set active configuration to Typical configuration
    EUNIT_PRINT( _L8( "Set Active configuration Typical" ) );
    SetActiveConfigurationL( KHSPSTestAppUid, KHSPSActiveConfTypical );
    
    // Attach to HSPS, get active app configuration which generates full application configuration with IDs 
    EUNIT_PRINT( _L8( "Attach to HSPS service IConfiguration interface" ) );
    AttachServiceL( KHSPS, KHSPSConfigurationIf, KHSPSTestAppUid );
                            
    // Test step 4: add the installed v2 plugin to 1st view in the Typical root configuration
    EUNIT_PRINT( _L8( "Test step 4" ) );
    RunTestStepSyncL(
        ( TUint8* )requestnotify_6_ts_4_method,
        ( TUint8* )requestnotify_6_ts_4_input,
        ( TUint8* )requestnotify_6_ts_4_output );
    EUNIT_PRINT( _L8( "Test step passed" ) );    
            
    TRAP_IGNORE( DetachServiceL() );
    
    // Test step 5: uninstall the SISX package and check output from received notification
    EUNIT_PRINT( _L8( "Test step 5" ) );
    RunTestStepAsyncL(
        ( TUint8* )requestnotify_6_ts_5_method,
        ( TUint8* )requestnotify_6_ts_5_input,
        ( TUint8* )requestnotify_6_ts_5_output,
        ( TInt ) requestnotify_6_ts_5_trigger );
    EUNIT_PRINT( _L8( "Test step passed" ) );
    
    EUNIT_PRINT( _L8( "Set Active configuration Minimal" ) );
    SetActiveConfigurationL( KHSPSTestAppUid, KHSPSActiveConfMinimal );
               
    // Attach to HSPS, get active app configuration which generates full application configuration with IDs 
    EUNIT_PRINT( _L8( "Pre conditions: Attach to HSPS service IConfiguration interface" ) );
    AttachServiceL( KHSPS, KHSPSConfigurationIf, KHSPSTestAppUid );
                                       
    // Test step 6: get plugin conf from the plugin instance in the Minimal root configuration    
    EUNIT_PRINT( _L8( "Test step 6" ) );    
    RunTestStepSyncL(
        ( TUint8* )requestnotify_6_ts_6_method,
        ( TUint8* )requestnotify_6_ts_6_input,
        ( TUint8* )requestnotify_6_ts_6_output );    
    EUNIT_PRINT( _L8( "Test step passed" ) );       
    }

//------------------------------------------------------------------------------
// Test case: SisxInstallation(1)
//------------------------------------------------------------------------------
void MT_CHSPSConfigurationIf::SisxInstallation_1_L()
    {       
    // Pre conditions        
    // Attach to HSPS 
    EUNIT_PRINT( _L8( "Pre conditions: Attach to HSPS service IConfiguration interface" ) );
    AttachServiceL( KHSPS, KHSPSConfigurationIf, KHSPSTestAppUid );

    // Test step 1: 
    EUNIT_PRINT( _L8( "Test step 1: simulate installation of a SISX package" ) );

    CFileMan* fileManager = CFileMan::NewL( iFileserver );
    CleanupStack::PushL( fileManager );

    // Copy installation files to c\private\200159c0\import
    User::LeaveIfError( fileManager->Copy(
        _L( "c:\\data\\mt_hsps\\installed_widget\\0\\" ),
        _L( "c:\\private\\200159c0\\import\\0998\\101FB657\\2000B133\\1.0\\0\\" ),
        CFileMan::ERecurse|CFileMan::EOverWrite ) );
    User::LeaveIfError( fileManager->Copy(
        _L( "c:\\data\\mt_hsps\\installed_widget\\1\\" ),
        _L( "c:\\private\\200159c0\\import\\0998\\101FB657\\2000B133\\1.0\\1\\" ),
        CFileMan::ERecurse|CFileMan::EOverWrite ) );
    User::LeaveIfError( fileManager->Copy(
        _L( "c:\\data\\mt_hsps\\installed_widget\\9\\" ),
        _L( "c:\\private\\200159c0\\import\\0998\\101FB657\\2000B133\\1.0\\9\\" ),
        CFileMan::ERecurse|CFileMan::EOverWrite ) );
    User::LeaveIfError( fileManager->Copy(
        _L( "c:\\data\\mt_hsps\\installed_widget\\widgetconfiguration.xml" ),
        _L( "c:\\private\\200159c0\\import\\0998\\101FB657\\2000B133\\1.0\\" ),
        CFileMan::EOverWrite ) );
    User::LeaveIfError( fileManager->Copy(
        _L( "c:\\data\\mt_hsps\\installed_widget\\manifest.dat" ),
        _L( "c:\\private\\200159c0\\import\\0998\\101FB657\\2000B133\\1.0\\" ),
        CFileMan::EOverWrite ) );
    User::LeaveIfError( fileManager->Copy(
        _L( "c:\\data\\mt_hsps\\installed_widget\\plugin_0998_101FB657_2000B133_1.0.dat" ),
        _L( "c:\\private\\200159c0\\import\\" ),
        CFileMan::EOverWrite ) );
            
    // Wait until configuration is installed
    User::After( 5000000 );
    
    // Make sure "InstalledWidget" is installed
    if ( !BaflUtils::FileExists( iFileserver, _L( "c:\\private\\200159c0\\themes\\2456\\270513751\\536916275\\1.0\\InstallWidgetConf.o0000" ) ) )
        {
        User::Leave( KErrGeneral );
        }
    
    EUNIT_PRINT( _L8( "Test step passed" ) );
        
    // Test step 2: 
    EUNIT_PRINT( _L8( "Test step 2: simulate un-installation of the SISX package" ) );
    
    // Remove installation files from c\private\200159c0\import
    User::LeaveIfError( fileManager->RmDir( _L( "c:\\private\\200159c0\\import\\0998\\" ) ) );
    fileManager->Attribs( _L( "c:\\private\\200159c0\\import\\plugin_0998_101FB657_2000B133.dat" ),
                          0,
                          KEntryAttReadOnly,
                          TTime( 0 ) ); // TTime(0) = preserve original time stamp.
    User::LeaveIfError( fileManager->Delete( _L( "c:\\private\\200159c0\\import\\plugin_0998_101FB657_2000B133_1.0.dat" ) ) );     
    // Removing of *.dat file causes configuration uninstallation
    // Wait until configuration is uninstalled
    User::After( 5000000 );

    // Make sure "InstalledWidget" is uninstalled
    if ( BaflUtils::FileExists( iFileserver, _L( "c:\\private\\200159c0\\themes\\2456\\270513751\\536916275\\1.0\\InstallWidgetConf.o0000" ) ) )
        {
        User::Leave( KErrGeneral );
        }

    EUNIT_PRINT( _L8( "Test step passed" ) );
    
    
    CleanupStack::PopAndDestroy( fileManager );
    }

//------------------------------------------------------------------------------
// Test case: NativeInstallation(1)
//------------------------------------------------------------------------------
void MT_CHSPSConfigurationIf::NativeInstallation_1_L()
    {       
    // Pre conditions                           
    _LIT(KFileThemes, "c:\\private\\200159C0\\themes\\2456\\270513751\\536916275\\1.0\\InstallWidgetConf.o0000");       
    
    // Attach to HSPS 
    EUNIT_PRINT( _L8( "Pre conditions: Attach to HSPS service IConfiguration interface" ) );
    AttachServiceL( KHSPS, KHSPSConfigurationIf, KHSPSTestAppUid );
    
    MT_CHspsInstallationService* installationService = MT_CHspsInstallationService::NewL();    
    CleanupStack::PushL( installationService );
        
    // Test step 1: 
    EUNIT_PRINT( _L8( "Test step 1" ) );        
    installationService->InstallConfigurationL( KHSPSInstallInstalledWidgetConf );
    if ( !BaflUtils::FileExists( iFileserver, KFileThemes) )
        {
        User::Leave( KErrGeneral );
        }
    EUNIT_PRINT( _L8( "Test step passed" ) );    
    
    // Test step 2:
    EUNIT_PRINT( _L8( "Test step 2" ) );             
    installationService->UninstallConfigurationL( KHSPSMTInterfaceUid, KHSPSInstalledWidgetConf );
    if ( BaflUtils::FileExists( iFileserver, KFileThemes) )
        {
        User::Leave( KErrGeneral );
        }
    EUNIT_PRINT( _L8( "Test step passed" ) );    
    
    CleanupStack::PopAndDestroy( installationService );
    }

//------------------------------------------------------------------------------
// Test case: SetActivePlugin(1)
//------------------------------------------------------------------------------
void MT_CHSPSConfigurationIf::SetActivePlugin_1_L()
    {
    // Pre conditions
    // Set active configuration to Typical configuration
    EUNIT_PRINT( _L8( "Pre conditions: Set Active configuration Typical" ) );
    SetActiveConfigurationL( KHSPSTestAppUid, KHSPSActiveConfTypical );    
    // Attach to HSPS 
    EUNIT_PRINT( _L8( "Pre conditions: Attach to HSPS service IConfiguration interface" ) );
    AttachServiceL( KHSPS, KHSPSConfigurationIf, KHSPSTestAppUid );
    
    // Test step 1
    EUNIT_PRINT( _L8( "Test step 1" ) );
    RunTestStepSyncL(
        ( TUint8* )setactiveplugin_1_ts_1_method,
        ( TUint8* )setactiveplugin_1_ts_1_input,
        ( TUint8* )setactiveplugin_1_ts_1_output );
    EUNIT_PRINT( _L8( "Test step passed" ) );    
    
    // Test step 2
    EUNIT_PRINT( _L8( "Test step 2" ) );
    RunTestStepSyncL(
        ( TUint8* )setactiveplugin_1_ts_2_method,
        ( TUint8* )setactiveplugin_1_ts_2_input,
        ( TUint8* )setactiveplugin_1_ts_2_output );
    EUNIT_PRINT( _L8( "Test step passed" ) );
    }

//------------------------------------------------------------------------------
// Test case: SetActivePlugin(2)
//------------------------------------------------------------------------------
void MT_CHSPSConfigurationIf::SetActivePlugin_2_L()
    {
    // Pre conditions
    // Set active configuration to Typical configuration
    EUNIT_PRINT( _L8( "Pre conditions: Set Active configuration Typical" ) );
    SetActiveConfigurationL( KHSPSTestAppUid, KHSPSActiveConfTypical );    
    // Attach to HSPS 
    EUNIT_PRINT( _L8( "Pre conditions: Attach to HSPS service IConfiguration interface" ) );
    AttachServiceL( KHSPS, KHSPSConfigurationIf, KHSPSTestAppUid );
    
    // Test step 1
    EUNIT_PRINT( _L8( "Test step 1" ) );
    RunTestStepSyncL(
        ( TUint8* )setactiveplugin_2_ts_1_method,
        ( TUint8* )setactiveplugin_2_ts_1_input,
        ( TUint8* )setactiveplugin_2_ts_1_output );
    EUNIT_PRINT( _L8( "Test step passed" ) );    
    
    // Test step 2
    EUNIT_PRINT( _L8( "Test step 2" ) );
    RunTestStepSyncL(
        ( TUint8* )setactiveplugin_2_ts_2_method,
        ( TUint8* )setactiveplugin_2_ts_2_input,
        ( TUint8* )setactiveplugin_2_ts_2_output );
    EUNIT_PRINT( _L8( "Test step passed" ) );
    }

//------------------------------------------------------------------------------
// Test case: SetActivePlugin(3)
//------------------------------------------------------------------------------
void MT_CHSPSConfigurationIf::SetActivePlugin_3_L()
    {
    // Pre conditions
    // Set active configuration to Typical configuration
    EUNIT_PRINT( _L8( "Pre conditions: Set Active configuration Typical" ) );
    SetActiveConfigurationL( KHSPSTestAppUid, KHSPSActiveConfTypical );    
    // Attach to HSPS 
    EUNIT_PRINT( _L8( "Pre conditions: Attach to HSPS service IConfiguration interface" ) );
    AttachServiceL( KHSPS, KHSPSConfigurationIf, KHSPSTestAppUid );
    
    // Test step 1
    EUNIT_PRINT( _L8( "Test step 1" ) );
    RunTestStepSyncL(
        ( TUint8* )setactiveplugin_3_ts_1_method,
        ( TUint8* )setactiveplugin_3_ts_1_input,
        ( TUint8* )setactiveplugin_3_ts_1_output );
    EUNIT_PRINT( _L8( "Test step passed" ) );    
    
    // Test step 2
    EUNIT_PRINT( _L8( "Test step 2" ) );
    RunTestStepSyncL(
        ( TUint8* )setactiveplugin_3_ts_2_method,
        ( TUint8* )setactiveplugin_3_ts_2_input,
        ( TUint8* )setactiveplugin_3_ts_2_output );
    EUNIT_PRINT( _L8( "Test step passed" ) );
    }

//------------------------------------------------------------------------------
// Test case: SetActivePlugin(4)
//------------------------------------------------------------------------------
void MT_CHSPSConfigurationIf::SetActivePlugin_4_L()
    {
    // Pre conditions
    // Set active configuration to Typical configuration
    EUNIT_PRINT( _L8( "Pre conditions: Set Active configuration Typical" ) );
    SetActiveConfigurationL( KHSPSTestAppUid, KHSPSActiveConfTypical );    
    // Attach to HSPS 
    EUNIT_PRINT( _L8( "Pre conditions: Attach to HSPS service IConfiguration interface" ) );
    AttachServiceL( KHSPS, KHSPSConfigurationIf, KHSPSTestAppUid );
    
    // Test step 1
    EUNIT_PRINT( _L8( "Test step 1" ) );
    RunTestStepSyncL(
        ( TUint8* )setactiveplugin_4_ts_1_method,
        ( TUint8* )setactiveplugin_4_ts_1_input,
        ( TUint8* )setactiveplugin_4_ts_1_output );
    EUNIT_PRINT( _L8( "Test step passed" ) );    
    
    // Test step 2
    EUNIT_PRINT( _L8( "Test step 2" ) );
    RunTestStepSyncL(
        ( TUint8* )setactiveplugin_4_ts_2_method,
        ( TUint8* )setactiveplugin_4_ts_2_input,
        ( TUint8* )setactiveplugin_4_ts_2_output );
    EUNIT_PRINT( _L8( "Test step passed" ) );
    }

//------------------------------------------------------------------------------
// Test case: SetActivePlugin(5)
//------------------------------------------------------------------------------
void MT_CHSPSConfigurationIf::SetActivePlugin_5_L()
    {
    // Pre conditions
    // Set active configuration to Typical configuration
    EUNIT_PRINT( _L8( "Pre conditions: Set Active configuration Typical" ) );
    SetActiveConfigurationL( KHSPSTestAppUid, KHSPSActiveConfTypical );    
    // Attach to HSPS 
    EUNIT_PRINT( _L8( "Pre conditions: Attach to HSPS service IConfiguration interface" ) );
    AttachServiceL( KHSPS, KHSPSConfigurationIf, KHSPSTestAppUid );
    
    // Test step 1
    EUNIT_PRINT( _L8( "Test step 1" ) );
    RunTestStepSyncL(
        ( TUint8* )setactiveplugin_5_ts_1_method,
        ( TUint8* )setactiveplugin_5_ts_1_input,
        ( TUint8* )setactiveplugin_5_ts_1_output );
    EUNIT_PRINT( _L8( "Test step passed" ) );    
    }

//------------------------------------------------------------------------------
// Test case: SetActivePlugin(6)
//------------------------------------------------------------------------------
void MT_CHSPSConfigurationIf::SetActivePlugin_6_L()
    {
    // Pre conditions
    // Set active configuration to Typical configuration
    EUNIT_PRINT( _L8( "Pre conditions: Set Active configuration Typical" ) );
    SetActiveConfigurationL( KHSPSTestAppUid, KHSPSActiveConfTypical );    
    // Attach to HSPS 
    EUNIT_PRINT( _L8( "Pre conditions: Attach to HSPS service IConfiguration interface" ) );
    AttachServiceL( KHSPS, KHSPSConfigurationIf, KHSPSTestAppUid );
    
    // Test step 1
    EUNIT_PRINT( _L8( "Test step 1" ) );
    RunTestStepSyncL(
        ( TUint8* )setactiveplugin_6_ts_1_method,
        ( TUint8* )setactiveplugin_6_ts_1_input,
        ( TUint8* )setactiveplugin_6_ts_1_output );
    EUNIT_PRINT( _L8( "Test step passed" ) );    
    }

//------------------------------------------------------------------------------
// Test case table
//------------------------------------------------------------------------------
void MT_CHSPSConfigurationIf::RestoreConfigurations_1_L()
    {
    // Pre conditions
    // Set active configuration to Typical configuration
    EUNIT_PRINT( _L8( "Pre conditions: Set Active configuration Typical" ) );
    SetActiveConfigurationL( KHSPSTestAppUid, KHSPSActiveConfTypical );    
    // Attach to HSPS 
    EUNIT_PRINT( _L8( "Pre conditions: Attach to HSPS service IConfiguration interface" ) );
    AttachServiceL( KHSPS, KHSPSConfigurationIf, KHSPSTestAppUid );
    
    // Test step 1
    EUNIT_PRINT( _L8( "Test step 1" ) );
    RunTestStepSyncL(
        ( TUint8* )restoreconfigurations_1_ts_1_method,
        ( TUint8* )restoreconfigurations_1_ts_1_input,
        ( TUint8* )restoreconfigurations_1_ts_1_output );
    EUNIT_PRINT( _L8( "Test step passed" ) );
    
    // Test step 2
    EUNIT_PRINT( _L8( "Test step 2" ) );
    RunTestStepSyncL(
        ( TUint8* )restoreconfigurations_1_ts_2_method,
        ( TUint8* )restoreconfigurations_1_ts_2_input,
        ( TUint8* )restoreconfigurations_1_ts_2_output );
    EUNIT_PRINT( _L8( "Test step passed" ) );
    
    // Test step 3
    EUNIT_PRINT( _L8( "Test step 3" ) );
    RunTestStepSyncL(
        ( TUint8* )restoreconfigurations_1_ts_3_method,
        ( TUint8* )restoreconfigurations_1_ts_3_input,
        ( TUint8* )restoreconfigurations_1_ts_3_output );
    EUNIT_PRINT( _L8( "Test step passed" ) );
    
    // Test step 4
    EUNIT_PRINT( _L8( "Test step 4" ) );
    RunTestStepSyncL(
        ( TUint8* )restoreconfigurations_1_ts_4_method,
        ( TUint8* )restoreconfigurations_1_ts_4_input,
        ( TUint8* )restoreconfigurations_1_ts_4_output );
    EUNIT_PRINT( _L8( "Test step passed" ) );
    
    // Test step 5
    EUNIT_PRINT( _L8( "Test step 5" ) );
    RunTestStepSyncL(
        ( TUint8* )restoreconfigurations_1_ts_5_method,
        ( TUint8* )restoreconfigurations_1_ts_5_input,
        ( TUint8* )restoreconfigurations_1_ts_5_output );
    EUNIT_PRINT( _L8( "Test step passed" ) );
    
    // Test step 6
    EUNIT_PRINT( _L8( "Test step 6" ) );
    RunTestStepSyncL(
        ( TUint8* )restoreconfigurations_1_ts_6_method,
        ( TUint8* )restoreconfigurations_1_ts_6_input,
        ( TUint8* )restoreconfigurations_1_ts_6_output );
    EUNIT_PRINT( _L8( "Test step passed" ) );
    }

//------------------------------------------------------------------------------
// Test case: Customization(1)
//------------------------------------------------------------------------------
void MT_CHSPSConfigurationIf::Customization_1_L()
    {        
    // Pre conditions
        
    EUNIT_PRINT( _L8( "Pre conditions: Set Active configuration Minimal" ) );
    SetActiveConfigurationL( KHSPSTestAppUid, KHSPSActiveConfMinimal );    
        
    EUNIT_PRINT( _L8( "Pre conditions: Attach to HSPS service IConfiguration interface" ) );
    AttachServiceL( KHSPS, KHSPSConfigurationIf, KHSPSTestAppUid );
            
    // Simulate customization by copying configuration files to D drive ("ROM" stuff is on C)
    CFileMan* fileManager = CFileMan::NewL( iFileserver );
    CleanupStack::PushL( fileManager );
    User::LeaveIfError( 
        fileManager->Copy(
            _L( "c:\\data\\mt_hsps\\installed_widget\\widgetconfiguration_customized.xml" ),
            _L( "d:\\data\\mt_hsps\\installed_widget\\widgetconfiguration.xml" ),
            CFileMan::ERecurse|CFileMan::EOverWrite 
            )             
        );    
    CleanupStack::PopAndDestroy( fileManager );
                
    MT_CHspsInstallationService* installationService = MT_CHspsInstallationService::NewL();    
    CleanupStack::PushL( installationService );
        
    // Test step 1: install installed_widget which has customized content in D drive
    EUNIT_PRINT( _L8( "Test step 1" ) );        
    installationService->InstallConfigurationL( KHSPSInstallInstalledWidgetConf );
    EUNIT_PRINT( _L8( "Test step passed" ) );    
            
    // Test step 2: check settings from the installed_widget, it should hold
    // settings from the widgetconfiguration_customized.xml file    
    EUNIT_PRINT( _L8( "Test step 2" ) );             
    RunTestStepSyncL(
        ( TUint8* )customization_1_ts_2_method,
        ( TUint8* )customization_1_ts_2_input,
        ( TUint8* )customization_1_ts_2_output );             
    EUNIT_PRINT( _L8( "Test step passed" ) );
                
    CleanupStack::PopAndDestroy( installationService );    
    }

//------------------------------------------------------------------------------
// Test case: RestoreActiveAppConf(1)
//------------------------------------------------------------------------------
void MT_CHSPSConfigurationIf::RestoreActiveAppConf_1_L()
    {        
    // Pre conditions: activate configuration which hasn't got statuslicenceerestorable status        
    EUNIT_PRINT( _L8( "Pre conditions: Set Active configuration Minimal" ) );
    SetActiveConfigurationL( KHSPSTestAppUid, KHSPSActiveConfMinimal );    

    // Get ODT and fill in the plugin DOMs
    EUNIT_PRINT( _L8( "Pre conditions: Attach to HSPS service IConfiguration interface" ) );
    AttachServiceL( KHSPS, KHSPSConfigurationIf, KHSPSTestAppUid );
    
    // Test step 1: invalidate state of the active application configuration
    EUNIT_PRINT( _L8( "Test step 1" ) );    
    RunTestStepSyncL(                   
        ( TUint8* )restoreactiveappconf_1_ts_1_method,
        ( TUint8* )restoreactiveappconf_1_ts_1_input,
        ( TUint8* )restoreactiveappconf_1_ts_1_output );   
    EUNIT_PRINT( _L8( "Test step passed" ) );
        
    // Test step 2: retrieve the app conf, Operator configuration should be now active    
    EUNIT_PRINT( _L8( "Test step 2" ) );
    RunTestStepSyncL(                   
        ( TUint8* )restoreactiveappconf_1_ts_2_method,
        ( TUint8* )restoreactiveappconf_1_ts_2_input,
        ( TUint8* )restoreactiveappconf_1_ts_2_output );        
    EUNIT_PRINT( _L8( "Test step passed" ) );
    }


//------------------------------------------------------------------------------
// Test case: RestoreActiveAppConfL(2)
//------------------------------------------------------------------------------
void MT_CHSPSConfigurationIf::RestoreActiveAppConf_2_L()
    {        
    // Pre conditions: activate configuration with a statuslicenceerestorable status        
    EUNIT_PRINT( _L8( "Pre conditions: Set Active configuration Operator" ) );
    SetActiveConfigurationL( KHSPSTestAppUid, KHSPSActiveConfOperator );    

    // Get ODT and fill in the plugin DOMs
    EUNIT_PRINT( _L8( "Pre conditions: Attach to HSPS service IConfiguration interface" ) );
    AttachServiceL( KHSPS, KHSPSConfigurationIf, KHSPSTestAppUid );
    
    // Test step 1: invalidate state of the active application configuration
    EUNIT_PRINT( _L8( "Test step 1" ) );    
    RunTestStepSyncL(                   
        ( TUint8* )restoreactiveappconf_2_ts_1_method,
        ( TUint8* )restoreactiveappconf_2_ts_1_input,
        ( TUint8* )restoreactiveappconf_2_ts_1_output );   
    EUNIT_PRINT( _L8( "Test step passed" ) );
        
    // Test step 2: retrieve the app conf, we should fail installing the Operator 
    // configuration as it is not in ROM
    EUNIT_PRINT( _L8( "Test step 2" ) );
    RunTestStepSyncL(                   
        ( TUint8* )restoreactiveappconf_2_ts_2_method,
        ( TUint8* )restoreactiveappconf_2_ts_2_input,
        ( TUint8* )restoreactiveappconf_2_ts_2_output );        
    EUNIT_PRINT( _L8( "Test step passed" ) );
    }

//------------------------------------------------------------------------------
// Test case table
//------------------------------------------------------------------------------
EUNIT_BEGIN_TEST_TABLE(
    MT_CHSPSConfigurationIf,
    "This is a test suite for HSPS configuration interface.",
    "UNIT" )

    // Get configuration test group
    EUNIT_TEST(
        "GetActiveAppConf(1)",
        "IConfiguration",
        "GetActiveAppConf",
        "FUNCTIONALITY",
        SetupL, GetActiveAppConf_1_L, Teardown )

    EUNIT_TEST(
        "GetActiveAppConf(2)",
        "IConfiguration",
        "GetActiveAppConf",
        "FUNCTIONALITY",
        SetupL, GetActiveAppConf_2_L, Teardown )

    EUNIT_TEST(
        "GetActiveAppConf(3)",
        "IConfiguration",
        "GetActiveAppConf",
        "FUNCTIONALITY",
        SetupL, GetActiveAppConf_3_L, Teardown )

    EUNIT_TEST(
        "GetPluginConf(1)",
        "IConfiguration",
        "GetPluginConf",
        "FUNCTIONALITY",
        SetupL, GetPluginConf_1_L, Teardown )
        
    EUNIT_TEST(
        "GetPluginConf(2)",
        "IConfiguration",
        "GetPluginConf",
        "FUNCTIONALITY",
        SetupL, GetPluginConf_2_L, Teardown )

    EUNIT_TEST(
        "GetPluginConf(3)",
        "IConfiguration",
        "GetPluginConf",
        "FUNCTIONALITY",
        SetupL, GetPluginConf_3_L, Teardown )        

    EUNIT_TEST(
        "GetPluginConf(4)",
        "IConfiguration",
        "GetPluginConf",
        "FUNCTIONALITY",
        SetupL, GetPluginConf_4_L, Teardown )        

    EUNIT_TEST(
        "GetPluginConf(5)",
        "IConfiguration",
        "GetPluginConf",
        "FUNCTIONALITY",
        SetupL, GetPluginConf_5_L, Teardown )        

    // Modify Configuration test group
    EUNIT_TEST(
        "GetPlugins(1)",
        "IConfiguration",
        "GetPlugins",
        "FUNCTIONALITY",
        SetupL, GetPlugins_1_L, Teardown )
        
    EUNIT_TEST(
        "GetPlugins(2)",
        "IConfiguration",
        "GetPlugins",
        "FUNCTIONALITY",
        SetupL, GetPlugins_2_L, Teardown )
        
    EUNIT_TEST(
        "GetPlugins(3)",
        "IConfiguration",
        "GetPlugins",
        "FUNCTIONALITY",
        SetupL, GetPlugins_3_L, Teardown )
        
    EUNIT_TEST(
        "GetPlugins(4)",
        "IConfiguration",
        "GetPlugins",
        "FUNCTIONALITY",
        SetupL, GetPlugins_4_L, Teardown )
        
    EUNIT_TEST(
        "GetPlugins(5)",
        "IConfiguration",
        "GetPlugins",
        "FUNCTIONALITY",
        SetupL, GetPlugins_5_L, Teardown )

    EUNIT_TEST(
        "GetPlugins(6)",
        "IConfiguration",
        "GetPlugins",
        "FUNCTIONALITY",
        SetupL, GetPlugins_6_L, Teardown )
    
    EUNIT_TEST(
        "GetPlugins(7)",
        "IConfiguration",
        "GetPlugins",
        "FUNCTIONALITY",
        SetupL, GetPlugins_7_L, Teardown )

    EUNIT_TEST(
        "GetPluginList(1)",
        "IConfiguration",
        "GetPluginList",
        "FUNCTIONALITY",
        SetupL, GetPluginList_1_L, Teardown )

    EUNIT_TEST(
        "GetPluginList(2)",
        "IConfiguration",
        "GetPluginList",
        "FUNCTIONALITY",
        SetupL, GetPluginList_2_L, Teardown )

    EUNIT_TEST(
        "GetPluginList(3)",
        "IConfiguration",
        "GetPluginList",
        "FUNCTIONALITY",
        SetupL, GetPluginList_3_L, Teardown )

    EUNIT_TEST(
        "GetPluginList(4)",
        "IConfiguration",
        "GetPluginList",
        "FUNCTIONALITY",
        SetupL, GetPluginList_4_L, Teardown )

    EUNIT_TEST(
        "GetPluginList(5)",
        "IConfiguration",
        "GetPluginList",
        "FUNCTIONALITY",
        SetupL, GetPluginList_5_L, Teardown )

    EUNIT_TEST(
        "GetPluginList(6)",
        "IConfiguration",
        "GetPluginList",
        "FUNCTIONALITY",
        SetupL, GetPluginList_6_L, Teardown )

    EUNIT_TEST(
        "GetPluginList(7)",
        "IConfiguration",
        "GetPluginList",
        "FUNCTIONALITY",
        SetupL, GetPluginList_7_L, Teardown )

    EUNIT_TEST(
        "GetPluginList(8)",
        "IConfiguration",
        "GetPluginList",
        "FUNCTIONALITY",
        SetupL, GetPluginList_8_L, Teardown )

    EUNIT_TEST(
        "AddPlugin(1)",
        "IConfiguration",
        "AddPlugin",
        "FUNCTIONALITY",
        SetupL, AddPlugin_1_L, Teardown )

    EUNIT_TEST(
        "AddPlugin(2)",
        "IConfiguration",
        "AddPlugin",
        "FUNCTIONALITY",
        SetupL, AddPlugin_2_L, Teardown )

    EUNIT_TEST(
        "AddPlugin(3)",
        "IConfiguration",
        "AddPlugin",
        "FUNCTIONALITY",
        SetupL, AddPlugin_3_L, Teardown )

    EUNIT_TEST(
        "AddPlugin(4)",
        "IConfiguration",
        "AddPlugin",
        "FUNCTIONALITY",
        SetupL, AddPlugin_4_L, Teardown )

    EUNIT_TEST(
        "AddPlugin(5)",
        "IConfiguration",
        "AddPlugin",
        "FUNCTIONALITY",
        SetupL, AddPlugin_5_L, Teardown )

    EUNIT_TEST(
        "AddPlugin(6)",
        "IConfiguration",
        "AddPlugin",
        "FUNCTIONALITY",
        SetupL, AddPlugin_6_L, Teardown )

    EUNIT_TEST(
        "AddPlugin(7)",
        "IConfiguration",
        "AddPlugin",
        "FUNCTIONALITY",
        SetupL, AddPlugin_7_L, Teardown )

    EUNIT_TEST(
        "AddPlugin(8)",
        "IConfiguration",
        "AddPlugin",
        "FUNCTIONALITY",
        SetupL, AddPlugin_8_L, Teardown )

    EUNIT_TEST(
        "AddPlugin(9)",
        "IConfiguration",
        "AddPlugin",
        "FUNCTIONALITY",
        SetupL, AddPlugin_9_L, Teardown )

    EUNIT_TEST(
        "AddPlugin(10)",
        "IConfiguration",
        "AddPlugin",
        "FUNCTIONALITY",
        SetupL, AddPlugin_10_L, Teardown )

    EUNIT_TEST(
        "AddPlugin(11)",
        "IConfiguration",
        "AddPlugin",
        "FUNCTIONALITY",
        SetupL, AddPlugin_11_L, Teardown )

    EUNIT_TEST(
        "RemovePlugin(1)",
        "IConfiguration",
        "RemovePlugin",
        "FUNCTIONALITY",
        SetupL, RemovePlugin_1_L, Teardown )
        
    EUNIT_TEST(
        "RemovePlugin(2)",
        "IConfiguration",
        "RemovePlugin",
        "FUNCTIONALITY",
        SetupL, RemovePlugin_2_L, Teardown )

    EUNIT_TEST(
        "RemovePlugin(3)",
        "IConfiguration",
        "RemovePlugin",
        "FUNCTIONALITY",
        SetupL, RemovePlugin_3_L, Teardown )

    EUNIT_TEST(
        "RemovePlugin(4)",
        "IConfiguration",
        "RemovePlugin",
        "FUNCTIONALITY",
        SetupL, RemovePlugin_4_L, Teardown )

   EUNIT_TEST(
        "SetPluginSettings(1)",
        "IConfiguration",
        "SetPluginSettings",
        "FUNCTIONALITY",
        SetupL, SetPluginSettings_1_L, Teardown )
    
    EUNIT_TEST(
       "SetPluginSettings(2)",
       "IConfiguration",
       "SetPluginSettings",
       "FUNCTIONALITY",
       SetupL, SetPluginSettings_2_L, Teardown )

    EUNIT_TEST(
       "SetPluginSettings(3)",
       "IConfiguration",
       "SetPluginSettings",
       "FUNCTIONALITY",
       SetupL, SetPluginSettings_3_L, Teardown )
    
    EUNIT_TEST(
       "SetPluginSettings(4)",
       "IConfiguration",
       "SetPluginSettings",
       "FUNCTIONALITY",
       SetupL, SetPluginSettings_4_L, Teardown )

    EUNIT_TEST(
       "SetPluginSettings(5)",
       "IConfiguration",
       "SetPluginSettings",
       "FUNCTIONALITY",
       SetupL, SetPluginSettings_5_L, Teardown )
    
    EUNIT_TEST(
       "SetPluginSettings(6)",
       "IConfiguration",
       "SetPluginSettings",
       "FUNCTIONALITY",
       SetupL, SetPluginSettings_6_L, Teardown )

    EUNIT_TEST(
	   "GetPluginSettings(1)",
	   "IConfiguration",
	   "GetPluginSettings",
	   "FUNCTIONALITY",
	   SetupL, GetPluginSettings_1_L, Teardown )

    EUNIT_TEST(
       "GetPluginSettings(2)",
       "IConfiguration",
       "GetPluginSettings",
       "FUNCTIONALITY",
       SetupL, GetPluginSettings_2_L, Teardown )

    EUNIT_TEST(
       "GetPluginSettings(3)",
       "IConfiguration",
       "GetPluginSettings",
       "FUNCTIONALITY",
       SetupL, GetPluginSettings_3_L, Teardown )

    EUNIT_TEST(
       "MovePlugins(1)",
       "IConfiguration",
       "MovePlugins",
       "FUNCTIONALITY",
       SetupL, MovePlugins_1_L, Teardown )

    EUNIT_TEST(
       "MovePlugins(2)",
       "IConfiguration",
       "MovePlugins",
       "FUNCTIONALITY",
       SetupL, MovePlugins_2_L, Teardown )

    EUNIT_TEST(
       "MovePlugins(3)",
       "IConfiguration",
       "MovePlugins",
       "FUNCTIONALITY",
       SetupL, MovePlugins_3_L, Teardown )

    EUNIT_TEST(
       "MovePlugins(4)",
       "IConfiguration",
       "MovePlugins",
       "FUNCTIONALITY",
       SetupL, MovePlugins_4_L, Teardown )

    EUNIT_TEST(
       "MovePlugins(5)",
       "IConfiguration",
       "MovePlugins",
       "FUNCTIONALITY",
       SetupL, MovePlugins_5_L, Teardown )

    EUNIT_TEST(
       "MovePlugins(6)",
       "IConfiguration",
       "MovePlugins",
       "FUNCTIONALITY",
       SetupL, MovePlugins_6_L, Teardown )
       
    EUNIT_TEST(   
       "ReplacePlugin(1)",
       "IConfiguration",
       "ReplacePlugin",
       "FUNCTIONALITY",
       SetupL, ReplacePlugin_1_L, Teardown )     
       
    EUNIT_TEST(
       "GetAppConfs(1)",
       "IConfiguration",
       "GetAppConfs",
       "FUNCTIONALITY",
       SetupL, GetAppConfs_1_L, Teardown )

    EUNIT_TEST(
       "SetActiveAppConf(1)",
       "IConfiguration",
       "SetActiveAppConf",
       "FUNCTIONALITY",
       SetupL, SetActiveAppConf_1_L, Teardown )

    EUNIT_TEST(
       "SetActiveAppConf(2)",
       "IConfiguration",
       "SetActiveAppConf",
       "FUNCTIONALITY",
       SetupL, SetActiveAppConf_2_L, Teardown )
       
    EUNIT_TEST(
       "SetActiveAppConf(3)",
       "IConfiguration",
       "SetActiveAppConf",
       "FUNCTIONALITY",
       SetupL, SetActiveAppConf_3_L, Teardown )
       
    EUNIT_TEST(
       "SetActiveAppConf(4)",
       "IConfiguration",
       "SetActiveAppConf",
       "FUNCTIONALITY",
       SetupL, SetActiveAppConf_4_L, Teardown )

    EUNIT_TEST(
       "RequestNotify(1)",
       "IConfiguration",
       "RequestNotify",
       "FUNCTIONALITY",
       SetupL, RequestNotify_1_L, Teardown )
       
    EUNIT_TEST(
       "RequestNotify(2)",
       "IConfiguration",
       "RequestNotify",
       "FUNCTIONALITY",
       SetupL, RequestNotify_2_L, Teardown )
       
    EUNIT_TEST(
       "RequestNotify(3)",
       "IConfiguration",
       "RequestNotify",
       "FUNCTIONALITY",
       SetupL, RequestNotify_3_L, Teardown )

    EUNIT_TEST(
       "RequestNotify(4)",
       "IConfiguration",
       "RequestNotify",
       "FUNCTIONALITY",
       SetupL, RequestNotify_4_L, Teardown )

    EUNIT_TEST(
       "RequestNotify(5)",
       "IConfiguration",
       "RequestNotify",
       "FUNCTIONALITY",
       SetupL, RequestNotify_5_L, Teardown )
       
   EUNIT_TEST(
       "RequestNotify(6)",
       "IConfiguration",
       "RequestNotify",
       "FUNCTIONALITY",
       SetupL, RequestNotify_6_L, Teardown )

	EUNIT_TEST(
       "SisxInstallation(1)",
       "IConfiguration",
       "SisxInstallation",
       "FUNCTIONALITY",
       SetupL, SisxInstallation_1_L, Teardown )       
       
    EUNIT_TEST(   
       "NativeInstallation(1)",
       "IConfiguration",
       "NativeInstallation",
       "FUNCTIONALITY",
       SetupL, NativeInstallation_1_L, Teardown )

    EUNIT_TEST(   
       "SetActivePlugin(1)",
       "IConfiguration",
       "SetActivePlugin",
       "FUNCTIONALITY",
       SetupL, SetActivePlugin_1_L, Teardown )

    EUNIT_TEST(   
       "SetActivePlugin(2)",
       "IConfiguration",
       "SetActivePlugin",
       "FUNCTIONALITY",
       SetupL, SetActivePlugin_2_L, Teardown )

    EUNIT_TEST(   
       "SetActivePlugin(3)",
       "IConfiguration",
       "SetActivePlugin",
       "FUNCTIONALITY",
       SetupL, SetActivePlugin_3_L, Teardown )

    EUNIT_TEST(   
       "SetActivePlugin(4)",
       "IConfiguration",
       "SetActivePlugin",
       "FUNCTIONALITY",
       SetupL, SetActivePlugin_4_L, Teardown )

    EUNIT_TEST(   
       "SetActivePlugin(5)",
       "IConfiguration",
       "SetActivePlugin",
       "FUNCTIONALITY",
       SetupL, SetActivePlugin_5_L, Teardown )

    EUNIT_TEST(   
       "SetActivePlugin(6)",
       "IConfiguration",
       "SetActivePlugin",
       "FUNCTIONALITY",
       SetupL, SetActivePlugin_6_L, Teardown )

    EUNIT_TEST(   
       "RestoreConfigurations(1)",
       "IConfiguration",
       "SetActivePlugin",
       "FUNCTIONALITY",
       SetupL, RestoreConfigurations_1_L, Teardown )

    EUNIT_TEST(   
       "Customization(1)",
       "IConfiguration",
       "SetActivePlugin",
       "FUNCTIONALITY",
       SetupL, Customization_1_L, Teardown )

    EUNIT_TEST(   
       "RestoreActiveAppConfL(1)",
       "IConfiguration",
       "SetActivePlugin",
       "FUNCTIONALITY",
       SetupL, RestoreActiveAppConf_1_L, Teardown )

    EUNIT_TEST(   
       "RestoreActiveAppConfL(2)",
       "IConfiguration",
       "SetActivePlugin",
       "FUNCTIONALITY",
       SetupL, RestoreActiveAppConf_2_L, Teardown )
       
    EUNIT_END_TEST_TABLE

//  END OF FILE
