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
* Description:  Retrieves configurations from the Definition Repository
*
*/


#include "hsps_builds_cfg.hrh"

#include <f32file.h> 
#include <centralrepository.h>
#ifdef HSPS_LOG_ACTIVE
#include <hspsodtdump.h>
#include <hspslogbus.h>
#endif

#include "hspsresource.h"
#include "hspsdomdocument.h"
#include "hspsdomnode.h"
#include "hspsthememanagement.h"
#include "hspsdefinitionrepository.h"
#include "hspsodt.h"
#include "hspsresult.h"
#include "hspsreqnotifparam.h"
#ifdef _hsps_PERFORMANCE_TEST_
#include "hspstimemon.h"
#endif
#include "hspsclientrequesthandler.h"
#include "hspsmaintenancehandler.h"
#include "hspssecurityenforcer.h"
#include "hspsthemeserver.h"
#include "hspscenreplistener.h"
#include "hspsserverutil.h"
#include "hspsdomattribute.h"
#include "hspsdomlist.h"
#include "hspsdomdepthiterator.h"
#include "hspsdomnode.h"
#include "hspsconfiguration.h"


_LIT(KSourcesSubFolder, "\\sources\\");
_LIT(KLocalesSubFolder, "\\locales\\");



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
    

// ============================ MEMBER FUNCTIONS ===============================

// -----------------------------------------------------------------------------
// ChspsClientRequestHandler::ChspsClientRequestHandler
// C++ default constructor can NOT contain any code, that
// might leave.
// -----------------------------------------------------------------------------
//
ChspsClientRequestHandler::ChspsClientRequestHandler( ChspsThemeServer& aThemeServer ): 
    iThemeServer( aThemeServer ), 
    iCentralRepository( aThemeServer.CentralRepository() ),  
    iDefinitionRepository( aThemeServer.DefinitionRepository() ),
    iSecurityEnforcer( aThemeServer.SecurityEnforcer() ),
    iHeaderListCache (aThemeServer.HeaderListCache() )            
    { 
    iLastSuccessThemeLoadTime = 0;
    iRestoreDefault = EFalse;
    }

// -----------------------------------------------------------------------------
// ChspsDefinitionRepository::NewL
// Two-phased constructor.
// -----------------------------------------------------------------------------
//
ChspsClientRequestHandler* ChspsClientRequestHandler::NewL( ChspsThemeServer& aThemeServer )
    {
    ChspsClientRequestHandler* h = ChspsClientRequestHandler::NewLC( aThemeServer );
    CleanupStack::Pop(h);
    return (h);
    }

// -----------------------------------------------------------------------------
// ChspsDefinitionRepository::NewLC
// Two-phased constructor.
// -----------------------------------------------------------------------------
//
ChspsClientRequestHandler* ChspsClientRequestHandler::NewLC( ChspsThemeServer& aThemeServer )
    {
    ChspsClientRequestHandler* h = new (ELeave) ChspsClientRequestHandler( aThemeServer );
    CleanupStack::PushL(h);
    h->ConstructL();
    return (h);
    }

// Destructor
ChspsClientRequestHandler::~ChspsClientRequestHandler()
    {
    delete iOdt;
    delete iResult;
    delete iReqNotifParams;
    delete iRequestData;
    iFs.Close();
    delete iCenRepListener;
    iDefinitionRepository.UnregisterObserver( *this );
    }

// -----------------------------------------------------------------------------
// ChspsDefinitionRepository::ConstructL
// Symbian 2nd phase constructor can leave.
// -----------------------------------------------------------------------------
//
void ChspsClientRequestHandler::ConstructL()
    {
    // call the base class ConstructL
    User::LeaveIfError(iFs.Connect());
    iResult = ChspsResult::NewL();
    iReqNotifParams = ChspsRequestNotificationParams::NewL();
    iDefinitionRepository.RegisterObserverL( *this );    
    iCenRepListener = NULL;    
    
    }

