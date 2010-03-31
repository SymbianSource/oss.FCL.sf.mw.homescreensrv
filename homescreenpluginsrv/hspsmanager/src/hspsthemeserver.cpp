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
* Description:  HSPS Theme Server. For more information, see the header.
*
*/


#define __INCLUDE_CAPABILITY_NAMES__

// INCLUDE FILES
#include <mw/MemoryManager.h>
#include <centralrepository.h>
#include <f32file.h>
#include <bautils.h>
#include <sbdefs.h>
#include <sysversioninfo.h>
#include <sysutil.h>

#include "hsps_builds_cfg.hrh"
#include "hspsthemeserver.h"
#include "hspsthemeserversession.h"
#include "hspssecurityenforcer.h"
#include "hspsserverutil.h"
#include "hspsdefinitionrepository.h"
#include "hspsdefinitionengineinterface.h"
#include "hspsconfiguration.h"
#include "hspsfamily.h"
#ifdef _hsps_PERFORMANCE_TEST_
#include "hspstimemon.h"
#endif //_hsps_PERFORMANCE_TEST_

#ifndef __DISABLE_SISX_INSTALLATION_
#include "hspsrominstaller.h"
#include "hspsautoinstaller.h"
#include "hspsinstaller.h"
#include "hspsinstallationhandler.h"
#endif //__DISABLE_SISX_INSTALLATION_

#ifdef HSPS_LOG_ACTIVE
#include <hspsodtdump.h>
#include <hspslogbusfile.h>
#endif

// CONSTANTS

// Directory for the SISX installation files
_LIT( KImportDirectoryC, "c:\\private\\200159c0\\import\\" );

// Directories for backup folders
_LIT( KBackupThemesDirectoryC, "c:\\private\\200159c0\\backup\\themes\\" );

// Name of the installation file
_LIT( KManifestFile, "manifest.dat" );

// Name of the configuration file
_LIT( KConfFile, "*.o0000" );

// File prefixes for the *.DAT imports
_LIT( KPrefixFileApp, "app_");
_LIT( KPrefixFilePlugin, "plugin_");

_LIT( KUpgradePlugin, "upgrade_plugin" );
_LIT( KUpgrade, "upgrade" );
_LIT( KDoubleBackSlash, "\\" );
_LIT( KUpgradePluginWild, "upgrade_plugin_*.dat" );
// Just in case emulator cannot read system version we use this to
// prevent ROM installation on every boot up
_LIT( KDummyVersionInfo, "dummyversioninfo" );

const TUint KAppId_hspsAS =  270486738; //  0x101F4CD2 from S60 3.1 Xuikon AppShell in 3.1 product.
const TUint KAppId_hspsAI =  271012080 ; // 0x102750F0 support for Xuikon-based ActiveIdle 
const TUint KAppId_LE =    270551469 ; // 0x102049AD LayoutTest

const TInt KAppIdAny = -1; // All applications

// ======== LOCAL FUNCTIONS ========

// -----------------------------------------------------------------------------
// ResetAndDestroyHBufCArray 
// cleanup item for HBufC dynamic arrays
// -----------------------------------------------------------------------------
static void ResetAndDestroyHBufC8Array( TAny* aArray )
    {
    CArrayPtrSeg<HBufC8>* array = reinterpret_cast< CArrayPtrSeg<HBufC8>* >( aArray );
    array->ResetAndDestroy();
    delete array;
    }

// -----------------------------------------------------------------------------
// ResetAndDestroyChspsODTArray 
// cleanup item for ChspsODT dynamic arrays
// -----------------------------------------------------------------------------
static void ResetAndDestroyChspsODTArray( TAny* aArray )
    {
    CArrayPtrSeg<ChspsODT>* array = reinterpret_cast< CArrayPtrSeg<ChspsODT>* >( aArray );
    array->ResetAndDestroy();
    delete array;
    }

// -----------------------------------------------------------------------------
// Callback function for removing repository lock if error occurs while repository is locked
// -----------------------------------------------------------------------------
//
static void UnlockRepository( TAny* aObject )
    {
    ChspsDefinitionRepository* DefRep = reinterpret_cast<ChspsDefinitionRepository*>( aObject );
        
    if (DefRep->Locked())
        {
        DefRep->Unlock();
        }
    }

// -----------------------------------------------------------------------------
// GetFWVersion
// Gets firmware version of phone and sets it to buffer given as parameter
// -----------------------------------------------------------------------------
//
static void GetFWVersion( TDes& aVersion )
    {
    TInt err = SysVersionInfo::GetVersionInfo( SysVersionInfo::EFWVersion, aVersion );
    if ( err != KErrNone )
        {
        // emulator might not have fwid*.txt files so we need to read sw version
        // by using sysutil.
        aVersion.Zero();
        err = SysUtil::GetSWVersion( aVersion );
        if ( err != KErrNone )
            {
            // for some reason emulator cannot read version info
            aVersion.Copy( KDummyVersionInfo );
            }
        }
    }

// -----------------------------------------------------------------------------
// RunServerL()
// Server startup code
// Perform all server initialisation, in particular creation of the
// scheduler and server and then run the scheduler
// -----------------------------------------------------------------------------
//
static void RunServerL()
    {
#ifdef _hsps_PERFORMANCE_TEST_
    ChspsTimeMon::PrintUserMem( _L("hspsThemeServer::RunServerL(): - initialising..") );
#endif//_hsps_PERFORMANCE_TEST_
    // naming the server thread after the server helps to debug panics
    User::LeaveIfError(User::RenameThread(KhspsThemeServerName));
    // create and install the active scheduler we need
    CActiveScheduler* s=new(ELeave) CActiveScheduler;
    CleanupStack::PushL(s);
    CActiveScheduler::Install(s);
    // create the server (leave it on the cleanup stack)
    ChspsThemeServer::NewLC();
    // Initialisation complete, now signal the client
    RProcess::Rendezvous(KErrNone);
    CActiveScheduler::Start();
    // Cleanup the server and scheduler
    CleanupStack::PopAndDestroy( 2, s );
#ifdef _hsps_PERFORMANCE_TEST_
    ChspsTimeMon::PrintUserMem( _L("hspsThemeServer::RunServerL(): - server fully running") );
#endif//_hsps_PERFORMANCE_TEST_
    }

// -----------------------------------------------------------------------------
// E32Main
// Server process entry-point
// -----------------------------------------------------------------------------
//
TInt E32Main()
    {
    RAllocator* iAllocator = MemoryManager::SwitchToFastAllocator();
    
    __UHEAP_MARK;
    CTrapCleanup* cleanup=CTrapCleanup::New();
    TInt r=KErrNoMemory;
    if (cleanup)
        {
        TRAP(r,RunServerL());
        delete cleanup;
        }
    __UHEAP_MARKEND;
    
    MemoryManager::CloseFastAllocator(iAllocator);
    
    return r;
    }
    
// ======== MEMBER FUNCTIONS ========

#ifdef _hsps_SERVER_SHUTDOWN_ENABLED_
// -----------------------------------------------------------------------------
// CShutdown::NewL
// (other items were commented in a header).
// -----------------------------------------------------------------------------
//
CShutdown* CShutdown::NewL( MShutdownObserver& aObserver )
    {
    CShutdown* object = new (ELeave) CShutdown( aObserver );
    CleanupStack::PushL( object );
    object->ConstructL();
    CleanupStack::Pop( object );
    return object;    
    }

// -----------------------------------------------------------------------------
// CShutdown::CShutdown
// (other items were commented in a header).
// -----------------------------------------------------------------------------
//
CShutdown::CShutdown( MShutdownObserver& aObserver ) : CTimer(-1), iObserver( aObserver )
    {
    CActiveScheduler::Add(this);
    }
  
// -----------------------------------------------------------------------------
// CShutdown::ConstructL
// (other items were commented in a header).
// -----------------------------------------------------------------------------
//
void CShutdown::ConstructL()
    {
    CTimer::ConstructL();
    }

// -----------------------------------------------------------------------------
// CShutdown::Start
// (other items were commented in a header).
// -----------------------------------------------------------------------------
//
void CShutdown::Start()
    {
    After( KhspsShutdownDelay );
    }

// -----------------------------------------------------------------------------
// CShutdown::RunL
// (other items were commented in a header).
// -----------------------------------------------------------------------------
//
void CShutdown::RunL()
    {
    // Notify observer.
    iObserver.ShutdownInitiating();
    
    // Stop active scheduler.
    CActiveScheduler::Stop();
    }
#endif // _hsps_SERVER_SHUTDOWN_ENABLED_

// -----------------------------------------------------------------------------
// ChspsThemeServer::ChspsThemeServer
// (other items were commented in a header).
// -----------------------------------------------------------------------------
//
inline ChspsThemeServer::ChspsThemeServer():CPolicyServer(0,hspsThemePolicy,ESharableSessions)
    {
    }
  
// -----------------------------------------------------------------------------
// ChspsThemeServer::NewLC
// (other items were commented in a header).
// -----------------------------------------------------------------------------
//
CServer2* ChspsThemeServer::NewLC()
    {
#ifdef _hsps_PERFORMANCE_TEST_ 
    ChspsTimeMon::PrintUserMem( _L("SERVER: - ready to install server.") );
#endif    
    ChspsThemeServer* self=new(ELeave) ChspsThemeServer;
    CleanupStack::PushL(self);
    self->ConstructL();

#ifdef _hsps_PERFORMANCE_TEST_ 
    ChspsTimeMon::PrintUserMem( _L("SERVER: - server installed.") );
#endif        
    return self;
    }

// -----------------------------------------------------------------------------
// ChspsThemeServer::ConstructL
// 2nd phase construction - ensure the timer and server objects are running
// (other items were commented in a header).
// -----------------------------------------------------------------------------
//
void ChspsThemeServer::ConstructL()
    {
    User::LeaveIfError( iFsSession.Connect() );
    
#ifdef HSPS_LOG_ACTIVE    
    iLogBus = ChspsLogBusFile::NewL( ChspsLogBusFile::CreateLogFilename( _L("themeserver") ) );
    iLogBus->LogText( _L( "--------------------------------------------------------" ) );
    iLogBus->LogText( _L( "hspsThemeServer: constructing server" ) );
#endif
           
    // Setup a runtime array of headers (=header cache)
    iHeaderListCache = new ( ELeave ) CArrayPtrSeg< ChspsODT >( KHeaderListGranularity );
        
    iDefinitionRepository = ChspsDefinitionRepository::NewL();
#ifdef HSPS_LOG_ACTIVE    
    iDefinitionRepository->SetLogBus( iLogBus );
#endif
    // Setup a Plug-in Repository listener
    iCentralRepository = CRepository::NewL( KhspsThemeStatusRepositoryUid ); 
    
    // Get active device language
    iDeviceLanguage = GetDeviceLanguage();
#ifdef HSPS_LOG_ACTIVE    
    iLogBus->LogText( _L( "ChspsThemeServer::GetDeviceLanguage() %d" ), iDeviceLanguage );
#endif

    // Setup a search mask for finding headers from the cache
    iCacheMask = ChspsODT::NewL();

    // Setup a listener for changes in the Central Repository
    iCenRepListener = ChspsCenRepListener::NewL( *this, KhspsThemeStatusRepositoryUid );        
    
	// Access lists for client processes
    iSecurityEnforcer = ChspsSecurityEnforcer::NewL( *iDefinitionRepository, *iCentralRepository );
#ifdef HSPS_LOG_ACTIVE    
     iSecurityEnforcer->SetLogBus( iLogBus );
#endif     
    
    // Find header files from the Plug-in Repository
	UpdateHeaderListCacheL();  
    
    // Adds the server with the specified name to the active scheduler, and issues the 
    // first request for messages, and leaves if the operation fails
    StartL( KhspsThemeServerName );
    
    // Initialize remove index
    iThemeIndexToRemove = KErrNotFound;
                           
    // Listen to changes in any key
    iCenRepListener->SetupAll();     
        
    // Start observing the notifications
    iDefinitionRepository->RegisterObserverL( *this );
           

#if defined(WINSCW) || defined(__WINS__)
    // Resolution & orientation change listener
    iFamily = ChspsFamilyListener::NewL( *this );
#else 
    iFamily = ChspsFamily::NewL();
#endif //defined(WINSCW) || defined(__WINS__)
    
    // Auto-localize ODTs in the Definition Repository when the device language has changed
    HandleLanguageChangeL();            
    
    // Installs manifest files from the ROM drive when server is starting for the first time
    HandleRomInstallationsL();
        
#ifndef __DISABLE_SISX_INSTALLATION_
    // Start observing of import folder to enable autoinstallations
    EnableAutoInstallationL();
#endif //__DISABLE_SISX_INSTALLATION_    

    // Update configurations
    // When server is started all configurations with "WaitForConfirmation" state are updated
    // to "NotConfirmed" state to allow client to validate configurations
    User::LeaveIfError( iCentralRepository->Set( KCenrepKeyCheckConf, 0 ) );
    CheckConfigurationL( KAppIdAny );
    // Activate configuration checking
    // Configurations are checked every time when a new client session is created
    User::LeaveIfError( iCentralRepository->Set( KCenrepKeyCheckConf, 1 ) );

    // Backup event listener
    iBRObserver = CHSPSBRObserver::NewL( *this );
    iBRObserver->Start();

    // Initialize backup state
    iBRState = EhspsBRNormal;

#ifdef _hsps_SERVER_SHUTDOWN_ENABLED_    
    iShutdown = CShutdown::NewL( *this );
#endif // _hsps_SERVER_SHUTDOWN_ENABLED_
    
#ifdef HSPS_LOG_ACTIVE	
    iLogBus->LogText( _L( "hspsThemeServer: server fully constructed" ) );
    iLogBus->LogText( _L( "--------------------------------------------------------" ) );
#endif    
    }

// Destructor
ChspsThemeServer::~ChspsThemeServer()
    {
#ifdef _hsps_SERVER_SHUTDOWN_ENABLED_
    delete iShutdown;
    iShutdown = NULL;
#else
    // Server must call cleanup if shutdown object does not exist (and call it).
    Cleanup();
#endif // _hsps_SERVER_SHUTDOWN_ENABLED_        
    
#ifdef HSPS_LOG_ACTIVE    
    // Delete log bus as absolute last!
    // Some members might log destructor activity to this object.
    delete iLogBus;
    iLogBus = NULL;
#endif    
    
    iSessions.Reset();
    }

#ifdef _hsps_SERVER_SHUTDOWN_ENABLED_
// -----------------------------------------------------------------------------
// ChspsThemeServer::ShutdownInitiating
// From MShutdownObserver.
// -----------------------------------------------------------------------------
//
void ChspsThemeServer::ShutdownInitiating()
    {
    Cleanup();
    }
#endif // #ifdef _hsps_SERVER_SHUTDOWN_ENABLED_

