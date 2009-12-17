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
*/


#include "hsps_builds_cfg.hrh"

#include <e32base.h>
#include <f32file.h>
#include <bautils.h>
#include "hspsrominstaller.h"
#include "hspsthemeserver.h"
#include "hspsinstallationhandler.h"
#include "hspsclientrequesthandler.h"
#include "hspsserverutil.h"

_LIT( KInstallDirectoryZ, "z:\\private\\200159c0\\install\\" );
_LIT( KHsps, "hsps");

// ========================= MEMBER FUNCTIONS ==================================

// -----------------------------------------------------------------------------
// ChspsRomInstaller::NewL()
// Two-phased constructor.
// -----------------------------------------------------------------------------
ChspsRomInstaller* ChspsRomInstaller::NewL( 
		ChspsThemeServer& aThemeServer,
		RFs& aFsSession )
    {
    ChspsRomInstaller* self = NewLC( aThemeServer, aFsSession );
    CleanupStack::Pop( self );
    return( self ) ;
    }

// -----------------------------------------------------------------------------
// ChspsRomInstaller::NewLC()
// Two-phased constructor.
// -----------------------------------------------------------------------------
ChspsRomInstaller* ChspsRomInstaller::NewLC( 
		ChspsThemeServer& aThemeServer,
		RFs& aFsSession)
    {
    ChspsRomInstaller* self = new ( ELeave ) ChspsRomInstaller( aThemeServer, aFsSession );
    CleanupStack::PushL( self );
    self->ConstructL();
    return self;
    }

// -----------------------------------------------------------------------------
// ChspsRomInstaller::ConstructL()
// Symbian 2nd phase constructor can leave.
// -----------------------------------------------------------------------------
//
void ChspsRomInstaller::ConstructL()
    {    
    iInstallationHandler = ChspsInstallationHandler::NewL( iThemeServer );    
    
    // Prevent notifications from ROM based installations
    iInstallationHandler->DisableNotifications();
    }

// -----------------------------------------------------------------------------
// ChspsRomInstaller::ChspsRomInstaller()
// C++ default constructor can NOT contain any code, that might leave.
// -----------------------------------------------------------------------------
ChspsRomInstaller::ChspsRomInstaller(
		ChspsThemeServer& aThemeServer,
        RFs& aFsSession )
		: CActive(EPriorityStandard), 
		    iThemeServer(aThemeServer), 
		    iFsSession(aFsSession),
		    iRet(EhspsInstallThemeFailed)
    {
    CActiveScheduler::Add( this );
    }

// -----------------------------------------------------------------------------
// ChspsRomInstaller::~ChspsRomInstaller()
// Destructor.
// -----------------------------------------------------------------------------
//
ChspsRomInstaller::~ChspsRomInstaller()
    {            
    Cancel(); // Causes call to DoCancel()    
    delete iInstallationHandler; 
    iImportsArrayV1.ResetAndDestroy();    
    }

// -----------------------------------------------------------------------------
// ChspsRomInstaller::SetImportsFilterL()
// -----------------------------------------------------------------------------
//
TBool ChspsRomInstaller::SetImportsFilterL(      
        const TDesC& aFileFilter )                        
    {
    TFindFile fileFinder( iFsSession );
    CDir* fileList( NULL );
    fileFinder.FindWildByDir( aFileFilter, KInstallDirectoryZ, fileList );
    if ( fileList )
        {
        CleanupStack::PushL( fileList );
                
        TFileName sourceName;               
        for( TInt i = 0; i < fileList->Count(); i++ )       
            {
            const TEntry& entry = (*fileList)[i];                        
            sourceName.Copy( KInstallDirectoryZ );          
            sourceName.Append( entry.iName );                   
            iImportsArrayV1.AppendL( sourceName.AllocL() );            
            }
        
        CleanupStack::PopAndDestroy( fileList );
        fileList = NULL;
        }
           
    return EFalse;
    }

// -----------------------------------------------------------------------------
// ChspsRomInstaller::GetInstallationFoldersL()
// -----------------------------------------------------------------------------
//
void ChspsRomInstaller::GetInstallationFoldersL(  
        RPointerArray<HBufC>& aFolders )
    {
    aFolders.ResetAndDestroy();
    
    _LIT(KAllFolders, "*");    
    _LIT(KFolderSuffix, "\\");
    CDir* fileList( NULL );    
    TFindFile fileFinder( iFsSession );    
    fileFinder.FindWildByDir( KAllFolders, KInstallDirectoryZ, fileList );        
    if ( fileList )
        {
        CleanupStack::PushL( fileList );
        
        TFileName sourceName;
        TBool verChecked = EFalse;
        for( TInt i = 0; i < fileList->Count(); i++ )       
            {
            const TEntry& entry = (*fileList)[i];                        
            if ( entry.IsDir() )
                {                                  
                const TEntry& entry = (*fileList)[i];                        
                sourceName.Copy( KInstallDirectoryZ );          
                sourceName.Append( entry.iName );
                sourceName.Append( KFolderSuffix );

                if ( !verChecked )
                    {
                    // Check whether the V2 directory structure is available
                    TFileName nameV2;
                    nameV2.Copy( sourceName );                    
                    nameV2.Append( KHsps );
                    nameV2.Append( KFolderSuffix );
                    if( !BaflUtils::FolderExists( iFsSession, nameV2 ) )
                        {
                        CleanupStack::PopAndDestroy( fileList );
                        return;            
                        }
                    verChecked = ETrue;
                    }
                
                aFolders.AppendL( sourceName.AllocL() );                    
                }
            }
        
        CleanupStack::PopAndDestroy( fileList );
        fileList = NULL;
        }            
    }

