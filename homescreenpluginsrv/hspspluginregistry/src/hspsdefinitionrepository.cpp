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
* Description:  Service for maintaining contents of the "themes" folder
*
*/


// INCLUDE FILES
#include <s32file.h>
#include <s32mem.h>
#include <sysutil.h>

#include "hsps_builds_cfg.hrh"

#include "hspsthememanagement.h"
#include "hspsdefinitionrepository.h"
#include "hspsodt.h"

#ifdef HSPS_LOG_ACTIVE
#include <hspslogbus.h>
#endif

// CONSTANTS
const TInt KDefRepArrayGranularity = 5;

_LIT(KImportFolder, "import");
_LIT(KThemesFolder, "themes");
_LIT(KSourceFolder, "sources");
_LIT(KSourceFolderWithSeperators, "\\sources\\");
_LIT(KLocaleFolder, "locales");
_LIT(KBackupFolder, "backup");


_LIT(KDoubleBackSlash,"\\");

_LIT(KFolderFormat,"%d"); // folder format for localized resources

_LIT(KMatchAll, "*");


_LIT(KMatchSkeleton, ".o0000");

const TInt KTIntMahspsumbers( 11 );

// ============================= LOCAL FUNCTIONS ===============================
    
// ============================ MEMBER FUNCTIONS ===============================

// -----------------------------------------------------------------------------
// ChspsDefinitionRepository::ChspsDefinitionRepository
// C++ default constructor can NOT contain any code, that
// might leave.
// -----------------------------------------------------------------------------
//
ChspsDefinitionRepository::ChspsDefinitionRepository() :
        iCacheLastODT( NULL )
    {        
    }

// -----------------------------------------------------------------------------
// ChspsDefinitionRepository::ConstructL
// Symbian 2nd phase constructor can leave.
// -----------------------------------------------------------------------------
//
void ChspsDefinitionRepository::ConstructL()
    {
	User::LeaveIfError( iFs.Connect() );
	//Create private path if it doesn't exist already
	TInt err=iFs.CreatePrivatePath(EDriveC);
	if (err!=KErrNone && err!=KErrAlreadyExists)
		{
		User::Leave(err);	
		}
	HBufC* path = HBufC::NewLC( KMaxFileName );
    TPtr pathPtr = path->Des();
    User::LeaveIfError( iFs.PrivatePath( pathPtr ));     	
	AppendDesCIntoPathL( pathPtr, KImportFolder );
    AppendDesCIntoPathL( pathPtr, KDoubleBackSlash );
    iFs.MkDir( pathPtr );
    CleanupStack::PopAndDestroy( path );
    User::LeaveIfError(iFs.SetSessionToPrivate(EDriveC));
	iLicenseDefault = EFalse;
	iLockSemaphore = 0;
	
	iTempFileName1 = KNullDesC;
	iTempFileName2 = KNullDesC;
    }

// -----------------------------------------------------------------------------
// ChspsDefinitionRepository::NewL
// Two-phased constructor.
// -----------------------------------------------------------------------------
//
EXPORT_C ChspsDefinitionRepository* ChspsDefinitionRepository::NewL()
    {
    ChspsDefinitionRepository* self = new( ELeave ) ChspsDefinitionRepository;
    
    CleanupStack::PushL( self );
    self->ConstructL();
    CleanupStack::Pop( self );

    return self;
    }
    
// Destructor
ChspsDefinitionRepository::~ChspsDefinitionRepository()
    {
    iFs.Close();
    iRepositoryInfoQueue.Reset();
    iRepositoryInfoQueue.Close();
    iObservers.Reset();
    iObservers.Close(); 
    delete iPath;
    
    delete iCacheLastODT;    
    
	iTempFileName1 = KNullDesC;
	iTempFileName2 = KNullDesC;    
    }

// -----------------------------------------------------------------------------
// ChspsDefinitionRepository::SetLocaleL
// Saves the dtd-file into the repository.
// (other items were commented in a header).
// -----------------------------------------------------------------------------
EXPORT_C TInt ChspsDefinitionRepository::SetLocaleL( ChspsODT& aODT, ChspsResource& aResource )
    {
    // Populate a path based on ODT attributes and file extension of the resource file
    GetPathL( aODT, EResourceDTD );
    
    // Results are in iPath, copy resouce file from temp path to a new path in the repository
    TInt errorCode = CopyFileL( aResource.FileName() );
    if ( !errorCode )
    	{
    	// Store the new path
    	aResource.SetFileNameL( iPath->Des() );
    	}
    return errorCode;
    }

// -----------------------------------------------------------------------------
// ChspsDefinitionRepository::MakeODTPathL
// Creates a path for given ODT.
// (other items were commented in a header).
// ----------------------------------------------------------------------------- 
EXPORT_C void ChspsDefinitionRepository::MakeODTPathL( ChspsODT& aODT, ChspsResource& aResource )
    {
    // make sure the temporary buffer is empty
    iTempFileName1.Zero();
    
    // get the path for ODT-resource to store
    GetPathL( aODT, EResourceODT );

   	TPtr ptrPath = iPath->Des();
   	TParsePtr p( ptrPath );	
   	aODT.SetThemeShortNameL(p.Name());
    	
   	// iPath is modified
   	TPtrC pathPtr = iPath->Des();
   	iTempFileName1.Format( _L("%S%S"), &KCDrive, &pathPtr );
    aResource.SetFileNameL( iTempFileName1 ); 
   	    
    iTempFileName1.Zero(); 	    
    }
    

// -----------------------------------------------------------------------------
// ChspsDefinitionRepository::SetOdtL
// Saves the odt-structure into the repository.
// (other items were commented in a header).
// -----------------------------------------------------------------------------
//
EXPORT_C TInt ChspsDefinitionRepository::SetOdtL( const ChspsODT &aODT )
    {
    TInt ret = KErrGeneral;

#ifdef HSPS_LOG_ACTIVE
    if( iLogBus )
        {
        iLogBus->LogText( _L( "ChspsDefinitionRepository::SetOdtL(): - rewriting the binary file." ) );
        }
#endif

    GetPathL( aODT, EResourceODT );
    ret = WriteToFileL( aODT);


#ifdef HSPS_LOG_ACTIVE
    if( iLogBus )
        {
        iLogBus->LogText( _L( "ChspsDefinitionRepository::SetOdtL(): - rewriting done. return code: %d." ), ret );
        }
#endif

    if( aODT.ConfigurationType() == EhspsAppConfiguration )
        {
        delete iCacheLastODT;
		iCacheLastODT = NULL;        
        iCacheLastODT = aODT.CloneL();                
        }
    
    return ret;
    }

