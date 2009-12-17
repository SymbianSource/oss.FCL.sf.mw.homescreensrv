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
* Description:   
*  
*  HSPS Application Theme Management Service APIs
*  ************************************************
*  This file introduces the definitions of the interfaces and constanst to be a base of the
*  implementations of HSPS Application Theme Management services.
*  HSPS Application Theme Management Service APIs are introduced by defining Symbian M-classes
*  for each API-functionality groups. These functionality groups and their correcponding M-classes
*  are the following: 
*  - MhspsInstallationService  - defines services for Application Theme 
*                              installations,
*  - MhspsMaintenanceService   - defines services for listing, activation, 
*                              removing and restoring themes set as default, 
*  - MhspsClientRequestService - defines services for usage of Application Themes,
* 
*  - MhspsThemeManagementServiceObserver - a generic call-back service API to be utilized by client
*    applications of HSPS Theme Management Services, and
*  - MhspsClientRequestServiceObserver - a dedicated interface to define the call-back service to be
*    utilized by clients of HSPS Client Request Service. Typical client of the request service is
*    some rendering engine implementation like HSPS UI Engine.
*
*  Typical HSPS Application Theme Management Service use-cases
*  =============================================================
*  Usage of Application Theme is requested by Rendering Engines. Theme rendering happens when 
*  Application's UI (user interface) is shown on the screen of the target device. Principal
*  requestor of Installation and Maintenanace Service APIs is S60 Personalisation Application. 
*  Personalisation UI will present theme installation and activation functionality to the end-user.
*  Also, HSPS-application itself may want to offer some installation and maintenance 
*  functionality on their menus, for e.g. allow a user select the theme to be used - theme 
*  activation (or hot-swap). Some system services of target devices, like OTAPush or DM (Device
*  Management), may also use installation and maintenance APIs. 
*  This supports operators related use-cases where they want to maintain the customisation of mobile
*  phone applications on-line.
*
*  HSPS UI Manager
*  ***************** 
*  UI Manager is a Symbian Client/Server system which is intended to serve HSPS native 
*  applications - as well as S60-legacy applications - with Application Theme installation, 
*  maintenance and usage related services. UI Manager implements HSPS Application Theme Management
*  Service APIs. 
*
*  Client-side functionality and APIs
*  ==================================
*  In Client/Server system, only Client-side APIs are meaningful to the client
*  applications. Offered client-side APIs are divided in two categories; 
*  hspsClient API and hspsRequestClient API. Both clients run basicly in 
*  asynchronous mode. however, they offers synchronic services also when 
*  appropriate. Clients are are able to delegate server messages to their 
*  client applications. hspsClient API offers application theme installation 
*  and maintenance related services, meanwhile hspsRequestClient API offers 
*  services related to the usage of Application Themes. 
*                 
*  Server-side functionality and APIs
*  ==================================
*
*
*  Security
*  ========
*  Security in using services is quarantied by controlling the rights to list 
*  information of the theme set installed as well as to load individual themes.
*  The access control is based on SECUREID of application process.
*  Security is maintained behind the curtain by means of Symbian EKA2 Kernel 
*  services for Platform Security.
*
*  Scalability
*  ===========
*  Architecture of UI Manager supports scalability in next way: Services 
*  provided on server-side reflects the client-side APIs. Client-side objects 
*  implements exactly the same APIs than services on server-side. This approach
*  is selected to support scalability of the theme management system; 
*  small and one-application systems, for e.g. MP3-player, all services could
*  be implemented on client-side, meanwhile large systems can increase their 
*  performance and features providing powerful and centralised server-side 
*  theme handling operations like OTA Push, theme caching, theme editing etc. 
*  HSPS UI Manager implementation is targeted for medium-wide systems running
*  on Symbian OS 9.x, typically serving multiple applications and operators
*  on-line operations customization on single smartphone.  
*
*  HSPS Utility Classes
*  ======================
*  HSPS Utility Classes are used as parameters in HSPS Application Theme 
*  Management service calls. They are owned by serveice caller. Utility 
*  classes are kind of data objects and therefore they are not defined with 
*  specific interface (M-classes). Utility classes are the following:
*  - ChspsODT, and 
*  - ChspsResource. 
*
*  ChspsODT
*  ------
*  HSPS ChspsODT-class, nick-named ODT (Object Description Tree), class is a
*  base unit of information exchanged in HSPS Application Theme Management 
*  operations. 
*  ODT consist of an header-part and its associated DOM-document member class
*  on which it has a dependency. The header contains all information what is
*  needed to distinquish application themes from each others. 
*  HSPS Application Theme Management services handles mainly these 
*  ODT-headers, however, the DOM-document is accessed for applying theme 
*  updates by Installation Handler and to serve the theme usage requests from
*  Rendering Engines. For more information, see ChspsODT documentation. 
*  
*  ChspsResource
*  -----------
*  HSPS ChspsResource-class defines all information what is needed to maintain
*  resources of Application Theme. Every ChspsResource-objects defines just one 
*  resource source, however, the same resource source coud be referenced
*  multible times inside a theme - resources are usually shared. 
*  HSPS Application Theme Management system supports theme resourcing by 
*  offering resource conversations and storing services, and offering Rendering
*  Engines the following services:
*  1) the way to match a resource quoted in xml- or css-module to the 
*  corresponding resource source, and 2) a secure way to access a resource 
*  through the Symbian Platform Security's process separation wall.
*
*  ChspsResult
*  ---------
*  HSPS ChspsResult-class defines feature called Additional Return Code support
*  for HSPS Application Theme Management Services on service request return.
*  It is quaranteed that ChspsResult-object is always accessible after
*  client request whether the result was successful or not.
*  ChspsResult-class has attributes that informs the result as follows:
*  - iSystemError - Symbian OS returned error code
*  - iHSPSError - HSPS defined error code in HSPS error space
*  - iIntValue1   - additional information relevant the result. 
*  - iIntValue2   - additional information relevant the result.
*
*
*
*/


#ifndef __hspsTHEMEMANAGEMENT_H__
#define __hspsTHEMEMANAGEMENT_H__

#include <f32file.h> // RFile
#include <badesca.h> // descriptors

// --------------------
const TUid KSecureId_Psln =  	{0x10005A32};	// from S60 3.1 Plsn
const TUid KSecureId_RDAS =  	{0x10210EA1};	// 270601889 from S60 3.1 R&D AppShell for Xuikon
const TUid KSecureId_hspsAS =  	{0x101F4CD2};	// 270486738 from S60 3.1 Xuikon AppShell in 3.1 product.
const TUid KSecureId_hsps =    	{0x200159C6};	// HSPS configuration installer (ThemeInstallerCons.exe)
const TUid KSecureId_LE =    	{0x102049AD};
const TUid KSecureId_XTA =   	{0x1020747D}; 	// Xuikon Test Automation
const TUid KSecureId_XKON =  	{0x102049AF}; 	// Xuikon Demo
const TUid KSecureId_hspsAI =  	{0x102750F0}; 	// 271012080 support for Xuikon-based ActiveIdle 
const TUid KSecureId_GS =    	{0x100058EC}; 	// S60 3.1 GS-application(General Settings).
const TUid KSecureId_HUIMenu = 	{0x102828BD}; 
const TUid KSecureId_EUnit = 	{0x20000FB1};
const TUid KSecureId_Test =     {0x200159C5};   // HSPS ThemeManager Testapp (internal folder)


// -----------------

/**  Nokia VID: = VID_DEFAULT */
const TInt Nokia_VID  = 0x101FB657;

/** 
*   KhspsThemeStatusRepositoryUid. 
*   Central repository entry for HSPS. 
*/
const TUid KhspsThemeStatusRepositoryUid = {0x200159C9};

