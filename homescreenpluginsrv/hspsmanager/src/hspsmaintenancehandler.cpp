/*
* Copyright (c) 2002-2008 Nokia Corporation and/or its subsidiary(-ies).
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
* Description:  Implementation of HSPS MhspsMaintenanceService interface defined
*                in hspsThemeManagement.h. For details, see header file.
*
*/


#include <centralrepository.h>

#include "hsps_builds_cfg.hrh"
#include "hspsthememanagement.h"
#include "hspsdefinitionrepository.h"
#include "hspsresource.h"
#include "hspsresult.h"
#include "hspspluginidlist.h"
#include "hspsodt.h"
#include "hspsmaintenancehandler.h"
#include "hspsthemeserver.h"
#include "hspsinstallationhandler.h"
#include "hspssecurityenforcer.h"
#include "hspsconfiguration.h"
#include "hspsdomdocument.h"
#include "hspsdomlist.h"
#include "hspsdomdepthiterator.h"
#include "hspsdomnode.h"
#include "hspsdomattribute.h"
#include "hspsmanifest.h"
#include "hspsserverutil.h"
#include "hspsthemeserversession.h"

#ifdef HSPS_LOG_ACTIVE
#include <hspsodtdump.h>
#include <hspslogbus.h>
#endif


_LIT(KSourcesFolder, "\\sources\\");
_LIT(KLocalesFolder, "\\locales\\");

const TInt KAdditionalRequiredDiskSpace = 256 * 1024; /// 256KB in bytes.

// ============================= LOCAL FUNCTIONS ===============================

// -----------------------------------------------------------------------------
// Callback function for removing repository lock if error occurs while repository is locked
// Returns: void
// -----------------------------------------------------------------------------
//
LOCAL_C void UnlockRepository( TAny* aObject )
    {
    ChspsDefinitionRepository* DefRep = reinterpret_cast<ChspsDefinitionRepository*>( aObject );
        
    if( DefRep->Locked() )
        {
        DefRep->Unlock();
        }
    }

// -----------------------------------------------------------------------------
// ResetAndDestroyArray Callback function for cleaning up the CArrayPtr.
// Returns: void
// -----------------------------------------------------------------------------
//
LOCAL_C void ResetAndDestroyArray( TAny* aArray )
    {
    reinterpret_cast<CArrayPtrSeg<HBufC>*>( aArray )->ResetAndDestroy();
    }
    

// ============================ MEMBER FUNCTIONS ===============================

// -----------------------------------------------------------------------------
// ChspsMaintenanceHandler::ChspsMaintenanceHandler
// C++ default constructor can NOT contain any code, that
// might leave.
// -----------------------------------------------------------------------------
//
ChspsMaintenanceHandler::ChspsMaintenanceHandler( 
        ChspsThemeServer& aThemeServer, 
        const TUint aSecureId ): 
    CTimer(EPriorityLow), 
    iLanguage ( aThemeServer.DeviceLanguage() ),
    iThemeServer( aThemeServer ), 
    iSecureId( aSecureId ),
    iCentralRepository( aThemeServer.CentralRepository() ),    
    iDefinitionRepository( aThemeServer.DefinitionRepository() ),
    iSecurityEnforcer( aThemeServer.SecurityEnforcer() ),
    iHeaderListCache( aThemeServer.HeaderListCache() ),
    iFileMan( NULL )
    { 
    iDeliveryCount = 0;
    iSubscription = EFalse;
    }
    
// -----------------------------------------------------------------------------
// ChspsMaintenanceService::NewL
// Two-phased constructor.
// -----------------------------------------------------------------------------
//
ChspsMaintenanceHandler* ChspsMaintenanceHandler::NewL( 
        ChspsThemeServer& aThemeServer,
        const TUint aSecureId )
    {
    ChspsMaintenanceHandler* h = ChspsMaintenanceHandler::NewLC( 
            aThemeServer, aSecureId );
    CleanupStack::Pop(h);
    return (h);
    }

// -----------------------------------------------------------------------------
// ChspsMaintenanceService::NewLC
// Two-phased constructor.
// -----------------------------------------------------------------------------
//
ChspsMaintenanceHandler* ChspsMaintenanceHandler::NewLC( 
        ChspsThemeServer& aThemeServer,
        const TUint aSecureId )
    {
    ChspsMaintenanceHandler* h = new (ELeave) ChspsMaintenanceHandler( 
            aThemeServer, aSecureId );
    CleanupStack::PushL(h);
    h->ConstructL();
    return (h);
    }

// Destructor
ChspsMaintenanceHandler::~ChspsMaintenanceHandler()
    {
    Cancel();
    if (iHeaderDataList)
        {
        iHeaderDataList->ResetAndDestroy();
        }
    delete iHeaderDataList;
    delete iSearchMask;
    delete iSetMask;
    delete iResult;
    delete iFileMan;
    iDefinitionRepository.UnregisterObserver( *this );
    }


// -----------------------------------------------------------------------------
// ChspsMaintenanceHandler::ConstructL
// Symbian 2nd phase constructor can leave.
// -----------------------------------------------------------------------------
//
void ChspsMaintenanceHandler::ConstructL()
    {
    // call the base class ConstructL
    CTimer::ConstructL();
    iHeaderDataList = new( ELeave ) CArrayPtrSeg<HBufC8>(KHeaderListGranularity);
    iResult = ChspsResult::NewL();
    // add this timer to the active scheduler   
    CActiveScheduler::Add(this);
    iDefinitionRepository.RegisterObserverL( *this );
    }

// -----------------------------------------------------------------------------
// ChspsMaintenanceHandler::ServiceGetListHeadersL
// Fetches the header list from repository and writes the first header back to the caller 
// (other items were commented in a header).
// -----------------------------------------------------------------------------
//
void ChspsMaintenanceHandler::ServiceGetListHeadersL(const RMessage2& aMessage)
    {
    // only allow one request to be submitted at a time
    // all synch call must use local message pointer variable
    RMessagePtr2 messagePtr = aMessage;
    ThspsServiceCompletedMessage ret = EhspsGetListHeadersFailed;
    iDeliveryCount = 0;
        
    if (!IsActive() && !iSubscription)    // first request
        {
        TBuf8<KMaxHeaderDataLength8> searchMaskData;// save a search mask
        messagePtr.ReadL(1,searchMaskData,0);
        // constructing iSearcMask -object
        if (iSearchMask)
            {
            delete iSearchMask;
            iSearchMask = NULL;
            }
        iSearchMask = ChspsODT::NewL();
        iSearchMask->UnMarshalHeaderL(searchMaskData);
        
        // check the device language
        iLanguage  = iThemeServer.DeviceLanguage();        
 	    iSearchMask->SetOdtLanguage( (TInt)iLanguage );
                 
        // now there is a subscription
        iSubscription = ETrue;
        // fetch the header list from repository
        ret = hspsGetListHeaders(searchMaskData, *iHeaderDataList);
        if (ret == EhspsGetListHeadersSuccess)
            {
            TInt count = iHeaderDataList->Count();
            // result informs list count in query
            iResult->iIntValue1 = count;
            if ( !count )
                {
                ret = EhspsGetListHeadersEmpty;
                }
            }
            
        CompleteRequest(ret, messagePtr);    
        }
    else
        {
        iResult->iSystemError = KErrInUse;
        iResult->iSystemError = KErrInUse;
        CompleteRequest( EhspsServiceRequestError, iMessagePtr );
        }
    }

// -----------------------------------------------------------------------------
// ChspsMaintenanceHandler::ServiceGetNextHeaderL
// Writes back to the caller the next headers in the header list one at a time
// (other items were commented in a header).
// -----------------------------------------------------------------------------
//
void ChspsMaintenanceHandler::ServiceGetNextHeaderL(const RMessage2& aMessage)
    {
    // only allow one request to be submitted at a time
    iRequestMessage = (ThspsServiceRequestMessage) aMessage.Function();
    iCompletedMessage = EhspsGetListHeadersUpdate;
    // now using message pointer as member variable because of asynch call
    iMessagePtr = aMessage;
    if ( !IsActive() && iSubscription)  // requesting next as should
        {
        // is there headers to delivere left
        if (iHeaderDataList->Count() > iDeliveryCount)
            {                                    
            // Handle copying of logo icon resources
            CopyIconsToHomescreenL( aMessage.SecureId().iId );
            
            // at least one header on the list
            TPtr8 bufPtr( iHeaderDataList->At(iDeliveryCount)->Des() );
            iMessagePtr.WriteL(2, bufPtr, 0);
            // add list count
            iDeliveryCount++;
            // deliver a list item
            CompleteRequest( EhspsGetListHeadersUpdate, iMessagePtr );
            }
        }
    else
        {
        iResult->iSystemError = KErrInUse;
        iResult->iXuikonError = KErrPermissionDenied;
        CompleteRequest( EhspsServiceRequestError, iMessagePtr );
        }
    }
    
// -----------------------------------------------------------------------------
// ChspsMaintenanceHandler::CopyIconsToHomescreenL
// -----------------------------------------------------------------------------
//
void ChspsMaintenanceHandler::CopyIconsToHomescreenL(
        const TUint aAppUid ) 
    {        
    HBufC8* headerData = iHeaderDataList->At(iDeliveryCount);
    ChspsODT* header = ChspsODT::UnMarshalHeaderLC( headerData->Des() );
    if ( iServerSession && header )
        {    
        if( header->LogoFile().Length() )
            {                              
            // If a file reference was found from the logo declaration
            TFileName filename;
            if ( hspsServerUtil::IsFile( header->LogoFile(), filename ) )
                {
                if ( !iFileMan )
                    {                
                    iFileMan = CFileMan::NewL( iServerSession->FileSystem() );
                    }
    
                // Get client's private directory: 
                // We should use some common directory if there are more than one SAPI clients,
                // for now we can copy files to AI3's private folder
                _LIT( KPrivatePath, "c:\\private\\%X\\");
                TPath privatePath;            
                privatePath.Format( KPrivatePath, aAppUid );                
                                                                                        
                // Append private path to the logo file reference
                TInt offset = header->LogoFile().FindF( filename );
                if ( offset >= 0 )
                    {                                      
                    // Insert private path prior to the file reference                    
                    HBufC* logoBuf = HBufC::NewLC( 
                            privatePath.Length() + header->LogoFile().Length() );
                    logoBuf->Des().Copy( header->LogoFile() );
                    logoBuf->Des().Insert( offset, privatePath );
                    header->SetLogoFileL( logoBuf->Des() );
                    CleanupStack::PopAndDestroy( logoBuf );
                                        
                    // Replace the serialized header descriptor                    
                    HBufC8* newHeaderData = header->MarshalHeaderL();                    
                    CleanupStack::PushL( newHeaderData );
                    iHeaderDataList->InsertL( 
                           iDeliveryCount,
                           newHeaderData );                       
                    CleanupStack::Pop( newHeaderData );                    
                    delete headerData;
                    headerData = 0;
                    iHeaderDataList->Delete( iDeliveryCount + 1 );
                    }                               

                // Set target directory for file copying
                TPath targetPath;            
                targetPath.Copy( privatePath );
                targetPath.Append( filename );
                
                // Set source directory for file copying
                TFileName sourceFile;     
                _LIT( KThemesFolder, "themes\\" );
                iServerSession->FileSystem().SessionPath( sourceFile );
                sourceFile.Append( KThemesFolder );                
                sourceFile.Append( filename );
                
                // Create the target path and copy files when required
                hspsServerUtil::CopyResourceFileL(
                        iServerSession->FileSystem(),
                        *iFileMan, 
                        targetPath,
                        sourceFile );
                }
            }
        }
    CleanupStack::PopAndDestroy( header );
    }

// -----------------------------------------------------------------------------
// ChspsMaintenanceHandler::SetServerSession
// -----------------------------------------------------------------------------
//
void ChspsMaintenanceHandler::SetServerSession(
        ChspsThemeServerSession* aServerSession )
    {
    iServerSession = aServerSession;
    }

// -----------------------------------------------------------------------------
// ChspsMaintenanceHandler::ServiceSetActiveThemeL
// Sets the active theme and writes back that themes header data
// (other items were commented in a header).
// -----------------------------------------------------------------------------
//    
void ChspsMaintenanceHandler::ServiceSetActiveThemeL(const RMessage2& aMessage)
    {
    ThspsServiceCompletedMessage ret = EhspsSetActiveThemeFailed;
    // using message pointer as a local variable because of a synch call
    RMessagePtr2 messagePtr = aMessage;

    TBuf8<KMaxHeaderDataLength8> setMaskData;
    messagePtr.ReadL(1,setMaskData,0);
    //constructing setMask -object
    if( iSetMask )
        {
        delete iSetMask;
        iSetMask = NULL;
        }
    iSetMask = ChspsODT::NewL();
    iSetMask->UnMarshalHeaderL( setMaskData );
    
    // calling actual activation
    ChspsODT* odt;
    odt = ChspsODT::NewL();
    CleanupStack::PushL(odt);
    
    
    iDefinitionRepository.Lock();
    // In case of error. repository is unlocked    
    CleanupStack::PushL( TCleanupItem( UnlockRepository, &iDefinitionRepository ) );
    ret = hspsSetActiveTheme( *iSetMask, *odt );
    if ( ret == EhspsSetActiveThemeSuccess )
        {                
        // Store updated ODT to definition repository
        iDefinitionRepository.SetOdtL( *odt );
        
        HBufC8* headerdata;
        headerdata = odt->MarshalHeaderL();
        if (headerdata != NULL)
            {
            // Write activated ODT header data to client
            CleanupStack::PushL(headerdata);
            messagePtr.WriteL(2,headerdata->Des(),0);
            CleanupStack::PopAndDestroy(headerdata);
            
            ThspsRepositoryInfo info( EhspsODTActivated );
            iDefinitionRepository.RegisterNotification( info );
        }
    }
    iDefinitionRepository.Unlock();
    CleanupStack::Pop(&iDefinitionRepository);
        

    CleanupStack::Pop(odt);
    if (odt)
        {
        delete odt;
        odt = NULL;
        }
        
    // complete the message
    CompleteRequest(ret, messagePtr );        
    }


// -----------------------------------------------------------------------------
// ChspsMaintenanceHandler::ServiceRestoreDefaultL
// Restores the default theme according to the information in aMessage and writes 
// back restored themes header data. 
// (other items were commented in a header).
// -----------------------------------------------------------------------------
//    
void ChspsMaintenanceHandler::ServiceRestoreDefaultL( const RMessage2& aMessage )
    {
    ThspsServiceCompletedMessage ret = EhspsRestoreDefaultFailed;
    // using message pointer as a local variable because of synch call
    RMessagePtr2 messagePtr = aMessage;
    // TIpcArgs(TInt aAppUid, &aSetMaskData, &aHeaderData)
    TBuf8<KMaxHeaderDataLength8> setMaskData;
    messagePtr.ReadL( 1, setMaskData, 0 );
    //constructing setMask -object
    if ( iSetMask )
        {
        delete iSetMask;
        iSetMask = NULL;
        }
    iSetMask = ChspsODT::NewL();
    iSetMask->UnMarshalHeaderL( setMaskData );
    ChspsODT* odt;
    odt = ChspsODT::NewL();
    CleanupStack::PushL( odt );
    
    if ( !iDefinitionRepository.Locked() )
        {
        iDefinitionRepository.Lock();
        // In case of error. repository is unlocked    
        CleanupStack::PushL( TCleanupItem( UnlockRepository, &iDefinitionRepository ) );
        // calling actual restoration
        ret = hspsRestoreDefault( *iSetMask, *odt );
        if ( ret == EhspsRestoreDefaultSuccess )
            {
            HBufC8* headerdata;
            headerdata = odt->MarshalHeaderL();
            if (headerdata != NULL)
                {
                CleanupStack::PushL( headerdata );
                messagePtr.WriteL( 2, headerdata->Des(), 0 );
                CleanupStack::PopAndDestroy( headerdata );
                
                // inform for cache update to the repository so that everyone will know 
                // about the change
                ThspsRepositoryInfo info( EhspsCacheUpdate );
                iDefinitionRepository.RegisterNotification( info );
                }
            }
        iDefinitionRepository.Unlock();
        CleanupStack::Pop(&iDefinitionRepository);
        }
    else
        {
        ret = EhspsRestoreDefaultFailed;

#ifdef HSPS_LOG_ACTIVE  
        if( iLogBus )
            {
            iLogBus->LogText( _L( "ChspsMaintenanceHandler::ServiceRestoreDefaultL(): - failed, repository is locked." ) );
            }
#endif            
        }
        
    
    CleanupStack::PopAndDestroy( odt );
    // complete the message
    CompleteRequest( ret, messagePtr );
    }

// -----------------------------------------------------------------------------
// ChspsMaintenanceHandler::ServiceRemoveThemeL
// Reads ODT from RMessage2 and calls the actual removal method.
// (other items were commented in a header).
// -----------------------------------------------------------------------------
//    
void ChspsMaintenanceHandler::ServiceRemoveThemeL( const RMessage2& aMessage )
    {
    ThspsServiceCompletedMessage ret = EhspsRemoveThemeFailed;
    // using message pointer as a local variable because of synch call
    RMessagePtr2 messagePtr = aMessage;
    
    // TIpcArgs(TInt aAppUid, &aSetMaskData, &aHeaderData)
    TBuf8<KMaxHeaderDataLength8> setMaskData;
    messagePtr.ReadL( 1, setMaskData, 0 );
    
    //constructing setMask -object
    if ( iSetMask )
        {
        delete iSetMask;
        iSetMask = NULL;
        }
    
    iSetMask = ChspsODT::NewL();
    iSetMask->UnMarshalHeaderL( setMaskData );
   
    // calling actual removal
    ret = hspsRemoveThemeL( *iSetMask );
            
    // complete the message
    CompleteRequest( ret, messagePtr );
    }

// -----------------------------------------------------------------------------
// Appends an application configuration with the provided plugin configuration
// -----------------------------------------------------------------------------
//    
void ChspsMaintenanceHandler::ServiceAddPluginL( const RMessage2& aMessage )
    {
    // Defaults
    ThspsServiceCompletedMessage ret = EhspsAddPluginFailed;
    iResult->iXuikonError = KErrNotFound;
    TInt err = KErrNone;
    
    // Parameters
    RMessagePtr2 messagePtr = aMessage;
    
    // IPC slots: 
    // #0) output: externalized ChspsResult for error handling
    // #1) input: ThpsParamAddPlugin struct
    // #2) output: added plugin id
    
    // Get service parameters from IPC slot #1
    ThpsParamAddPlugin params;        
    TPckg<ThpsParamAddPlugin> packagedStruct(params);    
    aMessage.ReadL(1, packagedStruct);                        
    const TInt appUid = params.appUid;
    const TInt configurationId = params.configurationId;
    const TInt pluginUid = params.pluginUid;
    const TInt newPos = params.positionIndex;    
          
    // Application configuration
    ChspsODT *appODT = NULL;
    if ( !err )
        {    
        // Get active application configuration from the central repository
        TInt appConfUid;       
        err = iCentralRepository.Get( appUid, appConfUid );
        if ( err || appConfUid < 1 )
            {
            err = KErrNotFound;         
            }
        else
            {
            // Get application configuration
            appODT = ChspsODT::NewL();
            CleanupStack::PushL( appODT );
            err = iThemeServer.GetConfigurationL( 
                    appUid, 
                    appConfUid,
                    *appODT );                                    
            }
        }
        
    // Plugin configuration
    ChspsODT* pluginODT = NULL;
    if ( !err )
        {
        // Find the plugin configuration (interface is unknown, so 1st argument is set to zero)
        pluginODT = ChspsODT::NewL();
        CleanupStack::PushL( pluginODT );        
        err = iThemeServer.GetConfigurationL( 
                0, 
                pluginUid,
                *pluginODT );
        }
           
    // Check needed space for addition. Returns system wide error code.
    if( !err )
        {    
        err = hspsServerUtil::EnoughDiskSpaceAvailableL(
                *pluginODT, 
                iThemeServer.DeviceLanguage(),
                iServerSession->FileSystem(),
                EDriveC,
                KAdditionalRequiredDiskSpace );
        }
    
    // Modifications
    TInt usedConfId = 0;
    TInt usedPluginId = 0;
    if ( !err && appODT && pluginODT )
       {       
           if ( iDefinitionRepository.Locked() )
               {
            // Repository locked
               err = KErrAccessDenied;
               }
           else
               {
               // Lock the Plugin Repository (a.k.a. Def.rep)
               iDefinitionRepository.Lock();                                
            CleanupStack::PushL( TCleanupItem( UnlockRepository, &iDefinitionRepository ) );
                                                        
            // Get used conf & plugin ids
            GetUsedIdsL( 
                appODT->DomDocument(),
                usedConfId,
                usedPluginId );            
                
            // Add the plugin configuration into the application configuration
            err = AppendConfigurationL( 
                *appODT, 
                *pluginODT, 
                configurationId,                    
                newPos,
                usedConfId,
                usedPluginId );
        
#ifdef HSPS_LOG_ACTIVE            
            if( appODT && iLogBus )
                {
                ChspsOdtDump::Dump( *appODT, *iLogBus );
                }
#endif            
    
            // Stores the new application configuration into the repository
            if ( !err )
                {
                err = iDefinitionRepository.SetOdtL( *appODT );
                if ( err )
                    {
#ifdef HSPS_LOG_ACTIVE  
                    if( iLogBus )
                        {
                        iLogBus->LogText( _L( "ChspsMaintenanceHandler::ServiceAddPluginL(): - Updating failed with code: %d" ), err );
                        }
#endif
                    }
                }
            
            // Unlock after the changes have been done
            iDefinitionRepository.Unlock();
            CleanupStack::Pop(&iDefinitionRepository);

            if ( !err )
                {
                // Inform clients that the ODT has been modified
                ThspsRepositoryInfo info( 
                    ThspsRepositoryEvent(EhspsODTModified),
                    appODT->RootUid(),
                    appODT->ThemeUid(),
                    aMessage.SecureId().iId,
                    appODT->ProviderUid(),
                    0,
                    0,
                    pluginUid,usedPluginId,ETrue,
                    pluginODT->ThemeFullName(),
                    (TLanguage)(appODT->OdtLanguage() ) );                                
                iDefinitionRepository.RegisterNotification( info );
                }                                        
            }       
       }    
    
    // Cleaning
    if ( pluginODT )
        {
        CleanupStack::PopAndDestroy( pluginODT );
        pluginODT = NULL;
        }
    if ( appODT )
        {
        CleanupStack::PopAndDestroy( appODT );
        appODT = NULL;
        }
    
    // Error handling
    iResult->iXuikonError = err;
    if ( !err )
        {
        // Get plugin configuration
        ret = EhspsAddPluginSuccess;
        }

    // Return id of the added plugin    
    TPckg<TInt> packagedInt(usedPluginId);                                    
    messagePtr.WriteL(2, packagedInt );
    
    // Completion 
    CompleteRequest( ret, messagePtr );
    }