// -----------------------------------------------------------------------------
// ChspsClientRequestHandler::ServiceGetOdtL
// Serves hspsRequestClient on hspsGetODT() service call.
// (other items were commented in a header).
// -----------------------------------------------------------------------------
//
void ChspsClientRequestHandler::ServiceGetOdtL(const RMessage2& aMessage)
    {
    iThemeLoadRepeatCount = 0;
    TInt errorCode = KErrNone;
    RMessagePtr2 msgPtr = aMessage;
    // Get configuration from IPC slot #1
    ThspsConfiguration configuration;        
    TPckg<ThspsConfiguration> packagedStruct(configuration);    
    aMessage.ReadL(1, packagedStruct);                        
    iRootUid = configuration.rootUid;    
    iSecureId = msgPtr.SecureId().iId;
    iRequestMessage = (ThspsServiceRequestMessage) aMessage.Function();
    
    // Get header data from IPC slot #2
    TBuf8<KMaxHeaderDataLength8> requestData; 
    msgPtr.ReadL( 2, requestData, 0 );
    ThspsServiceCompletedMessage ret = EhspsGetODTFailed;
    
    // resetting previous request objects
    if ( iRequestData )
        {
        delete iRequestData;
        iRequestData = NULL;
        }
         
    // constructing new data request objects  
    iRequestData = requestData.AllocL();
    
    // reset previous ODT
    if ( iOdt )
        {
        delete iOdt;
        iOdt = NULL;
        }
    
    // constructing new ODT
    iOdt = ChspsODT::NewL();
    iOdt->UnMarshalHeaderL( iRequestData->Des() );

    TBool err = EFalse;
    
    if ( !iRootUid )
        {
        err = ETrue;
        iResult->iSystemError = KErrInUse;
        CompleteRequestL( EhspsServiceRequestError, msgPtr );
        }
        
    if ( !err )
        {
        // Get configuration: 1st attempt
        ret = hspsGetODT(iRootUid, *iOdt );
        
        // If fetching failed
        if ( ret != EhspsGetODTSuccess )
            {
            // If not restoring yet
            if ( iRestoreDefault )
                {
                // Restore a default configuration
                TRAP( errorCode, RestoreDefaultL( *iOdt ) );    
                if ( !errorCode )
                    {
                    // 2nd attempt: try to fetch the default configuration
                    ret = hspsGetODT(iRootUid, *iOdt );
                    if ( ret != EhspsGetODTSuccess )
                        {                         
#ifdef HSPS_LOG_ACTIVE  
                        if( iLogBus )
                            {
                            iLogBus->LogText( _L( "ChspsClientRequestHandler::ServiceGetODTL(): - default theme for appuid %d is not available." ),
                                    iOdt->RootUid() );
                            }
#endif                        
                        CompleteRequestL( EhspsGetODTFailed, msgPtr );
                        }
                    } // !errorCode
                } // iRestoreDefault            
            } // !EhspsGetODTSuccess
            
        // If a configuration was found (either after the first attempt or after restoring the default configuration)
        if ( ret == EhspsGetODTSuccess )
            {           
            // If an application configuration was fetched
            if ( iOdt->ConfigurationType() == EhspsAppConfiguration )
                {
                // Finnish installation of the application configuration
                HandlePluginReferencesL( *iOdt );
                }                
            
            // write ODT-file path back to the client for theme load
            msgPtr.WriteL( 3, iODTPath, 0 );
            CompleteRequestL( ret, msgPtr );
            } // success
        else 
            {
            if ( !iHeaderListCache.Count() )
                {
                // header list cache is empty. This may also occur when memory is full
#ifdef HSPS_LOG_ACTIVE  
                if( iLogBus )
                    {
                    iLogBus->LogText( _L( "ChspsClientRequestHandler::ServiceGetODTL(): - theme cache is empty." ) );
                    }
#endif                    
                CompleteRequestL( EhspsGetODTFailed, msgPtr ); 
                }
            else if ( iRestoreDefault )
                {
#ifdef HSPS_LOG_ACTIVE  
                if( iLogBus )
                    {
                    iLogBus->LogText( _L( "ChspsClientRequestHandler::ServiceGetODTL(): - default theme cannot be restored." ) );
                    }
#endif                   
                CompleteRequestL( EhspsGetODTFailed, msgPtr );
                }
            else if ( iDefinitionRepository.Locked() )
                {
                // possible repository lock, wait for checking the theme availability after a while
#ifdef HSPS_LOG_ACTIVE  
                if( iLogBus )
                    {
                    iLogBus->LogText( _L( "ChspsClientRequestHandler::ServiceGetODTL(): - theme repository is locked, trying again after reasonable time .." ) );
                    }
#endif                    
                CompleteRequestL( EhspsGetODTFailed, msgPtr ); 
                }
            else
                {
#ifdef HSPS_LOG_ACTIVE  
                if( iLogBus )
                    {
                    iLogBus->LogText( _L( "ChspsClientRequestHandler::ServiceGetODTL(): - theme load failed." ) );
                    }
#endif                    
                CompleteRequestL( EhspsGetODTFailed, msgPtr ); 
                }
            } // else (!EhspsGetODTSuccess)
        } // !err
        
    }
    
// -----------------------------------------------------------------------------
// ChspsClientRequestHandler::HandlePluginReferencesL()
// -----------------------------------------------------------------------------
//
void ChspsClientRequestHandler::HandlePluginReferencesL(
        ChspsODT& aAppODT )
    {
    // Input validation
    if ( aAppODT.ConfigurationType() != EhspsAppConfiguration )
        {
        User::Leave( KErrGeneral );
        }
    
    // Assume that plugins need to be added
    TBool addPlugins = ETrue;     
    
    // Check whether the plugins needs to be added
    ChspsDomDocument& appDom = aAppODT.DomDocument(); 
    ChspsDomDepthIterator* iter = ChspsDomDepthIterator::NewL( *appDom.RootNode() );
    CleanupStack::PushL( iter );
    ChspsDomNode* prevNode = NULL;
    ChspsDomNode* node = iter->First();
    TInt tempCount = 0;
    while( node && prevNode != node && addPlugins )
        {           
        const TDesC8& name = node->Name();                
        if ( name == KConfigurationElement )
            {
            tempCount++;
            if ( tempCount > 1 )
                {
                addPlugins = EFalse;
                }                           
            }    
        prevNode = node;    
        node = iter->NextL();   
        }
    CleanupStack::PopAndDestroy( iter );
    
    // If plugin configurations need to be added
    if ( addPlugins )
        {    
        if ( iDefinitionRepository.Locked() )
            {
            // Repository was locked by another session?!
            User::Leave( KErrAccessDenied );
            }
        
        // Lock the Plugin Repository (a.k.a. Def.rep)
        iDefinitionRepository.Lock();                               
        CleanupStack::PushL( TCleanupItem( UnlockRepository, &iDefinitionRepository ) );
                
        // Go through the XML document, find all plugin nodes and append plugin specifc XML configurations
        AppendPluginConfigurationsL( aAppODT );       

        //Append widget instance specific predefined settings
        User::LeaveIfError( AppendInitialSettingsL( aAppODT ));
        
        // Set first plugin nodes from levels to be active. Others are to be deactivated.
        hspsServerUtil::EditPluginNodeActivityL( aAppODT.DomDocument().RootNode(),
                                                 hspsServerUtil::EActivateFirst );
                
        // Update changes into the Plug-in Repository
        User::LeaveIfError( iDefinitionRepository.SetOdtL( aAppODT ) );           
        
        // Unlock after the changes have been done
        iDefinitionRepository.Unlock();
        CleanupStack::Pop(&iDefinitionRepository);        
        
#ifdef HSPS_LOG_ACTIVE    
        if( iLogBus )
            {
            iLogBus->LogText( _L( "ChspsClientRequestHandler::HandlePluginReferencesL(): - dumping updated full XML document" ) );
            ChspsOdtDump::Dump( aAppODT, *iLogBus );
            }
#endif        
        }
    }