// -----------------------------------------------------------------------------
// ChspsThemeServer::Cleanup
// (other items were commented in a header).
// -----------------------------------------------------------------------------
//
void ChspsThemeServer::Cleanup()
    {
#ifdef HSPS_LOG_ACTIVE      
    iLogBus->LogText( _L( "hspsThemeServer: server is shutting down" ) );
#endif    
            
    if( iDefinitionRepository )
        {
        iDefinitionRepository->UnregisterObserver( *this );
        }

#ifndef __DISABLE_SISX_INSTALLATION_
    DisableAutoInstallation();
#endif //__DISABLE_SISX_INSTALLATION_

    if ( iFamily )
        {
        delete iFamily;
        iFamily = NULL;
        }

    delete iCenRepListener;
    iCenRepListener = NULL;
    
    delete iRomInstaller;
    iRomInstaller = NULL;
    
    delete iAutoInstaller;    
    iAutoInstaller = NULL;
    
    if( iHeaderListCache )
        {
        iHeaderListCache->ResetAndDestroy();
        delete iHeaderListCache;  
        iHeaderListCache = NULL;
        }
             
    delete iDefinitionRepository;
    iDefinitionRepository = NULL;
    
    delete iCentralRepository;
    iCentralRepository = NULL;
    
    delete iSecurityEnforcer;
    iSecurityEnforcer = NULL;
    
    delete iCacheMask;
    delete iBRObserver;
    delete iActiveBackupClient;
    delete iBRHandler;

    iCacheMask = NULL;
    
    iFsSession.Close();    
    }

// -----------------------------------------------------------------------------
// ChspsThemeServer::NewSessionL
// Create a new client session. This should really check the version number.
// (other items were commented in a header).
// -----------------------------------------------------------------------------
//
CSession2* ChspsThemeServer::NewSessionL(const TVersion&,const RMessage2& aMessage ) const
    {
    return new (ELeave) ChspsThemeServerSession( aMessage.SecureId().iId );
    }

// -----------------------------------------------------------------------------
// ChspsThemeServer::AddSession
// A new session is being created
// (other items were commented in a header).
// -----------------------------------------------------------------------------
//
void ChspsThemeServer::AddSession( ChspsThemeServerSession* aSession )
    {
    if( aSession == NULL )
        {
        return;
        }
    
    iSessions.Append( aSession );    

#ifdef _hsps_SERVER_SHUTDOWN_ENABLED_    
    if( iShutdown->IsActive() )
        {
        iShutdown->Cancel();
        }        
#endif // _hsps_SERVER_SHUTDOWN_ENABLED_

#ifdef HSPS_LOG_ACTIVE    
    iLogBus->LogText( _L( "ChspsThemeServer::AddSession(): - now %d concurrent sessions." ),
        iSessions.Count() );
#endif    
    }


// -----------------------------------------------------------------------------
// ChspsThemeServer::DropSession
// A session is being destroyed
// (other items were commented in a header).
// -----------------------------------------------------------------------------
//
void ChspsThemeServer::DropSession( ChspsThemeServerSession* aSession )
    {
    const TInt index = iSessions.Find( aSession );
    if( index != KErrNotFound )
        {
        iSessions.Remove( index );
        }
    
#ifdef _hsps_SERVER_SHUTDOWN_ENABLED_    
    if( iSessions.Count() == 0 )
        {
        iShutdown->Cancel();
        iShutdown->Start();
        }
#endif // _hsps_SERVER_SHUTDOWN_ENABLED_

#ifdef HSPS_LOG_ACTIVE    
    iLogBus->LogText( _L( "ChspsThemeServer::DropSession(): - %d concurrent sessions left." ),
        iSessions.Count() );
#endif    
    }
  
// -----------------------------------------------------------------------------
// ChspsThemeServer::IncreaseRequestClientCount
// (other items were commented in a header).
// -----------------------------------------------------------------------------
//
void ChspsThemeServer::IncreaseRequestClientCount()
    {
#ifdef HSPS_LOG_ACTIVE    
    iLogBus->LogText( _L( "ChspsThemeServer: ChspsThemeServer::IncreaseRequestClientCount()" ) );
#endif
    
    iRequestClientCount++;
    } 
  
// -----------------------------------------------------------------------------
// ChspsThemeServer::DecreaseRequestClientCount
// (other items were commented in a header).
// -----------------------------------------------------------------------------
//
void ChspsThemeServer::DecreaseRequestClientCount()
    {
#ifdef HSPS_LOG_ACTIVE    
    iLogBus->LogText( _L( "ChspsThemeServer: ChspsThemeServer::DecreaseRequestClientCount()" ) );
#endif    

    if ( iRequestClientCount )
        {
        iRequestClientCount--;
        }
    } 
    
    
// -----------------------------------------------------------------------------
// ChspsThemeServer::HandleDefinitionRespositoryEvent()
// Handles events coming from hspsDefinitionRepository.
// (other items were commented in a header).
// -----------------------------------------------------------------------------
//
TBool ChspsThemeServer::HandleDefinitionRespositoryEvent( ThspsRepositoryInfo aRepositoryInfo )
    {
    TInt mask = aRepositoryInfo.iEventType;
    TBool ret = EFalse;

#ifdef HSPS_LOG_ACTIVE
    iLogBus->LogText( _L( "--------------------------------------------------------" ) );
#endif        

    if( aRepositoryInfo.iEventType & EhspsODTUpdated ||
        aRepositoryInfo.iEventType & EhspsODTModified ||
        aRepositoryInfo.iEventType & EhspsPluginReplaced )
        {
        for( TInt i = 0; i < iSessions.Count(); i++ )
            {
            if( iSessions[i]->AppUid() == aRepositoryInfo.iAppUid )
                {
                iSessions[i]->SetResourceFileCopyRequired( ETrue );
                }
            }        
        }    
    
    // If header cache should be updated from files in the Plug-in Repository
    if (mask & EhspsCacheUpdate)
        {
#ifdef HSPS_LOG_ACTIVE        
        iLogBus->LogText( _L( "ChspsThemeServer::HandleDefinitionRespositoryEvent(): - EhspsCacheUpdate occurred." ) );
#endif        
        // Update the cache from contents of the themes folder in drive C (slowish operation!) 
        TRAP_IGNORE( UpdateHeaderListCacheL() );  
        }
        
    // If a plug-in configuration was uninstalled from the Plug-in Repository
    if (mask & EhspsClean)
        {
#ifdef HSPS_LOG_ACTIVE        
        iLogBus->LogText( _L( "ChspsThemeServer::HandleDefinitionRespositoryEvent(): - EhspsClean occurred." ) );
#endif                  
        if(aRepositoryInfo.iLastNotification)
            {
            // Remove the provided ODT from the header list cache 
                    UpdateHeaderListCache(
                        EhspsCacheRemoveHeader,
                        aRepositoryInfo.iPluginIfUid,
                        aRepositoryInfo.iPluginProviderUid,
                        aRepositoryInfo.iPluginUid
                        );
            }
          
        }
    
    // If a plug-in configuration was installed into the Plug-in Repository
    if (mask & EhspsODTAdded)
        {
#ifdef HSPS_LOG_ACTIVE        
        iLogBus->LogText( _L( "ChspsThemeServer::HandleDefinitionRespositoryEvent(): - EhspsODTAdded occurred." ) );
#endif  
        if(aRepositoryInfo.iLastNotification)
            {
            // Add the provided ODT to the header list cache
                    UpdateHeaderListCache(
                        EhspsCacheAddHeader,
                        aRepositoryInfo.iPluginIfUid,
                        aRepositoryInfo.iPluginProviderUid,
                        aRepositoryInfo.iPluginUid
                        );
            }
                                 
        }

    // If a plug-in configuration was upgraded into the Plug-in Repository
    if( mask & EhspsODTUpdated )
        {
#ifdef HSPS_LOG_ACTIVE        
        iLogBus->LogText( _L( "ChspsThemeServer::HandleDefinitionRespositoryEvent(): - EhspsODTUpdate occurred." ) );
#endif  
        if( aRepositoryInfo.iLastNotification )
            {
            // Add the provided ODT to the header list cache
            UpdateHeaderListCache(
                EhspsCacheAddHeader,
                aRepositoryInfo.iPluginIfUid,
                aRepositoryInfo.iPluginProviderUid,
                aRepositoryInfo.iPluginUid
                );
            }                                 
        }    
    
    // If settings of an application configuration were modified
    if (mask & EhspsSettingsChanged)
        {
#ifdef HSPS_LOG_ACTIVE        
        iLogBus->LogText( _L( "ChspsThemeServer::HandleDefinitionRespositoryEvent(): - EhspsSettingsChanged occurred." ) );
#endif
        }

    // If a plug-in configuration was added into or moved within an application configuration
    if (mask & EhspsODTModified)
        {
#ifdef HSPS_LOG_ACTIVE        
        iLogBus->LogText( _L( "ChspsThemeServer::HandleDefinitionRespositoryEvent(): - EhspsODTModified occured." ) );
#endif
        }
    
    // If a plug-in configuration was removed from an application configuration
    if (mask & EhspsODTRemoved)
        {
#ifdef HSPS_LOG_ACTIVE        
        iLogBus->LogText( _L( "ChspsThemeServer::HandleDefinitionRespositoryEvent(): - EhspsODTRemoved occurred." ) );
#endif        
        }

#ifdef HSPS_LOG_ACTIVE
    iLogBus->LogText( _L( "--------------------------------------------------------" ) );
#endif
    
    return ret;
    }
    
// -----------------------------------------------------------------------------
// ChspsThemeServer::DefinitionRepository
// (other items were commented in a header).
// -----------------------------------------------------------------------------
//
ChspsDefinitionRepository& ChspsThemeServer::DefinitionRepository()
    {
    return *iDefinitionRepository;
    } 
  
// ChspsThemeServer::SecurityEnforcer
// (other items were commented in a header).
// -----------------------------------------------------------------------------
//
ChspsSecurityEnforcer& ChspsThemeServer::SecurityEnforcer()
    {
    return *iSecurityEnforcer;
    } 
  
  
// ChspsThemeServer::CentralRepository
// (other items were commented in a header).
// -----------------------------------------------------------------------------
//
CRepository& ChspsThemeServer::CentralRepository()
    {
    return *iCentralRepository;
    }
  
// ChspsThemeServer::DeviceLaguage
// (other items were commented in a header).
// -----------------------------------------------------------------------------
//
TLanguage ChspsThemeServer::DeviceLanguage()
    {
    iDeviceLanguage = GetDeviceLanguage();
    return iDeviceLanguage;
    }   
  
// -----------------------------------------------------------------------------
// ChspsThemeServer::UpdateHeaderListCache
// (other items were commented in a header).
// -----------------------------------------------------------------------------
//
CArrayPtrSeg<ChspsODT>& ChspsThemeServer::HeaderListCache()
    {
    return *iHeaderListCache;
    }   

// -----------------------------------------------------------------------------
// ChspsThemeServer::UninstallImport
// -----------------------------------------------------------------------------
//
void ChspsThemeServer::UninstallImport( 
		const CDir* aFilelist )
	{
	TInt count( 0 );
	if ( aFilelist )
		{
		count = aFilelist->Count();
		}
		
	// Loop installed configurations
    for( TInt i = 0; i < iInstalledSisxThemes.Count(); i++ )
        {
        // Suppose this is the one which was removed
        iThemeIndexToRemove = i;
        
        // Loop files under the imports folder 
        for( TInt j = 0; j < count; j++ )
            {
            TPtrC filename( ( *aFilelist )[j].iName );
            
            // If theme path exists
            if( filename == *( iInstalledSisxThemes[i] ) )
                {
                // Break inner loop
                iThemeIndexToRemove = KErrNotFound; 
                break;                       
                }
            }
        
        // iThemeIndexToRemove should now point the configuration which was removed
        RunAutoInstaller( EAutoInstallerUninstall );                                                   
        }  	
	}

// -----------------------------------------------------------------------------
// ChspsThemeServer::GetManifestFromImportLC
// -----------------------------------------------------------------------------
//
HBufC* ChspsThemeServer::GetManifestFromImportLC(
		const TDesC& aImportFile,
		const TDesC& aPath )		
	{	
	HBufC* manifestBuf = 0;
	
	const TChar KCharUnderscore('_');

    // Strip file prefix 
    TPtrC fixedName;        	                                
    TInt offset = aImportFile.Locate( KCharUnderscore );
    if( offset )
        {
        TPtrC filePrefix( aImportFile.Left(offset+1) );
        
        // If a valid prefix
        if ( filePrefix.CompareF( KPrefixFileApp) || filePrefix.CompareF( KPrefixFilePlugin ) )
        	{
        	fixedName.Set( aImportFile.Mid( filePrefix.Length() ) );
        	}
        }    	
    
    // If stripping was successfull
    if ( fixedName.Length() )
    	{
	    // Remove file extension
	    TParsePtrC parsePtr( fixedName );            		   
	    TPtrC fileName( parsePtr.Name() );
	    
	    // Setup path for the manifest file
	    manifestBuf = HBufC::NewLC( KMaxFileName );
	    TPtr manifestPtr( manifestBuf->Des() );		    
	    manifestPtr.Append( aPath );
	    manifestPtr.Append( fileName );
	                                                                                
	    // Replace underscores with backslashes
	    TInt pos( manifestPtr.Locate( KCharUnderscore ) );                   	            		    
	    while( pos != KErrNotFound )
	        {
	        manifestPtr.Replace( pos, 1, KDoubleBackSlash );
	        pos = manifestPtr.Locate( KCharUnderscore );                
	        }		           
	    manifestPtr.Append( KDoubleBackSlash );            
	
	    // Find a manifest.dat file from the generated path
	    CDir* manifestDir( NULL );		                      
	    TFindFile fileFinder( iFsSession );
	    fileFinder.FindWildByDir( KManifestFile, manifestPtr, manifestDir );    
	    
	    // Check whether the manifest was found
	    if( !manifestDir || manifestDir->Count() > 1 )
	        {
	        // Failed
	        CleanupStack::PopAndDestroy( 1, manifestBuf );
	        manifestBuf = NULL;
	        }                                            
	    else
	        {    
	        manifestPtr.Append( KManifestFile );
	        }	    
	    
	    delete manifestDir;
	    manifestDir = NULL;
    	}
    
    return manifestBuf;
	}

// -----------------------------------------------------------------------------
// ChspsThemeServer::CheckConfiguration
// -----------------------------------------------------------------------------
//
void ChspsThemeServer::CheckConfigurationL(
    const TInt aAppUid )        
    {
    
    if ( iHeaderListCache )
        {
        TInt count = iHeaderListCache->Count();
        if ( iDefinitionRepository->Locked() )
            {
            User::Leave( KErrInUse );
            }
        iDefinitionRepository->Lock();

        // Check if configuration checking is activated
        TPtrC8 confState;
        TPtrC8 nextConfState;
        TInt value( 0 );
        User::LeaveIfError( iCentralRepository->Get( KCenrepKeyCheckConf, value ) );
        if ( value == 1 )
            {
            // Configuration checking activated
            // All configurations with "WaitForConfiguration" state will be updated to
            // "Error" state
            confState.Set( KConfStateWaitForConfirmation );
            nextConfState.Set( KConfStateError );
            }
        else
            {
            // Configuration checking not activated
            // All configurations with "WaitForConfiguration" state will be updated to
            // "NotConfirmed" state
            confState.Set( KConfStateWaitForConfirmation );
            nextConfState.Set( KConfStateNotConfirmed );
            }
        
        for ( TInt i = 0; i < count; i++ )
            {
            ChspsODT* header = ( *iHeaderListCache )[ i ];
            if ( header->ConfigurationType() == EhspsAppConfiguration &&
                 ( header->RootUid() == aAppUid ||
                   aAppUid == KAppIdAny ) )
                {
                // Get ODT from definition repository
                
                ChspsODT* odt = ChspsODT::NewL();
                CleanupStack::PushL( odt );
                odt->SetRootUid( header->RootUid() );
                odt->SetThemeUid( header->ThemeUid() );        
                odt->SetConfigurationType( header->ConfigurationType() );
                odt->SetRootUid( header->RootUid() );
                odt->SetProviderUid( header->ProviderUid() );
                odt->SetThemeUid( header->ThemeUid() );
                odt->SetProviderNameL( header->ProviderName() );
                odt->SetThemeFullNameL( header->ThemeFullName() );
                odt->SetThemeShortNameL( header->ThemeShortName() );
                odt->SetThemeVersionL( header->ThemeVersion() );
                odt->SetDescriptionL( header->Description() );
                odt->SetFlags( header->Flags() );                                                                       
                odt->SetMultiInstance( header->MultiInstance() );

                User::LeaveIfError( iDefinitionRepository->GetOdtL( *odt ) );
 
                // Update configuration states
                TBool odtUpdated( EFalse );
                hspsServerUtil::UpdateConfigurationStateL(
                    *odt,
                    confState,
                    nextConfState,
                    odtUpdated );
                if ( odtUpdated )
                    {
                    // Update modified ODT to definition repository
                    User::LeaveIfError( iDefinitionRepository->SetOdtL( *odt ) );
                    }
                
                CleanupStack::PopAndDestroy( odt );
                }
            }
        iDefinitionRepository->Unlock();
        }
    }

