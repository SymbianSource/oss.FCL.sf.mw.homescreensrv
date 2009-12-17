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
* Description:  Implementation of CHSPSConfigurationIf class
*
*/


#include <e32base.h>
#include <e32def.h>

#include "hspsconfigurationif.h"
#include "hspsliwutilities.h"
#include "hspsliwvocabulary.hrh"
#include "hspsxmlelements.h"
#include "hspscallback.h"

#ifdef HSPS_LOG_ACTIVE
#include <hspslogbusfile.h>
#include <hspsliwdump.h>
#endif

using namespace LIW;


// ======== LOCAL FUNCTIONS ====================================================

// -----------------------------------------------------------------------------
// Cleanup function for CArrayPtrFlat arrays
// -----------------------------------------------------------------------------
//
static void DeleteArrayItems( TAny* aObject )
    {
    CArrayPtrFlat<ChspsODT>* array = reinterpret_cast<CArrayPtrFlat<ChspsODT>*>( aObject );
    array->ResetAndDestroy();
    delete array;
    }

// ======== MEMBER FUNCTIONS ===================================================

// -----------------------------------------------------------------------------
// Two-phased constructor.
// -----------------------------------------------------------------------------
//
CHSPSConfigurationIf* CHSPSConfigurationIf::NewL( TDesC8& aAppUid )
    {
    CHSPSConfigurationIf* self = new (ELeave) CHSPSConfigurationIf();
    CleanupStack::PushL( self );
    self->ConstructL( aAppUid );
    CleanupStack::Pop( self );
    return self;
    }

// -----------------------------------------------------------------------------
// Destructor.
// -----------------------------------------------------------------------------
//    
CHSPSConfigurationIf::~CHSPSConfigurationIf()
    {
    delete iHspsConfigurationService;
    delete iHspsPersonalisationService;
    delete iHspsLiwUtilities;
    
#ifdef HSPS_LOG_ACTIVE    
    // Delete liw dump object before logbus
    // since liw dump is dependant from ilogbus.
    delete iLiwDump;
    iLiwDump = NULL;
    delete iLogBus;
    iLogBus = NULL;
#endif    
    }

// -----------------------------------------------------------------------------
// Constructor
// -----------------------------------------------------------------------------
//
CHSPSConfigurationIf::CHSPSConfigurationIf()
    {
    }

// -----------------------------------------------------------------------------
// Symbian Constructor
// -----------------------------------------------------------------------------
//    
void CHSPSConfigurationIf::ConstructL( TDesC8& aAppUid )
    {       
#ifdef HSPS_LOG_ACTIVE    
    iLogBus = ChspsLogBusFile::NewL( ChspsLogBusFile::CreateLogFilename( _L("sapi_configurationIf") ) );
    iLiwDump = ChspsLiwDump::NewL( *iLogBus );    
#endif
        
    iHspsConfigurationService = CHspsConfigurationService::NewL();

#ifdef HSPS_LOG_ACTIVE    
    iHspsConfigurationService->SetLogBus( iLogBus );
#endif
    
    //create ODT.
    //Convert appUid.
    TInt appIdInt = -1;  	
	//convert appId to a number.
	TLex8 input( aAppUid );
	TInt convErr = input.Val( appIdInt );
	if( convErr != KErrNone ) 
		{
		User::Leave( KErrArgument );
		}
    iHspsConfigurationService->GetODTL( appIdInt ); 

    iHspsPersonalisationService = CHspsPersonalisationService::NewL();
    
    iHspsLiwUtilities = CHspsLiwUtilities::NewL();
    }

// -----------------------------------------------------------------------------
//  Executes the SAPI command
// -----------------------------------------------------------------------------
//
void CHSPSConfigurationIf::ExecuteCmdL( 
    const TDesC8& aCmdName,
    const CLiwGenericParamList& aInParamList,
    CLiwGenericParamList& aOutParamList,
    TUint aCmdOptions,
    MLiwNotifyCallback* aCallback )
    {
#ifdef HSPS_LOG_ACTIVE
    iLogBus->LogText( _L( "CHSPSConfigurationIf::ExecuteCmdL" ) );
    iLiwDump->LogLiwListAsText( const_cast<CLiwGenericParamList&>( aInParamList ) );   
    iLiwDump->LogLiwListAsBinary( const_cast<CLiwGenericParamList&>( aInParamList ) );
#endif

    if ( aCallback )
        {
        AsyncMethodCallL( 
            aCmdName,
            aInParamList,
            aOutParamList,
            aCmdOptions,
            aCallback );
        }
    else
        {
        SyncMethodCallL(
            aCmdName,
            aInParamList,
            aOutParamList );
        }

#ifdef HSPS_LOG_ACTIVE    
    iLiwDump->LogLiwListAsText( aOutParamList );      
    iLiwDump->LogLiwListAsBinary( aOutParamList );
#endif    
    }

