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
* Description:  Interface to HSPS service
*
*/

#include "hspspersonalisationservice.h"
#include "hspsserviceutilities.h"


// ======== LOCAL FUNCTIONS ====================================================


// ======== MEMBER FUNCTIONS ===================================================

// ---------------------------------------------------------------------------
// Two-phased constructor.
// ---------------------------------------------------------------------------
//
EXPORT_C CHspsPersonalisationService* CHspsPersonalisationService::NewL()
    {
    CHspsPersonalisationService* self = new (ELeave) CHspsPersonalisationService;
    CleanupStack::PushL( self );
    self->ConstructL();
    CleanupStack::Pop( self );
    return self;
    }
    
// ---------------------------------------------------------------------------
// Destructor.
// ---------------------------------------------------------------------------
//
CHspsPersonalisationService::~CHspsPersonalisationService()
    {
    if ( iHspsClient )
        {
        // Cancel asynchronous requests
        
        // Delete client
        delete iHspsClient;
        }
    }

// ---------------------------------------------------------------------------
// Constructor.
// ---------------------------------------------------------------------------
//
CHspsPersonalisationService::CHspsPersonalisationService()
    {
    }

// ---------------------------------------------------------------------------
// Two-phased constructor.
// ---------------------------------------------------------------------------
//
void CHspsPersonalisationService::ConstructL()
    {
    iHspsClient = ChspsClient::NewL( *this );
    }

// -----------------------------------------------------------------------------
// Get Plugin list
// -----------------------------------------------------------------------------
EXPORT_C void CHspsPersonalisationService::GetPluginListL(
    TDesC8& aInterface,
    TDesC8& aType,
    TUint32 aFamily,
    CArrayPtrFlat<ChspsODT>& aList )
    {
    // Setup a mask for finding plugins with defined interface 
    ChspsODT *searchMask = ChspsODT::NewL();
    CleanupStack::PushL( searchMask );        
    TInt interfaceUid = HspsServiceUtilities::HexString2IntL( aInterface );
    searchMask->SetRootUid( interfaceUid );
    if ( aType.Length() > 0 )
        {
        TUint type;
        HspsServiceUtilities::GetConfigurationTypeL( 
            aType,
            type );
        searchMask->SetConfigurationType( type );
        }

    searchMask->SetFamily( aFamily );
    
    TInt err = iHspsClient->hspsGetHeaders( 
        *searchMask, 
        aList );
    
    CleanupStack::PopAndDestroy( searchMask );
    
    if ( err == KErrNotFound )
        {
        // Plugin list empty
        aList.ResetAndDestroy();
        err = KErrNone;
        }
    
    User::LeaveIfError( err );
    }

// -----------------------------------------------------------------------------
// Add plugin to the configuration
// -----------------------------------------------------------------------------
EXPORT_C void CHspsPersonalisationService::AddPluginL(
    TInt aAppUid,
    TDesC8& aConfId,
    TDesC8& aPluginUid,
    TDesC8& aPosition,
    TInt& aPluginId )
    {

    TInt confId = HspsServiceUtilities::DecString2Int( aConfId );
    TInt pluginUid = HspsServiceUtilities::HexString2IntL( aPluginUid );
    if ( confId < 1 || pluginUid < 1 )
        {
        User::Leave( KErrArgument );
        }
    
    TInt position( -1 );
    if ( aPosition.Length() > 0 )
        {
        position = HspsServiceUtilities::DecString2Int( aPosition );
        }
        
    ThspsServiceCompletedMessage ret = iHspsClient->hspsAddPlugin(
        aAppUid,
        confId,
        pluginUid,
        position,
        aPluginId );

    if ( ret != EhspsAddPluginSuccess )
        {
        // Check if disk full error case.
        ChspsResult* errorDetails = ChspsResult::NewL();
        CleanupStack::PushL( errorDetails );
        iHspsClient->GethspsResult( *errorDetails );
        
        if( errorDetails->iXuikonError == KErrDiskFull )
            {
            User::Leave( KErrDiskFull );
            }                
        
        CleanupStack::PopAndDestroy( errorDetails );
        
        // Other error cases.
        User::Leave( KErrGeneral );
        }        
    }

