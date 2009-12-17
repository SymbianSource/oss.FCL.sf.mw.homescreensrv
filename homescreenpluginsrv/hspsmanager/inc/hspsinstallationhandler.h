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
*  Installation Handler.
* 
*  
*  ChspsInstallatioHandler is a server-side implementation of HSPS Configuration 
*  Management Services.
*  The Installation Service is intended to serve the HSPS SAPI client only.
*   
*  Services:
*  1) reads installation instructions from a manifest-file 
*  2) parses and updates xml configuration into a binary representation (ODT's DOM)
*  3) stores various resources, localized and common, into Plug-in Repository  
*     (a.k.a. Definition Repository)
*    
*  When installation is successful, Installation Handler utilizes HSPS Definition 
*  Repository to store the results. All resources are stored on Definition Repository 
*  and their related information is stored on the list of ChspsResource-objects 
*  - a resource list. This list of ChspsResource-objects is called Resource List. 
*  For more information, see hspsResource documentation.
* 
*  On theme updates, Installation Handler deploys ChspsSecurityEnforcer to control 
*  update rights. For more information, see ChspsSecurityEnforcer documentation  
*  and ChspsInstallatioHandler documentation
*
*
*/



#ifndef __hspsINSTALLATIONHANDLER_H__
#define __hspsINSTALLATIONHANDLER_H__

#include <contenthandler.h>
#include <parser.h>

#include "hspsthememanagement.h"
#include "hspsresource.h"
#include "hspsresult.h"

#ifdef HSPS_LOG_ACTIVE
class ChspsLogBus;
#endif
class CRepository;
class ChspsThemeServer;
class ChspsDefinitionRepository;
class ChspsDefinitionEngineInterface;
class ChspsODT;
class ChspsDomNode;
class ChspsDomStringPool;
class ChspsSecurityEnforcer;
class ChspsResource;
class ChspsDomList; 

using namespace Xml;

#if defined(WINSCW) || defined(__WINS__)
const ThspsFamily KDefaultFamily = EhspsFamilyQhd_tch;
#endif

enum TInstallationMode
    {
    EAsynchronousObject,
    EServiceHandler
    };

