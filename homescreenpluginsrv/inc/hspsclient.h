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
*  ChspsClient is a client-side implementation of MhspsInstallationService and MhspsMaintenanceService
*  interface definitions of Homescreen Plugin Configuration Managament Services APIs. Installation and 
*  Maintenance Services are intended to serve Homescreen as well as S60 legacy applications - especially
*  the Personalisation Application - with the plugin installation and maintenance 
*  functionality. Typical use-case is that client application initiates ChspsClient-object, connects
*  to hspsThemeServer with its application UID, and commences installation and maintenance service 
*  requests.
*
*  Security in using services is quarantied by controlling the rights to see Application Themes 
*  and operate with hsps Theme Server. This control is based on SECUREID of application process.
*  Security is maintained behind the curtain by means of Symbian EKA2 Kernel services for Platform
*  Security.
*
*  ChspsClient offers high- and low-level API-functionality. High-level and low-level calls are 
*  diffrentiated by their parametrization. High-level parametrization is directed to the be used by
*  applications which need human readable data and offer a user interface. Low-level parametrization
*  is directed to the machine-originated requestors like OTA Push or DM (Device Management) 
*  sub-systems where data is not needed to understand semanticly. 
*  ChspsClient delegates server messages to its client application by 
*  MhspsThemeManagementServiceObserver::HandlehspsClientMessage() call-back to be implemented by the 
*  client application.
*
*  Theme Installation Services
*  ===========================
*  Processing a installation task works in asynchronous mode, however, synchronous mode is applied 
*  everytime the installation is initiated. After checking installation rights and files validities,
*  hsps Theme Server will allow actual installation. Client application must call actual  
*  installation by commencing next phase execution explicitly. First call of installation returns 
*  the ODT-header of the theme to be installed. This is to enable previewing information about 
*  a theme to be installed. In this point, user can accept or decline the installation. 
*  Installation preview feature is useful especially in OTA-service cases when user acception on 
*  theme installation might be selected a a part of user policy settings.
*  A theme can include resources that can be imported to the target system as such like some audio
*  files etc., however, most of the images must be fitted on to the target device's color depth and
*  pixel density. In these cases, the needed conversions will be executed asynchronously on 
*  server-side. Asynchronous installation is executed in phases - resource by resource. 
*  Installation phases are promoted automaticly, however, client application can control and 
*  monitor installation phases, and also cancel installation at any time. If installation is 
*  cancelled, hspsThemeServer initiates roll-back functionality removing inconsistent installation.  
*
*  Theme Maintenanace Services:
*  ============================
*  Most of the maintenance functions are synchronous, only one is asynchronous; hspsGetNextHeader().
*  Others functions are straightforward to use. They are explained in the class declaration section
*  below. The asynchronous service is applied only in theme listing service calls.
*  Client application initiates the theme listing by calling synchronous function 
*  hspsGetListHeaders(). This call initiates the query. Client application should call 
*  hspsGetNextHeader() after checking that any themes matching on current query is found or that query
*  is valid at least. This action is called a subscription. To receive actual theme listing, the 
*  client application must start to listen call-back messages from the server about list delivery. 
*  The headers matching on query will be delivered immediately. Query remains until 
*  hspsCancelGetListHeaders() is called by client. If a new Application Theme exist in repository, 
*  it will be delivered if the query concers it. 
*
*
*
*/


#ifndef __ChspsCLIENT_H__
#define __ChspsCLIENT_H__

#include <e32base.h>

#include "hspsthememanagement.h"
#include "hspsclientsession.h"
#include "hspsresult.h"

#ifdef HSPS_LOG_ACTIVE
class ChspsLogBus;
#endif

/** hspsClient Panic Category. */
_LIT( KhspsClient, "hspsClient" );

/** hspsClient panic codes: */
enum ThspsClientPanics
    {
    EhspsClientBadState
    };