/** 
*   KMaxHeaderDataLength8. Maximun number of data bytes reserved for ODT-header.
*   This is same as the maximum file path length expressed in unicode chars
*/
const TInt KMaxHeaderDataLength8=512;

/** KHeaderListGranularity. List granularity used in header listing operations: */
const TInt KHeaderListGranularity = 4;

/** 
*   KHeaderListUpdatePollingTimeSpan. The time between the subsequent 
*   request to check changes in Definition Repository. 
*   The value is expressed in microseconds. 
*/
const TInt KHeaderListUpdatePollingTimeSpan = 1000000;

/** 
*   @deprecated KMaxElementDataLength. The maximum size of buffer reserved to
*   internalize theme. 
*/
const TInt KMaxElementDataLength = 1048576;

/** 
*   KPathListGranularity. Controls the granularity of the path list retrieved
*   from Definition Repository. 
*/
const TInt KPathListGranularity = 4;


/** 
*   KMahspsumberOfThemeLoadRepeats. Controls the maximum number of repeats when delivering 
*   the application theme and its resource list data in memory chunk
*   from the server side to the client size. Value 0 means infinite repeats.
*/
const TInt KMahspsumberOfThemeLoadRepeats = 10; // value 0 means infinite repeats.

/** 
*   KThemeLoadRepeatWaitTime. Controls the time to wait between the 
*   tryes to delivere application theme and its resource list data from the server 
*   side to the client size.
*/
const TInt KThemeLoadRepeatWaitTime = 5000000; // 5 seconds

/** 
*   KActiveThemeUpdatePollingTimeSpan. 
*   Constant KActiveThemeUpdatePollingTimeSpan controls the time slice 
*   between the checking the theme status change theme and/or resource updates. 
*   The value is expressed in microseconds.
*/
const TInt KActiveThemeUpdatePollingTimeSpan = 1000000; // 1 second

const TInt KMaxPluginIdLenght = 8;

/** 
*   KThemeInvalidatedTimeSpan
* Controls the time to wait for client application to notify it has rendered the theme
* successfully. If the notification is not received in time (or clientsession crashes)
* the default operator or licensee theme is restored.
*
* When client app. starts it requests theme with GetOdt-request
* After the client receives the theme it starts rendering it. 
* After succesfull rendering the client app subscribes for theme updates
* with GetODTUpdate-request. ThemeServer starts a timer when receiving 
* GetODT and if no GetODTUpdate- request if received before the timer expires
* the default operator or licensee theme is restored.
*
*/
const TInt KThemeInvalidatedTimeSpan = 120000000;

/** 
 * Client initiated service requests: an enumeration of function indexes
 *  
 * NOTE: hspsThemeRanges array in hspsThemeServer.h file needs to be in sync with the following enumerations,
 * failing to do so results in -5 (Not supported) errors.
 * 
 */
enum ThspsServiceRequestMessage
  {
  
  /**************************************
   * Installation related requests: 
   *************************************/
  EhspsInstallationBase = 0,
  
  /** 
   *	EhspsInstallTheme. 
   * 	Initiates synchronous and asynhronous theme installation process. 
   */
  EhspsInstallTheme = EhspsInstallationBase,
    
  /** 
   * 	EhspsInstallNextPhase:
   *	Promotes subsequent asyncronous installation phases 
   * 	and offers client process a point to cancel installation.    
   */
  EhspsInstallNextPhase,
  
  /** 
   * 	EhspsCancelInstallTheme:
   * 	Cancels asynchronous installation process. 
   */
  EhspsCancelInstallTheme,

  /** 
   *    EhspsReinstallConf:
   *    Initiates synchronous configuration's reinstallation process 
   */
  EhspsReinstallConf,
  
  
  /**************************************
   * Maintenance related requests: 
   *************************************/  
  EhspsMaintenanceBase,
  
  /** 
   *	EhspsGetListHeaders. 
   *   	Initiates listings of theme headers by setting a query on server. 
   */      
  EhspsGetListHeaders = EhspsMaintenanceBase,
    
  /**	
   * 	EhspsGetNextHeader:
   * 	Subscribes updates for received header list. 
   */ 
  EhspsGetNextHeader,
  
  /**	
   * 	EhspsCancelGetListHeaders. 
   * 	Cancels a subscription of header list updates. 
   */
  EhspsCancelGetListHeaders,
    
  /**	
   * 	EhspsSetActiveTheme. 
   * 	Theme activation. 
   */
  EhspsSetActiveTheme,
  
  /** 
   *   	EhspsRestoreDefault. 
   *	Restores the default theme in next order: 
   *   	1. Restore the user default theme if one exists.
   *   	2. Restore the operatot default theme if one exist.
   *   	3. Finally, as the last resource, restore the licensee default theme. 
   *   	Licencee default theme is located on ROM (Z-drive), therefore it is
   *   	quaranteed that application is in working condition allways.
   */
  EhspsRestoreDefault,
  
  /** 
   *   	EhspsRemoveTheme. 
   * 	Removes a given theme with all its dependent files and 
   *   	settings. Does not remove the theme which is currently active. 
   *   	Cannot remove the licencee default theme located on ROM, however, it can
   *   	remove possible updates on the licencee default them if not active.
   */
  EhspsRemoveTheme,
  
  /** 
   *  	EhspsAddPlugin. 
   * 	Adds a plug-in configuration into active application configuration.
   */
  EhspsAddPlugin,
  
  /** 
   *  	EhspsRemovePlugin. 
   * 	Removes a plug-in configuration from active application configuration.
   */
  EhspsRemovePlugin,

  /** 
   *    EhspsSetActivePlugin. 
   *    Set active plugin.
   */
  EhspsSetActivePlugin,

  /** 
   *    EhspsReplacePlugin. 
   *    Replaces a plug-in in an active application configuration.
   */
  EhspsReplacePlugin,
    
  /** 
   *    EhspsSetSettings. 
   *    Sets settings of plugin in active configuration
   */
  EhspsSetPluginSettings,
  
   /** 
    *    EhspsGetPluginOdt. 
    *    Gets plugin odt by UID
    */
   EhspsGetPluginOdt,

  /** 
   *  	EhspsMovePlugins 
   * 	Updates plugin positions within a configuration
   */
  EhspsMovePlugins,

  /**   EhspsSetConfState 
   *    Updates configuration's state. 
   */
  EhspsSetConfState,

  /**   EhspsRestoreActiveAppConf 
   *    Restore active application configuration in following order
   *    1) Restore application configuration from backup folder
   *    2) Activate licensee default restorable configuration
   *    3) Reinstall licensee default restorable configuration  
   */
  EhspsRestoreActiveAppConf,
  /** 
   *  Updating plugin configuration
   */
    
  EhspsUpdatePluginConf,
  

  /**************************************
   * Client Request related requests: 
   *************************************/
  EhspsClientRequestBase,
  
  /** 
   *   	EhspsGetODT:
   *	Get currently active theme and resource list for specified application.    
   */    
  EhspsGetODT = EhspsClientRequestBase,  
    
  /** 
   * 	EhspsGetODTUpdate. 
   * 	Subscribes theme status change and theme update messages.  
   */
  EhspsGetODTUpdate,
    
  /**
   * 	EhspsCancelGetODTUpdate. 
   * 	Cancels a subscription of theme updates. 
   */
  EhspsCancelGetODTUpdate,
    
  /** 
   *   	EhspsAccessResourceFile. 
   *	Request access to resource file on hspsThemeServer private folder.
   */
  EhspsAccessResourceFile,
    
  /**
   *    EhspsCopyResources
   *    Copies resource files to the destination folder.
   *    Given destination folder is only prefix for actual destination folder.
   */
  EhspsCopyResources,
  
  /**************************************
   *	Out of range 
   *************************************/  
  EhspsNotSupported  
  };
  