// -----------------------------------------------------------------------------
// Synchronous method calls
// -----------------------------------------------------------------------------
//
void CHSPSConfigurationIf::SyncMethodCallL( 
    const TDesC8& aCmdName,
    const CLiwGenericParamList& aInParamList,
    CLiwGenericParamList& aOutParamList )
    {
    TInt err;
    //GETACTIVEAPPCONF
    if ( aCmdName.CompareF( KGetActiveAppConf ) == 0 )
        {
        TRAP( err, GetActiveAppConfL( aOutParamList ) );
        }
    //GETPLUGINCONF
    else if( aCmdName.CompareF( KGetPluginConf ) == 0 )
        {
        TRAP( err, GetPluginConfL( aInParamList, aOutParamList ) );
        }
    //GETPLUGINS
    else if( aCmdName.CompareF( KGetPlugins ) == 0 )
        {
        TRAP( err, GetPluginsL( aInParamList, aOutParamList ) );
        }
    //GETPLUGINLIST
    else if( aCmdName.CompareF( KGetPluginList ) == 0 )
        {
        TRAP( err, GetPluginListL( aInParamList, aOutParamList ) );
        }
    //ADDPLUGIN
    else if( aCmdName.CompareF( KAddPlugin ) == 0 )
        {
        TRAP( err, AddPluginL( aInParamList, aOutParamList ) );
        }
    //REMOVEPLUGIN
    else if( aCmdName.CompareF( KRemovePlugin ) == 0 )
        {
        TRAP( err, RemovePluginL( aInParamList, aOutParamList ) );
        }
    //REPLACEPLUGIN
    else if( aCmdName.CompareF( KReplacePlugin ) == 0 )
        {
        TRAP( err, ReplacePluginL( aInParamList, aOutParamList ) );
        }    
    //SETPLUGINSETTINGSL
    else if( aCmdName.CompareF( KSetPluginSettings ) == 0 )
        {
        TRAP( err, SetPluginSettingsL( aInParamList, aOutParamList ) );
        }
    //GETPLUGINSETTINGSL
    else if( aCmdName.CompareF( KGetPluginSettings ) == 0 )
        {
        TRAP( err, GetPluginSettingsL( aInParamList, aOutParamList ) );
        }
    //MOVEPLUGINSL
    else if( aCmdName.CompareF( KMovePlugins ) == 0 )
        {
        TRAP( err, MovePluginsL( aInParamList, aOutParamList ) );
        }
    //GETAPPCONFSL
    else if( aCmdName.CompareF( KGetAppConfs ) == 0 )
        {
        TRAP( err, GetAppConfsL( aInParamList, aOutParamList ) );
        }
    //SETACTIVEAPPCONFL
    else if( aCmdName.CompareF( KSetActiveAppConf ) == 0 )
        {
        TRAP( err, SetActiveAppConfL( aInParamList, aOutParamList ) );
        }
    //SETCONFSTATEL
    else if( aCmdName.CompareF( KSetConfState ) == 0 )
        {
        TRAP( err, SetConfStateL( aInParamList, aOutParamList ) );
        }
    //SETACTIVEPLUGIN
    else if( aCmdName.CompareF( KSetActivePlugin ) == 0 )
        {
        TRAP( err, SetActivePluginL( aInParamList, aOutParamList ) );
        }
    //UNKNOWN COMMAND
    else
        {
        // Invalid command
        err = KErrArgument;
        }
    if ( err != KErrNone )
        {
        SetErrorL( aOutParamList, err );
        }
    }