// -----------------------------------------------------------------------------
// ChspsThemeServer::InstallImportsL
// -----------------------------------------------------------------------------
//
void ChspsThemeServer::InstallImportsL( 
		const CDir& aFilelist )
	{		                        
    const TInt count( aFilelist.Count() );
		
    // Go through the filtered file list
    for( TInt importIndex = 0; importIndex < count; importIndex++ )
        {        
        TBool found( EFalse );
        TPtrC importName( aFilelist[importIndex].iName );
        	
    	// Is the file already pending an installation?
        for( TInt j = 0; !found && j < iInstallableSisxThemes.Count(); j++ )
            {                
            if( importName == *( iInstallableSisxThemes[j] ) )
                {
                found = ETrue;
                }                    
            }  
        
        // Is the file already installed?
        for( TInt j = 0; !found && j < iInstalledSisxThemes.Count(); j++ )
            {
            if( importName == *( iInstalledSisxThemes[j] ) )
                {
                found = ETrue;
                }
            }
        
        if ( !found )
        	{
        	// Get manifest from the import's name in C drive 
        	HBufC* manifestBuf = GetManifestFromImportLC( 
        	        importName, 
        	        KImportDirectoryC );        	
        	if( manifestBuf )
        		{        		
        		// Append the manifest 
        		iManifestFiles.AppendL( manifestBuf );
	            CleanupStack::Pop( manifestBuf );          
	            
	        	// Append the import
		        iInstallableSisxThemes.AppendL( importName.AllocL() );
        		}
        	}
        }
    
    // Now that the manifest list has been parsed, initiate installation of 
    // the new manifest files
    if( iAutoInstallerState != EAutoInstallerStateInstall )
        {                        
        RunAutoInstaller( EAutoInstallerStateInstall );                                      
        }           
	}

// -----------------------------------------------------------------------------
// ChspsThemeServer::HandleConfigurationImportsL
// -----------------------------------------------------------------------------
//
void ChspsThemeServer::HandleConfigurationImportsL(
		const TDesC& aFilePrefix )
	{
	// Set mask for finding files under imports folder
    TPtrC filter( KFilterAllPluginImportsV1 ); 
    if ( aFilePrefix.CompareF( KPrefixFileApp ) == 0 )
    	{
    	filter.Set( KFilterAllAppImportsV1 );
    	}

    // Get a listing of plugin_*.dat files from the imports folder in C drive
	CDir* importsDir( NULL );  	        
    TFindFile fileFinder( iFsSession );
    fileFinder.FindWildByDir( filter, KImportDirectoryC, importsDir );    
    CleanupStack::PushL( importsDir );
    
    // pathFileList can be NULL when no path_*.dat files are available  in the import folder. 
    // This can mean that currently the last sisx package is under unistallation, and because 
    // of that, we need to check that case with pathFileCount = 0
    TInt importsCount( 0 );    
    if( importsDir )
        {
        importsCount = importsDir->Count();
        }
    
    // Find out number of installed themes with the provided file prefix
    TInt installedCount = 0;    
    const TInt prefixLength = aFilePrefix.Length();
    for( TInt i=0; i < iInstalledSisxThemes.Count(); i++  )
    	{  
    	TPtrC ptr( iInstalledSisxThemes[ i ]->Des() );    	
    	if ( ptr.Length() > prefixLength )
    		{
    		if ( ptr.Left( prefixLength ).CompareF( aFilePrefix ) == 0 )
    			{
    			installedCount++;
    			}
    		}
    	}
    
    if( importsCount < installedCount )        
        {
        // there were less *.dat files than in the last evaluation
        UninstallImport( importsDir );         
        }                                
    else if( importsCount > installedCount && importsDir )
        {
        // If a *.dat file was added since the last evaluation        
        InstallImportsL( *importsDir );
        }

    CleanupStack::PopAndDestroy( importsDir );                  	
	}

// -----------------------------------------------------------------------------
// ChspsThemeServer::HandleFileSystemChangeL
// -----------------------------------------------------------------------------
//
void ChspsThemeServer::HandleFileSystemChangeL()
    {    
    // Install unprocessed plug-in configurations from the import folders,  
    // these might be referenced by one or more application configurations - thus,
    // this step should be done first!
    HandleConfigurationImportsL( KPrefixFilePlugin );
    
    // Install remaining application configurations from the import folders
    HandleConfigurationImportsL( KPrefixFileApp );
    
    // File system change can also mean that a theme is being updated           
    HandleThemeUpgradeL();    
    }

// -----------------------------------------------------------------------------
// ChspsThemeServer::HandleFileSystemChange
// -----------------------------------------------------------------------------
//
void ChspsThemeServer::HandleFileSystemChange()
    {    
    // Fancy way of checking whether SISX installations are enabled :)    
#ifndef __DISABLE_SISX_INSTALLATION_
    
    // Register again ...
    iFileChangeListener->Setup();
    
    // ... and then handle the change
    TRAP_IGNORE( HandleFileSystemChangeL() );
    
#endif // __DISABLE_SISX_INSTALLATION_    
     }

// -----------------------------------------------------------------------------
// ChspsThemeServer::CheckActiveTheme
// CheckActiveTheme the specified theme.
// (other items were commented in a header).
// -----------------------------------------------------------------------------
void ChspsThemeServer::CheckActiveThemesL()    
   {        
#ifndef __DISABLE_SISX_INSTALLATION_        
    TInt themeUid;
    RArray<TUint32> res;
    CleanupClosePushL( res );
    TInt errorCode = KErrNone;
    iCentralRepository->FindL( 0x00000000,0xFFFFFFFF, res );
    const TInt keyCount = res.Count();
    for (TInt i = 0; i < keyCount; i++ )   	    
   	    {   	    
   	    errorCode = iCentralRepository->Get( res[i], themeUid );
   	    if ( !errorCode )
   	        {   	        
   	        HBufC* matchthemeUid = HBufC::NewLC( KMaxFileName );
   	        TPtr matchthemeUidPtr = matchthemeUid->Des();         
            matchthemeUidPtr.Append( KPrefixFileApp );     
            matchthemeUidPtr.Append( _L("*_") );
            matchthemeUidPtr.AppendNumUC( themeUid, EHex );
            matchthemeUidPtr.Append( _L( "_*" ) );                
            
            HBufC* pathFileName = HBufC::NewL ( KMaxFileName );
            TPtr pathFileNamePtr = pathFileName->Des();    
            CDir* fileList;  
            TFindFile fileFinder( iFsSession );                    
            // Find path info file for matching unique themeuid
            fileFinder.FindWildByDir( *matchthemeUid, KImportDirectoryC, fileList );       
            if( fileList )
                {
                TBool isItOpen = EFalse;                            
                pathFileNamePtr.Copy( KImportDirectoryC );    
                pathFileNamePtr.Append( (*fileList)[0].iName );                
                if ( res[i] == KAppId_hspsAS )
                    {
                    errorCode = iFsSession.IsFileOpen( pathFileNamePtr, isItOpen );    
                    if ( !isItOpen )
                        {
                        errorCode = iFileAS.Open(iFsSession, pathFileNamePtr, EFileRead );
                        }           
                    }
                else if ( res[i] == KAppId_hspsAI )
                    {
                    errorCode = iFsSession.IsFileOpen( pathFileNamePtr, isItOpen );
                    if ( !isItOpen )
                        {
                        errorCode = iFileAI.Open(iFsSession, pathFileNamePtr, EFileRead );
                        }           
                    }
                else if ( res[i] == KAppId_LE )
                    {
                    errorCode = iFsSession.IsFileOpen( pathFileNamePtr, isItOpen );
                    if ( !isItOpen )
                        {
                        errorCode = iFileLT.Open(iFsSession, pathFileNamePtr, EFileRead );
                        }           
                    }                                                
                }
            delete fileList;
            delete pathFileName;
            CleanupStack::PopAndDestroy( matchthemeUid ); 
   	        }
   	    }
   	res.Close();    
   	CleanupStack::PopAndDestroy(1, &res);
#endif //__DISABLE_SISX_INSTALLATION_   	      
    }
    
// -----------------------------------------------------------------------------
// ChspsThemeServer::UnlockSisxImportL()
// -----------------------------------------------------------------------------
//
void ChspsThemeServer::UnlockSisxImportL( 
        const TUint32 aAppUid )
    {            
__UHEAP_MARK;    
    // Setup a filter for finding an SISX import with the provided application uid
    HBufC* filterBuf = HBufC::NewLC( KMaxFileName );
    TPtr filterPtr = filterBuf->Des();        
    filterPtr.Copy( KPrefixFileApp );        
    filterPtr.AppendNumUC( aAppUid, EHex );        
    _LIT(KFilter, "_*_*_1.0.dat");
    filterPtr.Append( KFilter );    
         
    // Find imports with the generated filter (ROM based imports should not be found)
    CDir* importsDir( NULL );  
    TFindFile fileFinder( iFsSession );                    
    fileFinder.FindWildByDir( filterPtr, KImportDirectoryC, importsDir );   
    if( importsDir )
        {        
        HBufC* importBuf = HBufC::NewLC ( KMaxFileName );
        TPtr importPtr = importBuf->Des();        
                            
        // CleanupStack is not required since there are no leaving function calls below
        
        // Loop application specific imports
        TInt errorCode = KErrNone;
        TBool isOpen = EFalse;
        for ( TInt i = 0; i< importsDir->Count(); i++ )
            {
            importPtr.Copy( KImportDirectoryC );                       
            importPtr.Append( (*importsDir)[i].iName );
            errorCode = iFsSession.IsFileOpen( importPtr, isOpen );
            if ( errorCode == KErrNone )
                {
                // Only a single file should be open
                if( isOpen ) 
                    {               
                    // Close application specific file handles
                    if ( aAppUid == KAppId_hspsAS )
                        {                                         
                        iFileAS.Close();
                        }
                    else if ( aAppUid == KAppId_hspsAI )
                        {                                                        
                        iFileAI.Close(); 
                        }
                    else if ( aAppUid == KAppId_LE )
                        {                            
                        iFileLT.Close();                                           
                        }                           
                    }
                }
            }
        
        CleanupStack::PopAndDestroy( importBuf );
        delete importsDir;
        }        
            
    CleanupStack::PopAndDestroy( filterBuf );
__UHEAP_MARKEND;    
    }

// -----------------------------------------------------------------------------
// ChspsThemeServer::LockSisxImportL()
// -----------------------------------------------------------------------------
//
void ChspsThemeServer::LockSisxImportL( 
        const TUint32 aAppUid,
        const TInt aConfUid )
    {
__UHEAP_MARK;                
    // Setup a filter for finding specific application configuration
    HBufC* filterBuf = HBufC::NewLC( KMaxFileName );
    TPtr filterPtr = filterBuf->Des();                         
    filterPtr.Copy( KPrefixFileApp );
    filterPtr.Append( _L("*_*_") );
    filterPtr.AppendNumUC( aConfUid, EHex );
    filterPtr.Append( _L( "_1.0.dat" ) );
    
    // Find imports with the generated filter (ROM based imports should not be found)
    CDir* importsDir( NULL );  
    TFindFile fileFinder( iFsSession );
    fileFinder.FindWildByDir( filterPtr, KImportDirectoryC, importsDir );           
    if ( importsDir )        
       {
       CleanupStack::PushL( importsDir );
       
       // themeuid is unique so count should be 1           
       if ( importsDir->Count() == 1 )
           {
           HBufC* importBuf = HBufC::NewLC ( KMaxFileName );
           TPtr importPtr = importBuf->Des();               
           importPtr.Copy( KImportDirectoryC );
           importPtr.Append( (*importsDir)[0].iName );           
       
           TBool isOpen = EFalse;
           TInt errorCode = iFsSession.IsFileOpen( importPtr, isOpen );
           if ( errorCode == KErrNone && !isOpen )
               {
               // Lock the application specific SISX imports
               if ( aAppUid == KAppId_hspsAS )
                   {
                   errorCode = iFileAS.Open( iFsSession, importPtr, EFileRead );                   
                   }
               else if ( aAppUid == KAppId_hspsAI )
                   {
                   errorCode = iFileAI.Open( iFsSession, importPtr, EFileRead );                    
                   }
               else if ( aAppUid == KAppId_LE )
                   {
                   errorCode = iFileLT.Open( iFsSession, importPtr, EFileRead );                    
                   }   
               }
           CleanupStack::PopAndDestroy( importBuf );
           }
       
       CleanupStack::PopAndDestroy( importsDir );           
       }
    
    CleanupStack::PopAndDestroy( filterBuf );                
__UHEAP_MARKEND;    
    }


// -----------------------------------------------------------------------------
// ChspsThemeServer::HandleCenRepChangeL()
// Handles events coming from CentralRepository.
// -----------------------------------------------------------------------------
//
void ChspsThemeServer::HandleCenRepChangeL( const TUint32 aKey )
    {    
    switch ( aKey )
        {        
        case KCenrepKeyLang:
        case KCenrepKeyPostRFS:
        case KCenrepKeyCheckConf:
            {
            // No processing required
            }
            break;
        default:
            {
                        
            // If SISX installations were enabled
#ifndef __DISABLE_SISX_INSTALLATION_                            
            // Get active application configuration for the provided application uid        
            TInt confUid(0);
            TInt errorCode = iCentralRepository->Get( aKey, confUid );    
            if ( !errorCode && confUid > 0 )
                {
                // Remove file lock from active SISX import
                UnlockSisxImportL( aKey );    
                // Lock the new SISX import (prevents uninstallation of the SISX 
                // while it's being used as an active application configuration)
                LockSisxImportL( aKey, confUid );
                }
#endif //__DISABLE_SISX_INSTALLATION_
            
            }
        }
    
    // Set up the listener to listen to changes in Central Repository for all settings 
    iCenRepListener->SetupAll();
    }
    