/** 
*  ThspsServiceCompletedMessage. Response codes used by client-server 
*  communication to indicate a result of the completion of requested service. 
*/    
enum ThspsServiceCompletedMessage
    {
/** Installation related responses: */

    /** 
     * EhspsInstallThemeSuccess. Theme installation was successful. 
     */
    EhspsInstallThemeSuccess,

    /** 
     *  EhspsInstallThemeFailed. Theme instalation was failed. 
     */
    EhspsInstallThemeFailed,
        
    /** 
    *   EhspsInstallPhaseSuccess. Installation service was performed an 
    *   installation phase successfully. 
    */
    EhspsInstallPhaseSuccess, 

    /** 
     * EhspsReinstallConfSuccess. Configuration reinstallation was successful. 
     */
    EhspsReinstallConfSuccess,

    /** 
     *  EhspsReinstallConfFailed. Configuration reinstalation was failed. 
     */
    EhspsReinstallConfFailed,

/** Maintenance: */
    
    /** 
    *   EhspsGetListHeadersSuccess. Query of a header list was successful. 
    *   Client must start listen the delivery events.
    */
    EhspsGetListHeadersSuccess,    
    
    /** 
     *   EhspsGetListHeadersFailed. Headers cannot be listed. 
     */
    EhspsGetListHeadersFailed, 
    
    /** 
     *   EhspsGetListHeadersUpdate. A header is arrived. 
     */
    EhspsGetListHeadersUpdate, 
    
    /** 
     *  EhspsGetListHeadersUpdateData
     *  EhspsGetListHeadersUpdate for observing low-level API-calls. 
     */
    EhspsGetListHeadersUpdateData, 
    
    /** 
     *   EhspsGetListHeadersEmpty. Header list is empy - no matching headers on 
     *   current query. Client must clear the list screen. 
     */
    EhspsGetListHeadersEmpty,
      
    /** 
     *   EhspsGetListHeadersNoChange. No change in headers. Server side only. 
     */
    EhspsGetListHeadersNoChange,
   
    /** 
     *   EhspsGetListHeadersRestart. Header list is canged radiacally. Listing 
     *   must start from the begin. Client must clear the list screen. 
     */
    EhspsGetListHeadersRestart,
    
    /** 
     *   EhspsGetListHeadersRestartData. 
     *   EhspsGetListHeadersRestart for observing low-level API-calls.
     */
    EhspsGetListHeadersRestartData,
    
    /** 
     * EhspsSetActiveThemeSuccess. Theme activation was successful. 
     */ 
    EhspsSetActiveThemeSuccess, 
    
    /** 
     * EhspsSetActiveThemeFailed. Theme activation has failed. 
     */
    EhspsSetActiveThemeFailed, 
    
    /** 
     * EhspsRestoreDefaultSuccess. Restoring the default theme was successful. 
     */
    EhspsRestoreDefaultSuccess,
    
    /** 
     * EhspsRestoreDefaultFailed. Restoring the default theme was failed. 
     */
    EhspsRestoreDefaultFailed,
    
    /** 
     * EhspsRemoveThemeSuccess. Theme removal was successful. 
     */
    EhspsRemoveThemeSuccess, 
    
    /** 
     * EhspsRemoveThemeFailed. Theme removal was failed. 
     */
    EhspsRemoveThemeFailed,
    
    /** 
     * EhspsAddPluginSuccess. A plugin was added succesfully. 
     */
    EhspsAddPluginSuccess,
    
    /** 
     * EhspsAddPluginFailed. Failed to add a plugin. 
     */
    EhspsAddPluginFailed,
    
    /** 
     * EhspsRemovePluginSuccess. A plugin was removed succesfully. 
     */
    EhspsRemovePluginSuccess,
    
    /** 
     * EhspsRemovePluginFailed. Failed to remove a plugin. 
     */
    EhspsRemovePluginFailed,

    /** 
     * EhspsSetActivePluginSuccess. A plugin was activated succesfully. 
     */
    EhspsSetActivePluginSuccess,    

    /** 
     * EhspsSetActivePluginFailed. Failed to activate a plugin. 
     */
    EhspsSetActivePluginFailed,    
    
    /** 
     * EhspsSetSettingsSuccess. A plugin settings was updated succesfully. 
     */
    EhspsSetPluginSettingsSuccess,
        
    /** 
     * EEhspsSetSettingsFailed. Failed to set settings of plugin. 
     */
    EhspsSetPluginSettingsFailed,
    
    /** 
     * EhspsMovePluginsSuccess. A plugins list was updated succesfully. 
     */
    EhspsMovePluginsSuccess,

    /** 
     * EhspsMovePluginsFailed. Failed to update a plugins list.
     */
    EhspsMovePluginsFailed,

    /** 
     * EhspsReplacePluginSuccess. A plugin was replace succesfully. 
     */
    EhspsReplacePluginSuccess,

    /** 
     * EhspsReplacePluginFailed. Failed to update a plugins list.
     */
    EhspsReplacePluginFailed,
    
    /**
     *  EhspsGetPluginOdtSuccess. Getting plugin odt was successful.
     */
    EhspsGetPluginOdtSuccess,
    
    /** 
     * EhspsGetPluginOdtFailed. Failed to get plugin odt.
     */
    EhspsGetPluginOdtFailed,
    
/* Client Requests: */
    
    /** 
    *   EhspsGetODTSuccess. 
    *   ODT and associated resource list (if any) were received successfully. 
    */
    EhspsGetODTSuccess, 
    
    /** 
     * EhspsGetODTFailed. ODT was not received. Request failed.
     */
    EhspsGetODTFailed,
    
    /** 
    *   EhspsGetODTLowMemoryStrategy. Memory chunk big enough could not be created
    *   by the server, switching to low memory strategy. 
    */
    EhspsGetODTLowMemoryStrategy,
    
    /** 
    *   EhspsGetODTUpdateSuccess. Subscription of the theme updates and status 
    *   changes was successful. Cient must start observe the server events. 
    */
    EhspsGetODTUpdateSuccess,
    
    /**
    * 	EhspsSettingsLoaded. Loading settings for a theme was successful.
    */
    EhspsSettingsLoaded,
    
    /**
    *	EhspsSettingsLoadFailed. Loading settings for a theme failed.
    */
    EhspsSettingsLoadFailed,
    
    /**
    *	EhspsSettingsUpdated. Updating settings for a theme was successful.
    */
    EhspsSettingsUpdated,
    
    /**
    *	EhspsSettingsUpdateFailed. Updating settings for a theme failed.
    */
    EhspsSettingsUpdateFailed,
    /** 
    *   EhspsGetODTUpdateFailed. 
    *   Subscription of the theme updates and status changes failed. 
    */
    EhspsGetODTUpdateFailed, 
    
    /** 
    *   EhspsGetODTUpdateStatus. Theme status is changed. 
    *   Client must reset rendering and request theme again. 
    */
    EhspsGetODTUpdateStatus, 
    
    /** EhspsGetODTUpdateHot. Theme is updated. Client should reload the theme. */
    EhspsGetODTUpdateHot,
  
    /** 
    *   EhspsGetODTUpdateEmpty. 
    *   No update available. Client may retain subscription. 
    */
    EhspsGetODTUpdateEmpty,
  
    /** EhspsGetODTUpdateNone. No update available. Used only on server-side. */
    EhspsGetODTUpdateNone,

    /** 
    *   EhspsAccessResourceFileSuccess. 
    *   Request to get access to a theme resource file was successful. 
    */
    EhspsAccessResourceFileSuccess,
    
    /** 
    *   EhspsAccessResourceFileFailed. Request to get access to a theme resource
    *   file was successful. Possible in due to security fault.
    */
    EhspsAccessResourceFileFailed,
    
/** Generic: */
    /** 
    *   @deprecated EhspsThemeServiceCanceled. 
    *   Server has cancelled the service request possible in due to client 
    *   cancellation request. Usually this is not handled by client because of
    *   the cancelled request on client-side. This protocol should not cause any
    *   problem.
    */  
    EhspsServiceRequestCanceled,    
    
    /** 
    *   EhspsServiceRequestSheduled. Server has sheduled the asynchronous service 
    *   request. Client must start observe the server events. 
    */
    EhspsServiceRequestSheduled,
    
    /** 
    *   EhspsServiceNotSupported. 
    *   Server does not support servinng this request in current implementation.
    */
    EhspsServiceNotSupported,
    
    /** 
    *   EhspsServiceRequestError. Server rejects the service request maybe because
    *   the semaphore for this service in rised, or The request was violating 
    *   the security. Addidional return Code tells the exact reason.
    */  
    EhspsServiceRequestError,
    
    /** 
    *   EhspsResourceCopyFailed.
    */  
    EhspsResourceCopyFailed,

    /** 
    *   EhspsResourceCopySuccess.
    */  
    EhspsResourceCopySuccess,

    /** 
    *  Configuration state changed successfully
    */  
    EhspsSetConfStateSuccess,

    /** 
    *  Configuration state change failed
    */  
    EhspsSetConfStateFailed,

    /** 
    *  Active application configuration restored successfully
    */  
    EhspsRestoreActiveAppConfSuccess,

    /** 
    *  Active application configuration restoring failed
    */  
    EhspsRestoreActiveAppConfFailed,
    /** 
    *  Updating plugin configuration failed
    */
    EhspsUpdatePluginFailed,
    /** 
     *  Updating plugin configuration succeed
     */ 
    EhspsUpdatePluginSuccess,
    
    /**
    * Resolution changed, theme should be changed (emulator environment) 
    */
    EhspsResolutionChangedUpdate    

    };  

 