// -----------------------------------------------------------------------------
// Finds last id value from the provided DOM.
// -----------------------------------------------------------------------------
//
void ChspsMaintenanceHandler::GetUsedIdsL(
        ChspsDomDocument& aDom,
        TInt& aLastUsedConfId,
        TInt& aLastUsedPluginId )        
    {
    aLastUsedConfId = 0;
    aLastUsedPluginId = 0;
    
    ChspsDomDepthIterator* iter = ChspsDomDepthIterator::NewL( *aDom.RootNode() );
    CleanupStack::PushL( iter );
        
    // Find a plugin node with an id attribute that matches the provided parent plugin id
    ChspsDomNode* node = iter->First();    
    while( node )
        {                
        const TDesC8& name = node->Name();
        
        // A Configuration element was found
        if ( name == KConfigurationElement )
            {
            // Get value of the id attribute (should exist)
            ChspsDomList& attrList = node->AttributeList();
            ChspsDomAttribute* attr = static_cast<ChspsDomAttribute*>( attrList.FindByName(KConfigurationAttrId) );
            if ( !attr )
                {
#ifdef HSPS_LOG_ACTIVE  
                if( iLogBus )
                    {
                    iLogBus->LogText( _L( "ChspsMaintenanceHandler::GetUsedIdsL(): Id attribute is missing!" ) );
                    }
#endif                
                User::Leave( KErrCorrupt );                
                }
            
            TInt id(0);
            const TDesC8& idDesc = attr->Value();
            TLex8 lex( idDesc );                                
            if ( lex.Val( id ) )
                {
#ifdef HSPS_LOG_ACTIVE  
                if( iLogBus )
                    {
                    iLogBus->LogText( _L( "ChspsMaintenanceHandler::GetUsedIdsL(): Invalid id" ) );
                    }
#endif                
                User::Leave( KErrCorrupt );                    
                }
            if ( id > aLastUsedConfId )
                {
                aLastUsedConfId = id;
                }    
            }
        
        // A Plugin element was found 
        else if ( name == KPluginElement )
            {
            // Get value of the id attribute (should exist)
            ChspsDomList& attrList = node->AttributeList();
            ChspsDomAttribute* attr = static_cast<ChspsDomAttribute*>( attrList.FindByName(KPluginAttrId) );
            if ( !attr )
                {
#ifdef HSPS_LOG_ACTIVE  
                if( iLogBus )
                    {
                    iLogBus->LogText( _L( "ChspsMaintenanceHandler::GetUsedIdsL(): Id attribute is missing!" ) );
                    }
#endif                
                User::Leave( KErrCorrupt );                
                }
            
            TInt id(0);
            const TDesC8& idDesc = attr->Value();
            TLex8 lex( idDesc );                                
            if ( lex.Val( id ) )
                {
#ifdef HSPS_LOG_ACTIVE  
                if( iLogBus )
                    {
                    iLogBus->LogText( _L( "ChspsMaintenanceHandler::GetUsedIdsL(): Invalid id" ) );
                    }
#endif                
                User::Leave( KErrCorrupt );                    
                }
            if ( id > aLastUsedPluginId )
                {
                aLastUsedPluginId = id;
                }                                
            }
        node = iter->NextL();      
        }
    CleanupStack::PopAndDestroy( iter );        
    }

// -----------------------------------------------------------------------------
// Appends an application configuration with an instance of a plugin configuration.
// -----------------------------------------------------------------------------
//
TInt ChspsMaintenanceHandler::AppendConfigurationL(
        ChspsODT& aAppODT,
        const ChspsODT& aPluginODT,
        const TInt aConfigurationId,        
        const TInt aNewPosition,
        TInt& aConfId,
        TInt& aPluginId )
    {            
    TInt ret = KErrNone;
    
    // Input validation
    if ( aPluginODT.ConfigurationType() == EhspsAppConfiguration 
            || aConfigurationId < 0 
            || aPluginODT.ThemeUid() < 1             
            || aConfId < 0
            || aPluginId < 0 )
        {
        return KErrArgument;
        }
                
    // Find a specific configuration node
     ChspsDomNode* confNode = hspsServerUtil::FindConfigurationNodeL( 
            aAppODT, 
            aConfigurationId );
    if ( !confNode )
        {
        return KErrNotFound;        
        }
    
    // Find first plugins node under the searchFromNode node
    ChspsDomNode* pluginsNode = FindPluginsNode( *confNode );                      
    if ( !pluginsNode )
        {
        // If there is no plugins node, it means we cannot add plugins!
        return KErrAccessDenied;
        }
    
    // Find plugin references from the plugin configuration being added,
    // generate ids and update the plugin dom accordingly (changes are not saved)    
    ret = HandlePluginReferencesL(
            aAppODT,
            (ChspsODT&)aPluginODT, 
            aConfId,
            aPluginId );            
    if ( !ret )
        {
        // Get new unique id
        aPluginId++;
        
        // Append the application configuration with the full plugin configuration dom
        ret = AppendPluginConfigurationL(
                aAppODT,
                *pluginsNode,
                aPluginODT,                         
                aNewPosition,
                aPluginId );                                                                            
        }                    
               
    return ret;
    }

// -----------------------------------------------------------------------------
// Finds the plugins node of the provided plugin node.
// -----------------------------------------------------------------------------
//
ChspsDomNode* ChspsMaintenanceHandler::FindPluginsNode(
        ChspsDomNode& aPluginNode )                
    {        
    ChspsDomNode* pluginsNode = NULL;
    ChspsDomNode* confNode = &aPluginNode;    
    if ( aPluginNode.Name() != KConfigurationElement )
        {
        ChspsDomList& childList = aPluginNode.ChildNodes();    
        confNode = (ChspsDomNode*)childList.FindByName( KConfigurationElement );
        }
    if ( confNode )
        {        
        ChspsDomNode* controlNode = (ChspsDomNode*)confNode->ChildNodes().FindByName( KControlElement );
        if ( controlNode )
            {            
            pluginsNode = (ChspsDomNode*)controlNode->ChildNodes().FindByName( KPluginsElement );
            }
        }

#ifdef HSPS_LOG_ACTIVE  
    if ( !pluginsNode )
        {
        if( iLogBus )
            {
            iLogBus->LogText( _L( "ChspsMaintenanceHandler::FindPluginsNode(): failed to find a plugins node" ) );
            }
        }
#endif
        
    return pluginsNode;
    }

// -----------------------------------------------------------------------------
// Appends a plugins list with a new plugin node
// -----------------------------------------------------------------------------
//
TInt ChspsMaintenanceHandler::AppendPluginConfigurationL(
        ChspsODT& aAppODT,
        ChspsDomNode& aPluginsNode,        
        const ChspsODT& aPluginODT,                
        const TInt aNewPosition,
        const TInt aNewPluginId )        
    {          
    TInt err = KErrGeneral;
    
    // Input validation
    if ( aPluginODT.ThemeUid() < 1 || aNewPluginId < 1 )
        {
        return KErrArgument;
        }
        
    // Find a node to be added, step over any optional xml elements 
    // (start cloning from the configuration element onwards)    
    ChspsDomDocument& appDom = aAppODT.DomDocument();
    ChspsDomDepthIterator* pluginIter = ChspsDomDepthIterator::NewL( *aPluginODT.DomDocument().RootNode() );
    CleanupStack::PushL( pluginIter );                                
    ChspsDomNode* node = pluginIter->First();                              
    TBool steppingtoConfNode(EFalse);                     
    while(node && !steppingtoConfNode)
        {
        const TDesC8& pluginNodeName = node->Name();
         
        // Find the Configuration element
        if( pluginNodeName == KConfigurationElement )
            {            
            steppingtoConfNode=ETrue;
            }
        else
            {
            node = pluginIter->NextL();
            }                 
        }
    CleanupStack::PopAndDestroy( pluginIter );
    
    if ( node )
        {
        // The node instance should now hold the configuration element etc        
        ChspsDomNode* confNode = node->CloneL( aPluginsNode.StringPool());
        CleanupStack::PushL( confNode );
                
        // Create a new plugin node with mandatory attributes
        ChspsDomNode* pluginNode = appDom.CreateElementNSL( 
            KPluginElement,         
            aPluginsNode.Namespace()            
            );
        CleanupStack::PushL( pluginNode );
                                       
        // Attach to the plugin node
        pluginNode->AddChildL( confNode );
        confNode->SetParent( pluginNode );
        
        // Set attributes of the plugin node                
        hspsServerUtil::AddAttributeNumericL( *pluginNode, KPluginAttrId, aNewPluginId );
        hspsServerUtil::AddAttributeNumericL( *pluginNode, KPluginAttrUid, aPluginODT.ThemeUid(), EHex );

        // Add active attribute.
        // If first plugin in list, then set as active.
        // Otherwise other plugin should already be active at this level.
        if( aPluginsNode.ChildNodes().Length() == 0 )
            {
            hspsServerUtil::AddAttributeDescL( *pluginNode, KPluginAttrActive, KPluginActiveStateActive );
            }
        else
            {
            hspsServerUtil::AddAttributeDescL( *pluginNode, KPluginAttrActive, KPluginActiveStateNotActive );
            }
        
        // Also make sure that if new node has child nodes, that they get active parameter also.
        hspsServerUtil::EditPluginNodeActivityL( pluginNode,
                                                 hspsServerUtil::EActivateFirst );
        
        // Check when to insert and when to add to the end
        if ( aNewPosition < 0 || aNewPosition >= aPluginsNode.ChildNodes().Length() )
            {
            // Append the plugin node to the end
            aPluginsNode.AddChildL( pluginNode );
            }
        else
            {
            // Insert the plugin node to the position
            aPluginsNode.AddChildL( pluginNode, aNewPosition );    
            }        
        pluginNode->SetParent( &aPluginsNode );
        
        CleanupStack::Pop( pluginNode );
        CleanupStack::Pop( confNode );
        err = KErrNone;
        }
                             
    return err;
    }

// -----------------------------------------------------------------------------
// -----------------------------------------------------------------------------
// ChspsMaintenanceHandler::HandlePluginReferencesL()
// ImportPlugins phase.
// -----------------------------------------------------------------------------
//
TInt ChspsMaintenanceHandler::HandlePluginReferencesL( 
        ChspsODT& aAppODT,
        ChspsODT& aPluginODT,        
        TInt& aLastConfId,
        TInt& aLastPluginId )
    {   
    
    ChspsDomDocument& pluginDom = aPluginODT.DomDocument(); 
    ChspsDomDepthIterator* iter = ChspsDomDepthIterator::NewL( *pluginDom.RootNode() );
    CleanupStack::PushL( iter );
            
    // Each configuration element get's an unique id value - same applies to the plugin elements.
    // The ids are then used to reference a specific configuration or plugin instance 
    // in the whole application configuration
        
    ChspsDomNode* node = iter->First();
    while( node )
        {            
        const TDesC8& name = node->Name();
        
        // Configuration element 
        if ( name == KConfigurationElement )
            {                                    
            // Generate an ID attribute for the configuration element
            aLastConfId++;
            hspsServerUtil::AddAttributeNumericL( *node, KConfigurationAttrId, aLastConfId );            
            }
        
        // Plugin element
        else if ( name == KPluginElement )
            {            
            // Check parent element
            ChspsDomNode* parentNode = node->Parent();
            const TDesC8& parentName = parentNode->Name();
            if( parentName == KPluginsElement )
                { 
                ChspsDomList& attrList = node->AttributeList();
                
                // Get configuration attribute from the plugin configuration                                                               
                ChspsDomAttribute* pluginUidAttr = 
                    static_cast<ChspsDomAttribute*> ( attrList.FindByName(KPluginAttrUid) );                                                
                if( !pluginUidAttr )
                    {
#ifdef HSPS_LOG_ACTIVE  
                    if( iLogBus )
                        {
                        iLogBus->LogText( _L( "ChspsMaintenanceHandler::HandlePluginReferencesL(): - Invalid XML" ) );
                        }
#endif                    
                    User::Leave( KErrCorrupt );
                    }         
                
                // Convert uids from string to numeric format                                        
                const TDesC8& pluginUidValue = pluginUidAttr->Value();                    
                const TUid pluginUid = hspsServerUtil::ConvertDescIntoUid(pluginUidValue);
                               
                // Get plugin configuration                                 
                ChspsODT* pluginOdt = ChspsODT::NewL();
                CleanupStack::PushL( pluginOdt );
                TInt err = iThemeServer.GetConfigurationL( 
                        0, 
                        pluginUid.iUid,
                        *pluginOdt );                
                if ( err )
                    {
#ifdef HSPS_LOG_ACTIVE  
                    if( iLogBus )
                        {
                        iLogBus->LogText( _L( "ChspsMaintenanceHandler::HandlePluginReferencesL(): - Failed to find a plugin" ) );
                        }
#endif

                    // Append an empty configuration with error status
                    AddErrorConfigurationL( 
                            pluginDom,
                            *node,
                            pluginUid.iUid );
                    
                    // Generate an ID attribute for the plugin element
                    aLastPluginId++;
                    hspsServerUtil::AddAttributeNumericL( *node, KPluginAttrId, aLastPluginId );                                        
                    }
                else
                    {
                    // Set value of the ID attribute
                    aLastPluginId++;
                    hspsServerUtil::AddAttributeNumericL( *node, KPluginAttrId, aLastPluginId );
                    
                                    
                    // Copy resources of the referenced plug-in instance to the application configuration
                    TInt resourceCount = pluginOdt->ResourceCount();
                    for ( TInt index=0; index < resourceCount; index++ )
                        {                    
                        ChspsResource& pluginResource = pluginOdt->ResourceL(index);
                        // Add only those that are located under the sources folder                                            
                        if ( pluginResource.FileName().FindF( KSourcesFolder ) > 0
                            || pluginResource.FileName().FindF( KLocalesFolder ) > 0 )
                                                
                            {                                                
                            ChspsResource* r = pluginResource.CloneL();
                            CleanupStack::PushL( r );
                            aAppODT.AddResourceL( r ); 
                            CleanupStack::Pop( r );
                            }
                        }
                    
                    ChspsDomDocument& document = pluginOdt->DomDocument();   
                    if ( !document.RootNode() )
                        {
                        User::Leave( KErrGeneral );
                        }
                    
                    // Find the KConfigurationElement to step over any optional xml elements 
                    ChspsDomDepthIterator* pluginIter = ChspsDomDepthIterator::NewL( *document.RootNode() );
                    CleanupStack::PushL( pluginIter );                                
                    ChspsDomNode* pluginNode =  pluginIter->First();                              
                    TBool steppingtoConfNode(EFalse);                     
                    while(pluginNode && !steppingtoConfNode)
                        {
                        const TDesC8& pluginNodeName = pluginNode->Name();
                         
                        if( pluginNodeName == KConfigurationElement )
                            {
                            steppingtoConfNode=ETrue;
                            }
                        else
                            {
                            pluginNode = pluginIter->NextL();
                            }                 
                        }
                    CleanupStack::PopAndDestroy( pluginIter );
                    
                    // Copy the plugin configuration to the main document.
                    ChspsDomNode* rootCopy = pluginNode->CloneL( node->StringPool());
                    rootCopy->SetParent( node );
                    node->AddChildL( rootCopy );
                    }
               
                CleanupStack::PopAndDestroy( pluginOdt );
                }
            }
        node = iter->NextL();      
        }
    CleanupStack::PopAndDestroy( iter );
    
    // Copy resources of the plugin to the application configuration    
    TInt resourceCount = aPluginODT.ResourceCount();
    for ( TInt index=0; index < resourceCount; index++ )
        {                    
        ChspsResource& pluginResource = aPluginODT.ResourceL(index);
        
        // Add only those that are located under the sources folder
        if ( pluginResource.FileName().FindF( KSourcesFolder ) > 0
              || pluginResource.FileName().FindF( KLocalesFolder ) > 0 )
            {                                                
            ChspsResource* r = pluginResource.CloneL();
            CleanupStack::PushL( r );
            aAppODT.AddResourceL( r ); 
            CleanupStack::Pop( r );
            }
        }
    
    return KErrNone;
    }


// -----------------------------------------------------------------------------
// Service for removing a plugin
// -----------------------------------------------------------------------------
//    
void ChspsMaintenanceHandler::ServiceRemovePluginL( const RMessage2& aMessage )
    {
    // Defaults
    ThspsServiceCompletedMessage ret = EhspsRemovePluginFailed;
    iResult->iXuikonError = KErrNotFound;
    TInt err = KErrNone;
    
    // Parameters
    RMessagePtr2 messagePtr = aMessage;
    
    // IPC slots: 
    // #0) output: externalized ChspsResult for error handling
    // #1) input: ThpsParamRemovePlugin struct
    
    // Get service parameters from IPC slot #1
    ThpsParamRemovePlugin params;        
    TPckg<ThpsParamRemovePlugin> packagedStruct(params);    
    aMessage.ReadL(1, packagedStruct);                        
    const TInt appUid = params.appUid;    
    const TInt pluginId = params.pluginId;               
        
    if ( pluginId < 1 )
        {
        err = KErrArgument;
        }
    
    // Application configuration
    ChspsODT *appODT = NULL;
    if ( !err )
        {    
        // Get active application configuration from the central repository
        TInt appConfUid;       
        err = iCentralRepository.Get( appUid, appConfUid );
        if ( err || appConfUid < 1 )
            {
            err = KErrNotFound;         
            }
        else
            {
            appODT = ChspsODT::NewL();
            CleanupStack::PushL( appODT );
            err = iThemeServer.GetConfigurationL( 
                    appUid, 
                    appConfUid,
                    *appODT );                                        
            }
        }

    // Modifications    
    if ( !err )
        {       
        if ( iDefinitionRepository.Locked() )
            {
            // Repository locked
            err = KErrAccessDenied;
            }
        else
            {
            TInt pluginUid;
            TBuf<KMaxFileName> pluginName;
            err = GetConfigurationNodeDataL(*appODT, 
                    pluginId,pluginUid, pluginName);
            // Lock the Plugin Repository (a.k.a. Def.rep)
            iDefinitionRepository.Lock();                                
            CleanupStack::PushL( TCleanupItem( UnlockRepository, &iDefinitionRepository ) );
            if( !err )
                {
                // Remove the plugin configuration from the application configuration
                err = RemoveConfigurationL( 
                    *appODT, 
                    pluginId );                                            
                }
            if ( !err )
                {

#ifdef HSPS_LOG_ACTIVE                
                if( appODT && iLogBus )
                    {
                    ChspsOdtDump::Dump( *appODT, *iLogBus );
                    }
#endif                
                // Stores the new application configuration into the repository
                err = iDefinitionRepository.SetOdtL( *appODT );
                if ( err )
                    {
#ifdef HSPS_LOG_ACTIVE  
                    if( iLogBus )
                        {
                        iLogBus->LogText( _L( "ChspsMaintenanceHandler::ServiceRemovePluginL(): - Updating failed" ) );
                        }
#endif                    
                    }
                }
            
            // Unlock after the changes have been done
            iDefinitionRepository.Unlock();
            CleanupStack::Pop(&iDefinitionRepository);

            if ( !err )
                {
                // Inform clients that the ODT has been modified
                ThspsRepositoryInfo info( 
                    ThspsRepositoryEvent(EhspsODTRemoved),
                    appODT->RootUid(),
                    appODT->ThemeUid(),
                    aMessage.SecureId().iId,
                    appODT->ProviderUid(),
                    0,0,
                    pluginUid,pluginId,ETrue,
                    pluginName,
                    (TLanguage)(appODT->OdtLanguage())
                    );                                
                iDefinitionRepository.RegisterNotification( info );
                }                                        
            }       
       }    
    
    // Cleaning
    if ( appODT )
        {
        CleanupStack::PopAndDestroy( appODT );
        appODT = NULL;
        }
    
    // Error handling
    iResult->iXuikonError = err;
    if ( !err )
        {        
        ret = EhspsRemovePluginSuccess;
        }
    
    // Completion 
    CompleteRequest( ret, messagePtr );
    }