class RhspsClientSession;
class MhspsInstallationService;
class MhspsMaintenanceService;
class MhspsThemeManagementServiceObserver;
class ChspsODT;
class ChspsResult;

/**
 *  ChspsClient. 
 *  ChspsClient performs all client-side theme installation and maintenance related functionality.
 *  Theme maintenance services includes theme listings, theme activation, theme removal, and default
 *  theme restoring functionality.
 *
 *  Class inheritance:
 *  ==================
 *  ChspsClient derives CActive for asynchronous request handling, and MhspsInstallationService and 
 *  MhspsMaintenanceService from Homescreenpluginservices. 
 *
 *  Theme installation functionality:
 *  =================================
 *  Client application must call hspsInstallNextPhaseL() after testing that the synchronous call 
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
 *  For more information of manifest-file format, see Homescreen wikipages.
 *
 *  Installation functions can return one of the following ThspsServiceCompletedMessage-codes:
 *  - EhspsInstallThemeSuccess,
 *  - EhspsInstallPhaseSuccess, or
 *  - EhspsInstallThemeFailed.
 *
 *  Client application must implement MhspsThemeManagementServiceObserver-interface and listen these 
 *  messages mentioned. Let it be emphasised that both synchronous and asynchronous calls can return
 *  the codes above. Installation functions may also return one of the following codes:
 *  - EhspsServiceRequestSheduled, or
 *  - EhspsServiceRequestError.
 *
 *  For explanation of the meanings of these messages, see ThspsServiceCompletedMessage-documentation.
 *
 *  Theme maintenance functionality:
 *  ================================
 *  Most of the maintenance functions are synchronous, only one is asynchronous; hspsGetNextHeader().
 *  Others functions are straightforward to use but hspsGetListHeaders(). This will be explained next; 
 *  synchronous call hspsGetListHeaders() initiates the theme header listing by passing search mask 
 *  to HSPS Server's Maintenance Service. This search mask is called a query. It also passes
 *  a list object in where maintenance service should append theme-header objects (type of 
 *  ChspsODT-class without DOM-document) when one is retrieved asychronously. Search mask should be 
 *  filled with proper parameters matching the need in hand. If no parameters are given, all headers
 *  of Application Themes available in Definition Repository will be delivered, otherwise, 
 *  only sub-set of theme-headers will be delivered. After the query-call hspsGetListHeaders(), the 
 *  delivering of the theme-headers is asynchronous. Asynchronous service must be initiated by 
 *  calling hspsGetNextHeader() after checking that any theme matching on current query is found or
 *  that query is valid anyway. This action is called a subscription. To receive theme
 *  listing, the client application must implement MhspsThemeManagementServiceObserver-interface and
 *  start to listen call-back messages. 
 *  The headers matching on query will be delivered immediately. Query remains until 
 *  hspsCancelGetListHeaders() is called by client. If a new Application Theme exist in repository, it
 *  will be delivered. As mentioned, when hspsGetListHeaders() returns, the return value must be checked.
 *  Return value could be one of the following:
 *  - EhspsGetListHeadersSuccess - there is at least one Application Theme available matching on query. 
 *                               Call first hspsGetNextHeader() to get the headers. Same call will set 
 *                               the subscribtion of new ones possible coming available later.
 *  - EhspsGetListHeadersEmpty   - there is no themes matching on the query available at the time, 
 *                               however, some might be exist later in due to installations. 
 *                               Client application should retain the subcription.
 *  - EhspsGetListHeadersFailed  - service request failed for some reason. Possible reasons are for e.g.
 *                               the missing rights to list headers queried.  
 *   
 *  Additional Return Code Support (ChspsResult) will express for e.g. the number of headers to be
 *  delivered or possible system error code in failed cases.
 *  
 *  By MhspsServiceObserver::HandlehspsServiceMessage() call-back function implementation, client 
 *  application must listen the following ThspsServiceCompletedMessage-messages:
 *
 *  - EhspsGetListHeadersUpdate  - header list on the client side has a new object appended at the end
 *                               of the list,
 *  - EhspsGetListHeadersRestart - header list on server side has changed so much that the client must 
 *                               empty the list printed on screen. New list will be delivered 
 *                               immediately. The subscription stands.
 *  - EhspsGetListHeadersEmpty   - header list on server side is now empty. The client must empty 
 *                               the list on screen if printed. The subscription stands. 
 *  - EhspsGetListHeadersFailed  - operation has failed. Client should cancel request and restart.
 *
 *  Maintenanace functions may also return one of the following codes:
 *  - EhspsServiceRequestSheduled, or
 *  - EhspsServiceRequestError.
 *
 *  For explanation of the meanings of these messages, see ThspsServiceCompletedMessage-documentation.
 *
 *
 * @lib hspsClient.lib
 * @since S60 5.0
 * @ingroup group_hspsclients
 */