/** 
*   ThspsThemeStatus. 
*   Theme status flag-definitions. Theme status flags are bit-masked. 
*/
enum ThspsThemeStatus
  {
  /** EhspsThemeStatusNone. Theme has no specific staus. This is a common case. */
  EhspsThemeStatusNone            = 0x0000, //0b0000000000000000,
  
  /** 
  *   EhspsThemeStatusActive. 
  *   This theme is currently activated in scope of its application. 
  */
  EhspsThemeStatusActive          = 0x0001, //0b0000000000000001,
  
  /** 
  *   EhspsThemeStatusLicenceeDefault. 
  *   This theme is licencee default. It is located on ROM (Z-drive) 
  */
  EhspsThemeStatusLicenceeDefault = 0x0002, //0b0000000000000010,
  
  /** EhspsThemeStatusOperatorDefault. This theme is set as operator default. */
  EhspsThemeStatusOperatorDefault = 0x0004, //0b0000000000000100,
  
  /** EhspsThemeStatusUserDefault. This theme has set as user default. */
  EhspsThemeStatusUserDefault     = 0x0008,  //0b0000000000001000
  
  /** EhspsThemeStatusMakeActive. Activates the theme after installation. */
  EhspsThemeStatusMakeActive     = 0x0010,  //0b0000000000010000

  /** EhspsThemeStatusClean. Removes the RAM-installed themes on default restoration. */
  EhspsThemeStatusClean    = 0x0020,  //0b0000000000100000
  
  /** 
  *   EhspsThemeStatusLicenceeRestorable. This theme is restored when licensee default 
  *   theme is restored. When using this flag, the ThemeStatusLicenceeDefault
  *   -flag is also automatically activated. It is located on ROM (Z-drive) 
  */
  EhspsThemeStatusLicenceeRestorable = 0x0040, //0b00000000001000000,
  
  EhspsThemeStatusLocked = 0x0080, //0b00000000010000000,
  /** EhspsThemeStatusHitchcock. Indicates that the theme is meant for a Hitchcock application. */
  EhspsThemeStatusHitchcock = 0x0100, //0b00000000100000000
  /** EhspsThemeStatusIncludeControlBlock. Includes control block to ODT when called with hspsGetODT. */
  EhspsThemeStatusIncludeControlBlock = 0x0200 //0b00000001000000000
  };  


/**
 *   ThspsInstallationPhase.
 *   The phases of the installation state-machine in Installation Service. 
 *   In case of failed or canceled installation, the Roll-back process will take 
 *   place from the phase where the instaaltion was before the failure.
 */
enum ThspsInstallationPhase
  {
  /** EhspsPhaseIdle. */
  EhspsPhaseIdle,  
  
  /** EhspsPhaseInitialise. Header ok and query validity check */
  EhspsPhaseInitialise,
  
  /** EhspsPhaseCleanup. */
  EhspsPhaseCleanup,
      
  /** EhspsPhaseInstallODT. */
  EhspsPhaseInstallSkeleton,
  
  /** EhspsPhaseImportPlugins. */
  EhspsPhaseImportPlugins,
    
  /** EhspsPhaseRollBack. */
  EhspsPhaseRollBack
  };


/**
 * ThspsFamily.
 * Identifies resolution for which the widget for designed for.
 * Should be in sync with the hspsmanifest.h file. 
 */
enum ThspsFamily
    {
    EhspsFamilyUnknown = 0x00,
    EhspsFamilyQvga = 0x01,
    EhspsFamilyQvga2 = 0x02,
    EhspsFamilyVga = 0x04,
    EhspsFamilyVga3 = 0x08,
    EhspsFamilyQhd = 0x10,
    EhspsFamilyQhd_tch = 0x20,
    EhspsFamilyVga_tch = 0x40
    };

/**
 * ThspsConfigurationType.
 * Indentifies the configuration type, which is diffrent for application and plugin themes.  
 */
enum ThspsConfigurationType
    {
    EhspsAppConfiguration = 1,
    EhspsViewConfiguration,
    EhspsWidgetConfiguration,
    EhspsTemplateConfiguration    
    };

/**
 * ThspsConfigurationState.
 * Indentifies the configuration state  
 */
enum ThspsConfigurationState
    {
    EhspsConfStateNotConfirmed = 1,
    EhspsConfStateWaitForConfirmation,
    EhspsConfStateConfirmed,
    EhspsConfStateError  
    };

/**
 * ThspsConfStateChangeFilter
 * Indentifies the configuration state change options
 */
enum ThspsConfStateChangeFilter
    {
    EhspsConfStateChangeNoFilter = 1,
    EhspsConfStateChangePlugins
    };

/**
 * ThspsConfiguration.
 * A definition for passing several uids from client to server session.
 */
struct ThspsConfiguration
    {
    // Application uid or Interface uid
    TInt rootUid;
    
    // Theme uid
    TInt themeUid;
    
    // Type of the configuration
    ThspsConfigurationType type;
    };


/**
 * Input parameters for the AddPlugin service
 */
struct ThpsParamAddPlugin
	{
	TInt appUid;					// uid of the application configuration
	TInt configurationId;			// id of the configuration being modified 
	TInt pluginUid;					// uid of the plugin configuration to be added
	TInt positionIndex;				// position of the added plugin in the configuration's plugins list 
	};

/**
 * Input parameters for the RemovePlugin service
 */
struct ThpsParamRemovePlugin
    {
    TInt appUid;                    // uid of the application configuration
    TInt pluginId;                    // id of the plugin configuration instance
    };

/**
 * Input parameters for the ActivatePlugin service
 */
struct ThpsParamSetActivePlugin
    {
    TInt appUid;                    // uid of the application configuration
    TInt pluginId;                  // id of the plugin configuration instance
    };