// -----------------------------------------------------------------------------
// Remove plugin from the configuration
// -----------------------------------------------------------------------------
EXPORT_C void CHspsPersonalisationService::RemovePluginL(
    TInt aAppUid,
    TDesC8& aPluginId )
    {

    TInt pluginId = HspsServiceUtilities::DecString2Int( aPluginId );
    if ( pluginId < 1 )
        {
        User::Leave( KErrArgument );
        }

    ThspsServiceCompletedMessage ret = iHspsClient->hspsRemovePlugin(
        aAppUid,
        pluginId );
    
    if ( ret != EhspsRemovePluginSuccess )
        {
        User::Leave( KErrNotFound );
        }
    }

// -----------------------------------------------------------------------------
// Set plugin settings
// -----------------------------------------------------------------------------
EXPORT_C void CHspsPersonalisationService::SetPluginSettingsL(
    TInt aAppUid,
    TDesC8& aPluginId,
    ChspsDomDocument& aDom,
    TBool aPluginStoringStatus)
    {
  
    // Setup a mask for finding plugins with defined interface 
    ChspsODT *searchMask = ChspsODT::NewL();
    CleanupStack::PushL( searchMask );        
    searchMask->SetRootUid( aAppUid );
    const TInt id = HspsServiceUtilities::DecString2Int( aPluginId );
    ThspsServiceCompletedMessage ret = 
     iHspsClient->hspsSetPluginSettings( *searchMask, id, aDom, aPluginStoringStatus );
    
    CleanupStack::PopAndDestroy( searchMask );
  
    if ( ret != EhspsSetPluginSettingsSuccess )
        {
        User::Leave( KErrGeneral );
        }
    }

// -----------------------------------------------------------------------------
// Get plugin Odt
// -----------------------------------------------------------------------------
EXPORT_C void CHspsPersonalisationService::GetPluginOdtL(
    TInt aAppUid,
    TDesC8& aPluginUid,
    ChspsODT* aPluginOdt )
    {
    const TInt plugUid = HspsServiceUtilities::HexString2IntL( aPluginUid );    
    ThspsServiceCompletedMessage ret = iHspsClient->hspsGetPluginOdtL( aAppUid, plugUid, aPluginOdt );
    
    if( ret != EhspsGetPluginOdtSuccess )
    	{
    	User::Leave( KErrNotFound );  
    	}
    }

// -----------------------------------------------------------------------------
// Updates plugin positions in a configuration
// -----------------------------------------------------------------------------
EXPORT_C void CHspsPersonalisationService::MovePluginsL(
    const TInt aAppUid,
    TDesC8& aConfId,
    CArrayFixFlat<TInt>& aPluginIds )
    {
    const TInt confId = HspsServiceUtilities::DecString2Int( aConfId );        
    if ( confId < 1 )
        {
        User::Leave( KErrArgument );
        }
        
    const ThspsServiceCompletedMessage ret = iHspsClient->hspsMovePluginsL(
        aAppUid,
        confId,
        aPluginIds );
    
    if ( ret != EhspsMovePluginsSuccess )
        {
        User::Leave( KErrArgument );
        }
    }

// -----------------------------------------------------------------------------
// Gets a list of available application configurations
// -----------------------------------------------------------------------------
EXPORT_C void CHspsPersonalisationService::GetAppConfListL(
    TInt aAppUid,
    TUint32 aFamily,
    CArrayPtrFlat<ChspsODT>& aList )
    {
    // Setup a mask for finding application configurations 
    ChspsODT *searchMask = ChspsODT::NewL();
    CleanupStack::PushL( searchMask );        
    searchMask->SetRootUid( aAppUid );
    searchMask->SetConfigurationType( EhspsAppConfiguration );
    searchMask->SetFamily( aFamily );

    // Get application configurations
    User::LeaveIfError( iHspsClient->hspsGetHeaders( 
        *searchMask, 
        aList ) );

    CleanupStack::PopAndDestroy( searchMask );

    }

// -----------------------------------------------------------------------------
// Set active application configuration
// -----------------------------------------------------------------------------
EXPORT_C void CHspsPersonalisationService::SetActiveAppConfL(
    TInt aAppUid,
    TDesC8& aConfUid )
    {

    const TInt confUid = HspsServiceUtilities::HexString2IntL( aConfUid );        

    // Setup a set mask for active configuration 
    ChspsODT *setMask = ChspsODT::NewL();
    CleanupStack::PushL( setMask );        
    setMask->SetRootUid( aAppUid );
    setMask->SetThemeUid( confUid );
    setMask->SetConfigurationType( EhspsAppConfiguration );

    ChspsODT *activeConf = ChspsODT::NewL();
    CleanupStack::PushL( activeConf );        

    // Get application configurations
    const ThspsServiceCompletedMessage ret = iHspsClient->hspsSetActiveTheme( 
        *setMask, 
        *activeConf );

    CleanupStack::PopAndDestroy( activeConf );
    CleanupStack::PopAndDestroy( setMask );

    if ( ret != EhspsSetActiveThemeSuccess )
        {
        User::Leave( KErrNotFound );
        }
    }

