/*
* Copyright (c) 1020 Nokia Corporation and/or its subsidiary(-ies).
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
* Description:  HSPS server
* 
*  Architecture
*  ************
*  ChspsThemeServer derives CPolicyServer when compiled on EKA2 based Symbian OS.
*  ChspsThemeServer depends on the following classes:
*  - ChspsInstallationHandler which implements MhspsInstallationHandler,
*  - ChspsMaintenanceHandler which implements MhspsMaintenanceHandler,
*  - ChspsClientRequestHandler which implements MhspsClientRequestHandler,
*  - ChspsDefinitionRepository and ChspsODT from Xuikon, 
*  - ChspsFileChangeListener,
*  - ChspsAutoInstaller, and
*  - CRepository from Symbian.
*
*  Functionality
*  *************
*  When client connects to hspsThemeServer, the access rights are checked. 
*  If access is granted, a server session is created. Once connected, client may
*  perform service requests as long as it is connected. Service requests are 
*  served by initiating appropriate service handlers. These service handlers are
*  Symbian active objects, based on CActive-class. In using active ojects, it is
*  possible to perform long-lasting service requests without blocking others 
*  clients getting served on-time. Active objects are also used to assist 
*  services to perform asynchronous operations internally, for e.g. when service
*  is executing slow media conversions. Furthermore, some of these services are
*  extended to implement timer-functionality, based on CTimer-class. 
*  These active timers are used to control different time-based server 
*  operations and initiate service-state refreshing functionality.
*  hspsThemeServer contains auto-install feature that can install Application
*  Theme on SISX-packet exctraction.
*    
*  
*  Installation Handler
*  ====================
*  ChspsInstallatioHandler-active object performs all installation related
*  functionality. Installation concerns reading installation instructions from 
*  manifest-file, utilizind Xuikon Definition Engine to parse xml- and 
*  css-files, and apply style and localisation information defined in dtd-files
*  into the DOM-document created. Installer also handles resources that theme 
*  may include. Some resource types may need conversions to fit on the target
*  device's screen or audio device etc. In this case, the installer performs 
*  media conversions by its Media Converter. Conversions are asynchronous and
*  they deploy MIME-types. For more information of media conversions, see 
*  ChspsMediaConverter documentation. When installation is successful, 
*  Installation Handler utilizes Xuikon Definition Repository to store the 
*  results. The resulted objects are for e.g. unlocalised and localised 
*  versions of application theme. Theme associated resources are also handled 
*  in unlocalised and localised versions. Resources are stored on repository 
*  and their related information is stored on the list of ChspsResource-objects
*  - a resource list. This list of ChspsResource-objects is called Resource List.
*  Definition Repository is located in hspsThemeServer private  folder. 
*  Repository is organisized by theme header information. 
*  For more information, see hspsResource documentation.
*  On theme updates, i.e. installation over previous one, Installation Handler
*  deploys ChspsSecurityEnforcer to control theme update rights.
*  For more information, see ChspsSecurityEnforcer documentation and 
*  ChspsInstallatioHandler documentation. 
*  
*  Maintenance Handler
*  ===================
*  ChspsMaintenanceHandler-active timer performs all theme maintenance related 
*  functionality. Maintenance services includes theme listing, theme activation,
*  theme removal and default theme restoring operations. Maintenance functions 
*  deploy Xuikon Definition Repository and Symbian Central Repository. 
*  Definition Repository maintains Application Themes in save place and Central
*  Repository maintains the information about which theme is active for what 
*  application. Information in Central Repository is presented in pairs of 
*  application UID and active theme UID.
*  For more information, see ChspsMaintenanceHandler documentation. 
*
*  Client Request Handler
*  ======================
*  ChspsClientRequestHandler-active timer performs all request accesses from 
*  Rendering Engine. When request exists, the just right one application theme 
*  is delivered to client process using memory chunk passing. Resource List is
*  delivered too. Client Request Handler uses Xuikon Definition Repository and 
*  Symbian Central Repository to get match theme on the request. Client Request
*  Handler serves also resource accesses. In using file handle passing 
*  introduced by Symbian Platform Security concept, Client Request Service 
*  arranges a resource access to hspsThemeServer's private folder for the client 
*  process. RFile handle passing is used. 
*  For more information, see ChspsClientRequestHandler documentation.
*
*  File Change Listener
*  ==================
*  ChspsFileChangeListener listens to given file folder and notifies its observer
*  if there is any file change. Observer is given as a parameter on 
*  initialising the listener. Observer must implement MhspsFileChangeObserver-interface.
*  FileChangeListener is asynchronous object that deploys Symbian RFs client-sideAutoInstaller
*  handle on File Manager. FileChangeListener is intended to be used by any object
*  that wants listen Symbian file system events.
*
*  Auto Installer
*  ==============
*  ChspsAutoInstaller installs theme from given location. It is intended
*  to be used by hspsThemeServer directly. AutoInstaller is initiated
*  when FileChangeListener finds installable theme source on given
*  location. Theme source is set during the SISX-package exctraction. 
*  AutoInstaller is deployed by calling its function InstallThemeL with manifest-file 
*  of the installable theme source set.
*  AutoInstaller deploys ChspsInstallationHandler. ChspsInstallationHandler
*  is asynchronous object, there for AutoInstaller is asynchronous object
*  as well capable to deploy asynchronous installation.
*  When installation ends, AutoInstaller calls its observer with 
*  ThspsServiceCompletedMessage-type parameter. Observer is given on
*  initialising of AutoInstaller and it must be ChspsThemeServer-type.
*  AutoInstaller have functions to get ODT-header of successfully installed
*  theme and ChspsResult-typed object expressing errors in unsuccessful cases.   
*
*  Security Enforcer
*  =================
*  ChspsSecurityEnforcer performs all security related functions in Xuikon
*  Application Theme Management services.
*  These functions are the following:
*  - locking policy functionality,
*  - style locking policy functionality,
*  - control user access on service request, and
*  - validate content of user queries.
*
*  Performance accelerators
*  ************************
*
*  HeaderListCache
*  ---------------
*  To speed up serving the theme request operations initiated by Rendering 
*  Engine in theme usage cases, hspsThemeServer maintains cache of the header
*  information of the themes stored in Definition Repository. When installation
*  occurs, cache is updated. First time the cache is updated when hspsThemeServer
*  starts - server is started by system starter service. This quaranties
*  that cache is up to date and hit will occur on the very first access. 
*  Cache is updated also by MaintenanceHandler and ClientRequestHandler on 
*  the cases where content of repository is affected.
* 
*  Pre-initialized objects
*  -----------------------
*  hspsThemeServer speeds up the performance also by pre-initialized objects 
*  which it passes to ChspsClientRequestHandler when one is initiated on new 
*  Client Request Service server session. These objects are the following:
*  - ChspsDefinitionRepository
*  - CRepository for the KhspsThemeStatusRepositoryUid  {0x10207342} 
*
*
*/