// -----------------------------------------------------------------------------
// Service for setting plugin active.
// -----------------------------------------------------------------------------
//    
void ChspsMaintenanceHandler::ServiceSetActivePluginL( const RMessage2& aMessage )
    {
    // Init output parameters.       
    iResult->ResetData();
    
    // Read input parameters.
    ThpsParamSetActivePlugin params;
    TPckg<ThpsParamSetActivePlugin> packagedStruct( params );
    aMessage.ReadL( 1, packagedStruct );
    
    // Get uid of active application configuration.        
    TInt appConfUid = 0;       
    TInt err = iCentralRepository.Get( params.appUid, appConfUid );
    if ( err != KErrNone || appConfUid < 1 )
        {
        err = KErrNotFound;
        }
    
    // Load configuration from repository.
    ChspsODT* appODT = NULL;
    if( err == KErrNone )
        {
        appODT = ChspsODT::NewL();
        CleanupStack::PushL( appODT );
        err = iThemeServer.GetConfigurationL( 
                params.appUid, 
                appConfUid,
                *appODT );
        }

    // Initialize notification structure.
    ThspsRepositoryInfo notification( EhspsPluginActivated );
    if( err == KErrNone && appODT )
        {
        notification.iAppUid = appODT->RootUid();
        notification.iAppConfUid = appODT->ThemeUid();
        notification.iSecureId = 0;
        notification.iAppConfProviderUid = 0;
        notification.iPluginIfUid = 0;
        notification.iPluginProviderUid = 0;
        notification.iPluginUid = 0;
        notification.iPluginId = 0;
        notification.iLastNotification = ETrue;
        notification.iName = KNullDesC();
        notification.iLanguage = ELangTest;        
        }
    
    // Find required node and set it active.
    if( err == KErrNone )
        {
        // Find a plugin node with the provided id.
        ChspsDomNode* pluginNode = hspsServerUtil::FindPluginNodeL( *appODT,
                                                                    params.pluginId );
        if ( pluginNode )
            {        
            // Get parent node.
            ChspsDomNode* parentNode = pluginNode->Parent();
            if ( parentNode )
                {
                // Deactivate all plugins at defined level. Do not recurse.
                const TInt KDepth = 1;
                hspsServerUtil::EditPluginNodeActivityL( parentNode,
                                                         hspsServerUtil::EDeactivateAll,
                                                         KDepth );
    
                // Activate defined plugin.
                hspsServerUtil::AddAttributeDescL( *pluginNode,
                                                    KPluginAttrActive,
                                                    KPluginActiveStateActive ); 
                
                // Fill notification up2date.
                notification.iPluginUid = hspsServerUtil::GetPluginUid( pluginNode ).iUid;
                notification.iPluginId = hspsServerUtil::GetPluginId( pluginNode );
                }
            else
                {
                err = KErrCorrupt;
                }
            }
        else
            {
            err = KErrNotFound;
            }
        }
    
    if( err == KErrNone )
        {
        if ( iDefinitionRepository.Locked() )
            {
            // Repository locked
            err = KErrAccessDenied;
            }
        }
    
    if( err == KErrNone )
        {
        // Lock and push to cleanup stack.
        iDefinitionRepository.Lock();                                
        CleanupStack::PushL( TCleanupItem( UnlockRepository, &iDefinitionRepository ) );
        
        // Stores the new application configuration into the repository
        err = iDefinitionRepository.SetOdtL( *appODT );

        // Unlock and remove from cleanupstack.
        iDefinitionRepository.Unlock();
        CleanupStack::Pop( &iDefinitionRepository );
        }
    
    // Notify.
    if( err == KErrNone )
        {
        iDefinitionRepository.RegisterNotification( notification );
        }
    
    // Set output parameter values and complete request.    
    iResult->iSystemError = err;
    iResult->iXuikonError = err;
    
    // Complete the message.    
    RMessagePtr2 messagePtr = aMessage;    
    if( err == KErrNone )
        {
        CompleteRequest( EhspsSetActivePluginSuccess, messagePtr );
        }
    else
        {
        CompleteRequest( EhspsSetActivePluginFailed, messagePtr );
        }    
    
#ifdef HSPS_LOG_ACTIVE                
        if( iLogBus )
            {
            ChspsOdtDump::Dump( *appODT, *iLogBus );
            }
#endif    
        
    // Cleanup.
    if ( appODT )
        {
        CleanupStack::PopAndDestroy( appODT );
        appODT = NULL;
        }    

#ifdef HSPS_LOG_ACTIVE                
    if( iLogBus )
        {
        if( err == KErrNone )
            {
            iLogBus->LogText( _L( "ChspsMaintenanceHandler::ServiceSetActivePluginL: - OK" ) );
            }
        else
            {
            iLogBus->LogText( _L( "ChspsMaintenanceHandler::ServiceSetActivePluginL: - Error" ) );
            }
        }
#endif      
    }

// -----------------------------------------------------------------------------
// Get plugin's configuration node attributes(plugin name, pluginUid ) 
// Can expanded to get configuration Id and plugin type also
// -----------------------------------------------------------------------------
//
TInt ChspsMaintenanceHandler::GetConfigurationNodeDataL(
        ChspsODT& aAppODT,
        const TInt aPluginId,
        TInt& aPluginUid,
        TDes& aPluginName )
    {
    TInt err = KErrNotFound;
    ChspsDomNode *node = hspsServerUtil::FindPluginNodeL( aAppODT, aPluginId );
    if ( node )
        {
        ChspsDomDepthIterator* iter = ChspsDomDepthIterator::NewL( *node );
        CleanupStack::PushL( iter );

            // Find a configuration node with an id attribute that matches the provided id
        ChspsDomNode* node2 = iter->First();
        TBool jobDone = EFalse;
        while( node2 && !jobDone )
           {               
           const TDesC8& name = node2->Name();
                
            // An element was found 
            if ( name == KConfigurationElement )
                {
                TInt pluginUid = 0;
                TBuf<KMaxFileName> pluginName;
                ChspsDomList& attrList = node2->AttributeList();                 
                ChspsDomAttribute* pluginUidAttr = 
                static_cast<ChspsDomAttribute*> ( attrList.FindByName(KConfigurationAttrUid) );                                                
                if( !pluginUidAttr )
                    {
#ifdef HSPS_LOG_ACTIVE  
                    if( iLogBus )
                        {
                        iLogBus->LogText( _L( "ChspsMaintenanceHandler::GetConfigurationNodeDataL(): - Invalid XML" ) );
                        }
#endif                    
                    err = KErrGeneral;
                    }         
                else            
                    {
                                    // Convert uids from string to numeric format                                        
                    const TDesC8& pluginUidValue = pluginUidAttr->Value();                    
                    const TUid uid = hspsServerUtil::ConvertDescIntoUid(pluginUidValue);               
                    if ( uid.iUid > 0 )
                       {                   
                       pluginUid = uid.iUid;
                       // Override default status
                       err = KErrNone;
                       }
                    }
                if( !err )
                    {
                    ChspsDomAttribute* pluginUidNameAttr = 
                    static_cast<ChspsDomAttribute*> ( attrList.FindByName(KConfigurationAttrName) );                                                
                    if( !pluginUidNameAttr )
                        {
#ifdef HSPS_LOG_ACTIVE  
                        if( iLogBus )
                            {
                            iLogBus->LogText( _L( "ChspsMaintenanceHandler::GetConfigurationNodeDataL(): - Invalid XML" ) );
                            }
#endif                        
                        err = KErrGeneral;
                        }         
                    else            
                        {
                        // Convert uids from string to numeric format                                        
                        const TDesC8& pluginNameValue = pluginUidNameAttr->Value();                                   
                        if ( pluginNameValue.Length() > 0 )
                            {                   
                            pluginName.Copy( pluginNameValue );
                            err = KErrNone;
                            }
                        }            
                    }
                if( !err )
                    {
                    aPluginUid = pluginUid;
                    aPluginName.Copy( pluginName );
                    jobDone = ETrue;
                    }
                }
            node2 = iter->NextL();
           }
        CleanupStack::PopAndDestroy( iter );
        }

         
    return err;
    }
    
// -----------------------------------------------------------------------------
// ChspsMaintenanceHandler::ServiceReplacePluginL()
// -----------------------------------------------------------------------------
//
void ChspsMaintenanceHandler::ServiceReplacePluginL( const RMessage2& aMessage )
    {
    // Defaults
    ThspsServiceCompletedMessage ret = EhspsReplacePluginFailed;
    iResult->iXuikonError = KErrNotFound;
    TInt err = KErrNone;
    
    // Parameters
    RMessagePtr2 messagePtr = aMessage;
    
    // IPC slots: 
    // #0) output: externalized ChspsResult for error handling
    // #1) input: ThpsParamReplacePlugin struct                
    ThspsParamReplacePlugin params;        
    TPckg<ThspsParamReplacePlugin> packagedStruct(params);    
    aMessage.ReadL(1, packagedStruct);
    const TInt appUid( params.appUid );
    const TInt pluginId( params.pluginId );    
    const TInt confUid( params.confUid );                              
    if ( pluginId < 1 || confUid  < 1 )
        {
        err = KErrArgument;
        }
    
    // Application configuration
    ChspsODT *appODT = NULL;
    if ( !err )
        {    
        // Get active application configuration from the central repository
        TInt appConfUid;       
        err = iCentralRepository.Get( appUid, appConfUid );
        if ( err || appConfUid < 1 )
            {
            err = KErrNotFound;         
            }
        else
            {
            appODT = ChspsODT::NewL();
            CleanupStack::PushL( appODT );
            err = iThemeServer.GetConfigurationL( 
                    appUid, 
                    appConfUid,
                    *appODT );                                        
            }
        }
        
    // Plugin configuration
    ChspsODT *pluginODT = NULL;
    if ( !err )
        {
        // Find the plugin configuration (interface is unknown, so 1st argument is set to zero)
        pluginODT = ChspsODT::NewL();
        CleanupStack::PushL( pluginODT );        
        err = iThemeServer.GetConfigurationL( 
                0, 
                confUid,
                *pluginODT );
        }

    // Check needed space for replace. Returns system wide error code.
    if( !err )
        {
        err = hspsServerUtil::EnoughDiskSpaceAvailableL(
                *pluginODT, 
                iThemeServer.DeviceLanguage(),
                iServerSession->FileSystem(),
                EDriveC,
                KAdditionalRequiredDiskSpace );
        }
    
    // Modifications    
    if ( !err )
        {       
        if ( iDefinitionRepository.Locked() )
            {
            // Repository locked
            err = KErrAccessDenied;
            }
        else
            {                        
            // Lock the Plugin Repository (a.k.a. Def.rep)
            iDefinitionRepository.Lock();                                
            CleanupStack::PushL( TCleanupItem( UnlockRepository, &iDefinitionRepository ) );            
            
            // Remove the plugin configuration from the application configuration
            err = ReplaceConfigurationL( 
                *appODT, 
                pluginId,
                *pluginODT );                                            
            if ( !err )
                {

#ifdef HSPS_LOG_ACTIVE                
                if( appODT && iLogBus )
                    {
                    ChspsOdtDump::Dump( *appODT, *iLogBus );
                    }
#endif                
                // Stores the new application configuration into the repository
                err = iDefinitionRepository.SetOdtL( *appODT );
                // Unlock after the changes have been done
                iDefinitionRepository.Unlock();
                if ( err )
                    {
#ifdef HSPS_LOG_ACTIVE  
                    if( iLogBus )
                        {
                        iLogBus->LogText( _L( "ChspsMaintenanceHandler::ServiceReplacePluginL(): - Updating failed" ) );
                        }
#endif                    
                    }
                else
                    {
                    // Inform clients that the ODT has been modified
                    ThspsRepositoryInfo info( 
                        ThspsRepositoryEvent(EhspsPluginReplaced),
                        appODT->RootUid(),
                        appODT->ThemeUid(),
                        aMessage.SecureId().iId,
                        0,
                        0,
                        0,
                        confUid,
                        pluginId,
                        ETrue,
                        KNullDesC(),
                        (TLanguage)(appODT->OdtLanguage())
                        );                                
                    iDefinitionRepository.RegisterNotification( info );
                    }
                }
            else
                {
                // Unlock repository
                iDefinitionRepository.Unlock();
                }
            
            CleanupStack::Pop(&iDefinitionRepository);
            }       
        }
    
    // Cleaning
    if ( pluginODT )
        {
        CleanupStack::PopAndDestroy( pluginODT );
        }    
    if ( appODT )
        {
        CleanupStack::PopAndDestroy( appODT );
        appODT = NULL;
        }

    // Error handling
    iResult->iXuikonError = err;
    if ( !err )
        {
        ret = EhspsReplacePluginSuccess;
        }
    
    // Completion 
    CompleteRequest( ret, messagePtr );
    }

// -----------------------------------------------------------------------------
// ChspsMaintenanceHandler::ReplaceConfigurationL()
// -----------------------------------------------------------------------------
//
TInt ChspsMaintenanceHandler::ReplaceConfigurationL(
        ChspsODT& aAppODT,
        const TInt aPluginId,
        const ChspsODT& aPluginODT )
    {           
    // Input validation
    if ( aAppODT.ConfigurationType() != EhspsAppConfiguration 
            || aAppODT.ThemeUid() < 1
            || aPluginId < 1
            || aPluginODT.ConfigurationType() == EhspsAppConfiguration            
            || aPluginODT.ThemeUid() < 1 )
        {
        return KErrArgument;
        }
    
    TInt err = KErrNotFound;
    
    // Find a plugin node with the provided plugin id    
    ChspsDomNode* pluginNode = hspsServerUtil::FindPluginNodeL( aAppODT, aPluginId);
    if ( pluginNode )
        {
        ChspsDomNode* pluginsNode = pluginNode->Parent();
        
        // Get current position in the plugins list
        TInt currentPosition = FindPluginPosition( *pluginNode );
        if ( currentPosition < 0 )
            {
            User::Leave( KErrGeneral );
            }
        
        // Get configuration id from existing configuration
        ChspsDomNode* oldConfNode = (ChspsDomNode *)pluginNode->ChildNodes().Item( 0 );
        if ( !oldConfNode )
            {
            User::Leave( KErrGeneral );
            }                                        
        ChspsDomList& attrList = oldConfNode->AttributeList();
        ChspsDomAttribute* attr = static_cast<ChspsDomAttribute*>( attrList.FindByName(KConfigurationAttrId) );
        if ( !attr )
           {               
           User::Leave( KErrGeneral );
           }                
        const TInt oldConfId = hspsServerUtil::DecString2Int( attr->Value() );
        oldConfNode = 0;
                
        // Remove old plugin configuration instance and it's resources from the application configuration
        err = RemoveConfigurationL( aAppODT, aPluginId );        
        if ( err )
            {
            User::Leave( KErrGeneral );
            }
        pluginNode = 0;
        
        // Find configuration node from the plugin ODT        
        ChspsDomNode* confNode = aPluginODT.DomDocument().RootNode();
        if ( confNode->Name().CompareF(KConfigurationElement) != 0 )
            {
            User::Leave( KErrGeneral ); 
            }                                       
                                             
        // Add configuration id attribute        
        hspsServerUtil::AddAttributeNumericL( *confNode, KConfigurationAttrId, oldConfId );                               
        
        // Append new plugin configuration node to the application configuration
        err = AppendPluginConfigurationL(                              
                aAppODT, 
                *pluginsNode,
                aPluginODT,
                currentPosition,
                aPluginId );            
        
        if( !err )
            {
            // Add resources of the new plugin into the application configuration
            AddPluginResourcesL( 
                    aAppODT, 
                    aPluginODT.ThemeUid() );
            }            
        }
        
    return err;
    }

TInt ChspsMaintenanceHandler::FindPluginPosition(     
        ChspsDomNode& aPluginNode )
    {
    TInt pos = -1;        
    ChspsDomList& childList = aPluginNode.Parent()->ChildNodes();
    for( TInt nodeIndex=0; nodeIndex < childList.Length(); nodeIndex++ )
        {
        ChspsDomNode* node = (ChspsDomNode *)childList.Item( nodeIndex );
        
        if ( node == &aPluginNode )
            {
            pos = nodeIndex;            
            break;
            }
        }
    return pos;
    }


// -----------------------------------------------------------------------------
// ChspsMaintenanceHandler::SetLogBus()
// -----------------------------------------------------------------------------
//
#ifdef HSPS_LOG_ACTIVE        
void ChspsMaintenanceHandler::SetLogBus( ChspsLogBus* aLogBus )
    {
    iLogBus = aLogBus;
    }
#endif

// -----------------------------------------------------------------------------
// ChspsMaintenanceHandler::IsViewConfiguration()
// -----------------------------------------------------------------------------
//
TBool ChspsMaintenanceHandler::IsViewConfiguration(
        ChspsDomNode& aPluginNode )
    {
    TBool isView = EFalse;
    
    ChspsDomNode* confNode = 
            (ChspsDomNode*)aPluginNode.ChildNodes().FindByName( KConfigurationElement );
    if( confNode )
        {
        ChspsDomAttribute* typeAttr = 
                (ChspsDomAttribute*)confNode->AttributeList().FindByName( KConfigurationAttrType );
        isView = ( typeAttr->Value().CompareF( KConfTypeView ) == 0 );            
        }
    return isView;
    }

// -----------------------------------------------------------------------------
// ChspsMaintenanceHandler::RemoveConfigurationL()
// -----------------------------------------------------------------------------
//
TInt ChspsMaintenanceHandler::RemoveConfigurationL(
        ChspsODT& aAppODT,
        const TInt aPluginId )
    {
    TInt err = KErrNotFound;
    
    // Find a plugin node with the provided id
    ChspsDomNode *pluginNode = hspsServerUtil::FindPluginNodeL( aAppODT, aPluginId );
    if ( pluginNode )
        {
        // Remove the node
        err = RemoveConfigurationL( aAppODT, *pluginNode );
        }
    
    return err;    
    }