// -----------------------------------------------------------------------------
// ChspsThemeServer::HandleAutoInstallerEvent
// (other items were commented in a header).
// -----------------------------------------------------------------------------
//
void ChspsThemeServer::HandleAutoInstallerEvent( ThspsServiceCompletedMessage aMsg )
    {    
#ifndef __DISABLE_SISX_INSTALLATION_    
    switch ( aMsg )
        {
        case EhspsInstallPhaseSuccess:
            // Phase was success, installation will continue
            break;        
        case EhspsInstallThemeSuccess:            
            {
            // Mark current theme as installed
        	TBool isInstalled = EFalse;
        	
        	//check if theme is already listed as installed
        	//there is a possibility that the install is an upgrade and the theme would be alredy listed 
        	//in the iInstalledSisxThemes
        	if( iInstalledSisxThemes.Count() > iCurrentManifestFile )
        	    {
        	    for (TInt i = 0; i < iInstalledSisxThemes.Count(); i++)
        	        {
        	        if ( *iInstalledSisxThemes[i] == *iInstallableSisxThemes[ iCurrentManifestFile ] )
        	            {
        	            isInstalled = ETrue;
        	            break;
        	            }
        	        }
        		}
        	
        	//if theme is not installed yet than add it to iInstalledSisxThemes
        	if ( !isInstalled ) 
        		{
        		iInstalledSisxThemes.Append( iInstallableSisxThemes[ iCurrentManifestFile ] ); 
        		}              
        	
        	iCurrentManifestFile++;
            
            // Check if more installable files available
            delete iAutoInstaller;
            iAutoInstaller = NULL;
                                                                                                            
            RunAutoInstaller( EAutoInstallerStateInstall );                       
            }
            break;             
        case EhspsRemoveThemeSuccess:            
            {
            delete iInstalledSisxThemes[ iThemeIndexToRemove ];
            iInstalledSisxThemes[ iThemeIndexToRemove ] = NULL;
            iInstalledSisxThemes.Remove( iThemeIndexToRemove );
            }
            /* flow through */
        case EhspsRemoveThemeFailed:               
            /* flow through */            
        default:                        
            StopAutoInstaller();            
            break;
        }        
#endif //__DISABLE_SISX_INSTALLATION_
    }

// -----------------------------------------------------------------------------
// ChspsThemeServer::RunAutoInstaller
// (other items were commented in a header).
// -----------------------------------------------------------------------------
//
TBool ChspsThemeServer::RunAutoInstaller( TAutoInstallerStates aState )
    {
#ifndef __DISABLE_SISX_INSTALLATION_                       
    TBool started( EFalse );
    
    if( !iAutoInstaller )
        {
        TRAP_IGNORE( iAutoInstaller = ChspsAutoInstaller::NewL( *this ) );
#ifdef HSPS_LOG_ACTIVE        
        if ( iAutoInstaller )
            {           
            iAutoInstaller->SetLogBus( iLogBus );
            }
#endif
        }
    
    iAutoInstallerState = aState;
    
    switch( aState )
        {
        case EAutoInstallerStateInstall:       
            {
            if( iCurrentManifestFile < iManifestFiles.Count() )
                {
                TInt ret = KErrNone;

                TRAPD( err, ret = iAutoInstaller->InstallThemeL( *iManifestFiles[ iCurrentManifestFile ] ) ); 
                if( err == KErrNone && ret == KErrNone )
                    {                                       
                    started = ETrue;
                    }    
                }
            }
            break;
        case EAutoInstallerUninstall:
            {
            if( iThemeIndexToRemove != KErrNotFound && iThemeIndexToRemove < iInstalledSisxThemes.Count() )
                {                                   
                TRAPD( err, iAutoInstaller->UnInstallThemeL( *iInstalledSisxThemes[ iThemeIndexToRemove ] ) )

                if( !err )                    
                    {
                    started = ETrue;
                    }
                }
            }
            break;
        default:    
            {
            iAutoInstallerState = EAutoInstallerStateIdle;
            }
            break;                        
        }
        
    if( !started )
        {
        StopAutoInstaller();
        }
    
    return started;
#else
    return EFalse;    
#endif // __DISABLE_SISX_INSTALLATION_    
    }

// -----------------------------------------------------------------------------
// ChspsThemeServer::StopAutoInstaller
// (other items were commented in a header).
// -----------------------------------------------------------------------------
//
void ChspsThemeServer::StopAutoInstaller()
    {        
#ifndef __DISABLE_SISX_INSTALLATION_    
    switch( iAutoInstallerState )
        {
        case EAutoInstallerStateInstall:
            {
            iCurrentManifestFile = 0;
            iInstallableSisxThemes.Reset();
            iManifestFiles.ResetAndDestroy();
            }
            break;
        case EAutoInstallerUninstall:
            {
            iThemeIndexToRemove = KErrNotFound;            
            }
            break;
        default:
            break;            
        }
        
    iAutoInstallerState = EAutoInstallerStateIdle;          
    
    delete iAutoInstaller;
    iAutoInstaller = NULL;
#endif // __DISABLE_SISX_INSTALLATION_
    }

// -----------------------------------------------------------------------------
// ChspsThemeServer::CheckInstalledPluginUsageL
// -----------------------------------------------------------------------------
//
TInt ChspsThemeServer::CheckInstalledPluginUsageL( const TInt aPluginUid )        
    { 
    TInt sum = 0;
    if (  iHeaderListCache )
        {
        TInt count = iHeaderListCache->Count();
        if ( iDefinitionRepository->Locked() )
            {
            User::Leave( KErrInUse );
            }
        iDefinitionRepository->Lock();
        for ( TInt i = 0; i < count; i++ )
            {
            ChspsODT* header = ( *iHeaderListCache )[ i ];
            if ( header->ConfigurationType() == EhspsAppConfiguration )
                {
                
                ChspsODT* odt = ChspsODT::NewL();
                CleanupStack::PushL( odt );
                odt->SetRootUid( header->RootUid() );
                odt->SetThemeUid( header->ThemeUid() );        
                odt->SetConfigurationType( header->ConfigurationType() );
                odt->SetRootUid( header->RootUid() );
                odt->SetProviderUid( header->ProviderUid() );
                odt->SetThemeUid( header->ThemeUid() );
                odt->SetProviderNameL( header->ProviderName() );
                odt->SetThemeFullNameL( header->ThemeFullName() );
                odt->SetThemeShortNameL( header->ThemeShortName() );
                odt->SetThemeVersionL( header->ThemeVersion() );            
                odt->SetDescriptionL( header->Description() );
                odt->SetFlags( header->Flags() );                                                                       
                odt->SetMultiInstance( header->MultiInstance() );
                User::LeaveIfError( iDefinitionRepository->GetOdtL( *odt ) );
                TInt count = 0;
                hspsServerUtil::PluginInstanceCountL( *odt, aPluginUid, count );
                sum = sum + count;
                CleanupStack::PopAndDestroy( odt );
                }
            }
        iDefinitionRepository->Unlock();
        
        }
    return sum;
    }

// -----------------------------------------------------------------------------
// ChspsThemeServer::PluginInHeaderCache
// -----------------------------------------------------------------------------
//
TBool ChspsThemeServer::PluginInHeaderCache( const TUid aPluginUid )        
    { 
    TBool found = EFalse;
    
    if ( iHeaderListCache )
        {        
        const TInt count = iHeaderListCache->Count();
        for ( TInt i = 0; i < count; i++ )
            {
            ChspsODT* header = (*iHeaderListCache)[i];
            if( header->ThemeUid() == aPluginUid.iUid )
                {
                found = ETrue;
                break;
                }
            }                
        }
    
    return found;
    }

// -----------------------------------------------------------------------------
// ChspsThemeServer::UpdateHeaderListCacheL
// -----------------------------------------------------------------------------
//
void ChspsThemeServer::UpdateHeaderListCacheL()
    {
    if ( iHeaderListPending )
        {
#ifdef HSPS_LOG_ACTIVE        
        iLogBus->LogText( _L( "ChspsThemeServer::UpdateHeaderListCacheL(): already updating the cache" ) );
#endif                
        }
    else
        {
        iHeaderListPending = ETrue;
        
#ifdef _hsps_PERFORMANCE_TEST_
        TTime start_time;        
        start_time = ChspsTimeMon::StartTiming(_L("timer started")); 
#endif    
        
        // Get headers from the Plug-in Repository
        CArrayPtrSeg<ChspsODT>* headerList =  new (ELeave)CArrayPtrSeg<ChspsODT>(KHeaderListGranularity);
        CleanupStack::PushL( headerList );
                
        // Reset search mask
        iCacheMask->SetRootUid( 0 );
        iCacheMask->SetProviderUid( 0 );
        iCacheMask->SetThemeUid( 0 );
        
        TInt err = DoUpdateHeaderListCache( *headerList );
        User::LeaveIfError( err );
                                
        if ( headerList->Count() > 0 )
            {
            // Destroys all objects whose pointers form the elements of the array, 
            // before resetting the array.
            iHeaderListCache->ResetAndDestroy();
                        
            TInt count = headerList->Count() - 1;
            for (TInt i = count; i >= 0; --i)
                {
                // Update the cache array (ownership is transferred)
                TRAP( err, iHeaderListCache->InsertL(0, headerList->At(i)) );
                                                   
                // If allocation failed
                if ( err )
                    {
                    break;
                    }
                
                // Complete the ownership transfer
                headerList->Delete(i);                        
                }
            }

        // Destroys all objects whose pointers form the elements of the array, 
        // before resetting the array.
        headerList->ResetAndDestroy();
        CleanupStack::PopAndDestroy( headerList );

        // calculating service time
#ifdef _hsps_PERFORMANCE_TEST_        
        ChspsTimeMon::StopTiming( start_time, _L( "ChspsThemeServer::UpdateHeaderListCache(): - timer stopped" ) );
#endif       
                        
        iHeaderListPending = EFalse;
        }      
    }

// -----------------------------------------------------------------------------
// ChspsThemeServer::UpdateHeaderListCache
// -----------------------------------------------------------------------------
//
TInt ChspsThemeServer::UpdateHeaderListCache(    
        ThspsCacheOperations aOperation,
        const TInt aRootUid,
        const TInt aProviderUid,
        const TInt aConfigurationUid )
    {
    TInt err = KErrNone;
    
    // Input validation
    if ( aRootUid < 1 || aProviderUid < 1 || aConfigurationUid < 1 )
        {
        err = KErrArgument;
        }    
            
    if ( !err )
        {
        if ( aOperation == EhspsCacheAddHeader )
            {
            // Add or replace a header in the header list cache
            TRAP( 
                err, 
                HeaderCacheAddL(
                    aRootUid, 
                    aProviderUid, 
                    aConfigurationUid ) 
                );
            }
        else
            {
            // Remove a header from the header list cache
            err = HeaderCacheRemove( 
                    aRootUid, 
                    aProviderUid, 
                    aConfigurationUid 
                    );            
            }   
        }
    
    if ( err )
        {
#ifdef HSPS_LOG_ACTIVE                     
        iLogBus->LogText( 
            _L( "ChspsThemeServer::UpdateHeaderListCache(): - Failed to update %d header in the cache!" ), 
            aConfigurationUid 
            );
#endif            
        }
    
    return err;
    }

// -----------------------------------------------------------------------------
// ChspsThemeServer::DoUpdateHeaderListCache
// -----------------------------------------------------------------------------
//
TInt ChspsThemeServer::DoUpdateHeaderListCache(
        CArrayPtrSeg<ChspsODT>& aHeaderList )       
    {              
    // Get headers matching the search mask
    TRAPD( err, GetHeaderListL( aHeaderList, *iCacheMask ) );
    if ( err )
    	{
#ifdef HSPS_LOG_ACTIVE                     
        iLogBus->LogText( 
            _L( "ChspsThemeServer::DoUpdateHeaderListCache(): - failed %d"),
            err
            );
#endif
    	}
    
    return err;         
    }

// -----------------------------------------------------------------------------
// ChspsThemeServer::HeaderCacheAddL
// -----------------------------------------------------------------------------
//
void ChspsThemeServer::HeaderCacheAddL(
        const TInt aRootUid,
        const TInt aProviderUid,
        const TInt aConfigurationUid )
    {        
    // Setup a search mask    
    iCacheMask->SetRootUid( aRootUid );
    iCacheMask->SetProviderUid( aProviderUid );
    iCacheMask->SetThemeUid( aConfigurationUid );
    
    // Setup a list for headers matching the search mask
    CArrayPtrSeg<ChspsODT>* newHeadersList =  new (ELeave)CArrayPtrSeg<ChspsODT>(KHeaderListGranularity);
    CleanupStack::PushL( newHeadersList );
            
    // Find the headers
    User::LeaveIfError( DoUpdateHeaderListCache( *newHeadersList ) );
    
    // There should be only one match 
    if ( newHeadersList->Count() != 1 )
        {
#ifdef HSPS_LOG_ACTIVE        
        iLogBus->LogText( 
            _L( "ChspsThemeServer::HeaderCacheAddL(): - same %S header was found several times!" ), 
            &(newHeadersList->At(0)->ThemeShortName()) 
            );        
#endif        
        User::Leave( KErrGeneral );
        }    
            
    // Set new default position 
    TInt newIndex = 0;
    
    // Check if old version of the header instance exists in the cache
    TBool replaced = EFalse;
    ChspsODT* header = NULL;
    for( TInt index=0; !replaced && index < iHeaderListCache->Count(); index++ )
        {
        header = iHeaderListCache->At( index );
        if ( header )
            {
            if ( header->RootUid() == aRootUid 
                    && header->ProviderUid() == aProviderUid 
                    && header->ThemeUid() == aConfigurationUid )
                {
                // Remove the old header instance
                iHeaderListCache->Delete( index );
                delete header;
                header = NULL;
                replaced = ETrue;
                newIndex = index;
                }
            }
        }
    
    // Append the new version into the specified position (ownership is transferred)    
    iHeaderListCache->InsertL(newIndex, newHeadersList->At(0) );

#ifdef HSPS_LOG_ACTIVE        
    iLogBus->LogText( _L( "ChspsThemeServer::HeaderCacheAddL(): - header '%S' was added/updated to cache!" ), &(newHeadersList->At(0)->ThemeShortName()) );        
#endif    
    
    // Complete the ownership transfer 
    newHeadersList->Delete(0);      
        
    // Cleanup
    newHeadersList->ResetAndDestroy();
    CleanupStack::PopAndDestroy( newHeadersList );        
    }

// -----------------------------------------------------------------------------
// ChspsThemeServer::HeaderCacheRemove
// -----------------------------------------------------------------------------
//
TInt ChspsThemeServer::HeaderCacheRemove(            
        const TInt aRootUid,
        const TInt aProviderUid,
        const TInt aConfigurationUid )
    {
    TInt err = KErrNone;
    
    // Find the header to be removed from the cache        
    ChspsODT* header = NULL;
    for( TInt index=0; index < iHeaderListCache->Count(); index++ )
        {
        header = iHeaderListCache->At( index );
        if ( header )
            {
            if ( header->RootUid() == aRootUid 
                    && header->ProviderUid() == aProviderUid 
                    && header->ThemeUid() == aConfigurationUid )
                {
#ifdef HSPS_LOG_ACTIVE        
                iLogBus->LogText( _L( "ChspsThemeServer::HeaderCacheRemove(): - header '%S' was removed from cache!" ), &(header->ThemeShortName()) );        
#endif                               
                iHeaderListCache->Delete( index );
                delete header;
                header = NULL;
                break;
                }
            }
        }
    
    return err;   
    }

// -----------------------------------------------------------------------------
// ChspsThemeServer::CompareL
// (other items were commented in a header).
// -----------------------------------------------------------------------------
//
TBool ChspsThemeServer::ComparePaths(const ChspsODT& aOldHeader, const ChspsODT& aNewHeader)
  {
  TBool ret;
  if  (
        (
        aOldHeader.RootUid() == aNewHeader.RootUid()
        )
    &&
        (
        aOldHeader.ProviderUid() == aNewHeader.ProviderUid()
        )
    &&
        (
        aOldHeader.ThemeUid() == aNewHeader.ThemeUid()
        )       
    &&
        (
        !aOldHeader.ThemeVersion().Compare( aNewHeader.ThemeVersion())
        )
    )
	  {
	  ret = ETrue;
	  }
  else
	  {
	  ret = EFalse;
	  }
  return ret; 
  }
  