#ifndef __hspsTHEMESERVER_H__
#define __hspsTHEMESERVER_H__

#include <e32base.h>
#include <abclient.h>

#include "hspsfilechangelistener.h"
#include "hspscenreplistener.h"
#if defined(WINSCW) || defined(__WINS__)
#include "hspsfamilylistener.h"
#endif // defined(WINSCW)
#include "hspsthememanagement.h"
#include "hspsodt.h"
#include "hspsdefinitionrepository.h"
#include "hspsbrobserver.h"
#include "hspsbrhandler.h"

#include "hsps_builds_cfg.hrh"

/** hspsThemeServer name: */
_LIT(KhspsThemeServerName,"hspsthemeserver");

/** Supported manifest version */
_LIT(KhspsSupportedManifestVersion,"1.0");

/** hspsThemeServer Uid3 = SECUREID. */
const TUid KhspsThemeServerUid3 = {0x200159C0}; //old uid 0x10207254};

/** 
*   KhspsThemeServerMajorVersionNumber, KhspsThemeServerMinorVersionNumber, 
*   and KhspsThemeServerBuildVersionNumber.
*   These constants define a version to be given for creating a session 
*   with the hspsThemeServer.
*/
const TUint KhspsThemeServerMajorVersionNumber=1;
const TUint KhspsThemeServerMinorVersionNumber=0;
const TUint KhspsThemeServerBuildVersionNumber=0;