/**
 * Input parameter for the GetSettings service 
 */
struct ThspsParamGetPluginOdt
	{
	TInt appUid;					// uid of the application configuration
	TInt pluginUid;					// uid of the plugin
	};

/**
 * Input parameters for the MovePlugins service
 */
const TInt KMaxPluginIdsLength = 2048;
struct ThpsParamMovePlugins
	{
	TInt appUid;					// uid of the application configuration
	TInt configurationId;			// id of the configuration being modified 
	TBuf8<KMaxPluginIdsLength> pluginIdsBuf; // descriptor for passing externalized plugin ids
	};

/**
 * Input parameter for the SetConfState service 
 */
struct ThspsParamSetConfState
    {
    TInt appUid;                       // uid of the application
    TInt confId;                       // id of the configuration
    ThspsConfigurationState state;     // new configuration state
    ThspsConfStateChangeFilter filter; // filter parameter
    };

/**
 * Input parameter for the ReinstallConf service 
 */
struct ThspsParamReinstallConf
    {
    TInt appUid;                    // uid of the application
    TInt confUId;                   // uid of the reinstalled configuration
    };

/**
 * Input parameter for the RestoreActiveAppConf service 
 */
struct ThspsParamRestoreActiveAppConf
    {
    TInt appUid;                    // uid of the application
    TInt confUid;                   // uid of the restored configuration
    };

/**
 * Input parameter for the ReplacePlugin service 
 */
struct ThspsParamReplacePlugin
    {
    TInt appUid;                    // uid of the application
    TInt pluginId;                  // id of the existing plugin to be replaced
    TInt confUid;                   // uid of the new plugin configuration
    };
/**
 * Input parameter for the ReplacePlugin service 
 */
struct ThspsParamSetPluginSettings
    {
    TInt pluginId;                  // id of the existing plugin to be replaced
    TBool storingStatus;            // storing status telling if modified plugin settings are needed to stored its reference.
    };


class ChspsODT;
class ChspsResource;
class ChspsDomDocument;
class ChspsRequestNotificationParams;
/**
*  Part of HSPS Plugin Configuration Management Service APIs.
*  MhspsThemeManagementServiceObserver. Interface definition for the HSPS 
*  Installation and Maintenance Service related events observation.
*  The call-back function HandlehspsClientMessage() must be inplemented by client 
*  application if it want to handle messages coming from the 
*  hspsInstallationHandler and hspsMaintenanceHandler. 
*  For more information of different Installation and Maintenance Service 
*  related messages, see the documentaion of ThspsServiceCompletedMessage.
*
*  @since S60 5.0
*  @ingroup group_hspsclients
*/
class MhspsThemeManagementServiceObserver
    {
    public:
                
        /**
        * HandlehspsClientMessage
        * 
        * @since S60 5.0
        * @param aMessage contains a service completion message returned from 
        *        the hspsInstallationHandler and hspsMaintenanceHandler to their 
        *        observer.
        */
        virtual void HandlehspsClientMessage(ThspsServiceCompletedMessage aMessage) = 0;
    };

/**
*  Part of HSPS Plugin Configuration Management Service APIs.
*  MhspsClientRequestServiceObserver. Interface definition for the HSPS Client 
*  Request Service events observation. 
*  The call-back function HandlehspsRequestClientMessage() must be inplemented by
*  the client applications if it want to handle messages coming from the 
*  hspsClientRequestHandler. 
*  For more information of different Client Request Service related messages, 
*  see the documentaion of ThspsServiceCompletedMessage.
*
*  @since S60 5.0
*  @ingroup group_hspsclients
*/
class MhspsClientRequestServiceObserver
    {
    public:
        /**
        * HandlehspsRequestClientMessage
        * 
        * @since S60 5.0
        * @param aMessage contains a service completion message returned from 
        *        the hspsClientRequestHandler to its observer.
        * @param aParams contains data for changes what are observed
        */
        virtual void HandlehspsRequestClientMessageL(ThspsServiceCompletedMessage aMessage, ChspsRequestNotificationParams& aParams ) = 0;
    };

/**
*  Part of HSPS Plugin Configuration Management Service APIs.
*  MhspsInstallationService. API-definition for HSPS Plugin Installation Service.
*  HSPS Plugin Installation Service is intended to serve Homescreen as well as S60 
*  legacy applications - expecially the Personalisation Application - with the 
*  Homescreen Application Configuartion installation functionality. Implementation of this 
*  interface will offer high- and low-level API-functionality. 
*  High-level and low-level calls are diffrentiated by their parametrization. 
*  High-level parametrization is directed to the be used by applications which 
*  need human readable data and offer a user interface. Low-level 
*  parametrization is directed to the machine-originated requestors like 
*  OTA Push or DM (Device Management) sub-systems where data is not needed to
*  understand semanticly.
*
*  Processing a installation task works in asynchronous mode, howvere, synchronous mode is applied 
*  everytime the installation is initiated. After checking installation rights and files valididies,
*  hsps Theme Server will allow actual installation. Client application must call actual  
*  installation by commencing next phase execution explicitly. First call of installation returns 
*  the ODT-header of the teme to be installed. This is to enable previewing information about 
*  a theme to be installed. In this point, user can accept or decline the installation. 
*  Installation preview feature is useful expecially in OTA-service cases when user acception on 
*  theme installation might be selected a a part of user policy settings.
*  A theme can include resources that can be imported to the target system as such like some audio
*  files etc., however, most of the images must be fitted on to the target device's color depth and
*  pixel density. In these cases, the needed conversions will be executed asynchronously on 
*  server-side. Asynchronous installation is executed in phases - resource by resource. 
*  Installation phases are promoted automaticly, however, client application can control and 
*  monitor installation phases, and also cancel installation at any time. If installation is 
*  canceled, hspsThemeServer initiates roll-back functionality removing inconsistent installation.  
*
*  Client application must call hspsInstallNextPhase() after testing that the synchronous call 
*  hspsInstallTheme() has returned EhspsInstallPhaseSuccess return code signalling that actual 
*  installation is allowed. The rest of the installation phases will be executed automatically,
*  however, installation can be interrupted by calling hspsCancelInstallTheme() at any time. 
*  If installation is canceled, hspsThemeServer initiates roll-back functionality removing
*  inconsistent installation.
*  Installation service uses specific manifest-file format to get informed about the xml, css, 
*  dtd, and resource files to be installed. Resource files might be locale-specific or generic
*  as well.
*  hspsThemeServer takes care of saving everything on their appropriate places in the target device's
*  user disk. Theme storage is located in hspsThemeServer's private-folder.
*  Locales are instructed in manifest file also. Manifest file's file-extension must be .dat, 
*  but actually, the file-name can be whatever, however, when low-level parametrization is used, 
*  the name must be "manifest.dat" and it must be the last file extracted from the installation 
*  package.
*  For more information of manifest-file format, see "Xuikon Manifest File User Guide.doc".
*
*  Installation functions can return one of the following 
*  ThspsServiceCompletedMessage-codes:
*  - EhspsInstallThemeSuccess,
*  - EhspsInstallPhaseSuccess, or
*  - EhspsInstallThemeFailed.
*
*  Client application must implement MhspsThemeManagementServiceObserver-interface
*  and listen these messages
*  mentionaed. Let it be emphasised that both synchronous and asynchronous calls 
*  can return the codes above.
*  Installation functions may also return one of the following codes:
*  - EhspsServiceRequestSheduled, or
*  - EhspsServiceRequestError.
*
*  For explanation of the meanings of these messages, see 
*  ThspsServiceCompletedMessage-documentation.
*   
*  @since S60 5.0
*  @ingroup group_hspsclients
*/
class MhspsInstallationService
    {
    public:
        /**
        * hspsInstallTheme
        * 
        * @since S60 5.0
        * @param aManifestFileName is the full path to the installation script file - a manifest 
        *        file.
        * @param aHeader is an empty ChspsODT-object in which a valid ODT-header of the newly 
        *        installed theme will be returned if the request is successful. The use of 
        *        ChspsODT-type parameter follows the high-level parametrization schema.
        * @return ThspsServiceCompletedMessage expressing the result of the call.
        */
        virtual ThspsServiceCompletedMessage hspsInstallTheme(const TDesC& aManifestFileName,
                                  ChspsODT& aHeader) = 0;

        /**
        * hspsInstallTheme
        * 
        * @since S60 5.0
        * @param aManifestFileName is full path of the installation script file - a manifest file
        * @param aHeaderData will return ODT-header of the newly installed theme in serialized 
        *        (i.e. marshalled) data mode. This follows the low-level parametrization schema.  
        * @return ThspsServiceCompletedMessage expressing the result of the call.
        */
        virtual ThspsServiceCompletedMessage hspsInstallTheme(const TDesC& aManifestFileName,
                                  TDes8& aHeaderData) = 0;
                        
        /** hspsInstallNextPhase
        *
        * @since S60 5.0
        * @param aHeader is an empty ChspsODT-object in which a valid ODT-header of the latest 
        *        installation phase completed if the request was successful. 
        *        The use of ChspsODT-type parameter follow the high-level parametrization schema.
        * @return ThspsServiceCompletedMessage expressing the result of the call.
        */
        virtual ThspsServiceCompletedMessage hspsInstallNextPhaseL(ChspsODT& aHeader) = 0;
        
        /** hspsInstallNextPhase
        * 
        * @since S60 5.0
        * @param aHeaderData will return ODT-header of the latest installation phase in 
        *        serialized (i.e. marshalled) data mode. This follows the low-level 
        *        parametrization schema.  
        * @return ThspsServiceCompletedMessage expressing the result of the call.
        */
        virtual ThspsServiceCompletedMessage hspsInstallNextPhaseL(TDes8& aHeaderData) = 0;
      
        /** hspsCancelInstallTheme
        * 
        * @since S60 5.0
        * @return ThspsServiceCompletedMessage expressing the result of the call.
        */
        virtual ThspsServiceCompletedMessage hspsCancelInstallTheme() = 0;

    };
    