// -----------------------------------------------------------------------------
// ChspsClientRequestHandler::AddErrorConfigurationL()
// -----------------------------------------------------------------------------
//
void ChspsClientRequestHandler::AddErrorConfigurationL(
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
// ChspsClientRequestHandler::PrepareAndCompleteRequestL()
// -----------------------------------------------------------------------------
//
void ChspsClientRequestHandler::PrepareAndCompleteRequestL( const ThspsRepositoryInfo& aInfo,
                                                            const ThspsServiceCompletedMessage& aMessage )
    {
    TBool LastDefinitionRepositoryEvent(ETrue);
    if( !iReqNotifParams )
        {
        User::Leave( KErrCorrupt );
        }
    if( !aInfo.iLastNotification )
        {
        LastDefinitionRepositoryEvent = EFalse;
        }
    
    // Prepare request notification object.
    if(iReqNotifParams->iEvent == EhspsNoEvent || iReqNotifParams->iEvent == aInfo.iEventType )
        {
        iReqNotifParams->iEvent = aInfo.iEventType;  
        iReqNotifParams->iAppUid = aInfo.iAppUid;
        iReqNotifParams->iAppConfUid = aInfo.iAppConfUid;
        iReqNotifParams->iOrigUid = aInfo.iSecureId;
        iReqNotifParams->iPluginUid = aInfo.iPluginUid;
        iReqNotifParams->iCount++;
        iReqNotifParams->iPluginIds.Append( aInfo.iPluginId );
     
        if( aInfo.iName.Length() > 0 )
            {
            iReqNotifParams->SetNameL( aInfo.iName );
            }   
        }
    if( LastDefinitionRepositoryEvent )
        {
        // Externalize request notification object to message structure.
        TInt errorCode = KErrNone;
        RDesWriteStream writeBuf( iReqNotifData );
        CleanupClosePushL( writeBuf );
        TRAP( errorCode, iReqNotifParams->ExternalizeL( writeBuf ) );
        CleanupStack::PopAndDestroy();
            
        if ( !iMessagePtr.IsNull() )
            {
            if ( errorCode )    
                { 
                iMessagePtr.WriteL( 1, KNullDesC8, 0 );
                }    
            else
                {
                iMessagePtr.WriteL( 1, iReqNotifData, 0 );   
                }
            }   
        // theres no request message pending until request new one
        iRequestMessage = EhspsClientRequestBase;
        // Send message.    
        CompleteRequestL( aMessage, iMessagePtr );
        
        iReqNotifParams->ResetData();
        
        }
    
   
    }

// -----------------------------------------------------------------------------
// ChspsClientRequestHandler::AppendPluginConfigurationsL()
// -----------------------------------------------------------------------------
//
void ChspsClientRequestHandler::AppendPluginConfigurationsL( 
        ChspsODT& aAppODT )
    {    
    ChspsDomDocument& appDom = aAppODT.DomDocument(); 
    ChspsDomDepthIterator* iter = ChspsDomDepthIterator::NewL( *appDom.RootNode() );
    CleanupStack::PushL( iter );
           
    // Each configuration element get's an unique id value - same applies to the plugin elements.
    // The ids are then used for referencing a specific configuration or plug-in instance 
    // in the whole application configuration
    TInt confId = 0;
    TInt pluginId = 0;    
    
    ChspsDomNode* prevNode = NULL;
    ChspsDomNode* node = iter->First();
    while( node && prevNode != node )
        {   
        
        const TDesC8& name = node->Name();
        
        // Configuration element 
        if ( name == KConfigurationElement )
            {                                   
            // Generate an ID attribute for the configuration element
            confId++;
            hspsServerUtil::AddAttributeNumericL( *node, KConfigurationAttrId, confId );                                    
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
                        iLogBus->LogText( _L( "ChspsClientRequestHandler::AppendPluginConfigurationsL(): - Invalid XML" ) );
                        }
#endif    
                    
                    User::Leave( KErrCorrupt );
                    }         
                
                // Convert uids from string to numeric format                                        
                const TDesC8& pluginUidValue = pluginUidAttr->Value();                    
                const TUid pluginUid = hspsServerUtil::ConvertDescIntoUid(pluginUidValue);
                               
                // Get plugin configuration                                 
                ChspsODT* pluginOdt = ChspsODT::NewL();
                const TInt interfaceUid = 0; 
                CleanupStack::PushL( pluginOdt );
                iThemeServer.GetConfigurationL( 
                    interfaceUid, 
                    pluginUid.iUid,
                    *pluginOdt
                    );                
                if ( !pluginOdt || !pluginOdt->ThemeUid() )
                    {
#ifdef HSPS_LOG_ACTIVE  
                    if( iLogBus )
                        {
                        iLogBus->LogText( _L( "ChspsClientRequestHandler::AppendPluginConfigurationsL(): - Failed to find plugin uid %d" ),
                                pluginUid.iUid );
                        }
#endif
    
                    // Append an empty configuration with error status
                    AddErrorConfigurationL( 
                            appDom,
                            *node,
                            pluginUid.iUid );
                    
                    // Generate an ID attribute for the plugin element
                    pluginId++;
                    hspsServerUtil::AddAttributeNumericL( *node, KPluginAttrId, pluginId );                                        
                    }
                else
                    {                
                    // Generate an ID attribute for the plugin element
                    pluginId++;
                    hspsServerUtil::AddAttributeNumericL( *node, KPluginAttrId, pluginId );
                                    
                    // Copy plug-in resources to the application configuration              
                    TInt resourceCount = pluginOdt->ResourceCount();
                    for ( TInt index=0; index < resourceCount; index++ )
                        {                   
                        ChspsResource* pluginResource = &pluginOdt->ResourceL(index);
                        
                        // Add only those that are located under the sources or locales folder
                        if ( pluginResource->FileName().FindF( KSourcesSubFolder ) > 0
                            || pluginResource->FileName().FindF( KLocalesSubFolder ) > 0 )
                            {                                               
                            ChspsResource* r = pluginResource->CloneL();
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
                    while( pluginNode && !steppingtoConfNode )
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
                    
                    if ( pluginNode )
                        {
                        // Copy the plugin configuration to the main document.
                        ChspsDomNode* rootCopy = pluginNode->CloneL( node->StringPool());
                        if ( rootCopy )
                            {
                            rootCopy->SetParent( node );
                            node->AddChildL( rootCopy );
                            }
                        }
                    } // !pluginOdt else
                
                CleanupStack::PopAndDestroy( pluginOdt );
                } // KPluginsElement
            } 
        prevNode = node;    
        node = iter->NextL();   
        }
    CleanupStack::PopAndDestroy( iter );     
    
    }