// -----------------------------------------------------------------------------
// Asynchronous method calls
// -----------------------------------------------------------------------------
//
void CHSPSConfigurationIf::AsyncMethodCallL( 
    const TDesC8& aCmdName,
    const CLiwGenericParamList& aInParamList,
    CLiwGenericParamList& aOutParamList,
    TUint aCmdOptions,
    MLiwNotifyCallback* aCallback )
    {
    TInt err = KErrNotSupported;
    TInt32 transactionId = -1;
    //REQUEST NOTIFICATION
    if(aCmdName.CompareF( KRequestNotification ) == 0)
        {
        if( aCallback && !(aCmdOptions & KLiwOptCancel) )
            {
            TRAP( err, RequestNotificationL( aInParamList, aCallback, transactionId ) );
            }
        else
            {
            if ( aCmdOptions & KLiwOptCancel )
                {
                TRAP(err, iHspsConfigurationService->UnRegisterObserverL());
                }
            }
        }
    //UNKNOWN COMMAND
    else
        {
        // Invalid command
        err = KErrArgument;
        }
    // Append transaction id in case of asynchronous requests
    if( aCallback && ( KLiwOptASyncronous & aCmdOptions ) && 
                            ( err == KErrNone ) && ( transactionId != -1 ) )
        {
        
        SetErrorL( aOutParamList, err );
        aOutParamList.AppendL(TLiwGenericParam( KTransactionID, 
                                    TLiwVariant( TInt32( transactionId ))));        
        }
                
       
    if ( err != KErrNone )
        {
        SetErrorL( aOutParamList, err );
        }      
    }

// -----------------------------------------------------------------------------
// Error in the parameter.
// -----------------------------------------------------------------------------
//
void CHSPSConfigurationIf::SetErrorL( CLiwGenericParamList& aOutParamList,
									 TInt32 err )
    {
    aOutParamList.Reset();
    CHspsLiwUtilities::AppendStatusL( err, aOutParamList );
    }

// -----------------------------------------------------------------------------
// Closes the interface
// -----------------------------------------------------------------------------
//
void CHSPSConfigurationIf::Close()
    {
    delete this;
    }

// -----------------------------------------------------------------------------
// Gets root plugin of active configuration.
// -----------------------------------------------------------------------------
//
void CHSPSConfigurationIf::GetActiveAppConfL( 
    CLiwGenericParamList& aOutParamList )
	{
	//InParamList not needed,
	TPtrC8 pluginId;
	pluginId.Set( KHspsLiwEmptyValue );	 
	
   	// Get configuration node of application configuration
   	ChspsDomNode* node = &( iHspsLiwUtilities->FindRootNodeByTagL( 
   	    KConfigurationElement, 
   	    *( iHspsConfigurationService->GetDOML().RootNode() ) ) );
   	
   	if ( !node )
   	    {
   	    User::Leave( KErrNotFound );
   	    }
   	
    // Get configuration state
   	TPtrC8 state;
   	CHspsLiwUtilities::GetAttributeValueL( 
   	    *node, 
   	    KConfigurationAttrState, 
   	    state );

    if ( state.Compare( KConfStateError ) == 0 )
        {
        // Restore active application configuration
        TPtrC8 confUid;
        CHspsLiwUtilities::GetAttributeValueL( 
            *node, 
            KConfigurationAttrUid, 
            confUid );        
        TInt appUid;
        iHspsConfigurationService->GetAppUidL( appUid );
        iHspsPersonalisationService->RestoreActiveAppConfL( 
            appUid, 
            confUid );
        
        // Invalidate ODT.
        iHspsConfigurationService->InvalidateODT();
        }
   	
	iHspsLiwUtilities->GetConfigurationL( 
	    KHspsLiwAppConf,
        pluginId,
        iHspsConfigurationService->GetDOML(),
        aOutParamList );
	
    }

// -----------------------------------------------------------------------------
// Gets plugin of active configuration.
// -----------------------------------------------------------------------------
//
void CHSPSConfigurationIf::GetPluginConfL( 
    const CLiwGenericParamList& aInParamList, 
    CLiwGenericParamList& aOutParamList )
    {
    TInt pos = 0;    
    const TLiwGenericParam* inParam = aInParamList.FindFirst( pos, KPluginId );
    
    if( inParam )
        {
        TPtrC8 pluginId;
        TLiwVariant inParamVariant = inParam->Value();
        pluginId.Set( inParamVariant.AsData() );       

        iHspsLiwUtilities->GetConfigurationL( 
            KHspsLiwPluginConf,
            pluginId,
            iHspsConfigurationService->GetDOML(),
            aOutParamList );
        }
    else
        {
        User::Leave( KErrArgument );
        }
    }

