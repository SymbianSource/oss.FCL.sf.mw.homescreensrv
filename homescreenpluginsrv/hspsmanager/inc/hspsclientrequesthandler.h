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
* Description:  ChspsClientRequestHandler is a server-side implementation for
*                retrieving configurations from the HSPS's Definition Repository.
*                Configurations are binary presentations of a manifest, XML and
*                resource files - actually, instances of the hspsODT class.
*                Due to perfomance aspects, instead of the binary files, a file 
*                handle is returned over the IPC channel, and on client side, 
*                the handle is then used for internalizing the requested 
*                configuration.
*                  
*
*/


#ifndef __hspsCLIENTREQUESTHANDLER_H__
#define __hspsCLIENTREQUESTHANDLER_H__

#include <e32base.h>

#include "hspscenreplistener.h"
#include "hspsreqnotifparam.h"

class MhspsClientRequestService;
class ChspsSecurityEnforcer;
class CRepository;
class ChspsODT;
class ChspsFileAccess;
class ChspsResource;
class ChspsThemeServer;
class ChspsResult;
class ChspsRequestNotifcationParams;

#ifdef HSPS_LOG_ACTIVE
class ChspsLogBus;
#endif

/**
* 
*  Class ChspsClientRequestHandler. ChspsClientRequestHandler performs all Xuikon Client Request Service 
*  related functionality of Xuikon Theme Mangement Service APIs.
*  Xuikon Client Request Service API is intended to serve Rendering Engines with Application Themes 
*  and their associated resources. ChspsClientRequestHandler will provide access to 
*  Xuikon's Application Theme database maintained by Xuikon Definition Repository.
*  Definition Repository maintains Application Themes in ChspsODT-objects. 
*  Themes can have resources like bitmaps etc. Resources can be accessed through this API also. 
*  Every individual resource has corresponding ChspsResource-object. hspsResource-object is delivered 
*  to the client side in a resource list which is associated to the theme. 
*  Resource list is included in ODT as well as DOM-document which contains UI-definition. 
*  There can be only one theme activated for a specific application at one time. 
*  The information of activation of themes is stored in Symbian Central Repository -object. 
*  The Central Repository is requested on every theme request that Rendering Engine commences. 
*
*  Class inheritance:
*  ==================
*  ChspsClientRequestHandler derives CTimer for active timer functionality. It implements 
*  MhspsClientRequestService-interface for Xuikon Client Request Service API functionality,
*  and MhspsDefinitionRepositoryObserver for listening to the definition repository events.  
*
*  Functionality:
*  ==============
*  To initiate theme usage, Rendering Engine must call hspsGetODT().
*  
*  hspsGetODT() returns one of the following codes:
*  - EhspsGetODTSuccess. Theme was received successfully. Client can start its rendering.
*  - EhspsGetODTFailed. Theme was not received. Check Additional Return Code in hspsResult-object
*   for the exact reason. Theme load can fail for e.g. for missing rights.
*  - EhspsGetODTLowMemoryStrategy. Possible memory low to create chunk. Client should
*                                implement some low memory strategy. In version 3.1, 
*                                the low-memory stategy is use direct file handle in
*                                accessing ODT. System can be configured to use
*                                low-memory strategy only.
*
*  After receiving the theme, Rendering Engine may subscribe the theme related events. 
*  These events may concern the changes of the theme status (meaning that some other theme is 
*  activated), or that the theme is updated. In both cases, client should reload the theme and do 
*  the rendering again. To act as a subscriber of these events, client must implement the interface
*  MhspsClientRequestServiceObserver. Subscription is applied by calling hspsGetODTUpdate(). 
*  This function returns immediately one of the following codes:
*  - EhspsGetODTUpdateSuccess - The subscription of the theme updates and/or status changes was 
                              successful. The cient must start observe the server events.
*  - EhspsGetODTUpdateFailed  - The subscription of the theme updates and status changes was failed.
*
*  Once success-response was received, the observation of the Client Request Service events can 
*  return one of the following codes:
*
*  - EhspsGetODTUpdateStatus - Theme status was changed. Client must reset rendering and request theme
*                            again. 
*  - EhspsGetODTUpdateHot    - Theme was updated. Client should reload the theme and then do the 
*                            rendering again. 
*  
*  Rendering Engine can get information of the theme resources through the ChspsResource-objects 
*  delivered on the resource list in ODT. When ChspsODT-object's DOM-document has a reference to 
*  some resource-URL, the access method to the resource can be found in ChspsResource-object. 
*  When access-method is resolved, client should call hspsAccessResourceFile() function with resource
*  file path and empty RFile-handle. In successful cases, valid file handle to the associated 
*  resource file is returned.
*  This functionality is arranged by the means of Symbian 9.0 EKA2 Kernel's support for Platform 
*  Security Concept. The return code can be one of the following:
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
*  @ingroup group_hspsserversession
*  @lib hspsThemeServer.exe
*  @since S60 5.0
*/       
class ChspsClientRequestHandler : public CBase,
                                public MhspsClientRequestService,
                                public MhspsDefinitionRepositoryObserver,
                                public MhspsCenRepObserver
                                               
    {
    public: // Constructors and destructor
        /**
        * NewL
        *
        * @since S60 5.0
        */
        static ChspsClientRequestHandler* NewL( ChspsThemeServer& aThemeServer );
        /**
        * NewLC
        *
        * @since S60 5.0
        */
        static ChspsClientRequestHandler* NewLC( ChspsThemeServer& aThemeServer );
             
        /**
        * ~ChspsClientRequestHandler
        * Destructor.
        */
        ~ChspsClientRequestHandler();
      
    public: // New functions
        /**
        * ServiceGetOdtL
        *
        * @since S60 5.0
        * @param aMessage contains the data received from the client
        */
        void ServiceGetOdtL(const RMessage2& aMessage);             
        /**
        * ServiceGetOdtUpdateL
        *
        * @since S60 5.0
        * @param aMessage contains the data received from the client
        */
        void ServiceGetOdtUpdateL(const RMessage2& aMessage);
        /**
        * ServiceAccessResourceFileL
        *
        * @since S60 5.0
        * @param aMessage contains the data received from the client
        */
        void ServiceAccessResourceFileL(const RMessage2& aMessage);

        /**
         * Finnishes installation of application configurations
         * @since S60 5.0
         * @param aAppODT is an ODT instance of an application configuration
         */
        void HandlePluginReferencesL(
                ChspsODT& aAppODT );

#ifdef HSPS_LOG_ACTIVE  
        /**
        * SetLogBus
        *
        * @since S60 5.0
        * @param aLogBus Log bus to be set.
        */
        void SetLogBus( ChspsLogBus* aLogBus );
#endif        
        
    public: // From MhspsClientRequestService
            
        /** 
        * From MhspsClientRequestService hspsGetODT
        * @since S60 3.1
        * @param aAppUid is the UID of the application for which the theme is requested for.
        * @param aODT is an empty ChspsODT-object in where the theme is expected to be placed when 
        *        the call returns.
        * @return ThspsServiceCompletedMessage expressing the result of the call.
        */
        ThspsServiceCompletedMessage hspsGetODT(TInt aAppUid, ChspsODT& aODT);
        
        /** 
        * From MhspsClientRequestService hspsGetODT
        * @since S60 3.1
        * @param aAppUid is the UID of the application for which the theme is requested for.
        * @param aODT is an empty ChspsODT-object in where the theme is expected to be placed when 
        *             the call returns.
        * @param aResourceList is an empty list-object able to contain ChspsResource-objects. 
        *        On return of the call, every resource object on the list will describe a resource 
        *        belonging to the theme in hand. 
        * @return ThspsServiceCompletedMessage expressing the result of the call.
        *
        * DEPRECATED
        */  
        ThspsServiceCompletedMessage hspsGetODT(TInt aAppUid, ChspsODT& aODT, 
                          CArrayPtrSeg<ChspsResource>& aResourceList);
        
        /** 
        * From MhspsClientRequestService hspsGetODTUpdate
        * @since S60 3.1
        * @return ThspsServiceCompletedMessage expressing the result of the call.
        */
        ThspsServiceCompletedMessage hspsGetODTUpdate();
        
        /** 
        * From MhspsClientRequestService hspsCancelGetODTUpdate
        * @since S60 3.1
        * @return ThspsServiceCompletedMessage telling the result of the call.
        */
        ThspsServiceCompletedMessage hspsCancelGetODTUpdate();
        
        /** 
        * From MhspsClientRequestService hspsAccessResourceFile
        * @since S60 3.1
        * @param aResourceFileName is the path for resource file to requested. Resource file name is
        *        given in every ChspsResource-object listed on the resource list.
        * @param aFile is an empty RFile object which a valid file handle to the resourec file will
        *        be placed on return. 
        *        The file access is arranged by means of Symbian 9.0 EKA2 Platform Security file 
        *        handle sharing concept.
        * @return ThspsServiceCompletedMessage expressing the result of the call.
        */
        ThspsServiceCompletedMessage hspsAccessResourceFile(
            const TDesC& /*aResourceFileName*/,
            const ThspsConfiguration& /*aConfiguration*/,
            RFile& /*aFile*/ );
  
    public: // from MhspsDefinitioRepositoryObserver
         
         TBool HandleDefinitionRespositoryEvent( ThspsRepositoryInfo aRepositoryInfo );
  
    public: // from MhspsCenRepObserver
         
        void HandleCenRepChangeL( const TUint32 aId );
         
    protected:
        /**
        * ChspsClientRequestHandler
        * C++ default constructor.
        * @since S60 5.0
        */
        ChspsClientRequestHandler( ChspsThemeServer& aThemeServer );
        
        /**
        * ConstructL
        * By default Symbian 2nd phase constructor is private
        * @since S60 5.0
        */
        void ConstructL();
  
    private:
                
        /**
        * RestoreDefaultL
        *
        * @since S60 3.1
        */
        void RestoreDefaultL( ChspsODT& aOdt );
        
        /**
        * CallGetODTPathL
        *
        * @since S60 5.0
        */
        void CallGetODTPathL( ChspsODT& aSearchMask, TDes& aODTPath );
        
        /**
        * CompleteRequest
        *
        * @since S60 5.0
        */
        void CompleteRequestL(const ThspsServiceCompletedMessage aReturnMessage,
                   RMessagePtr2& aMessagePtr, const TDesC8& aHeaderData = KNullDesC8 );
                
        /**
        * RestoreValidThemeL
        * Restores operator/licensee default theme
        * @since S60 3.1
        * @param aCorruptedOdt  Odt which failed to render
        */
        void RestoreValidThemeL(ChspsODT& aCorruptedOdt);
        
      /**
        * HandleDefinitionRespositoryEventL
        *
        * @since S60 5.0
        */
        
        TBool HandleDefinitionRespositoryEventL( ThspsRepositoryInfo aRepositoryInfo );
        
        /**
         * Finds plug-in references from the provided application configuration and append 
         * XML document with plug-ins specific XML.
         * Applies to plug-in configurations of type "application configuration" only.
         * @param aAppODT is an ODT instance of an application configuration
         */
        void AppendPluginConfigurationsL( 
                ChspsODT& aAppODT );
        
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
         * Prepare given ChspsRequestNotificationParams for sending.
         * 
         * @since S60 5.0
         * @param aInfo         Info to build params from.    
         * @param aMessage      Completed message.
         */
        void PrepareAndCompleteRequestL( const ThspsRepositoryInfo& aInfo,
                                         const ThspsServiceCompletedMessage& aMessage );                                                                           
         
	    /**
         * Finds plug-in's initial settings from the provided root configuration and modify 
         * plugin specific settings with these initial settings.
		 *
		 * @since S60 5.0
         * @param aAppODT is an ODT instance of an application configuration
         * @return KErNone if succeed
		 */
        TInt AppendInitialSettingsL( ChspsODT& aAppODT );
		
        /**
         * Parse initial settings items.
         * 
         * @since S60 5.0
         * @param aInitialSettingsNode         Initial settings node to be parsed(source node).    
         * @param aSettingsNode                Actual settings node(target node) where to initial settings needs to be copied.
         * @return KErNone if succeed
		 */
        TInt ParseInitialSettingsItemsL( ChspsDomNode& aInitialSettingsNode,ChspsDomNode& aSettingsNode);
		
        /**
         * Handling initial settings property nodes.
         * 
         * @since S60 5.0
         * @param aPropertyNode        Initial settings property node(property source node).    
         * @param aNode                Target node.
		 * @return KErNone if succeed
         */
        TInt HandlePropertyNodeL( ChspsDomNode& aPropertyNode,ChspsDomNode& aNode );  
		
        /**
         * Handling attributes.
         * 
         * @since S60 5.0
         * @param aAttrName       Attribute name.    
         * @param aAttrValue      Attribute value.
		 * @param aNode           Target property node.
		 * @return KErNone if succeed.
         */     
        TInt HandlePropertyAttributesL(const TDesC8& aAttrName,
                                    const TDesC8& aAttrValue,
                                    ChspsDomNode& aNode );
       
        /**
         * Finds a root node for liw structure from dom document by identifier
		 *
		 * @since S60 5.0
         * @param aNodeIdentifier An Tag.
         * @param aNodeIdentifier An Identifier.
         * @param aNode A Dom Node where the node is searched from.
         * @return Result node.
         */
        ChspsDomNode& FindRootNodeByIdentifierL( const TDesC8& aNodeTag,
                                                        const TDesC8& aNodeIdentifier,
                                                        ChspsDomNode& aNode );
        
    protected: // Functions from base classes

       
    private:    // Data
        // the next member variables should be used in asynchronous message handling only
        RMessagePtr2 iMessagePtr;
        TFileName iODTPath;
       
        // tells to restore default in repeated theme load cases
        TBool iRestoreDefault;
        ChspsResult* iResult;
        TBuf8<KMaxResultDataLength8> iResultData;
        ChspsRequestNotificationParams* iReqNotifParams;
        TBuf8<KMaxReqNotifDataLength8> iReqNotifData;
        HBufC8* iRequestData;
        
        // Index of the requested function
        ThspsServiceRequestMessage iRequestMessage;
        
        ThspsServiceCompletedMessage iCompletedMessage;
        TTime iLastHeaderListUpdateTime;
        TTime iLastSuccessThemeLoadTime;
        // application or interface uid
        TUint iRootUid;        
        // theme uid from the most recent request
        TUint iActiveThemeUid;
        TUint iSecureId;
        ChspsODT* iOdt;
        RFs iFs;
        TInt iThemeLoadRepeatCount;
        // ownership of the next objects is held by ChspsThemeServer
        ChspsThemeServer& iThemeServer;        
        // Reference to central repository
        CRepository& iCentralRepository;  
        // Pointer to Central Repository Listener        
        ChspsCenRepListener* iCenRepListener;
        // Reference to definition repository
        ChspsDefinitionRepository& iDefinitionRepository;
        // Reference to security enforcer
        ChspsSecurityEnforcer& iSecurityEnforcer;
        // header list cache
        CArrayPtrSeg<ChspsODT>& iHeaderListCache;   
        // Reserved pointer for future extension
        TAny* iReserved;

#ifdef HSPS_LOG_ACTIVE        
        // Log bus to be used. Not owned.
        ChspsLogBus* iLogBus;
#endif        
    };

#endif //__hspsCLIENTREQUESTHANDLER_H__

// end of file