// -----------------------------------------------------------------------------
// ChspsClientRequestHandler::AppendInitialSettingsL()
// -----------------------------------------------------------------------------
//
TInt ChspsClientRequestHandler::AppendInitialSettingsL( 
        ChspsODT& aAppODT )
    {    
    ChspsDomDocument& appDom = aAppODT.DomDocument(); 
    ChspsDomDepthIterator* iter = ChspsDomDepthIterator::NewL( *appDom.RootNode() );
    CleanupStack::PushL( iter );
           
    TInt error = KErrNone;  
    
    ChspsDomNode* prevNode = NULL;
    ChspsDomNode* parentNode = NULL;
    ChspsDomNode* node = iter->First();
    while( node && error == KErrNone  && prevNode != node)
        {  
        
        if( prevNode )
            {
            const TDesC8& prevNodeName = prevNode->Name();
            if( prevNodeName == KInitialSettingsElement )
                {
                // clean initial_settings
                parentNode = prevNode->Parent();
                parentNode->ChildNodes().RemoveItem(prevNode);
                delete prevNode;
                prevNode = NULL;
                }
            }
            
        const TDesC8& name = node->Name();
        
        // Initial settings
        if ( name == KInitialSettingsElement )
            {
            // Check parent element
            parentNode = node->Parent();
            const TDesC8& parentName = parentNode->Name();            
            if( parentName == KPluginElement )
                { 
                ChspsDomList& initial_settings_childList = node->ChildNodes();                                              
                ChspsDomNode* initialSettingsNode = 
                        static_cast<ChspsDomNode*>(initial_settings_childList.FindByName( KSettingsElement ));
                if( initialSettingsNode )
                    {
                        
                    ChspsDomList& initial_items = initialSettingsNode->ChildNodes();                                    
                    ChspsDomNode* controlNode = hspsServerUtil::FindNodeByTagL(KControlElement, *parentNode ); 
                          
                    if( controlNode )
                        {
                        ChspsDomList& controlNode_childList = controlNode->ChildNodes();
                    
                        ChspsDomNode* settingsNode = static_cast<ChspsDomNode*>(controlNode_childList.FindByName( KSettingsElement ));
                    
                        if( settingsNode )
                            {
                            ChspsDomList& items = settingsNode->ChildNodes();
                                    
                            if( items.Length() == initial_items.Length() )                    
                                {
                                TInt index = controlNode->ItemIndex( *settingsNode );
                                controlNode->DeleteChild(settingsNode);    
                                ChspsDomNode* clone = initialSettingsNode->CloneL( aAppODT.DomDocument().StringPool() );
                                CleanupStack::PushL( clone  );
                                controlNode->AddChildL( clone, index );
                                clone->SetParent( controlNode );             
                                CleanupStack::Pop( clone );
                                }
                            else if( items.Length() > initial_items.Length() )
                                {
                                error = ParseInitialSettingsItemsL(*initialSettingsNode,*settingsNode);        
                                }
                            else
                                {
                                // Child counts should match, set plugin to Error state
                                const TInt pluginUid = hspsServerUtil::GetPluginUid( parentNode ).iUid;
                                __ASSERT_DEBUG( pluginUid > 0, User::Leave( KErrCorrupt ) );
                                AddErrorConfigurationL( 
                                    appDom,
                                    *parentNode,
                                    pluginUid );                                        
                                }
                            }
                        else
                            {
                            error = KErrNotFound;
                            }
                        }
                    else
                        {
                        error = KErrCorrupt;
                        }
                        
                    // clean settings under initialsettings
                    node->ChildNodes().RemoveItem( initialSettingsNode );
                    delete initialSettingsNode;
                    initialSettingsNode = NULL;                    
                        
                    } 
                else
                    {
                    // initialSettingsNode (KSettingsElement) not found
                    error = KErrCorrupt;
                    }
                   
                }                   
            }
        
        prevNode = node;    
        node = iter->NextL();   
        }
    CleanupStack::PopAndDestroy( iter );      
          
    return error;
    
    }
 
// -----------------------------------------------------------------------------
// ChspsClientRequestHandler::ParseInitialSettingsItems
// -----------------------------------------------------------------------------
//   
TInt ChspsClientRequestHandler::ParseInitialSettingsItemsL(ChspsDomNode& aInitialSettingsNode,ChspsDomNode& aSettingsNode)
    {
    TInt error(KErrNone);
    ChspsDomDepthIterator* iter = ChspsDomDepthIterator::NewL( aInitialSettingsNode );
    CleanupStack::PushL( iter );
    
    ChspsDomNode* sourceNode = iter->First();
    
    ChspsDomNode* prevSourceNode = NULL;
    ChspsDomNode* targetNode = NULL;
    
    while( sourceNode && error == KErrNone && sourceNode != prevSourceNode   )
        {
        const TDesC8& s_name = sourceNode->Name();
         
        if ( s_name == KItemElement )
            {
            ChspsDomList& s_attrList = sourceNode->AttributeList();
                                                                           
            ChspsDomAttribute* s_itemIdAttr = 
                          static_cast<ChspsDomAttribute*> ( s_attrList.FindByName(KItemAttrId) ); 
            if( s_itemIdAttr )
                {
                targetNode = &( FindRootNodeByIdentifierL( KItemElement, s_itemIdAttr->Value(), aSettingsNode ));
                
                if( targetNode )
                    {
                    if( HandlePropertyNodeL( *sourceNode, *targetNode) != KErrNone )
                        {
                        error = KErrNotFound;
                        }
                    }
                else
                    {
                    error = KErrNotFound;
                    }
                }
            else
                {
                error = KErrCorrupt;
                }
            }
        prevSourceNode = sourceNode;    
        sourceNode = iter->NextL();
        }
    
    CleanupStack::PopAndDestroy( iter );  
    return error;
    }

// -----------------------------------------------------------------------------
// ChspsClientRequestHandler::HandlePropertyNodeL
// ----------------------------------------------------------------------------- 
TInt ChspsClientRequestHandler::HandlePropertyNodeL( 
     ChspsDomNode& aSourceItemNode, ChspsDomNode& aTargetItemNode  )
    {
   
   TInt error(KErrNone);

   TInt propertiesCount = (aSourceItemNode.ChildNodes()).Length();     
     
   if ( propertiesCount > 0 )
       {
       
       ChspsDomDepthIterator* iter = ChspsDomDepthIterator::NewL( aSourceItemNode );
       CleanupStack::PushL( iter ); 
       
       ChspsDomNode* node = iter->First();
       ChspsDomNode* prevNode = NULL;
       
            // Find items for plugin settings
       while( node && error == KErrNone && node != prevNode )
           {
           const TDesC8& name = node->Name();
           
           if ( name == KPropertyElement ) 
               {
               ChspsDomList& attrList = node->AttributeList();
                                                                                          
               ChspsDomAttribute* nameAttr = static_cast<ChspsDomAttribute*> ( attrList.FindByName(KPropertyAttrName) ); 
               ChspsDomAttribute* valueAttr = static_cast<ChspsDomAttribute*> ( attrList.FindByName(KPropertyAttrValue) );     
               
               if( nameAttr && valueAttr )
                   {
                    const TDesC8& propertyName = nameAttr->Value();
                    const TDesC8& propertyValue = valueAttr->Value();
                           
                   error = HandlePropertyAttributesL(propertyName, propertyValue, aTargetItemNode );
                       
                   }
               else
                   {
                   error = KErrCorrupt;
                   }
               }
                   
            prevNode = node;    
            node = iter->NextL();                 
            }
       CleanupStack::PopAndDestroy( iter ); 
        }
   else
       {
       error=KErrNotFound;
       }
        
    return error;
   
    }