/*
*  KDefaultMessageSlots. Number of hspsThemeServer message slots.
*  Server slots allocation rules: 1 for synchronous request, 1 for each
*  concurrent outstanding asynchronous requests and 1 for each cancellation
*  => 3 in this case.
*/
const TInt KDefaultMessageSlots = 3;

// Forward declarations.
class ChspsDefinitionRepository;
class ChspsDefinitionEngineInterface;
class CRepository;
class ChspsSecurityEnforcer;
class ChspsRomInstaller;
class ChspsAutoInstaller;
class ChspsThemeServerSession;

#ifdef HSPS_LOG_ACTIVE
class ChspsLogBus;
#endif

// Keys for the Central Repository
const TInt KCenrepKeyLang                = 100000001;      // Device language from previous startup
const TInt KCenrepKeyPostRFS             = 100000002;      // Holds information whether the post RFS initializations have been done 
const TInt KCenrepKeyCheckConf           = 100000003;      // Holds information whether the configuration states must be checked 
const TInt KCenrepKeyAccessControlList   = 100000004;      // Holds information about security access control list

const TInt KCenRepBufferSize = 255;   // Control Access List length

/*
* Server's policy.
*/

/* Total number of ranges */
const TUint hspsThemeRangeCount = 4;

/* Definition of the ranges of IPC numbers */
const TInt hspsThemeRanges[hspsThemeRangeCount] = 
        {
        EhspsInstallationBase, 	/* installation messages (0,1,2 ) */
        EhspsMaintenanceBase, 	/* maintenance messages (3,4,5,6,7,8) */
        EhspsClientRequestBase, /* client request messages (9,10,11,12,13) */
        EhspsNotSupported, 		/* and up; non implemented function end of range check ; ENotSupported */ 
        }; 

/* Policy to implement for each of the above ranges */       
const TUint8 hspsThemeElementsIndex[hspsThemeRangeCount] = 
        {
        CPolicyServer::ECustomCheck, //applies to 0th range
        CPolicyServer::ECustomCheck, //applies to 1st range
        CPolicyServer::ECustomCheck, //applies to 2nd range
        CPolicyServer::ENotSupported //applies to 3rd range (out of range IPC)
        };

/* Specific capability checks */
const CPolicyServer::TPolicyElement hspsThemeElements[] = 
        {
        {_INIT_SECURITY_POLICY_C1(ECapabilityReadUserData), CPolicyServer::EFailClient},  
        //policy "0"; fail call if ReadUserData not present
        {_INIT_SECURITY_POLICY_C1(ECapabilityTCB), CPolicyServer::EFailClient} 
        //policy "1"; fail call if TCB not present
        };

/* Package all the above together into a policy */
const CPolicyServer::TPolicy hspsThemePolicy =
        {
        CPolicyServer::EAlwaysPass, /* specifies all connect attempts should pass */
        hspsThemeRangeCount,  /* number of ranges */                                  
        hspsThemeRanges,  /* ranges array */
        hspsThemeElementsIndex, /* elements<->ranges index */
        hspsThemeElements,    /* array of elements */
        };

// File filters related to imports folder
_LIT( KFilterAllImportsV1, "*.dat");
_LIT( KFilterAllAppImportsV1, "app_*.dat");
_LIT( KFilterAllPluginImportsV1, "plugin_*.dat");
_LIT( KFilterAllImportsV2, "*.*");

#ifdef _hsps_SERVER_SHUTDOWN_ENABLED_
/** 
 * MShutdownObserver
 * Observer for automatic shutdown class.
 * 
 * @since S60 5.0  
 */
class MShutdownObserver
    {
public:
    /**
    * Notifies observer that shutdown is initiating.
    * 
    * @since S60 5.0
    */
    virtual void ShutdownInitiating() = 0;
    };

/** 
 * CShutdown.
 * Auto shutdown class (standard pattern)
 * 
 * @since S60 5.0  
 */
class CShutdown : public CTimer
    {    
public:
    /**
    * Create new shutdown timer.
    * 
    * @since S60 5.0 
    * @param aObserver Shutdown observer.
    */    
    static CShutdown* NewL( MShutdownObserver& aObserver );

    /**
    * Start shutdown timer.
    * 
    * @since S60 5.0
    */        
    void Start();
  
protected:
    /**
    * C++ constructor.
    *
    * @since S60 5.0  
    * @param aObserver Shutdown observer.
    */        
    CShutdown( MShutdownObserver& aObserver );    
    
    /**
    * Symbian 2nd phase constructor.
    * 
    * @since S60 5.0
    */        
    void ConstructL();

private:
    /**
    * From CActive.
    */        
    void RunL();
    
private:
    // Shutdown delay.
    enum { KhspsShutdownDelay = 2000000 };
    
    // Shutdown observer.
    MShutdownObserver& iObserver;
  };
