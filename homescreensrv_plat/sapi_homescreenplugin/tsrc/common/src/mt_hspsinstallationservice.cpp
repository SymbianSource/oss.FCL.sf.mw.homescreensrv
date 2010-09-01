/*
* Copyright (c) 2004 Nokia Corporation and/or its subsidiary(-ies).
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
*/



#include <e32svr.h>
#include <flogger.h>
#include <f32file.h>

#include <e32base.h>
#include <bacline.h>
#include <bautils.h>

#include <hspsthememanagement.h>
#include <hspsodt.h>
#include <hspsclient.h>
#include <hspsresult.h>

#include "mt_hspsinstallationservice.h"
 
// -----------------------------------------------------------------------------
// CThemeInstaller::NewL()
// 
// 
// -----------------------------------------------------------------------------
//
EXPORT_C MT_CHspsInstallationService* MT_CHspsInstallationService::NewL()
    {    
    MT_CHspsInstallationService* self = new ( ELeave ) MT_CHspsInstallationService;
    CleanupStack::PushL( self );
    self->ConstructL( );
    CleanupStack::Pop( self );
    return( self );    
    }


// -----------------------------------------------------------------------------
// CThemeInstaller::ConstructL()
// 
// 
// -----------------------------------------------------------------------------
//
void MT_CHspsInstallationService::ConstructL()
    {
    iResult = ChspsResult::NewL();
  	iHspsClient = ChspsClient::NewL( *this );
  	iHeader = NULL;  
    iLoop = new ( ELeave )CActiveSchedulerWait(); 
    }
   
// -----------------------------------------------------------------------------
// CThemeInstaller::~CThemeInstaller()
// 
// 
// -----------------------------------------------------------------------------
//
MT_CHspsInstallationService:: ~MT_CHspsInstallationService()
    {    
    delete iHspsClient;
    delete iResult;
    delete iHeader;
    delete iLoop;
    }

// ---------------------------------------------------------------------------
// Constructor.
// ---------------------------------------------------------------------------
//
MT_CHspsInstallationService::MT_CHspsInstallationService()
    {
    }     
// -----------------------------------------------------------------------------
// Gets program arguments and starts the installer
// -----------------------------------------------------------------------------
//

void MT_CHspsInstallationService::InstallConfigurationL(
    const TDesC& aManifestFileName )
    {    
    // Init
    RFs fs;           
    User::LeaveIfError( fs.Connect() );
    CleanupClosePushL( fs );

    // Check if the file exists
    BaflUtils baf;
    if( baf.FileExists( fs, aManifestFileName ) )
        {
        InstallThemeL( aManifestFileName );
        iLoop->Start();
        }
    else
        {
        User::Leave( KErrNotFound );
        }
    CleanupStack::PopAndDestroy( 1, &fs ); // directoryList, args, Fs   
    }
    
// -----------------------------------------------------------------------------
// Initiates installing of a confgiruation from the provided manifest file
// -----------------------------------------------------------------------------
//
void MT_CHspsInstallationService::InstallThemeL( 
    const TDesC& aManifestFileName )
    {      	
    
	if ( iHeader )
		{
		delete iHeader;
		iHeader = NULL;
		}			
	iHeader = ChspsODT::NewL();
    
	TParse p;
    p.Set( aManifestFileName, NULL, NULL );
    TPtrC ptr = p.NameAndExt();
    
	ThspsServiceCompletedMessage ret = iHspsClient->hspsInstallTheme( aManifestFileName, *iHeader );
	
	switch ( ret )
	    {
	    case EhspsInstallThemeSuccess:
	        // Installation completed - Update status
	        iHspsClient->GethspsResult( *iResult );
	        break;
	    case EhspsInstallPhaseSuccess:
	        // Continue installation
	        iHspsClient->hspsInstallNextPhaseL( *iHeader );
	        break;
	    default:
	        // Installation failed
	        User::Leave( KErrGeneral );
	        break;
	    }
    } 

void MT_CHspsInstallationService::UninstallConfigurationL(
    TInt aAppUid,
    TInt aConfUid )
    {
    ChspsODT* odt = ChspsODT::NewL();
    CleanupStack::PushL( odt );
    odt->SetRootUid( aAppUid );
    odt->SetThemeUid( aConfUid );
    odt->SetProviderUid( 270513751 );
    
    ThspsServiceCompletedMessage ret = iHspsClient->hspsRemoveThemeL( *odt );
    
    CleanupStack::PopAndDestroy( odt );
    }

// -----------------------------------------------------------------------------
// MT_CHspsInstallationService::HandlehspsClientMessage()
// 
// Handles events received from themeserver via hspsClient
// -----------------------------------------------------------------------------

void MT_CHspsInstallationService::HandlehspsClientMessage(
    ThspsServiceCompletedMessage aEvent )
	{
    TInt errorCode = KErrNone;
	
	TBuf<KMaxHeaderDataLength8> tmp;
	iHspsClient->GethspsResult(*iResult);

	switch( aEvent )
	    {
        case EhspsInstallThemeFailed:
            // Flow through
	    case EhspsInstallThemeSuccess:
	        // Installation completed
	        iLoop->AsyncStop();
	        break;
	    default:
	        // No operation - Continue installation
	        break;
	    }	
	}

// End of File