//-----------------------------------------------------------------------------
// ChspsClientRequestHandler::HandlePropertyAttributesL 
// -----------------------------------------------------------------------------
//
TInt ChspsClientRequestHandler::HandlePropertyAttributesL(
         const TDesC8& aAttrName,
         const TDesC8& aAttrValue,
        ChspsDomNode& aNode )
    {
   
   
    TInt error(KErrNotFound);
    ChspsDomDepthIterator* iter = ChspsDomDepthIterator::NewL( aNode );
    CleanupStack::PushL( iter );                                
    ChspsDomNode* node = iter->First();                              
    TBool replaced(EFalse);                     
    
    while( node && !replaced )
        {
        const TDesC8& nodeName = node->Name();
        if( nodeName == KPropertyElement )
            {
            ChspsDomList& attrList = node->AttributeList();
            ChspsDomAttribute* attr = static_cast<ChspsDomAttribute*>( attrList.FindByName(KPropertyAttrName) );     
            if ( attr )
                {
                const TDesC8& name = attr->Value(); 
                if (aAttrName.Compare( name )== 0 ) 
                    {
                    ChspsDomAttribute* attr2 = static_cast<ChspsDomAttribute*>( attrList.FindByName(KPropertyAttrValue) );
                    if( attr2 )
                        {
                        attr2->SetValueL( aAttrValue );
                        replaced=ETrue;
                        error = KErrNone;
                        }
                    }
                }
            }
        if( !replaced )
            {
            node = iter->NextL();
            }
        }
                
    CleanupStack::PopAndDestroy( iter );
    
    return error;
   
    }  

// -----------------------------------------------------------------------------
// ChspsClientRequestHandler::FindRootNodeByIdentifierL
// -----------------------------------------------------------------------------
ChspsDomNode& ChspsClientRequestHandler::FindRootNodeByIdentifierL( 
        const TDesC8& aNodeTag,
        const TDesC8& aNodeIdentifier,
        ChspsDomNode& aNode )
    {
    ChspsDomDepthIterator* iter = ChspsDomDepthIterator::NewL( aNode );
    CleanupStack::PushL( iter );
    ChspsDomNode* targetNode( NULL );
    ChspsDomNode* node = iter->First();
    TBool found = EFalse;
    while( !found && node )
        {
        const TDesC8& name = node->Name();
        if ( name.Compare( aNodeTag ) == 0)
            {
            ChspsDomList& attrList = node->AttributeList();
            
            ChspsDomAttribute* attr = static_cast<ChspsDomAttribute*>( 
                                          attrList.FindByName( KItemAttrId ));
            if( attr )
                {                                          
                const TDesC8& value = attr->Value();
                if( value.CompareF( aNodeIdentifier ) == 0 )
                    {
                    found = ETrue;
                    targetNode = node;
                    }
                }
            }
        node = iter->NextL();
        }   
    CleanupStack::PopAndDestroy( iter );
    return *targetNode;
    }
// -----------------------------------------------------------------------------
// ChspsClientRequestHandler::ServiceGetOdtUpdateL
// Serves hspsRequestClient on hspsGetODTUpdate() service call.
// (other items were commented in a header).
// -----------------------------------------------------------------------------
//
void ChspsClientRequestHandler::ServiceGetOdtUpdateL(const RMessage2& aMessage)
    {
#ifdef HSPS_LOG_ACTIVE  
    if( iLogBus )
        {
        iLogBus->LogText( _L( "ChspsClientRequestHandler::ServiceGetOdtUpdateL(): - subscription received." ) );
        }
#endif
        // only allow one request to be submitted at a time and request must be valid
   
    if( iMessagePtr.IsNull() ) 
        {
        iMessagePtr = aMessage;
        iRequestMessage = (ThspsServiceRequestMessage)aMessage.Function();     
        if ( !iCenRepListener )
            {
            iCenRepListener = ChspsCenRepListener::NewL( *this, KhspsThemeStatusRepositoryUid );
            }
        iCenRepListener->Setup( iOdt->RootUid() );
        }
    else
        {
        CompleteRequestL(EhspsServiceRequestError, iMessagePtr );
        }
    }


// -----------------------------------------------------------------------------
// ChspsClientRequestHandler::ServiceAccessResourceFileL
// Serves hspsRequestClient on hspsAccessResourceFile() service call.
// Allows synchronic request to be submitted even if there is a asynchronous request outstanding.
// This function should return a file handle to a file in its private area 
// in the parameter fileSubSessionHandle
// (other items were commented in a header).
// -----------------------------------------------------------------------------
//
void ChspsClientRequestHandler::ServiceAccessResourceFileL(const RMessage2& aMessage)
    {
    // incoming data: TIpcArgs: &iReturnData, aAppUid, &aFileName, &fileSubSessionHandle 
    TInt errorCode = KErrNone;
    
    // Read packaged structure from slot 1
    ThspsConfiguration configuration;        
    TPckg<ThspsConfiguration> packagedStruct(configuration);    
    aMessage.ReadL(1, packagedStruct);
        
    // Read packaged file name from slot 2
    TFileName filename;
    RMessagePtr2 messagePtr = aMessage;
    aMessage.ReadL( 2, filename, 0 ); //file to open is in param slot 2
#ifdef HSPS_LOG_ACTIVE  
    if( iLogBus )
        {
        iLogBus->LogText( _L( "ChspsClientRequestHandler::ServiceAccessResourceFileL(): - %S" ),
                &filename );
        }
#endif
    
    // file server session and file to share to client
    RFile file;
    errorCode = iFs.ShareProtected();
    errorCode = file.Open(iFs, filename, EFileShareReadersOnly );
    CleanupClosePushL( file );
    
    // transfer the file to the client in parameter 3 of the IPC message
    if ( !errorCode )
        {
        // client takes care of it from now on
        errorCode = file.TransferToClient(aMessage,3);
        }
    else
        {
        // MARKMOIL 14.1.2006: 
		// Make sure that cache is up to date. This is actually needed in emulator only
		// because of possible manually altered repository content by a user;
		// user has removed a theme or resource. But next try should provide a better result.
#ifdef HSPS_LOG_ACTIVE  
        if( iLogBus )
            {
            iLogBus->LogText( _L( "ChspsClientRequestHandler::ServiceAccessResourceFileL(): - cannnot access file, restoring default.." ) );
            }
#endif
        
		//RestoreDefaultL( *iOdt ); 
		iThemeServer.UpdateHeaderListCacheL();
        }
        
        CleanupStack::PopAndDestroy( &file ); 
        
    User::LeaveIfError(errorCode);
    }