// -----------------------------------------------------------------------------
// ChspsThemeServer::GetDeviceLanguage()
// (other items were commented in a header).
// -----------------------------------------------------------------------------
//
TLanguage ChspsThemeServer::GetDeviceLanguage()
    {
    return User::Language();
    } 
	
// -----------------------------------------------------------------------------
// ChspsThemeServer::GetHeaderListL
// (other items were commented in a header).
// -----------------------------------------------------------------------------
//
void ChspsThemeServer::GetHeaderListL(CArrayPtrSeg<ChspsODT>& aHeaderList, const ChspsODT& aSearchMask )    
    {
    CArrayPtrSeg<ChspsODT>* licenceeDefaultList = new (ELeave)CArrayPtrSeg<ChspsODT>( KHeaderListGranularity );
    CleanupStack::PushL( TCleanupItem( ResetAndDestroyChspsODTArray, licenceeDefaultList ) );
    
    CArrayPtrSeg<HBufC8>* headerDataList = new (ELeave)CArrayPtrSeg<HBufC8>( KHeaderListGranularity );
    CleanupStack::PushL( TCleanupItem( ResetAndDestroyHBufC8Array, headerDataList ) );
    iDefinitionRepository->GetThemeListAsStreamL( *headerDataList, aSearchMask );
        
    // Convert data streams into header instances and populate the lincencee and header lists    
    for ( TInt i=0; i < headerDataList->Count(); i++ )
        {
        HBufC8* headerData = headerDataList->At(i);
        if ( headerData )
            {
            ChspsODT* header = ChspsODT::UnMarshalHeaderLC( *headerData );                        
            if ( header->Flags() & EhspsThemeStatusLicenceeDefault )
                {                
                licenceeDefaultList->AppendL( header );
                }
            else
                {
                // Get header's file path
                HBufC* fileBuf = HBufC::NewLC ( KMaxFileName );
                TPtr filePtr = fileBuf->Des();    
                iDefinitionRepository->GetResourcePathL( *header, EResourceODT, filePtr );
                aHeaderList.AppendL( header );                 
                CleanupStack::PopAndDestroy( fileBuf ); 
                }        
            CleanupStack::Pop( header );  
            }
        }
    
    // Cleanup the data streams
    CleanupStack::PopAndDestroy( headerDataList );            
        

    // remove duplicate headers from the licencee list  
    if( aHeaderList.Count() )
        {
        // go through the theme list again and put back the themes without update
        const TInt headersCount = aHeaderList.Count();        
        for( TInt i = 0; i < licenceeDefaultList->Count(); i++ )
            {
            ChspsODT* licenceeHeader = licenceeDefaultList->At( i );
            
            TBool found( EFalse );            
            for( TInt j = 0; !found && j < headersCount; j++ )
                {
                ChspsODT* header = aHeaderList.At( j );
                
                // compare two themes to find out are they they the same
                if( ComparePaths( *header, *licenceeHeader ) )
                    {
                    // there was an update, this theme does not need licencee default
                    licenceeDefaultList->Delete( i );
                    delete licenceeHeader;
                    // next i
                    i--;
                    found = ETrue;
                    }
                }
            }       
        }

    // append headers from the licencee list into the headers list
    for ( TInt i = 0; i < licenceeDefaultList->Count(); i++)
        {
        ChspsODT* licenceeHeader = licenceeDefaultList->At(i);
        
        HBufC8* data = licenceeHeader->MarshalHeaderL();
        CleanupStack::PushL( data );
        
        ChspsODT* odt = ChspsODT::UnMarshalHeaderLC( data->Des() );
        aHeaderList.AppendL( odt );
        CleanupStack::Pop( odt );
        
        CleanupStack::PopAndDestroy( data );
        }
        
    CleanupStack::PopAndDestroy( licenceeDefaultList );
            
    // Modify headers flags     
    for ( TInt i = 0; i < aHeaderList.Count(); i++ )
        {        
        ChspsODT* header = aHeaderList.At(i);
        TInt appUid = header->RootUid();
        if ( header->ConfigurationType() == EhspsAppConfiguration )
            {        
            // Check whether the application has an active configuration
            TInt confUid = 0;            
            if ( iCentralRepository->Get( appUid, confUid ) == KErrNone && confUid > 0 )
                {
                // If active application configuration is the same as the header in question 
                if ( header->ThemeUid() == confUid )
                    {
                    // Update header's flags
                    TUint flags = header->Flags();
                    header->SetFlags( flags | EhspsThemeStatusActive );
                    }
                }
            }
        }
 
    }
    	
	
// -----------------------------------------------------------------------------
// ChspsThemeServer::CustomSecurityCheckL
// (other items were commented in a header).
// -----------------------------------------------------------------------------
//
CPolicyServer::TCustomResult ChspsThemeServer::CustomSecurityCheckL(const RMessage2& aMsg, 
    TInt& /*aAction*/, TSecurityInfo& /*aMissing*/ )
    {
    CPolicyServer::TCustomResult ret;
    if ( iSecurityEnforcer->CheckAccessRightsL( aMsg ) )
        {
        ret = EPass;
        }
    else
        {
        ret = EFail;
        }
    return ret;
    }

// -----------------------------------------------------------------------------
// ChspsThemeServerSession::HandleThemeUpgradeL
// (other items were commented in a header).
// -----------------------------------------------------------------------------
//
void ChspsThemeServer::HandleThemeUpgradeL()
	{
	CDir* upgradePathFileList( NULL );
    TFindFile upgradeFileFinder( iFsSession );
    
    // If a upgrade_path_*.dat file appears in the import directory this means that a theme is being upgraded
    upgradeFileFinder.FindWildByDir( KUpgradePluginWild , 
    		KImportDirectoryC, upgradePathFileList );  
    if( !upgradePathFileList )
    	{
    	return;
    	}
	// only one upgrade_path file can be present because it is deleted after upgrade
	if ( upgradePathFileList->Count() == 1 )
		{
		CleanupStack::PushL( upgradePathFileList );
		TPtrC upgradePathFile( ( *upgradePathFileList )[0].iName ); 

		//path_*.dat theme file
		HBufC* fileNamePathOnly = HBufC::NewLC( KMaxFileName );
		TPtr fileNamePathOnlyPtr( fileNamePathOnly->Des() );
		TParsePtrC parsePtrTemp( upgradePathFile.Right( 
			upgradePathFile.Length() - KUpgrade().Length() - 1 ) );  
		fileNamePathOnlyPtr.Append( parsePtrTemp.FullName() ); 
		
		TBool isInstalled = EFalse;
    	//check if theme is installed
    	for (TInt i = 0; i < iInstalledSisxThemes.Count(); i++)
    		{
    		if ( *iInstalledSisxThemes[i] == *fileNamePathOnly )
    			{
    			isInstalled = ETrue;
    			break;
    			}
    		}
    	
    	if ( !isInstalled )
			{
			// base theme package is not an installed theme
#ifdef HSPS_LOG_ACTIVE			
	        iLogBus->LogText( _L( "hspsThemeServer: ChspsThemeServer::HandleThemeUpgradeL(): Base theme package not installed" ) );
#endif			        
			}
    	else
			{
			//upgrade_path_*.dat file path portion
    		TParsePtrC parsePtr( upgradePathFile.Right( 
				upgradePathFile.Length() - KUpgradePlugin().Length() - 1 ) );
    		TPtrC fileNameUpgradePath( parsePtr.Name() );
    		
            //path to manifest
    		HBufC* manifestFile = HBufC::NewLC( KMaxFileName );
            TPtr manifestFilePtr( manifestFile->Des() );
            manifestFilePtr.Append( KImportDirectoryC );
            manifestFilePtr.Append( fileNameUpgradePath );
                                                                                                                               
            TInt pos( manifestFilePtr.Locate( TChar( '_' ) ) );
            while( pos != KErrNotFound )
                {
                manifestFilePtr.Replace( pos, 1, KDoubleBackSlash );
                pos = manifestFilePtr.Locate( TChar( '_' ) );                
                }
                   
            manifestFilePtr.Append( KDoubleBackSlash );            
            manifestFilePtr.Append( KManifestFile );           
            
            //store manifest file for autoinstaller
            iManifestFiles.AppendL( manifestFile );

            //Store current path_*.dat file
            iInstallableSisxThemes.AppendL( ( *fileNamePathOnly ).AllocL() );
            iCurrentManifestFile = 0;
    		
    		if( !RunAutoInstaller( EAutoInstallerStateInstall ) )
	            {
	            // All installation files are not present
	            // here should be some kind of error handling
	            }  	

    		CleanupStack::Pop( manifestFile ); //manifestFile
			}
		
		HBufC * upgradePathFileToDelete = HBufC::NewLC( KMaxFileName );
		TPtr upgradePathFileToDeletePtr( upgradePathFileToDelete->Des() );
		upgradePathFileToDeletePtr.Append( KImportDirectoryC );
		upgradePathFileToDeletePtr.Append( upgradePathFile );
		
		//delete the upgrade_path_*.dat auxiliary file
		TInt err = iFsSession.Delete( *upgradePathFileToDelete );
		if ( err != KErrNone )
			{
#ifdef HSPS_LOG_ACTIVE
			//file not deleted error handling			
			iLogBus->LogText( _L( "hspsThemeServer: ChspsThemeServer::HandleThemeUpgradeL(): %d ,upgrade_path_*.dat file not deleted" ), err );
#endif			                
			}
		
		CleanupStack::PopAndDestroy( upgradePathFileToDelete );
		CleanupStack::PopAndDestroy( fileNamePathOnly );
		CleanupStack::PopAndDestroy( upgradePathFileList );	
		}
	else 
		{
#ifdef HSPS_LOG_ACTIVE		
		//more than one upgrade_path is present. in import directory error handling
        iLogBus->LogText( _L( "hspsThemeServer: ChspsThemeServer::HandleThemeUpgradeL(): more than one upgrade_path_*.dat file" ) );
#endif		        
		}
	}

// -----------------------------------------------------------------------------
// Checks whether the device language has been changed and updates 
// configurations accordingly
// -----------------------------------------------------------------------------
//
void ChspsThemeServer::HandleLanguageChangeL()
	{
#ifdef HSPS_LOG_ACTIVE          
        iLogBus->LogText( _L( "ChspsThemeServer::HandleLanguageChangeL()") );
#endif
        
	// Set key for fetching previously used language from the cenrep
	const TInt KCenrepLangKey = 100000001;
	
	// The key should already exist
	const TUint32 fullMask = 0xFFFFFFFF;
	RArray<TUint32> res;	
	CleanupClosePushL( res );
	
	iCentralRepository->FindL( KCenrepLangKey, fullMask, res );
   	if ( res.Count() == 0 )
   		{
#ifdef HSPS_LOG_ACTIVE   		
   		iLogBus->LogText( _L( "ChspsThemeServer::HandleLanguageChangeL(): - Couldn't find %d key from the HSPS cenrep!" ), KCenrepLangKey );
#endif   		             

   		User::Leave( KErrNotFound );
   		}	
	
   	// Check the cenrep key's value
	TInt prevLanguage = ELangNone;
    iCentralRepository->Get( KCenrepLangKey, prevLanguage );
    
    // If the language has been changed
    if( iDeviceLanguage != prevLanguage )
    	{
    	// Update all the ODTs so that the localized configurations can be fetched    	
    	LocalizeConfigurationsL();
    		
    	// Finally, update the cenrep with the new language
    	TInt errorCode = iCentralRepository->Set( KCenrepLangKey, iDeviceLanguage );
    	if ( errorCode  )
    		{
#ifdef HSPS_LOG_ACTIVE    		
	        iLogBus->LogText( _L( "ChspsThemeServer::HandleLanguageChangeL(): - Failed to update the cenrep!" ) );
#endif    		        

    		User::Leave( KErrWrite );
    		}
    	else
    		{
#ifdef HSPS_LOG_ACTIVE    		
	        iLogBus->LogText( _L( "ChspsThemeServer::HandleLanguageChangeL(): - themes were localized successfully" ) );
#endif    		        
    		}
    	}
    
    res.Close();  
    CleanupStack::PopAndDestroy(1, &res);    
	}


// -----------------------------------------------------------------------------
// ChspsThemeServer::SetHeadersLanguages()
// -----------------------------------------------------------------------------
//
void ChspsThemeServer::LocalizeConfigurationsL()
	{
#ifdef HSPS_LOG_ACTIVE              
    iLogBus->LogText( _L( "ChspsThemeServer::LocalizeConfigurationsL()" ) );
#endif	
	
	// Check header count 	
	TInt headerCount = HeaderListCache().Count();	
	if( headerCount )
		{				
		_LIT8(KhspsDefinitionEngine, "hspsdefinitionengine");		
		ChspsDefinitionEngineInterface* defEngine = ChspsDefinitionEngineInterface::NewL(KhspsDefinitionEngine);
		CleanupStack::PushL( defEngine );
			
		// Loop found headers
		ChspsODT* header = NULL;
		const TInt KInterfaceUid = 0; 
		for( TInt headerIndex=0; headerIndex < headerCount; headerIndex++ )
			{			
			// Get pointer to the header (contains minimal information from an ODT)
			header = iHeaderListCache->At( headerIndex );
									
			// Get the ODT based on header UIDs
			ChspsODT* odt = ChspsODT::NewL();
			CleanupStack::PushL( odt );		
			GetConfigurationL(
				KInterfaceUid,
				header->ThemeUid(),
				*odt );		
			if ( !odt || !odt->ThemeUid() )
	        	{
#ifdef HSPS_LOG_ACTIVE	        	
       	        iLogBus->LogText( _L( "ChspsThemeServer::LocalizeConfigurationsL(): - Failed to retrieve header with an configuration uid %d" ),
       	                header->ThemeUid() );
#endif	        	                
	        	                
	        	User::Leave( KErrNotFound );
	        	}
			
			// Try to localize the ODT according to the new device language
			TBool localized = LocalizeL(
				*defEngine,
				*odt );
			if ( localized )
				{
				// Set header's name from the ODT
				header->SetThemeFullNameL( odt->ThemeFullName() );
				header->SetDescriptionL( odt->Description() );
				}
			
			CleanupStack::PopAndDestroy( odt );			
			} // for loop		
		
		CleanupStack::PopAndDestroy( defEngine );
		
		} // count
	}