// -----------------------------------------------------------------------------
// Gets list of plugins with defined interface
// -----------------------------------------------------------------------------
//
void CHSPSConfigurationIf::GetPluginsL( 
        const CLiwGenericParamList& aInParamList, 
        CLiwGenericParamList& aOutParamList )
    {
    TInt pos;
    TPtrC8 interface;
    TPtrC8 type;
    const TLiwGenericParam* inParam;
    TLiwVariant inParamVariant;
    
    // Get interface parameter (mandatory)
    pos = 0;
    inParam = aInParamList.FindFirst( 
        pos, 
        KHspsLiwInterface );
    
    if( inParam )
        {
        inParamVariant = inParam->Value();
        interface.Set( inParamVariant.AsData() );    

        // Get type parameter (optional)
        pos = 0;
        inParam = aInParamList.FindFirst( 
            pos, 
            KHspsLiwType );
        if ( inParam )
            {
            inParamVariant = inParam->Value();
            type.Set( inParamVariant.AsData() );
            }
        
        // Get headers list of defined interface
        TUint32 family;
        iHspsConfigurationService->GetFamilyL( family );
        CArrayPtrFlat<ChspsODT>* list = 
            new ( ELeave )CArrayPtrFlat<ChspsODT>( KHeaderListGranularity );
        CleanupStack::PushL( TCleanupItem( DeleteArrayItems, list ) );
        iHspsPersonalisationService->GetPluginListL( 
            interface, 
            type,
            family,
            *list );
        
        // Create GetPlugins output parameters
        CHspsLiwUtilities::GetPluginsOutputL( *list, aOutParamList );
        CleanupStack::PopAndDestroy( list );
        }
    else
        {
        // Invalid method call
        User::Leave( KErrArgument );
        }
    }

// -----------------------------------------------------------------------------
// Gets list of plugins with defined interface
// -----------------------------------------------------------------------------
//
void CHSPSConfigurationIf::GetPluginListL( 
    const CLiwGenericParamList& aInParamList, 
    CLiwGenericParamList& aOutParamList )
    {
    
    TInt pos;
    TPtrC8 pluginId;
    TPtrC8 type;
    const TLiwGenericParam* inParam;
    TLiwVariant inParamVariant;

    // Get type parameter
    pos = 0;
    inParam = aInParamList.FindFirst( 
        pos, 
        KHspsLiwType );
    
    if( inParam )
        {
        inParamVariant = inParam->Value();
        type.Set( inParamVariant.AsData() );
        // Validate plugin type
        if ( type.Compare( KHspsLiwViewConf ) != 0 &&
             type.Compare( KHspsLiwWidgetConf ) != 0 &&
             type.Compare( KHspsLiwTemplateConf ) != 0 )
            {
            User::Leave( KErrArgument );
            }
        }
    
    // Get plugin id parameter
    pos = 0;
    inParam = aInParamList.FindFirst( 
        pos, 
        KPluginId );
    
    if ( inParam )
        {
        inParamVariant = inParam->Value();
        pluginId.Set( inParamVariant.AsData() );
        // Check that plugin node exists
        ChspsDomNode* node = &( CHspsLiwUtilities::FindRootNodeByIdentifierL( 
            KPluginElement,
            pluginId,
            *( iHspsConfigurationService->GetDOML().RootNode() ) ) );
        if ( !node )
            {
            User::Leave( KErrNotFound );
            }
        }
        
    // Create GetPluginList output parameters
    CHspsLiwUtilities::GetPluginListOutputL( 
        *iHspsConfigurationService->GetDOML().RootNode(),
        type,
        pluginId,
        aOutParamList );
    
    }