// -----------------------------------------------------------------------------
// ChspsClientRequestHandler::SetLogBus
// Set log bus to be used
// -----------------------------------------------------------------------------
//
#ifdef HSPS_LOG_ACTIVE
void ChspsClientRequestHandler::SetLogBus( ChspsLogBus* aLogBus )
    {
    iLogBus = aLogBus;
    }
#endif

// -----------------------------------------------------------------------------
// ChspsClientRequestHandler::hspsGetODT
// Implements hspsGetODT() API-function of MhspsClientRequestService interface.
// (other items were commented in a header).
// -----------------------------------------------------------------------------
//
ThspsServiceCompletedMessage ChspsClientRequestHandler::hspsGetODT(TInt aAppUid, ChspsODT& aODT)
    {
    // Get application's active theme from the central repository
    TInt themeUid;       
    TInt errorCode = iCentralRepository.Get( aAppUid, themeUid );    
    if ( !errorCode )
        {
        // Update ODT: prepare mask for a search
        aODT.SetRootUid( aAppUid );
        aODT.SetThemeUid( themeUid );
        
        // Store the active theme uid 
        iActiveThemeUid = themeUid;
        
        if ( iRequestMessage == EhspsGetODT )
            {
            TRAP(errorCode, CallGetODTPathL( aODT, iODTPath ));
            }
        else
            {
            errorCode = KErrGeneral;
            }
        }
    
    ThspsServiceCompletedMessage ret = EhspsGetODTFailed; 
    if (errorCode)
        {
        ret = EhspsGetODTFailed;
        iResult->iSystemError = errorCode;  
        }
    else
        {
        ret = EhspsGetODTSuccess;   
        }
  return ret;
  }

// -----------------------------------------------------------------------------
// ChspsClientRequestHandler::hspsGetODT
// Implements hspsGetODT(with resource  API-function of MhspsClientRequestService interface.
// (other items were commented in a header).
// -----------------------------------------------------------------------------
//
ThspsServiceCompletedMessage ChspsClientRequestHandler::hspsGetODT(TInt /*aAppUid*/, ChspsODT& /*aODT*/,
                                                      CArrayPtrSeg<ChspsResource>& /*aResourceList*/)
    {
    return EhspsServiceNotSupported;
    }


// -----------------------------------------------------------------------------
// ChspsClientRequestHandler::hspsGetODTUpdate
// Implements hspsGetODTUpdate() API-function of MhspsClientRequestService interface.
// (other items were commented in a header).
// -----------------------------------------------------------------------------
//
ThspsServiceCompletedMessage ChspsClientRequestHandler::hspsGetODTUpdate()
    {
    return EhspsServiceNotSupported;
    }
  

// -----------------------------------------------------------------------------
// ChspsClientRequestHandler::hspsCancelGetODTUpdate
// Implements hspsCancelGetODTUpdate() API-function of MhspsClientRequestService interface.
// (other items were commented in a header).
// -----------------------------------------------------------------------------
//
ThspsServiceCompletedMessage ChspsClientRequestHandler::hspsCancelGetODTUpdate()
    {
    ThspsServiceCompletedMessage ret = EhspsServiceRequestCanceled;
    iRequestMessage = EhspsCancelGetODTUpdate;
    TRAP_IGNORE(CompleteRequestL( ret, iMessagePtr ));
    return ret;
    }
  
// -----------------------------------------------------------------------------
// ChspsClientRequestHandler::hspsAccessResourceFile
// Not supported
// (other items were commented in a header).
// -----------------------------------------------------------------------------
//
ThspsServiceCompletedMessage ChspsClientRequestHandler::hspsAccessResourceFile( 
        const TDesC& /*aResourceFileName*/,
        const ThspsConfiguration& /*aConfiguration*/,
        RFile& /*aFile*/ )
    {
    return EhspsServiceNotSupported;
    }