// -----------------------------------------------------------------------------
// ChspsThemeServer::LocalizeL()
// -----------------------------------------------------------------------------
//
TBool ChspsThemeServer::LocalizeL( 
		ChspsDefinitionEngineInterface& aEngine,
		ChspsODT& aOdt )
	{		  			
	TBool localized = ETrue;
	iFsSession.SetSessionToPrivate( EDriveC );
	
	TLanguage requestedLanguage = DeviceLanguage();
	if ( requestedLanguage == ELangNone )
	    {
	    requestedLanguage = ELangTest;
	    }		
	
    if ( aOdt.OdtLanguage() != requestedLanguage )
        {
        localized = EFalse;
        TBool domChanged = EFalse;
        	
    	// Localize XML configuration with the requested language
    	if ( aOdt.ConfigurationType() == EhspsAppConfiguration )
    	    {	        
    	    // Apply plugin and application specific DTD files 
    	    domChanged = LocalizeApplicationConfigurationL( 
    	            aEngine, 
    	            aOdt,
    	            requestedLanguage );
    	    }
    	else
    	    {	    
    	    // Apply plugin specific DTD files only
    	    domChanged = LocalizePluginConfigurationL( 
                    aEngine, 
                    aOdt,
                    requestedLanguage,
                    0 );    	
    	    }
    	
    	if ( domChanged )
    		{
    		// Update ODT with the new localized name from the XML configuration
    		TPtrC8 name( hspsServerUtil::FindConfigurationAttrL( aOdt, KConfigurationAttrName ) );
    		if ( name.Length() )
    			{					
    			HBufC* buf = HBufC::NewLC( name.Length() );
    			buf->Des().Copy( name );
    			aOdt.SetThemeFullNameL( buf->Des() );
    			CleanupStack::PopAndDestroy( buf );
    
    			// The ODT instance has now been localized - either with the requested language or a test language
    			aOdt.SetOdtLanguage( requestedLanguage );    			    			
    			}
    		
    		// Update ODT with the new localized description from the XML configuration
    		TPtrC8 desc( hspsServerUtil::FindConfigurationAttrL( aOdt, KConfigurationAttrDesc ) );
            if ( desc.Length() )
                {
                HBufC* buf = HBufC::NewLC( desc.Length() );
                buf->Des().Copy( desc );
                aOdt.SetDescriptionL( buf->Des() );
                CleanupStack::PopAndDestroy( buf );                
                }
            
            // Save the changes
            iDefinitionRepository->SetOdtL( aOdt );                     
            localized = ETrue;
    		}    	    	
    	
    	if ( !localized )
    		{
#ifdef HSPS_LOG_ACTIVE		
            iLogBus->LogText( 
                    _L( "ChspsThemeServer::LocalizeL(): - failed to localize header %S!" ),
                    &aOdt.ThemeShortName() 
                    );
#endif		        
    		}
    
#ifdef HSPS_LOG_ACTIVE          
        iLogBus->LogText( _L( "ChspsThemeServer::LocalizeL() - DOM dump after localization:" ) );                                       
        // Print DOM debug
        ChspsOdtDump::Dump( aOdt, *iLogBus );
#endif          
        }
    
	return localized;
	}

// -----------------------------------------------------------------------------
// ChspsThemeServer::LocalizeApplicationConfigurationL()
// -----------------------------------------------------------------------------
//
TBool ChspsThemeServer::LocalizeApplicationConfigurationL(
        ChspsDefinitionEngineInterface& aEngine,
        ChspsODT& aOdt,
        const TLanguage aRequestedLanguage )
    {    
#ifdef HSPS_LOG_ACTIVE      
    iLogBus->LogText( 
        _L( "ChspsThemeServer::LocalizeApplicationConfigurationL(): - Localizing %S application configuration..." ), 
        &aOdt.ThemeShortName() 
        );
#endif    
    
    // Localize the application configuration itself
    TInt resourceFilter = aOdt.ThemeUid(); 
    TBool domLocalized = LocalizePluginConfigurationL(
            aEngine,
            aOdt,
            aRequestedLanguage,
            resourceFilter );    
    
    // Next, localize all plugins instances in the DOM
    if ( domLocalized )
        {
        // Find all unique plugin uids that are used in the application configuration 
        RArray<TInt> pluginArray;
        CleanupClosePushL( pluginArray );            
        hspsServerUtil::FindUniquePluginsL( aOdt, pluginArray );
        
        // Loop the plugins
        const TInt pluginsCount = pluginArray.Count();
        for( TInt pluginIndex=0; domLocalized && pluginIndex < pluginsCount; pluginIndex++ )
            {
            // Apply plugin specific DTD files only
            TBool pluginLocalized = LocalizePluginConfigurationL(
                    aEngine,
                    aOdt,
                    aRequestedLanguage,
                    pluginArray[pluginIndex] );                    
            }
        
        pluginArray.Reset();
        CleanupStack::PopAndDestroy( &pluginArray );
        }
    
    return domLocalized;
    }

// -----------------------------------------------------------------------------
// ChspsThemeServer::LocalizePluginConfigurationL()
// -----------------------------------------------------------------------------
//
TBool ChspsThemeServer::LocalizePluginConfigurationL(
        ChspsDefinitionEngineInterface& aEngine,
        ChspsODT& aOdt,
        const TLanguage aRequestedLanguage,
        TInt aPluginUid )
    {               
    // If the plugin has already been localized as requested
    if ( aOdt.OdtLanguage() == aRequestedLanguage )
        {
        return ETrue;
        }
    
    TBool domLocalized = EFalse;
    const TInt resourceCount = aOdt.ResourceCount();
    
    // Localize XML document         
    TFileName pluginFolder;
    for( TInt resourceIndex=0; !domLocalized && resourceIndex < resourceCount; resourceIndex++ )
        {                       
        ChspsResource& r = aOdt.ResourceL( resourceIndex );        
        if ( r.ResourceType() == EResourceDTD )
            {
            TBool isPluginSpecific = ETrue;
            
            // If plugin uid restriction was set (localizing an application configuration)
            if ( aPluginUid > 0 )
                {       
                // Check whether the resource belongs to the plugin 
                pluginFolder.Format( _L("\\%d\\"), aPluginUid );
                isPluginSpecific = ( r.FileName().Find(pluginFolder) > 0 );                    
                }            
            if ( isPluginSpecific && r.Language() == aRequestedLanguage )             
                {                               
                // Parses the given DTD file and replaces all entity referencies in the DOM tree            
                aEngine.ApplyLanguageL( 
                    iFsSession, 
                    r.FileName(), 
                    aOdt );                                                             
                domLocalized = ETrue;
#ifdef HSPS_LOG_ACTIVE
                if ( aPluginUid > 0 )
                    {
                    iLogBus->LogText( 
                        _L( "ChspsThemeServer::LocalizePluginConfigurationL(): - %S/%X instances localized to %d language!" ), 
                        &aOdt.ThemeShortName(),
                        aPluginUid,
                        aRequestedLanguage );                   
                    }
                else
                    {
                    iLogBus->LogText( 
                        _L( "ChspsThemeServer::LocalizePluginConfigurationL(): - %S localized to %d language!" ), 
                        &aOdt.ThemeShortName(),                        
                        aRequestedLanguage );
                    }                
#endif                
                }                               
            }
        }   

    // If the requested locale couldn't be found   
    if ( !domLocalized )
        {                                                
        // Localize XML document with the test locale (mandatory language, used by HSPS developers)
        for( TInt resourceIndex=0; !domLocalized && resourceIndex < resourceCount; resourceIndex++ )
            {
            ChspsResource& r = aOdt.ResourceL( resourceIndex );            
            if ( r.ResourceType() == EResourceDTD )
                {
                TBool isPluginSpecific = ETrue;
                
                // If plugin uid restriction was set (localizing an application configuration)
                if ( aPluginUid > 0 )
                    {       
                    // Check whether the resource belongs to the plugin 
                    pluginFolder.Format( _L("\\%d\\"), aPluginUid );
                    isPluginSpecific = ( r.FileName().Find(pluginFolder) > 0 );                    
                    }           
                if ( isPluginSpecific && r.Language() == ELangTest )
                    {                                
                    // Parses the given DTD file and replaces all entity referencies in the DOM tree            
                    aEngine.ApplyLanguageL( 
                        iFsSession, 
                        r.FileName(), 
                        aOdt );
                    domLocalized = ETrue;
#ifdef HSPS_LOG_ACTIVE      
                    if ( aPluginUid > 0 )
                        {
                        iLogBus->LogText( 
                            _L( "ChspsThemeServer::LocalizePluginConfigurationL(): - %S/%X instances localized to %d language!" ), 
                            &aOdt.ThemeShortName(),
                            aPluginUid,
                            ELangTest );                   
                        }
                    else
                        {
                        iLogBus->LogText( 
                            _L( "ChspsThemeServer::LocalizePluginConfigurationL(): - %S localized to %d language!" ), 
                            &aOdt.ThemeShortName(),                        
                            ELangTest );
                        }
#endif                                                
                    }            
                }
            }           
        }    
                
    return domLocalized;
    }

// -----------------------------------------------------------------------------
// ChspsThemeServer::GetConfigurationL()
// -----------------------------------------------------------------------------
//
TInt ChspsThemeServer::GetConfigurationL(
		const TInt aInterfaceUid,
		const TInt aConfigurationUid,
		ChspsODT& aRetrievedODT )
	{			
	// Input validation
	if ( aInterfaceUid < 0 || aConfigurationUid < 1)
		{
#ifdef HSPS_LOG_ACTIVE		
		iLogBus->LogText( _L( "ChspsThemeServer::GetConfigurationL(): - Invalid arguments" ) );
#endif
		
        return KErrArgument;        		
		}	
			
	TInt headerCount = HeaderListCache().Count();
    if ( headerCount < 1 )
        {
        UpdateHeaderListCacheL();        
        headerCount = HeaderListCache().Count();
        if ( headerCount < 1 )
            {
            // configurations were not found or repository is locked        
#ifdef HSPS_LOG_ACTIVE        
            iLogBus->LogText( _L( "ChspsThemeServer::GetConfigurationL(): - Cache is empty" ) );
#endif
            return KErrAbort;        	
            }
        }

#ifdef HSPS_LOG_ACTIVE        
    iLogBus->LogText( _L( "ChspsThemeServer::GetConfigurationL(): - header count in cache: %d" ), iHeaderListCache->Count() );
#endif    
    
	// Loop headers in the cache
    TBool found = EFalse;
    ChspsODT* h = NULL;
    for ( TInt i=0; !found && i<iHeaderListCache->Count(); i++ )
        {
        h = iHeaderListCache->At(i);
        TBool rootUidMatched = ETrue;
        TBool themeUidMatched = ETrue;
        if ( aInterfaceUid )
        	{
        	rootUidMatched = ( h->RootUid() == aInterfaceUid );        	
        	}       
    	themeUidMatched = ( h->ThemeUid() == aConfigurationUid );
    
        found = ( rootUidMatched && themeUidMatched );
//#ifdef HSPS_LOG_ACTIVE        
//        iLogBus->LogText( _L( "ChspsThemeServer::GetConfigurationL(): - header '%S' matched: %d" ), &(h->ThemeShortName()), found );        
//#endif        
        } 	        	       	              
    if ( !found )
        {
#ifdef HSPS_LOG_ACTIVE           
        iLogBus->LogText( _L( "ChspsThemeServer::GetConfigurationL(): - theme %D was not found!" ), aConfigurationUid );
#endif
        return KErrNotFound;        
        }

    // Copy properties which are needed by the definition repository to form up ODT paths            
    aRetrievedODT.SetRootUid( h->RootUid() );
    aRetrievedODT.SetThemeUid( h->ThemeUid() );        
    aRetrievedODT.SetConfigurationType(h->ConfigurationType());
    aRetrievedODT.SetRootUid(h->RootUid());
    aRetrievedODT.SetProviderUid(h->ProviderUid());
    aRetrievedODT.SetThemeUid(h->ThemeUid());
    aRetrievedODT.SetProviderNameL(h->ProviderName());
    aRetrievedODT.SetThemeFullNameL(h->ThemeFullName());
    aRetrievedODT.SetThemeShortNameL(h->ThemeShortName());
    aRetrievedODT.SetThemeVersionL(h->ThemeVersion());            
    aRetrievedODT.SetFlags(h->Flags());                                                                	      
    aRetrievedODT.SetMultiInstance( h->MultiInstance() );
                
    // Get a complete ODT instance from the definition repository
    if ( iDefinitionRepository->GetOdtL( aRetrievedODT ) )    
        {        
#ifdef HSPS_LOG_ACTIVE        
        iLogBus->LogText( _L( "ChspsThemeServer::GetConfigurationL(): - theme cannot be accessed." ) );
#endif                
        return KErrAccessDenied;
        }    
    
    return KErrNone;
	}

// -----------------------------------------------------------------------------
// ChspsThemeServer::InstallUDAWidgetsL()
// -----------------------------------------------------------------------------
//
void ChspsThemeServer::InstallUDAWidgetsL()
    {
    //Get list of uda dir's
    TPtrC filter( KFilterAllPluginImportsV1 );
    CDir* importDir( NULL );           
    TFindFile fileFinder( iFsSession );
    fileFinder.FindWildByDir( filter, KImportDirectoryC, importDir );    
    CleanupStack::PushL( importDir );
    
    if ( importDir && importDir->Count() > 0 )
        {
        CHSPSInstaller* installer = CHSPSInstaller::NewL( *this );
        CleanupStack::PushL( installer );
        
        for ( TInt i = 0; i < importDir->Count(); i++ )
            {
            TPtrC udaName( (*importDir)[i].iName );
            // Get manifest path
            HBufC* manifestBuf = GetManifestFromImportLC( 
                    udaName, 
                    KImportDirectoryC );
            
            //install
            TRAPD( err, installer->InstallConfigurationL( *manifestBuf ) );
            if ( err != KErrNone )
                {
#ifdef HSPS_LOG_ACTIVE                            
                iLogBus->LogText( _L( "ChspsThemeServer::InstallUDAWidgetsL(): - Installation failed" ) );                
#endif
                }
            CleanupStack::PopAndDestroy( manifestBuf );
            }
        CleanupStack::PopAndDestroy( installer );
        }
    CleanupStack::PopAndDestroy( importDir ); 
    }

// -----------------------------------------------------------------------------
// ChspsThemeServer::HandleRomInstallationsL()
// -----------------------------------------------------------------------------
//
void ChspsThemeServer::HandleRomInstallationsL()
    {    
#ifdef HSPS_LOG_ACTIVE        
        iLogBus->LogText( _L( "ChspsThemeServer::HandleRomInstallationsL()" ) );
#endif    
    
    const TInt key = KCenrepKeyPostRFS;
    const TUint32 fullMask = 0xFFFFFFFF;
    RArray<TUint32> res;
    CleanupClosePushL( res );
    iCentralRepository->FindL( key, fullMask, res );
    if ( res.Count() == 0 )
        {
        // The key should already exist!
#ifdef HSPS_LOG_ACTIVE        
        iLogBus->LogText( _L( "ChspsThemeServer::HandleRomInstallationsL(): - Couldn't find %d key from the HSPS cenrep!" ), key );
#endif                        
                        
        User::Leave( KErrNotFound );
        }   
    
    // Check the key's value
    // lets use KSysUtilVersionTextLength instead of KSysVersionInfoRevisionLength if we
    // have to use SysUtil::GetSWVersion because KSysUtilVersionTextLength is bigger.
    TBuf<KSysUtilVersionTextLength> fwVersion;
    TInt errorCode = iCentralRepository->Get( key, fwVersion );
    if( ( errorCode == KErrNone ) &&
        ( fwVersion.Length() == 0 ) )
        {
        // Install widgets from \private\200159C0\install\ directories (ROM and UDA image)
        InstallWidgetsL();

        // Install widgets from \private\200159C0\imports\ directory (UDA image)
        InstallUDAWidgetsL();
        
        // Post RFS installations have been done, prevent re-installations at next startup
        // by reading firmware version and saving it to cenrep.
        GetFWVersion( fwVersion );
        
        errorCode = iCentralRepository->Set( key, fwVersion );
        if ( errorCode  )
            {
#ifdef HSPS_LOG_ACTIVE
            iLogBus->LogText( _L( "ChspsThemeServer::HandleRomInstallationsL(): - Failed to update the cenrep!" ) );
#endif

            User::Leave( KErrWrite );
            }
        else
            {
#ifdef HSPS_LOG_ACTIVE
            iLogBus->LogText( _L( "ChspsThemeServer::HandleRomInstallationsL(): - ROM configurations were installed successfully." ) );
#endif
            }
        }
    else
        {
        // We have to check whether phone software has been updated for example by FOTA.
        // If the firmware version is different we need to run backup and restore to
        // check whether there are any new widgets introduced and check can we use
        // still same application configure. If firmware is same nothing needs to be done.
        TBuf<KSysUtilVersionTextLength> currentFWVersion;
        GetFWVersion( currentFWVersion );        
        if ( fwVersion != currentFWVersion )
            {
            // Phone software has been updated.
            CreateBackupDataL();
            InstallWidgetsL();
            RestoreApplicationConfigurationsL();
            // Save new firmware version to cenrep
            if ( errorCode == KErrNone )
                {
                errorCode = iCentralRepository->Set( key, currentFWVersion );                
                }
            else if ( errorCode == KErrArgument )
                {
                // We came here because older version of HSPS cenrep value
                // of key was integer and now it is string.
                // Delete key and create new with different type
                iCentralRepository->Delete( key );
                errorCode = iCentralRepository->Create( key, currentFWVersion );                
                }
            
            if ( errorCode  )
                {
#ifdef HSPS_LOG_ACTIVE
            iLogBus->LogText( _L( "ChspsThemeServer::HandleRomInstallationsL(): - Failed to update the cenrep!" ) );
#endif

                User::Leave( KErrWrite );
                }
            else
                {
#ifdef HSPS_LOG_ACTIVE
            iLogBus->LogText( _L( "ChspsThemeServer::HandleRomInstallationsL(): - ROM configurations were installed successfully." ) );
#endif
                }
            }
        }
    
#if defined(WINSCW) || defined(__WINS__)
    // Activate client specific root configurations from active display resolution
    ActivateRootConfigurationsL();
#endif // defined(WINSCW)    

    res.Close();    
    CleanupStack::PopAndDestroy(1, &res); 
    }