/**
*  Class ChspsInstallationHandler.
*  ChspsInstallationHandler performs all theme installation related functionality for hspsThemeServer.
*
*  Class inheritance:
*  ==================
*  ChspsInstallationHandler implements MhspsInstallationService from HSPS Theme Management Service
*  and MContentHandler for XML-parsing of manifest file. For more information, see the architecture 
*  description in the beginning of this file. 
*
*  Functionality:
*  ============== 
*  ChspsInstallationHandler offer high- and low-level API-functionality. High-level and low-level 
*  calls are diffrentiated by their parametrization. High-level parametrization is directed to the
*  be used by applications which need human readable data and offer a user interface. Low-level 
*  parametrization is directed to the machine-originated requestors like OTA Push or DM (Device 
*  Management) sub-systems where data is not needed to understand semanticly.
*  Processing the installation task can be synchronous or asynchronous. Synchronous mode is 
*  applied everytime an installation is initiated. Application Theme may include resources. 
*  Importing these resources to the target device may need conversions. Such resources like some
*  audio files etc., can be imported directly, however, most of the image-resources must be fitted
*  for the target device because target screen device's color depth and/or pixel density varies.
*  In this case, the needed conversions will be executed asynchronously. Asynchronous installation
*  is executed in phases - resource by resource. Client application must initiate asynchronous mode
*  by calling hspsInstallNextPhaseL() after testing that the synchronous call hspsInstallTheme() 
*  has returned EhspsInstallPhaseSuccess return code. This means that there are resources to convert 
*  asynchronoulsly. The rest of the asynchronous installation phases will be executed automatically, 
*  however, installation can be interrupted by calling hspsCancelInstallTheme() at any time. 
*  If installation is canceled, installation handler initiates roll-back functionality removing 
*  inconsistent installation.
*  Installation Service uses specific manifest-file format to get informed about the xml, css, dtd,
*  and resource files to be installed. Resource files might be locale-specific or generic as well.
*  HSPS Definition Repository takes care of saving everything on their appropriate places in the
*  target device. Repository, i.e. theme storage, is loacted on hspsThemeServer's private-folder. 
*  The storage is organized by the information contained in ODT-header.
*  Locales are instructed in manifest file also. Manifest file's file-extension must be .dat, but
*  actually, the file-name can be whatever, however, when low-level parametrization is used, the 
*  name must be "manifest.dat" and it must be the last file extracted from the installation package.
*  For more information of manifest-file format, see "HSPS Manifest File User Guide.doc".
*
*  Installation functions can return one of the following ThspsServiceCompletedMessage-codes:
*  - EhspsInstallThemeSuccess,
*  - EhspsInstallPhaseSuccess, or
*  - EhspsInstallThemeFailed.
*
*  Client application must implement MhspsThemeManagementServiceObserver-interface and listen the 
*  messages mentionaed. Let it be emphasised that both synchronous and asynchronous calls can return
*  the codes above. Installation functions may also return one of the following codes:
*  - EhspsServiceRequestSheduled, or
*  - EhspsServiceRequestError.
*
*  For explanation of the meanings of these messages, see ThspsServiceCompletedMessage-documentation.
*  
*  @ingroup group_hspsserversession
*  @lib hspsThemeServer.exe
*  @since S60 5.0
*/
class ChspsInstallationHandler : public CBase, 
                                 public MhspsInstallationService, 
                                 public MContentHandler
    {
    public:
        
    public:
        /**
        * Two-phased constructor.
        */
        static ChspsInstallationHandler* NewL( ChspsThemeServer& aThemeServer );
        static ChspsInstallationHandler* NewLC( ChspsThemeServer& aThemeServer );

        /**
        * Destructor.
        */      
        ~ChspsInstallationHandler();
  
    public:  // New functions

        /**
        * ServiceInstallThemeL
        * Starts the actual installation in ChspsInstallationHandler.
        * @since S60 5.0
        * @param aMessage Message from client
        */      
        void ServiceInstallThemeL(
                const RMessage2& aMessage);
      
        /**
        * ServiceInstallNextPhaseL
        * Starts subsequent installation phases.
        * @since S60 5.0
        * @param aMessage Message from client
        */            
        void ServiceInstallNextPhaseL(
                const RMessage2& aMessage);
      
        /** 
        * hspsInstallNextPhase
        * @since S60 5.0
        * This is server-side only function.
        * @param aHeaderData will return ODT-header of the latest installation phase in serialized 
        *        (i.e. marshalled)
        *        data mode. This follows the low-level parametrization schema.  
        * @param aRequestStatus will return the status of the asynchronous request returned by 
        *        installer.
        * @return TInthspsServiceCompletedMessage expressing the result of the call.
        */
        void hspsInstallNextPhaseL( 
                TDes8& aHeaderData, 
                TRequestStatus& aRequestStatus );
        
        /** 
        * CheckAutoInstallationValidityL
        * Checks if auto installation of a theme is permitted
        * @since S60 5.0
        * This is server-side only function.
        * @return TBool ETrue if auto installation can proceed.
        */
        TBool CheckAutoInstallationValidityL();

#ifdef HSPS_LOG_ACTIVE        
        /** 
        * Set log bus.
        */
        void SetLogBus( ChspsLogBus* aLogBus );
#endif        
    
        /**
         * Disables "configuration was installed" notifications
         * @since S60 5.0
         */
        void DisableNotifications();
        
    public: // Functions from base classes

        /**
        * From MhspsInstallationService hspsInstallTheme
        * 
        * @since S60 5.0
        * @param aManifestFileName is the full path to the installation script file - a manifest 
        *        file.
        * @param aHeader is an empty ChspsODT-object in which a valid ODT-header of the newly 
        *        installed theme will be returned if the request is successful. The use of 
        *        ChspsODT-type parameter follows the high-level parametrization schema.
        * @return ThspsServiceCompletedMessage expressing the result of the call.
        */
        inline ThspsServiceCompletedMessage hspsInstallTheme( 
                const TDesC& /*aManifestFileName*/, 
                ChspsODT& /*aHeader*/ );
        
        /**
        * From MhspsInstallationService hspsInstallTheme
        * 
        * @since S60 5.0
        * @param aManifestFileName is full path of the installation script file - a manifest file
        * @param aHeaderData will return ODT-header of the newly installed theme in serialized 
        *        (i.e. marshalled)
        *        data mode. This follows the low-level parametrization schema.  
        * @return ThspsServiceCompletedMessage expressing the result of the call.
        */
        ThspsServiceCompletedMessage hspsInstallTheme( 
                const TDesC& aManifestFileName, 
                TDes8& aHeaderData );
                        
        /**
        * From MhspsInstallationService hspsInstallNextPhaseL
        * 
        * @since S60 5.0
        * @param aHeader is an empty ChspsODT-object in which a valid ODT-header of the latest 
        *        installation phase completed if the request was successful. The use of ChspsODT-type
        *        parameter follow the high-level parametrization schema.
        * @return ThspsServiceCompletedMessage expressing the result of the call.
        */
        inline ThspsServiceCompletedMessage hspsInstallNextPhaseL( 
                ChspsODT& /*aHeader*/ );

        /**
        * From MhspsInstallationService hspsInstallNextPhaseL
        * 
        * @since S60 5.0
        * @param aHeaderData will return ODT-header of the latest installation phase in 
        *        serialized (i.e. marshalled) data mode. This follows the low-level 
        *        parametrization schema.  
        * @return ThspsServiceCompletedMessage expressing the result of the call.
        */
        ThspsServiceCompletedMessage hspsInstallNextPhaseL( 
                TDes8& aHeaderData );
        
        /**
        * From MhspsInstallationService hspsCancelInstallTheme
        * 
        * @since S60 5.0
        * @return ThspsServiceCompletedMessage expressing the result of the call.
        */
        ThspsServiceCompletedMessage hspsCancelInstallTheme();

        //From MContentHandler
        
        /**
        * OnStartDocumentL
        * @since S60 5.0
        * This method is a callback to indicate the start of the document.
        * @param  aDocParam Specifies the various parameters of the document.
        * @arg  aDocParam.iCharacterSetName The character encoding of the document.
        * @param  aErrorCode is the error code. 
        * If this is not KErrNone then special action may be required.
        */
        inline void OnStartDocumentL(
                const RDocumentParameters& aDocParam, 
                TInt aErrorCode);

        /**
        * OnEndDocumentL
        * @since S60 5.0
        * This method is a callback to indicate the end of the document.
        * @param  aErrorCode is the error code. 
        * If this is not KErrNone then special action may be required.
        */
        inline void OnEndDocumentL(
                TInt aErrorCode);

        /**
        * OnStartElementL
        * @since S60 5.0
        * This method is a callback to indicate an element has been parsed.
        * @param  aElement is a handle to the element's details.
        * @param  aAttributes contains the attributes for the element.
        * @param  aErrorCode is the error code.
        *    If this is not KErrNone then special action may be required.
        */
        void OnStartElementL(
                const RTagInfo& aElement, 
                const RAttributeArray& aAttributes, 
                TInt aErrorCode);

        /**
        * OnEndElementL
        * @since S60 5.0
        * This method is a callback to indicate the end of the element has been reached.
        * @param  aElement is a handle to the element's details.
        * @param  aErrorCode is the error code.
        * If this is not KErrNone then special action may be required.
        */
        void OnEndElementL(
                const RTagInfo& aElement, 
                TInt aErrorCode);

        /**
        * OnContentL
        * @since S60 5.0
        * This method is a callback that sends the content of the element.
        * Not all the content may be returned in one go. The data may be sent in chunks.
        * When an OnEndElementL is received this means there is no more content to be sent.
        * @param  aBytes is the raw content data for the element. 
        *     The client is responsible for converting the data to the 
        *     required character set if necessary.
        *     In some instances the content may be binary and must not be converted.
        * @param  aErrorCode is the error code.
        *     If this is not KErrNone then special action may be required.
        */
        void OnContentL(
                const TDesC8& aBytes, 
                TInt aErrorCode);
  
        /**
        * OnStartPrefixMappingL
        * @since S60 5.0
        * This method is a notification of the beginning of the scope of a prefix-URI Namespace mapping.
        * This method is always called before the corresponding OnStartElementL method.
        * @param  aPrefix is the Namespace prefix being declared.
        * @param  aUri is the Namespace URI the prefix is mapped to.
        * @param  aErrorCode is the error code.
        *         If this is not KErrNone then special action may be required.
        */
        inline void OnStartPrefixMappingL(
                const RString& aPrefix, 
                const RString& aUri, 
                TInt aErrorCode);

        /**
        * OnEndPrefixMappingL
        * @since S60 5.0
        * This method is a notification of the end of the scope of a prefix-URI mapping.
        * This method is called after the corresponding DoEndElementL method.
        * @param aPrefix is the Namespace prefix that was mapped.
        * @param aErrorCode is the error code.
        *        If this is not KErrNone then special action may be required.
        */
        inline void OnEndPrefixMappingL(
                const RString& aPrefix, 
                TInt aErrorCode);

        /**
        * OnIgnorableWhiteSpaceL
        * @since S60 5.0
        * This method is a notification of ignorable whitespace in element content.
        * @param  aBytes are the ignored bytes from the document being parsed.
        * @param  aErrorCode is the error code.
        * If this is not KErrNone then special action may be required.
        */
        inline void OnIgnorableWhiteSpaceL(
                const TDesC8& aBytes, 
                TInt aErrorCode);

        /**
        * OnSkippedEntityL
        * @since S60 5.0
        * This method is a notification of a skipped entity. If the parser encounters an 
        * external entity it does not need to expand it - it can return the entity as aName 
        * for the client to deal with.
        * @param  aName is the name of the skipped entity.
        * @param  aErrorCode is the error code.
        * If this is not KErrNone then special action may be required.
        */
        inline void OnSkippedEntityL(
                const RString& aName, 
                TInt aErrorCode);

        /**
        * OnProcessingInstructionL
        * @since S60 5.0
        * This method is a receive notification of a processing instruction.
        * @param  aTarget is the processing instruction target.
        * @param  aData is the processing instruction data. If empty none was supplied.
        * @param  aErrorCode is the error code.
        * If this is not KErrNone then special action may be required.
        */
        inline void OnProcessingInstructionL(
                const TDesC8& aTarget, 
                const TDesC8& aData,
                TInt aErrorCode);

        /**
        * OnError
        * @since S60 5.0
        * This method indicates an error has occurred.
        * @param  aError is the error code
        */
        inline void OnError(
                TInt aErrorCode);

        /**
        * GetExtendedInterface
        * @since S60 5.0
        * This method obtains the interface matching the specified uid.
        * @return 0 if no interface matching the uid is found.
        *         Otherwise, the this pointer cast to that interface.
        * @param  aUid the uid identifying the required interface.
        */
        inline TAny* GetExtendedInterface(
                const TInt32 aUid);
  
    protected:  // New functions
       
        /**
        * Rolls back the installation if it fails or is cancelled.
        * @since S60 5.0
        * @param aOdt The ODT of the theme
        */
        void RollBackL( ChspsODT& aOdt );
 
    private:    
        /**
        * C++ default constructor.
        * @since S60 5.0
        */
        ChspsInstallationHandler( 
                ChspsThemeServer& aThemeServer );
          
        /**
        * By default Symbian 2nd phase constructor is private.
        * @since S60 5.0
        */ 
        void ConstructL();
      
        /**
        * CheckHeaderL
        * Checks the header information and stores it to iOdt
        * @since S60 5.0
        */
        void CheckHeaderL();
        
        /**
        * InstallOdtL
        * Installs the theme skeleton
        * @since S60 5.0
        */
        void InstallSkeletonL( 
                ThspsServiceCompletedMessage& aReturnMsg );        
                
        /**
         * Adds parsed resource into a temporary resource list, from which the resources
         * are applied to an ODT instance at later phase
         * @since S60 5.0
         * @param aArray is either iResourceArray or iLocalizedResourceArray instance 
         * @param aFilename is full path and filename to the resource in an installation folder
         * @param aLanguage is language of the resource (use ELangNone if it's a common resource)
         * @param aResourceType is MIME type or NULL      
         * @param aMimetype (Optional)   
         * @param aTag (Optional)
         */
        void AddResourceL(
                CArrayPtrSeg<ChspsResource>& aArray, 
                const TPtrC aFilename,
                const TLanguage aLanguage,
                const ThspsResourceType aResourceType,
                const TPtrC8 aMimetype,
                const TPtrC8 aTag );
                                
        /**
        * CleanupL
        * Executes cleaning of the target folder prior to the installation process
        * @since S60 5.0
        */
        void CleanupL( 
                const ChspsODT& aUpdateMask );
                
        /**
         * ImportPluginsL         
         * Imports plug-in DOMs referenced by application configuration, other 
         * configuration types are ignored.
         * @since S60 5.0
         */
        void ImportPluginsL( );
        
        /**
        * ParseDocumentL
        * Calls the definition engine to create the dom 
        * @since S60 5.0
        */
        void ParseDocumentL( 
                ChspsODT& aOdt );
        
        /**
        * CompleteRequestL
        * Completes the request message
        * @since S60 5.0
        */
        void CompleteRequestL(const ThspsServiceCompletedMessage aReturnMessage, 
                const TDesC8& aHeaderData = KNullDesC8 );

        /**
        * Send update notifications if necessary.
        * @since S60 5.0
        */        
        void NotifyOdtUpdatedL();        

        /**
        * Get active application configurations.
        * @since S60 5.0        
        * @param aActiveAppConfs    Target list for active application configurations.
        */        
        void GetActiveAppConfsL( CArrayPtrSeg<ChspsODT>& aActiveAppConfs );
        
        /**
        * SetODTAsResourceL
        * Stores the given ODT as a resources on the resource list
        * @since S60 5.0
        */
        void SetODTAsResourceL( 
                ChspsODT& aOdt );

        /**
        * ActivateThemeL
        * Activates the installed theme.
        * @since S60 5.0
        */
        void ActivateThemeL();
                                               
        /**
         * Parses given DOM document and appends it with a content from possible plugin DOMs.                  
         * @since S60 5.0
         * @param aAppODT is the ODT which should be modified                  
         */
        void AppendPluginConfigurationsL( 
                ChspsODT& aAppODT );
                         
        /**
         * Resets memeber variables. 
         * Used by the autoinstaller when installation handler is created only once.
         * @since S60 5.0         
         */
        void ResetL();
        
        /**
         * Finds locale specific subdirectories and resources and appends those
         * into the resource array
         * @since S60 5.0
         * @param aPath is a directory where the locale specific subdirectories exits
         */
        void AddLocalesL(
                const TDesC& aPath );
        
        /**
         * Adds localized resources from the provided subdirectory
         * @since S60 5.0
         * @param aLanguage is name of the subdirectory
         * @param aPath is path to the subdirectory where the locale specific resources can be found                  
         */
        void AddLocalizedResourcesL(
                const TDesC& aPath,
                const TLanguage aLanguage );
        
        /**
         * Returns a path if V2 directory structure is in use 
         * and if an interface uid is known.
         * @return path to a Xuikon etc folder or an empty string
         */
        TFileName GetInterfacePath();
                
        /**
         * Add DTD files from the HSPS folder.
         */
        void AddHspsLocalesV2L(
                const TDesC& aPath );
        
        void AddLocalizedResourcesDTDV2L(
                const TDesC& aPath,
                const TLanguage aLanguage );
        
        void AddInterfaceResourcesV2L(
                const TDesC& aPath );

        void AddLocalizedResourcesV2L(
                const TDesC& aPath,
                const TLanguage aLanguage );
        
        /**
         * Validates manifest contents and installs files into the Plug-in Repository.
         * @since S60 5.0
         */
        void ValidateL();
        
        /**
         * Validates provided UID value.
         * @param aUid Value to be checked
         */
        void ApplyUidRangeTestsL( const TUint aUid );
        
        /**
         * Indicated whether installed plugin was instansiated 
         * in one or more application configurations.
         * @since S60 5.0
         * @return ETrue if the plugin was in use
         */
        TBool IsPluginUsedInAppConfsL();
        
        /**
         * Parser for the filelogo and filepreview elements in manifest files.                  
         * If an icon was provided, it is added to an resource array for copying 
         * to client's private folder. Allocates heap for the result.
         * Supported values:        
         * - skin(<major id> <minor id>):mif(<path> <bitmapid> <maskid>)
         * - mif(<path> <bitmapid> <maskid>)
         * - uid(<application uid>)
         * - <file name>.<png/svg>         
         * @param aValue8 Value of the element         
         * @param aTag A tag for the file resource
         * @param aResultString Fixed declaration with a valid path reference 
         */
        void ParseIconDeclarationL( 
                HBufC8& aValue8, 
                const TDesC8& aTag,
                HBufC*& aResultString );
        
    public: 
        
        ChspsResult* iResult;
    
    private:// Data
                                
        // Stores the current message being processed during asynchronous conversion
        RMessagePtr2 iMessagePtr;
        
        // Result data
        TBuf8<KMaxResultDataLength8> iResultData;
                
        // Maintains the phases in which the installation currently is 
        ThspsInstallationPhase iInstallationPhase;
        
        // Maintains the request status in asynchronous object installation mode
        TRequestStatus* iRequestStatus;
        
        // Flag that indicates that the installation service is used as asynchronous object,
        TInstallationMode iInstallationMode;   
        
        // Pointer to definition engine
        ChspsDefinitionEngineInterface* iDefEngine;
                        
        // Pointer to xml parser
        Xml::CParser* iXmlParser;
        
        // Pointer to ODT of the theme currently being installed
        ChspsODT* iOdt;
        
        // Contains the ODT's header information in marshalled form
        HBufC8* iHeaderData;
                
        // Contains all the resources of an ODT instance when installing has finished
        CArrayPtrSeg<ChspsResource>* iResourceList;
        
        // Contains names of the resource files under the locale specific subdirectories
        CArrayPtrSeg<ChspsResource>* iTempLocalizedResourceList;
        
        HBufC8* iMediaType;                              
        
        // Tag value from a resource file
        HBufC8* iResourceTag;
        
        // True if parsing localized resources (assumes that elements are always in predefined order) 
        TBool iLocalized;
        
        // Holds the default language (applied if there is no DTD for the device language)
        TLanguage iDefaultSpecification;       
        TBool iDefaultSpecificationSet;
                       
        // Path to installation files
        TPtrC iThemeFilePath;
        
        // Application or interface UID of the installed theme
        TUint iRootUid;
        
        // Provider UID of the installed theme
        TUint iProviderUid;
        
        // Theme UID of the installed theme
        TUint iThemeUid;
        
        // Full name of the installed theme
        HBufC* iThemeFullName;
        
        // Short name of the installed theme
        HBufC8* iThemeShortName;
        
        // Version of the installed theme
        HBufC8* iThemeVersion;
        
        // Description of the widget
        HBufC8* iThemeDesc;
        
        // Version of the requested parser
        HBufC* iPackageVersion;
        
        // Family mask for different resolutions and interfaces (vga, vga_tch, qhd_tch, etc)
        TUint32 iFamilyMask;
        
        // Type of the installed application (for example "Hitchcock")
        HBufC8* iApplicationType;
        
        // XML file name of the installed theme
        HBufC* iXmlFile;
        
        // Name of the DTD file (same for each locale)
        HBufC* iDtdFile;
                        
        // Stores the content during parsing of the manifest file 
        HBufC8* iContent;

        // Handle to the file server session
        RFs iFsSession;
                
        // Stores the resource during asynchronous conversion
        ChspsResource* iResource;
        
        // Stores the configuration type
        ThspsConfigurationType iConfigurationType;
        
        // Stores the theme status during installation
        TUint32 iThemeStatus;
        
        // Flag that indicates a missing file during installation -> installation fails
        TBool iFileNotFound;
               
        // Reference to Theme Server
        ChspsThemeServer& iThemeServer;
        
        // Reference to definition repository
        ChspsDefinitionRepository& iDefinitionRepository;
        
        // Reference to security enforcer
        ChspsSecurityEnforcer& iSecurityEnforcer;
                
        // Reference to central repository
        CRepository& iCentralRepository;
        
        // header list cache
        CArrayPtrSeg<ChspsODT>& iHeaderListCache;   
        
        // Set if the package being installed is supported by the server
        TBool iPackageVerSupported;
        
        // Set if "EhspsODTAdded" -notifications should be blocked (e.g. ROM installations)
        TBool iDisableNotifications;

        // Set if installation files are located in ROM
        TBool iRomInstallation;
        
        // Set if widget mutliinstance flag
        TInt32 iMultiInstance;

        // Set if the multiinstance element is present in the manifest.dat file
        TBool iMultiInstanceFound;

#ifdef HSPS_LOG_ACTIVE        
        /**
         * Log bus.
         */        
        ChspsLogBus* iLogBus;
#endif        
        
        /**
         * Installation type.
         */
        enum TInstallationType
            {
            EInstallationTypeNew = 0, // New installation.
            EInstallationTypeUpdate   // Update.
            };
        
        // Installation type for currently active installation.
        TInstallationType iInstallationType;        
    };

    #include "hspsinstallationhandler.inl"

#endif //__hspsINSTALLATIONHANDLER_H__