// -----------------------------------------------------------------------------
// ChspsDefinitionRepository::GetOdtL
// Retrieves the odt-structure from the repository as a reference.
// (other items were commented in a header).
// -----------------------------------------------------------------------------
//
EXPORT_C TInt ChspsDefinitionRepository::GetOdtL( ChspsODT& aODT )
    {
    TInt errorCode = KErrNone;
    
    if ( aODT.Flags() & EhspsThemeStatusLicenceeDefault )
		{
		iLicenseDefault = ETrue;
		}
	else
		{
		iLicenseDefault = EFalse;		
		}

    if( iCacheLastODT &&
        iCacheLastODT->RootUid() == aODT.RootUid() &&
        iCacheLastODT->ThemeUid() == aODT.ThemeUid() )  
        {
        aODT.CloneL( *iCacheLastODT );
        }
    else       
        {    
        TRAP( errorCode, GetPathL( aODT, EResourceODT ));
        if ( !errorCode )
            {
            errorCode = ReadFromFileL( *iPath, aODT ); 
            }         
        }
    
    iLicenseDefault = EFalse;
    
    return errorCode;
    }

// -----------------------------------------------------------------------------
// ChspsDefinitionRepository::GetOdtHeaderL
// Retrieves the odt-structure from the repository as a reference.
// (other items were commented in a header).
// -----------------------------------------------------------------------------
//
EXPORT_C TInt ChspsDefinitionRepository::GetOdtHeaderL( TDes& aPath, TLanguage /*aLang*/, ChspsODT& aODT )
    {    
    TInt err( KErrNotFound );
    
    CDesCArraySeg* themeList = new ( ELeave ) CDesCArraySeg( KDefRepArrayGranularity );
    CleanupStack::PushL( themeList );
    
    SelectFilesFromPathL( *themeList, aPath, ESortByDate, KMatchSkeleton );
    
    if( themeList->MdcaCount() > 0 )
        {
        
        TPtrC name = themeList->MdcaPoint( 0 );
        if ( name.FindF( KSourceFolderWithSeperators ) == KErrNotFound )
        	{
	        HBufC8* header( StreamHeaderFromFileL( name ) );
	        CleanupStack::PushL( header );
	        
	        if( header )
	            {
	            aODT.UnMarshalHeaderL( *header );            
	            err = KErrNone;
	            }
	            
	        CleanupStack::PopAndDestroy( header );            
        	}
        }
    
    CleanupStack::PopAndDestroy( themeList );

    User::LeaveIfError( err );
            
    return err;
    }
    
// -----------------------------------------------------------------------------
// ChspsDefinitionRepository::GetODTPathL
// Retrieves the path of given ODT.
// (other items were commented in a header).
// ----------------------------------------------------------------------------- 
//
EXPORT_C void ChspsDefinitionRepository::GetResourcePathL( const ChspsODT& aODT, ThspsResourceType aResourceType, TDes& aFilePath )
    {
    // get the path for source resources to store
    GetPathL( aODT, aResourceType );
    TPtrC ptr = iPath->Des();
	// modifying the resource file path on the resource
	aFilePath.Format( _L("%S%S"), &KCDrive, &ptr );
    }    

    
// -----------------------------------------------------------------------------
// ChspsDefinitionRepository::GetThemePathListL
// Retrieves the list of installed themes.
// (other items were commented in a header).
// -----------------------------------------------------------------------------
EXPORT_C void ChspsDefinitionRepository::GetThemePathListL( CDesCArraySeg& aThemeList, const ChspsODT& aODT )
    {    
    CDesCArraySeg* pathList = new( ELeave ) CDesCArraySeg(KDefRepArrayGranularity);
    CleanupStack::PushL( pathList );
    
    CDesCArraySeg* tempPathList = new( ELeave ) CDesCArraySeg(KDefRepArrayGranularity);
    CleanupStack::PushL( tempPathList );
               
    // The "themes" folder must exist at this point (C drive)
    _LIT( KDriveC, "C:" );
    TFileName themesPath;
    User::LeaveIfError( iFs.PrivatePath( themesPath ));
    HBufC* path = HBufC::NewLC( KMaxFileName );
    TPtr pathPtr = path->Des();        
    AppendDesCIntoPathL( pathPtr, KDriveC );    
    AppendDesCIntoPathL( pathPtr, themesPath );
    AppendDesCIntoPathL( pathPtr, KThemesFolder );
    AppendDesCIntoPathL( pathPtr, KDoubleBackSlash );    
    
    // Allocate memory for the folder name to be found (uid folder)
    HBufC* match = HBufC::NewLC( KTIntMahspsumbers );
    TPtr matchPtr = match->Des();
           
    // Setup a mask from the search criterias     
    TInt8 rootUid = aODT.RootUid() ? 1 : 0;
    TInt8 provUid = aODT.ProviderUid() ? 2 : 0;
    TInt8 themeUid = aODT.ThemeUid() ? 4 : 0;
    TInt8 mask = rootUid | provUid | themeUid;
    
    TInt index( 0 );
    
    switch ( mask )
        {
        case 0://No data in ODT.
            {
            MatchingFoldersFromAllDrivesL( KMatchAll, pathPtr, *pathList );
            }
            break;
        case 1://AppUid given.
        case 3://AppUid and ProviderUid given.
        case 7://AppUid, ProviderUid and ThemeUid given.
            {
            if ( aODT.RootUid() )
                {
                AppendNumIntoPathL( pathPtr, aODT.RootUid() );
                AppendDesCIntoPathL( pathPtr, KDoubleBackSlash );
                }            
            if ( aODT.ProviderUid() )
                {
                AppendNumIntoPathL( pathPtr, aODT.ProviderUid() );
                AppendDesCIntoPathL( pathPtr, KDoubleBackSlash );
                }                
            if ( aODT.ThemeUid() )
                {
                AppendNumIntoPathL( pathPtr, aODT.ThemeUid() );
                AppendDesCIntoPathL( pathPtr, KDoubleBackSlash );
                }
            MatchingFoldersFromAllDrivesL( KMatchAll, pathPtr, *pathList );
            }
            break;
        case 2://ProviderUid given.
            {
            MatchingFoldersFromAllDrivesL( KMatchAll, pathPtr, *tempPathList );
            matchPtr.Zero();
            matchPtr.AppendNum( aODT.ProviderUid() );
            for ( index = 0; index < tempPathList->Count(); index++ )
                {
                MatchingFoldersL( *match, tempPathList->MdcaPoint( index ), *pathList );
                }
            }
            break;
        case 4://ThemeUid given.
            {
            MatchingFoldersFromAllDrivesL( KMatchAll, pathPtr, *pathList );
            for ( index = 0; index < pathList->Count(); index++ )
                {
                MatchingFoldersL( *match, pathList->MdcaPoint( index ), *tempPathList );
                }
            matchPtr.Zero();
            matchPtr.AppendNum( aODT.ThemeUid() );
            pathList->Reset();
            for ( index = 0; index < tempPathList->Count(); index++ )
                {
                MatchingFoldersL( *match, tempPathList->MdcaPoint( index ), *pathList );
                }
            }
            break;
        case 6://ProviderUid and ThemeUid given.
            {
            MatchingFoldersFromAllDrivesL( KMatchAll, pathPtr, *pathList );
            matchPtr.Zero();
            matchPtr.AppendNum( aODT.ProviderUid() );
            for ( index = 0; index < pathList->Count(); index++ )
                {
                MatchingFoldersL( *match, pathList->MdcaPoint( index ), *tempPathList );
                }
            matchPtr.Zero();
            matchPtr.AppendNum( aODT.ThemeUid() );
            pathList->Reset();
            for ( index = 0; index < tempPathList->Count(); index++ )
                {
                MatchingFoldersL( *match, tempPathList->MdcaPoint( index ), *pathList );
                }
            }
            break;
        case 5://AppUid and ThemeUid given.
            {
            // Get paths matching the 1st argument into pathList array
            matchPtr.Zero();
            matchPtr.AppendNum( aODT.RootUid() );
            MatchingFoldersFromAllDrivesL( *match, pathPtr, *pathList );
            // From previous results, match and store results into tempPathList array
            matchPtr.Zero();
            matchPtr = KMatchAll;
            for ( index = 0; index < pathList->Count(); index++ )
                {
                MatchingFoldersL( *match, pathList->MdcaPoint( index ), *tempPathList );
                }
            // From previous results, find mathing plugins and store them back to the pathList
            matchPtr.Zero();
            matchPtr.AppendNum( aODT.ThemeUid() );
            pathList->Reset();
            for ( index = 0; index < tempPathList->Count(); index++ )
                {
                MatchingFoldersL( *match, tempPathList->MdcaPoint( index ), *pathList );
                }
            }
            break;
        default:
            {
            pathList->AppendL( pathPtr );
            }
            break;
        }

    TInt err( KErrNotFound );
    for ( TInt i = 0; i < pathList->MdcaCount(); i++ )
        {
        err = MatchingFoldersL( KMatchAll, pathList->MdcaPoint(i), *pathList );
        if ( err == KErrNotFound )
            {
            TPtrC pathPtr = pathList->MdcaPoint(i);
           	aThemeList.AppendL( pathPtr );	
            }
        }
    CleanupStack::PopAndDestroy( 4, pathList );//path, match, pathList, tempPathList
    }