// -----------------------------------------------------------------------------
// ChspsClientRequestHandler::HandleDefinitionRespositoryEvent()
// Handles events coming from hspsDefinitionRepository.
// (other items were commented in a header).
// -----------------------------------------------------------------------------
//
TBool ChspsClientRequestHandler::HandleDefinitionRespositoryEvent( ThspsRepositoryInfo aRepositoryInfo )
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
// ChspsClientRequestHandler::HandleDefinitionRespositoryEventL()
// Handles events coming from hspsDefinitionRepository.
// (other items were commented in a header).
// -----------------------------------------------------------------------------
//
TBool ChspsClientRequestHandler::HandleDefinitionRespositoryEventL( ThspsRepositoryInfo aRepositoryInfo )
    {    
    if ( aRepositoryInfo.iEventTime <= iLastSuccessThemeLoadTime )
        {
        // Do not consume this event because there could
        // be some other still waiting for it.
        return EFalse;
        }

    // Check if event regards currently active configuration. 
    TBool affectsActiveConfiguration = EFalse;
    if( aRepositoryInfo.iAppUid == iRootUid &&
        aRepositoryInfo.iAppConfUid == iActiveThemeUid )
        {        
        affectsActiveConfiguration = ETrue;
        }    
    
    // Work.
    if ( ( aRepositoryInfo.iEventType & EhspsODTUpdated ) &&
         affectsActiveConfiguration )
        {
#ifdef HSPS_LOG_ACTIVE  
        if( iLogBus )
            {
            iLogBus->LogText( _L( "ChspsClientRequestHandler::HandleDefinitionRespositoryEvent(): - theme update occurred." ) );
            }
#endif
        
        if ( !iDefinitionRepository.Locked() )
            {
            PrepareAndCompleteRequestL( aRepositoryInfo, EhspsGetODTUpdateHot );                
            }
        }
    else if( aRepositoryInfo.iEventType & EhspsODTAdded )
        {       
        //new plugin installed
#ifdef HSPS_LOG_ACTIVE  
        if( iLogBus )
            {
            iLogBus->LogText( _L( "ChspsClientRequestHandler::HandleDefinitionRespositoryEvent(): - theme update occurred." ) );
            }
#endif
        
        if ( !iDefinitionRepository.Locked() )
            {
            PrepareAndCompleteRequestL( aRepositoryInfo, EhspsAddPluginSuccess );            
            }
        }        
     else if ( ( aRepositoryInfo.iEventType & EhspsSettingsChanged ) &&
               affectsActiveConfiguration )
        {
#ifdef HSPS_LOG_ACTIVE  
        if( iLogBus )
            {
            iLogBus->LogText( _L( "ChspsClientRequestHandler::HandleDefinitionRespositoryEvent(): - theme settings changed occurred." ) );
            }
#endif
        
        if ( !iDefinitionRepository.Locked() )
            {
            PrepareAndCompleteRequestL( aRepositoryInfo, EhspsSetPluginSettingsSuccess );            
            }
        }    
     else if ( ( aRepositoryInfo.iEventType & EhspsODTModified ) &&
               affectsActiveConfiguration )
         {
#ifdef HSPS_LOG_ACTIVE  
        if( iLogBus )
            {
            iLogBus->LogText( _L( "ChspsClientRequestHandler::HandleDefinitionRespositoryEvent(): - theme settings changed occurred." ) );
            }
#endif
         
         if ( !iDefinitionRepository.Locked() ) 
             {
             PrepareAndCompleteRequestL( aRepositoryInfo, EhspsGetODTUpdateHot );
             }
         }
     else if ( ( aRepositoryInfo.iEventType & EhspsODTRemoved ) &&
               affectsActiveConfiguration )
         {
#ifdef HSPS_LOG_ACTIVE  
          if( iLogBus )
              {
              iLogBus->LogText( _L( "ChspsClientRequestHandler::HandleDefinitionRespositoryEvent(): - theme settings changed occurred." ) );
              }
#endif
        
          if ( !iDefinitionRepository.Locked() ) 
              {
              PrepareAndCompleteRequestL( aRepositoryInfo, EhspsRemovePluginSuccess );
              }
          }
     else if ( ( aRepositoryInfo.iEventType & EhspsClean ) &&
               affectsActiveConfiguration )
         {
#ifdef HSPS_LOG_ACTIVE  
          if( iLogBus )
              {            
              iLogBus->LogText( _L( "ChspsClientRequestHandler::HandleDefinitionRespositoryEvent(): - theme settings changed occurred." ) );
              }
#endif           
          if ( !iDefinitionRepository.Locked() )
              {
              PrepareAndCompleteRequestL( aRepositoryInfo, EhspsGetODTUpdateHot );
              }
         }
     else if ( ( aRepositoryInfo.iEventType & EhspsPluginActivated ) &&
               affectsActiveConfiguration )
         {
#ifdef HSPS_LOG_ACTIVE  
          if( iLogBus )
              {            
              iLogBus->LogText( _L( "ChspsClientRequestHandler::HandleDefinitionRespositoryEvent(): - plugin active state change occured." ) );
              }
#endif           
          if ( !iDefinitionRepository.Locked() )
              {
              PrepareAndCompleteRequestL( aRepositoryInfo, EhspsSetActivePluginSuccess );
              }
         }
     else if ( ( aRepositoryInfo.iEventType & EhspsPluginReplaced ) &&
               affectsActiveConfiguration )
         {
#ifdef HSPS_LOG_ACTIVE  
          if( iLogBus )
              {            
              iLogBus->LogText( _L( "ChspsClientRequestHandler::HandleDefinitionRespositoryEvent(): - plugin replace occured." ) );
              }
#endif           
          if ( !iDefinitionRepository.Locked() )
              {
              PrepareAndCompleteRequestL( aRepositoryInfo, EhspsReplacePluginSuccess );
              }
         }
          
    // do not consume this event because there could be some other still waiting for it    
    return EFalse;
    }

// -----------------------------------------------------------------------------
// ChspsClientRequestHandler::HandleCenRepChangeL()
// Handles events coming from CentralRepository.
// (other items were commented in a header).
// -----------------------------------------------------------------------------
//
void ChspsClientRequestHandler::HandleCenRepChangeL( const TUint32 aId )
    {        
    // 1. Check if another theme is activated
    TInt themeUid(0);
    TInt errorCode = iCentralRepository.Get( aId, themeUid );
    if ( !errorCode )
        {
        if ( iActiveThemeUid != themeUid )
            {
#ifdef HSPS_LOG_ACTIVE  
            if( iLogBus )
                {
                iLogBus->LogText( _L( "ChspsClientRequestHandler::HandleCenRepChangeL(): - theme status change occurred." ) );
                }
#endif
            
            if ( !iDefinitionRepository.Locked() )
                {
                
                ThspsRepositoryInfo info( 
                                   ThspsRepositoryEvent(EhspsODTActivated),
                                   aId,
                                   themeUid,
                                   0,0,0,0,
                                   0,0,
                                   ETrue,
                                   KNullDesC(),
                                   ELangTest);  
                                    
                           
                
                PrepareAndCompleteRequestL( info, EhspsGetODTUpdateStatus );
                
                }
            }
        }
    }
    
// -----------------------------------------------------------------------------
// ChspsClientRequestHandler::CompleteRequestL()
// Completes client request. Writes hspsResult data back.
// (other items were commented in a header).
// -----------------------------------------------------------------------------
//  
void ChspsClientRequestHandler::CompleteRequestL(const ThspsServiceCompletedMessage aReturnMessage,
                        RMessagePtr2& aMessagePtr, const TDesC8& /*aHeaderData*/ )
    {
    TInt errorCode = KErrNone;
    RDesWriteStream writeBuf( iResultData );
    CleanupClosePushL( writeBuf );
    TRAP( errorCode, iResult->ExternalizeL( writeBuf ));
    CleanupStack::PopAndDestroy();
           
    if ( !aMessagePtr.IsNull() ) 
        {
        if ( errorCode )
            {
            aMessagePtr.WriteL( 0, KNullDesC8, 0 );
            }
        else
            {
            aMessagePtr.WriteL( 0, iResultData, 0 );
            }
        aMessagePtr.Complete( aReturnMessage ); 
        } 
    }    
    