// -----------------------------------------------------------------------------
// Removes an plugin instance from the provided application configuration
// -----------------------------------------------------------------------------
//
TInt ChspsMaintenanceHandler::RemoveConfigurationL(
        ChspsODT& aAppODT,
        ChspsDomNode& aPluginNode )
    {
    TInt err = KErrNotFound;
                   
    // Get parent node
    ChspsDomNode *parentNode = aPluginNode.Parent();
    if ( parentNode )
        {                                            
        ChspsDomList& attrList = aPluginNode.AttributeList();
                   
        // Get uid attribute from the node
        TInt pluginUid = 0;            
        ChspsDomAttribute* pluginUidAttr = 
            static_cast<ChspsDomAttribute*> ( attrList.FindByName(KPluginAttrUid) );                                                
        if( !pluginUidAttr )
            {
#ifdef HSPS_LOG_ACTIVE  
            if( iLogBus )
                {
                iLogBus->LogText( _L( "ChspsMaintenanceHandler::RemoveConfigurationL(): - Invalid XML" ) );
                }
#endif
            
            err = KErrGeneral;
            }         
        else            
            {
            // Convert uids from string to numeric format                                        
            const TDesC8& pluginUidValue = pluginUidAttr->Value();                    
            const TUid uid = hspsServerUtil::ConvertDescIntoUid(pluginUidValue);                
            if ( uid.iUid > 0 )
                {                    
                pluginUid = uid.iUid;
                // Override default status
                err = KErrNone;
                }
            }

        // Store activity state for use after deletion.
        TBool pluginWasActive = EFalse;
        ChspsDomAttribute* pluginActivityAttr = 
            static_cast<ChspsDomAttribute*>( attrList.FindByName( KPluginAttrActive ) );                                                
        if( pluginActivityAttr )
            {
            if( pluginActivityAttr->Value().CompareF( KPluginActiveStateActive ) == 0 )
                {
                pluginWasActive = ETrue;
                }
            }
                        
        if ( !err )
            {            
            // If user is removing a view plugin
            if( IsViewConfiguration( aPluginNode ) )
                {
                // first all subplugins need to be removed, so that the application's resource array 
                // (visible in ODT dump) contains only valid resources
                ChspsDomNode* confNode = 
                    (ChspsDomNode*)aPluginNode.ChildNodes().FindByName( KConfigurationElement );
                if( confNode )
                    {
                    ChspsDomNode* controlNode = 
                            (ChspsDomNode*)confNode->ChildNodes().FindByName( KControlElement );
                    if( controlNode )
                        {
                        ChspsDomNode *pluginsNode = 
                                (ChspsDomNode*)controlNode->ChildNodes().FindByName( KPluginsElement );
                        if( pluginsNode )
                            {
                            RPointerArray<ChspsDomNode> nodeArray;
                            CleanupClosePushL( nodeArray );
                            
                            // Get plugin nodes
                            TInt pluginCount = pluginsNode->ChildNodes().Length();
                            for( TInt pluginIndex=0; pluginIndex < pluginCount; pluginIndex++ )                                    
                                {
                                nodeArray.Append( (ChspsDomNode*)pluginsNode->ChildNodes().Item( pluginIndex ) );
                                }                                
                            
                            // Remove the nodes and related resources
                            for( TInt pluginIndex=0; pluginIndex < pluginCount; pluginIndex++ )
                                {
                                RemoveConfigurationL( aAppODT, *nodeArray[pluginIndex ] );
                                }
                            
                            nodeArray.Reset();
                            CleanupStack::PopAndDestroy( 1, &nodeArray );
                            }                                                            
                        }
                    }                
                }
        
            // Get number of plugin instances with the plugin uid
            TInt instanceCount = 0;
            GetPluginInstanceCountL( 
                aAppODT, 
                pluginUid, 
                instanceCount );                                                        
                
            // Remove plugin resources from the application configuration:
            // By default remove all plugin's resources from all instances
            // - otherwise, after upgrades, there might be various versions of the same resources
            err = RemovePluginResourcesL( aAppODT, pluginUid );
            if ( !err )
                {
                // If the application configuration holds other instances of the same plugin                     
                if ( instanceCount > 1 )
                    {
                    // Put back the resources
                    AddPluginResourcesL( 
                            aAppODT,
                            pluginUid );
                    }
                
                // Remove the plugin node from the plugins node    
                parentNode->DeleteChild( &aPluginNode );
                }                
            }

        // If plugin was succesfully deleted and was active ->
        // need set another plugin active.
        // ( Choose to activate topmost item. )
        // ( Use depth of 1 to affect only one level. )
        if ( !err && pluginWasActive )
            {
            const TInt KDepth = 1;
            hspsServerUtil::EditPluginNodeActivityL( parentNode,
                                                     hspsServerUtil::EActivateFirst,
                                                     KDepth );       
            }
        }        

    return err;
    }

//----------------------------------------------------------------------------
// Finds a plugin node with the provided id which is also a children of 
// the provided plugins node 
// ----------------------------------------------------------------------------
//
ChspsDomNode* ChspsMaintenanceHandler::FindChildPluginNode(
        ChspsDomNode& aPluginsNode,
        const TInt aPluginId )
    {
    ChspsDomNode* pluginNode = NULL;
    
    // Loop child nodes
    ChspsDomList& childList = aPluginsNode.ChildNodes();
    for( TInt nodeIndex=0; nodeIndex < childList.Length(); nodeIndex++ )
        {
        // Find a specific node with the provided id value
        ChspsDomNode* node = (ChspsDomNode *)childList.Item( nodeIndex );
        if ( node )
            {
            // Find the ID attribute
            ChspsDomList& attrList = node->AttributeList();
            ChspsDomAttribute* attr = static_cast<ChspsDomAttribute*>( attrList.FindByName(KPluginAttrId) );
            if ( attr )
                {
                TInt id = 0;
                const TDesC8& idValue = attr->Value();
                TLex8 lex( idValue );                                
                if ( lex.Val( id ) == 0 && id == aPluginId )
                    {                                    
                    pluginNode = node;
                    break;
                    }
                }
            }
        }
    
    return pluginNode;
    }

// -----------------------------------------------------------------------------
// Returns a count of plugin instances.
// -----------------------------------------------------------------------------
//
void ChspsMaintenanceHandler::GetPluginInstanceCountL(
        const ChspsODT& aAppODT,        
        const TInt aPluginUid,
        TInt& aInstanceCount )
                
    {        
    aInstanceCount = 0;
    
    ChspsDomDocument& dom = aAppODT.DomDocument();
    ChspsDomDepthIterator* iter = ChspsDomDepthIterator::NewL( *dom.RootNode() );
    CleanupStack::PushL( iter );

    // Find a plugin node with the provided id attribute
    ChspsDomNode* node = iter->First();
    ChspsDomNode* prevNode = NULL;
    TBool jobDone = EFalse;
    while( node && !jobDone && prevNode != node )
        {                
        const TDesC8& name = node->Name();
        
        // Plugin element was found 
        if ( name == KPluginElement )
            {           
            ChspsDomList& attrList = node->AttributeList();                    
            ChspsDomAttribute* attr = static_cast<ChspsDomAttribute*>( attrList.FindByName(KPluginAttrUid) );            
            if ( !attr )
                {
                // Mandatory information is missing for some reason (should be set at installation handler)!
                // Exit with NULL
                jobDone = ETrue;                
                }
            else
                {
                // Convert from (hex?) string into TUid presentation
                const TUid uid = hspsServerUtil::ConvertDescIntoUid( attr->Value() );                            
                if ( aPluginUid == uid.iUid )
                    {
                    aInstanceCount++;
                    }
                }
            }
        
        prevNode = node;        
        node = iter->NextL();        
        }
    CleanupStack::PopAndDestroy( iter );    
    }

// -----------------------------------------------------------------------------
// Removes plugin resources from the provided ODT
// -----------------------------------------------------------------------------
//
TInt ChspsMaintenanceHandler::RemovePluginResourcesL(
        ChspsODT& aAppODT,
        const TInt aPluginUid )        
    {            
    // Loop resources of the application configuration            
    for(TInt aresIndex = 0; aresIndex < aAppODT.ResourceCount(); aresIndex++ )
        {
        ChspsResource& ares = aAppODT.ResourceL( aresIndex );
                                
        // If the plugin resource was found at  resource list of the application configuration                        
        if ( ares.ConfigurationUid() == aPluginUid )
            {
            // Deletes resource from the application configuration
            aAppODT.DeleteResourceL( aresIndex );
            aresIndex--;
            }
        
        }                
    
    return KErrNone;
    }

// -----------------------------------------------------------------------------
// Adds plugin resources to the provided application ODT
// -----------------------------------------------------------------------------
//
void ChspsMaintenanceHandler::AddPluginResourcesL(
        ChspsODT& aAppODT,
        const TInt aPluginUid )
    {    
    // Get plugin ODT
    ChspsODT* pluginODT = ChspsODT::NewL();         
    CleanupStack::PushL( pluginODT );
    TInt ret = iThemeServer.GetConfigurationL( 
            0, 
            aPluginUid,
            *pluginODT );
    
    if ( !ret && pluginODT->ThemeUid() )
        {                   
        // Copy resources of the referenced plug-in instance to the application configuration
        TInt resourceCount = pluginODT->ResourceCount();
        for ( TInt index=0; index < resourceCount; index++ )
            {                    
            ChspsResource& pluginResource = pluginODT->ResourceL(index);
            // Add only those that are located under the sources folder                                            
            if ( pluginResource.FileName().FindF( KSourcesFolder ) > 0
                || pluginResource.FileName().FindF( KLocalesFolder ) > 0 )                               
                {                                                
                ChspsResource* r = pluginResource.CloneL();
                CleanupStack::PushL( r );
                aAppODT.AddResourceL( r ); 
                CleanupStack::Pop( r );
                }
            }
        }
    
    if ( pluginODT )
        {
        CleanupStack::PopAndDestroy( pluginODT );        
        }
    }

// -----------------------------------------------------------------------------
// Service for personalizing settings
// -----------------------------------------------------------------------------
//    
void ChspsMaintenanceHandler::ServiceSetPluginSettingsL( const RMessage2& aMessage )
    {
    // Defaults
    ThspsServiceCompletedMessage ret = EhspsSetPluginSettingsFailed;
    iResult->iXuikonError = KErrNotFound;
    TInt err = KErrNone;
    
    // Parameters
    RMessagePtr2 messagePtr = aMessage;
     
    TBuf8<KMaxHeaderDataLength8> setMaskData;
  
    
    ChspsODT* odt = NULL;   
    TInt appConfUid;
    TInt pluginId;
    TBool storingStatus(EFalse);
    
    // Checking active congiruation and security
    messagePtr.ReadL(1,setMaskData,0);
    
    if (iSetMask)
         {
         delete iSetMask;
         iSetMask = NULL;
         }
    
    iSetMask = ChspsODT::NewL();
    iSetMask->UnMarshalHeaderL( setMaskData );
      
    iCentralRepository.Get( iSetMask->RootUid(), appConfUid );
                         
    // Getting active configuration
    odt = ChspsODT::NewL();
    CleanupStack::PushL(odt);
       
    err = iThemeServer.GetConfigurationL( 
                        iSetMask->RootUid(), 
                        appConfUid,
                        *odt );  
    
    if (iSetMask)
        {
        delete iSetMask;
        iSetMask = NULL;
        }
    
    // Set settings to active configuration
    if( !err  )
        {
        if( iDefinitionRepository.Locked() )
            {
            // Repository locked
            err = KErrAccessDenied;
            }
        else
            {
            // Lock the Plugin Repository (a.k.a. Def.rep)
            iDefinitionRepository.Lock();
            CleanupStack::PushL( TCleanupItem( UnlockRepository, &iDefinitionRepository ) );
            //Read pluginID
            // Get configuration
            ThspsParamSetPluginSettings params;        
            TPckg<ThspsParamSetPluginSettings> packagedStruct(params);    
                
            messagePtr.ReadL( 2, packagedStruct );
                
            pluginId = params.pluginId ;
            storingStatus = params.storingStatus ;
            
            
            
            // read Domdata
            HBufC8* domData = HBufC8::NewL( messagePtr.GetDesLengthL( 3 ) );
            CleanupStack::PushL( domData );
            TPtr8 domPtr = domData->Des();
            messagePtr.ReadL( 3, domPtr, 0 );
            RDesReadStream readBuf( *domData );
            CleanupClosePushL( readBuf );
            
            ChspsDomDocument* domDocument = ChspsDomDocument::NewL( readBuf );
            
            CleanupStack::PopAndDestroy( &readBuf );
            CleanupStack::PopAndDestroy( domData );
            
            CleanupStack::PushL( domDocument );
            err = hspsSetPluginSettingsL( *odt, pluginId, *domDocument );
            
            if( !err )
                {

#ifdef HSPS_LOG_ACTIVE             
                if( odt && iLogBus )
                    {
                    ChspsOdtDump::Dump( *odt, *iLogBus );
                    }
#endif
                // Stores the new application configuration into the repository
                err = iDefinitionRepository.SetOdtL( *odt );               
                
                if( !err )
                    {
#ifdef HSPS_LOG_ACTIVE  
                    if( iLogBus )
                        {
                        iLogBus->LogText( _L( "ChspsMaintenanceHandler::ServiceSetPluginSettingsL(): - Updating Success" ) );
                        }
#endif                    
                    }
                }
            CleanupStack::PopAndDestroy( domDocument );
            
            // Unlock after the changes have been done
            iDefinitionRepository.Unlock();
            CleanupStack::Pop(&iDefinitionRepository);
            
            if( storingStatus )
                {
                // Lock the Plugin Repository (a.k.a. Def.rep)
                iDefinitionRepository.Lock();
                CleanupStack::PushL( TCleanupItem( UnlockRepository, &iDefinitionRepository ) );
                
                ChspsDomNode* pluginNode = hspsServerUtil::FindPluginNodeL( *odt, pluginId );
        
                if( pluginNode )
                    { 
                      
                    ChspsDomList& attrList = pluginNode->AttributeList();
                    // Get configuration attribute from the plugin configuration                                                               
                    ChspsDomAttribute* pluginUidAttr = static_cast<ChspsDomAttribute*> ( attrList.FindByName(KPluginAttrUid) );                                                
                    
                    if( pluginUidAttr )
                        {               
                         
                        // Convert uids from string to numeric format                                        
                        const TDesC8& pluginUidValue = pluginUidAttr->Value();                    
                        const TUid pluginUid = hspsServerUtil::ConvertDescIntoUid(pluginUidValue);
                        // Get plugin configuration                                 
                        ChspsODT* pluginOdt = ChspsODT::NewL();
                        CleanupStack::PushL( pluginOdt );
                          
                        err = iThemeServer.GetConfigurationL( 
                                          0, 
                                          pluginUid.iUid,
                                          *pluginOdt ); 
                          
                        if( !err )
                            {
                            // read Domdata
                             HBufC8* domData = HBufC8::NewL( messagePtr.GetDesLengthL( 3 ) );
                             CleanupStack::PushL( domData );
                             TPtr8 domPtr = domData->Des();
                             messagePtr.ReadL( 3, domPtr, 0 );
                             RDesReadStream readBuf( *domData );
                             CleanupClosePushL( readBuf );
                              
                             ChspsDomDocument* domDocument = ChspsDomDocument::NewL( readBuf );
                              
                             CleanupStack::PopAndDestroy( &readBuf );
                             CleanupStack::PopAndDestroy( domData );
                              
                             CleanupStack::PushL( domDocument );
                              
                             err = hspsSavePluginSettingsL( *pluginOdt, *domDocument );
                                                        
                             if( !err )
                                 {
#ifdef HSPS_LOG_ACTIVE                             
                                 if( odt && iLogBus )
                                      {
                                      ChspsOdtDump::Dump( *pluginOdt, *iLogBus );
                                      }
#endif
                                  // Stores the new application configuration into the repository
                                 err = iDefinitionRepository.SetOdtL( *pluginOdt );               
                                                  
                                 if( !err )
                                     {
#ifdef HSPS_LOG_ACTIVE                  
                                      if( iLogBus )
                                          {
                                          iLogBus->LogText( _L( "ChspsMaintenanceHandler::ServiceSetPluginSettingsL(): - Updating plugin reference Success" ) );
                                          }
#endif                    
                                     }
                                  }                     
                              CleanupStack::PopAndDestroy( domDocument );
                              }
                          if ( pluginOdt )
                              {
                              CleanupStack::PopAndDestroy( pluginOdt );
                              pluginOdt = NULL;
                              }
                                                  
                          }
                      else
                          {
                          err = KErrCorrupt;
                          }
                      }
                  else
                      {
                      err = KErrNotFound;
                      }
                
                // Unlock after the changes have been done
                iDefinitionRepository.Unlock();
                CleanupStack::Pop(&iDefinitionRepository);
                }
           
            if ( !err )
                {
                // Inform clients that the ODT has been modified
                ThspsRepositoryInfo info( 
                   ThspsRepositoryEvent(EhspsSettingsChanged),
                   odt->RootUid(),
                   odt->ThemeUid(),
                   aMessage.SecureId().iId, //=Any file
                   odt->ProviderUid(),
                   0,0,
                   0,pluginId,ETrue,odt->ThemeFullName(),
                   (TLanguage)(odt->OdtLanguage())
                    );                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                          
                iDefinitionRepository.RegisterNotification( info );         
                }
            }     
        }

    // Cleaning
    if (odt)
        {
        CleanupStack::PopAndDestroy( odt );
        odt = NULL;
        }
    
    // Error handling
    iResult->iXuikonError = err;
    if ( !err )
        {       
        ret = EhspsSetPluginSettingsSuccess;
        }
    
    // Completion 
    CompleteRequest( ret, messagePtr );
    }

// -----------------------------------------------------------------------------
//
// Service to get plugin Odt according plugin uid
// packed parameters 
// -----------------------------------------------------------------------------
//
void ChspsMaintenanceHandler::ServiceGetPluginOdtL( const RMessage2& aMessage )
    {
#ifdef HSPS_LOG_ACTIVE  
    if( iLogBus )
        {
        iLogBus->LogText( _L( "ChspsMaintenanceHandler::ServiceGetPluginOdtL: - service called" ) );
        }
#endif
    
    iMessagePtr = aMessage;
    TInt err = KErrNone;

    // Get configuration
    ThspsParamGetPluginOdt params;        
    TPckg<ThspsParamGetPluginOdt> packagedStruct(params);    
    iMessagePtr.ReadL(1, packagedStruct); 
    
    ChspsODT* odt = ChspsODT::NewL();
    CleanupStack::PushL( odt );
    err = iThemeServer.GetConfigurationL( 0, params.pluginUid, *odt );

    if( !err )
        {
        TPath odtPath;
        // Resolve odt path if no errors were occured 
        iDefinitionRepository.GetResourcePathL( *odt, EResourceODT, odtPath );
        TPckg<ThspsParamGetPluginOdt> packagedStruct( params ); 
        iMessagePtr.WriteL(2, odtPath);
        }
#ifdef HSPS_LOG_ACTIVE
    else
        {
        if( iLogBus )
            {
            iLogBus->LogText( _L( "ChspsMaintenanceHandler::ServiceGetPluginOdtL: - error occured during GetConfigurationL()" ) );
            }
        }
#endif            
    
    CleanupStack::PopAndDestroy( odt );

    // Error handling
    iResult->iXuikonError = err;

    if ( err )
        {
#ifdef HSPS_LOG_ACTIVE  
        if( iLogBus )
            {
            iLogBus->LogText( _L( "ChspsMaintenanceHandler::ServiceGetPluginOdtL: - EhspsGetPluginOdtFailed code: %d" ), err );
            }
#endif
        
        CompleteRequest( EhspsGetPluginOdtFailed, iMessagePtr );
        }
    else
        {
#ifdef HSPS_LOG_ACTIVE  
        if( iLogBus )
            {
            iLogBus->LogText( _L( "ChspsMaintenanceHandler::ServiceGetPluginOdtL: - EhspsGetPluginOdtSuccess" ) );
            }
#endif

        CompleteRequest( EhspsGetPluginOdtSuccess, iMessagePtr );
        }
        
    } 


// -----------------------------------------------------------------------------
// hspsMaintenanceHandler::hspsSetPluginSettings

// -----------------------------------------------------------------------------
//
TInt ChspsMaintenanceHandler::hspsSetPluginSettingsL(  ChspsODT& aOdt,
                               TInt aPluginId,  ChspsDomDocument&  aDom )
    {
    TInt error = KErrNotFound;
    
    
    TUint value = aPluginId;
   
        // Find a plugin node with the provided id
        ChspsDomNode* pluginNode = hspsServerUtil::FindPluginNodeL( aOdt, value );
        if( !pluginNode )
            {
            error =  KErrNotFound;
            return error;
            }
        ChspsDomNode* controlNode = &(FindNodeByTagL(KControlElement, *pluginNode )); 
        if( !controlNode )
            {
            error =  KErrNotFound;
            return error;
            }        
        ChspsDomList& childList = controlNode->ChildNodes();
        ChspsDomNode* settingsNode = (ChspsDomNode*)childList.FindByName( KSettingsElement );
        if( !settingsNode )
            {
            error =  KErrNotFound;
            return error;
            }
        TInt index = controlNode->ItemIndex( *settingsNode );
        controlNode->DeleteChild(settingsNode);
       
        ChspsDomDepthIterator* iter = ChspsDomDepthIterator::NewL( *aDom.RootNode() );
        CleanupStack::PushL( iter );                                
        ChspsDomNode* node = iter->First();
        ChspsDomNode* prevNode = NULL;
        TBool jobDone = EFalse;
        while( node && !jobDone && node != prevNode)
            {
            const TDesC8& name = node->Name();
            if( name == KSettingsElement )
                {
                // Attach to the plugin node
                
                ChspsDomNode* clone = node->CloneL( aOdt.DomDocument().StringPool() );
                CleanupStack::PushL( clone  );
                controlNode->AddChildL( clone, index );
                clone->SetParent( controlNode );
                CleanupStack::Pop( clone );
                jobDone=ETrue;
                error=KErrNone;
                }
            prevNode=node;
            node=iter->NextL();
            }
        CleanupStack::PopAndDestroy( iter ); 
        
    
         
    return error;
   
    }