// -----------------------------------------------------------------------------
// ChspsDefinitionRepository::GetThemeListAsStreamL
// Retrieves the headers of installed themes as an array of streams.
// (other items were commented in a header).
// -----------------------------------------------------------------------------
//
EXPORT_C void ChspsDefinitionRepository::GetThemeListAsStreamL( CArrayPtrSeg<HBufC8>& aHeaderList, const ChspsODT& aODT )
    {
    CDesCArraySeg* pathList = new( ELeave ) CDesCArraySeg(KDefRepArrayGranularity);
    CleanupStack::PushL( pathList );
    GetThemePathListL( *pathList, aODT );
    CDesCArraySeg* themeList = new( ELeave ) CDesCArraySeg(KDefRepArrayGranularity);
    CleanupStack::PushL( themeList );
    
    for ( TInt i = 0; i < pathList->MdcaCount(); i++ )
        {
        SelectFilesFromPathL( *themeList,  pathList->MdcaPoint( i ), ESortByDate, KMatchSkeleton );
        }
    
    for ( TInt j = 0; j < themeList->MdcaCount(); j++ )
        {
        TPtrC ptr( themeList->MdcaPoint( j ) );
        if ( ptr.FindF( KSourceFolderWithSeperators ) == KErrNotFound )
        	{
	        HBufC8* temp = StreamHeaderFromFileL( themeList->MdcaPoint( j ) );
	        if ( temp )
	            {
	            aHeaderList.AppendL( temp );         
	            }
        	}
        }
    CleanupStack::PopAndDestroy( 2, pathList );//themeList, pathList
    }
 
// -----------------------------------------------------------------------------
// ChspsDefinitionRepository::RemoveThemeL
// Removes the specified theme.
// (other items were commented in a header).
// -----------------------------------------------------------------------------
EXPORT_C void ChspsDefinitionRepository::RemoveThemeL( const ChspsODT& aODT )
    {
    GetPathL( aODT, EResourceNone );
    RemoveDirectoryL();
    // theme removal is not informed to the clients through RepositoryInfoArray 
    // because this function is used also by the theme server's clean up functions - 
    // and it not necessary inform about them.
    // insteads, maintenance handler does inform the removal.
    }


// -----------------------------------------------------------------------------
// ChspsDefinitionRepository::MakeResourcePathL
// Makes a path in the repository that can be used to store the resource file. 
// The path is written back into aResource  
// The path depends on the locale under which the resource belongs.
// The path information will be used to retrieve the source later.
// (other items were commented in a header).
// -----------------------------------------------------------------------------    
EXPORT_C void ChspsDefinitionRepository::MakeResourcePathL( const ChspsODT& aODT, ChspsResource& aResource )
	{
    // get the path for source resources to store (results are stored into the iPath instance)
    GetPathL( aODT, aResource.ResourceType() );
    TPtr pathPtr = iPath->Des();
    
    // If the resource is of type "Other" then skip renaming of the file extension
    if ( aResource.ResourceType() == EResourceOther )
    	{
    	if ( aResource.Language() != ELangNone )
    	    {    	    
    	    AddLocalePathL( pathPtr, aODT.OdtLanguage() );
    	    AppendDesCIntoPathL( pathPtr, KDoubleBackSlash );
    	    }

    	// Append the iPath with the resource name
    	AppendDesCIntoPathL( pathPtr, aResource.ResourceId() );
    	
    	// Append with original file extension or fix it
    	const TDesC& name = aResource.FileName();
    	TParsePtr parsePtr( (TDes&)name );		   				
        TPtrC fileExtension = parsePtr.Ext();
        if ( fileExtension.Length() > 2 && fileExtension.Left(2).CompareF( _L(".o") ) == 0 )
            {            
            // Strip the first letter            
            TInt odtIndex(0);                  
            TLex lex( fileExtension.Mid(3) );
            if ( lex.Val( odtIndex ) == KErrNone && odtIndex >= 0 )
                {                
                fileExtension.Set( KMatchSkeleton );
                }
            }		
		AppendDesCIntoPathL( pathPtr, fileExtension );    		
    	}
    else
    	{
	    // If replaceable resource
	    if ( aResource.LockingPolicy() == EhspsUnlocked )
	    	{
	    	AppendDesCIntoPathL( pathPtr, aResource.ResourceId() );	    		    		    		
    		FileExtensionL( aResource.ResourceType(), aODT.OdtLanguage() );	    		
	    	}
	    else
	    	{
	    	// make phycical path
		    User::LeaveIfError(iFs.SetSessionToPrivate(EDriveC));
		    // create the directory structure on the path
		    TInt err = iFs.MkDirAll( *iPath );
		    if ( err != KErrNone && err != KErrAlreadyExists )
		        {
		        User::Leave( err );
		        }
		    AppendDesCIntoPathL( pathPtr, aResource.ResourceId() );    
	   	    FileExtensionL( aResource.ResourceType(), aODT.OdtLanguage() );
	    	}
    	}
    aResource.SetFileNameL( iPath->Des() );
	}
	