void ChspsRomInstaller::FindImportsV1L()
    {
    iImportsArrayV1.ResetAndDestroy();
    SetImportsFilterL( KFilterAllPluginImportsV1 );            
    SetImportsFilterL( KFilterAllAppImportsV1 );
    }

// -----------------------------------------------------------------------------
// ChspsRomInstaller::ImportsV1
// -----------------------------------------------------------------------------
//
const RPointerArray<HBufC>& ChspsRomInstaller::ImportsV1()
    {
    return iImportsArrayV1;
    }

// -----------------------------------------------------------------------------
// ChspsRomInstaller::InstallThemeL
// -----------------------------------------------------------------------------
//
ThspsServiceCompletedMessage ChspsRomInstaller::InstallThemeL( 
		const TDesC& aFileName )
    {            	
    // Start installation by reading the manifest file
    iRet = iInstallationHandler->hspsInstallTheme( aFileName, iHeaderData );    
    if ( iRet == EhspsInstallThemeSuccess && !IsActive() )
        {                
        // Continue with remaining installation phases
        SetActive();
        iInstallationHandler->hspsInstallNextPhaseL( iHeaderData, iStatus );        
        
        // Wait until the installation phases have been executed (async->sync) 
        CActiveScheduler::Start();
        }
    
    return iRet;
    }

// -----------------------------------------------------------------------------
// ChspsRomInstaller::ReinstallThemeL
// -----------------------------------------------------------------------------
//
ThspsServiceCompletedMessage ChspsRomInstaller::ReinstallThemeL(
        const TInt aAppUid,
        const TInt aConfigurationUid )
    {    
    ThspsServiceCompletedMessage ret = EhspsInstallThemeFailed;
    
    iImportsArrayV1.ResetAndDestroy();
    
    if ( aAppUid > 0 && aConfigurationUid > 0 )
        {
        // Setup a filter for finding a specific import
        _LIT(KFormat, "app_%X_*_%X_1.0.dat");    
        HBufC* filter = HBufC::NewLC( KMaxFileName );
        filter->Des().AppendFormat( KFormat, aAppUid, aConfigurationUid );                        
        SetImportsFilterL( *filter );        
        CleanupStack::PopAndDestroy( filter );
        
        // There should be only one import matching the UIDs
        if ( iImportsArrayV1.Count() == 1 )
            {           
            // Get path for a manifest from the import's file name
            HBufC* manifestBuf = iThemeServer.GetManifestFromImportLC( 
                    iImportsArrayV1[0]->Des(),
                    KInstallDirectoryZ );
            if ( manifestBuf )
                {         
                // Sync request
                ret = InstallThemeL( manifestBuf->Des() );
                CleanupStack::PopAndDestroy( manifestBuf );
                }
            }
        
        iImportsArrayV1.ResetAndDestroy();
        }
    
    // Complete application configuration reinstallation
    ChspsODT* odt = ChspsODT::NewL();
    CleanupStack::PushL( odt );
    User::LeaveIfError( iThemeServer.GetConfigurationL( aAppUid, aConfigurationUid, *odt ) );
    if ( odt->ConfigurationType() == EhspsAppConfiguration )
        {
        // Add plugin configurations to the application configuration
        ChspsClientRequestHandler* clientReqHandler = ChspsClientRequestHandler::NewL( iThemeServer );
        CleanupStack::PushL( clientReqHandler );
        clientReqHandler->HandlePluginReferencesL( *odt );
        CleanupStack::PopAndDestroy( clientReqHandler );
        }
    CleanupStack::PopAndDestroy( odt );
    
    return ret;
    }

// -----------------------------------------------------------------------------
// ChspsRomInstaller::SetLogBus()
// -----------------------------------------------------------------------------
//
#ifdef HSPS_LOG_ACTIVE        
void ChspsRomInstaller::SetLogBus( ChspsLogBus* aLogBus )
    {
    iLogBus = aLogBus;
    iInstallationHandler->SetLogBus( aLogBus );
    }
#endif

// -----------------------------------------------------------------------------
// ChspsRomInstaller::RunError
// -----------------------------------------------------------------------------
//
TInt ChspsRomInstaller::RunError( TInt /*aError*/ )
    {
    // Called when error occurred in asynchronous request
    CActiveScheduler::Stop();    
    return KErrNone;
    }  
  
// -----------------------------------------------------------------------------
// ChspsRomInstaller::RunL
// -----------------------------------------------------------------------------
//
void ChspsRomInstaller::RunL()
    {
    iRet = (ThspsServiceCompletedMessage)iStatus.Int();
    switch ( iStatus.Int() )
        {                     
        case EhspsInstallPhaseSuccess:
        	{
        	// Execute next phase of the installation
            if ( !IsActive() )
                {                
                SetActive();
                iInstallationHandler->hspsInstallNextPhaseL( iHeaderData, iStatus  );
                }
        	}
        	break;
        	
        case EhspsInstallThemeSuccess:
        case EhspsInstallThemeFailed:                        
        default:
            {
            // Allow continuation of the InstallTheme function
            CActiveScheduler::Stop();            
            }
        	break;
        }
    }

// -----------------------------------------------------------------------------
// ChspsRomInstaller::DoCancel()
// -----------------------------------------------------------------------------
//
void ChspsRomInstaller::DoCancel()
    {
    // Cancels any outstanding operation - nothing to do  
    }
        
// End of File