// -----------------------------------------------------------------------------
// Finds a node from a dom document.
// Looks for the next node tag.
// -----------------------------------------------------------------------------
ChspsDomNode& ChspsMaintenanceHandler::FindNodeByTagL( 
        const TDesC8& aNodeTag,
        ChspsDomNode& aDomNode )
    {
    ChspsDomDepthIterator* iter = ChspsDomDepthIterator::NewL( aDomNode );
    CleanupStack::PushL( iter );
    ChspsDomNode* targetNode( NULL );
    ChspsDomNode* node = iter->First();
    TBool found = EFalse;
    while( !found && node )
        {
        const TDesC8& name = node->Name();
        if ( name.Compare( aNodeTag ) == 0 )
            {  
            found = ETrue;
            targetNode = node;
            }
        node = iter->NextL();
        }   
    CleanupStack::PopAndDestroy( iter );
    return *targetNode;
    }        

// -----------------------------------------------------------------------------
// ChspsMaintenanceHandler::hspsSavePluginSettingsL

// -----------------------------------------------------------------------------
//
TInt ChspsMaintenanceHandler::hspsSavePluginSettingsL(  
        ChspsODT& aOdt,
        ChspsDomDocument&  aDom )
    {
    ChspsDomNode* node =  aOdt.DomDocument().RootNode();
    ChspsDomNode* controlNode = &(FindNodeByTagL(KControlElement, *node )); 
        
    if( !controlNode )    
        {
        return KErrNotFound;
        }
    
    ChspsDomList& childList = controlNode->ChildNodes();
    ChspsDomNode* settingsNode = (ChspsDomNode*)childList.FindByName( KSettingsElement );
    
    if( !settingsNode )
        {
        return KErrNotFound;
        }

    TInt index = controlNode->ItemIndex( *settingsNode );
    controlNode->DeleteChild(settingsNode);
   
    ChspsDomDepthIterator* iter = ChspsDomDepthIterator::NewL( *aDom.RootNode() );
    CleanupStack::PushL( iter );                                
    ChspsDomNode* iterNode = iter->First();
    ChspsDomNode* prevNode = NULL;
    TBool jobDone = EFalse;

    TInt error( KErrNotFound );

    while( iterNode && !jobDone && iterNode != prevNode)
        {
        const TDesC8& name = iterNode->Name();
        if( name == KSettingsElement )
            {
            // Attach to the plugin node
            
            ChspsDomNode* clone = iterNode->CloneL( aOdt.DomDocument().StringPool() );
            CleanupStack::PushL( clone  );
            controlNode->AddChildL( clone, index );
            clone->SetParent( controlNode );
            CleanupStack::Pop( clone );
            jobDone=ETrue;
            error=KErrNone;
            }
        prevNode=iterNode;
        iterNode=iter->NextL();
        }
    CleanupStack::PopAndDestroy( iter ); 

    return error;
    }

// Service for updating plugin positions
// -----------------------------------------------------------------------------
//
void ChspsMaintenanceHandler::ServiceMovePluginsL( const RMessage2& aMessage )
    {
    // Defaults
    ThspsServiceCompletedMessage ret = EhspsMovePluginsFailed;
    iResult->iXuikonError = KErrNotFound;
    TInt err = KErrNone;
    
    // Parameters
    RMessagePtr2 messagePtr = aMessage;
    
    // IPC slots: 
    // #0) output: externalized ChspsResult for error handling
    // #1) input: a struct
    
    // Get service parameters from IPC slot #1
    ThpsParamMovePlugins params;        
    TPckg<ThpsParamMovePlugins> packagedStruct(params);    
    aMessage.ReadL(1, packagedStruct);                        
    const TInt appUid = params.appUid;
    const TInt confId = params.configurationId;        
    TPtrC8 bufPtr( params.pluginIdsBuf );
    
    // Internalize the plugin ids array from a descriptor
    const TInt KGranularity = 6;
    ChspsPluginIdList* idArray = new (ELeave)ChspsPluginIdList( KGranularity );
    CleanupStack::PushL( idArray );
    RDesReadStream readStream( bufPtr );
    CleanupClosePushL( readStream );
    TRAP( err, idArray->InternalizeL(readStream) );        
    CleanupStack::PopAndDestroy( &readStream );

    if ( err || appUid < 1 || confId < 1 || idArray->Count() < 1 )
        {
#ifdef HSPS_LOG_ACTIVE  
        if( iLogBus )
            {
            iLogBus->LogText( _L( "ChspsMaintenanceHandler::ServiceMovePluginsL(): invalid input" ) );
            }
#endif
        
        err = KErrArgument;
        }

    // Application configuration
    ChspsODT *appODT = NULL;
    if ( !err )
        {    
        // Get active application configuration from the central repository
        TInt appConfUid;       
        err = iCentralRepository.Get( appUid, appConfUid );
        if ( err || appConfUid < 1 )
            {
            err = KErrNotFound;         
            }
        else
            {
            appODT = ChspsODT::NewL();
            CleanupStack::PushL( appODT );
            err = iThemeServer.GetConfigurationL( 
                    appUid, 
                    appConfUid,
                    *appODT );                                        
            }
        }

    // Modifications    
    if ( !err )
       {       
           if ( iDefinitionRepository.Locked() )
               {
            // Repository locked
               err = KErrAccessDenied;
               }
           else
               {
               // Lock the Plugin Repository (a.k.a. Def.rep)
               iDefinitionRepository.Lock();                                
            CleanupStack::PushL( TCleanupItem( UnlockRepository, &iDefinitionRepository ) );
            
            // Update the provided configuration  
            err = UpdatePluginListL( 
                    *appODT, 
                    confId, 
                    *idArray );                
            if ( !err )            
                {

#ifdef HSPS_LOG_ACTIVE                
                if( appODT && iLogBus )
                    {
                    iLogBus->LogText( _L( "ChspsMaintenanceHandler::ServiceRemovePluginL() - plugins moved:" ) );
                    ChspsOdtDump::Dump( *appODT, *iLogBus );
                    }
#endif
                // Stores the new application configuration into the repository
                err = iDefinitionRepository.SetOdtL( *appODT );
                if ( err )
                    {
#ifdef HSPS_LOG_ACTIVE  
                    if( iLogBus )
                        {
                        iLogBus->LogText( _L( "ChspsMaintenanceHandler::ServiceRemovePluginL(): - Updating failed" ) );
                        }
#endif                    
                    }
                }
            else
                {
#ifdef HSPS_LOG_ACTIVE  
                if( iLogBus )
                    {
                    iLogBus->LogText( _L( "ChspsMaintenanceHandler::ServiceRemovePluginL(): - UpdatePluginListL failed" ) );
                    }
#endif
                }
            
            // Unlock after the changes have been done
            iDefinitionRepository.Unlock();
            CleanupStack::Pop(&iDefinitionRepository);

            if ( !err )
                {
                // Inform clients that the ODT has been modified
                ThspsRepositoryInfo info( 
                    ThspsRepositoryEvent(EhspsODTModified),
                    appODT->RootUid(),
                    appODT->ThemeUid(),
                    0, //=Any file
                    appODT->ProviderUid(),
                    (TLanguage)(appODT->OdtLanguage())
                    );                                
                iDefinitionRepository.RegisterNotification( info );
                }                                        
               }       
       }    
    
    // Cleaning
    if ( appODT )
        {
        CleanupStack::PopAndDestroy( appODT );
        appODT = NULL;
        }
    
    CleanupStack::PopAndDestroy( idArray );
        
    // Error handling
    iResult->iXuikonError = err;
    if ( !err )
        {        
        ret = EhspsMovePluginsSuccess;
        }
    
    // Completion 
    CompleteRequest( ret, messagePtr );    
    }

// -----------------------------------------------------------------------------
// Service for updating configuration state
// -----------------------------------------------------------------------------
//
void ChspsMaintenanceHandler::ServiceSetConfStateL( const RMessage2& aMessage )
    {
    // Parameters
    RMessagePtr2 messagePtr = aMessage;
    
    // Get service parameters from IPC slot #1
    ThspsParamSetConfState params;        
    TPckg<ThspsParamSetConfState> packagedStruct( params );    
    aMessage.ReadL( 1, packagedStruct );                      
       
    // Reserve definition repository
    if ( iDefinitionRepository.Locked() )
        {
        // Definition repository reserved
        User::Leave( KErrAccessDenied );
        }
    iDefinitionRepository.Lock();                               
    CleanupStack::PushL( TCleanupItem( UnlockRepository, &iDefinitionRepository ) );

    // Get application configuration
    TInt appConfUid;       
    User::LeaveIfError( iCentralRepository.Get( params.appUid, appConfUid ) );                    
    ChspsODT* appODT = ChspsODT::NewL();
    CleanupStack::PushL( appODT );
    User::LeaveIfError( iThemeServer.GetConfigurationL( 
        params.appUid, 
        appConfUid,
        *appODT ) );                                      
    
    // Update configuration state
    SetConfStateL( *appODT, params.confId, params.state, params.filter );
    
    // Store updated configuration
    User::LeaveIfError( iDefinitionRepository.SetOdtL( *appODT ) );
    CleanupStack::PopAndDestroy( appODT );
    
    // Release definition repository
    iDefinitionRepository.Unlock();
    CleanupStack::Pop( &iDefinitionRepository );
    
    // Completion 
    CompleteRequest( EhspsSetConfStateSuccess, messagePtr ); 
    }

// -----------------------------------------------------------------------------
// Service for restoring active application configuration
// -----------------------------------------------------------------------------
//
void ChspsMaintenanceHandler::ServiceRestoreActiveAppConfL( const RMessage2& aMessage )
    {
    // Parameters
    RMessagePtr2 messagePtr = aMessage;
    ThspsServiceCompletedMessage ret( EhspsRestoreActiveAppConfFailed );
    TInt err( KErrNone );
    
    // Get service parameters from IPC slot #1
    ThspsParamRestoreActiveAppConf params;        
    TPckg<ThspsParamRestoreActiveAppConf> packagedStruct( params );    
    aMessage.ReadL( 1, packagedStruct );                      
    
    // Create search criteria
    ChspsODT* searchMask = ChspsODT::NewL();
    CleanupStack::PushL( searchMask );
    searchMask->SetRootUid( params.appUid );
    searchMask->SetThemeUid( params.confUid );

    // Get configuration header
    ChspsODT* confHeader( NULL );
    TInt pos( 0 );
    iThemeServer.GetConfigurationHeader( *searchMask, confHeader, pos );

    if ( confHeader &&
         !iDefinitionRepository.Locked() )
        {            
        // Lock definition repository
        iDefinitionRepository.Lock();                               
        // Restore backup configuration if it exists
        err = iDefinitionRepository.RestoreBackupConfiguration( *confHeader );
        // Release definition repository
        iDefinitionRepository.Unlock();

        if ( err != KErrNone )
            {
            // Configuration backup not found - Restore default configuration
            ChspsODT* odt = ChspsODT::NewL();
            CleanupStack::PushL( odt );
            RestoreDefaultAppConfL( confHeader, *odt );
            iDefinitionRepository.SetOdtL( *odt );
            CleanupStack::PopAndDestroy( odt );
            }
        ret = EhspsRestoreActiveAppConfSuccess;
        }
    
    CleanupStack::PopAndDestroy( searchMask );

    // Completion 
    CompleteRequest( ret, messagePtr ); 
    }

//----------------------------------------------------------------------------
// ChspsMaintenanceHandler::UpdatePluginListL()
// ----------------------------------------------------------------------------
//
TInt ChspsMaintenanceHandler::UpdatePluginListL(
        ChspsODT& aAppODT,
        const TInt aConfigurationId, 
        const CArrayFixFlat<TInt>& aPluginIdList )       
    {
    TInt err = KErrNone;
    
    // Find the configuration node
    ChspsDomNode* confNode = hspsServerUtil::FindConfigurationNodeL( aAppODT, aConfigurationId );
    if ( !confNode )
        {        
#ifdef HSPS_LOG_ACTIVE  
        if( iLogBus )
            {
            iLogBus->LogText( _L( "ChspsMaintenanceHandler::UpdatePluginListL(): - configuration node was not found" ) );
            }
#endif
        
        err = KErrNotFound;
        }
    
    ChspsDomNode* controlNode = NULL;
    if ( !err )
        {
        // Find a control node under the configuration node
        controlNode = (ChspsDomNode *)confNode->ChildNodes().FindByName( KControlElement );
        if ( !controlNode )
            {            
#ifdef HSPS_LOG_ACTIVE  
            if( iLogBus )
                {
                iLogBus->LogText( _L( "ChspsMaintenanceHandler::UpdatePluginListL(): - control node was not found" ) );
                }
#endif
            
            err = KErrNotFound;
            }    
        }
    
    ChspsDomNode* oldPluginsNode = NULL;
    if ( !err )
        {
        // Find a plugins node under the control node
        oldPluginsNode = (ChspsDomNode *)controlNode->ChildNodes().FindByName( KPluginsElement );
        if ( !oldPluginsNode )
            {    
#ifdef HSPS_LOG_ACTIVE  
            if( iLogBus )
                {
                iLogBus->LogText( _L( "ChspsMaintenanceHandler::UpdatePluginListL(): - plugin node was not found" ) );
                }
#endif            
            err = KErrNotFound;
            }
        else
            {
#ifdef HSPS_LOG_ACTIVE  
            if( iLogBus )
                {
                iLogBus->LogText( _L( "ChspsMaintenanceHandler::UpdatePluginListL(): - old plugins node has %d childs" ),
                        oldPluginsNode->ChildNodes().Length() );
                }
#endif            
            }
        }
    
    const TInt nodeCount = aPluginIdList.Count();
    if ( !err )
        {
        // Plugin node count and provided id count should match        
        if ( nodeCount < 1 || nodeCount != oldPluginsNode->ChildNodes().Length() )
            {
            err = KErrArgument;
            }
        }
    
    // Find all plugin nodes under the plugins node in the requested sequence
    CArrayFixFlat<ChspsDomNode*>* nodeArray = NULL;
    if ( !err )
        {
        nodeArray = new (ELeave)CArrayFixFlat<ChspsDomNode*>( nodeCount );
        CleanupStack::PushL( nodeArray );
        ChspsDomNode* node = NULL;
        for( TInt nodeIndex=0; nodeIndex < nodeCount; nodeIndex++ )        
            {                        
            const TInt pluginId = aPluginIdList.At(nodeIndex); 
            node = FindChildPluginNode( 
                *oldPluginsNode, 
                pluginId );
            if ( !node )
                {
#ifdef HSPS_LOG_ACTIVE  
                if( iLogBus )
                    {
                    iLogBus->LogText( _L( "ChspsMaintenanceHandler::UpdatePluginListL(): - FindChildPluginNode failure" ) );
                    }
#endif
                
                err = KErrNotFound;
                break;
                }
            nodeArray->AppendL( node );

#ifdef HSPS_LOG_ACTIVE  
            if( iLogBus )
                {
                iLogBus->LogText( _L( "ChspsMaintenanceHandler::UpdatePluginListL(): - plugin id=%d" ),
                        pluginId );
                }
#endif            
            }
        }
    
    ChspsDomNode* newPluginsNode = NULL;
    if ( !err )
        {            
        // Clone the plugins node
        newPluginsNode = oldPluginsNode->CloneWithoutKidsL( oldPluginsNode->StringPool() );
        CleanupStack::PushL( newPluginsNode );
    
        // Add the cloned plugins node into the control node (removal of the old plugins node is done later)        
        const TInt KNewPosition = 0;
        controlNode->ChildNodes().AddItemL( newPluginsNode, KNewPosition );        
        newPluginsNode->SetParent( controlNode );
            
        // Transfer ownership of the plugin nodes
        const TInt arrayCount = nodeArray->Count();
        for( TInt arrayIndex=0; arrayIndex < arrayCount; arrayIndex++)
            {        
            ChspsDomNode* pluginNode = nodeArray->At(arrayIndex);
            if ( !pluginNode )
                {
                err = KErrGeneral;
                break;
                }
            newPluginsNode->AddChildL( pluginNode );        
            pluginNode->SetParent( newPluginsNode );                    
            }    
        }
    
    if ( !err )
        {
        // Remove plugin nodes from the old plugins node
        const TInt remainingCount = oldPluginsNode->ChildNodes().Length();
        for( TInt nodeIndex=0; nodeIndex < remainingCount; nodeIndex++)
            {
            ChspsDomNode* node = (ChspsDomNode*)oldPluginsNode->ChildNodes().Item( 0 );    
            if ( node )
                {
                oldPluginsNode->ChildNodes().RemoveItem( node );
                }
            }                
        if ( oldPluginsNode->ChildNodes().Length() != 0 )
            {
            err = KErrGeneral;
            }        
        }

    if ( !err )
        {
        // Remove the old plugins node from the control node
        controlNode->ChildNodes().RemoveItem( oldPluginsNode );
        delete oldPluginsNode;
        oldPluginsNode = NULL;        
        }
    
    if ( newPluginsNode )
        {
        CleanupStack::Pop( newPluginsNode );
        }
    if ( nodeArray )
        {
        CleanupStack::PopAndDestroy( nodeArray );
        }
    
    return err;
    }

//----------------------------------------------------------------------------
// ChspsMaintenanceHandler::SetConfStateL()
// ----------------------------------------------------------------------------
//
void ChspsMaintenanceHandler::SetConfStateL(
    ChspsODT& aAppODT,      
    TInt aConfId,
    ThspsConfigurationState aState,
    ThspsConfStateChangeFilter aFilter )     
    {

    // Get state attribute value
    TPtrC8 state;
    switch ( aState )
        {
        case EhspsConfStateNotConfirmed:
            state.Set( KConfStateNotConfirmed );
            break;
        case EhspsConfStateWaitForConfirmation:
            state.Set( KConfStateWaitForConfirmation );
            break;
        case EhspsConfStateConfirmed:
            state.Set( KConfStateConfirmed );
            break;
        default:
            state.Set( KConfStateError );
            break;
        }
    
    // Find a configuration node with an id attribute that matches the 
    // provided id and update found node state attribute
    // If aConfId is -1 all configuration nodes are updated
    ChspsDomDocument& dom = aAppODT.DomDocument();
    ChspsDomDepthIterator* iter = ChspsDomDepthIterator::NewL( *dom.RootNode() );
    CleanupStack::PushL( iter );
    ChspsDomNode* node = iter->First();
    ChspsDomNode* prevNode = NULL;
    TBool jobDone = EFalse;
    while( node && !jobDone && prevNode != node )    
        {                
        const TDesC8& name = node->Name();
        
        // Configuration node is found 
        if ( name == KConfigurationElement )
            {
            if ( aConfId != -1 )
                {
                ChspsDomList& attrList = node->AttributeList();                    
                ChspsDomAttribute* idAttr = static_cast<ChspsDomAttribute*>( 
                    attrList.FindByName( KConfigurationAttrId ) );            
                if ( !idAttr )
                    {
                    User::Leave( KErrNotFound );
                    }
                TInt id(0);            
                const TDesC8& idValue = idAttr->Value();                        
                TLex8 lex( idValue );                        
                lex.Val( id );
                TBuf8<10> confId;
                confId.Num( aConfId );
                if ( aConfId == id && aFilter == EhspsConfStateChangeNoFilter )
                    {
                    hspsServerUtil::AddAttributeDescL( 
                        *node, 
                        KConfigurationAttrState, 
                        state );
                    jobDone = ETrue;
                    }
                else if ( aFilter == EhspsConfStateChangePlugins && 
                        hspsServerUtil::GetParentNode( 
                            *node,
                            KConfigurationElement,
                            KConfigurationAttrId,
                            confId ) )
                    {
                    hspsServerUtil::AddAttributeDescL( 
                        *node, 
                        KConfigurationAttrState, 
                        state );
                    }
                }
            else
                {
                hspsServerUtil::AddAttributeDescL( 
                    *node,
                    KConfigurationAttrState,
                    state );
                }
            }
        
        prevNode = node;        
        node = iter->NextL();
        }
    CleanupStack::PopAndDestroy( iter );

    }

