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
* Description:  Implementation of MT_CHSPS class
*
*/


#include <e32base.h>
#include <e32cmn.h> 
#include <bautils.h>
#include <centralrepository.h>
#include <hspsthememanagement.h>
#include <hspsclient.h>
#include <eunitmacros.h>
#include <eunitdecorators.h>

#include "mt_hsps.h"
#include "mt_hspsteststep.h"
#include "hspsserviceutilities.h"
#include "hspspersonalisationservice.h"
#include "mt_hspsinstallationservice.h"

// ======== LOCAL CONSTANTS ====================================================
// LIW variant names
_LIT8( KHspsAppUid, "appUid" );

// Heap size for test step thread
const TUint KDefaultHeapSize = 0x100000;       

// Test step data
typedef struct
    {
    TUint8* iMethod;
    TUint8* iInParams;
    TUint8* iExpOutParams;
    TInt iTriggerCase;
    TBool iAsync;
    MT_CHSPS* iBaseObject;
    }TTestStepDataStr;

// ======== LOCAL FUNCTIONS ====================================================

// ---------------------------------------------------------------------------
// Calls asynchronous service method
// ---------------------------------------------------------------------------
//
void HSPSCallServiceL( TAny* aPointer )
    {
    TTestStepDataStr* testStepData = (TTestStepDataStr*)aPointer;
                
    // Create test step
    MT_CHSPSTeststep* ts = MT_CHSPSTeststep::NewL( 
                                        *(testStepData->iBaseObject->Interface()),
                                        testStepData->iMethod,
                                        testStepData->iInParams,
                                        testStepData->iExpOutParams,
                                        testStepData->iAsync );
    CleanupStack::PushL( ts );
    
    // Call service method
    ts->CallServiceL();

    // Start wait loop for test step execution
    CActiveScheduler::Start();

    // Check test step result
    User::LeaveIfError( ts->CheckOutParams() ); 

    // Clean test step.
    CleanupStack::PopAndDestroy( ts );
    }

// ---------------------------------------------------------------------------
// Test step thread
// ---------------------------------------------------------------------------
//
TInt HSPSTestStepThread( TAny* aPointer )
    {
    TTestStepDataStr* testStepData = (TTestStepDataStr*)aPointer;
    
    // Create cleanup stack for the test step thread
    CTrapCleanup* theCleanupStack = CTrapCleanup::New();

    // Create active scheduler. Do not start yet.
    CActiveScheduler* scheduler = new( ELeave ) CActiveScheduler;
    CActiveScheduler::Install( scheduler );    
        
    // Attach to service.
    TInt err = KErrNone;
    TRAP( err, 
        testStepData->iBaseObject->AttachServiceL( KHSPS,
                                                   KHSPSConfigurationIf,
                                                   KHSPSTestAppUid,
                                                   EFalse ); ); // No timing since it crashed
                                                                // (we are in separate thread than EUnit now)
    
    if( err == KErrNone )
        {
        // Call asychronous service method
        TRAP( err, HSPSCallServiceL( aPointer ) );

        // Detach
        TRAP_IGNORE( testStepData->iBaseObject->DetachServiceL() );        
        }
    
    // Remove active scheduler.
    CActiveScheduler::Install( NULL );
    delete scheduler;
    scheduler = NULL;
    
    // Clean clean up stack.
    delete theCleanupStack;  
    
    // Signal EUnit thread that asynchronous service request is completed
    RThread::Rendezvous( err );
    
    // And finally exit.
    return err;      
    }

// ======== MEMBER FUNCTIONS ===================================================


// -----------------------------------------------------------------------------
// Constructor
// -----------------------------------------------------------------------------
//
MT_CHSPSResource::MT_CHSPSResource( 
        const TDesC& aPath, 
        TInt aExpectedSize )
    {
    iPath.Copy( aPath );
    iSize = aExpectedSize;
    }

// -----------------------------------------------------------------------------
// Destructor.
// -----------------------------------------------------------------------------
//
MT_CHSPSResource::~MT_CHSPSResource()
    {    
    }


// -----------------------------------------------------------------------------
// Constructor
// -----------------------------------------------------------------------------
//
MT_CHSPS::MT_CHSPS()
    {
    }

// -----------------------------------------------------------------------------
// Destructor.
// -----------------------------------------------------------------------------
//    
MT_CHSPS::~MT_CHSPS()
    {
    if ( iInterface )
    	{
        iInterface->Close();
        }
    if ( iServiceHandler )
    	{
        delete iServiceHandler;
        }
    if ( iService )
    	{
        delete iService;
        }
    iResourceArray.ResetAndDestroy();
    }