// -----------------------------------------------------------------------------
// ChspsThemeServer::InstallWidgetsL()
// -----------------------------------------------------------------------------
//
void ChspsThemeServer::InstallWidgetsL()
    {    
    __ASSERT_DEBUG( !iRomInstaller, User::Leave( KErrGeneral) );	
	iRomInstaller = ChspsRomInstaller::NewL( *this, iFsSession );	
#ifdef HSPS_LOG_ACTIVE            	
	iRomInstaller->SetLogBus( iLogBus );
#endif
		
	RPointerArray<HBufC> pluginFolders;
    CleanupClosePushL( pluginFolders );				
	        
    // Find UDA and ROM widgets to be installed     
    iRomInstaller->FindInstallationFilesL( pluginFolders );
            
	// Install the manifest files    
    for( TInt index=0; index < pluginFolders.Count(); index++ )
        {         
        TPtrC namePtr( pluginFolders[index]->Des() );                               
#ifdef HSPS_LOG_ACTIVE            
        iLogBus->LogText( _L( "ChspsThemeServer::InstallWidgetsL(): - installing configuration: %S" ), &namePtr );
#endif      
                
        // Synchronous method
        ThspsServiceCompletedMessage ret = iRomInstaller->InstallThemeL( namePtr  );
        if ( ret != EhspsInstallThemeSuccess )
            {
#ifdef HSPS_LOG_ACTIVE            
            iLogBus->LogText( _L( "ChspsThemeServer::InstallWidgetsL(): - installation failed: %S" ), &namePtr );
#endif                  
//            User::Leave( KErrAbort );
            }
        }
    
    if ( pluginFolders.Count() == 0 )
        {
#ifdef HSPS_LOG_ACTIVE            
        iLogBus->LogText( _L( "ChspsThemeServer::InstallWidgetsL(): - mandatory plugins were not found!" ) );
#endif                                  
        // Mandatory plugins were missing 
        User::Leave( KErrCorrupt );
        }
        
    pluginFolders.ResetAndDestroy();
    CleanupStack::PopAndDestroy( 1, &pluginFolders );
		
	// The ROM installer is not needed anymore and therefore it can be released
	delete iRomInstaller;
	iRomInstaller = 0;
	
	// Force updating of the header cache
    ThspsRepositoryInfo info( EhspsCacheUpdate );
    iDefinitionRepository->RegisterNotification( info );
	}

// -----------------------------------------------------------------------------
// ChspsThemeServer::GetConfigurationHeader()
// -----------------------------------------------------------------------------
//
void ChspsThemeServer::GetConfigurationHeader(
    ChspsODT& aSearchMask,
    ChspsODT*& aConfHeader,
    TInt& aPos )
    {
    ChspsODT* header( NULL );
    TBool rootUidMatch( ETrue );
    TBool themeUidMatch( ETrue );
    TBool flagsMatch( ETrue );
    TBool typeMatch( ETrue );
    aConfHeader = NULL;
    for ( TInt i = aPos; i < iHeaderListCache->Count() && aConfHeader == NULL; i++ )
        {
        header = iHeaderListCache->At(i);
        if ( aSearchMask.RootUid() && 
             aSearchMask.RootUid() != header->RootUid() )
            {
            // Root UID defined in search criteria and Root UID does not match
            rootUidMatch = EFalse;
            }
        // else Root UID check ignored
        if ( aSearchMask.ThemeUid() && 
             aSearchMask.ThemeUid() != header->ThemeUid() )
            {
            // Theme UID defined in search criteria and Theme UID does not match
            themeUidMatch = EFalse;
            }
        // else Theme UID check ignored
        if ( aSearchMask.Flags() && 
             !( aSearchMask.Flags() & header->Flags() ) )
            {
            // Header flags defined in search criteria and Header flags does not match
            flagsMatch = EFalse;
            }
        // else header flags check ignored
        if ( aSearchMask.ConfigurationType() &&
             aSearchMask.ConfigurationType() != header->ConfigurationType() )
            {
            typeMatch = EFalse;
            }
        // else configuration type ignored
        if ( rootUidMatch && themeUidMatch && flagsMatch && typeMatch )
            {
            aConfHeader = header;
            aPos = i;
            }
        else
            {
            rootUidMatch = ETrue;
            themeUidMatch = ETrue;
            flagsMatch = ETrue;
            typeMatch = ETrue;
            }
        }                                     
    }

// -----------------------------------------------------------------------------
// ChspsThemeServer::ReinstallConfL()
// -----------------------------------------------------------------------------
//
void ChspsThemeServer::ReinstallConfL(
    TInt aAppUid,
    TInt aConfUid )
    {
    
    RFs fileserver;
    CleanupClosePushL( fileserver );
    User::LeaveIfError( fileserver.Connect() );   
    
    ChspsRomInstaller* installer = ChspsRomInstaller::NewL( *this, fileserver );
    CleanupStack::PushL( installer );
    
    installer->ReinstallThemeL( aAppUid, aConfUid );
    
    CleanupStack::PopAndDestroy( installer );
    CleanupStack::PopAndDestroy( &fileserver );

    }

// -----------------------------------------------------------------------------
// ChspsThemeServer::HandleBREventL()
// -----------------------------------------------------------------------------
//
void ChspsThemeServer::HandleBREventL(
    TInt aEvent )
    {

    switch ( iBRState )
        {
        case EhspsBRRestore:
            {
            HandleBRRestoreStateL( aEvent );
            }
            break;
        case EhspsBRBackup:
            {
            HandleBRBackupStateL( aEvent );
            }
            break;
        default: // EhspsBRNormal
            {
            HandleBRNormalStateL( aEvent );
            }
            break;
        }    

    }

// -----------------------------------------------------------------------------
// ChspsThemeServer::HandleBRNormalStateL()
// -----------------------------------------------------------------------------
//
void ChspsThemeServer::HandleBRNormalStateL(
    TInt aEvent )
    {
    if ( aEvent & conn::EBURBackupFull ||
         aEvent & conn::EBURBackupPartial )
        {
        CreateBRHandlerL( aEvent );
        if ( iBRHandler && iActiveBackupClient )
            {
            iBRState = EhspsBRBackup;
#ifdef HSPS_LOG_ACTIVE            
            iLogBus->LogText( _L( "ChspsThemeServer::HandleBRNormalStateL(): - Backup starts" ) );
#endif                  
            }
        }

    if ( aEvent & conn::EBURRestoreFull ||
         aEvent & conn::EBURRestorePartial )
        {
        CreateBRHandlerL( aEvent );
        if ( iBRHandler && iActiveBackupClient )
            {
            iBRState = EhspsBRRestore;
#ifdef HSPS_LOG_ACTIVE            
            iLogBus->LogText( _L( "ChspsThemeServer::HandleBRNormalStateL(): - Restore starts" ) );
#endif                  
            }
        }
    }

// -----------------------------------------------------------------------------
// ChspsThemeServer::HandleBRBackupStateL()
// -----------------------------------------------------------------------------
//
void ChspsThemeServer::HandleBRBackupStateL(
    TInt aEvent )
    {
    if ( aEvent & conn::EBURNormal )
        {
        DeleteBRHandler();
        iBRState = EhspsBRNormal;
#ifdef HSPS_LOG_ACTIVE            
        iLogBus->LogText( _L( "ChspsThemeServer::HandleBRBackupStateL(): - Backup completed" ) );
#endif                  
        }
    }

// -----------------------------------------------------------------------------
// ChspsThemeServer::HandleBRRestoreState()
// -----------------------------------------------------------------------------
//
void ChspsThemeServer::HandleBRRestoreStateL(
    TInt aEvent )
    {
    if ( aEvent & conn::EBURNormal )
        {
        // Disable auto installations
        DisableAutoInstallation();
        // Restore imported configurations from backup/import folder
        RestoreImportedConfigurationsL();
        // Restore application configurations from backup/themes folder
        RestoreApplicationConfigurationsL();
        // Restore completed
        DeleteBRHandler();
        iBRState = EhspsBRNormal;
        EnableAutoInstallationL();
#ifdef HSPS_LOG_ACTIVE            
        iLogBus->LogText( _L( "ChspsThemeServer::HandleBRRestoreStateL(): - Restore completed" ) );
#endif                  
        }
    }

// -----------------------------------------------------------------------------
// ChspsThemeServer::CreateBRHandlerL()
// -----------------------------------------------------------------------------
//
void ChspsThemeServer::CreateBRHandlerL(
    TInt aEvent )
    {
    if ( !iBRHandler )
        {
        iBRHandler = CHSPSBRHandler::NewL( *this );
        }
    if ( !iActiveBackupClient )
        {
        iActiveBackupClient = conn::CActiveBackupClient::NewL( iBRHandler );
        }
    if ( aEvent & conn::EBURBackupPartial ||
         aEvent & conn::EBURRestorePartial )
        {
        if( !iActiveBackupClient->DoesPartialBURAffectMeL() )
            {
            DeleteBRHandler();
            }
        }
    if ( iBRHandler && iActiveBackupClient )
        {
        iActiveBackupClient->ConfirmReadyForBURL( KErrNone );
        }
    }

// -----------------------------------------------------------------------------
// ChspsThemeServer::DeleteBRHandler()
// -----------------------------------------------------------------------------
//
void ChspsThemeServer::DeleteBRHandler()
    {
    delete iActiveBackupClient;
    iActiveBackupClient = NULL;
    delete iBRHandler;
    iBRHandler = NULL;
    }

// -----------------------------------------------------------------------------
// ChspsThemeServer::CreateBackupDataL()
// -----------------------------------------------------------------------------
//
void ChspsThemeServer::CreateBackupDataL()
    {

    // Clear previous backup data
    iDefinitionRepository->ClearBackupsL();
    
    // Get first application configuration header
    ChspsODT* searchMask = ChspsODT::NewL();
    CleanupStack::PushL( searchMask );
    searchMask->SetConfigurationType( EhspsAppConfiguration );
    ChspsODT* confHeader( NULL );
    TInt pos( 0 );
    GetConfigurationHeader( *searchMask, confHeader, pos );
    
    while ( confHeader != NULL )
        {
        // Backup application configuration
        iDefinitionRepository->BackupConfigurationL( *confHeader );
        // Get next application configuration header
        pos++;
        GetConfigurationHeader( *searchMask, confHeader, pos );
        }
        
    CleanupStack::PopAndDestroy( searchMask );
    }

// -----------------------------------------------------------------------------
// ChspsThemeServer::EnableAutoInstallationL()
// -----------------------------------------------------------------------------
//
void ChspsThemeServer::EnableAutoInstallationL()
    {

    // Reset installation arrays
    iInstallableSisxThemes.Reset();
    iInstalledSisxThemes.ResetAndDestroy();
    iManifestFiles.ResetAndDestroy();                   

    // Get installed configurations from the import folders 
    CDir* fileList( NULL );    
    TFindFile fileFinder( iFsSession );    
    fileFinder.FindWildByDir( 
        KFilterAllImportsV1, 
        KImportDirectoryC, 
        fileList );     
    CleanupStack::PushL( fileList );    
    if( fileList ) 
        {            
        const TInt installedCount = fileList->Count(); 
        for( TInt i = 0; i < installedCount ; i++ )                   
            {           
            const TEntry& entry = (*fileList)[i];                                              
            iInstalledSisxThemes.AppendL( entry.iName.AllocL() );            
            }           
        }           
    CleanupStack::PopAndDestroy( fileList );        

    // Create file change listener to observe changes in import folder
    delete iFileChangeListener;
    iFileChangeListener = NULL;
    iFileChangeListener = ChspsFileChangeListener::NewL( 
        iFsSession, 
        *this, 
        ENotifyAll, 
        KImportDirectoryC, 
        TImportDirListener );    
#ifdef HSPS_LOG_ACTIVE    
    iFileChangeListener->SetLogBus( iLogBus );
#endif        
    iFileChangeListener->Setup();           

    }

// -----------------------------------------------------------------------------
// ChspsThemeServer::DisableAutoInstallation()
// -----------------------------------------------------------------------------
//
void ChspsThemeServer::DisableAutoInstallation()
    {
    // Reset installation arrays
    iInstallableSisxThemes.Reset();
    iInstalledSisxThemes.ResetAndDestroy();
    iManifestFiles.ResetAndDestroy();                   

    // Delete import folder observer
    delete iFileChangeListener;
    iFileChangeListener = NULL;
    }

// -----------------------------------------------------------------------------
// ChspsThemeServer::RestoreImportedConfigurationsL()
// -----------------------------------------------------------------------------
//
void ChspsThemeServer::RestoreImportedConfigurationsL()
    {
    // Check that import folder exists and can be opened
    if ( BaflUtils::CheckFolder( iFsSession, KImportDirectoryC ) == KErrNone )
        {
        // Get list of restored configuration manifest.dat files included in 
        // import directory
        RArray <TFileName> files;
        CleanupClosePushL( files );
        hspsServerUtil::FindFilesL( 
            KImportDirectoryC, 
            KManifestFile, 
            files );
        
        // Install found configurations
        TInt err( KErrNone );
        CHSPSInstaller* installer = CHSPSInstaller::NewL( *this );
        CleanupStack::PushL( installer );
        for ( TInt i = 0; i < files.Count(); i++ )
            {
#ifdef HSPS_LOG_ACTIVE            
            iLogBus->LogText( _L( "ChspsThemeServer::InstallRestoredConfigurationsL(): - Install configuration: %S" ), &files[ i ] );
#endif                  
            TRAP( err, installer->InstallConfigurationL( files[ i ] ) );
#ifdef HSPS_LOG_ACTIVE            
            if ( err != KErrNone )
                {
                iLogBus->LogText( _L( "ChspsThemeServer::InstallRestoredConfigurationsL(): - Installation failed: %S" ), &files[ i ] );
                }
#endif                  
            }
        CleanupStack::PopAndDestroy( installer );
        CleanupStack::PopAndDestroy( &files );
        }
    }

