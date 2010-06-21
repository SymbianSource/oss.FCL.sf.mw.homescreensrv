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
* Description:  Implementaion of HSPS MhspsInstallationService interface defined 
*                in hspsThemeManagement.h. For details, see the header file.
*
*/


#include <bautils.h>
#include <utf.h>
#include <centralrepository.h>

#include "hsps_builds_cfg.hrh"
#include "hspsthememanagement.h"
#include "hspsdefinitionrepository.h"
#include "hspsodt.h"
#include "hspsdefinitionengineinterface.h"
#include "hspsdomdocument.h"
#include "hspsresource.h"
#include "hspsresult.h"
#include "hspsthemeserver.h"
#include "hspsinstallationhandler.h"
#include "hspssecurityenforcer.h"
#include "hspsuimanagererrorcodes.h"
#include "hspsdomattribute.h"
#include "hspsdomlist.h"
#include "hspsdomdepthiterator.h"
#include "hspsdomnode.h"
#include "hspsconfiguration.h"
#include "hspsmanifest.h"
#include "hspsserverutil.h"
#include "hspsfamilylistener.h"


#ifdef HSPS_LOG_ACTIVE
#include <hspsodtdump.h>
#include <hspslogbus.h>
#endif

#ifdef _hsps_PERFORMANCE_TEST_
#include "hspstimemon.h"
#endif

_LIT8(KUnknownMimeType, "unknown");    

_LIT8(KhspsDefinitionEngine, "hspsdefinitionengine");

_LIT(KPathDelim, "\\");
_LIT(KPrivateInstall, "\\private\\200159c0\\install\\");
_LIT(KHsps, "\\hsps\\" );
_LIT(KXuikon, "xuikon" );

const TInt KMaxMediaTypeLength = 100;

// ========================= LOCAL FUNCTIONS ==================================

// ----------------------------------------------------------------------------
// CleanupResetAndDestroy()
// ----------------------------------------------------------------------------
//
template<class T>
static void CleanupResetAndDestroy( TAny* aObj )
    {
    if( aObj )
        {
        static_cast<T*>( aObj )->ResetAndDestroy();
        }
    }

// ----------------------------------------------------------------------------
// CleanupResetAndDestroyPushL
// ----------------------------------------------------------------------------
//
template<class T>
static void CleanupResetAndDestroyPushL(T& aArray)
    {
    CleanupStack::PushL( TCleanupItem( &CleanupResetAndDestroy<T>, &aArray ) );
    }

// ========================= MEMBER FUNCTIONS ==================================

// -----------------------------------------------------------------------------
// Callback function for removing repository lock if error occurs while repository is locked
// -----------------------------------------------------------------------------
//
LOCAL_C void UnlockRepository( TAny* aObject )
    {
    ChspsDefinitionRepository* DefRep = reinterpret_cast<ChspsDefinitionRepository*>( aObject );
        
    if (DefRep->Locked())
        {
        DefRep->Unlock();
        }
    }

// -----------------------------------------------------------------------------
// ChspsInstallationHandler::ChspsInstallationHandler()
// C++ default constructor can NOT contain any code, that
// might leave.
// -----------------------------------------------------------------------------
//
ChspsInstallationHandler::ChspsInstallationHandler( ChspsThemeServer& aThemeServer )
    : iThemeServer( aThemeServer ), 
    iDefinitionRepository( aThemeServer.DefinitionRepository() ),
    iSecurityEnforcer( aThemeServer.SecurityEnforcer() ),    
    iCentralRepository( aThemeServer.CentralRepository() ),
    iHeaderListCache( aThemeServer.HeaderListCache() )
    {     
    iPackageVerSupported = EFalse;
    iInstallationPhase = EhspsPhaseIdle;
    iConfigurationType = EhspsAppConfiguration;
    iThemeStatus = EhspsThemeStatusNone;
    iFileNotFound = EFalse;
    iLocalized = EFalse;             
    iDefaultSpecificationSet = EFalse;
    iDefaultSpecification = ELangNone;
    iInstallationMode = EServiceHandler;
    iTrustedInstallation = EFalse;
	iInstallationType = EInstallationTypeNew;
	iInstallFromUDAEmmc = ETrue;
	iFamilyMask = 0;
    }
    
// -----------------------------------------------------------------------------
// ChspsInstallationHandler::NewL
// Two-phased constructor.
// -----------------------------------------------------------------------------
//
ChspsInstallationHandler* ChspsInstallationHandler::NewL( ChspsThemeServer& aThemeServer )
    {
    ChspsInstallationHandler* h = ChspsInstallationHandler::NewLC( aThemeServer );
    CleanupStack::Pop( h );
    return ( h );
    }

// -----------------------------------------------------------------------------
// ChspsInstallationHandler::NewL
// Two-phased constructor.
// -----------------------------------------------------------------------------
//
ChspsInstallationHandler* ChspsInstallationHandler::NewLC( ChspsThemeServer& aThemeServer )
    {
    ChspsInstallationHandler* h = new (ELeave) ChspsInstallationHandler( aThemeServer );
    CleanupStack::PushL( h );
    h->ConstructL();
    return ( h );
    }


// -----------------------------------------------------------------------------
// Destructor
// -----------------------------------------------------------------------------
//
ChspsInstallationHandler::~ChspsInstallationHandler()
    {        
    iFsSession.Close();
    delete iXmlParser;
    delete iOdt;
    delete iDtdFile;
    delete iHeaderData;
    delete iThemeFullName;
    delete iThemeShortName;
    delete iThemeVersion;
    delete iThemeDesc;
    delete iResourceTag;
    delete iPackageVersion;
    if ( iTempLocalizedResourceList )
        {
        iTempLocalizedResourceList->ResetAndDestroy();
        delete iTempLocalizedResourceList;
        }
    if( iResourceList )
        {
        iResourceList->Reset(); // ODT has an ownership of the items
        delete iResourceList;
        }
    delete iContent;
    delete iXmlFile;
    delete iMediaType;
    delete iResource;
    delete iResult;
    
    if ( iDefEngine )
        {
        delete iDefEngine;
        }

    REComSession::FinalClose(); 

    }
    
    
    
    
// -----------------------------------------------------------------------------
// ChspsInstallationHandler::ConstructL
// Symbian 2nd phase constructor can leave.
// -----------------------------------------------------------------------------
//
void ChspsInstallationHandler::ConstructL()
    {
    _LIT8(KMimeType, "text/xml");
    MContentHandler* contentHandler = this;
    iXmlParser = Xml::CParser::NewL( KMimeType, *contentHandler );
    
     iDefEngine = ChspsDefinitionEngineInterface::NewL(KhspsDefinitionEngine);
         
    iOdt = ChspsODT::NewL();
    iResourceList = new( ELeave ) CArrayPtrSeg<ChspsResource>( KPathListGranularity ); 
    iTempLocalizedResourceList = new( ELeave ) CArrayPtrSeg<ChspsResource>( KPathListGranularity );
    User::LeaveIfError( iFsSession.Connect() );    
    iResult = ChspsResult::NewL();
     
    iMultiInstanceFound = EFalse;
    
    }

// -----------------------------------------------------------------------------
// ChspsInstallationHandler::ServiceInstallThemeL
// Starts the actual installation in ChspsInstallationHandler.
// (other items were commented in a header).
// -----------------------------------------------------------------------------
//
void ChspsInstallationHandler::ServiceInstallThemeL( const RMessage2& aMessage )
    {    
 	// incoming arguments: TIpcArgs: &iResultData, &aManifestFileName, &aHeader
 	iMessagePtr = aMessage;
 	TBuf8<KMaxHeaderDataLength8> headerdata;
	
	iInstallationPhase = EhspsPhaseIdle;
	ThspsServiceCompletedMessage ret = EhspsInstallThemeFailed;
 	    
    if ( !iDefinitionRepository.Locked() )
        {
        // read name of the manifest file
        TBuf<KMaxHeaderDataLength8> manifestfilename;                
        iMessagePtr.ReadL( 1, manifestfilename, 0 );

#ifdef HSPS_LOG_ACTIVE  
        if( iLogBus )
            {
            iLogBus->LogText( _L( "ChspsInstallationHandler::ServiceInstallThemeL() - Manifest file '%S'" ),
                    &manifestfilename );
            }
#endif        

		// install the manifest file
		ret = hspsInstallTheme( manifestfilename, headerdata );
		
		// now there will be the time for query validity check
		if ( ret == EhspsInstallThemeSuccess )
			{
   		    ret = EhspsInstallPhaseSuccess;
            }
        }
       else
        {
        iResult->ResetData();
        iResult->iSystemError = KErrInUse;
        } 
    
    CompleteRequestL(ret, headerdata);
    }


// -----------------------------------------------------------------------------
// ChspsInstallationHandler::ServiceInstallNextPhaseL
// Starts subsequent installation phases.
// (other items were commented in a header).
// -----------------------------------------------------------------------------
//
void ChspsInstallationHandler::ServiceInstallNextPhaseL( const RMessage2& aMessage )
    {        
#ifdef HSPS_LOG_ACTIVE  
    if( iLogBus )
        {
        iLogBus->LogText( _L( "ChspsInstallationHandler::ServiceInstallNextPhaseL():" ) );
        }
#endif
    
    TBuf8<KMaxHeaderDataLength8> headerdata;
    iResult->ResetData();
    iInstallationMode = EServiceHandler;
    ThspsServiceCompletedMessage ret = EhspsInstallThemeFailed;
    // incoming arguments: TIpcArgs: &iResultData, &KNullDesC, &aHeaderData    
    iMessagePtr = aMessage;
    
    // calling installation of next phase
    ret = hspsInstallNextPhaseL( headerdata );
    CompleteRequestL( ret, headerdata );        
    }


// -----------------------------------------------------------------------------
// ChspsInstallationHandler::InstallNextPhase()
// Must be completed by EhspsInstallThemeSuccess, EhspsInstallPhaseSuccess or EhspsInstallThemeFailed 
// (other items were commented in a header).
// -----------------------------------------------------------------------------
//
void ChspsInstallationHandler::hspsInstallNextPhaseL( 
        TDes8& aHeaderData, 
        TRequestStatus& aRequestStatus )
    {
    ThspsServiceCompletedMessage ret = EhspsServiceNotSupported;
    iInstallationMode = EAsynchronousObject;
    iRequestStatus = &aRequestStatus;
    *iRequestStatus = KRequestPending;
    ret = hspsInstallNextPhaseL( aHeaderData );
    CompleteRequestL( ret );
    }

