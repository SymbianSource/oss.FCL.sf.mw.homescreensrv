/*
* Copyright (c) 2004,2008 Nokia Corporation and/or its subsidiary(-ies).
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


#include "hspsthemeinstallercons.h"
#include <e32svr.h>
#include <flogger.h>
#include <f32file.h>
#include "hspsthememanagement.h"
#include <e32base.h>
#include <bacline.h>
#include <bautils.h>

#include "hspsodt.h"
#include "hspsclient.h"

_LIT(KDATFileExtension,".dat");
_LIT(KLogFile,"hspsthemeinstallercons.log");
_LIT(KLogFolder,"hspsthemeinstallercons");
_LIT(KLogFilePath,"c:\\logs\\hspsthemeinstallercons\\hspsthemeinstallercons.log");
_LIT(KThemeSourcePath,"c:\\data\\installs\\test\\");
_LIT(KDefaultInput, "install.dat");

#define _WRITELOG(aDescription) RFileLogger::Write( KLogFolder,KLogFile,EFileLoggingModeOverwrite,	aDescription );   
#define _WRITE_ERRORLOG(aErrCode) WriteErrorLog(aErrCode);   



class CThemeInstaller:public CBase, public MhspsThemeManagementServiceObserver
    {
    public:
    	~CThemeInstaller();
        static CThemeInstaller* NewL(TInt aAppUid);
        static CThemeInstaller* NewLC(TInt aAppUid);
        void StartL();
	
	private:
    	void InstallThemeL(const TDesC& aManifestFileName);
    	void InstallFilesL(CDesCArraySeg& aManifestFile);
    	void GetFilesL(RFs& aFs, CDesCArraySeg& aFileList, const TDesC& aDirectory);
        void DeleteDirL(RFs& aFs, const TDesC& aDir);
    	void HandlehspsClientMessage(ThspsServiceCompletedMessage aMessage);
        void ConstructL(TInt aAppUid);
        void GetDirectoriesL(
        		RFs& aFs,
        		TPtrC aPath,
        		TPtrC aPluginsFile,
        		CDesCArrayFlat& aDirectoryList );
        void InstallFilesFromL( 
        		RFs& aFs,        		
        		TPtrC aDirectory );
    private:
        TBuf8<KMaxHeaderDataLength8> iHeaderData;
        TRequestStatus iStatus;
        ChspsClient *ihspsClient;
        ChspsResult* iResult;
        TInt iConvertedResourceCount;
        ChspsODT* iHeader;
        TInt iListCount;
        CDesCArraySeg* iThemeList;
        TInt iAppUid;
    };

CThemeInstaller* themeinstaller;
CActiveScheduler* scheduler;





// -----------------------------------------------------------------------------
// E32Main
// -----------------------------------------------------------------------------
//
GLDEF_C TInt E32Main() // main function called by E32
    {
	CTrapCleanup* cleanup = CTrapCleanup::New(); // get clean-up stack
	TRAPD( error, startupL() ); // more initialization, then do example
	
	delete scheduler;
	delete themeinstaller;
	delete cleanup; // destroy clean-up stack
			
	// Forward errors to be catched with ERRORLEVEL method in batch files
	return error; // and return
    }
    
// -----------------------------------------------------------------------------
// WriteErrorLog
// 
// Writes error to log
// -----------------------------------------------------------------------------
//
void WriteErrorLog(TInt error)
    {
    if (error == ENoFilesNotFound)
        {
         _WRITELOG( _L("ERROR::Theme files not found") ); 
        }
    else if (error == EErrorDeletingDir)
        {
         _WRITELOG( _L("ERROR::Could not delete themes") ); 
        }
    else if (error == EErrorDirNotExists)
        {
         _WRITELOG( _L("ERROR::Source directory does not exist") ); 
        }
    else
        {
         _WRITELOG( _L("ERROR::Unknown error") ); 
        }
    }
 
 
// -----------------------------------------------------------------------------
// startup
// 
// 
// -----------------------------------------------------------------------------
//
void startupL()
    {    
    TInt errorCode = KErrNone;   
     
    // Start active scheduler
    scheduler = new ( ELeave ) CActiveScheduler;
    CleanupStack::PushL( scheduler );
    CActiveScheduler::Install( scheduler );

    TUid appuid = KUidhspsThemeInstallerCons;
    themeinstaller = CThemeInstaller::NewL( appuid.iUid );
    CleanupStack::PushL( themeinstaller );
     
    TRAP( errorCode, themeinstaller->StartL() );
    
    _WRITELOG( _L("") );
    
    if( errorCode )
        {
	    _WRITE_ERRORLOG( errorCode );
	    _WRITELOG( _L("Failed to start an installation!") );    
	    }

		_WRITELOG( _L("Installer done") );
    
    CleanupStack::Pop( themeinstaller );
    CleanupStack::Pop( scheduler );
     
    User::LeaveIfError( errorCode );     
    }
    
// -----------------------------------------------------------------------------
// CThemeInstaller::NewL()
// 
// 
// -----------------------------------------------------------------------------
//
CThemeInstaller* CThemeInstaller::NewL( TInt aAppUid )
    {    
    CThemeInstaller* self = NewLC( aAppUid );
    CleanupStack::Pop( self );
    return( self );    
    }

// -----------------------------------------------------------------------------
// CThemeInstaller::NewLC()
// 
// 
// -----------------------------------------------------------------------------
//
CThemeInstaller* CThemeInstaller::NewLC( TInt aAppUid )
    {    
    CThemeInstaller* self = new ( ELeave ) CThemeInstaller();
    CleanupStack::PushL( self );
    self->ConstructL( aAppUid );
    return self;    
    }

// -----------------------------------------------------------------------------
// CThemeInstaller::ConstructL()
// 
// 
// -----------------------------------------------------------------------------
//
void CThemeInstaller::ConstructL( TInt aAppUid )
    {
    iResult = ChspsResult::NewL();
  	ihspsClient = ChspsClient::NewL( *this );
  	iHeader = NULL;
  	iListCount = 0;
  	iConvertedResourceCount = 0;
  	iAppUid = aAppUid;
  	iThemeList = NULL;  	
    }
   
// -----------------------------------------------------------------------------
// CThemeInstaller::~CThemeInstaller()
// 
// 
// -----------------------------------------------------------------------------
//
CThemeInstaller:: ~CThemeInstaller()
    {    
    delete ihspsClient;
    delete iResult;
    delete iHeader;
    delete iThemeList;   
    }
     
// -----------------------------------------------------------------------------
// Gets program arguments and starts the installer
// -----------------------------------------------------------------------------
//
void CThemeInstaller::StartL()
    {    
    // Init
    RFs fs;           
    User::LeaveIfError( fs.Connect() );
    CleanupClosePushL( fs );
    
    // Set logging
    fs.MkDirAll( KLogFilePath );
    _WRITELOG(_L("Starting installer"));
                       
    // Check arguments
    CCommandLineArguments* args = CCommandLineArguments::NewLC();
    TInt argsCount = args->Count();
    TPtrC inputFile;    
    const TInt firstArgIndex = 2;
    if ( argsCount < firstArgIndex )
        {
        _WRITELOG(_L("Missing an argument!"));
//        User::LeaveIfError( KErrArgument );        
        inputFile.Set( KDefaultInput );
        }
    else
        {
        inputFile.Set( args->Arg(1) );        
        }
    
    // Setup an array for directory names
    const TInt KGranularity = 10;
    CDesCArrayFlat* directoryList = new ( ELeave ) CDesCArrayFlat( KGranularity );
    CleanupStack::PushL( directoryList );
            
    // Get directory names
    GetDirectoriesL( fs, KThemeSourcePath().Ptr(), inputFile, *directoryList );        
        
    // Install manifest files from the directories
	TInt count = directoryList->MdcaCount();
	for( TInt i=0; i<count; i++ )
		{				
		TPtrC dirPtr( directoryList->MdcaPoint(i) );		
		InstallFilesFromL( fs, dirPtr );
		}
	    
    CleanupStack::PopAndDestroy( 3, &fs ); // directoryList, args, Fs   
    }

// -----------------------------------------------------------------------------
// Installs a new configuration from the provided manifest file.
// -----------------------------------------------------------------------------
//
void CThemeInstaller::InstallFilesFromL( 
		RFs& aFs,
		TPtrC aDirectory )
	{
	TInt errorCode = EErrorDirNotExists;
	
    // Check if the dir exists
    BaflUtils baf;
    if( baf.PathExists( aFs, aDirectory ) )
        {
        errorCode = KErrNone;
        
        // Setup an array for manifests (member variable)
        if ( iThemeList )
        	{
        	delete iThemeList;
        	iThemeList = NULL;
        	}
        iThemeList = new( ELeave ) CDesCArraySeg( KArrGranularity );        
                
        // Get manifests located at the directory
        GetFilesL( aFs, *iThemeList, aDirectory );
        if( iThemeList->Count() > 0 )
            {
            // Install first manifest
       	    InstallThemeL( iThemeList->MdcaPoint( 0 ) );       	           	    
       	 
       	    // Handle asynch messages
       	    scheduler->Start();       	    
            }
        else
            {
            errorCode = ENoFilesNotFound;
            }                   
        }
                 
    User::LeaveIfError( errorCode );
    }
     
// -----------------------------------------------------------------------------
// Reads directory names from the input file and appends names into the provided array
// -----------------------------------------------------------------------------
//
void CThemeInstaller::GetDirectoriesL(
		RFs& aFs,
		TPtrC aPath,
		TPtrC aPluginsFile,
		CDesCArrayFlat& aDirectoryList )
	{			
	TFileName directoryFile;
	directoryFile.Copy( aPath );        
	directoryFile.Append( aPluginsFile );
	
	// Convert into a 8-bit descriptor
	_LIT8(KLogPrefix, "Reading directory names from the " );
	_LIT8(KLogSuffix, " file.." );
	const TInt lineLength = KLogPrefix().Length() + directoryFile.Length() + KLogSuffix().Length();
	HBufC8* line = HBufC8::NewLC(lineLength );
	TPtr8 linePtr( line->Des() );	
	linePtr.Append( KLogPrefix ); 	
	linePtr.Append( directoryFile );
	linePtr.Append( KLogSuffix );	
	_WRITELOG( linePtr );
	CleanupStack::PopAndDestroy( line );
	line = NULL;
	
	// Check if the f exists
    BaflUtils baf;
    if( !baf.FileExists( aFs, directoryFile ) )
    	{
    	_WRITELOG(_L("File not found!"));
    	User::LeaveIfError( KErrNotFound );
    	}
		
	// Open a file handle
	RFile file;
	User::LeaveIfError( file.Open(aFs, directoryFile, EFileRead|EFileShareReadersOnly) );
	CleanupClosePushL(file);
	
	// Get size of the file
    TInt fileSize;
    User::LeaveIfError( file.Size(fileSize) );
        
    // Get file content
    HBufC8* buf8 = HBufC8::NewLC( fileSize );    
    TPtr8 ptr8( buf8->Des() );
    User::LeaveIfError( file.Read(ptr8) );
    
    // Convert from a 8bit to 16bit descriptor
    HBufC16* buf16 = HBufC16::NewLC( ptr8.MaxLength() );
    TPtr16 ptr16 = buf16->Des();
    ptr16.Copy( ptr8 );
           
    // Parse directories from the string    
    TInt offset = 0;
    _LIT(KLineSeperator, "\r\n");
    _LIT(KDirectorySuffix, "\\");
    do     
    	{
    	offset = ptr16.Find( KLineSeperator );
    	TPtrC ptr;
    	if ( offset < 1 )
    		{
    		ptr.Set( ptr16 );
    		}
    	else
    		{
    		ptr.Set( ptr16.Left( offset ) );
    		}    	
    	if ( ptr.Length() )
    		{
	    	HBufC* nameBuf = HBufC::NewLC( aPath.Length() + ptr.Length() + KDirectorySuffix().Length() );
	    	TPtr fullPtr( nameBuf->Des() );
	    	fullPtr.Copy( aPath );
	    	fullPtr.Append( ptr );
	    	fullPtr.Append( KDirectorySuffix );
	    	aDirectoryList.AppendL( fullPtr );
	    	CleanupStack::PopAndDestroy( nameBuf );
    		}
    	if ( offset > 0 )
    		{
    		ptr16 = ptr16.Mid( offset + KLineSeperator().Length() );
    		}
    	}
    while ( offset > 0 );    
           
    // Cleanup
    CleanupStack::PopAndDestroy( buf16 ); 
    CleanupStack::PopAndDestroy( buf8 );
    CleanupStack::PopAndDestroy( ); // file              
	}

// -----------------------------------------------------------------------------
// Deletes theme installation path
// -----------------------------------------------------------------------------
//
void CThemeInstaller::DeleteDirL( RFs& aFs, const TDesC& aDir )
    {    
    _WRITELOG( _L("Deleting existing themes") );
    
    HBufC* path = HBufC::NewLC( KMaxFileName );
    TPtr pathPtr = path->Des();
    pathPtr.Append( aDir );
    
    CFileMan* fileMan = CFileMan::NewL( aFs );
    CleanupStack::PushL( fileMan );
    TInt err = fileMan->RmDir( pathPtr );
    
    CleanupStack::PopAndDestroy( fileMan );
    CleanupStack::PopAndDestroy( path );
    
    if( err )
        {
        _WRITE_ERRORLOG( EErrorDeletingDir );  
        }  
    }

// -----------------------------------------------------------------------------
// Searches for theme files from path and adds them to list
// -----------------------------------------------------------------------------
//
void CThemeInstaller::GetFilesL( RFs& aFs, CDesCArraySeg& aFileList, const TDesC& aDirectory )
    {
    CDir* dirList;
    User::LeaveIfError( aFs.GetDir( aDirectory,
                       KEntryAttMaskSupported, ESortByDate, dirList ) );
                       
    CleanupStack::PushL( dirList );                   
                       
    for( TInt i = 0; i < dirList->Count(); i++ )
        {
        if ( !( *dirList )[i].IsDir() )
            {
            HBufC* path = HBufC::NewLC( aDirectory.Length() + ( *dirList )[i].iName.Length() );
            TPtr pathPtr = path->Des();
            pathPtr.Copy( aDirectory );
            TEntry  entry = ( *dirList )[i];
            pathPtr.Append( entry.iName );                
            TParse p;
            p.Set( pathPtr, NULL, NULL );
            
            if ( p.Ext().CompareF( KDATFileExtension ) == 0 )
              	{
                aFileList.AppendL( pathPtr );
        	    }
            
            CleanupStack::PopAndDestroy( path );  
            }
        }
    
    CleanupStack::PopAndDestroy( dirList );
    }
    
// -----------------------------------------------------------------------------
// Initiates installing of a confgiruation from the provided manifest file
// -----------------------------------------------------------------------------
//
void CThemeInstaller::InstallThemeL( const TDesC& aManifestFileName )
    {      	
    
    TBuf<KMaxHeaderDataLength8> tmp;
    _WRITELOG(_L(""));
    tmp.Format( _L("Manifest: %S"), &aManifestFileName );
	_WRITELOG(tmp);
    
	if ( iHeader )
		{
		delete iHeader;
		iHeader = NULL;
		}			
	iHeader = ChspsODT::NewL();
    
	TParse p;
    p.Set( aManifestFileName, NULL, NULL );
    TPtrC ptr = p.NameAndExt();
    
	ThspsServiceCompletedMessage ret = ihspsClient->hspsInstallTheme( aManifestFileName, *iHeader );
	
	if( ret == EhspsInstallThemeSuccess )
		{
		ihspsClient->GethspsResult( *iResult );
 		}
	else if( ret == EhspsInstallPhaseSuccess )
		{
		ihspsClient->GethspsResult( *iResult );
		iConvertedResourceCount = 0;
	
  		tmp.Format( _L("Installing ..") );
  		_WRITELOG(tmp);
		ihspsClient->hspsInstallNextPhaseL( *iHeader );
		}
	else if( ret == EhspsInstallThemeFailed )
    	{
    	ihspsClient->GethspsResult(*iResult);
    	TBuf<KMaxHeaderDataLength8> tmp2; 
    	TInt syserr = iResult->iSystemError;
    	TInt hspserr = iResult->iXuikonError;
    	tmp.Format(_L("Installation of \'%S\' failed. System error: %d, HSPS error: %d."), &ptr, syserr, hspserr );
    	_WRITELOG(tmp);    	    	
    	User::Leave( KErrGeneral );
    	}
    else if( ret == EhspsServiceNotSupported )
    	{
    	_WRITELOG(_L("EhspsServiceNotSupported"));
    	}
    else if( ret == EhspsServiceRequestError )
    	{
    	_WRITELOG(_L("EhspsServiceRequestError"));
    	}
    else if( ret == EhspsServiceRequestCanceled )
    	{
    	_WRITELOG(_L("EhspsServiceRequestCanceled"));
    	}
    else
    	{
    	tmp.Format(_L("Undefined response: %d"), ret); 
    	_WRITELOG(tmp);
    	}         
    } 

// -----------------------------------------------------------------------------
// CThemeInstaller::HandlehspsClientMessage()
// 
// Handles events received from themeserver via hspsClient
// -----------------------------------------------------------------------------

void CThemeInstaller::HandlehspsClientMessage(ThspsServiceCompletedMessage aEvent)
	{
    TInt errorCode = KErrNone;
	
	TBuf<KMaxHeaderDataLength8> tmp;
	ihspsClient->GethspsResult(*iResult);
    
    if( aEvent == EhspsInstallThemeSuccess )
		{
   		iListCount++;

   		if ( iListCount < iThemeList->MdcaCount() )
			{
			TRAP( errorCode, InstallThemeL( iThemeList->MdcaPoint( iListCount ) ) );

			if( errorCode )
				{
				tmp.Format( _L("InstallThemeL() failed...") );
				_WRITELOG( tmp );
				}
			}
		else
		    {
            scheduler->Stop();
		    }
   		}   
	else if( aEvent == EhspsInstallPhaseSuccess )
	    {
	    
		TBuf<KMaxHeaderDataLength8> tmp;
		
		if( iResult->iIntValue1 == EhspsPhaseCleanup )
			{
			tmp.Format(_L("Removing previous configuration .."));
			}
		else if( iResult->iIntValue1 == EhspsPhaseInstallSkeleton )
			{
			tmp.Format(_L("Installing a new configuration .."));
			}		
		else
			{
			tmp.Format(_L("Processing phase %d .."), iResult->iIntValue1 );
			}
			
		_WRITELOG(tmp);
 		
 		} 
 	else if( aEvent == EhspsInstallThemeFailed )
		{

    	TBuf<KMaxHeaderDataLength8> tmp2; 
    	TInt syserr = iResult->iSystemError;
    	TInt hspserr = iResult->iXuikonError;
    	TInt defengerr = iResult->iIntValue1;
    	TInt subcomperr = iResult->iIntValue2;
 		  tmp.Format(_L("Installation of /'%S/' failed. Sys.err: %d, hsps.err: %d, Def.eng.err: %d, subcomp.err: %d."), 
 		              &iHeader->ThemeFullName(), syserr, hspserr, defengerr, subcomperr );
 		  _WRITELOG(tmp); 		   
      scheduler->Stop();          	
   		}
  else
    	{      
      tmp.Format(_L("Unknown event received: %d"), aEvent );
	     _WRITELOG(tmp);
	    }
	}

