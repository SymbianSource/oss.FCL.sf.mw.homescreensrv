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
*  ChspsMaintenanceHandler is a server-side implementation of MhspsMaintenanceService API-definition of
*  HSPS Theme Management Services.
*  Theme Maintenance Service is intended to serve HSPS applications as well as S60 legacy 
*  applications - especially Personalisation Application - with Theme Maintenance Services. 
*  Theme maintenance can take place after a set of themes are installed by HSPS Theme 
*  Installation Service. Maintenance services includes theme listings, theme activation, 
*  theme removal, and default theme restoring functionality.
*
*  Maintenance functions deploy Definition Repository and Symbian Central Repository. 
*  Definition Repository keeps Application Themes in save place and Central Repository keeps the
*  information about which theme is active for  what application. Information in Central Repository
*  is stored in pairs of application UID and active theme UID.
*  ChspsMaintenanceHandler utilizes ChspsODT class to exchange the information of Application Themes. 
*  DOM-document member of ChspsODT is not attached in theme maintenance operations, only the theme 
*  header. For more information, see HSPS Theme Management Service APIs documentation.
*
*
*/
   

#ifndef __hspsMAINTENANCEHANDLER_H__
#define __hspsMAINTENANCEHANDLER_H__

#ifdef HSPS_LOG_ACTIVE
class ChspsLogBus;
#endif

class MhspsMaintenanceService;
class ChspsDefinitionRepository;
class ChspsSecurityEnforcer;
class ChspsODT;
class CRepository;
class ChspsThemeServer;
class ChspsThemeServerSession;
class ChspsResult;
class ChspsDomNode;