// -----------------------------------------------------------------------------
// ChspsInstallationHandler::ResetL()
// -----------------------------------------------------------------------------
void ChspsInstallationHandler::ResetL()
    {
    iThemeFilePath.FillZ();    
    iFamilyMask = 0;
    iInstallationPhase = EhspsPhaseInitialise;    
    iThemeStatus = EhspsThemeStatusNone;
    iTrustedInstallation = EFalse;
    iFileNotFound = EFalse;
    delete iMediaType;
    iMediaType = NULL;
    delete iThemeDesc;
    iThemeDesc = NULL;
    delete iResourceTag;
    iResourceTag = NULL;
    iLocalized = EFalse;    
    iDefaultSpecificationSet = EFalse;
    iDefaultSpecification = ELangNone;
    if ( iOdt )
       {
       delete iOdt;
       iOdt = NULL;
       iOdt = ChspsODT::NewL();
       }
    
    delete iDtdFile;
    iDtdFile = NULL;
    
    delete iHeaderData;
    iHeaderData = NULL;
    if ( iDefEngine )
       {
       delete iDefEngine; 
       iDefEngine = NULL;
       iDefEngine = ChspsDefinitionEngineInterface::NewL(KhspsDefinitionEngine);
       }   
    if ( iResourceList )
       {
       iResourceList->Reset(); // ODT has an ownership of the items
       delete iResourceList;
       iResourceList = NULL;
       iResourceList = new( ELeave ) CArrayPtrSeg<ChspsResource>( KPathListGranularity ); 
       }    
    if ( iTempLocalizedResourceList )
       {
       iTempLocalizedResourceList->ResetAndDestroy(); 
       delete iTempLocalizedResourceList;
       iTempLocalizedResourceList = NULL;
       iTempLocalizedResourceList = new( ELeave ) CArrayPtrSeg<ChspsResource>( KPathListGranularity );
       }
    iInstallationType = EInstallationTypeNew;
    }

// -----------------------------------------------------------------------------
// ChspsInstallationHandler::hspsInstallTheme()
// From MhspsInstallationService 
// Must be completed by EhspsInstallThemeSuccess, EhspsInstallPhaseSuccess or EhspsInstallThemeFailed 
// (other items were commented in a header).
// -----------------------------------------------------------------------------
//
ThspsServiceCompletedMessage ChspsInstallationHandler::hspsInstallTheme(
        const TDesC& aManifestFileName, 
        TDes8& aHeaderData)
    {
    // Assume that the installation fails
    ThspsServiceCompletedMessage ret = EhspsInstallThemeFailed;                       
    iResult->iXuikonError = 0;    
    iResult->iIntValue2 = 0;
    
    TRAPD( err, DoInstallThemeL(aManifestFileName) );
    if( !err )
        {
        // correct headerdata is in iHeaderData set by CheckHeaderL()
        aHeaderData = iHeaderData->Des();        
                   
        // Set next phase
        iInstallationPhase = EhspsPhaseCleanup;
        ret = EhspsInstallThemeSuccess;
        }
    else
        {     
#ifdef HSPS_LOG_ACTIVE  
        if( iLogBus )
            {
            iLogBus->LogText( _L( "ChspsInstallationHandler::hspsInstallTheme(): - Installation failed with error code %d" ),
                    err );
            }
#endif       
        }
        
    iResult->iSystemError = err;
    return ret;
    }

// -----------------------------------------------------------------------------
// ChspsInstallationHandler::DoInstallThemeL()
// -----------------------------------------------------------------------------
void ChspsInstallationHandler::DoInstallThemeL(
        const TDesC& aManifest )
    {        
    // Reset memeber variables    
    ResetL();
        
    TParsePtrC parsePtr( aManifest );        
                
    // Store the path to the installation file     
    __ASSERT_DEBUG( aManifest.Length() < KMaxFileName, User::Leave( KErrBadName ) );
    iThemeFilePath.Copy( parsePtr.DriveAndPath() );    
        
    // If trying to install widgets from one of our private install directories
    const TPath path = parsePtr.Path();
    if( path.Length() > KPrivateInstall().Length() 
            && path.Left( KPrivateInstall().Length() ) == KPrivateInstall() ) 
        {                    
        // Check if ROM or UDA installation was requested
        if( parsePtr.DrivePresent() )
            {            
            TInt driveNumber;
            if ( RFs::CharToDrive( parsePtr.Drive()[0], driveNumber ) == KErrNone )
                {
                iTrustedInstallation = ( driveNumber == EDriveZ || driveNumber == EDriveC );                
                }
            }
        }    
            
    if( !BaflUtils::FileExists( iFsSession, aManifest ) )
        {
#ifdef HSPS_LOG_ACTIVE    
        if ( iLogBus )
            {           
            iLogBus->LogText( _L( "ChspsInstallationHandler::DoInstallThemeL() - *** Manifest was not found!" ) );
            }
#endif    
        User::Leave( KErrNotFound );
        }
       
#ifdef HSPS_LOG_ACTIVE    
    if ( iLogBus )
        {            
        iLogBus->LogText( _L( "ChspsInstallationHandler::DoInstallThemeL() - *** Parsing a manifest file" ) );
        }
#endif       
    
    // Parse XML from the manifest file
    Xml::ParseL( *iXmlParser, iFsSession, aManifest );    
                    
    // The manifest file has been read at this point and following callbacks have been executed:
    // OnContent(), OnStartElement() and OnEndElement()
    
    // Performance optimization: do not check if installing from rom.
    if( !iTrustedInstallation )
        {
        // Check type of installation        
        TBool instancesFound = IsPluginUsedInAppConfsL();            
        if( iThemeServer.PluginInHeaderCache( TUid::Uid( iThemeUid ) ) && instancesFound )                    
            {
            // Plugin should be found from cache, update notifications are
            // sent only when plugins are used by one/more app configurations
            iInstallationType = EInstallationTypeUpdate;
            }
        else
            {
            iInstallationType = EInstallationTypeNew;
            }
        }                
        
    // Check the parsed input        
    FinalizeParsingL();                    
    }

// -----------------------------------------------------------------------------
// ChspsInstallationHandler::GetInterfacePath() 
// -----------------------------------------------------------------------------
//
TFileName ChspsInstallationHandler::GetInterfacePath()
    {        
    TFileName path;
    
    TParse pathParser;
    pathParser.Set( iThemeFilePath, NULL, NULL );                               
    pathParser.PopDir(); // pop locale specific folder  
                
    TPtrC parentFolder = pathParser.FullName().Mid( pathParser.FullName().Length() - KHsps().Length() );
    if ( parentFolder.CompareF(KHsps) == 0 )
        {
        pathParser.PopDir(); // pop "hsps" folder 
        path.Copy( pathParser.FullName() );
        path.Append( KXuikon );
        path.Append( KPathDelim );
        }
    return path;
    }

// -----------------------------------------------------------------------------
// ChspsInstallationHandler::ValidateL() 
// -----------------------------------------------------------------------------
//
void ChspsInstallationHandler::FinalizeParsingL()
    {               
    // Add resources
    TFileName interfacePath( GetInterfacePath() );       
    if ( interfacePath.Length() )        
        {
        // If name of the DTD file was specified in the manifest
        if ( iDtdFile )
            { 
            TParse pathParser;
            pathParser.Set( iThemeFilePath, NULL, NULL );                               
            pathParser.PopDir(); // pop locale specific folder
            
            // Find locale specific DTD file
            AddLocalesL( pathParser.FullName(), ETrue );
            }
        
        // Find Xuikon resources of each locale 
        AddInterfaceResourcesV2L( interfacePath );
       }
    else
       {
       // Find DTD files and locale specific resources from subdirectories under the installation path
       AddLocalesL( iThemeFilePath, EFalse );                       
       }       
          
   // Validate other input from the manifest
   CheckHeaderL();
           
   if ( iSecurityEnforcer.CheckThemeLockingL( *iOdt ) )
       {
       iResult->iXuikonError = KErrThemeStatusLocked;    
#ifdef HSPS_LOG_ACTIVE  
       if( iLogBus )
           {
           iLogBus->LogText( _L( "ChspsInstallationHandler::ValidateL(): - CheckThemeLockingL" ) );
           }
#endif              
       User::Leave( KErrAccessDenied );
       } 
                           
#ifdef HSPS_LOG_ACTIVE  
   // printing some debug-info
   TPtrC xmlfile = iXmlFile->Des();
   if( iLogBus )
       {
       iLogBus->LogText( _L( "ChspsInstallationHandler::ValidateL(): - iXmlFile = '%S'" ),
               &xmlfile );

       iLogBus->LogText( _L( "ChspsInstallationHandler::ValidateL(): - resources included = %d" ),
               iResourceList->Count() );
       }
#endif                   
    }
         

// -----------------------------------------------------------------------------
// Execution of specific installation phases and transitions in between.
// Must be completed by EhspsInstallThemeSuccess, EhspsInstallPhaseSuccess or EhspsInstallThemeFailed 
// -----------------------------------------------------------------------------
//
ThspsServiceCompletedMessage ChspsInstallationHandler::hspsInstallNextPhaseL( TDes8& aHeaderData )
       {
       // Defaults
       ThspsServiceCompletedMessage ret = EhspsInstallPhaseSuccess;
       TInt errorCode = KErrNone;   
    
    if ( !iDefinitionRepository.Locked() )
        {
        iDefinitionRepository.Lock();
        }

    //For unlocking repository in error cases
    CleanupStack::PushL( TCleanupItem( UnlockRepository, &iDefinitionRepository ) );
    
       switch ( iInstallationPhase )
        {
        case EhspsPhaseIdle:
               {
               ret = EhspsServiceRequestError;
               }
               break;
        
        case EhspsPhaseCleanup:
            {            
            if ( iOdt )
                {
                // Remove existing/old files from the Plug-in Repository
                TRAP( errorCode, CleanupL( *iOdt ) );                
                }
            if ( errorCode )
                {
                iResult->iSystemError = errorCode;
                iResult->iXuikonError = errorCode;
                CleanupStack::Pop(&iDefinitionRepository);
                return EhspsInstallThemeFailed;
                } 
            else
                {
                iInstallationPhase = EhspsPhaseInstallSkeleton;
                }
            }
               break;
                                       
        case EhspsPhaseInstallSkeleton:
               {
               // Parses and stores DOM into the ODT being installed
            iResult->iIntValue2 = 0;
               TRAP( errorCode, InstallSkeletonL( ret ) );
               if ( errorCode )
                   {
                   ret = EhspsInstallThemeFailed;
                   }
               else
                   {
                   iResult->iIntValue2 = 0;
                   
                   // Success - installation finished
                ret = EhspsInstallThemeSuccess;
                iInstallationPhase = EhspsPhaseIdle;
                   }
               }
               break;
                   
        default:
            {
            iResult->iSystemError = KErrNotReady;
            ret = EhspsServiceRequestError;
            }
            break;
        } // switch
        
       if ( iHeaderData )
           {
           aHeaderData = iHeaderData->Des();
           }
       else
           {
           aHeaderData = KNullDesC8;
           }
   
    CleanupStack::Pop(&iDefinitionRepository);
       return ret;
       }
            
            