// -----------------------------------------------------------------------------
//  Attach to defined service providers interface
// -----------------------------------------------------------------------------
//
void MT_CHSPS::AttachServiceL( 
    const TDesC8& aSapi,
    const TDesC8& aInterface,
    const TDesC8& aAppUid,
    const TBool aUseTiming )
    {
    iServiceHandler = CLiwServiceHandler::NewL();

    iService = CLiwCriteriaItem::NewL( 1, aInterface, aSapi );
    iService->SetServiceClass( TUid::Uid( KLiwClassBase ) );
    
    RCriteriaArray interestList;
    interestList.AppendL( iService );
    iServiceHandler->AttachL( interestList );
    interestList.Reset();

    CLiwGenericParamList* inParamList = &( iServiceHandler->InParamListL() );
    CLiwGenericParamList* outParamList = &( iServiceHandler->OutParamListL() );

    TLiwGenericParam appUid;
    TLiwVariant uidVar;
    uidVar.Set( aAppUid );
    appUid.SetNameAndValueL( KHspsAppUid, uidVar );
    inParamList->AppendL( appUid );
    appUid.Reset();
    
    if( aUseTiming )
        {
        TPtrC8 method;
        method.Set( _L8( "Attach" ) );
        StartTiming( method );
        }

    iServiceHandler->ExecuteServiceCmdL( 
        *iService, 
        *inParamList, 
        *outParamList );

    if( aUseTiming )
        {
        StopTiming();
        }
    
    TInt pos = 0;
    outParamList->FindFirst( pos, aInterface );
    if( pos != KErrNotFound )
        {
        iInterface = ( *outParamList )[ pos ].Value().AsInterface();    
        inParamList->Reset();
        outParamList->Reset();        
        }
    else
        {
        inParamList->Reset();
        outParamList->Reset();        
        User::Leave( KErrNotFound );
        }
}

// -----------------------------------------------------------------------------
//  Detach from attached interface
// -----------------------------------------------------------------------------
//
void MT_CHSPS::DetachServiceL(  )
    {
    if ( iInterface )
    	{
        iInterface->Close();
        iInterface = NULL;
    	}
    if ( iService && iServiceHandler )
        {
        RCriteriaArray interestList;
        interestList.AppendL( iService );
        iServiceHandler->DetachL( interestList );   
        interestList.Reset();
        }
    if ( iServiceHandler )
        {
        delete iServiceHandler;
        iServiceHandler = NULL;
        }
    if ( iService )
        {
        delete iService;
        iService = NULL;
        }
    }

// -----------------------------------------------------------------------------
// Runs synchronous test step
// -----------------------------------------------------------------------------
//    
void MT_CHSPS::RunTestStepSyncL(
    TUint8* aMethod,
    TUint8* aInParams,
    TUint8* aExpOutParams,
    TBool aMeasureExecTime )
    {
    // AttachServiceL not done?
    ASSERT ( iInterface );    
    
    // Create test step
    MT_CHSPSTeststep* ts = MT_CHSPSTeststep::NewL( 
        *iInterface,
        aMethod,
        aInParams,
        aExpOutParams,
        EFalse,
        aMeasureExecTime );
    CleanupStack::PushL( ts );
    
    // Call synchronous service method
    ts->CallServiceL();
    
    User::LeaveIfError( ts->CheckOutParams() );
    
    CleanupStack::PopAndDestroy( ts );
    }
// -----------------------------------------------------------------------------
// Runs request notification triggerer
// -----------------------------------------------------------------------------
//    
void MT_CHSPS::RunNotificationTriggerL( TInt aTriggerCase )
    {
    switch( MT_THSPSTrigger(aTriggerCase) )
        {
        case EHspsTriggerRunRemovePluginCase:
            {
            RunRemovePluginCase();
            }
            break;
        case EHspsTriggerRunAddPluginCase:
            {
            RunAddPluginCase();
            }
            break;
        case EHspsTriggerRunInstallationCase:
            {
            RunInstallationCase();
            }
            break;
        case EHspsTriggerRunSetActivePluginCase:
            {
            RunSetActivePluginCase();
            }
            break;
        case EHspsTriggerRunReplacePluginCase:
            {
            RunReplacePluginCase();
            }
            break;
        case EHspsTriggerRunUpdateInstallationCase:
            {
            RunUpdateInstallationCase();
            }
            break;
        case EHspsTriggerRunUninstallationCase:    
            {
            RunUninstallationCase();
            }     
            break;
        default:
            break;
        }
    }