// -----------------------------------------------------------------------------
// Add a new plugin to the defined configuration
// -----------------------------------------------------------------------------
//
void CHSPSConfigurationIf::AddPluginL( 
    const CLiwGenericParamList& aInParamList, 
    CLiwGenericParamList& aOutParamList )
    {
    TInt pos;
    TPtrC8 confId;
    TPtrC8 pluginUid;
    TPtrC8 position;
    const TLiwGenericParam* inParam;
    TLiwVariant inParamVariant;
    TInt pluginId;
    TInt err( KErrNone );
    
    // Get confId parameter (mandatory)
    pos = 0;
    inParam = aInParamList.FindFirst( 
        pos, 
        KHspsLiwConfId );
    
    if( inParam )
        {
        inParamVariant = inParam->Value();
        confId.Set( inParamVariant.AsData() );    
        }
    else
        {
        // Mandatory element missing
        err = KErrArgument;
        }
    if ( err == KErrNone )
        {
        // Get pluginUid parameter (mandatory)
        pos = 0;
        inParam = aInParamList.FindFirst( 
            pos, 
            KHspsLiwPluginUid );
        if ( inParam )
            {
            inParamVariant = inParam->Value();
            pluginUid.Set( inParamVariant.AsData() );    
            }
        else
            {
            // Mandatory element missing
            err = KErrArgument;
            }
        }
    if ( err == KErrNone )
        {
        // Get position parameter (optional)
        pos = 0;
        inParam = aInParamList.FindFirst( 
            pos, 
            KHspsLiwPosition );
        if ( inParam )
            {
            inParamVariant = inParam->Value();
            position.Set( inParamVariant.AsData() );    
            }
        }
    
    User::LeaveIfError( err );
    
    // Add plugin
    TInt appUid;
    iHspsConfigurationService->GetAppUidL( appUid );
    
    iHspsPersonalisationService->AddPluginL(
        appUid,
        confId,
        pluginUid,
        position,
        pluginId
        );

    // Invalidate ODT.
    iHspsConfigurationService->InvalidateODT();
    
    // Create output parameters
    CHspsLiwUtilities::AddPluginOutputL( pluginId, aOutParamList );

    }

// -----------------------------------------------------------------------------
// Removes a plugin from the active configuration
// -----------------------------------------------------------------------------
//
void CHSPSConfigurationIf::RemovePluginL( 
    const CLiwGenericParamList& aInParamList, 
    CLiwGenericParamList& aOutParamList )
    {
    TInt err( KErrNone );
    const TLiwGenericParam* inParam;
    TLiwVariant inParamVariant;
    TPtrC8 pluginId;
    TInt pos;

    // Get pluginId parameter (mandatory)
    pos = 0;
    inParam = aInParamList.FindFirst( 
        pos, 
        KPluginId );
    
    if( inParam )
        {
        inParamVariant = inParam->Value();
        pluginId.Set( inParamVariant.AsData() );    
        }
    else
        {
        // Mandatory element missing
        err = KErrArgument;
        }

    User::LeaveIfError( err );

    // Remove plugin
    TInt appUid;
    iHspsConfigurationService->GetAppUidL( appUid );
    
    iHspsPersonalisationService->RemovePluginL(
        appUid,
        pluginId );

    // Invalidate ODT.
    iHspsConfigurationService->InvalidateODT();

    // Create output parameters
    CHspsLiwUtilities::RemovePluginOutputL( aOutParamList );

    }

// -----------------------------------------------------------------------------
// Replaces a plugin from the active configuration
// -----------------------------------------------------------------------------
//
void CHSPSConfigurationIf::ReplacePluginL( 
    const CLiwGenericParamList& aInParamList, 
    CLiwGenericParamList& aOutParamList )
    {
    TInt err( KErrNone );
    const TLiwGenericParam* inParam;
    TLiwVariant inParamVariant;
    TPtrC8 pluginId;
    TPtrC8 confUid;
    TInt pos;

    // Get pluginId parameter (mandatory)
    pos = 0;
    inParam = aInParamList.FindFirst( 
        pos, 
        KPluginId );
    
    if( inParam )
        {
        inParamVariant = inParam->Value();
        pluginId.Set( inParamVariant.AsData() );    
        }
    else
        {
        // Mandatory element missing
        err = KErrArgument;
        }

    if ( err == KErrNone )
        {
        // Get pluginUid parameter (mandatory)
        pos = 0;
        inParam = aInParamList.FindFirst( 
            pos, 
            KHspsLiwConfUid );
        if ( inParam )
            {
            inParamVariant = inParam->Value();
            confUid.Set( inParamVariant.AsData() );    
            }
        else
            {
            // Mandatory element missing
            err = KErrArgument;
            }
        }
    
    
    User::LeaveIfError( err );

    // Get active application uid
    TInt appUid;
    iHspsConfigurationService->GetAppUidL( appUid );
    
    // Replace plugin in the application configuration
    iHspsPersonalisationService->ReplacePluginL(
        appUid,
        pluginId,
        confUid );

    // Invalidate ODT.
    iHspsConfigurationService->InvalidateODT();

    // Create output parameters
    CHspsLiwUtilities::ReplacePluginOutputL( aOutParamList );
    }