// -----------------------------------------------------------------------------
// ChspsInstallationHandler::hspsCancelInstallTheme()
// Cancels the theme installation
// (other items were commented in a header).
// -----------------------------------------------------------------------------
//
ThspsServiceCompletedMessage ChspsInstallationHandler::hspsCancelInstallTheme()
    {
    TRAP_IGNORE( CompleteRequestL( EhspsServiceRequestCanceled ));
    return EhspsServiceRequestCanceled;
    }            


// -----------------------------------------------------------------------------
// ChspsInstallationHandler::CheckAutoInstallationValidityL()
// Returns EFalse if the user tries to install Licensee default theme
// (other items were commented in a header).
// -----------------------------------------------------------------------------
//
TBool ChspsInstallationHandler::CheckAutoInstallationValidityL()
    {
    if ( iOdt->Flags() & EhspsThemeStatusLicenceeDefault )
        {
        iResult->iXuikonError = KErrIllegalInstallation; 
        return EFalse;
        }
    return ETrue;
    }

// -----------------------------------------------------------------------------
// ChspsInstallationHandler::SetLogBus()
// -----------------------------------------------------------------------------
//
#ifdef HSPS_LOG_ACTIVE        
void ChspsInstallationHandler::SetLogBus( ChspsLogBus* aLogBus )
    {
    iLogBus = aLogBus;
    }
#endif


// ChspsInstallationHandler::InstallOdtL()
// Installs DOM-document ODT and processes resource list
// (other items were commented in a header).
// -----------------------------------------------------------------------------
//
void ChspsInstallationHandler::InstallSkeletonL( ThspsServiceCompletedMessage& /*aReturnMsg*/ )
    {
#ifdef HSPS_LOG_ACTIVE  
    if( iLogBus )
        {
        iLogBus->LogText( _L( "hspsInstallationHandler::InstallSkeletonL(): - installing and localizing configuration" ) );
        }
#endif  
          
    iOdt->SetOdtLanguage( ELangNone );
        
    // Parse DOM
    ParseDocumentL( *iOdt );
    
    // Save ODT itself as a resource        
    SetODTAsResourceL( *iOdt );                               
                                                     
    // Add resources parsed from the manifest file into the new ODT instance, generate
    // target paths and update the iResourcesList array with the new paths
    User::LeaveIfError( iDefinitionRepository.SetResourceListL( *iOdt, *iResourceList ) );      
           
    // Update DOM's configuration node with attributes from the manifest    
    hspsServerUtil::GenerateConfigurationAttributesL( *iOdt );
    
    TBuf8<10> uid;
    _LIT8( KFormat8, "%X" );
    _LIT8( KHexPrefix, "0x" );
    uid.Append( KHexPrefix );
    uid.AppendFormat( KFormat8, iOdt->ThemeUid() );

    // Update configuration max child count
    if ( hspsServerUtil::FindNodeByTagL( 
        KPluginsElement, 
        *( iOdt->DomDocument().RootNode() ) ) )
        {
        TPtrC8 maxChildCnt;
        // Plugins node found - Configuration can include child configurations
        TRAPD( err, hspsServerUtil::GetAttributeValueL( 
            *iOdt,
            KConfigurationElement,
            KConfigurationAttrUid,
            uid,
            KConfigurationAttrMaxChild,
            maxChildCnt) );
        if ( err )
            {
            // Set default max child configuration count
            hspsServerUtil::SetAttributeValueL(
                *iOdt,
                KConfigurationElement,
                KConfigurationAttrUid,
                uid,
                KConfigurationAttrMaxChild,
                _L8( "6" ) );
            }
        }
    else
        {
        // Configuration cannot indluce child configurations
        hspsServerUtil::SetAttributeValueL(
            *iOdt,
            KConfigurationElement,
            KConfigurationAttrUid,
            uid,
            KConfigurationAttrMaxChild,
            _L8( "0" ) );
        }

    if ( iTrustedInstallation )
        {
        // Update configuration state to KConfStateConfirmed
        hspsServerUtil::SetAttributeValueL( 
            *iOdt,
            KConfigurationElement,
            KConfigurationAttrUid,
            uid,
            KConfigurationAttrState,
            KConfStateConfirmed
            );
        }
    
    iOdt->SetOdtLanguage( ELangTest );
    
    // Add a resouces node and its objects from the manifest into the DOM
    hspsServerUtil::GenerateObjectAttributesL( *iOdt );        
        

        
            
    // Make localizations according to the active device language
    iThemeServer.LocalizeL(
        *iDefEngine,
        *iOdt );
    
    // Store the ODT instance into the file system
    iDefinitionRepository.SetOdtL( *iOdt );  
       }

// -----------------------------------------------------------------------------
// ChspsInstallationHandler::CheckHeaderL()
// Checks the installed theme's header correctness
// (other items were commented in a header).
// -----------------------------------------------------------------------------
//
void ChspsInstallationHandler::CheckHeaderL()
    {
    // Check whether the manifest is supported by the installer
    if ( !iPackageVerSupported )
        {
#ifdef HSPS_LOG_ACTIVE  
        if( iLogBus )
            {
            iLogBus->LogText( _L( "ChspsInstallationHandler::CheckHeaderL(): - package version is not supported by the server!" ) );
            iLogBus->LogText( _L( "ChspsInstallationHandler::CheckHeaderL(): - Update plug-in configurations and try again" ) );
            }
#endif
    
        User::Leave( KErrNotSupported );     
        }

    ChspsFamily* family = iThemeServer.Family();
    if ( !family )
        {
        User::Leave( KErrNotSupported );
        }

    ThspsFamily familyType = family->GetFamilyType();
    if ( familyType == EhspsFamilyUnknown )
        {
        User::Leave( KErrNotSupported );
        }
#if !defined(WINSCW) && !defined(__WINS__)
    if ( !( familyType & iFamilyMask ) )
        {
        User::Leave( KErrNotSupported );
        }
#endif // !defined(WINSCW) && !defined(__WINS__)
    iOdt->SetFamily( iFamilyMask );    
            
    // Store root, provider and theme uid
    if ( iRootUid && iProviderUid && iThemeUid )
        {
        iOdt->SetRootUid( iRootUid );
        iOdt->SetProviderUid( iProviderUid );
        iOdt->SetThemeUid( iThemeUid );
        }
    else
        {
#ifdef HSPS_LOG_ACTIVE  
        if( iLogBus )
            {
            iLogBus->LogText( _L( "ChspsInstallationHandler::CheckHeaderL(): - Manifest file error, no UIDs" ) );
            }
#endif    
        
        // If the content is empty, the file is supposedly wrong
        if( !iContent )
            {
            iResult->iXuikonError = KErrWrongManifestFile;
            User::Leave( KErrNotFound );            
            }
        // something goes wrong...
        if( !iRootUid && !iProviderUid && !iThemeUid &&
            !iThemeFullName && !iThemeShortName && !iThemeVersion )
            {
            iResult->iXuikonError = KErrOtherXuikonError;
            User::Leave( KErrNotFound );
            }
        if( !iRootUid )
            {
            iResult->iXuikonError = KErrAppUidDefinitionMissing;
            User::Leave( KErrNotFound );
            }
        if( !iProviderUid )
            {
            iResult->iXuikonError = KErrProviderUidDefinitionMissing;
            User::Leave( KErrNotFound );
            }
        if( !iThemeUid )
            {
            iResult->iXuikonError = KErrThemeUidDefinitionMissing;
            User::Leave( KErrNotFound );
            }
        User::Leave(KErrNotFound);
        }
    
    // Store name, short name and version
    HBufC* themeFullName = NULL;
    HBufC* themeShortName = NULL;
    HBufC* themeVersion = NULL;    
    if ( iThemeFullName && iThemeShortName && iThemeVersion )
        {
        themeFullName = HBufC::NewLC( iThemeFullName->Length() );
        themeFullName->Des().Copy( *iThemeFullName ); 
        themeShortName = HBufC::NewLC( iThemeShortName->Length() );
        themeShortName->Des().Copy( *iThemeShortName ); 
        themeVersion = HBufC::NewLC( iThemeVersion->Length() );
        themeVersion->Des().Copy( *iThemeVersion );
        }
    else
        {
#ifdef HSPS_LOG_ACTIVE  
        if( iLogBus )
            {
            iLogBus->LogText( _L( "ChspsInstallationHandler::CheckHeaderL(): - Manifest file error, no names" ) );
            }
#endif    
        
        if( !iThemeFullName )
            {
            iResult->iXuikonError = KErrThemeFullNameDefinitionMissing;
            User::Leave( KErrNotFound );
            }
        if( !iThemeShortName )
            {
            iResult->iXuikonError = KErrThemeShortDefinitionNameMissing;
            User::Leave( KErrNotFound );
            }
        if( !iThemeVersion )
            {
            iResult->iXuikonError = KErrThemeVersionDefinitionMissing;
            User::Leave( KErrNotFound );
            }
        User::Leave(KErrNotFound);
        }
    iOdt->SetThemeFullNameL( themeFullName->Des() );
    iOdt->SetThemeShortNameL( themeShortName->Des() );
    iOdt->SetThemeVersionL( themeVersion->Des() );
    if ( iThemeDesc )
        {        
        HBufC* buf = HBufC::NewLC( iThemeDesc->Length() );
        buf->Des().Copy( iThemeDesc->Des() );
        iOdt->SetDescriptionL( buf->Des() );
        CleanupStack::PopAndDestroy( buf );
        }    
    iOdt->SetMultiInstance( iMultiInstance );
    iMultiInstanceFound = EFalse;

    CleanupStack::PopAndDestroy( themeVersion );
    CleanupStack::PopAndDestroy( themeShortName );
    CleanupStack::PopAndDestroy( themeFullName );
    
    iOdt->SetConfigurationType( iConfigurationType );
    iOdt->SetFlags( iThemeStatus );   

    // If configuration file is missing
    if( !iXmlFile ) 
        {
#ifdef HSPS_LOG_ACTIVE  
        if( iLogBus )
            {
            iLogBus->LogText( _L( "ChspsInstallationHandler::CheckHeaderL(): - XML was not declared!" ) );
            }
#endif                   
        iResult->iXuikonError = KErrXmlFileDefinitionMissing;
        User::Leave( KErrNotFound );
        }
     
    // If name of the DTD file has been set
    if ( iDtdFile )
        {
        // Expect localization for at least ELangTest (folder name "00")
        if ( !iDefaultSpecificationSet || iDefaultSpecification != ELangTest )
            {
            iResult->iXuikonError = KErrDtdFileNotFound;
#ifdef HSPS_LOG_ACTIVE  
            if( iLogBus )
                {
                iLogBus->LogText( _L( "ChspsInstallationHandler::CheckHeaderL(): - check localization!" ) );
                }
#endif            
            User::Leave( KErrNotFound );
            }
        }
    
    // all header-information is in place, now try to marshall it as a return of the service call
    iHeaderData = iOdt->MarshalHeaderL();
    }