// -----------------------------------------------------------------------------
// ChspsDefinitionRepository::SetResourceListL
// Stores the resource list to the repository.
// (other items were commented in a header).
// -----------------------------------------------------------------------------    
EXPORT_C TInt ChspsDefinitionRepository::SetResourceListL( ChspsODT& aODT, const CArrayPtrSeg<ChspsResource>& aResourceList )
	{
	// make sure the temporary buffers are empty.
	iTempFileName1.Zero();
	iTempFileName2.Zero();
    
    TInt errorCode = KErrNone;
       
    TInt tempLanguage = aODT.OdtLanguage();    
            
    // Loop provided resources and copy them to correct places in the repository
    for ( TInt i=0; i<aResourceList.Count(); i++ )
    	{
    	ChspsResource* res = aResourceList.At(i);
    	
    	aODT.SetOdtLanguage( res->Language() );
    	
    	if ( res->ResourceType() == EResourceDTD )
			{												
			SetLocaleL( aODT, *res );							
			}
    	else
    		{	    		    	
			// save source path because res->FileName() will be modified by MakeResourcePath but 
			// the original path is still needed
	        HBufC* fileName = HBufC::NewLC( KMaxFileName );
	        TPtr fileNamePtr = fileName->Des();
	        fileNamePtr.Append( res->FileName() );                                            
		    	    
		    // strip the extension temporarily    	        	    
		    TParsePtr k( fileNamePtr );
			TPtrC name = k.Name();
		    res->SetResourceIdL(name);
		    
		    // copy a non-cacheable resource from its location into the repository 
	    	MakeResourcePathL( aODT, *res );
	        
	    	// restore the file extension
	        TPtrC nameAndExt = k.NameAndExt();
	        res->SetResourceIdL(nameAndExt);
	        
			// copy the resource file to the repository
			errorCode = CopyFileL( *fileName, iPath->Des() );
			if ( errorCode )
			    {
#ifdef HSPS_LOG_ACTIVE
                if( iLogBus )
                    {
                    iLogBus->LogText( _L( "ChspsDefinitionRepository::SetResourceListL() - resource copying failed" ) );
                    }
#endif
			    }
			
			CleanupStack::PopAndDestroy( fileName );
			
			
			// Set drive information
			iTempFileName1.Format( _L("%S"), &res->FileName() );				
			TParsePtr f( iTempFileName1 );		
			TPtrC path = f.Path();
			TPtrC file = f.NameAndExt();			    
		    iTempFileName2.Format(_L("%S%S%S"), &KCDrive, &path, &file );
	        res->SetFileNameL( iTempFileName2 );			    
    		}
    	aODT.AddResourceL( res );
        }

    aODT.SetOdtLanguage( tempLanguage );
	iTempFileName1.Zero();
	iTempFileName2.Zero();

	return errorCode;
	}

// -----------------------------------------------------------------------------
// ChspsDefinitionRepository::GetResourceListL
// Retrieves the resource list from the repository. ODT-header given is a search mask.
// (other items were commented in a header).
// -----------------------------------------------------------------------------    
EXPORT_C TInt ChspsDefinitionRepository::GetResourceListL( const ChspsODT& aODT, CArrayPtrSeg<ChspsResource>& aResourceList )
	{
	TInt errorCode = KErrNone;
	GetPathL( aODT, EResourceODT );
	CFileStore* store = OpenFileStoreLC( *iPath );
	
	if( !store )
		{
		errorCode = KErrNotFound;
		}
	// Construct and open the root stream which contains the representation of our objects.
	if( !errorCode )
		{	
		RStoreReadStream instream;
		CleanupClosePushL( instream );
		
		instream.OpenLC( *store, store->Root() );
		
		ChspsODT* odt = ChspsODT::NewL();
	    CleanupStack::PushL( odt );
			  
	    odt->InternalizeHeaderL( instream );
        // consumes header delimiter
        instream.ReadInt16L();
        odt->InternalizeResourceListL( instream );    
		// destroy the direct file store object (closes the file)
		for ( TInt i = 0; i < odt->ResourceCount(); i++ ) 
		    {
		    aResourceList.AppendL( odt->ResourceL( i ).CloneL());
		    }
						    
        CleanupStack::PopAndDestroy( odt );
        
   		// Destroy the stream object and close the intstream
   		CleanupStack::PopAndDestroy( 2, &instream );
                
        // Destroy the direct file store object (closes the file) 		    
		CleanupStack::PopAndDestroy( store );
		}
	
	return errorCode;
	}
	
// -----------------------------------------------------------------------------
// ChspsDefinitionRepository::GetRepositoryInfo
// Returns a refrence to a RArray-object containing changes in the repository system.
// It is a responsibilty of the caller to lock the repository for modifications.
// (other items were commented in a header).
// -----------------------------------------------------------------------------   
EXPORT_C void ChspsDefinitionRepository::RegisterNotification( ThspsRepositoryInfo aRepositoryInfo )
    {
    iRepositoryInfoQueue.Append( aRepositoryInfo );
    
    for ( TInt i=0; i < iRepositoryInfoQueue.Count(); i++ )
        {
        // take an info from the queue
        ThspsRepositoryInfo info = iRepositoryInfoQueue[i];
        TBool consumed = EFalse;
        // notify the observers until someone consumes the info
        for ( TInt j=0; !consumed && j < iObservers.Count(); j++ )
            {    
            if ( iObservers[j]->HandleDefinitionRespositoryEvent( info ) )
                {
                // observer has handled the event, stop this loop
                consumed = ETrue;
                }    
            }
        // delete info from the queue   
        iRepositoryInfoQueue.Remove( i ); 
        }
    iRepositoryInfoQueue.Compress();    
    }

// -----------------------------------------------------------------------------
// ChspsDefinitionRepository::RegisterObserverL
// Registers an observer. Observers will be notified when there are definition repository 
// events available.
// (other items were commented in a header).
// -----------------------------------------------------------------------------  
EXPORT_C void ChspsDefinitionRepository::RegisterObserverL
                                              ( const MhspsDefinitionRepositoryObserver& aObserver )
    {
    iObservers.Append( &aObserver );
    }
    
// -----------------------------------------------------------------------------
// ChspsDefinitionRepository::UnregisterObserver
// When an observer stops observation of definition repository events, it must
// unregister itself.
// (other items were commented in a header).
// -----------------------------------------------------------------------------  
EXPORT_C void ChspsDefinitionRepository::UnregisterObserver
                                                ( const MhspsDefinitionRepositoryObserver& aObserver )
    {
    TInt found = iObservers.Find( &aObserver ); 
    if ( !( found == KErrNotFound ))
        {
        iObservers.Remove( found );
        }
    }    