// -----------------------------------------------------------------------------
// Sets plugin settings
// -----------------------------------------------------------------------------
//
void CHSPSConfigurationIf::SetPluginSettingsL( 
    const CLiwGenericParamList& aInParamList, 
    CLiwGenericParamList& aOutParamList )
    {
    TInt error = KErrNone;
    TPtrC8 pluginId;
    TInt pos = 0;
    TPtrC8 storingParams;
    TBool pluginStoringStatus(EFalse); // EFalse only rootconfiguration is stored.
    
    const CLiwList* settingsList;
    
    const TLiwGenericParam* inParam = aInParamList.FindFirst( pos, KPluginId );
        
    if( inParam )
        {
        TLiwVariant inParamVariant = inParam->Value();
        pluginId.Set( inParamVariant.AsData() );  
        }
    else
        {
        // Mandatory element missing
        error = KErrArgument;
        }
    
    if( error == KErrNone )
        {
        pos=0;
        const TLiwGenericParam* settingsParam = aInParamList.FindFirst( pos, KHspsLiwSettings );
        TLiwVariant settingsParamVariant;
        if( settingsParam )
            {
            settingsParamVariant = settingsParam->Value();
            settingsList = settingsParamVariant.AsList();
            }
        else
            {
            // Mandatory element missing
            error = KErrArgument;
            }
        }
    if( error == KErrNone )
        {
       
        ChspsDomDocument& domDocument = iHspsConfigurationService->GetDOML( );
            
        ChspsDomDocument* settingsDom = ChspsDomDocument::NewL();
        CleanupStack::PushL( settingsDom );
        
        error = iHspsLiwUtilities->SetPluginSettingsNodeL(
                        *settingsList,    
                        pluginId,
                        domDocument,
                        *settingsDom
                        );
        
        if( error == KErrNone )
            {
            
            // Get position parameter (optional)
            pos = 0;
            inParam = aInParamList.FindFirst( 
                        pos, 
                        KHspsLiwStoringParams );
            
                    
            if ( inParam )       
                {
                TLiwVariant inParamVariant  = inParam->Value();
                storingParams.Set( inParamVariant.AsData() );
                if( storingParams.Compare( KHspsLiwStorePluginRefence ) == 0)
                    {
                    pluginStoringStatus = ETrue;
                    }
                }
            TInt appUid;
            iHspsConfigurationService->GetAppUidL( appUid );
            iHspsPersonalisationService->SetPluginSettingsL(
                                         appUid,
                                         pluginId,
                                         *settingsDom,
                                         pluginStoringStatus
                                         );

            // Invalidate ODT.
            iHspsConfigurationService->InvalidateODT();
            }
        
        CleanupStack::PopAndDestroy( settingsDom );      
        }
        
    User::LeaveIfError( error );           
 
    // Create output parameters
    CHspsLiwUtilities::SetPluginSettingsOutputL( aOutParamList );

    }

// -----------------------------------------------------------------------------
// Gets plugin settings
// -----------------------------------------------------------------------------
//
void CHSPSConfigurationIf::GetPluginSettingsL( 
    const CLiwGenericParamList& aInParamList, 
    CLiwGenericParamList& aOutParamList )
    {
#ifdef HSPS_LOG_ACTIVE
    iLogBus->LogText( _L( "CHSPSConfigurationIf::GetPluginSettingsL" ) );
#endif
    
	TInt pos = 0;	
	const TLiwGenericParam* inParam = aInParamList.FindFirst( pos, KHspsLiwPluginUid );
	
	if( inParam )
		{
	    TInt appUid;
	    iHspsConfigurationService->GetAppUidL( appUid );
	    
		TPtrC8 pluginUid;
		TLiwVariant inParamVariant = inParam->Value();
		pluginUid.Set( inParamVariant.AsData() );   	
	
		ChspsODT* settingsOdt = ChspsODT::NewL();
    	CleanupStack::PushL( settingsOdt );
		iHspsPersonalisationService->GetPluginOdtL( appUid, pluginUid, settingsOdt );
		
	    // Create output parameters
		iHspsLiwUtilities->GetPluginSettingsOutputL( settingsOdt->DomDocument(), aOutParamList );

#ifdef HSPS_LOG_ACTIVE		
		iLiwDump->LogLiwListAsText( aOutParamList );
#endif
		
	    CleanupStack::PopAndDestroy( settingsOdt );
		}
	else
		{
		User::Leave( KErrArgument );
		}

    }