// -----------------------------------------------------------------------------
// ChspsInstallationHandler::ParseDocumentL
// Parses the skeleton DOM with the help of Definition Engine
// (other items were commented in a header).
// -----------------------------------------------------------------------------
//
void ChspsInstallationHandler::ParseDocumentL( ChspsODT& aOdt )
    {
    TInt deferror = KErrNone;
    TInt errorCode = KErrNone;
    ChspsDefinitionEngineInterface::TError t_error;  

#ifdef _hsps_PERFORMANCE_TEST_    
    // service time prints
    _LIT( KStartTiming, "ChspsInstallationHandler::ParseDocumentL(): - calling iDefEngine->CreateDOM()..");
    TTime start_time = ChspsTimeMon::StartTiming( KStartTiming );
#endif    
    
    t_error = iDefEngine->CreateDOM( iFsSession, *iXmlFile, aOdt );
          
    deferror = t_error.iDefEngError + t_error.iSubComponentError;
    
    if (!deferror)
        {
        // get ODT information
           ChspsDomDocument& domDocument = aOdt.DomDocument();
        TInt domNodeCount = domDocument.DomNodeCount();
        // write the result
        iResult->iIntValue1 = 0;
        iResult->iIntValue2 = domNodeCount;

#ifdef _hsps_PERFORMANCE_TEST_  
        // calculating service time
         _LIT( KStopTiming, "ChspsInstallationHandler::ParseDocumentL(): - Parsing success." );
         ChspsTimeMon::StopTiming(start_time, KStopTiming );
#endif
        }
    else
        {
        errorCode = KErrCorrupt;
        iResult->iXuikonError = errorCode;

        iResult->iIntValue1 = t_error.iDefEngError;
        iResult->iIntValue2 = t_error.iSubComponentError;

#ifdef HSPS_LOG_ACTIVE  
        if( iLogBus )
            {
            iLogBus->LogText( _L( "ChspsInstallationHandler::ParseDocumentL(): - TRAP returned %d" ),
                    errorCode );
            iLogBus->LogText( _L( "ChspsInstallationHandler::ParseDocumentL(): - ChspsDefinition Engine::TError.iParserError: %d" ),
                    t_error.iDefEngError );
            iLogBus->LogText( _L( "ChspsInstallationHandler::ParseDocumentL(): - ChspsDefinition Engine::TError.iSubComponentError: %d" ),
                    t_error.iSubComponentError );
            }
#endif      
        }
          
    User::LeaveIfError(errorCode);        
    }

// -----------------------------------------------------------------------------
// ChspsInstallationHandler::AddResourceL()
// Adds parsed resource into a temporary resource list, from which the resources
// are applied to an ODT instance at later phase
// -----------------------------------------------------------------------------
//
void ChspsInstallationHandler::AddResourceL(
        CArrayPtrSeg<ChspsResource>& aArray, 
        const TPtrC aFilename,
        const TLanguage aLanguage,
        const ThspsResourceType aResourceType,
        const TPtrC8 aMimetype,
        const TPtrC8 aTag )        
    {        
    TParsePtrC parsePtr( aFilename );
        
    ChspsResource* r = ChspsResource::NewL();
    CleanupStack::PushL(r);
    
    r->SetLockingPolicy( EhspsUnlocked );
    if ( iThemeStatus & EhspsThemeStatusLicenceeDefault )
        {
        r->SetLockingPolicy( EhspsLocked );
        }                   
    r->SetResourceIdL( parsePtr.NameAndExt() );
    
    // Common resources are parsed first - thus when the iLanguageSpecification is other than ELangNone, 
    // then we are parsing language specific resources
    r->SetConfigurationUid( iThemeUid );
    r->SetLanguage( aLanguage );       
    r->SetFileNameL( aFilename );
    r->SetResourceType( aResourceType );
    if ( aMimetype.Length() )
        {                
        r->SetMimeTypeL( TDataType( aMimetype ) );
        }
    const TInt l = aTag.Length();
    if ( l )
        {
        HBufC* buf = HBufC::NewLC( l );
        buf->Des().Copy( aTag );
        r->SetTagsL( *buf );
        CleanupStack::PopAndDestroy( buf );
        }
    aArray.AppendL( r );
    
    CleanupStack::Pop( r );
    }
    
// -----------------------------------------------------------------------------
// ChspsInstallationHandler::CleanupL()
// Executes cleaning of the target folder prior to the installation process
// -----------------------------------------------------------------------------
//    
void ChspsInstallationHandler::CleanupL( const ChspsODT& aOdt )    
    {        
#ifdef HSPS_LOG_ACTIVE  
    if( iLogBus )
        {
        iLogBus->LogText( _L( "hspsInstallationHandler::CleanupL(): - removing previous installation" ) );
        }
#endif    
    
    // Get ODT's relative path in Plug-in Repository without the drive information             
    HBufC* relativePath = HBufC::NewLC( KMaxFileName );
    TPtr ptr = relativePath->Des();             
    iDefinitionRepository.GetODTPathL( aOdt, ptr );
    
    // Strip file name and append path with a drive symbol
    TParsePtr parser( ptr );
    parser.PopDir();               
    TPtrC confPath = parser.Path();                          
    TPath file;                        
    file.Format( _L("%S%S"), &KCDrive, &confPath );    
    CleanupStack::PopAndDestroy( relativePath );
            
    // If there was an existing configuration folder
    if( BaflUtils::FileExists( iFsSession, file ) )
        {
        // Remove the old configuration
        CFileMan* fileMan = CFileMan::NewL( iFsSession );
        CleanupStack::PushL( fileMan );
        User::LeaveIfError( fileMan->RmDir( file ) );
        CleanupStack::PopAndDestroy( fileMan );    
        }                                
    
    iThemeServer.UpdateHeaderListCache(
            EhspsCacheRemoveHeader,        
            aOdt.RootUid(),
            aOdt.ProviderUid(),
            aOdt.ThemeUid() );            
     }
    

// -----------------------------------------------------------------------------
// ChspsInstallationHandler::RollBackL()
// Rolls back the installation if it fails or is cancelled. Functionality depends
// on the state where the installation was.
// (other items were commented in a header).
// -----------------------------------------------------------------------------
//
void ChspsInstallationHandler::RollBackL(ChspsODT& aOdt)
    {
#ifdef HSPS_LOG_ACTIVE  
    if( iLogBus )
        {
        iLogBus->LogText( _L( "hspsInstallationHandler::RollBackL(): - rolling back.." ) );
        }
#endif    
    
    // then rolling back the installation in according to in which state it was before cancel
    switch ( iInstallationPhase )
        {
        case EhspsPhaseIdle:                
        case EhspsPhaseInitialise:
        case EhspsPhaseCleanup:
            break;        
        case EhspsPhaseInstallSkeleton:        
            {
            iDefinitionRepository.RemoveThemeL( aOdt );
            iInstallationPhase = EhspsPhaseIdle; // roll-back is completed
            }
            break;
        default:
            break;
        }
    }