#endif // _hsps_SERVER_SHUTDOWN_ENABLED_

// States of the auto installer
enum TAutoInstallerStates
    {
    EAutoInstallerStateIdle = 0,
    EAutoInstallerStateInstall = 1,
    EAutoInstallerUninstall = 2
    };

// Operations for header cache updates
enum ThspsCacheOperations 
    {
    EhspsCacheAddHeader,
    EhspsCacheRemoveHeader
    };

// Backup states
enum ThspsBRStates 
    {
    EhspsBRNormal = 0,
    EhspsBRBackup,
    EhspsBRRestore
    };


/**
 * @ingroup group_hspsserver
 * ChspsThemeServer. 
 * @since S60 5.0
 **/
class ChspsThemeServer : public CPolicyServer, 
                       public MhspsFileChangeObserver,
                       public MhspsDefinitionRepositoryObserver,
                       public MhspsCenRepObserver
#ifdef _hsps_SERVER_SHUTDOWN_ENABLED_
                       , public MShutdownObserver
#endif // _hsps_SERVER_SHUTDOWN_ENABLED_         
#if defined(WINSCW) || defined(__WINS__)                         
                       , public MhspsFamilyObserver
#endif // defined(WINSCW)

    {
public:
	
    /**
    * NewLC
    * @since S60 3.1
    */
    static CServer2* NewLC();
    
    /**
    * AddSession
    * 
    * @param aSession   Session that is added. Ownership not transferred!.
    * 
    * @since S60 3.1
    */
    void AddSession( ChspsThemeServerSession* aSession );
    
    /**
    * DropSession
    * 
    * @param aSession   Session that is dropped. Ownership not transferred!.
    * 
    * @since S60 3.1
    */
    void DropSession( ChspsThemeServerSession* aSession );
    
public: // public functions
	
    /**
    * UpdateHeaderListCacheL
    * Re-scans the file system under the Plug-in Repository. 
    * Execution is slow and should be used only when absolutely required. 
    * Updates headerlist cache
    * @since S60 5.0
    */
    void UpdateHeaderListCacheL();
    
    /**
     * Updates the header list cache based on the provided ODT information.
     * Should be preferred over the previous API.     
     * @since S60 5.0
     * @param aOperation is an enumeration of the cache operations
     * @param aRootUid is an UID of the application/interface
     * @param aProviderUid is an UID of the provider
     * @param aConfUid is an UID of the plug-in configuration
     */
    TInt UpdateHeaderListCache(    
        ThspsCacheOperations aOperation,
        const TInt aRootUid,
        const TInt aProviderUid,
        const TInt aConfigurationUid );
    
    /**
    * CleanupTemporaryFilesL
    * Cleans temporary files
    * @since S60 3.1
    */
    void CleanupTemporaryFilesL(TInt secureId = 0);
    
    /**
    * IncreaseRequestClientCount
    * Increases count of client requests
    * @since S60 5.0
    */
    void IncreaseRequestClientCount();
    
    /**
    * DecreaseRequestClientCount
    * Decreases count of client requests
    * @since S60 5.0
    */
    void DecreaseRequestClientCount();
        
    /**
    * HandleAutoInstallerEvent
    * Handles autoinstaller events
    * @since S60 3.1
    */
    void HandleAutoInstallerEvent( ThspsServiceCompletedMessage aMsg );
            
    /**
    * DefinitionRepository
    * @since S60 3.1
    */
    ChspsDefinitionRepository& DefinitionRepository();
    
    /**
    * SecurityEnforcer
    * @since S60 3.1
    */
    ChspsSecurityEnforcer& SecurityEnforcer();
    
    /**
    * CentralRepository
    * @since S60 3.1
    */
    CRepository& CentralRepository();
    
    /**
    * DeviceLanguage
    * Returns device language
    * @since S60 3.1
    */
    TLanguage DeviceLanguage();
        
    /**
    * HeaderListCache
    * Returns headerlist cache
    * @since S60 3.1
    */
    CArrayPtrSeg<ChspsODT>& HeaderListCache();
    
    /**
    * GetConfigurationL
    * Retrieves a configuration with the provided UIDs.
    * @since S60 5.0    
    * @param aInterfaceUid An uid of the interface or application (e.g. AI3, Java etc), optional.
    * @param aConfigurationUid An uid of the plugin configuration     
    * @param aRetrievedODT Retrieved configuration
    * @return KErrNone if operation succeeded
    */
    TInt GetConfigurationL(
		const TInt aInterfaceUid,
		const TInt aConfigurationUid,
		ChspsODT& aRetrievedODT );

    /**
    * GetConfigurationHeaderL
    * Retrieves a configuration header matching the search criteria
    * @since S60 5.0    
    * @param aSearchMask ODT header defining search criteria
    * @param aConfHeader Found ODT header
    * @param aPos Start/Found position
    */
    void GetConfigurationHeader(
        ChspsODT& aSearchMask,
        ChspsODT*& aConfHeader,
        TInt& aPos );
    
    /**
     * Localizes an ODT.
     * Testing: change first two digits from \epoc32\RELEASE\WINSCW\udeb\
     * Z\resource\BootData\Languages.txt file and reboot the emulator.     
     * @since S60 5.0
     * @param aEngine is a service for replacing DTD entities in XML.
     * @param aOdt is an ODT instance to be localized.     
     * @return ETrue if the ODT was successfully localized
     */
    TBool LocalizeL( 
		ChspsDefinitionEngineInterface& aEngine,
		ChspsODT& aOdt );
    
    /**
     * Parses name of a manifest from the provided import's name.                      
     * @since S60 5.0
     * @param aImportFile is a name of the import to be parsed
     * @param aPath is a path from which the import should be found
     * @return descriptor holding name of the manifest or NULL     
     */
    HBufC* GetManifestFromImportLC(
        const TDesC& aImportFile,
        const TDesC& aPath );

    /**
     * Check the installed configurations
     * @param aAppUid Application UID which configurations are checked
     * @since S60 5.0
     */
    void CheckConfigurationL(
        const TInt aAppUid );

    /**
     * Reinstalls defined configuration
     * @param aAppUid Application UID which configuration is reinstalled
     * @param aConfUid Configuration UID of reinstalled configuration
     * @since S60 5.0
     */
    void ReinstallConfL(
        TInt aAppUid,
        TInt aConfUid );

    /**
     * Handles received backup event
     * @since S60 5.0
     * @param aEvent Backup event
     */
    void HandleBREventL( TInt aEvent );

    /**
     * Prepares backup data for passive backup
     * @since S60 5.0
     */
    void CreateBackupDataL();


    /**
     * Check the installed plugin is used in some application configuration
     * @since S60 5.0
     * @param aPluginUid Plugin UID of installed configuration
     */
   TInt CheckInstalledPluginUsageL( const TInt aPluginUid );

    /**
     * Check if given plugin is in header cache.
     * 
     * @since S60 5.0
     * @param aPluginUid Plugin UID to search from header cache.
     */    
    TBool PluginInHeaderCache( const TUid aPluginUid );

   /**
    * Cleanup server resources. This method is called when server is been
    * shut down and active scheduler is still installed.
    * 
    * @since S60 5.0
    */
   void Cleanup();
   
   /**
    * Activates provided application configuration. 
    * @param aSetMask Identifies an application configuration which is to be activated.
    * @param aOdt Activated application configuration
    */
   TInt ActivateThemeL(
           const ChspsODT& aSetMask, 
           ChspsODT& aOdt );

   /**
    * Retrieves active application configuration from Central Repository.
    * @since S60 5.0
    * @param aAppUid Identifies the application
    * @param aOdt Contains currently active application configuration
    */
   TInt GetActivateAppConfigurationL(
           const TInt aAppUid, 
           ChspsODT& aOdt );
   
   /**
    * Gets family information from an active application configuration.
    * @since S60 5.0
    * @param aAppUid Identifies the application    
    * @returns The active family
    */
   TUint32 GetActiveFamilyL( 
           const TInt aAppUid );      
   
public: // from MhspsFileChangeObserver
	
    /**
    * HandleFileSystemChange
    * Handles change of filesystem
    * @since S60 3.1
    */
    void HandleFileSystemChange();
    
    /**
    * HandleRepositoryChange
    * Handles change of repository
    * @since S60 3.1
    */
    inline void HandleRepositoryChange() {};

#ifdef _hsps_SERVER_SHUTDOWN_ENABLED_
public: // From MShutdownObserver
    /**
    * From MShutdownObserver.
    */
    void ShutdownInitiating();    
#endif // _hsps_SERVER_SHUTDOWN_ENABLED_
    
    
public: // from MhspsDefinitioRepositoryObserver
	
    /**
    * HandleDefinitionRespositoryEvent
    * Handles change events received from definition repository
    * @since S60 3.1
    */
    TBool HandleDefinitionRespositoryEvent( ThspsRepositoryInfo aRepositoryInfo );
    
public: // from MhspsCenRepObserver
         
    void HandleCenRepChangeL( const TUint32 aId );    
     
#if defined(WINSCW) || defined(__WINS__)    
public: // from MshspFamilyObserver    
    TBool HandleFamilyChangeL( const ThspsFamily aNewFamily );
#endif // defined(WINSCW)
    
private:
	
    /**
    * ChspsThemeServer
    * @since S60 3.1
    */
    ChspsThemeServer();
    
    /**
    * ~ChspsThemeServer
    * @since S60 3.1
    */
    ~ChspsThemeServer();
    
    /**
    * ConstructL
    * @since S60 3.1
    */
    void ConstructL();
    
    /**
    * NewSessionL
    * @since S60 3.1
    */
    CSession2* NewSessionL(const TVersion& aVersion, const RMessage2& aMessage) const;
    
    /**
    * ComparePaths
    * @since S60 3.1
    */
    TBool ComparePaths(const ChspsODT& aOldHeader, const ChspsODT& aNewHeader);
    
    /**
    * GetHeaderListL
    * Gets current headerlist from filesystem
    * @param aHeaderList Destination for read headerlists from filesystem  
    * @param aSearchMask SearchMask to use when reading headerlists from filesystem  
    * @since S60 3.1
    */
    void GetHeaderListL(CArrayPtrSeg<ChspsODT>& aHeaderList, const ChspsODT& aSearchMask );
                   
    /**
    * CheckActiveThemesL
    * Check active themes from the central repository and if needed open path info file for them
    * @since S60 3.1
    */               
    void CheckActiveThemesL();
    
    //From CPolicyServer
    /**
    * CustomSecurityCheckL
    * @since S60 3.1
    */
    CPolicyServer::TCustomResult CustomSecurityCheckL(const RMessage2& aMsg, TInt& aAction,
                              TSecurityInfo& aMissing);
                              
    /**
    * HandleFileSystemChangeL
    * Does the actual filesystem change handling
    * @since S60 3.2
    */
    void HandleFileSystemChangeL(); 

    /**
    * RunAutoInstaller
    * Runs auto installer
    * @since S60 5.0
    */    
    TBool RunAutoInstaller( TAutoInstallerStates aState );

    /**
    * StopAutoInstaller
    * Stops auto installer and does house keeping after installation
    * @since S60 3.2
    */        
    void StopAutoInstaller();
   
   /**
    * HandleThemeUpgrade;
    * Handles the SIS theme upgrade
    */        
    void HandleThemeUpgradeL();
    
    /**
    * Checks whether the device language has been changed and updates 
	* configurations accordingly
	*/
    void HandleLanguageChangeL();
    
    /**
     * Localizes ODT files according to the device language.
     * @since S60 5.0
     */
    void LocalizeConfigurationsL();
    
    /**     
     * Localizes an application configuration with a device language. 
     * This function is used by the LocalizeL service.
     * @since S60 5.0
     * @param aEngine is a service for replacing DTD entities in XML.
     * @param aRequestedLanguage is the requsted language
     * @return ETrue if all the application and plugin configurations were localized
     */
    TBool LocalizeApplicationConfigurationL(
        ChspsDefinitionEngineInterface& aEngine,
        ChspsODT& aOdt,
        const TLanguage aRequestedLanguage );
    
    /**
     * Localizes a plugin configuration with a device language. 
     * This function is used by the LocalizeL service.
     * @since S60 5.0
     * @param aEngine is a service for replacing DTD entities in XML.
     * @param aRequestedLanguage is the requsted language
     * @param aPluginUid is > 0 when LocalizeApplicationConfigurationL() is used 
     * @return ETrue if the plugin configuration was localized
     */
    TBool LocalizePluginConfigurationL(
        ChspsDefinitionEngineInterface& aEngine,
        ChspsODT& aOdt,
        const TLanguage aRequestedLanguage,
        TInt aPluginUid );
                
    /**
     * Handles installation of ROM based manifest files when a Restore Factory Settings (RFS)
     * operation has been executed (controlled via the Central Repository).    
     * This function is executed at bootup when the server starts. 
     * @since S60 5.0 
     */
    void HandleRomInstallationsL();
    
    /**
     * Installs widgets located at \private\200159C0\install\ directories.
     * @since S60 5.0 
     */
    void InstallWidgetsL();
                                        
    /**
     * Initiates uninstallation of a manifest file located under the imports folder.
     * @since S60 5.0
     * @param aFilelist is a list of dat files under the imports folder or NULL 
     */
    void UninstallImport( 
		const CDir* aFilelist );
                
    /**
     * Installs new configuration based on the information in imports name.
     * @since S60 5.0
     * @param aFilelist is a list of dat files to be parsed under the imports folder
     */    
    void InstallImportsL( 
		const CDir& aFilelist );        
    
    /**
     * Handles installation/uninstallation of imported configurations.
     * A file filter controls which type of configurations are to be handled. 
     * @since S60 5.0
     * @param aFilePrefix is a prefix for finding either plugin or application configuration imports.
     */
    void HandleConfigurationImportsL(
		const TDesC& aFilePrefix );    
    
    /**
     * 
     * @since S60 5.0
     * @param aAppUid is an UID of the application
     */
    void UnlockSisxImportL( 
            const TUint32 aAppUid );
    
    /**
     * 
     * @since S60 5.0
     * @param aAppUid is an UID of the application
 	 * @param aConfUid is an UID of the active configuration
     */
    void LockSisxImportL( 
            const TUint32 aAppUid,
			const TInt aConfUid );

    /**
    * GetDeviceLanguage
    * @since S60 5.0
    */
    TLanguage GetDeviceLanguage();
    
    /**     
     * Retrieves a list of headers matching a iCacheMask search mask.
     * @since S60 5.0
     * @param aHeaderList is a list of headers found from the Plug-in Repository
     * @return KErrNone if operation was successfull
     */
    TInt DoUpdateHeaderListCache(
            CArrayPtrSeg<ChspsODT>& aHeaderList );       

    
    /**
     * Adds specific header to the cache.
     * An alternative to UpdateHeaderListCacheL API for updating the header 
     * cache with a minimum performance hit. 
     * @since S60 5.0
     * @param aRootUid is an UID of the application/interface
     * @param aProviderUid is an UID of the provider
     * @param aConfUid is an UID of the plug-in configuration
     */
    void HeaderCacheAddL(
            const TInt aRootUid,
            const TInt aProviderUid,
            const TInt aConfigurationUid );
    
    /**
     * Removes specific header from the cache.
     * An alternative to UpdateHeaderListCacheL API for updating the header 
     * cache with a minimum performance hit. 
     * @since S60 5.0
     * @param aRootUid is an UID of the application/interface
     * @param aProviderUid is an UID of the provider
     * @param aConfUid is an UID of the plug-in configuration
     */
    TInt HeaderCacheRemove( 
            const TInt aRootUid,
            const TInt aProviderUid,
            const TInt aConfigurationUid );   

    /**
     * Handles backup events in EhspsBRNormal backup state
     * @since S60 5.0
     * @param aEvent is a received backup event
     */
    void HandleBRNormalStateL(
        TInt aEvent );

    /**
     * Handles backup events in EhspsBRBackup backup state
     * @since S60 5.0
     * @param aEvent is a received backup event
     */
    void HandleBRBackupStateL(
        TInt aEvent );

    /**
     * Handles backup events in EhspsBRRestore backup state
     * @since S60 5.0
     * @param aEvent is a received backup event
     */
    void HandleBRRestoreStateL(
        TInt aEvent );

    /**
     * Creates active backup and restore handler
     * @since S60 5.0
     * @param aEvent is a received backup event
     */
    void CreateBRHandlerL(
        TInt aEvent );

    /**
     * Deletes active backup and restore handler
     * @since S60 5.0
     */
    void DeleteBRHandler();

    /**
     * Enables autoinstallations
     * @since S60 5.0
     */
    void EnableAutoInstallationL();

    /**
     * Disable autoinstallations
     * @since S60 5.0
     */
    void DisableAutoInstallation();

    /**
     * Installs restored configurations from backup/import folder
     * @since S60 5.0
     */
    void RestoreImportedConfigurationsL();

    /**
     * Restores application configurations from backup/themes folder
     * @since S60 5.0
     */
    void RestoreApplicationConfigurationsL();

    /**
     * Validates restored application configuration
     * @since S60 5.0
     * @param aOdt is reference to application configuration to be validated
     */
    void ValidateRestoredConfigurationL(
        ChspsODT& aOdt );

    /**
     * Restore configuration
     * @since S60 5.0
     * @param aOdt is reference to application configuration to be restored
     */
    void RestoreConfigurationL(
        ChspsODT& aOdt );
    
    /**
     * Install all widgets from uda
     * @since S60 5.2
     */
    void InstallUDAWidgetsL();
    
#if defined(WINSCW) || defined(__WINS__)    

    /**
     * Executed at startup to activate root configurations which were designed
     * for the current resolution.
     * @since S60 5.0 
     */
    void ActivateRootConfigurationsL();
           
#endif // defined(WINSCW)
        
#ifdef _hsps_SERVER_SHUTDOWN_ENABLED_    
    CShutdown* iShutdown;
#endif // _hsps_SERVER_SHUTDOWN_ENABLED_

    /* 
    * iRequestClientCount counts the number of RequestClientHandler instances
    * when this number is 0, no resource file access exist and the temporary resource
    * file instances can be removed safely
    */
    TInt iRequestClientCount;
      
    // Contents of the header cache
    CArrayPtrSeg<ChspsODT>* iHeaderListCache;
    TBool iHeaderListPending;
           
    RFs iFsSession;
    ChspsDefinitionRepository* iDefinitionRepository;
    CRepository* iCentralRepository;
    ChspsSecurityEnforcer* iSecurityEnforcer;
    ChspsFileChangeListener* iFileChangeListener;
    
    // Active device language
    TLanguage iDeviceLanguage;
        
    ChspsODT* iCacheMask;
    
    // Installation handler for ROM imports 
    ChspsRomInstaller* iRomInstaller;
    
    // Installation handler for SISX imports
    ChspsAutoInstaller* iAutoInstaller;
                               
    // Array of installable *.dat files under the imports folder
    RPointerArray<HBufC> iInstallableSisxThemes;
    
    // Array of installed *.dat files under the imports folder
    RPointerArray<HBufC> iInstalledSisxThemes;        
    
    // Array of installable manifest files
    RPointerArray<HBufC> iManifestFiles;
    
    // Index of a manifest in autoinstallation
    TInt iCurrentManifestFile;
    
    // State of the SISX installer
    TInt iAutoInstallerState;
    
    // Index of the theme to be removed
    TInt iThemeIndexToRemove;
    
	// File locks for preventing uninstallation of active SISX configurations
    RFile iFileAS;
    RFile iFileAI;
    RFile iFileLT;
    
    // Pointer to Central Repository Listener        
    ChspsCenRepListener* iCenRepListener;

    // Pointer to backup event listener
    CHSPSBRObserver* iBRObserver;

    // Backup state
    ThspsBRStates iBRState;

    // Active backup client
    conn::CActiveBackupClient* iActiveBackupClient;
    
    // Backup handler
    CHSPSBRHandler* iBRHandler;
    

#if defined(WINSCW) || defined(__WINS__)
    // Listener for resolution and orientation changes
    ChspsFamilyListener* iFamilyListener;
#endif // defined(WINSCW)    
    
#ifdef HSPS_LOG_ACTIVE    
    /**
     * Log bus.
     */        
    ChspsLogBus* iLogBus;
#endif    
    
    RPointerArray<ChspsThemeServerSession> iSessions; // Sessions not owned!
    };

#endif //__hspsTHEMESERVER_H__