// -----------------------------------------------------------------------------
// Set active application configuration
// -----------------------------------------------------------------------------
EXPORT_C void CHspsPersonalisationService::SetConfStateL(
    TInt aAppUid,
    TDesC8& aConfId,
    TDesC8& aState,
    TDesC8& aFilter )
    {

    const TInt confId = HspsServiceUtilities::DecString2Int( aConfId );        

    ThspsConfigurationState state;
    HspsServiceUtilities::GetConfigurationStateL( aState, state );
    
    ThspsConfStateChangeFilter filter;
    HspsServiceUtilities::GetConfigurationStateFilterL( aFilter, filter );
    // Set configuration state
    const ThspsServiceCompletedMessage ret = iHspsClient->hspsSetConfState( 
        aAppUid, 
        confId,
        state,
        filter );

    if ( ret != EhspsSetConfStateSuccess )
        {
        User::Leave( KErrNotFound );
        }
    }

// -----------------------------------------------------------------------------
// Restore active application configuration
// -----------------------------------------------------------------------------
EXPORT_C void CHspsPersonalisationService::RestoreActiveAppConfL(
    TInt aAppUid,
    TDesC8& aConfUid )
    {

    const TInt confUid = HspsServiceUtilities::HexString2IntL( aConfUid );        

    // Set configuration state
    const ThspsServiceCompletedMessage ret = iHspsClient->hspsRestoreActiveAppConf( 
        aAppUid,
        confUid );

    if ( ret != EhspsRestoreActiveAppConfSuccess )
        {
        User::Leave( KErrNotFound );
        }
    }

// -----------------------------------------------------------------------------
// Activates a plugin in active application configuration
// -----------------------------------------------------------------------------
EXPORT_C void CHspsPersonalisationService::SetActivePluginL(
    TInt aAppUid,
    TDesC8& aPluginId )
    {

    TInt pluginId = HspsServiceUtilities::DecString2Int( aPluginId );
    if ( pluginId < 1 )
        {
        User::Leave( KErrArgument );
        }

    ThspsServiceCompletedMessage ret = iHspsClient->hspsSetActivePlugin(
        aAppUid,
        pluginId );
    
    if ( ret != EhspsSetActivePluginSuccess )
        {
        User::Leave( KErrNotFound );
        }
    }

// -----------------------------------------------------------------------------
// Repaces a plugin in active application configuration
// -----------------------------------------------------------------------------
EXPORT_C void CHspsPersonalisationService::ReplacePluginL(
        const TInt aAppUid,
        const TDesC8& aPluginId,
        const TDesC8& aConfUid )
    {
    const TInt pluginId = HspsServiceUtilities::DecString2Int( aPluginId );
    const TInt confUid = HspsServiceUtilities::HexString2IntL( aConfUid );
    if ( aAppUid < 1 || pluginId < 1 || confUid < 1 )
        {
        User::Leave( KErrArgument );
        }
        
    const ThspsServiceCompletedMessage ret = iHspsClient->hspsReplacePlugin(
            aAppUid,
            pluginId,
            confUid );

    if( ret != EhspsReplacePluginSuccess )
        {
        // Check if disk full error case.
        ChspsResult* errorDetails = ChspsResult::NewL();
        CleanupStack::PushL( errorDetails );
        iHspsClient->GethspsResult( *errorDetails );
        
        if( errorDetails->iXuikonError == KErrDiskFull )
            {
            User::Leave( KErrDiskFull );
            }                
        
        CleanupStack::PopAndDestroy( errorDetails );
        
        // Other error cases.
        User::Leave( KErrNotFound );
        }
    }


// ---------------------------------------------------------------------------
// Hsps client service observer
// ---------------------------------------------------------------------------
//
void CHspsPersonalisationService::HandlehspsClientMessage( ThspsServiceCompletedMessage  /*aMessage*/ )
    {
    // Asynchronous service handling
    
    }
// ======== GLOBAL FUNCTIONS ===================================================


// End of file

