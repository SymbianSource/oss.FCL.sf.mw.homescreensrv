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

_LIT(KPrivateInstallC, "C:\\private\\200159c0\\install\\");
_LIT(KPrivateInstallE, "E:\\private\\200159c0\\install\\");
_LIT(KPrivateInstallZ, "Z:\\private\\200159c0\\install\\");

_LIT(KBackslash, "\\");
_LIT(KHsps, "hsps");
_LIT(KTestLanguage, "00");
_LIT(KManifest, "manifest.dat");

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
    }

// -----------------------------------------------------------------------------
// ChspsRomInstaller::InstallL()
// -----------------------------------------------------------------------------
//
void ChspsRomInstaller::InstallL()
    {
    RPointerArray<HBufC> fileArray;
    CleanupResetAndDestroyPushL( fileArray );
            
    // Find UDA and ROM widgets to be installed     
    FindInstallationFilesL( fileArray );
            
    // Install the manifest files    
    for( TInt index=0; index < fileArray.Count(); index++ )
        {         
        TPtrC namePtr( fileArray[index]->Des() );                               
                
        // Synchronous method
        ThspsServiceCompletedMessage ret = EhspsInstallThemeFailed;
        TRAP_IGNORE( ret = InstallThemeL( namePtr  ) );
        if ( ret != EhspsInstallThemeSuccess )
            {
//            User::Leave( KErrAbort );
            }
        }
    
    if ( fileArray.Count() == 0 )
        {
        // Mandatory plugins were missing 
        User::Leave( KErrCorrupt );
        }
            
    CleanupStack::PopAndDestroy(); // fileArray
    }


// -----------------------------------------------------------------------------
// ChspsRomInstaller::FindInstallationFilesL()
// -----------------------------------------------------------------------------
//
void ChspsRomInstaller::FindInstallationFilesL(  
        RPointerArray<HBufC>& aFileArray )
    {
    __ASSERT_DEBUG( aFileArray.Count() == 0, User::Leave( KErrArgument ) );
                    
    DoFindInstallationFilesL( aFileArray, KPrivateInstallE );
    DoFindInstallationFilesL( aFileArray, KPrivateInstallC );    
    DoFindInstallationFilesL( aFileArray, KPrivateInstallZ );    
    }

// -----------------------------------------------------------------------------
// ChspsRomInstaller::DoFindInstallationFilesL()
// -----------------------------------------------------------------------------
//
void ChspsRomInstaller::DoFindInstallationFilesL(  
        RPointerArray<HBufC>& aFileArray,
        const TDesC& aPath )
    {               
    TFindFile fileFinder( iFsSession );    
    fileFinder.SetFindMask( 
         KDriveAttExclude|KDriveAttRemovable|KDriveAttRemote|KDriveAttSubsted );
    CDir* dirList( NULL );    
    fileFinder.FindWildByPath( aPath, NULL, dirList );
    if ( dirList )
        {
        CleanupStack::PushL( dirList );
                     
        const TInt count = dirList->Count();        
        for( TInt i = 0; i < count; i++ )
            {
            const TEntry& dirEntry = (*dirList)[i];                        
            if ( dirEntry.IsDir() )
                {                                              
                TFileName manifest( aPath );
                manifest.Append( dirEntry.iName );
                manifest.Append( KBackslash );
                manifest.Append( KHsps );
                manifest.Append( KBackslash );
                manifest.Append( KTestLanguage );
                manifest.Append( KBackslash );
                manifest.Append( KManifest );                                
                                                
                // Check for duplicates
                TBool isShadowed = EFalse;
                TParsePtrC manifestPtr( manifest );                
                for( TInt i=0; i < aFileArray.Count(); i++ )
                    {
                    TParsePtrC ptr( aFileArray[i]->Des() );
                    if( ptr.Path() == manifestPtr.Path() )
                        {
                        isShadowed = ETrue;
                        break;
                        }
                    }
                
                if( !isShadowed )
                    {           
                    if( BaflUtils::FileExists( iFsSession, manifest ) )
                        {                        
                        HBufC* nameBuf = manifest.AllocLC();                
                        aFileArray.AppendL( nameBuf );
                        CleanupStack::Pop( nameBuf );
                        }
                    }
                }
            }
        
        CleanupStack::PopAndDestroy( dirList );
        dirList = 0;
        }           
    }

// -----------------------------------------------------------------------------
// ChspsRomInstaller::FindInstallationFileL
// -----------------------------------------------------------------------------
//
void ChspsRomInstaller::FindInstallationFileL(  
        const TInt aConfigurationUid,
        TFileName& aManifest )
    {                                
    aManifest.FillZ();
    
    _LIT(KFormat, "*_%X");    
    TFileName fileMask;
    fileMask.Format( KFormat, aConfigurationUid );
                            
    TFindFile fileFinder( iFsSession );
    CDir* dirList( NULL );           
    fileFinder.FindWildByDir( fileMask, KPrivateInstallZ, dirList );
    if ( !dirList )
        {
        User::Leave( KErrNotFound );
        }
    
    CleanupStack::PushL( dirList );                

    const TInt count = dirList->Count();        
    for( TInt i = 0; i < count; i++ )
        {        
        const TEntry& dirEntry = (*dirList)[i];                        
        if ( dirEntry.IsDir() )
            {
            // Populate path for the manifest file
            const TEntry& folderEntry = (*dirList)[i];
                        
            aManifest.Copy( KPrivateInstallZ );
            aManifest.Append( dirEntry.iName );
            aManifest.Append( KBackslash );
            aManifest.Append( KHsps );
            aManifest.Append( KBackslash );
            aManifest.Append( KTestLanguage );
            aManifest.Append( KBackslash );
            aManifest.Append( KManifest );
            break;
            }
        }            
    CleanupStack::PopAndDestroy( dirList );               
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
        iRet = EhspsInstallThemeFailed;
        
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
    __ASSERT_DEBUG( aAppUid > 0 && aConfigurationUid > 0, User::Leave( KErrArgument ) );                 
    
    ThspsServiceCompletedMessage ret = EhspsInstallThemeFailed;
    
    // Find an installation file from the ROM
    TFileName manifest;
    FindInstallationFileL( 
        aConfigurationUid,
        manifest );                           
    if ( manifest.Length() > 0 )
        {                                
        // Install the plugin configuration (sync request)
        ret = InstallThemeL( manifest );                
        }        
    if( ret == EhspsInstallThemeSuccess )
        {                          
        // The installed application configuration should now hold only plugin references,
        // in addition it hasn't been updated to the header cache        
        iThemeServer.UpdateHeaderListCacheL();
                        
        // Complete reinstallation of the application configuration
        ChspsODT* odt = ChspsODT::NewL();
        CleanupStack::PushL( odt );
        User::LeaveIfError( iThemeServer.GetConfigurationL( aAppUid, aConfigurationUid, *odt ) );               
        if ( odt->ConfigurationType() == EhspsAppConfiguration )
            {    
            ChspsClientRequestHandler* clientReqHandler = ChspsClientRequestHandler::NewL( iThemeServer );
            CleanupStack::PushL( clientReqHandler );
            
            // Append configurations from referred plugins to the application configuration's DOM
            clientReqHandler->HandlePluginReferencesL( *odt );
            
            CleanupStack::PopAndDestroy( clientReqHandler );
            }
        CleanupStack::PopAndDestroy( odt );
        }
    
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
    iRet = EhspsInstallThemeFailed;
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