// -----------------------------------------------------------------------------
// ChspsDefinitionRepository::Lock
// (other items were commented in a header).
// -----------------------------------------------------------------------------    
EXPORT_C void ChspsDefinitionRepository::Lock()
	{
    iLockSemaphore++;
    
#ifdef HSPS_LOG_ACTIVE
    if( iLogBus )
        {
        iLogBus->LogText( _L( "ChspsDefinitionRepository::Lock() - semaphore increased to %d" ),
                iLockSemaphore );
        }
#endif    
	}

// -----------------------------------------------------------------------------
// ChspsDefinitionRepository::Unlock
// (other items were commented in a header).
// -----------------------------------------------------------------------------    
EXPORT_C void ChspsDefinitionRepository::Unlock()
	{
	if (iLockSemaphore )
		{
		iLockSemaphore--;
		
#ifdef HSPS_LOG_ACTIVE
        if( iLogBus )
            {
            iLogBus->LogText( _L( "ChspsDefinitionRepository::Unlock() - semaphore decreased to %d" ),
                    iLockSemaphore );
            }
#endif
		}
	}

	
// -----------------------------------------------------------------------------
// ChspsDefinitionRepository::Locked
// (other items were commented in a header).
// -----------------------------------------------------------------------------    
EXPORT_C TBool ChspsDefinitionRepository::Locked() const
	{
    return iLockSemaphore;
	}

// -----------------------------------------------------------------------------
// ChspsDefinitionRepository::GetODTPathLC
// Creates a path from the theme header information.
// (other items were commented in a header).
// -----------------------------------------------------------------------------
EXPORT_C void ChspsDefinitionRepository::GetODTPathL( const ChspsODT& aODT, TDes& aPath )
	{
	GetPathL( aODT, EResourceODT );
	if ( iPath )
		{
		aPath.Copy( iPath->Des() );
		}
	else
		User::Leave( KErrNotFound );
	}

// -----------------------------------------------------------------------------
// ChspsDefinitionRepository::SetLogBus
// Set log bus.
// -----------------------------------------------------------------------------
#ifdef HSPS_LOG_ACTIVE
EXPORT_C void ChspsDefinitionRepository::SetLogBus( void* aLogBus )
    {
    iLogBus = (ChspsLogBus*)aLogBus;
    }
#else
EXPORT_C void ChspsDefinitionRepository::SetLogBus( void* /*aLogBus*/ )
    {
    }
#endif

// -----------------------------------------------------------------------------
// ChspsDefinitionRepository::GetPathL
// Creates a path from the theme header information.
// (other items were commented in a header).
// -----------------------------------------------------------------------------
void ChspsDefinitionRepository::GetPathL( const ChspsODT& aODT, ThspsResourceType aResourceType )
    {
    delete iPath;
    iPath = NULL;
    iPath = HBufC::NewL( KMaxFileName );
    TPtr pathPtr = iPath->Des();
    
    TInt rootUid( aODT.RootUid() );
    TInt providerUid( aODT.ProviderUid() );
    TInt themeUid( aODT.ThemeUid() );
    
    const TDesC& providerName = aODT.ProviderName();
    const TDesC& themeShortName = aODT.ThemeShortName();
    const TDesC& fileVersion = aODT.ThemeVersion();
    
	User::LeaveIfError( iFs.PrivatePath( pathPtr ));
	
	AppendDesCIntoPathL( pathPtr, KThemesFolder );
    AppendDesCIntoPathL( pathPtr, KDoubleBackSlash );
    
    if ( rootUid )
        {
        // Append root uid
        AppendNumIntoPathL( pathPtr, rootUid );
        AppendDesCIntoPathL( pathPtr, KDoubleBackSlash );
        }
    if ( providerUid )
        {
        // Append provider uid
        AppendNumIntoPathL( pathPtr, providerUid );
        AppendDesCIntoPathL( pathPtr, KDoubleBackSlash );
        }
    if ( themeUid )
        {
        // Append configuration uid
        AppendNumIntoPathL( pathPtr, themeUid );
        AppendDesCIntoPathL( pathPtr, KDoubleBackSlash );
        } 
    if ( fileVersion.Length() )
        {
        // Append version
        AppendDesCIntoPathL( pathPtr, fileVersion );
        AppendDesCIntoPathL( pathPtr, KDoubleBackSlash );
        }
   
    // path for source files
    if ( aResourceType > EResourceODT )
        {        
        if ( aResourceType != EResourceXML )                
        	{
        	if ( aResourceType == EResourceDTD )
                {
                // <ver> locales <language>                
                AppendDesCIntoPathL( pathPtr, KLocaleFolder );
                AppendDesCIntoPathL( pathPtr, KDoubleBackSlash );
                AddLocalePathL( pathPtr, aODT.OdtLanguage() );                                        
                }
        	else
        	    {	                  
                // <ver> sources
                AppendDesCIntoPathL( pathPtr, KSourceFolder );                   
                }
        	AppendDesCIntoPathL( pathPtr, KDoubleBackSlash );	        	                	
        	}
        
        }
        
    // resource source files do not need full path and filename
    // because they must be stored as is 
    if ( aResourceType > EResourceNone && aResourceType < EResourceRES ) 
    	{
        if ( themeShortName.Length() )
            {
            // Append configuration name
            AppendDesCIntoPathL( pathPtr, themeShortName );
            }
        else if ( themeUid )
            {
            AppendNumIntoPathL( pathPtr, themeUid );
            }

        FileExtensionL( aResourceType, aODT.OdtLanguage() );
    	}
    }
   
// -----------------------------------------------------------------------------
// ChspsDefinitionRepository::AppendDesCIntoPathL
// Appends a descriptor into path. Leaves if fails.
// (other items were commented in a header).
// -----------------------------------------------------------------------------
void ChspsDefinitionRepository::AppendDesCIntoPathL( TDes& aPath, const TDesC& aText )
    {
    if ( ( KMaxFileName - aPath.Length() ) >=  aText.Length() )
        {
        aPath.Append( aText );
        }
    else
        {
        User::Leave( KErrOverflow );
        }
    }

// -----------------------------------------------------------------------------
// ChspsDefinitionRepository::AppendNumIntoPathL
// Appends a number into path. Leaves if fails.
// (other items were commented in a header).
// -----------------------------------------------------------------------------
void ChspsDefinitionRepository::AppendNumIntoPathL( TDes& aPath, const TUint aNum )
    {
    TInt len = 8;
    if ( ( KMaxFileName - aPath.Length() ) >=  len )
        {
        aPath.AppendNum( aNum );
        }
    else
        {
        User::Leave( KErrOverflow );
        }
    }