// -----------------------------------------------------------------------------
// ChspsInstallationHandler::CompleteRequestL()
// Completes client request
// (other items were commented in a header).
// -----------------------------------------------------------------------------
//    
void ChspsInstallationHandler::CompleteRequestL(const ThspsServiceCompletedMessage aReturnMessage, 
    const TDesC8& aHeaderData )
    {
    TBool wasLocked( EFalse );
        
    if( iDefinitionRepository.Locked() )
        {
        CleanupStack::PushL( TCleanupItem( UnlockRepository, &iDefinitionRepository ) );
        wasLocked = ETrue;
        }
        
    // only EhspsInstallThemeSuccess and EhspsInstallPhaseSuccess quaranties valid header to be returned
    TBool complete( ETrue );
    
    switch( iInstallationMode )
        {
        case EServiceHandler:
            {
            if( aReturnMessage == EhspsInstallThemeSuccess  )
                {
                // Activate the installed application configuration if EhspsThemeStatusMakeActive is set                
                if ( iThemeStatus & EhspsThemeStatusMakeActive )
                    {
                    if ( iConfigurationType == EhspsAppConfiguration )
                        {
                        ActivateThemeL();
                        }
                    }
                    
                // lets try to delete temp files. Server will delete them only 
                // if there are not current resource holders  
                iInstallationPhase = EhspsPhaseIdle;                           
                // write the result
                iResult->iIntValue1 = 0;
                iResult->iIntValue2 = 0;
                
                iDefinitionRepository.Unlock();
                
                if ( !iDisableNotifications )
                    {
                    // Inform server that a configuration was added or replaced and that the 
                    // header cache should be updated (specific header to be added can be obtained
                    // from the uids)
                    
                    if( iInstallationType == EInstallationTypeNew )
                        {
                        ThspsRepositoryInfo info( 
                            ThspsRepositoryEvent( EhspsODTAdded ),
                            0,
                            0,
                            0, //=Any file
                            0,
                            iOdt->RootUid(),
                            iOdt->ProviderUid(),
                            iOdt->ThemeUid(),0,ETrue,
                            iOdt->ThemeFullName(),
                            (TLanguage)( iOdt->OdtLanguage() )
                            );
                        
                        iDefinitionRepository.RegisterNotification( info );
                        }
                    else if( iInstallationType == EInstallationTypeUpdate )
                        {                        
                        NotifyOdtUpdatedL();                                                
                        }
                    }
                
#ifdef HSPS_LOG_ACTIVE  
                if( iLogBus )
                    {
                    iLogBus->LogText( _L( "ChspsInstallationHandler::CompleteRequestL(): - configuration %d was successfully installed (EServiceHandler)" ),
                            iOdt->ThemeUid() );
                    }
#endif    
                }
            else if ( aReturnMessage == EhspsInstallPhaseSuccess  )
                {
                iResult->iIntValue1 = iInstallationPhase;
                }
            else if ( aReturnMessage == EhspsServiceRequestSheduled )
                {
                complete = EFalse;
                }
            else if (aReturnMessage == EhspsServiceRequestCanceled)
                {
                iDefinitionRepository.Unlock();                 
                }
            else
                {
                // installation failed, resetting
                // system error and xuikon error are writen in iResult when error was encountered
                iInstallationPhase = EhspsPhaseIdle;
                
                if( !iMessagePtr.IsNull() )
                    {
                    iMessagePtr.WriteL( 2, KNullDesC8, 0 );
                    }
                    
                RollBackL ( *iOdt );
                iDefinitionRepository.Unlock();    
                }
               
               if( complete && !iMessagePtr.IsNull() )
                {
                RDesWriteStream writeBuf( iResultData );
                iResult->ExternalizeL( writeBuf );
                writeBuf.Close();
                iMessagePtr.WriteL( 0, iResultData, 0 );
                iMessagePtr.WriteL( 2, aHeaderData, 0 ); 
                iMessagePtr.Complete( aReturnMessage );    
                } 
            }
            break;
                
        case EAsynchronousObject:
            {
            if( aReturnMessage == EhspsInstallThemeSuccess  )
                {
                
                // Activate the installed application configuration if EhspsThemeStatusMakeActive is set
                if( iThemeStatus & EhspsThemeStatusMakeActive )
                    {
                    if ( iConfigurationType == EhspsAppConfiguration )
                        {
                        ActivateThemeL();
                        }
                    }
                                                           
                // lets try to delete temp files. Server will delete them only if there are not
                // current resource holders  
                iInstallationPhase = EhspsPhaseIdle;
                iDefinitionRepository.Unlock();    
                
                if ( !iDisableNotifications )
                    {
                    if( iInstallationType == EInstallationTypeNew )
                        {
                        // Inform server there are files to be cleaned and cache has been updated
                        ThspsRepositoryInfo info( ThspsRepositoryEvent( EhspsODTAdded ),
                                    0,
                                    0,
                                    0, //=Any file
                                    0,
                                    iOdt->RootUid(),
                                    iOdt->ProviderUid(),
                                    iOdt->ThemeUid(),0,ETrue,
                                    iOdt->ThemeFullName(),
                                    (TLanguage)( iOdt->OdtLanguage() ) );
                                    
                        iDefinitionRepository.RegisterNotification( info );
                        }
                    else if( iInstallationType == EInstallationTypeUpdate )
                        {                        
                        NotifyOdtUpdatedL();                                                
                        }                        
                    }

#ifdef HSPS_LOG_ACTIVE  
                if( iLogBus )
                    {
                    iLogBus->LogText( _L( "ChspsInstallationHandler::CompleteRequestL(): - configuration %d was successfully installed (EAsynchronousObject)" ),
                            iOdt->ThemeUid() );
                    }
#endif
                }
            else if( aReturnMessage == EhspsInstallPhaseSuccess  )
                {
                iResult->iIntValue1 = iInstallationPhase;
                }
            else if( aReturnMessage == EhspsServiceRequestSheduled )
                {
                complete = EFalse;
                }
            else
                {
                // installation failed, resetting
                // system error and xuikon error are written in iResult when error was encountered
                iInstallationPhase = EhspsPhaseIdle;
                RollBackL ( *iOdt );
                iDefinitionRepository.Unlock();    
                }
       
            if ( complete )
                {                                
                User::RequestComplete( iRequestStatus, (TInt)aReturnMessage );                 
                }
            }
            break;        
        default:
            {
            if( wasLocked )
                {
                CleanupStack::Pop( &iDefinitionRepository );
                }
                
               User::Leave( KErrCancel );
            }
            break;
        }
        
    if( wasLocked )
        {
        CleanupStack::Pop( &iDefinitionRepository );
        }    
    }

// -----------------------------------------------------------------------------
// ChspsInstallationHandler::IsPluginUsedInAppConfsL
// -----------------------------------------------------------------------------
//
TBool ChspsInstallationHandler::IsPluginUsedInAppConfsL()
    {
    TBool isUsed = EFalse;
    
    const TInt KArrayGranularity = 3;
    CArrayPtrSeg<ChspsODT>* activeAppConfs =  new ( ELeave ) CArrayPtrSeg<ChspsODT>( KArrayGranularity );
    CleanupStack::PushL( activeAppConfs );

    // Retrieve active application configurations. These are not owned!
    GetActiveAppConfsL( *activeAppConfs );
                            
    // Loop found headers
    for( TInt i = 0; i < activeAppConfs->Count(); i++ )
        {
        ChspsODT* const header = (*activeAppConfs)[i];       
        if( header )
            {                                        
            // Clone header to avoid modification of original
            ChspsODT* odt = header->CloneL();
            CleanupStack::PushL( odt );
            
            // Get full ODT instance with DOM data       
            User::LeaveIfError( iDefinitionRepository.GetOdtL( *odt ) );
            
            // Get all plugin instances with the configuration UID that was just installed
            RArray<TInt> pluginIds;
            CleanupClosePushL( pluginIds );                            
            hspsServerUtil::GetPluginIdsByUidL( 
                    *odt,
                    TUid::Uid( iThemeUid ),
                    pluginIds );            
            isUsed = ( pluginIds.Count() > 0 );                                   
            CleanupStack::PopAndDestroy(); // pluginIds
            
            CleanupStack::PopAndDestroy( odt );
            
            if ( isUsed )                
                {                               
                break;
                }                             
            }
        }
    
     
    
    CleanupStack::PopAndDestroy( activeAppConfs );
    
    return isUsed;
    }

// -----------------------------------------------------------------------------
// ChspsInstallationHandler::NotifyOdtUpdatedL
// -----------------------------------------------------------------------------
//
void ChspsInstallationHandler::NotifyOdtUpdatedL()
    {    
    // Nothing to do if notifications disabled.
    if( iDisableNotifications )
        {        
        return;
        }    

    // This method should only be called when type is update.
    if( iInstallationType != EInstallationTypeUpdate )
        {
#ifdef HSPS_LOG_ACTIVE  
        if( iLogBus )
            {
            iLogBus->LogText( _L( "ChspsInstallationHandler::NotifyOdtUpdatedL(): Error: Installation type is not update." ) );
            }
#endif
        
        return;
        }    
    
    // Sanity check.
    if( !iOdt )
        {
#ifdef HSPS_LOG_ACTIVE  
        if( iLogBus )
            {
            iLogBus->LogText( _L( "ChspsInstallationHandler::NotifyOdtUpdatedL(): Error: iOdt == NULL." ) );
            }
#endif
        
        return;
        }
                  
    const TInt KArrayGranularity = 2;
    CArrayPtrSeg<ChspsODT>* activeAppConfs =  new ( ELeave ) CArrayPtrSeg<ChspsODT>( KArrayGranularity );
    CleanupStack::PushL( activeAppConfs );

    // Retrieve active application configurations. These are not owned!
    GetActiveAppConfsL( *activeAppConfs );

    // Notification list.
    RArray<ThspsRepositoryInfo> notifications;
    CleanupClosePushL( notifications );    
    
    // Construct notifications for every changed plugin id
    // in every active application configuration.
    for( TInt i = 0; i < activeAppConfs->Count(); i++ )
        {
        ChspsODT* const activeODT = (*activeAppConfs)[i];
        
        // Sanity check.
        if( !activeODT )
            {
#ifdef HSPS_LOG_ACTIVE  
            if( iLogBus )
                {
                iLogBus->LogText( _L( "ChspsInstallationHandler::NotifyOdtUpdatedL(): Error: active ODT list contains NULL." ) );
                }
#endif
        
            continue;
            }
        
        // Clone activeOdt to avoid modification of original.
        ChspsODT* fullODT = activeODT->CloneL();
        CleanupStack::PushL( fullODT );

        User::LeaveIfError( iDefinitionRepository.GetOdtL( *fullODT ) );
        
        RArray<TInt> pluginIds;
        CleanupClosePushL( pluginIds );

        // Get plugin ids.
        hspsServerUtil::GetPluginIdsByUidL( *fullODT,
                                            TUid::Uid( iOdt->ThemeUid() ),
                                            pluginIds );
        
        // Construct notifications. One for each plugin instance
        // that is affected by update.
        for( TInt j = 0; j < pluginIds.Count(); j++ )
            {
            ThspsRepositoryInfo info( 
                ThspsRepositoryEvent( EhspsODTUpdated ),
                fullODT->RootUid(),
                fullODT->ThemeUid(),
                0, //=Any file
                0,
                iOdt->RootUid(),
                iOdt->ProviderUid(),
                iOdt->ThemeUid(),
                pluginIds[j], 
                EFalse,
                KNullDesC(),
                (TLanguage)( fullODT->OdtLanguage() ) );
            
            notifications.Append( info );
            }        
        
        CleanupStack::PopAndDestroy(); // pluginIds.
        CleanupStack::PopAndDestroy( fullODT );
        fullODT = NULL;
        
        // Send applications notifications in one group.
        for( TInt k = 0; k < notifications.Count(); k++ )
            {
            // If last...
            if( k == notifications.Count() - 1 )
                {
                // ... Modify accordingly.
                notifications[ k ].iLastNotification = ETrue;
                }
            
            // Send.
            iDefinitionRepository.RegisterNotification( notifications[ k ] );        
            }
        
        // Clean notifications list.
        notifications.Reset();
        }
    
    CleanupStack::PopAndDestroy(); // notifications.    
    CleanupStack::PopAndDestroy( activeAppConfs );
    activeAppConfs = NULL;
    }                        

// -----------------------------------------------------------------------------
// ChspsInstallationHandler::GetActiveAppConfsL
// -----------------------------------------------------------------------------
//    
void ChspsInstallationHandler::GetActiveAppConfsL( CArrayPtrSeg<ChspsODT>& aActiveAppConfs )
    {
    const TInt count = iHeaderListCache.Count();
    for( TInt i = 0; i < count; i++ )
        {
        ChspsODT* header = iHeaderListCache.At( i );
        
        if( header && header->ConfigurationType() == EhspsAppConfiguration )
            {
            // Check if application configuration is active.            
            TInt tmp = 0;            
            if ( iCentralRepository.Get( header->RootUid(), tmp ) == KErrNone )
                {
                if( tmp == header->ThemeUid() )
                    {
                    // Active application configuration found.
                    aActiveAppConfs.AppendL( header );
                    } 
                } 
            }
        }
    }