// Updates plugin positions
// -----------------------------------------------------------------------------
//
void CHSPSConfigurationIf::MovePluginsL( 
    const CLiwGenericParamList& aInParamList, 
    CLiwGenericParamList& aOutParamList )
    {
    TPtrC8 confId;
    TInt pos = 0;         
    CArrayFixFlat<TInt>* idArray = NULL;
        
    // Get confId
    const TLiwGenericParam* inParam = aInParamList.FindFirst( pos, KHspsLiwConfId );        
    if( !inParam )
        {
        // Mandatory element missing
        User::Leave( KErrArgument );
        }

    TLiwVariant inParamVariant = inParam->Value();
    confId.Set( inParamVariant.AsData() );  

    // Get plugin id list
    pos = 0;
    const TLiwGenericParam* pluginsParam = aInParamList.FindFirst( pos, KHspsLiwPlugins );
    if( !pluginsParam )
        {
        // Mandatory element missing
        User::Leave( KErrArgument );
        }

    TLiwVariant pluginsParamVariant;
    pluginsParamVariant = pluginsParam->Value();
    const CLiwList* pluginsList = pluginsParamVariant.AsList();                        
    const TInt KGranularity = 5;
    idArray = new ( ELeave )CArrayFixFlat<TInt>( KGranularity );
    CleanupStack::PushL( idArray );
    CHspsLiwUtilities::GetPluginIdListL( pluginsList, *idArray );

    TInt appUid;
    iHspsConfigurationService->GetAppUidL( appUid );
    iHspsPersonalisationService->MovePluginsL( 
            appUid,
            confId,
            *idArray );

    CleanupStack::PopAndDestroy( idArray );

    // Invalidate ODT.
    iHspsConfigurationService->InvalidateODT();
    
    // Create output parameters
    CHspsLiwUtilities::MovePluginsOutputL( aOutParamList );
    }

// -----------------------------------------------------------------------------
// Returns list of installed application configurations
// -----------------------------------------------------------------------------
//
void CHSPSConfigurationIf::GetAppConfsL( 
    const CLiwGenericParamList& /*aInParamList*/, 
    CLiwGenericParamList& aOutParamList )
    {
    // Get app UID of active configuration
    TInt appUid( 0 );
    iHspsConfigurationService->GetAppUidL( appUid );

    // Get list of application configurations
    TUint32 family;
    iHspsConfigurationService->GetFamilyL( family );
    CArrayPtrFlat<ChspsODT>* list = 
        new ( ELeave )CArrayPtrFlat<ChspsODT>( KHeaderListGranularity );
    CleanupStack::PushL( TCleanupItem( DeleteArrayItems, list ) );
    
    iHspsPersonalisationService->GetAppConfListL( 
        appUid,
        family,
        *list );
    
    // Create GetAppConfs output parameters
    CHspsLiwUtilities::GetAppConfsOutputL( *list, aOutParamList );
    
    // Cleanup application configuration list
    CleanupStack::PopAndDestroy( list );
    }

// -----------------------------------------------------------------------------
// Set application active configuration
// -----------------------------------------------------------------------------
//
void CHSPSConfigurationIf::SetActiveAppConfL( 
    const CLiwGenericParamList& aInParamList, 
    CLiwGenericParamList& aOutParamList )
    {
    // Get confUid
    TPtrC8 confUid;
    TInt pos = 0;         
    const TLiwGenericParam* inParam = aInParamList.FindFirst( 
        pos, 
        KHspsLiwConfUid );        
    if( !inParam )
        {
        // Mandatory element missing
        User::Leave( KErrArgument );
        }

    TLiwVariant inParamVariant = inParam->Value();
    confUid.Set( inParamVariant.AsData() );  

    // Get app UID
    TInt appUid( 0 );
    iHspsConfigurationService->GetAppUidL( appUid );

    // Set application configuration
    iHspsPersonalisationService->SetActiveAppConfL( appUid, confUid );                               
    
    // Invalidate ODT.
    iHspsConfigurationService->InvalidateODT();
    
    // Create output parameters
    CHspsLiwUtilities::SetActiveAppConfOutputL( aOutParamList );

    }