// -----------------------------------------------------------------------------
// Runs Remove Plugin for request notification tests
// -----------------------------------------------------------------------------
// 
void MT_CHSPS::RunRemovePluginCase()
    {
    _LIT8(KPluginId, "8");
     TBuf8<8> removedPluginId(KPluginId);
    CHspsPersonalisationService* hspsPersonalisationService = CHspsPersonalisationService::NewL();    
    CleanupStack::PushL( hspsPersonalisationService );
    hspsPersonalisationService->RemovePluginL(536874929, removedPluginId);
    CleanupStack::PopAndDestroy( hspsPersonalisationService );
    }
// -----------------------------------------------------------------------------
// Runs Remove Plugin for request notification tests
// -----------------------------------------------------------------------------
// 
void MT_CHSPS::RunAddPluginCase()
    {
    _LIT8(KViewConfId, "1");
    _LIT8(KPluginUid, "0x2000b120");
    _LIT8(KPluginposition, "-1");
    TBuf8<10> viewConfId(KViewConfId);
    TBuf8<10> addedPluginUid(KPluginUid);
    TBuf8<10> addedPluginposition(KPluginposition);
    CHspsPersonalisationService* hspsPersonalisationService = CHspsPersonalisationService::NewL();    
    CleanupStack::PushL( hspsPersonalisationService );
    
  
    TInt pluginId;    
    hspsPersonalisationService->AddPluginL(536874929, viewConfId, addedPluginUid, addedPluginposition, pluginId );
    iAddedPluginId = pluginId;
    CleanupStack::PopAndDestroy( hspsPersonalisationService );

    }

// -----------------------------------------------------------------------------
// Installs a plugin
// -----------------------------------------------------------------------------
// 
void MT_CHSPS::RunInstallationCase()
    {
    // Triggers a "PluginInstalled" notification to clients when finnished
    MT_CHspsInstallationService* hspsInstallationService = MT_CHspsInstallationService::NewL();    
    CleanupStack::PushL( hspsInstallationService );
    hspsInstallationService->InstallConfigurationL( KHSPSInstallInstalledWidgetConf );
    CleanupStack::PopAndDestroy( hspsInstallationService );  
    }

// -----------------------------------------------------------------------------
// Installs an update for the plugin
// -----------------------------------------------------------------------------
//
void MT_CHSPS::RunUpdateInstallationCase()
    {    
    // Triggers a "PluginUpdated" notification to clients when finnished
    MT_CHspsInstallationService* hspsInstallationService = MT_CHspsInstallationService::NewL();    
    CleanupStack::PushL( hspsInstallationService );
    hspsInstallationService->InstallConfigurationL( KHSPSInstallInstalledWidgetConfV2 );
    CleanupStack::PopAndDestroy( hspsInstallationService );
    }

// -----------------------------------------------------------------------------
// Uninstalls a plugin
// -----------------------------------------------------------------------------
//
void MT_CHSPS::RunUninstallationCase()
    {
    // Triggers a "PluginUninstalled" notification to clients when finnished
    MT_CHspsInstallationService* hspsInstallationService = MT_CHspsInstallationService::NewL();    
    CleanupStack::PushL( hspsInstallationService );
    hspsInstallationService->UninstallConfigurationL( 
            KHSPSMTInterfaceUid, 
            (TInt)KHSPSInstalledWidgetConf);
    CleanupStack::PopAndDestroy( hspsInstallationService );
    }

void MT_CHSPS::RestoreTestDataL( TInt aTriggerCase )
    {
    switch(aTriggerCase)
            {
            case 1:
                {
                RestoreRemovedPluginCase();
                }
                break;
            case 2:
                {
                RestoreAddedPluginCase();
                }
                break;
            default:
                break;
            }
    }

// -----------------------------------------------------------------------------
// Runs Remove Plugin for request notification tests
// -----------------------------------------------------------------------------
// 
void MT_CHSPS::RestoreRemovedPluginCase()
    {
    _LIT8(KViewConfId, "6");
    _LIT8(KPluginUid, "0x2000b120");
    _LIT8(KPluginposition, "-1");
    TBuf8<10> viewConfId(KViewConfId);
    TBuf8<10> addedPluginUid(KPluginUid);
    TBuf8<10> addedPluginposition(KPluginposition);
    CHspsPersonalisationService* hspsPersonalisationService = CHspsPersonalisationService::NewL();    
    CleanupStack::PushL( hspsPersonalisationService );
    
    TInt pluginId;    
    hspsPersonalisationService->AddPluginL(536874929, viewConfId, addedPluginUid, addedPluginposition, pluginId );
    
    CleanupStack::PopAndDestroy( hspsPersonalisationService );

    }