// -----------------------------------------------------------------------------
// Saves ODT itself as an resource
// -----------------------------------------------------------------------------
//    
void ChspsInstallationHandler::SetODTAsResourceL( ChspsODT& aOdt )
    {
    TInt errorCode = KErrNone;
    
    ChspsResource* res = ChspsResource::NewL();
    CleanupStack::PushL( res );
    
    ThspsLockingPolicy lockingPolicy = EhspsUnlocked;
    if ( aOdt.Flags() & EhspsThemeStatusLicenceeDefault )
        {
        lockingPolicy = EhspsLocked;
        }
        
    // ODT-resource is handled as a cached resource                 
    res->SetLockingPolicy( lockingPolicy );
    res->SetResourceType( EResourceODT );    
    res->SetResourceIdL( aOdt.ThemeShortName() );
    //res->SetFileNameL( filepath ); will be set by the repository       
    res->SetMimeTypeL(TDataType( KUnknownMimeType ));
    res->SetConfigurationUid( aOdt.ThemeUid() );
    
    // Creates a path for given ODT
    iDefinitionRepository.MakeODTPathL( aOdt, *res );
        
    aOdt.AddResourceL( res );
    
    CleanupStack::Pop(res);   
            
    User::LeaveIfError( errorCode );
    }


// -----------------------------------------------------------------------------
// ChspsInstallationHandler::ActivateThemeL()
// 
// (other items were commented in a header).
// -----------------------------------------------------------------------------
//
void ChspsInstallationHandler::ActivateThemeL()
    {
    const TUint32 fullMask = 0xFFFFFFFF;
    RArray<TUint32> res;
    CleanupClosePushL( res );
    
    TInt errorCode = KErrNone;
    iCentralRepository.FindL( iOdt->RootUid(), fullMask, res );
       if ( res.Count() == 0 )
        { 
        errorCode = iCentralRepository.Create( iOdt->RootUid(), iOdt->ThemeUid() );
        }
    else
        {
        errorCode = iCentralRepository.Set( iOdt->RootUid(), iOdt->ThemeUid() );
        }
    if ( !errorCode )
        {
        TUint flags = iOdt->Flags();
        iOdt->SetFlags( flags | EhspsThemeStatusActive );
        
        // If not processing ROM installations
        if ( !iDisableNotifications )
            {
            // inform for cache update to the repository so that everyone will know 
            // about the change
            ThspsRepositoryInfo info( EhspsODTActivated );
            iDefinitionRepository.RegisterNotification( info );
            }
        }
    res.Close();    
    CleanupStack::PopAndDestroy(); //res
    }
        
// -----------------------------------------------------------------------------
// ChspsInstallationHandler::OnStartElementL
// Verifies that the installer is able to parse the manifest file, 
// parses language specifications and resources 
// -----------------------------------------------------------------------------
//
 void ChspsInstallationHandler::OnStartElementL(
        const RTagInfo& aElement, 
        const RAttributeArray& aAttributes, 
        TInt /*aErrorCode*/)
    {
    TPtrC8 localName = aElement.LocalName().DesC();

    if( iContent )
        {
        delete iContent;
        iContent = NULL;
        }
    
    if ( localName == KPackage )
        {
        // Check whether the version attribute is specified
        TInt argCount = aAttributes.Count();
        if( argCount )
            {
            for( TInt argIndex=0; argIndex < argCount; argIndex++ )
                {                            
                const TDesC8& attName = aAttributes[argIndex].Attribute().LocalName().DesC();
                if ( attName.Compare(KPackageVersion) == 0 )
                    {
                    delete iPackageVersion;
                    iPackageVersion = NULL;
                    iPackageVersion = HBufC::NewL( KMaxFileName );
                    iPackageVersion->Des().Copy( aAttributes[argIndex].Value().DesC() );
                    // Is manifest supported by this parser?
                    iPackageVerSupported = EFalse;
                    if ( iPackageVersion->Des().Compare( KhspsSupportedManifestVersion ) == 0 )
                        {
                        iPackageVerSupported = ETrue;
                        }
                    break;
                    }
                }
            }
        }
    
    else if ( localName == KLocalized )
        {
        // From now on, we are parsing filenames of localized resources
        iLocalized = ETrue;
        }
         
    else if ( localName == KFileResource )
        {                    
        if ( iMediaType )
            {
            delete iMediaType;
            iMediaType = NULL;
            }
        
        // Get attributes of the resource element and store them for adding in ::OnEndElementL()   
        TInt attrCount( aAttributes.Count() );
        for( TInt i=0; i<attrCount; i++ )
            {
            const TDesC8& attValueDes8 = aAttributes[i].Value().DesC();                
            
            // If mediatype attribute was found
            if ( aAttributes[i].Attribute().LocalName().DesC() == KMediaType )
                {                                                
                TPtrC8 orginalPtr( aAttributes[i].Value().DesC() );
                TInt length = orginalPtr.Length();
                
                // Length limit
                if ( length > KMaxMediaTypeLength )
                    {
                    length = KMaxMediaTypeLength;
                    
#ifdef HSPS_LOG_ACTIVE  
                    if( iLogBus )
                        {
                        iLogBus->LogText( _L( "ChspsInstallationHandler::OnStartElementL(): - mediatype was too long!" ) );
                        }
#endif    
                    }                                                        
                    
                iMediaType = HBufC8::NewL( length );
                iMediaType->Des().Copy( orginalPtr.Left( length ) );
                }
            else if ( aAttributes[i].Attribute().LocalName().DesC() == KTag )
                {                
                delete iResourceTag;
                iResourceTag = NULL;
                if ( attValueDes8.Length() )
                    {
                    iResourceTag = attValueDes8.Left( KMaxTagsLength ).AllocL();                    
                    }
                }
            
            } // for
                
        }
    
    if ( !iMultiInstanceFound )
        {
        iMultiInstance = KMultiInstanceDefaultValue;
        }
    }
  