// -----------------------------------------------------------------------------
// ChspsMaintenanceService::hspsGetListHeadersL
// Gets the header list to aHeaderDataList
// (other items were commented in a header).
// -----------------------------------------------------------------------------
//
ThspsServiceCompletedMessage ChspsMaintenanceHandler::hspsGetListHeaders(const TDesC8& 
                                        /*aSearchMaskData*/, CArrayPtrSeg<HBufC8>& aHeaderDataList) 
    {
    // set the default response
    ThspsServiceCompletedMessage ret = EhspsGetListHeadersEmpty;
    TInt errorCode;
    TRAP( errorCode, GetHeaderListL( aHeaderDataList, *iSearchMask ) );
     if (errorCode)
        {
        ret = EhspsGetListHeadersFailed;
        }
    else
        {
        ret = EhspsGetListHeadersSuccess;
        }
     
    return ret; 
    }
    
// -----------------------------------------------------------------------------
// ChspsMaintenanceHandler::hspsGetNextHeader()
// Gets the header list to aHeaderDataList
// (other items were commented in a header).
// -----------------------------------------------------------------------------
//
ThspsServiceCompletedMessage ChspsMaintenanceHandler::hspsGetNextHeader()
    {
    return EhspsServiceNotSupported; 
     }
    
// -----------------------------------------------------------------------------
// ChspsMaintenanceHandler::hspsSetActiveTheme
// Sets the active theme to central repository.
// (other items were commented in a header).
// -----------------------------------------------------------------------------
//
ThspsServiceCompletedMessage ChspsMaintenanceHandler::hspsSetActiveTheme(const ChspsODT& aSetMask, 
                                                                                  ChspsODT& aOdt)
    {
    // set the default response
    ThspsServiceCompletedMessage ret = EhspsSetActiveThemeFailed;
    TInt errorCode = KErrNone;

#ifdef HSPS_LOG_ACTIVE  
    if( iLogBus )
        {
        iLogBus->LogText( _L( "ChspsMaintenanceHandler::hspsSetActiveTheme(): - requested for AppUid = %d, ThemeUid= %d." ),
                aSetMask.RootUid(),
                aSetMask.ThemeUid() );
        }
#endif
       
    TRAP( errorCode, errorCode = iThemeServer.ActivateThemeL( aSetMask, aOdt ));
    if ( errorCode )
           {
#ifdef HSPS_LOG_ACTIVE  
         if( iLogBus )
             {
             iLogBus->LogText( _L( "ChspsMaintenanceHandler::hspsSetActiveTheme(): - theme activation failed - bad ODT!" ) );
             }
#endif
        
         ret = EhspsSetActiveThemeFailed;    
            }
    else
          {
#ifdef HSPS_LOG_ACTIVE  
        if( iLogBus )
            {
            iLogBus->LogText( _L( "ChspsMaintenanceHandler::hspsSetActiveTheme(): - activated for AppUid = %d, ThemeUid= %d." ),
                    aOdt.RootUid(),
                    aOdt.ThemeUid() );
            }
#endif
        
          ret = EhspsSetActiveThemeSuccess;
        }
       return ret;
    }    

// -----------------------------------------------------------------------------
// ChspsMaintenanceHandler::hspsRestoreDefault
// Restores the default theme and returns either EhspsRestoreDefaultSuccess or 
// EhspsRestoreDefaultFailed.
// (other items were commented in a header).
// -----------------------------------------------------------------------------
//
ThspsServiceCompletedMessage ChspsMaintenanceHandler::hspsRestoreDefault( const ChspsODT& aSetMask,
                                                                                ChspsODT& aHeader )
    {
    // set the default response
    ThspsServiceCompletedMessage ret = EhspsRestoreDefaultFailed;
    TInt errorCode = 0;
    TRAP( errorCode, RestoredDefaultL( aSetMask, aHeader ) );
    if ( errorCode )
        {
        ret = EhspsRestoreDefaultFailed;    
         }
    else
        {
        ret = EhspsRestoreDefaultSuccess;
        }
    return ret;
    }    


// -----------------------------------------------------------------------------
// ChspsMaintenanceHandler::hspsRemoveThemeL
// Removes the theme by calling the appropriate method in definition repository.
// (other items were commented in a header).
// -----------------------------------------------------------------------------
//
ThspsServiceCompletedMessage ChspsMaintenanceHandler::hspsRemoveThemeL( const ChspsODT& aSetMask )
    {   
    ThspsServiceCompletedMessage ret( EhspsRemoveThemeFailed );

    if( !( aSetMask.Flags() & EhspsThemeStatusLicenceeDefault ) )                      
        {       
        RArray<ThspsRepositoryInfo> notifParams;
        
        if( !iDefinitionRepository.Locked() )
            {
            iDefinitionRepository.Lock();
                                                                       
            // In case of error. repository is unlocked    
            CleanupStack::PushL( TCleanupItem( UnlockRepository, &iDefinitionRepository ) );
                                                                                                       
            // Check what is being being uninstalled
            TInt error( KErrNone );
            if ( aSetMask.ConfigurationType() != EhspsAppConfiguration )
                {
                // Fix plugin instances and get notifications from valid cases
                TRAP( error, RemovePluginFromAppConfsL( aSetMask, notifParams ) );                    
                }                                
            
            if( !error )
                {
                // Remove the actual plugin from file system
                TRAP( error, RemoveThemeL( aSetMask ) );                
                }
                                               
            if( !error )
                {
                // Remove header from the cache
                iThemeServer.UpdateHeaderListCache(
                        EhspsCacheRemoveHeader,
                        aSetMask.RootUid(),
                        aSetMask.ProviderUid(),
                        aSetMask.ThemeUid() );
                }
            
            // Must be done before the notifications are sent
            iDefinitionRepository.Unlock();
            CleanupStack::Pop( &iDefinitionRepository );
            
            if ( !error )
                {
                // Send the notifications which will update the client UIs
                for( TInt i=0; i < notifParams.Count(); i++ )
                    {                                        
                    iDefinitionRepository.RegisterNotification( notifParams[i] );
                    }
                
                ret = EhspsRemoveThemeSuccess;                
                }                                                             
            }
        
        notifParams.Close();
        }
         
     return ret;
    }    

// -----------------------------------------------------------------------------
// RemovePluginFromAppConfsL
// -----------------------------------------------------------------------------
//
void ChspsMaintenanceHandler::RemovePluginFromAppConfsL( 
        const ChspsODT& aOdt,
        RArray<ThspsRepositoryInfo>& aNotificationParams )        
    {        
    __ASSERT_DEBUG( aOdt.ThemeUid(), User::Leave( KErrArgument ) );
           
    // Loop application configurations
    const TInt count = iHeaderListCache.Count();                  
    for ( TInt i = 0; i < count; i++ )
        {
        ChspsODT* header = iHeaderListCache.At(i);
        if ( header && header->ConfigurationType() == EhspsAppConfiguration )
            {                              
            // Get an ODT from the looped application configuration header                
            ChspsODT* appOdt = ChspsODT::NewL();
            CleanupStack::PushL( appOdt );
            
            // Fill only those values which are required by the GetOdtL call
            appOdt->SetRootUid( header->RootUid() );
            appOdt->SetProviderUid( header->ProviderUid() );
            appOdt->SetThemeUid( header->ThemeUid() );
            appOdt->SetProviderNameL( header->ProviderName() );
            appOdt->SetThemeShortNameL( header->ThemeShortName() );
            appOdt->SetThemeVersionL( header->ThemeVersion() );        
            User::LeaveIfError( iDefinitionRepository.GetOdtL( *appOdt ) );
             
            // Get active application configuration for the client in question
            TInt activeAppConfUid = 0;
            iCentralRepository.Get( appOdt->RootUid(), activeAppConfUid );
            
            // Get plugin id's from the instances in the application configuration
            RArray<TInt> pluginIds;   
            CleanupClosePushL( pluginIds );
            
            hspsServerUtil::GetPluginIdsByUidL( *appOdt,
                                                TUid::Uid( aOdt.ThemeUid() ),
                                                pluginIds  );
                                        
            // If there were plugin instances in an application configuration
            if ( pluginIds.Count() > 0 )
                {           
                                
                // If the application configuration is inactive
                if ( activeAppConfUid != appOdt->ThemeUid() )
                    {
                    // Prevent notifications and set state of the plugin instances to "uninstalled",
                    // AI3 will remove instances (or replaces them with an empty widget) 
                    // when it loads the configuration after it has first been activated
                    TBool updatesDone = InvalidateUninstalledPluginInstancesL( 
                            *appOdt,    
                            aOdt.ThemeUid(),
                            pluginIds );                    
#ifdef HSPS_LOG_ACTIVE  
                    if( iLogBus )
                        {
                        if ( updatesDone )
                            {
                            iLogBus->LogText( 
                                _L( "ChspsMaintenanceHandler::RemovePluginFromAppConfsL(): old plugin instances were updated in inactive root configuration" ) 
                                );
                            }                        
                        else
                            {
                            iLogBus->LogText( 
                                _L( "ChspsMaintenanceHandler::RemovePluginFromAppConfsL(): nothing was done" ) 
                                );
                            }
                        }                            
#endif                        
                    }
                else
                    {                    
                    // Notify active application configuration that old plugin instances need to be replaced                    
                    TBool lastNotification = EFalse;
                    for( TInt i = 0; i < pluginIds.Count(); i++ )
                       {
                       if( i == pluginIds.Count() - 1 )
                          {
                          lastNotification = ETrue;
                          }
                       ThspsRepositoryInfo info( 
                            ThspsRepositoryEvent( EhspsClean ),
                            appOdt->RootUid(),
                            appOdt->ThemeUid(),
                            0, //=Any file
                            appOdt->ProviderUid(),
                            aOdt.RootUid(),
                            aOdt.ProviderUid(),
                            aOdt.ThemeUid(),
                            pluginIds[i], 
                            lastNotification,
                            aOdt.ThemeFullName(),
                            (TLanguage)( aOdt.OdtLanguage() ) );
                       aNotificationParams.Append(info);
                       }
#ifdef HSPS_LOG_ACTIVE  
                    if( iLogBus )
                        {
                        iLogBus->LogText( 
                            _L( "ChspsMaintenanceHandler::RemovePluginFromAppConfsL(): plugin uninstalled notifications sent to SAPI for all instances" ) 
                            );
                        }
#endif                                               
                    }
                } // instance count > 0    
            else
                {                
                if ( activeAppConfUid == appOdt->ThemeUid() )
                    {
                    // Notify active application configuration which had no plugin instances
                    ThspsRepositoryInfo info( 
                        ThspsRepositoryEvent( EhspsClean ),
                        appOdt->RootUid(),
                        appOdt->ThemeUid(),
                        0, //=Any file
                        appOdt->ProviderUid(),
                        aOdt.RootUid(),
                        aOdt.ProviderUid(),
                        aOdt.ThemeUid(),
                        0, 
                        ETrue,
                        aOdt.ThemeFullName(),
                        (TLanguage)( aOdt.OdtLanguage() ) );
                    aNotificationParams.Append(info);
                    }
                }
            
            pluginIds.Close();
            CleanupStack::PopAndDestroy( 2, appOdt ); // appOdt, pluginIds                         
            appOdt = NULL;             
            
            } // app configuration
        
        } // header loop
       
    }

// -----------------------------------------------------------------------------
// ChspsMaintenanceHandler::InvalidateUninstalledPluginInstancesL
// -----------------------------------------------------------------------------
TBool ChspsMaintenanceHandler::InvalidateUninstalledPluginInstancesL(
        ChspsODT& aAppODT,    
        const TInt aPluginUid,
        const RArray<TInt>& aPluginIds )
    {            
    // Should be already locked by hspsRemoveThemeL()
    __ASSERT_DEBUG( iDefinitionRepository.Locked(), User::Leave( KErrGeneral ) );
    
#ifdef HSPS_LOG_ACTIVE  
    if( iLogBus )
        {        
        iLogBus->LogText( _L( "ChspsMaintenanceHandler::InvalidateUninstalledPluginInstancesL(): - Before Error state updates:") );
        ChspsOdtDump::Dump( aAppODT, *iLogBus );
        }
#endif
    
    TInt processedCount = 0;
    for( TInt idIndex = 0; idIndex < aPluginIds.Count(); idIndex++ )
        {        
        const TInt pluginId( aPluginIds[idIndex] );
        
        // If full DOM/application configuration hasn't been generated yet (plugin IDs are unset)
        if ( pluginId > 0 )
            {
            ChspsDomNode *pluginNode = hspsServerUtil::FindPluginNodeL( aAppODT, pluginId );
            __ASSERT_DEBUG( pluginNode, User::Leave( KErrArgument) );
            if ( pluginNode )
                {
                ChspsDomNode* confNode = (ChspsDomNode *)pluginNode->ChildNodes().Item( 0 );
                __ASSERT_DEBUG( confNode, User::Leave( KErrArgument) );
                if ( confNode )
                    {
                    // Indicate that the configuration is in error state
                    // Should be changed to "uninstalled" when Homescreen supports it silently 
                    hspsServerUtil::AddAttributeDescL( *confNode, KConfigurationAttrState, KConfStateError );                                       
                    processedCount++;
                    }
                }
            }
        }
    
    if ( processedCount )
        {
        // Remove uninstalled resources from the server (copies will remain in client's private directory)
        RemovePluginResourcesL( aAppODT, aPluginUid );
        
        // Store changes
        User::LeaveIfError( iDefinitionRepository.SetOdtL( aAppODT ) );
        }       
    
#ifdef HSPS_LOG_ACTIVE
    if( iLogBus )
        {
        if ( processedCount > 0 )
            {                
            iLogBus->LogText( _L( "ChspsMaintenanceHandler::InvalidateUninstalledPluginInstancesL(): - Error states updated") );
            ChspsOdtDump::Dump( aAppODT, *iLogBus );
            }
        else
            {
            iLogBus->LogText( _L( "ChspsMaintenanceHandler::InvalidateUninstalledPluginInstancesL(): - Error states were not added") );
            }
        }
#endif
    
    return ( processedCount > 0 );        
    }

// -----------------------------------------------------------------------------
// ChspsMaintenanceHandler::hspsPluginUpdateL
// (other items were commented in a header).
// -----------------------------------------------------------------------------

ThspsServiceCompletedMessage ChspsMaintenanceHandler::hspsPluginUpdateL( const ChspsODT& aOdt )
    {   
    ThspsServiceCompletedMessage ret( EhspsUpdatePluginFailed );
     
    if( !iDefinitionRepository.Locked() )
        {
        iDefinitionRepository.Lock();
        
        // In case of error. repository is unlocked 
        CleanupStack::PushL( TCleanupItem( UnlockRepository, &iDefinitionRepository ) );
                
        // set the default response
        ret = EhspsUpdatePluginSuccess;
        
        TInt errorCode( KErrNone );
        
        //create whole pluginOdt aOdt is only header for pluginOdt
        ChspsODT* odt = ChspsODT::NewL();
        CleanupStack::PushL( odt );
        odt->SetRootUid( aOdt.RootUid() );
        odt->SetThemeUid( aOdt.ThemeUid() );        
        odt->SetConfigurationType( aOdt.ConfigurationType() );
        odt->SetRootUid( aOdt.RootUid() );
        odt->SetProviderUid( aOdt.ProviderUid() );
        odt->SetThemeUid( aOdt.ThemeUid() );
        odt->SetProviderNameL( aOdt.ProviderName() );
        odt->SetThemeFullNameL( aOdt.ThemeFullName() );
        odt->SetThemeShortNameL( aOdt.ThemeShortName() );
        odt->SetThemeVersionL( aOdt.ThemeVersion() );
		odt->SetDescriptionL( aOdt.Description() );          
        odt->SetFlags( aOdt.Flags() ); 
        odt->SetMultiInstance( aOdt.MultiInstance() );
        User::LeaveIfError( iDefinitionRepository.GetOdtL( *odt ) );
        
        RArray<ThspsRepositoryInfo> notifParams;
        
        TRAP( errorCode, UpdatePluginFromAppConfsL( *odt, notifParams ));
        
        if( errorCode )
            {
            ret = EhspsUpdatePluginFailed;           
            }
                
        iDefinitionRepository.Unlock();
        CleanupStack::Pop( &iDefinitionRepository );

        if( ret == EhspsUpdatePluginSuccess )
            {
            if(notifParams.Count() > 0 )
                {
                //There is active app&appconfs to notify
                
                
                for( TInt i=0; i < notifParams.Count(); i++ )
                    {
                    iDefinitionRepository.RegisterNotification( notifParams[i] );
                    }
                }
            
            } 
        notifParams.Close();
        
        CleanupStack::PopAndDestroy( odt );
        }
         
    return ret;
    }
// -----------------------------------------------------------------------------
// ChspsMaintenanceHandler::UpdatePluginFromAppConfsL
// (other items were commented in a header).
//--------------------------------------------------------------------- 
void ChspsMaintenanceHandler::UpdatePluginFromAppConfsL( ChspsODT& aOdt,
        RArray<ThspsRepositoryInfo>& aNotificationParams )
    {
    
    if ( iHeaderListCache.Length() > 0 )
        {
        TInt count = iHeaderListCache.Count();
        for ( TInt i = 0; i < count; i++ )
            {
            ChspsODT* header = iHeaderListCache.At(i);
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
                User::LeaveIfError( iDefinitionRepository.GetOdtL( *odt ) );
                     
                
                RArray<TInt> pluginIds;
                
                hspsServerUtil::GetPluginIdsByUidL( *odt,
                                                    TUid::Uid( aOdt.ThemeUid() ),
                                                    pluginIds );
               
                
                if ( pluginIds.Count() > 0 )
                    {
                    
                    
                    TInt err = UpdatePluginConfigurationL( 
                            *odt,
                            aOdt,
                            pluginIds );
                 
                    User::LeaveIfError( iDefinitionRepository.SetOdtL( *odt ) );
                    
                   
                        
                    TBool status = EFalse;
                    for(TInt i = 0; i < pluginIds.Count(); i++ )
                        {
                        if( i == pluginIds.Count() - 1 )
                            {  
                            status = ETrue;
                            }
                          ThspsRepositoryInfo info( 
                                    ThspsRepositoryEvent( EhspsODTUpdated ),
                                    odt->RootUid(),
                                    odt->ThemeUid(),
                                    0, //=Any file
                                    0,
                                    aOdt.RootUid(),
                                    aOdt.ProviderUid(),
                                    aOdt.ThemeUid(),
                                    pluginIds[i], 
                                    status,
                                    aOdt.ThemeFullName(),
                                    (TLanguage)( aOdt.OdtLanguage() ) );
                        aNotificationParams.Append(info);
                        }
                    }
                        
                    
               
                pluginIds.Close();
                
                CleanupStack::PopAndDestroy( odt );                        
                                   
                }
            }
        
        }
    }
// -----------------------------------------------------------------------------
// ChspsMaintenanceHandler::UpdatePluginConfigurationL
// Not supported
// (other items were commented in a header).
// -----------------------------------------------------------------------------
//
TInt ChspsMaintenanceHandler::UpdatePluginConfigurationL(
        ChspsODT& aOdt, 
        ChspsODT& aPluginOdt,
        RArray<TInt>& aPluginIds )
    {
    TInt err(KErrNone);
    
    // remove old resources
    err = RemovePluginResourcesL( aOdt, aPluginOdt.ThemeUid() );
    
    if( err )
        {
        return err;
        }
    
    for( TInt i = 0; i < aPluginIds.Count() && err == KErrNone; i++ )
        {
        ChspsDomNode* configNode  = NULL;        
        ChspsDomNode* pluginNode =
                hspsServerUtil::FindPluginNodeL( aOdt, aPluginIds[i] );
        TInt index = 0;
        
        if ( pluginNode )
            {
            configNode = 
            hspsServerUtil::FindChildNodeByTagL( KConfigurationElement, *pluginNode, index );
            }
       
        if( configNode )
            {
            TInt id  = hspsServerUtil::DecString2Int(
                        configNode->AttributeValue(KConfigurationAttrId));
            hspsServerUtil::AddAttributeNumericL( *aPluginOdt.DomDocument().RootNode(), KConfigurationAttrId, id );
            
            pluginNode->ReplaceChildL(configNode,aPluginOdt.DomDocument().RootNode()); 
            }
        else
            {
            err = KErrNotFound;
            }
       
        }
    if( err )
        {
        return err;
        }
    
    _LIT(KSourcesFolder, "\\sources\\");
    _LIT(KLocalesFolder, "\\locales\\");
   // Add plugin resources
    TInt resourceCount = aPluginOdt.ResourceCount();
    for ( TInt index=0; index < resourceCount; index++ )
        {                       
        ChspsResource& pluginResource = aPluginOdt.ResourceL(index);    
            // Add only those that are located under the sources folder
        if ( pluginResource.FileName().FindF( KSourcesFolder ) > 0
                  || pluginResource.FileName().FindF( KLocalesFolder ) > 0 )
            {                                                   
            ChspsResource* r = pluginResource.CloneL();
            CleanupStack::PushL( r );
            aOdt.AddResourceL( r ); 
            CleanupStack::Pop( r );
            }
        }
    
    return err;
    }