// -----------------------------------------------------------------------------
// ChspsThemeServer::RestoreApplicationConfigurationsL()
// -----------------------------------------------------------------------------
//
void ChspsThemeServer::RestoreApplicationConfigurationsL()
    {
    // Get the list of application configuration files included
    // backup/themes directory
    RArray <TFileName> files;
    CleanupClosePushL( files );
    hspsServerUtil::FindFilesL( 
        KBackupThemesDirectoryC, 
        KConfFile, 
        files );
    
    // Get application configuration ODT's
    TInt err( KErrNone );
    CArrayPtrSeg<ChspsODT>* odts = 
        new ( ELeave )CArrayPtrSeg<ChspsODT>( KHeaderListGranularity );
    CleanupStack::PushL( TCleanupItem( ResetAndDestroyChspsODTArray, odts ) );
    
    for ( TInt i = 0; i < files.Count(); i++ )
        {
#ifdef HSPS_LOG_ACTIVE            
        iLogBus->LogText( _L( "ChspsThemeServer::RestoreApplicationConfigurationsL(): - Read application configuration ODT: %S" ), &files[ i ] );
#endif                  
        ChspsODT* odt = ChspsODT::NewL();
        CleanupStack::PushL( odt );
        TRAP( err, iDefinitionRepository->GetOdtL( files[ i ], *odt ) );
        if ( err == KErrNone )
            {
            // Add ODT to ODT list
            odts->AppendL( odt );
            CleanupStack::Pop( odt );
            }
        else
            {
#ifdef HSPS_LOG_ACTIVE            
            iLogBus->LogText( _L( "ChspsThemeServer::RestoreApplicationConfigurationsL(): - Application configuration ODT reading faild: %S" ), &files[ i ] );
#endif                  
            CleanupStack::PopAndDestroy( odt );
            }
        }
    
    // Validate application configurations
    for ( TInt i = 0; i < odts->Count(); i++ )
        {
        ChspsODT* odt = odts->At( i );
#ifdef HSPS_LOG_ACTIVE            
        iLogBus->LogText( _L( "ChspsThemeServer::RestoreApplicationConfigurationsL(): - Validate applicartion configuration: %S" ), &odt->ThemeFullName() );
#endif                  
        TRAP( err, ValidateRestoredConfigurationL( *odt ) );
        if ( err == KErrNone )
            {
            // Restore application configuration
            RestoreConfigurationL( *odt );
            }
        else
            {
#ifdef HSPS_LOG_ACTIVE            
            iLogBus->LogText( _L( "ChspsThemeServer::RestoreApplicationConfigurationsL(): - Validation failed: %S" ), &odt->ThemeFullName() );
#endif                  
            }
        }

    CleanupStack::PopAndDestroy( odts );
    CleanupStack::PopAndDestroy( &files );

    }

// -----------------------------------------------------------------------------
// ChspsThemeServer::ValidateRestoredConfigurationL()
// -----------------------------------------------------------------------------
//
void ChspsThemeServer::ValidateRestoredConfigurationL(
    ChspsODT& aOdt )
    {
    
    // Validate application configuration
    TBuf8<10> appConfUid; 
    _LIT8( KFormat8, "%X" );
    _LIT8( KHexPrefix, "0x" );
    appConfUid.Append( KHexPrefix );
    appConfUid.AppendFormat( KFormat8, aOdt.ThemeUid() );
    TPtrC8 appConfState;
    hspsServerUtil::GetAttributeValueL(
        aOdt,
        KConfigurationElement,
        KConfigurationAttrUid,
        appConfUid,
        KConfigurationAttrState,
        appConfState );
    
    if ( appConfState.Compare( KConfStateError ) == 0 )
        {
        // Invalid application configuration
        User::Leave( KErrGeneral );
        }
    
    // Check application configuration version
    ChspsODT* searchMask = ChspsODT::NewL();
    CleanupStack::PushL( searchMask );
    searchMask->SetThemeUid( aOdt.ThemeUid() );
    ChspsODT* header( NULL );
    TInt pos( 0 );
    GetConfigurationHeader( *searchMask, header, pos );
    if ( !header )
        {
        User::Leave( KErrNotFound );
        }
    else
        {
        hspsServerUtil::CheckConfigurationVersionL(
                        aOdt,
                        aOdt.ThemeUid(),
                        header->ThemeVersion() );
        }
    CleanupStack::PopAndDestroy( searchMask );

    // Validate included plugins
    // Get configuration UIDs from restored configuration
    RArray <TInt> uids;
    CleanupClosePushL( uids );
    hspsServerUtil::FindUniquePluginsL(
        aOdt,
        uids );
        
    for ( TInt i = 0; i < uids.Count(); i++ )
        {
        // Check if the configuration is installed
        TInt err( KErrNone );
        ChspsODT* searchMask = ChspsODT::NewL();
        CleanupStack::PushL( searchMask );
        searchMask->SetThemeUid( uids[ i ] );
        ChspsODT* header( NULL );
        TInt pos( 0 );
        GetConfigurationHeader( *searchMask, header, pos );
        if ( !header ) // Configuration not found
            {
            err = KErrNotFound;
            }
        else // Configuration found
            {
            // Check the configuration version
            TPtrC8 version;
            TRAP( err, hspsServerUtil::CheckConfigurationVersionL(
                aOdt,
                uids[ i ],
                header->ThemeVersion() ) );
            if ( err == KErrNone )
                {
                // Check that configuration resource files are found
                TRAP( err, hspsServerUtil::CheckResourceFilesL( 
                    aOdt, 
                    uids[ i ] ) );
                }
            }
        
        // Check configuration state attribute
        // Configuration UID as HEX string
        // 10 -> TInt as 0xXXXXXXXX string
        TBuf8<10> confUid; 
        _LIT8( KFormat8, "%X" );
        _LIT8( KHexPrefix, "0x" );
        confUid.Append( KHexPrefix );
        confUid.AppendFormat( KFormat8, uids[ i ] );
        TPtrC8 state;
        hspsServerUtil::GetAttributeValueL(
            aOdt,
            KConfigurationElement,
            KConfigurationAttrUid,
            confUid,
            KConfigurationAttrState,
            state );

        if ( err != KErrNone )        
            {
            // Invalid configuration
            state.Set( KConfStateError );
            // Delete related resource files
            const TInt count = aOdt.ResourceCount();
            for( TInt j( 0 ); j < count; j++ )
                {
                ChspsResource& resource = aOdt.ResourceL( j );
                if( resource.ConfigurationUid() == uids[ i ] )
                    {
                    aOdt.DeleteResourceL( j );
                    }
                }
            }
        else if ( state.CompareF( KConfStateError ) != 0 )
            {
            state.Set( KConfStateNotConfirmed );
            }
        hspsServerUtil::SetAttributeValueL(
            aOdt,
            KConfigurationElement,
            KConfigurationAttrUid,
            confUid,
            KConfigurationAttrState,
            state );
        
        CleanupStack::PopAndDestroy( searchMask );
        }
    
    CleanupStack::PopAndDestroy( &uids );
    }

// -----------------------------------------------------------------------------
// ChspsThemeServer::RestoreConfigurationL()
// -----------------------------------------------------------------------------
//
void ChspsThemeServer::RestoreConfigurationL(
    ChspsODT& aOdt )
    {
    
    if ( iDefinitionRepository->Locked() )
        {
        User::Leave( KErrServerBusy );
        }
    iDefinitionRepository->Lock();
    CleanupStack::PushL( TCleanupItem( UnlockRepository, iDefinitionRepository ) );
    
    iDefinitionRepository->SetOdtL( aOdt );
    // repository is unlocked in UnlockRepository function during PopAndDestroy
    CleanupStack::PopAndDestroy();
    }

// -----------------------------------------------------------------------------
// ChspsThemeServer::ActivateThemeL()
// -----------------------------------------------------------------------------
//
TInt ChspsThemeServer::ActivateThemeL( 
        const ChspsODT& aSetMask, 
        ChspsODT& aOdt )
    {
    TInt errorCode = GetConfigurationL( 
            aSetMask.RootUid(), 
            aSetMask.ThemeUid(), 
            aOdt );    
    if ( aOdt.ConfigurationType() != EhspsAppConfiguration )
        {
        errorCode = KErrArgument;
        }        
        
    if( !errorCode )
        {
        const TUint32 KMask = 0xFFFFFFFF;
        RArray<TUint32> res;
        CleanupClosePushL( res );
      
        iCentralRepository->FindL( aSetMask.RootUid(), KMask, res );
        if ( res.Count() == 0 )
            { 
            errorCode = iCentralRepository->Create( aSetMask.RootUid(), aSetMask.ThemeUid() );
            }
        else
            {
            errorCode = iCentralRepository->Set( aSetMask.RootUid(), aSetMask.ThemeUid() );
            }        
        if ( !errorCode )                
            {
            // Request re-evaluation of the new application configuration
            hspsServerUtil::UpdateAppConfigurationStateL( 
                aOdt,
                KConfStateConfirmed,
                KConfStateNotConfirmed );
                                      
            // Store changes to the Plug-in Repository
            iDefinitionRepository->SetOdtL( aOdt );                
            }

        res.Close();
        CleanupStack::PopAndDestroy(); // res
        }
    
    return errorCode;
    }

// -----------------------------------------------------------------------------
// ChspsThemeServer::GetActivateAppConfigurationL()
// -----------------------------------------------------------------------------
//
TInt ChspsThemeServer::GetActivateAppConfigurationL(
        const TInt aAppUid, 
        ChspsODT& aOdt )
    {            
    // Get active application configuration for the provided application
    TInt configurationUid = -1;
    TInt errorCode = iCentralRepository->Get( 
            aAppUid, 
            configurationUid );
    if ( !errorCode )
        {
        __ASSERT_DEBUG( configurationUid > 0, User::Leave( KErrArgument) );
        
        // Get the ODT object        
        errorCode = GetConfigurationL( 
                aAppUid, 
                configurationUid, 
                aOdt );        
        
        __ASSERT_DEBUG( aOdt.ConfigurationType() == EhspsAppConfiguration, User::Leave( KErrArgument) );        
        }
            
    return errorCode;        
    }

// -----------------------------------------------------------------------------
// ChspsThemeServer::GetActiveFamilyL()
// -----------------------------------------------------------------------------
//
TUint32 ChspsThemeServer::GetActiveFamilyL( 
        const TInt aAppUid )
    {
    if ( aAppUid < 1 ) 
        {
        User::Leave( KErrArgument );
        }    
    TUint32 familyMask( 0 );
    ChspsODT* appODT = ChspsODT::NewL();
    CleanupStack::PushL( appODT );
    if ( !GetActivateAppConfigurationL( aAppUid, *appODT ) )
        {
        familyMask = appODT->Family();
        }
    CleanupStack::PopAndDestroy( appODT );
    return familyMask;
    }

#if defined(WINSCW) || defined(__WINS__)
void ChspsThemeServer::ActivateRootConfigurationsL()
    {
    // Get family from the active resolution
    const ThspsFamily family = iFamily->GetFamilyType();
        
    // Try to activate an application configuration which was designed 
    // for the active resolution
    if ( !HandleFamilyChangeL( family ) )
        {
        // Try default family or just use the last one installed        
        HandleFamilyChangeL( KDefaultFamily );
        }
    }
#endif // defined(WINSCW) || defined(__WINS__)


// -----------------------------------------------------------------------------
// ChspsThemeServer::HandleFamilyChangeL()
// -----------------------------------------------------------------------------
//
TBool ChspsThemeServer::HandleFamilyChangeL( 
        const ThspsFamily aNewFamily )         
    {
#ifdef HSPS_LOG_ACTIVE        
    iLogBus->LogText( _L( "ChspsThemeServer::HandleFamilyChangeL(): resolution has changed, notifying clients..." ) );
#endif // HSPS_LOG_ACTIVE                
       
    __ASSERT_DEBUG( aNewFamily > EhspsFamilyUnknown && aNewFamily <= EhspsFamilyVga_tch, User::Leave( KErrArgument) );
    
    RArray<ChspsODT*> notificationTargets;
    CleanupClosePushL( notificationTargets );
    
    if ( iDefinitionRepository->Locked() )
        {
        User::Leave( KErrAccessDenied );
        }
    // In case of error. repository is unlocked
    iDefinitionRepository->Lock();           
    CleanupStack::PushL( TCleanupItem( UnlockRepository, iDefinitionRepository ) );
                         
    // Find first application configurations with the provided family id    
    ChspsODT* header = NULL;
    ChspsODT* targetHeader = NULL;
    for ( TInt headerIndex=0; headerIndex < iHeaderListCache->Count(); headerIndex++ )
        {
        header = iHeaderListCache->At( headerIndex );
        
        // It's totally acceptable that all clients haven't got resolution specific widgets,
        // in that case the UI should remaing the same - others should reload the application configuration
        // when appropriate for them
        if ( header->ConfigurationType() == EhspsAppConfiguration )
            {
                        
            // If the bit is set
            if ( header->Family() & aNewFamily )                
                {
            
                // Skip family change if it's already up to date
                TInt activeConfigurationUid = -1;
                User::LeaveIfError( 
                    iCentralRepository->Get( 
                        header->RootUid(), 
                        activeConfigurationUid )
                    );
                
                // Notify each client only once
                TBool alreadyIncluded = EFalse;
                for( TInt targetIndex=0; targetIndex < notificationTargets.Count(); targetIndex++ )
                    {
                    targetHeader = notificationTargets[ targetIndex ];
                    if ( header->RootUid() == targetHeader->RootUid() )
                        {
                        alreadyIncluded = ETrue;
                        break;
                        }
                    }                                                     
                if ( !alreadyIncluded )
                    {                
                    notificationTargets.Append( header );
                             
                    TBool updateRequired = ( activeConfigurationUid != header->ThemeUid() );                        
                    if ( updateRequired )
                        {                    
                        // Activate the resolution specific theme
                        ChspsODT* newOdt = ChspsODT::NewL();
                        CleanupStack::PushL( newOdt );                
                        ActivateThemeL( *header, *newOdt );                                                    
                        CleanupStack::PopAndDestroy( newOdt );
#ifdef HSPS_LOG_ACTIVE        
                        iLogBus->LogText( 
                            _L( "ChspsThemeServer::HandleFamilyChangeL(): application configuration %D has been activated" ),                     
                            header->ThemeUid()
                            );
#endif // HSPS_LOG_ACTIVE                
                        }            
                    } // updateRequired    
                
                } // header type
            
            }
        
        } // headers loop                    
    
    iDefinitionRepository->Unlock();
    CleanupStack::Pop(iDefinitionRepository);
        
    // Signal that we found an application configuration for the requested family
    TBool activated = ( notificationTargets.Count() > 0 );    
    
    // Reloading notifications are sent from void ChspsClientRequestHandler::HandleCenRepChangeL()       
    notificationTargets.Close();
    CleanupStack::PopAndDestroy( &notificationTargets );
    
#ifdef HSPS_LOG_ACTIVE        
    iLogBus->LogText( _L( "ChspsThemeServer::HandleFamilyChangeL(): notifications sent" ) );
#endif // HSPS_LOG_ACTIVE    
    
    return activated;
    }

// -----------------------------------------------------------------------------
// ChspsThemeServer::Family()
// -----------------------------------------------------------------------------
//
ChspsFamily* ChspsThemeServer::Family()
	{
	return iFamily;
	}

// end of file