// -----------------------------------------------------------------------------
// Runs Remove Plugin for request notification tests
// -----------------------------------------------------------------------------
// 
void MT_CHSPS::RestoreAddedPluginCase()
    {
    //_LIT8(KPluginId, "9");
    //iAddedPluginId
    TBuf8<10> removedPluginId;
    removedPluginId.AppendNum(iAddedPluginId);
    CHspsPersonalisationService* hspsPersonalisationService = CHspsPersonalisationService::NewL();    
    CleanupStack::PushL( hspsPersonalisationService );
    hspsPersonalisationService->RemovePluginL(536874929, removedPluginId);
    CleanupStack::PopAndDestroy( hspsPersonalisationService );

    }

// -----------------------------------------------------------------------------
// Runs Remove Plugin for request notification tests
// -----------------------------------------------------------------------------
// 
void MT_CHSPS::RunSetActivePluginCase()
    {
    _LIT8(KPluginId, "5");
    TBuf8<10> pluginId( KPluginId );
    CHspsPersonalisationService* hspsPersonalisationService = CHspsPersonalisationService::NewL();    
    CleanupStack::PushL( hspsPersonalisationService );

    hspsPersonalisationService->SetActivePluginL( 536874929, pluginId );
    CleanupStack::PopAndDestroy( hspsPersonalisationService );

    }

// -----------------------------------------------------------------------------
// Runs Replace Plugin for request notification tests
// -----------------------------------------------------------------------------
// 
void MT_CHSPS::RunReplacePluginCase()
    {
    _LIT8(KPluginId, "1");
    _LIT8(KPluginUid, "0x2000b102");
    TBuf8<10> replacePluginUid( KPluginUid );
    TBuf8<1> replacePluginId( KPluginId );
    CHspsPersonalisationService* hspsPersonalisationService = CHspsPersonalisationService::NewL();    
    CleanupStack::PushL( hspsPersonalisationService );
    
    hspsPersonalisationService->ReplacePluginL( 536874929, replacePluginId, replacePluginUid );
    
    CleanupStack::PopAndDestroy( hspsPersonalisationService );

    }

// -----------------------------------------------------------------------------
// Accessor to iInterface
// -----------------------------------------------------------------------------
//    
MLiwInterface* MT_CHSPS::Interface()
    {
    return iInterface;
    }

// -----------------------------------------------------------------------------
// Runs synchronous test step
// -----------------------------------------------------------------------------
//    
void MT_CHSPS::RunTestStepAsyncL(
    TUint8* aMethod,
    TUint8* aInParams,
    TUint8* aExpOutParams,
    TInt aTriggerCase)
    {

    // Create test step data
    TTestStepDataStr data;
    data.iMethod = aMethod;
    data.iInParams = aInParams;
    data.iExpOutParams = aExpOutParams;
    data.iTriggerCase = aTriggerCase;
    data.iAsync = ETrue;
    data.iBaseObject = this;
    
    // Create thread for the asynchronous test step
    RThread testThread;
    TInt err = testThread.Create( 
        _L( "TestStep" ),
        HSPSTestStepThread,
        0xA000, // 40kB
        KDefaultHeapSize * 2, // 2 times of base size
        KDefaultHeapSize * 8, // 8 times of base size - needed by Eunit
        ( TAny* )&data,
        EOwnerProcess );
        
    if ( err == KErrNone )
        {
        // This must be done before resume is called to get correct error code.
        TRequestStatus status;
        
        // Request signal from test step thread when test step is completed
        testThread.Rendezvous( status );        

        // Start test step thread
        testThread.Resume();        
        
        // Give test thread some time to initialize so that triggers are not ran before
        // test thread is connected to server.
        User::After( 2000000 );
        
        // change application configuration
        TRAP( err, RunNotificationTriggerL(aTriggerCase) );
        if( err != KErrNone )
            {
            // Shutdown thread and close handle. Then leave and
            // do not wait for event that is never triggered.
            if( testThread.ExitReason() == EExitPending ) // Check that process is alive.
                {
                testThread.Kill( KErrUnknown );
                }
            
            testThread.Close();
            User::Leave( err );
            }

        // Wait test step to be completed
        User::WaitForRequest( status );
        err = status.Int();
        
        // Give test thread some time to close
        User::After( 2000000 );
        
        // Terminate thread.
        testThread.Kill( KErrNone );
        testThread.Close();
        }
    
    User::LeaveIfError( err );
    
    }