// -----------------------------------------------------------------------------
// Parsing of the manifest elements.
// -----------------------------------------------------------------------------
//
void ChspsInstallationHandler::OnEndElementL( const RTagInfo& aElement, TInt /*aErrorCode*/ )
    {
    TPtrC8 localName = aElement.LocalName().DesC();
    
    if ( localName == KFamily )
        {
        const TPtrC8 familyPtr( iContent->Des() );        
        iFamilyMask |= ChspsFamily::GetFamilyType( familyPtr );
        }    
    else if ( localName == KConfigurationType )
        {
        // Get configuration type from the manifest
        if( *iContent == KManifestTypeApp )
            {
            iConfigurationType = EhspsAppConfiguration;
            }
        else if ( *iContent == KManifestTypeView )
            {
            iConfigurationType = EhspsViewConfiguration;
            }
        else if ( *iContent == KManifestTypeWidget )
            {
            iConfigurationType = EhspsWidgetConfiguration;
            }
         else if( *iContent == KManifestTypeTemplate )
            {            
            iConfigurationType = EhspsTemplateConfiguration;            
            }        
         else
             {
             User::Leave( KErrNotSupported );
             }
        }
    else if ( localName == KInterfaceUid )
        {
        if( iContent )
            {
            // Delete extra whitespaces.
            iContent->Des().TrimAll();
            
            // Convert to Uint.
            User::LeaveIfError( hspsServerUtil::HexString2Uint( *iContent, iRootUid ) );
            ApplyUidRangeTestsL( iRootUid );
            }
        }
    else if ( localName == KProviderUid )
        {
        if( iContent )
            {
            // Delete extra whitespaces.
            iContent->Des().TrimAll();

            // Convert to Uint.
            User::LeaveIfError( hspsServerUtil::HexString2Uint( *iContent, iProviderUid ) );                        
            ApplyUidRangeTestsL( iProviderUid );
            }
        }
    else if ( localName == KThemeUid )
        {
        if( iContent )
            {        
            // Delete extra whitespaces.
            iContent->Des().TrimAll();

            // Convert to Uint.
            User::LeaveIfError( hspsServerUtil::HexString2Uint( *iContent, iThemeUid ) );            
            ApplyUidRangeTestsL( iThemeUid );
            }
        }
    else if ( localName == KThemeStatus )
        {
        if( *iContent == KStatusNone )
            {
            iThemeStatus = iThemeStatus | EhspsThemeStatusNone;
            }
         else if( *iContent == KStatusLicenceeDefault )
            {
            iThemeStatus = iThemeStatus | EhspsThemeStatusLicenceeDefault;
            }
         else if( *iContent == KStatusLicenceeRestorable )
            {
            iThemeStatus = iThemeStatus | EhspsThemeStatusLicenceeRestorable;
            iThemeStatus = iThemeStatus | EhspsThemeStatusLicenceeDefault;
            }
         else if( *iContent == KStatusOperatorDefault )
            {
            iThemeStatus = iThemeStatus | EhspsThemeStatusOperatorDefault;
            }
         else if( *iContent == KStatusUserDefault )
            {
            iThemeStatus = iThemeStatus | EhspsThemeStatusUserDefault;
            }
        else if( *iContent == KStatusMakeActive )
            {
            iThemeStatus = iThemeStatus | EhspsThemeStatusMakeActive;
            }
        else if( *iContent == KStatusLocked )
            {
            iThemeStatus = iThemeStatus | EhspsThemeStatusLocked;
            }
        }
    else if ( localName == KThemeFullName )
        {
        // Store value of the parsed "fullname" element
        if ( iThemeFullName )
            {
            delete iThemeFullName;
            iThemeFullName = NULL;
            }
        iThemeFullName = HBufC::NewL( KMaxFileName );
        TInt contentLength = iContent->Des().Length(); 
        if ( contentLength > KMaxFileName )
            {
            contentLength = KMaxFileName;
            }
        TPtr ptr( iThemeFullName->Des() );
        ptr.Copy( iContent->Des().Left(contentLength) );                                           
        }
    else if ( localName == KThemeShortName )
        {
        delete iThemeShortName;
        iThemeShortName = NULL;
        iThemeShortName = HBufC8::NewL( KMaxFileName );
        TPtr8 themeShortNameDes( iThemeShortName->Des() );
        themeShortNameDes = *iContent;
        }
    else if ( localName == KThemeVersion )
        {
        delete iThemeVersion;
        iThemeVersion = NULL;
        iThemeVersion = HBufC8::NewL( KMaxFileName );
        TPtr8 themeVersionDes( iThemeVersion->Des() );
        themeVersionDes = *iContent;               
        }
    else if ( localName == KThemeDesc )
        {        
        if ( iContent )
            {
            delete iThemeDesc;
            iThemeDesc = NULL;
            iThemeDesc = HBufC8::NewL( KMaxDescLength );        
            TPtr8 descPtr8( iThemeDesc->Des() );
            descPtr8.Copy( (*iContent).Left( KMaxDescLength ) );        
            }
        }
    else if ( localName == KFileLogo )
        {
        if ( iContent )
            {            
            // Get possible file references and add them to the  
            // resource array with a logo tag
            HBufC* result = NULL;
            ParseIconDeclarationL( 
                    *iContent, 
                    KObjectAttrTagLogo,
                    result );
            if ( result )
                {
                // Store logo declaration
                CleanupStack::PushL( result );
                iOdt->SetLogoFileL( *result );
                CleanupStack::PopAndDestroy();
                }
            }
        }
    else if ( localName == KFilePreview )
        {
        if ( iContent )
            {            
            // Get possible file references and add them to the  
            // resource array with a preview tag
            HBufC* result = NULL;
            ParseIconDeclarationL( 
                    *iContent, 
                    KObjectAttrTagPreview,
                    result
                    );            
            if ( result )
                {
                // Store preview declaration
                CleanupStack::PushL( result );
                iOdt->SetPreviewFileL( *result );
                CleanupStack::PopAndDestroy();
                }            
            }
        }
    else if ( localName == KFileXML )
        {
        if( iContent )
            {
            // Convert from 8 to 16bit string
            HBufC* nameBuf = CnvUtfConverter::ConvertToUnicodeFromUtf8L( *iContent );                        
            // Find full path to the file 
            TFileName fullName;
            hspsServerUtil::FindFile(
                    iFsSession,                    
                    iThemeFilePath, 
                    nameBuf->Des(),
                    iInstallFromUDAEmmc,
                    fullName );
            delete nameBuf;
            nameBuf = NULL;
            if( !fullName.Length() )
                {
                iFileNotFound = ETrue;
                iResult->iXuikonError = KErrXmlFileNotFound;
                User::Leave( KErrNotFound );
                }
            delete iXmlFile;
            iXmlFile = NULL;
            iXmlFile = fullName.AllocL();
            }
        }
    else if ( localName == KFileDTD )
        {
        // Parse name of the DTD files
        if ( !iContent || iContent->Length() < 1 )
            {
            User::Leave( KErrArgument );
            }
        
        if ( iDtdFile )
            {
            delete iDtdFile;
            iDtdFile = 0;
            }                
        iDtdFile = HBufC::NewL( iContent->Length() );
        iDtdFile->Des().Copy( iContent->Des() );                       
        }    
    else if ( localName == KFileResource )
        {        
        // Following attributes are parsed in OnStartElement callback
        TPtrC8 mediaPtr;
        if ( iMediaType )
            {
            mediaPtr.Set( iMediaType->Des() );
            }
        TPtrC8 tagsPtr;
        if ( iResourceTag )
            {
            tagsPtr.Set( iResourceTag->Des() );
            }
                    
        // Parse name of the resource file and make 8bit to 16bit conversion     
        HBufC* fileName = CnvUtfConverter::ConvertToUnicodeFromUtf8L( *iContent );
        if ( !fileName || fileName->Des().Length() < 1 )
            {
            User::Leave( KErrArgument );
            }
        CleanupStack::PushL( fileName );
        
        // If parsing localized resources
        if ( iLocalized )
            {                                        
            // Add locale specific resources into temp array                        
            AddResourceL(
                *iTempLocalizedResourceList,
                *fileName,
                ELangNone,        
                EResourceOther,
                mediaPtr,
                tagsPtr );    
            }
        else
            {
            // If Xuikon resource
            TFileName interfacePath( GetInterfacePath() );
            HBufC* resourceFile = NULL;
            if( interfacePath.Length() )
                {
                _LIT(KSubFolder, "00\\");
                resourceFile = HBufC::NewLC( interfacePath.Length() + KSubFolder().Length() + fileName->Length() );
                resourceFile->Des().Copy( interfacePath );
                resourceFile->Des().Append( KSubFolder );
                resourceFile->Des().Append( *fileName );                
                }
            else
                {
                resourceFile = HBufC::NewLC( iThemeFilePath.Length() + fileName->Length() );
                resourceFile->Des().Copy( iThemeFilePath );
                resourceFile->Des().Append( *fileName );
                }
                                    
            // Validate the file
            if( !BaflUtils::FileExists( iFsSession, *resourceFile ) )
                {
#ifdef HSPS_LOG_ACTIVE  
                TBuf8<KMaxFileName> name8;
                name8.Copy( *resourceFile );
                if( iLogBus )
                    {
                    iLogBus->LogText( _L8( "ChspsInstallationHandler::OnEndElementL(): - resource file does not exist '%S'" ),
                            &name8 );
                    }
#endif            
                iFileNotFound = ETrue;
                iResult->iXuikonError = KErrResourceFileNotFound;
                }
            
            // Add common resources
            AddResourceL(
                *iResourceList,
                *resourceFile,
                ELangNone,
                EResourceOther,
                mediaPtr,
                tagsPtr );
            
            CleanupStack::PopAndDestroy( resourceFile );         
            }
        
        CleanupStack::PopAndDestroy( fileName );
        
        if ( iMediaType )
            {
            delete iMediaType;
            iMediaType = NULL;
            }
        if ( iResourceTag )
            {
            delete iResourceTag;
            iResourceTag = NULL;
            }
        }
    else if ( localName == KMultiInstance)
        {
        iMultiInstanceFound = ETrue;
        if( iContent )
            {
            // Delete extra whitespaces.
            iContent->Des().TrimAll();
            
            // Convert to int
            TLex8 lex( iContent->Des() );
            TInt err = lex.Val( iMultiInstance );
            // iContent is not a number - check for strings
            if ( err != KErrNone )
                {
                if ( iContent->CompareF( KMultiInstanceUnlimited ) 
                    == KErrNone )
                    {
                    iMultiInstance = KMultiInstanceUnlimitedValue;
                    }
                else if ( iContent->CompareF( KMultiInstanceHidden ) == 
                    KErrNone  )
                    {
                    iMultiInstance = KMultiInstanceHiddenValue;
                    }
                else 
                    {
                    User::Leave( KErrArgument );
                    }
                }
            // iContent is a number - check whether it is valid
            else
                {
                if ( ( iMultiInstance < KMultiInstanceMinimumCountValue ) || 
                    ( iMultiInstance > KMultiInstanceMaximumCountValue ) )
                    {
                    User::Leave( KErrArgument );                    
                    }
                }
            }
        }
    }

// -----------------------------------------------------------------------------
// The needed memory for each element information is reserved here.
// -----------------------------------------------------------------------------
//
void ChspsInstallationHandler::OnContentL(const TDesC8& aBytes, TInt aErrorCode)
    {
    if ( aErrorCode == KErrNone )
        {
        if ( !iContent )
            {
            iContent = HBufC8::NewL( aBytes.Size() );
            *iContent = aBytes;
            }
        else
            {
            iContent = iContent->ReAllocL( iContent->Size() + aBytes.Size() );
            TPtr8 c( iContent->Des() );
            c.Append( aBytes );
            }
        }
   
    }

// -----------------------------------------------------------------------------
// Disables "configuration was installed" notifications
// -----------------------------------------------------------------------------
//
void ChspsInstallationHandler::DisableNotifications()    
    {
    iDisableNotifications = ETrue;
    }

// -----------------------------------------------------------------------------
// Disables eclipsing from UDA and eMMC drives
// -----------------------------------------------------------------------------
//
void ChspsInstallationHandler::DisableUdaEmmcInstallations()
    {
    iInstallFromUDAEmmc = EFalse;
    }

// -----------------------------------------------------------------------------
// ChspsInstallationHandler::AddInterfaceResourcesV2L
// -----------------------------------------------------------------------------
//
void ChspsInstallationHandler::AddInterfaceResourcesV2L(
        const TDesC& aPath )
    {
    // Retrieve supported localizations.
    CArrayFixFlat<TInt>* systemEpocLanguageCodes = NULL;
    hspsServerUtil::GetInstalledLanguagesL(
            systemEpocLanguageCodes );
    CleanupStack::PushL( systemEpocLanguageCodes );
                            
    // Find all entries under the Xuikon folders that
    // match our eclipsing rules and localisations supported by phone.
    RArray<TInt> driveArray;
    CleanupClosePushL( driveArray );   
    TInt drive = hspsServerUtil::GetEmmcDrivePath( iFsSession );
    if ( drive != KErrNotFound )
        {
        driveArray.Append( drive );
        }
    
    driveArray.Append( EDriveC );
    driveArray.Append( EDriveZ );     
    
    FindResourceFilesL( aPath, ETrue, driveArray, systemEpocLanguageCodes );
   
    CleanupStack::PopAndDestroy(); // driveArray
    CleanupStack::PopAndDestroy( systemEpocLanguageCodes );    
    }    
    