/**
*  Class ChspsMaintenanceHandler.
*  ChspsMaintenanceHandler performs all theme maintenance related functionality for hspsThemeServer. 
*  Maintenance services includes theme listing, theme activation, theme removal and default theme 
*  restoring operations.
*  
*  Class inheritance:
*  ==================
*  ChspsMaintenanceHandler derives CTimer for active timer functionality. It implements 
*  MhspsMaintenanceService-interface from HSPS Theme Management Service APIs. 
*  For more information, see HSPS Theme Management Service API documentation.
*  
*  Functionality:
*  ==============
*  Maintenance functions are presented in high-level and low-level parametrization mode. 
*  This is the same approach that was introduced with HSPS Theme Installation Service 
*  - to support user-intefaces with human-readable high-level elements and on the other hand, 
*  machine-originated users with low-level data.
*  Most of the maintenance functions are synchronous, only one is asynchronous; hspsGetNextHeader().
*  Others functions are straightforward to use but hspsGetListHeaders(). This will be explained next; synchronous 
*  call hspsGetListHeaders() initiates the theme header listing by passing search mask to HSPS 
*  Theme Server's Maintenance Service. This search mask is called a query. It also passes a list 
*  object in where maintenance service should append theme-header objects (type of ChspsODT-class 
*  without DOM-document) when one is retrieved asychronously. Search mask should be filled with 
*  proper parameters matching the need in hand. If no parameters are given, all headers of 
*  Application Themes available in Definition Repository will be delivered, otherwise, 
*  only sub-set of theme-headers will be delivered. 
*  After the query-call hspsGetListHeaders(), the delivering of the theme-headers is asynchronous.
*  Asynchronous service must be initiated by calling hspsGetNextHeader() after checking that any 
*  theme matching on current query is found. This action is called a subscription. 
*  To receive theme listing, the client application must implement 
*  MhspsThemeManagementServiceObserver-interface and start to listen call-back messages. 
*  The headers matching on query will be delivered immediately. Query remains until 
*  hspsCancelGetListHeaders() is called by client. If a new Application Theme exist in repository,
*  it will be delivered. As mentioned, when hspsGetListHeaders() returns, the return value must 
*  be checked. Return value could be one of the following:
*  - EhspsGetListHeadersSuccess - there is at least one Application Theme available matching on 
*                               query. Call first hspsGetNextHeader() to get the headers. Same call
*                               will set the subscribtion of new ones possible coming available 
*                               later on.
*  - EhspsGetListHeadersEmpty   - there is no themes matching on the query available at the time, 
*                               however, some might be exist later in due to installations. 
*                               Client application should retain the subcription.
*  - EhspsGetListHeadersFailed  - service request failed in some reason. Possible reasons are for 
*                               e.g. the missing rights to list headers queried.  
*   
*  In the near future, there will be s.c. Additional Return Code Support feature available in 
*  HSPS. This code will express for e.g. the number of headers to be delivered or possible system 
*  error code in failed cases.
*  
*  By MhspsServiceObserver::HandlehspsServiceMessage() call-back function implementation, client 
*  application must listen the following ThspsServiceCompletedMessage-messages:
*  - EhspsGetListHeadersUpdate  - header list on the client side has a new object appended at the end
*                               of the list,
*  - EhspsGetListHeadersRestart - header list on server side has changed so much that the client must 
*                               empty the list printed on screen. New list will be delivered 
*                               immediately. The subscription stands.
*  - EhspsGetListHeadersEmpty   - header list on server side is now empty. The client must empty 
*                               the list on screen if printed. The subscription stands. 
*  - EhspsGetListHeadersFailed  - operation has failed. Client should cancel request and restart, 
*                               perhaps.
*
*  Maintenanace functions may also return one of the following codes:
*  - EhspsServiceRequestSheduled, or
*  - EhspsServiceRequestError.
*
*  For explanation of the meanings of these messages, see ThspsServiceCompletedMessage-documentation.
*
*  @ingroup group_hspsserversession
*  @lib hspsThemeServer.exe
*  @since S60 3.1
*/
class ChspsMaintenanceHandler : public CTimer, public MhspsMaintenanceService, 
                                             public MhspsDefinitionRepositoryObserver
    {
    public:  // Constructors and destructor
        /**
        * NewL.
        * Two-phased constructor.
        * @since S60 5.0
        * @param aThemeServer Server instance
        * @param aSecureId Identifies the client application which uses the handler
        */
        static ChspsMaintenanceHandler* NewL( 
                ChspsThemeServer& aThemeServer, 
                const TUint aSecureId = 0 );
        
        /**
        * NewLC.
        * Two-phased constructor.
        * @since S60 5.0
        * @param aThemeServer Server instance
        * @param aSecureId Identifies the client application which uses the handler
        */
        static ChspsMaintenanceHandler* NewLC( 
                ChspsThemeServer& aThemeServer, 
                const TUint aSecureId = 0 );
        
        /**
        * Destructor.
        */
        virtual ~ChspsMaintenanceHandler();
  
    public: // New functions
        
        /**
        * ServiceGetListHeadersL
        * @since S60 3.1
        * @param aMessage contains the data received from the client
        */
        void ServiceGetListHeadersL(const RMessage2& aMessage);
        
        /**
        * ServiceGetNextHeaderL
        * @since S60 3.1
        * @param aMessage contains the data received from the client
        */
        void ServiceGetNextHeaderL(const RMessage2& aMessage);
        
        /**
        * ServiceSetActiveThemeL
        * @since S60 3.1
        * @param aMessage contains the data received from the client
        */
        void ServiceSetActiveThemeL(const RMessage2& aMessage); 
        
        /**
        * ServiceRestoreDefaultL
        * @since S60 3.1
        * @param aMessage contains the data received from the client
        */
        void ServiceRestoreDefaultL(const RMessage2& aMessage); 
        
        /**
        * ServiceRemoveThemeL
        * @since S60 3.1
        * @param aMessage contains the data received from the client
        */
        void ServiceRemoveThemeL(const RMessage2& aMessage);  
        
        /**
        * ServiceAddPluginL
        * @since S60 5.0
        * @param aMessage contains the data received from the client
        */
        void ServiceAddPluginL(const RMessage2& aMessage);
        
        /**
        * ServiceRemovePluginL
        * @since S60 5.0
        * @param aMessage contains the data received from the client
        */
        void ServiceRemovePluginL( const RMessage2& aMessage );
        
        /**
        * ServiceSetActivePluginL
        * @since S60 5.0
        * @param aMessage contains the data received from the client
        */
        void ServiceSetActivePluginL( const RMessage2& aMessage );
                
        /**
        * ServiceReplacePluginL
        * @since S60 5.0
        * @param aMessage contains the data received from the client
        */
        void ServiceReplacePluginL( const RMessage2& aMessage );
        
        /**
        * ServiceSetPluginSettingsL
        * @since S60 5.0
        * @param aMessage contains the data received from the client
        */
        void ServiceSetPluginSettingsL( const RMessage2& aMessage );
  
        /* 
         * ServiceGetPluginOdtL
         * @since S60 5.0
         * @param aMessage contains
         */
        void ServiceGetPluginOdtL( const RMessage2& aMessage );
        
        /**  
        * ServiceMovePluginsL
        * @since S60 5.0
        * @param aMessage contains the data received from the client
        */
        void ServiceMovePluginsL( const RMessage2& aMessage );

        /**  
        * ServiceSetConfStateL
        * @since S60 5.0
        * @param aMessage contains the data received from the client
        */
        void ServiceSetConfStateL( const RMessage2& aMessage );

        /**  
        * ServiceRestoreActiveAppConfL
        * @since S60 5.0
        * @param aMessage contains the data received from the client
        */
        void ServiceRestoreActiveAppConfL( const RMessage2& aMessage );

#ifdef HSPS_LOG_ACTIVE        
        /** 
        * Set log bus.
        */
        void SetLogBus( ChspsLogBus* aLogBus );
#endif        

        /**
         * SetServerSession.
         * @since S60 5.0
         * @param @param aServerSession Pointer to owning server session
         */
        void SetServerSession( ChspsThemeServerSession* aServerSession );

        /**  
         * ServiceRestoreConfigurationsL
         * @since S60 5.2
         * @param aMessage contains the data received from the client
         */
        void ServiceRestoreConfigurationsL( const RMessage2& aMessage );                        
        
  public: // Functions from base classes

        /** 
        * From MhspsMaintenanceService hspsGetListHeaders
        * @since S60 3.1
        * @param aSearchMask is ChspsODT-object which attributes are filled to present search 
        *        parameters for theme set queried by client. This parametrisation follows 
        *        the high-level schema. 
        * @param aHeaderList is an list object able to carry ChspsODT-objects. 
        * @return ThspsServiceCompletedMessage expressing the result of the call.
        */
        ThspsServiceCompletedMessage hspsGetListHeaders(const TDesC8& aSearchMaskData, 
                CArrayPtrSeg<HBufC8>& aHeaderDataList);
        
        /** 
        * From MhspsMaintenanceService hspsGetNextHeader
        * @since S60 3.1
        * @return ThspsServiceCompletedMessage expressing the result of the call.
        */
        ThspsServiceCompletedMessage hspsGetNextHeader();         
        
        /**
        * From MhspsMaintenanceService hspsCancelGetListHeaders
        * @since S60 3.1
        * @return ThspsServiceCompletedMessage expressing the result of the call.
        */
        ThspsServiceCompletedMessage hspsCancelGetListHeaders();  
        
        /**
        * From MhspsMaintenanceService hspsSetActiveTheme
        * @since S60 3.1
        * @param aSetMask represents parameters by which the new theme activation will be done.
                 There must be sufficient set of parameters presented, at least a theme UID.
        * @param aOdt is an empty ODT-object which will contain the ODT of activated 
        *        theme on the return of the call. This parametrisation follows the high-level 
        *        schema. There must be sufficient set of parameters presented, at least a theme UID.
        * @return ThspsServiceCompletedMessage expressing the result of the call.
        */
        ThspsServiceCompletedMessage hspsSetActiveTheme( const ChspsODT& aSetMask, ChspsODT& aOdt );
        
        /**
        * From MhspsMaintenanceService hspsRestoreDefault
        * @since S60 3.1
        * @param aSetMask is an ODT-header parametrized enough to express the theme to be made
        *        active.
        * @param aHeader is an empty ODT-header object that will contain the header of the theme
        *        actually made active if the request was successful.
        * @return ThspsServiceCompletedMessage expressing the result of the call.
        */
        ThspsServiceCompletedMessage hspsRestoreDefault( const ChspsODT& aSetMask, ChspsODT& aHeader );
        
        /**
        * From MhspsMaintenanceService hspsRemoveThemeL
        * @since S60 3.1
        * @param aSetMask is an ODT-header parametrized enough to express the theme to be removed.
        * @return ThspsServiceCompletedMessage expressing the result of the call.
        */        
        ThspsServiceCompletedMessage hspsRemoveThemeL( const ChspsODT& aSetMask );
                
        /**
        * From MhspsMaintenanceService hspsGetListHeaders
        * @since S60 3.1
        * @param aSearchMaskData is serialized ChspsODT-object. Before serializing, attributes 
        *        in ChspsODT-object were filled to present a search parameters for theme set queried.
        *        Serialized data parametrisation follows the low-level schema.  
        * @param aHeaderList is an list object able to carry serialized ChspsODT-objects. 
        * @return ThspsServiceCompletedMessage expressing the result of the call.
        */
        ThspsServiceCompletedMessage hspsGetListHeaders(const ChspsODT& /*aSearchMask*/
                                                         , const TBool /*aCopyLogos*/ 
                                                         , CArrayPtrFlat<ChspsODT>& /*aHeaderList*/);
        
        /**
        * From MhspsMaintenanceService hspsSetActiveTheme
        * @since S60 3.1
        * @param aSetMaskData is externalized version of ChspsODT-object presenting parameters by 
        *        which the new theme activation will be done. There must be sufficient set of 
        *        parameters presented, at least a theme UID. This parametrisation follows the 
        *        low-level schema.
        * @param aHeaderData is an empty description for externalized ODT-object data. 
        *        When internalized, this object will contain the header of newly activated theme 
        *        as a result of the call.
        * @return ThspsServiceCompletedMessage expressing the result of the call.
        */   
        ThspsServiceCompletedMessage hspsSetActiveTheme(const TDesC8& /*aSetMaskData*/
                                                           , TDes8& /*aHeaderData*/);
        
        /**
        * From MhspsMaintenanceService hspsAddPlugin
        * Not used - see ServiceAddPluginL instead which has a diffrent input 
        * @since S60 5.0
        */
        ThspsServiceCompletedMessage hspsAddPlugin(
            const TInt /*aAppUid*/,
            const TInt /*aParentPluginId*/,
            const TInt /*aPluginUid*/,
            const TInt /*aPosition*/,
            TInt& /*aAddedPluginId*/ );                
 
        /**          
        * From MhspsMaintenanceService hspsRemovePlugin
        * Not used - see ServiceRemovePluginL instead which has a diffrent input
        */
        ThspsServiceCompletedMessage hspsRemovePlugin(
               const TInt /*aAppUid*/,                
               const TInt /*aPluginId*/ );

        /**          
        * From MhspsMaintenanceService hspsSetActivePlugin
        * Not used - see ServiceRemovePluginL instead which has a diffrent input
        */        
        ThspsServiceCompletedMessage hspsSetActivePlugin(
                const TInt aAppUid,
                const TInt aPluginId
                );

        /**          
        * From MhspsMaintenanceService hspsReplacePlugin
        * Not used - see ServiceReplacePluginL instead which has a diffrent input
        */               
        ThspsServiceCompletedMessage hspsReplacePlugin(
            const TInt /*aAppUid*/,
            const TInt /*aPluginId*/,             
            const TInt /*aConfUid*/ );
        
        /** 
        * From MhspsMaintenanceService hspsSetSettings
        * Not used - see ServiceRemovePluginL instead which has a diffrent input
        */
        ThspsServiceCompletedMessage hspsSetPluginSettings(
                const ChspsODT& /*aHeader*/,
                const TInt /*aPluginId*/,
                ChspsDomDocument& /*aDom*/,
                const TBool /*aPluginStoringStatus*/);

        /**          
        * From MhspsMaintenanceService hspsMovePluginsL
        * Not used - see ServiceMovePluginsL instead which has a diffrent input
        */
        ThspsServiceCompletedMessage hspsMovePluginsL(
               const TInt /*aAppUid*/,
               const TInt /*aConfId*/,               
               const CArrayFixFlat<TInt>& /*aPluginIdList*/ );
         
        TInt UpdatePluginListL(
            ChspsODT& aAppODT,
            const TInt aConfigurationId, 
            const CArrayFixFlat<TInt>& aPluginIdList );       

        /**          
        * From MhspsMaintenanceService hspsSetConfState
        * Not used - see ServiceSetConfStateL instead which has a diffrent input
        */
        ThspsServiceCompletedMessage hspsSetConfState(
            const TInt /*aAppUid*/,
            const TInt /*aConfId*/,             
            const ThspsConfigurationState /*aState*/,
            const ThspsConfStateChangeFilter /*aFilter*/ );

        /**          
        * From MhspsMaintenanceService hspsRestoreActiveAppConf
        * Not used - see ServiceRestoreActiveAppConfL instead which has a diffrent input
        */
        ThspsServiceCompletedMessage hspsRestoreActiveAppConf(
            const TInt /*aAppUid*/ );
        
        /** 
         * Updates plugin configuration in all application configuration
         *  
         * @since S60 5.0
         * @param apluginUid identifies the plugin configuration                
         * @return ThspsServiceCompletedMessage expressing the result of the call.
         */
        ThspsServiceCompletedMessage hspsPluginUpdateL(
                const ChspsODT& aOdt );

        

    public: // from MhspsDefinitioRepositoryObserver
         
        TBool HandleDefinitionRespositoryEvent( ThspsRepositoryInfo aRepositoryInfo );
        
    protected:  // Functions from base classes
        
       
        ChspsMaintenanceHandler( ChspsThemeServer& aThemeServer, const TUint aSecureId );
        
        /**
        * By default Symbian 2nd phase constructor is private.
        */
        void ConstructL();
        
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
        * C++ default constructor.
        */
        void GetHeaderListL( CArrayPtrSeg<HBufC8>& aHeaderDataList, const ChspsODT& aSearchMask );
        ThspsServiceCompletedMessage GetHeaderListUpdateL();
        void RestoredDefaultL( const ChspsODT& aSetMask, ChspsODT& aHeader );
        void RemoveThemeL( const ChspsODT& aSetMask );
        TBool CompareHeadersL(const TDesC8& aOldHeaderData, const TDesC8& aNewHeaderData);
        TBool ComparePaths(const ChspsODT& aOldHeader, const ChspsODT& aNewHeader);
        TBool FilterHeader(const ChspsODT& aMask, const ChspsODT& aHeader);
        // Completes the request message
        void CompleteRequest(const ThspsServiceCompletedMessage aReturnMessage, 
            RMessagePtr2& aMessagePtr, const TDesC8& aHeaderData = KNullDesC8 );
                
        /**
         * Finds last id value from the provided DOM.
         * @since S60 5.0
         * @param aDom is the application configuration in XML format
         * @param aLastUsedConfId is the largest used configurtion id
         * @param aLastUsedPluginId is the largest used plugin id
         */
        void GetUsedIdsL(
                ChspsDomDocument& aDom,
                TInt& aLastUsedConfId,
                TInt& aLastUsedPluginId
                );
        
        /**
         * Appends an application configuration with an instance of a plugin configuration.
         * @since S60 5.0
         * @param aAppODT is an instance of the the application configuration
         * @param aPluginODT is an ODT instance of the plugin configuration
         * @param aConfigurationId is an id of the configuration being modified, identifies a plugins list
         * @param aNewPosition is an index of the new position in the plugins list
         * @param aLastConfId is a value of the last configuration id
         * @param aLastPluginId is a value of the last plugin id 
         * @return KErrNone if the operation succeeded
         */
        TInt AppendConfigurationL(
                ChspsODT& aAppODT,
                const ChspsODT& aPluginODT,      
                const TInt aConfigurationId,    
                const TInt aNewPosition, 
                TInt& aLastConfId,
                TInt& aLastPluginId
                );
                
        /**
         * Finds the plugins node of the provided plugin node.
         * @since S60 5.0
         * @param aNode is the plugins node to be searched
         * @return plugins node or NULL if the node couldn't be found 
         */
        ChspsDomNode* FindPluginsNode(
                ChspsDomNode& aNode 
                );
        
        /**
         * Appends a plugins list with a new plugin configuration instance.
         * @since S60 5.0
         * @param aAppODT is the application configuration being modified
         * @param aPluginsNode is the plugins node where the new plugin instance should be added to
         * @param aPluginODT is an ODT instance of the plugin configuration to be added
         * @param aNewPosition is an index of the new position in the plugins list
         * @param aNewPluginId is an id of the new plugin instance 
         * @return KErrNone if the operation succeeded 
         */
        TInt AppendPluginConfigurationL(
                ChspsODT& aAppODT,                
                ChspsDomNode& aPluginsNode,                
                const ChspsODT& aPluginODT,                
                const TInt aNewPosition,
                const TInt aNewPluginId
                );

        
        /**
         * Finds plugin nodes from the plugin configuration being added,
         * appends referred configurations into the main plugin configuration,
         * and updates the plugin nodes with unique id attribute values. 
         * @since S60 5.0
         * @param aAppODT is an instance of the the application configuration
         * @param aPluginODT is an instance of the plugin configuration being added
         * @param aLastConfId is a value of the last configuration id
         * @param aLastPluginId is a value of the last plugin id
         * @return KErrNone
         */
        TInt HandlePluginReferencesL(
                ChspsODT& aAppODT,
                ChspsODT& aPluginODT,
                TInt& aLastConfId,
                TInt& aLastPluginId 
                );

        /**
         * Checks if the provided plugin configuration is a collection of plugin
         * configurations, e.g. view configuration.
         * Alternatively, we could check the type from header cache
         * @since S60 5.2
         * @param aPluginNode Plugin node
         * @return True if it is a view configuration
         */
        TBool IsViewConfiguration(
                ChspsDomNode& aPluginNode );
                
        /**
         * Removes an plugin instance from the provided application configuration
         * @since S60 5.0
         * @param aAppODT is an instance of the the application configuration         
         * @param aPluginId is an ID of the plugin instance to be removed
         * @return KErrNone if succeeded
         */
        TInt RemoveConfigurationL(
                ChspsODT& aAppODT,
                const TInt aPluginId );                
        
        /**
         * Removes an plugin instance from the provided application configuration
         * @since S60 5.0
         * @param aAppODT is an instance of the the application configuration         
         * @param aPluginNode is a node of the plugin instance to be removed
         * @return KErrNone if succeeded
         */
        TInt RemoveConfigurationL(
                ChspsODT& aAppODT,
                ChspsDomNode& aPluginNode );
        
        /**
         * Finds a plugin node with the provided id which is also a children of 
         * the provided plugins node
         * @since S60 5.0
         * @param aPluginsNode is an instance of the plugins node
         * @param aPluginId is an ID of the plugin instance to be found within the plugins node
         * @return Pointer to the plugin node or NULL
         */
        ChspsDomNode* FindChildPluginNode(
                ChspsDomNode& aPluginsNode,
                const TInt aPluginId );

        /**
         * Returns a count of plugin instances.
         * @since S60 5.0
         * @param aAppODT is an instance of the the application configuration         
         * @param aPluginUid is an UID of the plugin instances to be found
         * @param aInstanceCount is the return value         
         */
        void GetPluginInstanceCountL(
                const ChspsODT& aAppODT,        
                const TInt aPluginUid,
                TInt& aInstanceCount );
        

        
        /**
         * Adds plugin resouces to the provided ODT
         * @since S60 5.0
         * @param aAppODT is an instance of the the application configuration         
         * @param aPluginUid is an UID of the plugin instance         
         */
        void AddPluginResourcesL(
                ChspsODT& aAppODT,
                const TInt aPluginUid );
        
        /**
         * Sets Plugin settings
         * @since S60 5.0
         * @param aOdt is an instance of the the application configuration         
         * @param aPluginId is an ID of the plugin instance to be found
         * @param aDom is a Dom Document of the settings that is going to set/update 
         * @return KErrNone if succeeded
         */
        TInt hspsSetPluginSettingsL(
                ChspsODT& aOdt,
               
                TInt aPluginId,
                ChspsDomDocument& aDom);
        
        /**
         * Saves Plugin settings
         * @since S60 5.0
         * @param aODT is an instance of the the plugin configuration         
         * @param aDom is a Dom Document of the settings that is going to saved 
         * @return KErrNone if succeeded
         */
        TInt hspsSavePluginSettingsL(  
                        ChspsODT& aOdt,
                        ChspsDomDocument&  aDom );  
        /**
         * Finds a item node with the provided item id
         * @since S60 5.0
         * @param aNodeIdentifier An Tag.
         * @param aDomNode A Dom node where the target node is searched from.
         * @return Result node.
         */
        ChspsDomNode& FindNodeByTagL( 
                           const TDesC8& aNodeTag,
                           ChspsDomNode& aDomNode );
        
        /**
         * Gets configuration node with the provided plugin id
         * @since S60 5.0
         * @param aAppODT is an instance of the the application configuration         
         * @param aPluginId is an ID of the plugin instance to be found
         * @param aPluginUid is an UID of the plugin instance
         * @param aPluginname is a name of the plugin instance
         * @return Error code
         */
        TInt GetConfigurationNodeDataL(
                        ChspsODT& aAppODT,      
                        const TInt aPluginId,
                        TInt&  aPluginUid,
                        TDes& aPluginName );

        /**
         * Set configuration node state attribute value
         * @since S60 5.0
         * @param aAppODT is an instance of the the application configuration         
         * @param aConfId is an ID of the configuration node to be found
         * @param aState is a new value of the configuration node state attribute
         * @return void
         */
        void SetConfStateL(
            ChspsODT& aAppODT,      
            TInt aConfId,
            ThspsConfigurationState aState,
            ThspsConfStateChangeFilter aFilter );

        /**
         * Restores default configuration defined for the application
         * @since S60 5.0
         * @param aHeader ODT header which defines the restored configuration
         * @param aOdt restored application configuration
         */
        void RestoreDefaultAppConfL(
            ChspsODT*& aHeader,
            ChspsODT& aOdt );

        /**
         * Activates default configuration defined for the application
         * @since S60 5.2
         * @param aHeader application which default configuration is activated
         * @param aOdt activated application configuration
         */
        void ActivateDefaultAppConfL(
            const ChspsODT& aHeader,
            ChspsODT& aOdt );

        /**
        * HandleDefinitionRespositoryEventL
        *
        * @since S60 5.0
        */
        
        TBool HandleDefinitionRespositoryEventL( ThspsRepositoryInfo aRepositoryInfo );                 
       
        /**
        * RemovePluginFromAppConfs
        *
        * @since S60 5.0
        */
        void RemovePluginFromAppConfsL( 
                const ChspsODT& aOdt,
                RArray<ThspsRepositoryInfo>& aNotificationParams);
        
        /**
        * RemoveAppConfFromRepositoryL
        * Removes application confguration from definition repository
        * @since S60 5.2
        * @param aOdt application configuration to be removed
        * @param aNotificationParams notifications need to be sent after removal
        */
        void RemoveAppConfFromRepositoryL( 
                const ChspsODT& aOdt,
                RArray<ThspsRepositoryInfo>& aNotificationParams );

        /**
        * RemovePluginConfFromRepositoryL
        * Removes plugin confguration from definition repository
        * @since S60 5.2
        * @param aOdt plugin configuration to be removed
        * @param aNotificationParams notifications need to be sent after removal
        */
        void RemovePluginConfFromRepositoryL( 
                const ChspsODT& aOdt,
                RArray<ThspsRepositoryInfo>& aNotificationParams );

        /**
        * InvalidateUninstalledPluginInstancesL
        * Removes uninstalled plugin instances from an inactive
        * application configuration by changing them to error state.
        * @since S60 5.2
        * @param aAppODT application configuration to be updated
        * @param aPluginUid plugin to be uninstalled
        * @param aPluginIds an array on plugin ids to be processed
        * @return ETrue if the ODT was updated. 
        */
        TBool InvalidateUninstalledPluginInstancesL(
                ChspsODT& aAppODT,        
                const TInt aPluginUid,
                const RArray<TInt>& aPluginIds );
        
        /**
         * UpdatePluginFromAppConfsL
         *
         * @since S60 5.0
         */
        void UpdatePluginFromAppConfsL( ChspsODT& aOdt, 
                 RArray<ThspsRepositoryInfo>& aNotificationParams );
        /**
         * UpdatePluginConfigurationL
         *
         * @since S60 5.0
         */
        
        TInt UpdatePluginConfigurationL(
                ChspsODT& aOdt, 
                ChspsODT& aPluginOdt,
                RArray<TInt>& aPluginIds );
        
        /**
         * Replaces plugin configuration in the provided 
         * application configuration
         * @since S60 5.0
         * @param aAppODT is an instance of the the application configuration
         * @param aPluginId is an id of the plugin to be replaced
         * @param aPluginODT is an instance of the plugin configuration to be added
         */
        TInt ReplaceConfigurationL(
                ChspsODT& aAppODT,
                const TInt aPluginId,
                const ChspsODT& aPluginODT );
        
        /**
         * Returns position of a plugin in the plugins list
         * @since S60 5.0         
         * @param aPluginNode is a plugin to be found
         * @return position of -1
         */
        TInt FindPluginPosition(                
                ChspsDomNode& aPluginNode );
        
        /**
         * Appends missing plugin with a dummy configuration where status="Error"
         * @since S60 5.0
         * @param aAppDom is a DOM of an application configuration 
         * @param aMissingPluginNode is a plug-in node which was not found from the Plugin Repository
         * @param aPluginUid is an UID of the missing plugin
         */
        void AddErrorConfigurationL(
                ChspsDomDocument& aAppDom,
                ChspsDomNode& aMissingPluginNode,
                const TInt aPluginUid );
                                
        /**
         * Restores plug-ins by reinstallation.
         * @since S60 5.2          
         * @param aInstallUdaEmmc Set if UDA and eMMC drives should used 
         */
        void HandleReinstallationL(
                const TBool aInstallUdaEmmc );
                                
        /**
         * Removes all plugins from the plugins node and related 
         * resources from the resource array.
         * @since S60 5.2
         * @param aAppODT Application configuration
         * @return error code
         */
        TInt RestoreActiveViewL(
                ChspsODT& aAppODT );
        
        /**
         * Finds a plugin node which is the active view configuration.
         * @since S60 5.2
         * @param aAppODT Application configuration
         * @return Active plugin node or NULL
         */
        ChspsDomNode* FindActiveView(
                ChspsODT& aAppODT );
        
        /**
         * Removes all plugin configurations from the provided
         * plugin node (view configuration).
         * @since S60 5.2         
         * @param aAppODT Application configuration
         * @param aActivePluginNode Plugin node to be modified
         * @return error code 
         */
        TInt RemovePluginConfigurationsL(
                ChspsODT& aAppODT, 
                ChspsDomNode& aActivePluginNode );
        
        /**
         * Removes all unlocked views and reset the active view.
         * Can leave if DOM is corrupted (objects are not found) or in OOM cases.
         * @since S60 5.2   
         * @param aAppODT Application configuration
         */
        void RemoveUnlockedViewsL(
                ChspsODT& aAppODT );
        
        /**
         * Checks whether the plugin configuration was locked.
		 * @since S60 5.2   
         * @param aConfNode Configuration node
		 * @return True if it was locked
         */
        TBool IsConfigurationLocked(
                ChspsDomNode& aConfNode );
        
    private:   // Data
        RMessagePtr2 iMessagePtr;
        ChspsResult* iResult;
        TBuf8<KMaxResultDataLength8> iResultData;
        ThspsServiceRequestMessage iRequestMessage;     
        ThspsServiceCompletedMessage iCompletedMessage;
        CArrayPtrSeg<HBufC8>* iHeaderDataList;
        TBool iSubscription;
        TInt iDeliveryCount;
        ChspsODT* iSearchMask;
        ChspsODT* iSetMask;        
        
        ChspsThemeServer& iThemeServer;
        // Identifies the client application
        TUint iSecureId;
        // Reference to central repository
        CRepository& iCentralRepository;
        // Reference to definition repository
        ChspsDefinitionRepository& iDefinitionRepository;
        // Reference to security enforcer
        ChspsSecurityEnforcer& iSecurityEnforcer;
        CArrayPtrSeg<ChspsODT>& iHeaderListCache;        
        ChspsThemeServerSession* iServerSession;	// Not owned.
        CFileMan* iFileMan;
        TBool iMaintainLogoResources;
#ifdef HSPS_LOG_ACTIVE
        /**
         * Log bus.
         */        
        ChspsLogBus* iLogBus;
#endif        
    };


#endif //__hspsMAINTENANCEHANDLER_H__
// End of File