// -----------------------------------------------------------------------------
// ChspsDefinitionRepository::FileExtensionL
// Returns a file extension for the given TDefRepFileExtension.
// (other items were commented in a header).
// -----------------------------------------------------------------------------
void ChspsDefinitionRepository::FileExtensionL( ThspsResourceType aResourceType,
                                                                TInt /*aLanguage*/ )
    {
    TPtr pathPtr = iPath->Des();
    switch ( aResourceType )
        {
        case EResourceNone:
            {
            }
            break;
        case EResourceODT:
            {
            TBuf<6> extension( KODTFileExtension );
            AppendDesCIntoPathL( pathPtr, extension );
            }
            break;
        case EResourceDTD:
            {
            AppendDesCIntoPathL( pathPtr, KDTDFileExtension );
            }
            break;
        case EResourceXML:
            {
            AppendDesCIntoPathL( pathPtr, KXMLFileExtension );
            }
            break;        
        case EResourceDAT:
            {
            AppendDesCIntoPathL( pathPtr, KDATFileExtension );
            }
            break;
        default:
            {
            User::Leave( KErrArgument );
            }
            break;
        };
    }
    
// -----------------------------------------------------------------------------
// ChspsDefinitionRepository::PathExtensionL
// Returns a file extension for the given TDefRepFileExtension.
// (other items were commented in a header).
// -----------------------------------------------------------------------------
void ChspsDefinitionRepository::AddLocalePathL( TDes& aPath, TInt aLanguage )
    {        
    TBuf<6> extension;
    extension.AppendFormat( KFolderFormat, aLanguage );
    AppendDesCIntoPathL( aPath, extension );   
    }    

// -----------------------------------------------------------------------------
// ChspsDefinitionRepository::WriteToFileL
// Writes the ODT structure into file.
// (other items were commented in a header).
// -----------------------------------------------------------------------------
TInt ChspsDefinitionRepository::WriteToFileL( const ChspsODT& aODT )
    {
    TInt errorCode = KErrNone;
    errorCode = iFs.SetSessionToPrivate( EDriveC );
    if ( SysUtil::DiskSpaceBelowCriticalLevelL( &iFs, 0, EDriveC ) )
        {
        errorCode = KErrDiskFull;

#ifdef HSPS_LOG_ACTIVE
        if( iLogBus )
            {
            iLogBus->LogText( _L( "ChspsDefinitionRepository::WriteToFileL(): - disk full." ) );
            }
#endif        
        }
        
    if ( !errorCode )
   	    {
        // Create the directory structure
	    TInt err = iFs.MkDirAll( *iPath );
	    if ( err != KErrNone && err != KErrAlreadyExists )
	        {
	        errorCode = err;
	        
#ifdef HSPS_LOG_ACTIVE
            if( iLogBus )
                {
                iLogBus->LogText( _L( "ChspsDefinitionRepository::WriteToFileL(): - error %d." ),
                        err );
                }
#endif	        
	        }
	   	       
	    if ( !errorCode )    
		    {
            // Create (replace) the direct file store
	        CFileStore* store = CDirectFileStore::ReplaceLC( iFs, *iPath, EFileWrite);

	        // Must say what kind of file store.
	        store->SetTypeL( KDirectFileStoreLayoutUid );
    	
	        // Construct the output stream.
	        RStoreWriteStream outstream;
	        CleanupClosePushL( outstream );
	        
	        TStreamId id = outstream.CreateLC(*store);
    
	        // Stream out the ChspsODT
	        outstream << aODT;
	        outstream.CommitL();
	        	        
	        // Destroy the stream object and close the outstream
            CleanupStack::PopAndDestroy( 2, &outstream );

	        // Set this stream id as the root
	        store->SetRootL(id);
	        store->CommitL();

	        // Destroy the direct file store object (closes the file),
	        CleanupStack::PopAndDestroy( store );
		    }
   	    }
	return errorCode;
    }
   
// -----------------------------------------------------------------------------
// ChspsDefinitionRepository::ReadFromFileL
// Reads the ODT structure from the file.
// (other items were commented in a header).
// -----------------------------------------------------------------------------
TInt ChspsDefinitionRepository::ReadFromFileL( const TDesC& aPath, ChspsODT& aODT )
    {
	TInt errorCode = KErrNone;
	CFileStore* store = OpenFileStoreLC( aPath );
	if ( !store )
		{
		errorCode = KErrNotFound;
		}
	// Construct and open the root stream which 
	// contains the representation of our objects.
	if ( !errorCode )
		{
		RStoreReadStream instream;
		CleanupClosePushL( instream );
		
		instream.OpenLC(*store,store->Root());
		
		// Stream in the ODT
    	instream >> aODT;

        // Destroy the stream object and close the instream
        CleanupStack::PopAndDestroy( 2, &instream );

        // Destroy the direct file store object (closes the file),
        CleanupStack::PopAndDestroy( store );
		}
		
	return errorCode;
    }
              
// -----------------------------------------------------------------------------
// ChspsDefinitionRepository::StreamHeaderFromFileL
// Streams the ODT header from the file into descriptor.
// (other items were commented in a header).
// -----------------------------------------------------------------------------    
HBufC8* ChspsDefinitionRepository::StreamHeaderFromFileL( const TDesC& aPath )
    {
    HBufC8* dataStream( NULL );
    CFileStore* store( NULL );
    
    store = OpenFileStoreLC( aPath );
	
	if( store )
		{
    	// Construct and open the root stream, which 
		// contains the representation of our objects.
		RStoreReadStream instream;
		CleanupClosePushL( instream );
		
		instream.OpenLC( *store, store->Root() );
    
    	ChspsODT* odt = ChspsODT::NewL();
    	CleanupStack::PushL( odt );

    	odt->InternalizeHeaderL( instream );   	
    	
	
    	dataStream = odt->MarshalHeaderL();

    	CleanupStack::PopAndDestroy( odt );
    	
   		// Destroy the stream object and close the instream
   		CleanupStack::PopAndDestroy( 2, &instream );
   		
   		// Destroy the direct file store object (closes the file) 
		CleanupStack::PopAndDestroy( store ); 
		}
		
    return dataStream;
    }
    
// -----------------------------------------------------------------------------
// ChspsDefinitionRepository::OpenFileStoreLC
// Searches and opens the CFileStore from the any drive.
// (other items were commented in a header).
// -----------------------------------------------------------------------------    
CFileStore* ChspsDefinitionRepository::OpenFileStoreLC( const TDesC& aPath )
    {
    RFile file;        
    TDriveList drivelist;
    CFileStore* store = NULL;
    
    TInt err( KErrNotFound );
	iFs.SetSessionToPrivate( EDriveC );
	err = file.Open(iFs, aPath, EFileShareReadersOnly );
	if ( !err )
	    {
        // if the file is empty CDirectFileStore::FromLC leaves.
	    TInt size(0);
	    file.Size(size);
	    if(size)
	        {
	        // Oownership of the file passes to the store
    	    store = CDirectFileStore::FromLC( file );    	    
	        }
	    }
    
    if( err || !store )    
        {
        // Just for sanity
        file.Close();
        }
        
	iLicenseDefault = EFalse;
	
	return store;
    }
    
    