// ---------------------------------------------------------------------------
// Change Notification request to HSPS Service
// ---------------------------------------------------------------------------
//
void CHSPSConfigurationIf::RequestNotificationL( const CLiwGenericParamList& aInParamList, 
                                                    MLiwNotifyCallback* aCallback,
                                                    TInt32& aTransactionId )
    {
    aTransactionId = aCallback->GetTransactionID(); 
      
    CHspsReqNotifCallback* cb = CHspsReqNotifCallback::NewL(aCallback, aInParamList, 
            aTransactionId, *iHspsConfigurationService );
    
    CleanupStack::PushL( cb );
    
    TInt status = iHspsConfigurationService->RegisterObserverL( cb );
    // Issue request
    if( status == KErrNone)             
        {
        CleanupStack::Pop( cb );
        }
    else
        {
        CleanupStack::PopAndDestroy( cb );
        User::Leave(status);
        }        
    }

// -----------------------------------------------------------------------------
// Updates configuration's state
// -----------------------------------------------------------------------------
//
void CHSPSConfigurationIf::SetConfStateL( 
    const CLiwGenericParamList& aInParamList, 
    CLiwGenericParamList& aOutParamList )
    {

    // Get confId
    TPtrC8 confId;
    TInt pos = 0;         
    const TLiwGenericParam* inParam = aInParamList.FindFirst( 
        pos, 
        KHspsLiwConfId );        
    if( !inParam )
        {
        // Mandatory element missing
        User::Leave( KErrArgument );
        }
    
    TLiwVariant inParamVariant = inParam->Value();
    confId.Set( inParamVariant.AsData() );  

    // Get state
    TPtrC8 state;
    pos = 0;
    inParam = aInParamList.FindFirst( 
        pos, 
        KHspsLiwConfState );        
    if( !inParam )
        {
        // Mandatory element missing
        User::Leave( KErrArgument );
        }
    
    inParamVariant = inParam->Value();
    state.Set( inParamVariant.AsData() );  
    
    // Get filter parameter (optional)
    TPtrC8 filter;
    pos = 0;
    inParam = aInParamList.FindFirst( 
        pos, 
        KHspsLiwFilter );
    if ( inParam )
        {
        inParamVariant = inParam->Value();
        filter.Set( inParamVariant.AsData() );  
        }
    
    // Get app UID
    TInt appUid( 0 );
    iHspsConfigurationService->GetAppUidL( appUid );

    // Set application configuration
    iHspsPersonalisationService->SetConfStateL( appUid, confId, state, filter );                               
    
    // Invalidate ODT.
    iHspsConfigurationService->InvalidateODT();
    
    // Create output parameters
    CHspsLiwUtilities::SetConfStateOutputL( aOutParamList );

    }

// -----------------------------------------------------------------------------
// Gets list of plugins with defined interface
// -----------------------------------------------------------------------------
//
void CHSPSConfigurationIf::SetActivePluginL( 
    const CLiwGenericParamList& aInParamList, 
    CLiwGenericParamList& aOutParamList )
    {
    
    TInt pos;
    TPtrC8 pluginId;
    const TLiwGenericParam* inParam;
    TLiwVariant inParamVariant;
    
    // Get plugin id parameter
    pos = 0;
    inParam = aInParamList.FindFirst( 
        pos, 
        KPluginId );
    
    if ( inParam )
        {
        inParamVariant = inParam->Value();
        pluginId.Set( inParamVariant.AsData() );
        // Check that plugin node exists
        ChspsDomNode* node = &( CHspsLiwUtilities::FindRootNodeByIdentifierL( 
            KPluginElement,
            pluginId,
            *( iHspsConfigurationService->GetDOML().RootNode() ) ) );
        if ( !node )
            {
            User::Leave( KErrNotFound );
            }
        }
    else
        {
        // Manadatory plugin id parameter missing
        User::Leave( KErrArgument );
        }

    // Activate plugin
    TInt appUid;
    iHspsConfigurationService->GetAppUidL( appUid );
    
    iHspsPersonalisationService->SetActivePluginL(
        appUid,
        pluginId );

    // Invalidate ODT.
    iHspsConfigurationService->InvalidateODT();
    
    // Create output parameters
    CHspsLiwUtilities::SetActivePluginOutputL( aOutParamList );

    }

// ======== GLOBAL FUNCTIONS ===================================================


// End of file