// -----------------------------------------------------------------------------
// Set active configuration
// -----------------------------------------------------------------------------
//    
void MT_CHSPS::SetActiveConfigurationL(
    const TDesC8& aAppUid,
    TInt aConfId )
    {

    TInt appUid = HspsServiceUtilities::DecString2Int( aAppUid );
        
    // Set application configuration active
    SetCentralRepositoryKeyL( appUid, aConfId );
        
    }

void MT_CHSPS::AddResourceL(     
    const TDesC& aFileName,
    const TInt aExpectedSize )
    {    
    iResourceArray.AppendL( new MT_CHSPSResource(aFileName, aExpectedSize) );    
    }

void MT_CHSPS::ResetResources()
    {
    iResourceArray.ResetAndDestroy();
    }

// -----------------------------------------------------------------------------
// Check if all given files exist
// -----------------------------------------------------------------------------
//    
void MT_CHSPS::CheckResourcesL()
    {        
    RFs rFs;
    rFs.Connect();
    CleanupClosePushL( rFs );
    
    RFile file;        
    CleanupClosePushL( file );
    
    TInt err = KErrNone;
    TInt actualSize = 0;     
    for( TInt i = 0; i < iResourceArray.Count() && !err; i++ )
        {
        MT_CHSPSResource* r = iResourceArray[ i ];
        
        if ( !r || !r->iPath.Length() )
            {
            err = KErrArgument;                        
            }                
        if ( !err )
            {
            err = file.Open( rFs, r->iPath, EFileRead );
            }
        if ( !err )
            {
            file.Size( actualSize );
            file.Close();               
            if ( r->iSize >= 0 && actualSize != r->iSize )
                {
                err = KErrCorrupt;
                }
            }
        
        if ( err )
            {
            EUNIT_PRINT( r->iPath );
            User::Leave( err );
            }
        }        
        
    CleanupStack::PopAndDestroy(2, &rFs); // file, rFs        
    
    ResetResources();
    }

// -----------------------------------------------------------------------------
// Starts timing of SAPI method execution
// -----------------------------------------------------------------------------
//    
void MT_CHSPS::StartTiming( 
    TDesC8& aMethod )
    {
    iStartTime.HomeTime();
    EUNIT_PRINT( aMethod );
    }

// -----------------------------------------------------------------------------
// Calculates SAPI method execution time
// -----------------------------------------------------------------------------
//    
void MT_CHSPS::StopTiming()
    {
    TTime readyTime;
    readyTime.HomeTime();
    TTimeIntervalMicroSeconds delay = readyTime.MicroSecondsFrom( iStartTime );
    TTime transferTime( delay.Int64() );
    TBuf<64> timeString;
    transferTime.FormatL( timeString,_L( "Execution time: %S%C microseconds" ) );
    
    EUNIT_PRINT( timeString );
    
    }

// -----------------------------------------------------------------------------
// Sets Central Repository key value
// -----------------------------------------------------------------------------
//    
void MT_CHSPS::SetCentralRepositoryKeyL( 
    TInt aKey,
    TInt aValue )
    {
    CRepository* cr = CRepository::NewL( KhspsThemeStatusRepositoryUid );
    CleanupStack::PushL( cr );
    User::LeaveIfError( cr->Set( aKey, aValue ) );
        
    CleanupStack::PopAndDestroy( cr );    
    }

// -----------------------------------------------------------------------------
// Removes test configuration resource files
// -----------------------------------------------------------------------------
//    
void MT_CHSPS::RemoveResourceFilesL(
    RFs& aFileSession )
    {
    
    CFileMan* fileManager = CFileMan::NewL( aFileSession );
    CleanupStack::PushL( fileManager );
    fileManager->RmDir( _L( "c:\\private\\20000fb1\\2456\\" ) );
    fileManager->RmDir( _L( "c:\\private\\20000fb1\\536874929\\" ) );
    CleanupStack::PopAndDestroy( fileManager );

    }

// ---------------------------------------------------------------------------
// Hsps client service observer
// ---------------------------------------------------------------------------
//
void MT_CHSPS::HandlehspsClientMessage( ThspsServiceCompletedMessage /*aMessage*/ )
    {
    // Asynchronous service handling
    
    }

//  END OF FILE