// -----------------------------------------------------------------------------
// ChspsDefinitionRepository::SelectFilesFromPathL
// Retrieves all the files from the given path.
// (other items were commented in a header).
// -----------------------------------------------------------------------------
void ChspsDefinitionRepository::SelectFilesFromPathL( CDesCArraySeg& aFileList, const TDesC& aPath, 
                                                  TEntryKey aSortFlag, const TDesC& aFileExtension )
    {
    CDir* dirList;
    User::LeaveIfError( iFs.GetDir( aPath, KEntryAttMaskSupported, aSortFlag, dirList ));
    for ( TInt i = 0; i < dirList->Count(); i++)
        {
        if ( !(*dirList)[i].IsDir() )
            {
            HBufC* path = HBufC::NewLC( aPath.Length() + (*dirList)[i].iName.Length() );
            TPtr pathPtr = path->Des();
            AppendDesCIntoPathL( pathPtr, aPath );
            AppendDesCIntoPathL( pathPtr, (*dirList)[i].iName );
            TParse p;
            p.Set(pathPtr,NULL,NULL);
            
            // If the path is not pointing to the sources folder (e.g. is not a resource file)
            if ( pathPtr.FindF( KSourceFolderWithSeperators ) == KErrNotFound )
            	{       
            	// If requested and parsed file extensions match            	
                if ( !p.Ext().CompareF( aFileExtension ) )
            	    {
                    aFileList.AppendL( pathPtr );
             	    }	                
            	}
            CleanupStack::PopAndDestroy( path );  
            }
        }
    delete dirList;
    }
    

// -----------------------------------------------------------------------------
// ChspsDefinitionRepository::CopyFileL
// Copies a file from the given location into path,
// which is stored in the iPath member variable.
// (other items were commented in a header).
// -----------------------------------------------------------------------------
TInt ChspsDefinitionRepository::CopyFileL( const TDesC& aSourceFile, const TDesC& aDestinationFile )
    {
    TInt errorCode = KErrNone;
    iFs.SetSessionToPrivate( EDriveC );
    // create the directory structure if one does not exist
	TInt err = iFs.MkDirAll( aDestinationFile );
	if ( err != KErrNone && err != KErrAlreadyExists )
	    {
	    errorCode = err;
	    }
	if ( !errorCode )
	    {

	    TEntry entry;
        errorCode = iFs.Entry( aSourceFile, entry );
        if ( !errorCode && SysUtil::DiskSpaceBelowCriticalLevelL( &iFs, entry.iSize, EDriveC ) )
            {
            errorCode = KErrDiskFull;
            }
    
        if ( !errorCode )
    	    {
    	    CFileMan* fileMan = CFileMan::NewL( iFs );
    	    CleanupStack::PushL( fileMan );
    	    errorCode = fileMan->Copy( aSourceFile, aDestinationFile, ( CFileMan::ERecurse | CFileMan::EOverWrite ) );
    	    errorCode = fileMan->Attribs( *iPath, KEntryAttNormal,
                                                    KEntryAttReadOnly | KEntryAttHidden, TTime(0) );
    	    CleanupStack::PopAndDestroy( fileMan );
        	}
	    }
    return errorCode;
    }

// -----------------------------------------------------------------------------
// ChspsDefinitionRepository::MatchingFoldersFromAllDrivesL
// Retrieves all the matching folders from the given path from every drive.
// (other items were commented in a header).
// -----------------------------------------------------------------------------
void ChspsDefinitionRepository::MatchingFoldersFromAllDrivesL( const TDesC& aWildName,
                              const TDesC& aScanDir, CDesCArraySeg& aPathList )
    {
    TFindFile fileFinder( iFs );
    CDir* fileList;
    
    // Exclude external and ROM drives from the following search
    fileFinder.SetFindMask( KDriveAttExclude | KDriveAttRemovable |KDriveAttRemote |KDriveAttRom ); 
           
    // Searches, using wildcards, for one or more files/directories in a specified directory.
    // If no matching file is found in that directory, all available drives are searched in  
    // descending alphabetical order, from Y: to A:, and ending with the Z: drive.
    // The search ends when one or more matching filenames are found, or when every available  
    // drive has been unsuccessfully searched. 
    // To begin searching again after a successful match has been made, use FindWild().    
    TInt err = fileFinder.FindWildByDir( aWildName, aScanDir, fileList );
    while ( err == KErrNone )
        {
        TInt i;
        for ( i = 0; i < fileList->Count(); i++ )
            {
            TPtrC name( (*fileList)[i].iName );
            if( (*fileList)[i].IsDir() 
                    && KSourceFolder().CompareF( name ) != 0
                    && KLocaleFolder().CompareF( name ) != 0 )
                {
                TParse fullentry;
                fullentry.Set( name, &fileFinder.File(), NULL);

                HBufC* path = HBufC::NewLC( fullentry.FullName().Length() + KDoubleBackSlash().Length() );
                TPtr pathPtr = path->Des();
                pathPtr.Append( fullentry.FullName() );                
                pathPtr.Append( KDoubleBackSlash );
                aPathList.AppendL( pathPtr );
                CleanupStack::PopAndDestroy( path );
                }
            }//for
        delete fileList;
        //Go trough all the drives a->z
        err = fileFinder.FindWild( fileList );
        }//while
    }

// -----------------------------------------------------------------------------
// ChspsDefinitionRepository::MatchingFoldersL
// Retrieves all the matching folders from the given path and drive.
// (other items were commented in a header).
// -----------------------------------------------------------------------------
TInt ChspsDefinitionRepository::MatchingFoldersL( const TDesC& aWildName,
                              const TDesC& aScanDir, CDesCArraySeg& aPathList )
    {
    TInt foldersExists( KErrCancel );
    TFindFile fileFinder( iFs );
    CDir* fileList;
    
    TInt err = fileFinder.FindWildByDir( aWildName, aScanDir, fileList );
    if ( err == KErrNone )
        {
        TInt i;
        for ( i = 0; i < fileList->Count(); i++ )
            {
            TPtrC name( (*fileList)[i].iName );
            if( (*fileList)[i].IsDir() 
                    && KSourceFolder().CompareF( name ) != 0
                    && KLocaleFolder().CompareF( name ) != 0 )
                {
                foldersExists = KErrNone;
                TParse fullentry;
                fullentry.Set( name, &fileFinder.File(), NULL);

                HBufC* path = HBufC::NewLC( fullentry.FullName().Length() + KDoubleBackSlash().Length() );
                TPtr pathPtr = path->Des();
                pathPtr.Append( fullentry.FullName() );                
                pathPtr.Append( KDoubleBackSlash );
                aPathList.AppendL( pathPtr );
                CleanupStack::PopAndDestroy( path );
                }
            else
                {
                foldersExists = KErrNotFound;
                }
            }//for
        delete fileList;
        }//if
        
    return foldersExists;
    }