class ChspsClient : public CActive, public MhspsInstallationService, public MhspsMaintenanceService
    {
    public: // Constructors and destructors
        /**
        * NewL.
        * 
        * Two-phased constructor.
        * Creates a ChspsClient object using two phase construction,
        * and return a pointer to the created object.
        * 
        * @since S60 5.0 
        * @param aObserver The object to be used to handle updates from the server.
        */
        IMPORT_C static ChspsClient* NewL(MhspsThemeManagementServiceObserver& aObserver );

        /**
        * NewLC.
        * 
        * Two-phased constructor.
        * Creates a ChspsClinet object using two phase construction,
        * and return a pointer to the created object.
        * 
        * @since S60 5.0
        * @param aObserver The object to be used to handle updates from the server.
        */
        IMPORT_C static ChspsClient* NewLC( MhspsThemeManagementServiceObserver& aObserver );

        /**
        * ~ChspsClient.
        * 
        * Destructor.
        * Destroys the object and release all memory objects.
        */
        virtual ~ChspsClient();

    public: // New functions

        /**
        * From MhspsInstallationService.
        */

        IMPORT_C ThspsServiceCompletedMessage hspsInstallTheme(const TDesC& aManifestFileName,
                                   ChspsODT& aHeader);

        /**
        * From MhspsInstallationService.
        */
        IMPORT_C ThspsServiceCompletedMessage hspsInstallTheme(const TDesC& aManifestFileName, 
                                   TDes8& aHeaderData);

        /**
        * From MhspsInstallationService.
        */
        IMPORT_C ThspsServiceCompletedMessage hspsInstallNextPhaseL(ChspsODT& aHeader);

        /**
        * From MhspsInstallationService.
        */
        IMPORT_C ThspsServiceCompletedMessage hspsInstallNextPhaseL(TDes8& aHeaderData);

        /**
        * From MhspsInstallationService.
        */
        IMPORT_C ThspsServiceCompletedMessage hspsCancelInstallTheme();

        /**
        * From MhspsMaintenanceService.
        */
        IMPORT_C ThspsServiceCompletedMessage hspsGetListHeaders(
                const ChspsODT& aSearchMask,
                const TBool aCopyLogos, 
                CArrayPtrFlat<ChspsODT>& aHeaderList );
        
        /** 
        * From MhspsMaintenanceService.
        */
        IMPORT_C ThspsServiceCompletedMessage hspsGetListHeaders(const TDesC8& aSearchMaskData,
                                   CArrayPtrSeg<HBufC8>& aHeaderDataList);
        
        /**
        * From MhspsMaintenanceService.
        */  
        IMPORT_C ThspsServiceCompletedMessage hspsGetNextHeader();       
        
        /**
        * From MhspsMaintenanceService.
        */  
        IMPORT_C ThspsServiceCompletedMessage hspsCancelGetListHeaders();       
        
        /**
        * From MhspsMaintenanceService.
        */
        IMPORT_C ThspsServiceCompletedMessage hspsSetActiveTheme(const ChspsODT& aSetMask,
                                   ChspsODT& aHeader);
       
        /**
        * From MhspsMaintenanceService.
        */        
        IMPORT_C ThspsServiceCompletedMessage hspsSetActiveTheme(const TDesC8& aSetMaskData,
                                 TDes8& aHeaderData);
  
        /**
        * From MhspsMaintenanceService.
        */
        IMPORT_C ThspsServiceCompletedMessage hspsRestoreDefault(const ChspsODT& aSetMask,
                                   ChspsODT& aHeader);

        /**
        * From MhspsMaintenanceService.
        */
        IMPORT_C ThspsServiceCompletedMessage hspsRemoveThemeL(const ChspsODT& aSetMask);
        
        /**
        * GethspsResult
        * 
        * @since S60 5.0
        * @param aResult is a ChspsResult object containing additional information of query result.
        */
        IMPORT_C void GethspsResult(ChspsResult& aResult);

        /**
        * Returns ODT header list according to the defined search mask
        * 
        * @since S60 5.0 
        * @param aSearchMask is ChspsODT-object which attributes are filled to present search 
        *        parameters for theme set queried by client. This parametrisation follows 
        *        the high-level schema. 
        * @param aCopyLogos is set if client wants to view logos         
        * @param aHeaderList is an list object able to carry ChspsODT-objects. 
        * @return Error code
        */
        IMPORT_C TInt hspsGetHeaders(
                const ChspsODT& aSearchMask,
                const TBool aCopyLogos,
                CArrayPtrFlat<ChspsODT>& aHeaderList);

        /**
        * From MhspsMaintenanceService        
        */
        IMPORT_C ThspsServiceCompletedMessage hspsAddPlugin(
        		const TInt aAppUid,
        		const TInt aConfId,
        		const TInt aPluginUid,
        		const TInt aPosition,
        		TInt& aAddedPluginId 
        		);
        
        /**
        * From MhspsMaintenanceService        
        */
        IMPORT_C ThspsServiceCompletedMessage hspsRemovePlugin(
        		const TInt aAppUid,        		
        		const TInt aPluginId
        		);

        /**
        * From MhspsMaintenanceService        
        */
        IMPORT_C ThspsServiceCompletedMessage hspsSetActivePlugin(
                const TInt aAppUid,
                const TInt aPluginId
                );        
        
        /**
         * From MhspsMaintenanceService        
         */
        IMPORT_C ThspsServiceCompletedMessage hspsSetPluginSettings(
               const ChspsODT& aHeader,
               const TInt aPluginId,
               ChspsDomDocument& aDom,
               const TBool aPluginStoringStatus
               );

        /**
        * From MhspsMaintenanceService        
        */
        IMPORT_C ThspsServiceCompletedMessage hspsGetPluginOdtL(
        		const TInt aAppUid,        		
        		const TInt aPluginUid,
        		ChspsODT* aPluginOdt );
        
        /**
        * From MhspsMaintenanceService        
        */
        IMPORT_C ThspsServiceCompletedMessage hspsMovePluginsL(
        		const TInt aAppUid,        
        		const TInt aConfId,
        		const CArrayFixFlat<TInt>& aPluginIdList 
        		);

        /**
        * From MhspsMaintenanceService        
        */
        IMPORT_C ThspsServiceCompletedMessage hspsSetConfState(
                const TInt aAppUid,        
                const TInt aConfId,
                const ThspsConfigurationState aState,
                const ThspsConfStateChangeFilter aFilter
                );

        /**
        * From MhspsMaintenanceService        
        */
        IMPORT_C ThspsServiceCompletedMessage hspsRestoreActiveAppConf(
                const TInt aAppUid,
                const TInt aConfUid );

        /** 
         * Updates plugin configuration in all application configuration
         *  
         * @since S60 5.0
         * @param apluginUid identifies the plugin configuration                
         * @return ThspsServiceCompletedMessage expressing the result of the call.
         */
        IMPORT_C  ThspsServiceCompletedMessage hspsPluginUpdateL(
                          const ChspsODT& /* aOdt */ );

        /**
        * Reinstalls requested configuration
        * 
        * @since S60 5.0
        * @param aAppUid is interface.
        * @param aConfUid is ID of the reinstalled configuration.  
        * @return ThspsServiceCompletedMessage expressing the result of the call.
        */
        IMPORT_C ThspsServiceCompletedMessage hspsReinstallConf(
            TInt aAppUid, 
            TInt aConfUid );

        /** 
        * Internal log bus setter. Empty method in product builds.
        * 
        * @since S60 5.0
        * @param aLogBus    Log bus to be set.
        */
        IMPORT_C void SetLogBus( void* aLogBus );        
        
        /**
        * From MhspsMaintenanceService        
        */                               
        IMPORT_C ThspsServiceCompletedMessage hspsReplacePlugin(
                const TInt aAppUid,
                const TInt aPluginId,             
                const TInt aConfUid );
        
        /**
        * Restores plugin configurations by either removing all plugins
        * from the active view or by removing all extra views.
        * In latter case, the first locked view is emptied. If there are 
        * no locked views then the first view will remain while others
        * are removed.
        * @since S60 5.2
        * @param aAppUid Application uid
        * @param aReinstall  False if only the active view should be fixed,
        *                    True if also all extra views should removed                         
        * @return ThspsServiceCompletedMessage expressing the result of the call.
        */
        IMPORT_C ThspsServiceCompletedMessage hspsRestoreConfigurations(
            const TInt aAppUid,
            const TBool aReinstall );
        
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
        
        
    private: 
       
        /**
        * Updates iResult 
        * @param aResultData Result data
        */
        void UpdatehspsResult( TDesC8& aResultData );
        
        /**
        * Creates a header object from header data (iHeaderData) 
        * and adds created header to the header list (iHeaderList)
        * @param aHeaderData Header data
        */
        void AppendHeaderListL( TDesC8& aHeaderData );
        
        /**
        * Gets file handle and streams odt from the file.
        * @since S60 5.0
        * 
        * @param aOdtPath - odt file path
        * @param aAppUid - application uid for accessing to the file
        * @param aPluginOdt - odt for streaming
        * @return error code  
        */
        TInt StreamOdtL( TDes& aOdtPath, 
        		const TInt aAppUid, ChspsODT* aPluginOdt );
        
    private: // Constructors and destructors

        /**
        * ChspsClient.
        * Performs the first phase of two phase construction.
        * @param aObserver The object to be used to handle updates from the server.
        */
        ChspsClient( MhspsThemeManagementServiceObserver& aObserver);

        /**
        * ConstructL.
        * Performs the second phase construction of a ChspsClient object.
        */
        void ConstructL();

   
    private: // Members
    
        /**
        * iSession, the theme server session.
        */
        RhspsClientSession iSession;

        /**
        * iObserver, observer which handles updates from the server.
        */
        MhspsThemeManagementServiceObserver& iObserver;
        
        /**
        * Message to cancel outstanding request.
        */
        ThspsServiceRequestMessage iCancelRequestMessage;
        
    private: // Data    
        
        /**
        * An pointer array given by service requester to be filled with headers.
        */
        CArrayPtrSeg<HBufC8>* iHeaderDataList;
        
        CArrayPtrFlat<ChspsODT>* iHeaderList; 
        
        TBuf8<KMaxHeaderDataLength8> iHeaderData;
        TBuf8<KMaxResultDataLength8> iResultData;
        
        TBool iODTMode;
        
        ChspsODT* iOdt;
        ChspsODT* iHeader;
        
        HBufC8* iSearchMaskData;
        HBufC8* iSetMaskData;
        ChspsResult* iResult;
        TBool iSubscription;
        
#ifdef HSPS_LOG_ACTIVE
        /**
         * Log bus.
         */        
        ChspsLogBus* iLogBus;
#endif        
     };


#endif //__ChspsCLIENT_H__

// End of File