/**
*  Part of Xuikon Application Theme Management Service APIs.
*  MhspsMaintenanceService. API-definition for Xuikon Theme Maintenance Service.
*  Xuikon Theme Maintenance Service is intended to serve Xuikon applications as well as S60 legacy 
*  applications - especially Personalisation Application - with Theme Maintenance Services. 
*  Theme maintenance can take place after a set of themes are installed by Xuikon Theme Installation
*  Service. Maintenance services includes theme listings, theme activation, theme removal, and 
*  default theme restoring functionality.
*  Maintenance functions are presented in high-level and low-level parametrization mode. 
*  This is the same approach that was introduced with Xuikon Theme Installation Service - to support 
*  user-intefaces with human-readable high-level elements and on otherhand, machine-originated users 
*  with low-level data. Most of the maintenance functions are synchronous, only one is asynchronous;
*  hspsGetNextHeader(). Others functions are stright forward to use but hspsGetListHeaders(). This will 
*  be explained next; synchronous call hspsGetListHeaders() initiates the theme header listing by 
*  passing search mask to Xuikon Theme Server's Maintenance Service. This search mask is called 
*  a query. It also passes a list object in where maintenance service should append theme-header 
*  objects (type of ChspsODT-class without DOM-document) when one is retrieved asychronously. 
*  Search mask should be filled with proper parameters matching the need in hand. 
*  If no parameters are given, all headers of Application Themes available in Definition Repository
*  will be delivered, otherwise, only sub-set of theme-headers will be delivered. 
*  After the query-call hspsGetListHeaders(), the delivering of the theme-headers is asynchronous.
*  Asynchronous service must be initiated by calling hspsGetNextHeader() after checking that any 
*  theme matching on current query is found. This action is called a subscription. To receive theme
*  listing, the client application must implement MhspsThemeManagementServiceObserver-interface and 
*  start to listen call-back messages. The headers matching on query will be delivered immediately. 
*  Query remains until hspsCancelGetListHeaders() is called by client. If a new Application Theme 
*  exist in repository, it will be delivered. As mentioned, when hspsGetListHeaders() returns, 
*  the return value must be checked. Return value could be one of the following:
*  - EhspsGetListHeadersSuccess - there is at least one Application Theme available matching on query. Call first
*                               hspsGetNextHeader() to get the headers. Same call will set the subscribtion of 
*                               new ones possible coming available later.
*  - EhspsGetListHeadersEmpty   - there is no themes matching on the query available at the time, however, some might 
*                               be exist later in due to installations. Client application should retain the
*                               subcription.
*  - EhspsGetListHeadersFailed  - service request failed in some reason. Possible reasons are for e.g. the missing 
*                               rights to list headers queried.  
*   
*  In the near future, there will be s.c. Additional Return Code Support feature available in Xuikon. This code 
*  will express for e.g. the number of headers to be delivered or possible system error code in failed cases.
*  
*  By MhspsServiceObserver::HandlehspsServiceMessage() call-back function implementation, client application
*  must listen the following ThspsServiceCompletedMessage-messages:
*  - EhspsGetListHeadersUpdate  - header list on the client side has a new object appended at the end of the list,
*  - EhspsGetListHeadersRestart - header list on server side has changed so much that the client must empty 
*                               the list printed on screen. New list will be delivered immediately. 
*                               The subscription stands.
*  - EhspsGetListHeadersEmpty   - header list on server side is now empty. The client must empty 
*                               the list on screen if printed. The subscription stands. 
*  - EhspsGetListHeadersFailed  - operation has failed. Client should cancel request and restart, perhaps.
*
*  Maintenanace functions may also return one of the following codes:
*  - EhspsServiceRequestSheduled, or
*  - EhspsServiceRequestError.
*
*  For explanation of the meanings of these messages, see ThspsServiceCompletedMessage-documentation.
*   
*  @since S60 5.0
*  @ingroup group_hspsclients
*/
class MhspsMaintenanceService
    {
    public:
        
        /** hspsGetListHeaders
        * 
        * @since S60 5.0
        * @param aSearchMask is ChspsODT-object which attributes are filled to present search 
        *        parameters for theme set queried by client. This parametrisation follows the 
        *        high-level schema. 
        * @param aHeaderList is an list object able to carry ChspsODT-objects. 
        * @return ThspsServiceCompletedMessage expressing the result of the call.
        */
        virtual ThspsServiceCompletedMessage hspsGetListHeaders(const ChspsODT& aSearchMask,
                                 CArrayPtrFlat<ChspsODT>& aHeaderList) = 0;

        /** hspsGetListHeaders
        * 
        * @since S60 5.0
        * @param aSearchMaskData is serialized ChspsODT-object. Before serializing, attributes 
        *        in ChspsODT-object were filled to present a search parameters for theme set queried.
        *        Serialized data parametrisation follows the low-level schema.  
        * @param aHeaderDataList is an list object able to carry serialized ChspsODT-objects. 
        * @return ThspsServiceCompletedMessage expressing the result of the call.
        */
        virtual ThspsServiceCompletedMessage hspsGetListHeaders(const TDesC8& aSearchMaskData,
                                 CArrayPtrSeg<HBufC8>& aHeaderDataList) = 0;
                
        /** hspsGetNextHeader
        * 
        * @since S60 5.0
        * @return ThspsServiceCompletedMessage expressing the result of the call.
        */
        virtual ThspsServiceCompletedMessage hspsGetNextHeader() = 0;        
        
        /** hspsCancelGetListHeaders
        * 
        * @since S60 5.0
        * @return ThspsServiceCompletedMessage expressing the result of the call.
        */
        virtual ThspsServiceCompletedMessage hspsCancelGetListHeaders() = 0;        
           
        /** hspsSetActiveTheme
        * 
        * @since S60 5.0
        * @param aSetMask represents parameters by which the new theme activation will be done.
        *        There must be sufficient set of parameters presented, at least a theme UID.
        * @param aHeader is an empty ODT-object which will contain the header of activated 
        *        theme on the return of the call. This parametrisation follows the high-level 
        *        schema. There must be sufficient set of parameters presented, at least a theme UID.
        * @return ThspsServiceCompletedMessage expressing the result of the call.
        */
        virtual ThspsServiceCompletedMessage hspsSetActiveTheme(const ChspsODT&  aSetMask,
                                  ChspsODT& aHeader) = 0;

        /** hspsSetActiveTheme
        * 
        * @since S60 5.0
        * @param aSetMaskData is externalized version of ChspsODT-object presenting parameters by 
        *        which the new theme activation will be done. There must be sufficient set of 
        *        parameters presented, at least a theme UID. This parametrisation follows the 
        *        low-level schema.
        * @param aHeaderData is an empty description for externalized ODT-object data. 
        *        When internalized, this object will contain the header of newly activated theme
        *        as a result of the call.
        * @return ThspsServiceCompletedMessage expressing the result of the call.
        */
        virtual ThspsServiceCompletedMessage hspsSetActiveTheme(const TDesC8& aSetMaskData,
                                    TDes8& aHeaderData) = 0;
        
        /**
        * hspsRestoreDefault
        * 
        * @since S60 5.0
        * @param aSetMask is an ODT-header paramerized engough to express the theme querid to made
        *        active.
        * @param aHeader is an empty ODT-header object that will contain the header of the theme
        *        actually made active if the request was successful.
        * @return ThspsServiceCompletedMessage expressing the result of the call.
        */
        virtual ThspsServiceCompletedMessage hspsRestoreDefault(const ChspsODT&  aSetMask,
                                    ChspsODT& aHeader) = 0;

        /** hspsRemoveTheme
        * 
        * @since S60 5.0
        * @param aSetMask is an ODT-header parametrized enough to express the theme to be removed.
        * @return ThspsServiceCompletedMessage expressing the result of the call.
        */
        virtual ThspsServiceCompletedMessage hspsRemoveThemeL(const ChspsODT&  aSetMask) = 0;
 
        /** 
        * Adds a new plugin configuration to the defined application configuration.
        * 
        * @since S60 5.0
        * @param aAppUid identifies the application configuration to be updated        
        * @param aConfId is an ID of the configuration being modified
        * @param aPluginUid is an UID of the plugin configuration to be added
        * @param aPosition is an UID of the added/new plugin configuration instance
        * @param aAddedPluginId is an ID of the added plugin configuration        
        * @return ThspsServiceCompletedMessage expressing the result of the call.
        */
        virtual ThspsServiceCompletedMessage hspsAddPlugin(
        		const TInt aAppUid,
        		const TInt aConfId,
        		const TInt aPluginUid,
        		const TInt aPosition, 
        		TInt& aAddedPluginId ) = 0;
        
        /** hspsRemovePlugin
        * Removes an existing plugin configuration instance from the defined application configuration.
        * 
        * @since S60 5.0
        * @param aAppUid identifies the application configuration to be updated                
        * @param aPluginId is an ID of the plugin configuration instance to be removed                
        * @return ThspsServiceCompletedMessage expressing the result of the call.
        */
        virtual ThspsServiceCompletedMessage hspsRemovePlugin(
        		const TInt aAppUid,        		
        		const TInt aPluginId ) = 0;

        /**
        * Activates plugin.
        * 
        * @since S60 5.0
        * @param aAppUid    identifies the application configuration to be updated                
        * @param aPluginId  is an ID of the plugin configuration instance to be activated                
        * @return ThspsServiceCompletedMessage expressing the result of the call.
        */
        virtual ThspsServiceCompletedMessage hspsSetActivePlugin(
                const TInt aAppUid,
                const TInt aPluginId ) = 0;        
        
        /** 
        * hspsSetSettings
        * @since S60 5.0          
        */
        virtual ThspsServiceCompletedMessage hspsSetPluginSettings(
               const ChspsODT& aHeader,
               const TInt aPluginId,
               ChspsDomDocument& aDom,
               const TBool aPluginStoringStatus) = 0;

        /** 
        * Updates plugin positions in an existing plugins list.
        * 
        * @since S60 5.0
        * @param aAppUid identifies the application configuration to be updated
        * @param aConfId is an ID of the configuration being updated (parent of the plugins node)                
        * @param aPluginIdList is an array of plugin IDs for setting the plugin positions                
        * @return ThspsServiceCompletedMessage expressing the result of the call.
        */
        virtual ThspsServiceCompletedMessage hspsMovePluginsL(
        		const TInt aAppUid,        
        		const TInt aConfId,
        		const CArrayFixFlat<TInt>& aPluginIdList ) = 0;

        /** 
        * Updates configuration's state
        * 
        * @since S60 5.0
        * @param aAppUid identifies the application configuration
        * @param aConfId is an ID of the configuration which state is updated                
        * @param aState is a new state of the configuration                
        * @return ThspsServiceCompletedMessage expressing the result of the call.
        */
        virtual ThspsServiceCompletedMessage hspsSetConfState(
                const TInt aAppUid,        
                const TInt aConfId,
                const ThspsConfigurationState aState,
                const ThspsConfStateChangeFilter aFilter ) = 0;

        /** 
        * Restores active application configuration
        * @since S60 5.0
        * @param aAppUid identifies the application which configuration is requested to be restored
        * @param aConfUid identifies the configuration to be restored
        * @return ThspsServiceCompletedMessage expressing the result of the call.
        */
        virtual ThspsServiceCompletedMessage hspsRestoreActiveAppConf(
                const TInt aAppUid,
                const TInt aConfUid ) = 0;
        
        /** 
         * Updates plugin configuration in all application configurations
         * 
         * @since S60 5.0
         * @param aOdt is odt header information of the plugin configuration               
         * @return ThspsServiceCompletedMessage expressing the result of the call.
         */
        virtual ThspsServiceCompletedMessage hspsPluginUpdateL(
                const ChspsODT& aOdt ) = 0;
        
        /**          
         * Replaces existing plugin configuration instance in an active application configuration.
         * 
         * @since S60 5.0
         * @param aAppUid identifies the application configuration
         * @param aPluginId identifies the plugin to be replaced                
         * @param aConfUid is an uid of the new plugin configuration                
         * @return ThspsServiceCompletedMessage expressing the result of the call.
         */
        virtual ThspsServiceCompletedMessage hspsReplacePlugin(
                const TInt aAppUid,
                const TInt aPluginId,             
                const TInt aConfUid ) = 0;

    };
  