// -----------------------------------------------------------------------------
// ChspsClientRequestHandler::CallGetODTPathL
// Retrieves a file path to the provided ODT instance
// -----------------------------------------------------------------------------
//
void ChspsClientRequestHandler::CallGetODTPathL( ChspsODT& aODT, TDes& aODTPath )
    {
#ifdef _hsps_PERFORMANCE_TEST_
    TTime start_time;
    TBuf<KMaxHeaderDataLength8> tmp;
    tmp.Format(
        _L("ChspsClientRequestHandler::CallGetODTPathL(): -  getting path for theme %d .."), 
        aODT.ThemeUid()
        );
    start_time = ChspsTimeMon::StartTiming( tmp ); 
    ChspsTimeMon::PrintUserMem( _L("SERVER: - ready to get theme path.") );
#endif
    
    // retrieve specific ODT instance from the header cache
    TInt errorCode = iThemeServer.GetConfigurationL( 
            aODT.RootUid(), 
            aODT.ThemeUid(),
            aODT );        
    if ( !errorCode )
        {
        // get path to the ODT file
        iDefinitionRepository.GetResourcePathL( aODT, EResourceODT, aODTPath );
        iLastSuccessThemeLoadTime.HomeTime();
        
#ifdef _hsps_PERFORMANCE_TEST_ 
        // calculating service time
        tmp.Format(_L("ChspsClientRequestHandler::CallGetODTPathL(): - ODT ready."));      
        ChspsTimeMon::StopTiming(start_time, tmp);
        ChspsTimeMon::PrintUserMem( _L("SERVER: - theme file path ready.") );
#endif             
        }
    else
        {        
        // repository was unlocked but theme could not be found 
#ifdef HSPS_LOG_ACTIVE  
        if( iLogBus )
            {
            iLogBus->LogText( _L( "ChspsClientRequestHandler::CallGetODTPathL(): - theme cannot be accessed." ) );
            }
#endif
        // default theme must be restored
        iRestoreDefault = ETrue;
        }    
    
    User::LeaveIfError( errorCode );
    }
   
// -----------------------------------------------------------------------------
// ChspsClientRequestHandler::RestoreDefaultL
// Restoring default theme after failed theme load because no active theme found.
// (other items were commented in a header).
// -----------------------------------------------------------------------------
//
void ChspsClientRequestHandler::RestoreDefaultL( ChspsODT& aOdt )
    {
    TInt errorCode = KErrNone;
    ChspsMaintenanceHandler* mh = ChspsMaintenanceHandler::NewL(iThemeServer);
    CleanupStack::PushL(mh);
    ChspsODT* setmask = ChspsODT::NewL();
    CleanupStack::PushL( setmask );
    setmask->SetRootUid(iRootUid);
    // lets make sure that all themes are in cache
#ifdef HSPS_LOG_ACTIVE  
    if( iLogBus )
        {
        iLogBus->LogText( _L( "ChspsClientRequestHandler::RestoreDefaultL(): - updating cache." ) );
        }
#endif
    
    iThemeServer.UpdateHeaderListCacheL();     
    ThspsServiceCompletedMessage ret = mh->hspsRestoreDefault(*setmask, aOdt);
    if ( ret == EhspsRestoreDefaultSuccess )
        {
        // inform for cache update to the repository so that everyone will know 
        // about the change
        ThspsRepositoryInfo info( EhspsCacheUpdate );
        iDefinitionRepository.RegisterNotification( info );

#ifdef HSPS_LOG_ACTIVE  
        if( iLogBus )
            {
            iLogBus->LogText( _L( "ChspsClientRequestHandler::RestoreDefaultL(): - default theme restored." ) );
            }
#endif        
        }
    else
        {
#ifdef HSPS_LOG_ACTIVE  
    if( iLogBus )
        {
        iLogBus->LogText( _L( "ChspsClientRequestHandler::RestoreDefaultL(): - failed." ) );
        }
#endif
        
        errorCode = KErrNotFound;
        }
    iRestoreDefault = EFalse;
    CleanupStack::PopAndDestroy( 2, mh ); 
    User::LeaveIfError( errorCode );     
    }
       
// -----------------------------------------------------------------------------
// ChspsClientRequestHandler::RestoreValidThemeL
// 
// -----------------------------------------------------------------------------
void ChspsClientRequestHandler::RestoreValidThemeL(ChspsODT& aCorruptedOdt)    
    {

    // Get current theme
    ChspsODT* setmask = ChspsODT::NewL();
    CleanupStack::PushL(setmask);
    
    setmask->SetRootUid( aCorruptedOdt.RootUid() );
    setmask->SetThemeUid( aCorruptedOdt.ThemeUid() );

    if (aCorruptedOdt.Flags() & EhspsThemeStatusOperatorDefault)
        {
         // Need to delete corrupted operator theme to restore licensee default theme
        setmask->SetFlags(EhspsThemeStatusClean);    
        }

    ChspsODT* restoredOdt = ChspsODT::NewL();
    CleanupStack::PushL(restoredOdt);
                
    TInt errorCode = KErrNone;
    ChspsMaintenanceHandler* mh = ChspsMaintenanceHandler::NewL(iThemeServer);
    CleanupStack::PushL(mh);
    iThemeServer.UpdateHeaderListCacheL();     
    ThspsServiceCompletedMessage ret = mh->hspsRestoreDefault(*setmask, *restoredOdt);
    if ( ret == EhspsRestoreDefaultSuccess )
        {
        // inform for cache update to the repository so that everyone will know 
        // about the change
        ThspsRepositoryInfo info( EhspsCacheUpdate );
        iDefinitionRepository.RegisterNotification( info );
        }
    else
        {
        errorCode = KErrNotFound;
        }

    iRestoreDefault = EFalse;
    CleanupStack::PopAndDestroy( mh ); 
    
    CleanupStack::PopAndDestroy(restoredOdt);
    
    CleanupStack::PopAndDestroy(setmask);
    
    User::LeaveIfError( errorCode );     
    }


//  End of File  