// -----------------------------------------------------------------------------
// Finds locale specific subdirectories and resources and appends those
// into the resource array 
// Should be executed prior to the CheckHeader method!
// -----------------------------------------------------------------------------
//
void ChspsInstallationHandler::AddLocalesL(
        const TDesC& aPath,
        const TBool aProcessOnlyDTD )
    {
    // Retrieve phone supported language.
    CArrayFixFlat<TInt>* languageCodes = NULL;
    hspsServerUtil::GetInstalledLanguagesL( languageCodes );
    CleanupStack::PushL( languageCodes );     
    
    // Ensure that path contains '\' at the end.
    TFileName pathBase;
    pathBase.Copy( aPath );
    if( pathBase.Length() > KPathDelim().Length() )
        {
        if( pathBase.Mid( pathBase.Length() -
                          KPathDelim().Length()
                         ).Compare( KPathDelim ) != 0 )
            {
            pathBase.Append( KPathDelim );
            }
        }
            
    TParsePtrC driveParser( pathBase );
    TInt driveEnum = KErrNotFound;
    
    if( !aProcessOnlyDTD )
        {
        if( driveParser.DrivePresent() && driveParser.Drive().Length() > 0 )
            {
            User::LeaveIfError(
                    RFs::CharToDrive( ( driveParser.Drive() )[0],
                            driveEnum ) );
            }
        }
    
    _LIT( KFormatNN, "%02d" );    
    TFileName localePath;
    
    for( TInt i = 0; i < languageCodes->Count(); i++ )
        {                       
        const TInt languageCode = languageCodes->At( i );

        // Construct locale path using two digit minium
        // width and zero as padding.
        
        localePath.Copy( pathBase );
        if( languageCode < 10 )
            {
            localePath.AppendFormat( KFormatNN, languageCode );
            }
        else
            {
            localePath.AppendNum( languageCode );
            }
        localePath.Append( KPathDelim );

        // Check if folder for supported language exists.        
        TBool exists = EFalse;
        
        if( BaflUtils::FolderExists( iFsSession, localePath ) )
            {
            exists = ETrue;
            }
        
        // Support also one digit type folder naming possibly used by the imports.
        if( !exists && languageCode < 10 )
            {
            localePath.Copy( pathBase );
            localePath.AppendNum( languageCode );        
            localePath.Append( KPathDelim );
            
            if( BaflUtils::FolderExists( iFsSession, localePath ) )
                {
                exists = ETrue;
                }            
            }

        if( exists )
            {
            // If we found the first language specification          
            if ( !iDefaultSpecificationSet )
                {
                // Assume this is the default language shown incase 
                // there is no locale for the active UI language
                iDefaultSpecification = (TLanguage)languageCode;
                iDefaultSpecificationSet = ETrue;
                }                                                            
                
            // Add dtd file if existing.
            AddDtdFileL( localePath, (TLanguage)languageCode );
            
            if( !aProcessOnlyDTD )
                {
                // Find localized files from the provided directory
                RArray<TInt> driveArray;
                CleanupClosePushL( driveArray );                
                driveArray.Append( driveEnum );
                
                FindResourceFilesL( localePath,
                                    EFalse,
                                    driveArray,
                                    NULL );
                
                CleanupStack::PopAndDestroy(); // driveArray
                }
            }        
        }
    
    CleanupStack::PopAndDestroy( languageCodes );
    languageCodes = NULL;
    }

// -----------------------------------------------------------------------------
// Adds localized dtd resources from the provided subdirectory
// -----------------------------------------------------------------------------
//
void ChspsInstallationHandler::AddDtdFileL(
        const TDesC& aPath,
        const TLanguage aLanguage )
    {    
    // If FileDTD was declared
    if ( iDtdFile && iDtdFile->Des().Length() )
        {    
        // Append path with the default name of DTD files
        const TInt len = aPath.Length() + iDtdFile->Des().Length();
        HBufC* dtdPath = HBufC::NewLC( len );
        dtdPath->Des().Copy( aPath );    
        dtdPath->Des().Append( *iDtdFile );
            
        // Check whether the file exists
        if( !BaflUtils::FileExists( iFsSession, *dtdPath ) )
            {
#ifdef HSPS_LOG_ACTIVE  
            if( iLogBus )
                {
                iLogBus->LogText( _L( "ChspsInstallationHandler::AddLocalizedResourcesL(): - DTD file was not found '%S'" ),
                        &dtdPath );
                }
#endif            
            iFileNotFound = ETrue;
            iResult->iXuikonError = KErrDtdFileNotFound;
            User::Leave( KErrNotFound );
            }           
       
        // Store locale specific DTD files into the resource array
        TPtrC8 mediaType;
        TPtrC8 tagsPtr;        
        AddResourceL(
            *iResourceList,
            *dtdPath,
            aLanguage,
            EResourceDTD,
            mediaType,
            tagsPtr );
        
        CleanupStack::PopAndDestroy( dtdPath );
        }          
    }

// -----------------------------------------------------------------------------
// ChspsInstallationHandler::ApplyUidRangeTestsL
// -----------------------------------------------------------------------------
//
void ChspsInstallationHandler::ApplyUidRangeTestsL( const TUint aUid )
    {
    // Check for TUint to TInt conversion
    const TInt intValue( aUid );
    if ( intValue < 1 )
        {
        // Check UID ranges in the manifest file
#ifdef HSPS_LOG_ACTIVE  
        if( iLogBus )
            {
            iLogBus->LogText( _L( "ChspsInstallationHandler::ApplyUidRangeTestsL() - Invalid UID value '%d'" ), aUid );
            }
#endif         
        User::Leave( KErrArgument );
        }
    }


// -----------------------------------------------------------------------------
// ChspsInstallationHandler::ParseIconDeclarationL
// -----------------------------------------------------------------------------
//
void ChspsInstallationHandler::ParseIconDeclarationL(
        HBufC8& aValue8,
        const TDesC8& aTag,
        HBufC*& aResultString )
    {                
    // 8bit > 16bit conversion
    TInt size = KMaxFileName - iThemeFilePath.Length();
    aResultString = HBufC::NewLC( KMaxFileName );
    TPtr resultPtr( aResultString->Des() );    
    resultPtr.Copy( aValue8.Des().Left( size ) );       
    
    // check whether skin/mif/uid declarations were used
    TFileName filename;    
    if ( hspsServerUtil::IsLogoFile( resultPtr, filename ) )
        {                                                                              
        // check whether the file reference is valid
        TPath fullname;
        fullname.Copy( iThemeFilePath );
        fullname.Append( filename );      
        
        if( !BaflUtils::FileExists( iFsSession, fullname ) )
            {
#ifdef HSPS_LOG_ACTIVE  
            if( iLogBus )
                {
                iLogBus->LogText( 
                        _L( "ChspsInstallationHandler::ParseIconDeclarationL(): - '%S' was not found " ), 
                        &fullname );
                }
#endif               
//            User::Leave( KErrNotFound );
            }
        else
            {                                           
            // Store logo as a common resource file
            TPtrC8 mediaType;                     
            AddResourceL(
                    *iResourceList,
                    fullname,
                    ELangNone,
                    EResourceOther,
                    mediaType,
                    aTag );
            
            // get offset of the filename        
            TInt nameOffset = resultPtr.FindF( (TPtrC)filename );
            if ( nameOffset >= 0 )
              {        
              // get relative path under the HSPS 
              if ( iRootUid < 1 || iProviderUid < 1 || iThemeUid < 1 || !iThemeVersion || iThemeVersion->Des().Length() < 1 )
                  {
                  User::Leave( KErrArgument );
                  }  
              // 8bit > 16bit
              HBufC* verBuf = HBufC::NewLC( iThemeVersion->Des().Length() );
              verBuf->Des().Copy( *iThemeVersion );            
              _LIT(KPathFormat, "%D\\%D\\%D\\%S\\sources\\");                                 
              TFileName relativePath;                      
              relativePath.Format( 
                      KPathFormat, 
                      iRootUid,
                      iProviderUid,
                      iThemeUid,
                      verBuf );                                
              if ( resultPtr.Length() + relativePath.Length() > KMaxFileName - 1 )
                  {
                  User::Leave( KErrArgument );
                  }                
              resultPtr.Insert( nameOffset, relativePath );            
              CleanupStack::PopAndDestroy( verBuf );            
              }
            
            }       
        
        } // IsFile
       
    CleanupStack::Pop( aResultString );           
    }

// -----------------------------------------------------------------------------
// ChspsInstallationHandler::FindResourceFilesL
// -----------------------------------------------------------------------------
//
void ChspsInstallationHandler::FindResourceFilesL( const TDesC& aPath,
        const TBool aRecursive,
        RArray<TInt>& aDriveArray,
        CArrayFixFlat<TInt>* aDeviceLanguages )
    {
    RPointerArray<HBufC> filesArray;    
    CleanupResetAndDestroyPushL( filesArray );
    
    hspsServerUtil::FindResourcesL(
            iFsSession,
            aDriveArray,
            aPath,
            filesArray,
            aDeviceLanguages,
            aRecursive );    
    
    // Filter search results and select only relevant files
    TInt error = KErrNone;
    for( TInt i = 0; i < filesArray.Count(); i++ )
        {            
        const TFileName fileName = filesArray[ i ]->Des();
        
        // Check that file is listed in manifest resource.
        
        error = KErrNotFound;
        
        TParsePtrC fileNameParser( fileName );        
        const TFileName fixedName =
                hspsServerUtil::GetFixedOdtName( fileNameParser.NameAndExt() );
                
        ChspsResource* temporaryResource = NULL;
        
        for( TInt j = 0;
                j < iTempLocalizedResourceList->Count();
                j++ )
            {        
            temporaryResource = iTempLocalizedResourceList->At( j );
            if( fixedName.CompareF( temporaryResource->FileName() ) == 0  )
                {
                error = KErrNone;
                break;
                }
            }

        // Extract localisation code from path.
        
        TInt localizationCode = 0;
        
        if( error == KErrNone )
            {                        
            TPath path = fileNameParser.Path();                
            if( path.Right( KPathDelim().Length() ).Compare( KPathDelim() ) == 0 )
                {
                path.Delete( path.Length() - KPathDelim().Length(), KPathDelim().Length() );
                }
    
            TParsePtrC localizationParser( path );
            TLex lex( localizationParser.Name() );        
            error = lex.Val( localizationCode );
            
            if( error == KErrNone && localizationCode < ELangTest )
                {
                error = KErrCorrupt;
                }
            }
        
        // Check for duplicates.
        
        if( error == KErrNone )
            {
            for( TInt k = 0; k < iResourceList->Count(); k++ )
                {
                ChspsResource* resource = iResourceList->At( k );
                
                if( resource->Language() == localizationCode &&
                    resource->FileName().CompareF( fileName ) == 0 )
                    {
                    error = KErrAlreadyExists;
                    break;
                    }
                }
            }
        
        // Add to actual resource list. 
        if( error == KErrNone )
            {
            TPtrC8 tag;                                                               
            HBufC8* tagBuf8 = NULL;                        
            
            if ( temporaryResource->Tags().Length() )
                {
                tagBuf8 = HBufC8::NewLC( temporaryResource->Tags().Length() );
                tagBuf8->Des().Copy( temporaryResource->Tags() );
                tag.Set( tagBuf8->Des() );
                }
                        
            TDataType dataType( temporaryResource->MimeType() );            
            
            // Add localized files into the resource array                    
            AddResourceL(
                    *iResourceList,
                    fileName,
                    (TLanguage)localizationCode,
                    EResourceOther,
                    dataType.Des8(),
                    tag );
            
            if ( tagBuf8 )
                {
                CleanupStack::PopAndDestroy( tagBuf8 );
                }
            }
        }

    CleanupStack::PopAndDestroy(); // filesArray
    }

// end of file