// -----------------------------------------------------------------------------
// ChspsMaintenanceHandler::hspsGetListHeaders()
// Not supported
// (other items were commented in a header).
// -----------------------------------------------------------------------------
//
ThspsServiceCompletedMessage ChspsMaintenanceHandler::hspsGetListHeaders(const ChspsODT& /*aSearchMask*/,
                                                     CArrayPtrFlat<ChspsODT>& /*aHeaderList*/)        
    {
    return EhspsServiceNotSupported;
    }

// -----------------------------------------------------------------------------
// ChspsMaintenanceHandler::hspsSetActiveTheme()
// Not supported
// (other items were commented in a header).
// -----------------------------------------------------------------------------
//
ThspsServiceCompletedMessage ChspsMaintenanceHandler::hspsSetActiveTheme(const TDesC8& /*aSetMaskData*/,
                                                         TDes8& /*aHeaderData*/)
    {
    return EhspsServiceNotSupported;
    }

// -----------------------------------------------------------------------------
// ChspsMaintenanceHandler::hspsAddPlugin()
// Not supported - inherited from an interface and not used.
// -----------------------------------------------------------------------------
//
ThspsServiceCompletedMessage ChspsMaintenanceHandler::hspsAddPlugin(
            const TInt /*aAppUid*/,
            const TInt /*aParentPluginId*/,
            const TInt /*aPluginUid*/,
            const TInt /*aPosition*/,
            TInt& /*aAddedPluginId*/ )
    {
    return EhspsServiceNotSupported;
    }

// -----------------------------------------------------------------------------
// ChspsMaintenanceHandler::hspsRemovePlugin()
// Not supported - inherited from an interface and not used.
// -----------------------------------------------------------------------------
//
ThspsServiceCompletedMessage ChspsMaintenanceHandler::hspsRemovePlugin(
               const TInt /*aAppUid*/,                
               const TInt /*aPluginId*/ )
    {
    return EhspsServiceNotSupported;
    }

// -----------------------------------------------------------------------------
// ChspsMaintenanceHandler::hspsSetActivePlugin()
// Not supported - inherited from an interface and not used.
// -----------------------------------------------------------------------------
//
ThspsServiceCompletedMessage ChspsMaintenanceHandler::hspsSetActivePlugin(
            const TInt /*aAppUid*/,             
            const TInt /*aPluginId*/ )
    {
    return EhspsServiceNotSupported;
    }

// -----------------------------------------------------------------------------
// ChspsMaintenanceHandler::hspsReplacePlugin()
// Not supported - inherited from an interface and not used.
// -----------------------------------------------------------------------------
//
ThspsServiceCompletedMessage ChspsMaintenanceHandler::hspsReplacePlugin(
                const TInt /*aAppUid*/,
                const TInt /*aPluginId*/,             
                const TInt /*aConfUid*/ )
    {
    return EhspsServiceNotSupported;
    }

// -----------------------------------------------------------------------------
// ChspsMaintenanceHandler::hspsSetPluginSettings()
// Not supported - inherited from an interface and not used.
// -----------------------------------------------------------------------------
//
ThspsServiceCompletedMessage ChspsMaintenanceHandler::hspsSetPluginSettings(
                const ChspsODT& /*aHeader*/,
                const TInt /*aPluginId*/,
                ChspsDomDocument& /*aDom*/,
                const TBool /*aPluginStoringStatus*/)
    {
    return EhspsServiceNotSupported;
    }

// -----------------------------------------------------------------------------
// ChspsMaintenanceHandler::hspsMovePluginsL()
// Not supported - inherited from an interface and not used.
// -----------------------------------------------------------------------------
//
ThspsServiceCompletedMessage ChspsMaintenanceHandler::hspsMovePluginsL(
        const TInt /*aAppUid*/,
           const TInt /*aConfId*/,               
           const CArrayFixFlat<TInt>& /*aPluginIdList*/ )
    {
    return EhspsServiceNotSupported;
    }

// -----------------------------------------------------------------------------
// ChspsMaintenanceHandler::hspsSetConfState()
// Not supported - inherited from an interface and not used.
// -----------------------------------------------------------------------------
//
ThspsServiceCompletedMessage ChspsMaintenanceHandler::hspsSetConfState(
        const TInt /*aAppUid*/,
        const TInt /*aConfId*/,             
        const ThspsConfigurationState /*aState*/,
        const ThspsConfStateChangeFilter /*aFilter*/ )
    {
    return EhspsServiceNotSupported;
    }

// -----------------------------------------------------------------------------
// ChspsMaintenanceHandler::hspsRestoreActiveAppConf()
// Not supported - inherited from an interface and not used.
// -----------------------------------------------------------------------------
//
ThspsServiceCompletedMessage ChspsMaintenanceHandler::hspsRestoreActiveAppConf(
    const TInt /*aAppUid*/,
    const TInt /*aConfUid*/ )
    {
    return EhspsServiceNotSupported;
    }

// -----------------------------------------------------------------------------
// ChspsMaintenanceHandler::hspsCancelGetListHeaders()
// Cancels the GetListHeaders request
// (other items were commented in a header).
// -----------------------------------------------------------------------------
//
ThspsServiceCompletedMessage ChspsMaintenanceHandler::hspsCancelGetListHeaders()
    {
    if ( IsActive() )
        {
        Cancel();
        }
    iHeaderDataList->ResetAndDestroy(); // no headers
    iDeliveryCount = 0; // no delivered headers
    iSubscription = EFalse; // no subscription
    CompleteRequest( EhspsServiceRequestCanceled, iMessagePtr );
    // return with cancellation confirm    
    return EhspsServiceRequestCanceled;
    }
// -----------------------------------------------------------------------------
// ChspsClientRequestHandler::HandleDefinitionRespositoryEvent()
// Handles events coming from hspsDefinitionRepository.
// (other items were commented in a header).
// -----------------------------------------------------------------------------
//
TBool ChspsMaintenanceHandler::HandleDefinitionRespositoryEvent( ThspsRepositoryInfo aRepositoryInfo )
    {
    TInt errorCode = KErrNone;
    TBool status(EFalse);
    TRAP( errorCode, status = HandleDefinitionRespositoryEventL( aRepositoryInfo ) );
    if( errorCode != KErrNone )
        {
#ifdef HSPS_LOG_ACTIVE  
        if( iLogBus )
            {
            iLogBus->LogText( _L( "ChspsClientRequestHandler::HandleDefinitionRespositoryEvent(): - Error occured in HandleDefinitionRespositoryEventL" ) );
            }
#endif        
        }    
    return status;    
    }
// -----------------------------------------------------------------------------
// ChspsMaintenanceHandler::HandleDefinitionRespositoryEvent()
// Handles events coming from hspsDefinitionRepository.
// (other items were commented in a header).
// -----------------------------------------------------------------------------
//
TBool ChspsMaintenanceHandler::HandleDefinitionRespositoryEventL( ThspsRepositoryInfo aRepositoryInfo )
    {
    // is there any changes in cache
    if ( ( aRepositoryInfo.iEventType & EhspsCacheUpdate ) && !iDefinitionRepository.Locked() && iSubscription && !iMessagePtr.IsNull() )     
        {
        // cache changed, check if there is any changes when comparing to the local list    
        // and what kind of change there is
        iCompletedMessage = GetHeaderListUpdateL();
        // is there headers to deliver
        if ( iCompletedMessage == EhspsGetListHeadersRestart )
               {
               if (iHeaderDataList->Count())
                  {
                // at least one header on the list
                iMessagePtr.WriteL(2,iHeaderDataList->At(iDeliveryCount)->Des(),0);
                // add list count
                iDeliveryCount++; 
                // delivery of the first header
                 }
            CompleteRequest( EhspsGetListHeadersRestart, iMessagePtr );
               }
        else if ( iCompletedMessage == EhspsGetListHeadersUpdate )
               {
            // return list update, count continues from where it was
            iMessagePtr.WriteL(2,iHeaderDataList->At(iDeliveryCount)->Des(),0);
            // add list count
            iDeliveryCount++; 
            // deliver a list item
            CompleteRequest( EhspsGetListHeadersUpdate, iMessagePtr );
              }
        else if (iCompletedMessage == EhspsGetListHeadersEmpty)
               {
               CompleteRequest( EhspsGetListHeadersEmpty, iMessagePtr );
            // no list update available at the moment, continue the polling for list updates
               }
        }
    else if ( ( aRepositoryInfo.iEventType & EhspsCacheUpdate ) && iDefinitionRepository.Locked() 
        && !IsActive() && iSubscription )
        {
        After(KHeaderListUpdatePollingTimeSpan);    
        }      
    return EFalse;
    }

// -----------------------------------------------------------------------------
// ChspsMaintenanceHandler::DoCancel()
// Not implemented yet
// (other items were commented in a header).
// -----------------------------------------------------------------------------
//   
void ChspsMaintenanceHandler::DoCancel()
    {
    // no need to implement
    } 

// -----------------------------------------------------------------------------
// ChspsMaintenanceHandler::CompleteRequest()
// Completes client request
// (other items were commented in a header).
// -----------------------------------------------------------------------------
//    
void ChspsMaintenanceHandler::CompleteRequest(const ThspsServiceCompletedMessage aReturnMessage, 
    RMessagePtr2& aMessagePtr, const TDesC8& /*aHeaderData*/ )
    {
    TInt errorCode = KErrNone;
    RDesWriteStream writeBuf( iResultData );

    TRAP( errorCode, iResult->ExternalizeL( writeBuf ));

    writeBuf.Close();
    
    if ( !aMessagePtr.IsNull() )
        {
        if ( errorCode )
            {
            TRAP_IGNORE( aMessagePtr.WriteL( 0, KNullDesC8, 0 ));
            }
        else
            {
            TRAP_IGNORE( aMessagePtr.WriteL( 0, iResultData, 0 ));
            }
        aMessagePtr.Complete( aReturnMessage );    
        } 
    }
     
// -----------------------------------------------------------------------------
// ChspsMaintenanceHandler::RunError
// From CActive. Called when error occurred in asynchronous request
// (other items were commented in a header).
// -----------------------------------------------------------------------------
//
TInt ChspsMaintenanceHandler::RunError( TInt aError )
    {
    iResult->iSystemError = aError;
    iResult->iXuikonError = aError; 

#ifdef HSPS_LOG_ACTIVE  
    if( iLogBus )
        {
        iLogBus->LogText( _L( "ChspsMaintenanceHandler::RunError(): - error %d" ),
                aError );
        }
#endif
        
    CompleteRequest( EhspsGetListHeadersFailed, iMessagePtr );
    
    return KErrNone;
    }  

// -----------------------------------------------------------------------------
// ChspsMaintenanceHandler::RunL()
// Handles header list polling
// (other items were commented in a header).
// -----------------------------------------------------------------------------
//    
void ChspsMaintenanceHandler::RunL()
    {
    if ( !iDefinitionRepository.Locked()  && !iMessagePtr.IsNull() )     
        {
        // cache changed, check if there is any changes when comparring to the local list    
        // and what kind of change there is
        iCompletedMessage = GetHeaderListUpdateL();
        // is there headers to deliver
        if ( iCompletedMessage == EhspsGetListHeadersRestart)
               {
               if (iHeaderDataList->Count())
                  {
                // at least one header on the list
                iMessagePtr.WriteL(2,iHeaderDataList->At(iDeliveryCount)->Des(),0);
                // add list count
                iDeliveryCount++; 
                // delivery of the first header
                 }
            CompleteRequest( EhspsGetListHeadersRestart, iMessagePtr );
               }
        else if ( iCompletedMessage == EhspsGetListHeadersUpdate )
               {
            // return list update, count continues from where it was
            iMessagePtr.WriteL(2,iHeaderDataList->At(iDeliveryCount)->Des(),0);
            // add list count
            iDeliveryCount++; 
            // deliver a list item
            CompleteRequest( EhspsGetListHeadersUpdate, iMessagePtr );
              }
        else if (iCompletedMessage == EhspsGetListHeadersEmpty)
               {
               CompleteRequest( EhspsGetListHeadersEmpty, iMessagePtr );
            // no list update available at the moment, continue the polling for list updates
               }
        }
    else if ( !IsActive() )
        {
        After(KHeaderListUpdatePollingTimeSpan);    
        }      
    }

// -----------------------------------------------------------------------------
// ChspsMaintenanceHandler::GetHeaderListUpdateL
// Checks if the header list has changed and returns the appropriate 
// ThspsServiceCompletedMessage answer.
// (other items were commented in a header).
// -----------------------------------------------------------------------------
//
ThspsServiceCompletedMessage ChspsMaintenanceHandler::GetHeaderListUpdateL()
    {
    ThspsServiceCompletedMessage ret = EhspsGetListHeadersNoChange;
     
     // chech if repository have a cache update available 
       TInt oldcount = iHeaderDataList->Count();
       CArrayPtrSeg<HBufC8>* newheaderDataList = new( ELeave ) CArrayPtrSeg<HBufC8>
                            (KHeaderListGranularity);
       CleanupStack::PushL( TCleanupItem( ResetAndDestroyArray, newheaderDataList ) );
 
    GetHeaderListL( *newheaderDataList, *iSearchMask    );
      if ( newheaderDataList->Count() )
           {
           TBool reset = EFalse;
           TBool found = EFalse;
           TInt newcount = newheaderDataList->Count();      
           // cases:
           // 0. both lists are empty => no change
           if (!oldcount && !newcount)
               {
               ret = EhspsGetListHeadersNoChange;
               }
           else
           // 1. new list has members but old list is empty => just add new headers on old list 
           // => restart anyway
           if (!oldcount && newcount)
               {
                for (TInt k=0;k<newheaderDataList->Count();k++)
                {
                 HBufC8* l = newheaderDataList->At(k)->AllocL();
                 if (l != NULL)
                     {
                     CleanupStack::PushL(l);
                        iHeaderDataList->AppendL(l);
                        CleanupStack::Pop(l);
                     }
                     }
                  ret = EhspsGetListHeadersRestart; // start from begin                  
               }
           // 2. new list is empty and old has members => empty old list too => empty the list        
           else if (!newcount && oldcount)
               {
               // must fetch whole list again
               iHeaderDataList->ResetAndDestroy();
               iDeliveryCount = 0;  
               ret = EhspsGetListHeadersEmpty; // empty the list
               }
           // 3. old list and new list both have members, newcount < oldcount => reset
           else if (newcount < oldcount)
               {
               // must fetch whole list again
               iHeaderDataList->ResetAndDestroy();
               iDeliveryCount = 0;  
               GetHeaderListL( *iHeaderDataList, *iSearchMask );
               ret = EhspsGetListHeadersRestart; 
               }
           // 4. old list and new list both have members => examine next cases          
           else
               {
               TInt j=0;
               for ( TInt i=0; i<iHeaderDataList->Count();i++ )
                     {
                   HBufC8* old = iHeaderDataList->At(i);
                   if (old != NULL)
                       {
                       while ( j < newheaderDataList->Count() )
                           {
                           HBufC8* cand = newheaderDataList->At(j);
                           
                           if (cand != NULL)
                              {
                            if (CompareHeadersL(*old, *cand)) // is it there
                                     {
                                   // delete existing from the new list
                                   newheaderDataList->Delete(j);
                                   delete cand; //free allocated memory
                                   found = ETrue;
                                   break; // take next i in for for-loop
                                   }
                               else
                                   {
                                   // not found at this round, let see will the next one be a hit
                                   j++; // next j
                                   }
                              }
                             else
                              {
                              // not found at this round, let see will the next one be a hit
                               j++; // next j
                              }
                           } // while
                       }
                   // did it found a match for entry i on iHeaderDataList
                    if (!found)
                       {
                          // lists do not match
                       reset = ETrue;
                       break;
                       }
                   } // for
               if ( reset || newheaderDataList->Count() )
                   {
                   // must fetch whole list again
                   iHeaderDataList->ResetAndDestroy();
                   iDeliveryCount = 0;  
                   GetHeaderListL( *iHeaderDataList, *iSearchMask );
                   ret = EhspsGetListHeadersRestart; 
                   }
            else 
                   {
                iDeliveryCount = 0;  
                   ret = EhspsGetListHeadersNoChange;
                   }    
            }
        }
    else // no headers found => list must be emptied
        {
        // reset the list
        iHeaderDataList->ResetAndDestroy();
        iDeliveryCount = 0;  
        ret = EhspsGetListHeadersEmpty;    
        }
    CleanupStack::Pop( newheaderDataList );
    if ( newheaderDataList )
        {
        newheaderDataList->ResetAndDestroy();
        delete newheaderDataList;
        newheaderDataList = NULL;
        }
     return ret;
    }

// -----------------------------------------------------------------------------
// ChspsMaintenanceHandler::GetHeaderListL
// Fills aHeaderDataList with headers and sets the appropriate flag for active theme
// (other items were commented in a header).
// -----------------------------------------------------------------------------
//
void ChspsMaintenanceHandler::GetHeaderListL( 
        CArrayPtrSeg<HBufC8>& aHeaderDataList, 
        const ChspsODT& aSearchMask )
    {    
    HBufC8* headerBuf = aSearchMask.MarshalHeaderL();
    if ( !headerBuf )
        {
        User::Leave(KErrGeneral);
        }
    CleanupStack::PushL( headerBuf );    
    ChspsODT* searchOdt = ChspsODT::UnMarshalHeaderLC( *headerBuf );
    
    // Reset search results
    aHeaderDataList.ResetAndDestroy();
        
    for ( TInt i = 0; i < iHeaderListCache.Count(); i++ )
        {
        ChspsODT* header = iHeaderListCache.At( i );
        
        // Check whether the header matches the search criteria
        if ( FilterHeader( *searchOdt, *header ) )
            {
            // Append to the search results
            HBufC8* data = header->MarshalHeaderL();
            if ( data )
                {
                CleanupStack::PushL( data );
                aHeaderDataList.AppendL( data );
                CleanupStack::Pop( data );
                }
            }
        }                              
        
    CleanupStack::PopAndDestroy( 2, headerBuf ); // searchOdt, headerBuf    
    }

// -----------------------------------------------------------------------------
// ChspsMaintenanceHandler::RestoredDefaultL
// Goes through the themes for the application in order to find the Licensee 
// Default theme to restore.
// (other items were commented in a header).
// -----------------------------------------------------------------------------
//
void ChspsMaintenanceHandler::RestoredDefaultL( const ChspsODT& aSetMask, ChspsODT& aHeader )
    {
    TBool found = EFalse;
    TUint mask = 0;
    TUint rootUid = aSetMask.RootUid();
    ChspsODT* odt;
    
    if ( aSetMask.Flags() & EhspsThemeStatusOperatorDefault )
        {
        mask = EhspsThemeStatusOperatorDefault;
        }
    else if ( aSetMask.Flags() & EhspsThemeStatusUserDefault )
        {
        mask = EhspsThemeStatusUserDefault;
        }
        
    // If all but ROM based configurations should be removed
    if ( aSetMask.Flags() & EhspsThemeStatusClean )
        {
        for( TInt i = 0; i < iHeaderListCache.Count(); i++) // delete themes from c:
               {
               odt = iHeaderListCache.At(i);
               
               // Remove specific application configurations which are not matching the mask and
               // the licencee default flag
               if ( rootUid == odt->RootUid() 
                    && !( (odt->Flags() & mask) && (aSetMask.Flags() & mask) )
                    && !( odt->Flags() & EhspsThemeStatusLicenceeDefault ) )                  
                   {
                   // do not care about possible error
                   TRAP_IGNORE( RemoveThemeL( *odt ) );
                   }
            }
        // update cache after deletion
           iThemeServer.UpdateHeaderListCacheL();
        }
    
    // Try activating the first licencee restorable configuration in the cache
    for( TInt i = 0; i < iHeaderListCache.Count() && !found; i++ )
           {
           odt = iHeaderListCache.At(i);
           if( rootUid == odt->RootUid() && odt->Flags() & EhspsThemeStatusLicenceeRestorable )
               {
               if ( iThemeServer.ActivateThemeL( *odt, aHeader ) == KErrNone )
                   {
                   found = ETrue;
                   }
               }
        }
    // any default will do, depends on restoration level
    // this is safe because the default theme could be updated only with other default theme 
    mask = EhspsThemeStatusLicenceeDefault + EhspsThemeStatusOperatorDefault + EhspsThemeStatusUserDefault;

    // If no luck so far, try activating a "default" configuration with the new mask
    for( TInt i = 0; i < iHeaderListCache.Count() && !found; i++ )
           {
           odt = iHeaderListCache.At(i);
                      
           if( rootUid == odt->RootUid() && odt->Flags() & mask )
               {
               if ( iThemeServer.ActivateThemeL( *odt, aHeader ) == KErrNone )
                   {
                   found = ETrue;
                   }
               }
        }

    // If still no success, just select the first one in the cache
    if( !found )
        { // should not get here
        for( TInt i = 0; i < iHeaderListCache.Count() && !found; i++ ) // licensee default not found, activate some other theme
               {
               odt = iHeaderListCache.At(i);
               if( rootUid == odt->RootUid() )
                   {
                   if ( iThemeServer.ActivateThemeL( *odt, aHeader ) == KErrNone )
                       {
                       found = ETrue;
                       }
                   }
               }
        }
         
    // If there were no application specific configurations, give up
    if( !found )
        {
        // should not get here
        User::Leave( KErrNotFound );
        }      
    }