/**
*  Part of HSPS Plugin Configuration Management Service APIs.
*  MhspsClientRequestService. HSPS Client Request Service API-definition.
*  HSPS Client Request Service is intended to serve Rendering Engines with Application Themes 
*  and their associated resources. Implementation of this interface will provide access to 
*  HSPS Plugin Configuration database maintained by HSPS Definition Repository aka Plugin Registry.
*  Definition Repository maintains plugin configurations in ChspsODT-objects. 
*  Themes can have resources like bitmaps etc. Resources can be accessed through this API also. 
*  Every individual resource has corresponding ChspsResource-object. Resource-objects are delivered 
*  to the client side in a resource list which is associated to its theme. Resource list exist only 
*  if there is any resources belonging to a theme.
*  There can be only one theme activated for a specific application at one time. 
*  The information of activation of themes is stored in Symbian Central Repository -object. 
*  The Central Repository is requested on every theme request that Rendering Engine commences. 
*
*  To initaite theme usage, Rendering Engine must call hspsGetODT(). There is no high-level or 
*  low-level parametrization used, however, API supports two parametrization mode; retrieving a 
*  theme with or without resources. The cases of retrieving a theme without resources are currently
*  rare, but with partial theme update cases to be implemented with OTA Push and DM-cases, it will 
*  be quite applicable. 
*  
*  hspsGetODT() returns one of the following codes:
*  - EhspsGetODTSuccess. Theme was received successfully. Client can start its rendering.
*  - EhspsGetODTFailed. Theme was not received. Check Additional Return Code (not yet implemented) 
*                     for the exact reason. Theme load can fail for e.g. for missing rights.
*  - EhspsGetODTLowMemoryStrategy. Possible memory low to create memory chunk. Something must do to 
*                                free memory and then try again.
*
*  After receiving the theme, Rendering Engine may subscribe the theme related events. 
*  These events may concern the changes of the theme status (meaning that some other theme is 
*  activated), or that the theme is updated. In both cases, client should reload the theme and do 
*  the rendering again. To act as a subscriber of these events, client must implement the interface 
*  MhspsClientRequestServiceObserver. Subscription is applied by calling hspsGetODTUpdate(). 
*  This function returns immediately one of the following codes:
*  - EhspsGetODTUpdateSuccess - The subscription of the theme updates and/or status changes was 
*                             successful.           
*                             The cient must start observe the server events.
*  - EhspsGetODTUpdateFailed  - The subscription of the theme updates and status changes was failed.
*
*  Once success-response was received, the observation of the Client Request Service events can 
*  return one of the following codes:
*
*  - EhspsGetODTUpdateStatus - Theme status was changed. Client must reset rendering and request theme 
*                            again. 
*  - EhspsGetODTUpdateHot    - Theme was updated. Client should reload the theme and then do the 
*                            theme rendering again. 
*  
*  Rendering Engine can get information of the theme resources through the ChspsResource-objects 
*  delivered on Resource List. When ChspsODT-object's DOM-document has a reference to some URL, 
*  the access method to the URL-related resource can be found in ChspsResource-object. 
*  When access-method is clear, client should call hspsAccessResourceFile() function with resource 
*  file path and empty RFile-handle. In successful cases, valid file handle to the associated 
*  resource file is returned. This functionality is arranged by the means of Symbian 9.0 EKA2 
*  Kernel's support for Platform Security Concept. The return code can be one of the following:
*  - EhspsAccessResourceFileSuccess - request to get access to a theme resource file was successful.
*  - EhspsAccessResourceFileFailed  - request to get access to a theme resource file was unsuccessful  
*    possible in due to a security fault.
* 
*  Client Request Service functions may also return one of the following codes:
*  - EhspsServiceRequestSheduled, or
*  - EhspsServiceRequestError.
*
*  For explanation of the meanings of these messages, see ThspsServiceCompletedMessage-documentation.
*
*  @since S60 5.0
*  @ingroup group_hspsclients
*/
class MhspsClientRequestService
    {
    public:
        
        /** hspsGetODT
        * 
        * @since S60 5.0
        * @param aAppUid is the UID of the application for which the theme is requested for.
        * @param aODT is an empty ChspsODT-object in where the theme is expected to be placed when the
        *        call returns.
        * @return ThspsServiceCompletedMessage expressing the result of the call.
        */
        virtual ThspsServiceCompletedMessage hspsGetODT(TInt aAppUid, ChspsODT& aODT) = 0;
                      
        /** hspsGetODTUpdate
        * 
        * @since S60 5.0
        * @return ThspsServiceCompletedMessage expressing the result of the call.
        */
        virtual ThspsServiceCompletedMessage hspsGetODTUpdate() = 0;

        /** hspsCancelGetODTUpdate
        * 
        * @since S60 5.0
        * @return ThspsServiceCompletedMessage telling the result of the call.
        */
        virtual ThspsServiceCompletedMessage hspsCancelGetODTUpdate() = 0;        
        
        /** hspsAccessResourceFile
        * 
        * @since S60 5.0
        * @param aResourceFileName is the path for resource file to requested. Resource file name 
        *        is given in every ChspsResource-object listed on the resource list.
        * @param aConfiguration identifies the application or interface from which the theme is to be found
        * @param aFile is an empty RFile object which a valid file handle to the resourec file will 
        *        be placed on return. The file access is arranged by means of Symbian 9.0 EKA2 
        *        Platform Security file handle sharing concept.
        * @return ThspsServiceCompletedMessage expressing the result of the call.
        */
        virtual ThspsServiceCompletedMessage hspsAccessResourceFile( 
        		const TDesC& aResourceFileName,
        		const ThspsConfiguration& aConfiguration,
        		RFile& aFile) = 0;   
    };