// -----------------------------------------------------------------------------
// ChspsDefinitionRepository::CopyFileL
// Copies a file from the given location into path,
// which is stored in the iPath member variable.
// (other items were commented in a header).
// -----------------------------------------------------------------------------
TInt ChspsDefinitionRepository::CopyFileL( const TDesC& aSourceFile )
    {
    TInt errorCode = KErrNone;
    iFs.SetSessionToPrivate( EDriveC );
    // create the directory structure if one does not exist
	TInt err = iFs.MkDirAll( *iPath );
	if ( err != KErrNone && err != KErrAlreadyExists )
	    {
	    errorCode = err;
	    }
	if ( !errorCode )
		{
		TEntry entry;
    	errorCode = iFs.Entry( aSourceFile, entry );
    	if ( !errorCode && SysUtil::DiskSpaceBelowCriticalLevelL( &iFs, entry.iSize, EDriveC ) )
        	{
        	errorCode = KErrDiskFull;
        	}
        if ( !errorCode )       
        	{
    		CFileMan* fileMan = CFileMan::NewL( iFs );
    		CleanupStack::PushL( fileMan );
    		errorCode = fileMan->Copy( aSourceFile, *iPath, CFileMan::EOverWrite );
    		errorCode = fileMan->Attribs( *iPath, KEntryAttNormal,
                        KEntryAttReadOnly | KEntryAttHidden, TTime(0) );
    		CleanupStack::PopAndDestroy( fileMan );
        	}
		}
	return errorCode;
    }
    
// -----------------------------------------------------------------------------
// ChspsDefinitionRepository::RemoveDirectoryL
// Deletes the files and folders beneath the path,
// which is stored in the iPath member variable.
// (other items were commented in a header).
// -----------------------------------------------------------------------------
void ChspsDefinitionRepository::RemoveDirectoryL()
    {
    TInt errorCode = KErrNone;
    iFs.SetSessionToPrivate( EDriveC );
    CFileMan* fileMan = CFileMan::NewL( iFs );
    CleanupStack::PushL( fileMan );
    TPtr pathPtr = iPath->Des();
    TInt err = fileMan->RmDir( pathPtr );
    if ( err != KErrNone && err != KErrPathNotFound )
        {
        errorCode = err; // probably KErrInUse
        }
    else
    	{
     	//If the folder is empty -> remove it
    	TParsePtr path( pathPtr );
    	TInt flag( ETrue );
    	do
        	{
        	TInt err = path.PopDir();
        	if ( !err )
        	    {
        	    CDir* dirList;
        	    err = iFs.GetDir( path.Path(), KEntryAttMaskSupported,ESortByName, dirList );
            	if ( !err && dirList->Count() == 0 )
            	    {
            	    err = fileMan->RmDir( path.Path() );
            	    }
        	    else
            	    {
            	    flag = EFalse;
            	    }
        	    delete dirList;
        	    }
        	}
     	while ( flag );
    	}
    CleanupStack::PopAndDestroy( fileMan );
    User::LeaveIfError( errorCode );
    }

    
// -----------------------------------------------------------------------------
// ChspsDefinitionRepository::RestoreBackupConfiguration
// Restores defined configuration from backup folder
// (other items were commented in a header).
// -----------------------------------------------------------------------------
EXPORT_C TInt ChspsDefinitionRepository::RestoreBackupConfiguration(
    const ChspsODT& /*aODT*/ )
    {
    TInt err( KErrNotFound );
    
    return err;
    }

// -----------------------------------------------------------------------------
// ChspsDefinitionRepository::BackupConfigurationL
// Copies defined configuration to theme backup folder
// (other items were commented in a header).
// -----------------------------------------------------------------------------
EXPORT_C void ChspsDefinitionRepository::BackupConfigurationL(
    const ChspsODT& aODT )
    {
    HBufC* backupPath = HBufC::NewL( KMaxFileName );
    CleanupStack::PushL( backupPath );
    TPtr backupPathPtr = backupPath->Des();
    
    User::LeaveIfError( iFs.PrivatePath( backupPathPtr ));

    AppendDesCIntoPathL( backupPathPtr, KBackupFolder );
    AppendDesCIntoPathL( backupPathPtr, KDoubleBackSlash );
    AppendDesCIntoPathL( backupPathPtr, KThemesFolder );
    AppendDesCIntoPathL( backupPathPtr, KDoubleBackSlash );

    // Backup configuration file
    GetPathL( aODT, EResourceODT );
    CopyFileL( iPath->Des(), backupPathPtr );

    CleanupStack::PopAndDestroy( backupPath );
    
    }

// -----------------------------------------------------------------------------
// ChspsDefinitionRepository::ClearBackupsL
// Clears previous backup data
// (other items were commented in a header).
// -----------------------------------------------------------------------------
EXPORT_C void ChspsDefinitionRepository::ClearBackupsL()
    {

    HBufC* backupPath = HBufC::NewL( KMaxFileName );
    CleanupStack::PushL( backupPath );
    TPtr backupPathPtr = backupPath->Des();
    
    User::LeaveIfError( iFs.PrivatePath( backupPathPtr ));

    AppendDesCIntoPathL( backupPathPtr, KBackupFolder );
    AppendDesCIntoPathL( backupPathPtr, KDoubleBackSlash );

    RemoveDirectoryL( backupPathPtr );
    
    CleanupStack::PopAndDestroy( backupPath );
    
    }

// -----------------------------------------------------------------------------
// ChspsDefinitionRepository::GetOdtL
// Retrieves the odt-structure from the repository as a reference.
// (other items were commented in a header).
// -----------------------------------------------------------------------------
//
EXPORT_C void ChspsDefinitionRepository::GetOdtL( 
    const TDes& aPath, 
    ChspsODT& aODT )
    {    
    User::LeaveIfError( ReadFromFileL( aPath, aODT ) );
    }

// -----------------------------------------------------------------------------
// ChspsDefinitionRepository::RemoveDirectoryL
// Deletes the defined directory ( files and folders recursively )
// (other items were commented in a header).
// -----------------------------------------------------------------------------
void ChspsDefinitionRepository::RemoveDirectoryL(
    TDesC& aDirPath )
    {
    
    iFs.SetSessionToPrivate( EDriveC );
    CFileMan* fileMan = CFileMan::NewL( iFs );
    CleanupStack::PushL( fileMan );
    TInt err = fileMan->RmDir( aDirPath );
    CleanupStack::PopAndDestroy( fileMan );
    
    }

//  End of File  