// -----------------------------------------------------------------------------
// ChspsMaintenanceHandler::RemoveThemeL
// Removes given theme from repository. If theme is in use (KErrInUse), then the theme
// is added on cleanup list.
// (other items were commented in a header).
// -----------------------------------------------------------------------------
//
void ChspsMaintenanceHandler::RemoveThemeL( const ChspsODT& aSetMask )
    {
    // do not even try to delete a theme licencee default theme as it is located in rom
    if( !( aSetMask.Flags() & EhspsThemeStatusLicenceeDefault ) )                                 
        {
        iDefinitionRepository.RemoveThemeL( aSetMask );
        }                                       
    }

// -----------------------------------------------------------------------------
// ChspsMaintenanceHandler::CompareHeadersL
// Compares the two theme headers and returns ETrue if they are the same
// (other items were commented in a header).
// -----------------------------------------------------------------------------
//
TBool ChspsMaintenanceHandler::CompareHeadersL(const TDesC8& aOldHeaderData, const TDesC8& 
    aNewHeaderData)
    {
    TBool ret;
    ChspsODT* old = ChspsODT::UnMarshalHeaderLC(aOldHeaderData);
    ChspsODT* cand= ChspsODT::UnMarshalHeaderLC(aNewHeaderData);
    if     ( old->RootUid() == cand->RootUid()            
        && old->ProviderUid() == cand->ProviderUid()            
        && old->ThemeUid() == cand->ThemeUid()
        && !old->ThemeVersion().Compare(cand->ThemeVersion())            
        && old->Flags() == cand->Flags() 
        )
        {
        ret = ETrue;
        }
    else
        {
        ret = EFalse;
        }
    CleanupStack::PopAndDestroy( cand );
    CleanupStack::PopAndDestroy( old );
    return ret;    
    }
    
// -----------------------------------------------------------------------------
// ChspsMaintenanceHandler::ComparePaths
// (other items were commented in a header).
// -----------------------------------------------------------------------------
//
TBool ChspsMaintenanceHandler::ComparePaths(const ChspsODT& aOldHeader, const ChspsODT& aNewHeader)
  {
  TBool ret;
  if  ( aOldHeader.RootUid() == aNewHeader.RootUid()        
        && aOldHeader.ProviderUid() == aNewHeader.ProviderUid()        
        && aOldHeader.ThemeUid() == aNewHeader.ThemeUid()            
        && !aOldHeader.ThemeVersion().Compare(aNewHeader.ThemeVersion()) )    
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
// ChspsMaintenanceHandler::FilterHeader
// Compares the header and mask and returns true if the mask doesn't have any different
// values than the header (missing values are OK).
// (other items were commented in a header).
// -----------------------------------------------------------------------------
//
TBool ChspsMaintenanceHandler::FilterHeader(
        const ChspsODT& aMask, 
        const ChspsODT& aHeader )
    {
    TBool ret( EFalse );
    if (
            (   // 0 is not valid when comparing
                (aMask.RootUid() && aMask.RootUid() == aHeader.RootUid())
                ||
                 // 0 is OK in mask when aHeader is valid 
                (!aMask.RootUid() && aHeader.RootUid())
            )
        &&
            (
                (aMask.ProviderUid() && aMask.ProviderUid() == aHeader.ProviderUid())
                ||
                (!aMask.ProviderUid() && aHeader.ProviderUid())
            )            
        &&
            (
                (aMask.ThemeUid() && aMask.ThemeUid() == aHeader.ThemeUid())
                ||
                (!aMask.ThemeUid() && aHeader.ThemeUid())
            )
        &&                   
            (
                (aMask.ThemeVersion().Length() && !aMask.ThemeVersion().Compare( 
                                                                           aHeader.ThemeVersion()))
                ||
                (!aMask.ThemeVersion().Length() && aHeader.ThemeVersion().Length())
            )
        &&
            (
            ( aMask.ConfigurationType() && aMask.ConfigurationType() == aHeader.ConfigurationType() )
            ||
            ( !aMask.ConfigurationType() )
            )            
        && 
            // Show widgets designed for the active resolution or scalable
            ( ( aHeader.Family() & aMask.Family() ) || aHeader.Family() == 0 )
       )
        {
        ret = ETrue;
        }
    
    return ret;    
    }

// -----------------------------------------------------------------------------
// ChspsMaintenanceHandler::RestoreDefaultAppConfL
// (other items were commented in a header).
// -----------------------------------------------------------------------------
//
void ChspsMaintenanceHandler::RestoreDefaultAppConfL(
    ChspsODT*& aHeader,
    ChspsODT& aOdt)
    {
    
    // If active application configuration is LicenceeRestorable 
    if ( aHeader->Flags() & EhspsThemeStatusLicenceeRestorable )
        {
        // Reinstall the configuration from ROM
        iThemeServer.ReinstallConfL( aHeader->RootUid(), aHeader->ThemeUid() );
        }
    else
        {
        // Try to activate a configuation with the LicenceeRestorable status
        ChspsODT* searchMask = ChspsODT::NewL();
        CleanupStack::PushL( searchMask );
        searchMask->SetRootUid( aHeader->RootUid() );
        searchMask->SetFlags( EhspsThemeStatusLicenceeRestorable );
        TInt pos( 0 );
        iThemeServer.GetConfigurationHeader( *searchMask, aHeader, pos );
        if ( aHeader )
            {
            // Activate licensee restorable configuration
            iThemeServer.ActivateThemeL( *aHeader, aOdt );
            ThspsRepositoryInfo info( EhspsODTActivated );
            iDefinitionRepository.RegisterNotification( info );
            }
        else
            {
            // Licensee restorable configuration not found
            // There must be at least one licensee restorable configuration
            User::Leave( KErrNotFound );
            }
        CleanupStack::PopAndDestroy( searchMask );
        }    
    }

// -----------------------------------------------------------------------------
// ChspsMaintenanceHandler::AddErrorConfigurationL()
// -----------------------------------------------------------------------------
//
void ChspsMaintenanceHandler::AddErrorConfigurationL(
        ChspsDomDocument& aAppDom,
        ChspsDomNode& aMissingPluginNode,
        const TInt aPluginUid
        )        
    {    
    // Create a new dummy configuration element
    ChspsDomNode* confNode = aAppDom.CreateElementNSL( 
        KConfigurationElement,         
        aMissingPluginNode.Namespace()            
        );
    CleanupStack::PushL( confNode );        
        
    hspsServerUtil::AddAttributeDescL( *confNode, KConfigurationAttrType, KConfTypeWidget );
    _LIT8( KUnknown, "unknown" );
    hspsServerUtil::AddAttributeDescL( *confNode, KConfigurationAttrInterface, KUnknown );
    hspsServerUtil::AddAttributeNumericL( *confNode, KConfigurationAttrUid, aPluginUid, EHex );
    hspsServerUtil::AddAttributeDescL( *confNode, KConfigurationAttrName, KUnknown );
    hspsServerUtil::AddAttributeDescL( *confNode, KConfigurationAttrNameEntity, KUnknown );
    hspsServerUtil::AddAttributeDescL( *confNode, KConfigurationAttrVersion, KUnknown );        
    hspsServerUtil::AddAttributeNumericL( *confNode, KConfigurationAttrMaxChild, 0, EDecimal );        
    
    // Indicate that the configuration is in error state
    hspsServerUtil::AddAttributeDescL( *confNode, KConfigurationAttrState, KConfStateError );
            
    // Set parent node
    aMissingPluginNode.AddChildL( confNode );
    CleanupStack::Pop( confNode );

    confNode->SetParent( &aMissingPluginNode );
        
    // Mandatory configuration-control node
    ChspsDomNode* controlNode = aAppDom.CreateElementNSL( 
        KControlElement,         
        confNode->Namespace()            
        );
    CleanupStack::PushL( controlNode );    
    confNode->AddChildL( controlNode );
    CleanupStack::Pop( controlNode );

    controlNode->SetParent( confNode );
    
    // Mandatory configuration-control-settings node
    ChspsDomNode* settingsNode = aAppDom.CreateElementNSL( 
        KSettingsElement,         
        controlNode->Namespace()            
        );
    CleanupStack::PushL( settingsNode );    
    controlNode->AddChildL( settingsNode );
    CleanupStack::Pop( settingsNode );
    settingsNode->SetParent( controlNode );           
    
    
    // Mandatory configuration-resources node
    ChspsDomNode* resourcesNode = aAppDom.CreateElementNSL( 
        KResourcesElement,         
        confNode->Namespace()            
        );
    CleanupStack::PushL( resourcesNode );    
    confNode->AddChildL( resourcesNode );
    CleanupStack::Pop( resourcesNode );        
    resourcesNode->SetParent( confNode );           
    
    }

// -----------------------------------------------------------------------------
// ChspsMaintenanceHandler::ServiceRestoreConfigurationsL
// -----------------------------------------------------------------------------
//    
void ChspsMaintenanceHandler::ServiceRestoreConfigurationsL( const RMessage2& aMessage )
    {    
    ThspsServiceCompletedMessage ret = EhspsRestoreConfigurationsFailed;    
    
    // using message pointer as a local variable because of synch call
    RMessagePtr2 messagePtr = aMessage;
    
    // IPC slots: 
    // #0) output: externalized ChspsResult for error handling
    // #1) input: ThspsParamRestoreConfigurations struct                         
    ThspsParamRestoreConfigurations params;        
    TPckg<ThspsParamRestoreConfigurations> packagedStruct(params);    
    aMessage.ReadL(1, packagedStruct);                                
    if ( params.appUid < 1 )
        {
        User::Leave( KErrArgument );
        }
    // Enable modification of owned configurations only
    if( messagePtr.SecureId().iId != params.appUid )
        {
        User::Leave( KErrAccessDenied );
        }
    
    TInt err = KErrNone;
    if( iDefinitionRepository.Locked() )
        {
        // Repository locked
        err = KErrAccessDenied;
        }
            
    if( !err )
        {
        // Lock the Plugin Repository (a.k.a. Def.rep)
        iDefinitionRepository.Lock();                                
        CleanupStack::PushL( TCleanupItem( UnlockRepository, &iDefinitionRepository ) );
        
        // Get active root configuration for the client application
        ChspsODT* appODT = ChspsODT::NewL();
        CleanupStack::PushL( appODT );
        iThemeServer.GetActivateAppConfigurationL( 
                params.appUid,
                *appODT );     

#ifdef HSPS_LOG_ACTIVE                
        if( iLogBus )
            {
            iLogBus->LogText( 
                _L( "ChspsMaintenanceHandler::ServiceRestoreConfigurationsL(): - Dump before the changes:" ) 
                );                
            ChspsOdtDump::Dump( *appODT, *iLogBus );
            }
#endif
        
        TInt err = KErrNone;
        if ( !params.restoreAll )
            {
            // Remove all widgets from the active view
            err = RestoreActiveViewL( *appODT );
            }        
        
        // As a backup, if restoration of the active view fails,  
        // or if all views but the locked view should be removed
        if ( err || params.restoreAll )
            {                        
            // Remove all views but the locked one and reset active view            
            RemoveUnlockedViewsL( *appODT );
            
            // Remove all widgets from the active view
            err = RestoreActiveViewL( *appODT );
            }       

#ifdef HSPS_LOG_ACTIVE                
        if( iLogBus )
            {
            iLogBus->LogText( 
                _L( "ChspsMaintenanceHandler::ServiceRestoreConfigurationsL(): - Dump after the changes:" ) 
                );        
            ChspsOdtDump::Dump( *appODT, *iLogBus );
            }
#endif
        if( !err )
            {
            // Stores the new application configuration into the repository
            err = iDefinitionRepository.SetOdtL( *appODT );
            ret = EhspsRestoreConfigurationsSuccess;
            }
        
        CleanupStack::PopAndDestroy( appODT );        

        // Unlock after the changes have been done
        iDefinitionRepository.Unlock();
        CleanupStack::Pop(&iDefinitionRepository);                               
        }
                          
    // Error handling
    iResult->iXuikonError = err;    
    
    // complete the message
    CompleteRequest( ret, messagePtr );
    }

// -----------------------------------------------------------------------------
// ChspsMaintenanceHandler::RestoreActiveViewL
// -----------------------------------------------------------------------------
//
TInt ChspsMaintenanceHandler::RestoreActiveViewL(
        ChspsODT& aAppODT )
    {          
    TInt err = KErrCorrupt;
    
    // Find active view node
    ChspsDomNode* pluginNode = FindActiveView( aAppODT );
    if ( pluginNode )
        {    
        // Remove all plugins from the view configuration
        err = RemovePluginConfigurationsL( 
                aAppODT,
                *pluginNode ); 
        }   
    return err;
    }

// -----------------------------------------------------------------------------
// ChspsMaintenanceHandler::FindActiveView
// -----------------------------------------------------------------------------
//
ChspsDomNode* ChspsMaintenanceHandler::FindActiveView(
        ChspsODT& aAppODT )
    {       
    ChspsDomNode* pluginNode = NULL;
        
    // Get 1st configuration element
    ChspsDomNode* confNode = aAppODT.DomDocument().RootNode();
    if( confNode && confNode->Name().CompareF( KConfigurationElement ) == 0 )
        {                    
        // Get control element
        ChspsDomNode* controlNode = 
                (ChspsDomNode*)confNode->ChildNodes().FindByName( KControlElement );
        if( controlNode )
            {    
            // Get plugins element
            ChspsDomNode* pluginsNode = 
                    (ChspsDomNode*)controlNode->ChildNodes().FindByName( KPluginsElement );  
            if( pluginsNode )
                {                        
                // Find active plugin node under the plugins node 
                pluginNode = hspsServerUtil::GetActivePluginNode( pluginsNode );
                }
            }
        }
    return pluginNode;
    }

// -----------------------------------------------------------------------------
// ChspsMaintenanceHandler::RemovePluginConfigurationsL
// -----------------------------------------------------------------------------
//
TInt ChspsMaintenanceHandler::RemovePluginConfigurationsL(
        ChspsODT& aAppODT, 
        ChspsDomNode& aActivePluginNode )
    {
    TInt err = KErrCorrupt;
    
    // Find a configuration node    
    ChspsDomNode* confNode = 
            (ChspsDomNode*)aActivePluginNode.ChildNodes().FindByName( KConfigurationElement );
    if( confNode )
        {
        // Get control node
        ChspsDomNode* controlNode = 
                (ChspsDomNode*)confNode->ChildNodes().FindByName( KControlElement );
        if( controlNode )
            {            
            // Find a plugins node        
            ChspsDomNode* pluginsNode = 
                    (ChspsDomNode*)controlNode->ChildNodes().FindByName( KPluginsElement );
            if( pluginsNode )
                {
                // Loop plugin nodes            
                err = KErrNone;
                ChspsDomList& childNodes = pluginsNode->ChildNodes();
                for( TInt pluginIndex=childNodes.Length()-1; pluginIndex >= 0; pluginIndex-- )
                    {
                    ChspsDomNode* pluginNode = (ChspsDomNode*)childNodes.Item( pluginIndex );
                    if( pluginNode )
                        {
                        // Remove the plugin configuration instance                       
                        err = RemoveConfigurationL( 
                            aAppODT,
                            *pluginNode );
                        if( err )
                            {
                            break;
                            }
                        }                    
                    }
                }
            }
        }
    
    return err;    
    }

// -----------------------------------------------------------------------------
// ChspsMaintenanceHandler::IsConfigurationLocked
// -----------------------------------------------------------------------------
//
TBool ChspsMaintenanceHandler::IsConfigurationLocked(
        ChspsDomNode& aConfNode )
    {   
    TBool isLocked = EFalse;
    
    ChspsDomList& attrList = aConfNode.AttributeList();
    ChspsDomAttribute* attr = 
        static_cast<ChspsDomAttribute*>( attrList.FindByName( KConfigurationAttrLocking ) );                
    if( attr )
        {        
        isLocked = ( attr->Value().CompareF( KConfLockingLocked ) == 0 );
       }
    
    return isLocked;
    }

// -----------------------------------------------------------------------------
// ChspsMaintenanceHandler::RemoveUnlockedViewsL
// -----------------------------------------------------------------------------
//
void ChspsMaintenanceHandler::RemoveUnlockedViewsL(
        ChspsODT& aAppODT )
    {
    // Get 1st configuration element
    ChspsDomNode* confNode = aAppODT.DomDocument().RootNode();
    if( !confNode || confNode->Name().CompareF( KConfigurationElement) != 0 )
        {            
        User::Leave( KErrCorrupt );            
        }           
    
    ChspsDomNode* controlNode = 
            (ChspsDomNode*)confNode->ChildNodes().FindByName( KControlElement );
    if( !controlNode )
        {
        User::Leave( KErrCorrupt );
        }
    
    // Get plugins element
    ChspsDomNode* pluginsNode = 
            (ChspsDomNode*)controlNode->ChildNodes().FindByName( KPluginsElement );  
    if( !pluginsNode )
        {            
        User::Leave( KErrCorrupt );
        }
        
    // Find plugin nodes which should be removed        
    const TInt pluginCount = pluginsNode->ChildNodes().Length();
    if( pluginCount > 1 )
        {        
        // Array for nodes which should removed from the configuration
        // (don't touch the appended objects) 
        RPointerArray<ChspsDomNode> nodeArray;
        CleanupClosePushL( nodeArray );
        
        // Remove all but one view
        TBool foundLocked = EFalse;                
        for( TInt nodeIndex=0; nodeIndex < pluginCount; nodeIndex++ ) 
            {
            ChspsDomNode* pluginNode = 
                    (ChspsDomNode*)pluginsNode->ChildNodes().Item( nodeIndex );
            if( pluginNode )
                {
            
                ChspsDomNode* confNode = 
                        (ChspsDomNode*)pluginNode->ChildNodes().FindByName( KConfigurationElement );
                if( confNode )
                    {                                
                    TBool isLocked = IsConfigurationLocked( *confNode );
                    
                    // If the plugin configuration hasn't been locked or 
                    // if there are several locked plugin nodes then remove all the rest
                    if( !isLocked || foundLocked )
                        {
                        // Mark for removal
                        nodeArray.Append( pluginNode );
                        }
                    else
                        {
                        // If this is the 1st locked node
                        if( !foundLocked )
                            {
                            foundLocked = ETrue;
                            }
                        }
                    }
                }                
            }
                
        // If all the nodes were marked for removal
        if( nodeArray.Count() > 0 && nodeArray.Count() == pluginCount )
            {
            // Unmark the 1st node - remove from the array only
            nodeArray.Remove( 0 );
            }
        
        // Remove rest
        TInt err = KErrNone;
        for( TInt nodeIndex=0; nodeIndex < nodeArray.Count(); nodeIndex++ ) 
            {
            // Remove the plugin node, related resources and maintain activity information
            err = RemoveConfigurationL( 
                    aAppODT, 
                    *nodeArray[nodeIndex] );            
            if( err )
                {
#ifdef HSPS_LOG_ACTIVE                                            
                if( iLogBus )
                    {
                    iLogBus->LogText( _L( "ChspsMaintenanceHandler::RemoveUnlockedViewsL(): - Restoring failed with %d code" ), err );                    
                    }
#endif
                }

            }
        
        nodeArray.Reset();
        CleanupStack::PopAndDestroy( 1, &nodeArray );        
        }           
    }

// end of file