/**
* MhspsSecurityService.  
* Part of HSPS Application Theme Management Service APIs.
*  
*  These functions are the following:
*  - control user access on service request, and
*  - adjust user access in queries.
*
*  Function are explained below:
*
*  CheckAccessRightsL()
*  --------------------
*  CheckAccessRights function is used to limit client accesses to hsps Theme Server to 
*  the predefined ones. There will be a hardcoded access list with extension mechanism.
*  CheckQueryValidityL() function is used to limit theme request to the set allowed for this 
*  particular user. For example, S60 Personalisation Application is allowed to operate with
*  all themes in theme storage, however, AppShell is limited to operate with it own themes only.
*  CheckAccessRightsL() function is called by CPolicyServer when hspsThemeServer receives user 
*  request. Access rights are hard-coded here for S60 3.1, however, in later versions, 
*  support for dynamic configuration of access rights must atken care. This would be appropriate
*  to solve together TARM-policy implementation.
*
*  CheckQueryValidityL()
*  -------------------
*  CheckQueryValidityL() function is called by hsps Theme Server's service handlers 
*  (ChspsInstallionHandler, ChspsMaintenanceHandler, and ChspsClientRequestHandler) for adjusting 
*  user request to match the access rights level that user actually will have.
*  This function is to be called immediately when actual ODT is known. 
*  In the istallation cases, ODT is known after manifest-file parsing.
*  In the maintenanace cases, ODT is known immediately on query.
*  In the theme usage cases, ODT is known when it application theme has retrieved from 
*  UI Definition Repository.  
*  For instance, if user is requesting the theme listing (message hspsGetListHeaders) with
*  application UID set to 0 in query meaning that the query concerns all themes in storage.
*  However, if user is not S60 Personalisation Application, the query must not be allowed.
*  
*  @lib hspsThemeServer.exe
*  @since S60 5.0
*  @ingroup group_hspsserver
*/
class MhspsSecurityService
    {
    public:
        /** CheckL
        * 
        * @since S60 5.0
        * @param aOdt is the ODT of the theme whose  policy is checked
        */
        virtual void CheckIfLicenseeDefaultExistsL( const ChspsODT& aOdt ) = 0;

        /** 
        * CheckAccessRightsL
        * 
        * @since S60 5.0
        * @param aMessage is the RMessage2 containing the client request data. 
        *        Client's access rights to the hsps Theme Server is to be checked.
        * @return ETrue if request has passed the access rights check, otherwise return EFalse.
        */
        virtual TBool CheckAccessRightsL( const RMessage2& aMessage ) = 0;
         
    };

#endif// __hspsTHEMEMANAGEMENT_H__
