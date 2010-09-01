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
* Description:  Service for storing configurations into the file system

*
*/



#ifndef C_hspsDEFINITIONREPOSITORY_H
#define C_hspsDEFINITIONREPOSITORY_H

//  INCLUDES
#include <f32file.h>
#include <badesca.h>
#include <s32mem.h>
#include <fbs.h>
#include <e32property.h>

#include "hspsresource.h"

class CRepository;
#ifdef HSPS_LOG_ACTIVE
class ChspsLogBus;
#endif


/** 
* Publish & Subscribe functionality for controlling the ODT copies.
*
* IPC clients, such as such as sapi_homescreenplugin, should update any ChspsODT based 
* runtime copies when the active application configuration is updated. 
* Updated versions are indicated by an increment in the associated RProperty key.
**/
const TUid KPropertyHspsCat = {0x200159C0};
const TUint KPropertyAI3Key = {0x102750F0}; // Homescreen
const TUint KPropertyMTKey = {0x20000FB1}; // EUNIT MT
const TInt KPropertyKeyDefault = 1;


/**
*   ThspsRepositoryEvent.
*   Events used in ThspsRepositoryInfo.
**/

enum ThspsRepositoryEvent
    {
    EhspsODTActivated = 0x00, // active application configuration changed 
    EhspsODTAdded = 0x01, //new configuration installed
    EhspsODTUpdated = 0x02, //installing new version of plugin
    EhspsODTRemoved = 0x04, // plugin removed from configuration
    EhspsODTModified = 0x08,  // plugin added to configuration 
    EhspsCacheUpdate = 0x10, // forces full cache update
    EhspsClean = 0x20,  // destructors&unstalling plugin 
    EhspsSettingsChanged = 0x40, // settings changed
    EhspsPluginsMoved = 0x80, //plugins moved
    EhspsPluginActivated = 0x100,
    EhspsPluginReplaced = 0x200,
    EhspsNoEvent = 0x800
    };
    
/**
*   ThspsRepositoryInfo.
*   The Definitionrepository can inform client observers of changes to repository:
*   theme has been updated, theme added etc. A client needs to register to repository
*   (RegisterObserverL) to receive repository events. The client will receive the 
*   events via callback function HandleDefinitionRespositoryEvent().
*   If observer handles the event, it must return ETrue, else, it must return EFalse.
*/
class ThspsRepositoryInfo
    {
    public:
        ThspsRepositoryInfo( 
	    		ThspsRepositoryEvent aEventType, 
				TUint aAppUid = 0, 
				TUint aAppConfUid = 0, 
				TUint aSecureId = 0,
				TUint aAppConfProviderUid = 0,
				TUint aPluginIfUid = 0,
				TUint aPluginProviderUid = 0,
				TUint aPluginUid = 0,
				TUint aPluginId = 0,
				TBool aLastNotification = ETrue,
				TPtrC aName = KNullDesC(),
				TLanguage aLanguage = ELangTest )
        	{
            iEventTime.HomeTime();
            iEventType = aEventType;
            iAppUid = aAppUid;
            iAppConfUid = aAppConfUid;
            iSecureId = aSecureId;
            iAppConfProviderUid = aAppConfProviderUid;
            iPluginIfUid = aPluginIfUid;
            iPluginProviderUid = aPluginProviderUid;
            iPluginUid = aPluginUid;
            iPluginId = aPluginId;
            iLastNotification = aLastNotification;
            iName = aName;
            iLanguage = aLanguage;
            }
    public:  
        TTime iEventTime;
        ThspsRepositoryEvent iEventType;
        TUint iAppUid;
        TUint iAppConfUid;
        TUint iSecureId;
        TUint iAppConfProviderUid;
        TUint iPluginIfUid; 
        TUint iPluginProviderUid;
        TUint iPluginUid;
        TUint iPluginId;
        TBool iLastNotification;
        TBuf<KMaxFileName> iName;
        TLanguage iLanguage;
    };

/** 
* MhspsDefinitionRepositoryObserver is an interface definition to be implemented by the repository events 
* observers. 
*/
class MhspsDefinitionRepositoryObserver
    {
    /** 
    * Definition repository callback 
    * 
    * @since S60 5.0
    * @param aRepositoryInfo Repository callback info.
    */    
    public:
        virtual TBool HandleDefinitionRespositoryEvent( ThspsRepositoryInfo aRepositoryInfo ) = 0;
    };

// FORWARD DECLARATIONS
class ChspsODT;
class RWriteStream;
class RReadStream;
class CFileStore;
class ChspsDomDocument;

_LIT(KROMDrive,"z:");
_LIT(KCDrive,"c:");

/**
 *  hspsDefinitionRepository is a common and secure place to store the HSPS's
 *  definition files.
 *  
 *  Class provides the APIs to store and retrieve all the definition files: 
 *  locale, style, structure and resource files. 
 *
 *  Repository can store disk space limited number of ODT data structures and
 *  definition files (themes) for every application. Themes are identified by
 *  application uid, vendor name, theme name and theme version. Localisation is
 *  supported by naming the ODT for each language variant uniquely. TLanguage
 *  enumeration is used in the ODT header to specify the variant. The ODT can be
 *  retrieved from the repository as a fully constructed object or a descriptor
 *  stream.
 *  
 *  Existing themes can be queried from the repository for managing purposes. 
 *  Same method can be used to avoid accidental overwrites. If the query returns
 *  zero count for the appropriate ODT the new theme can be safely installed.
 *  Themes can be also removed with the valid header information. When the theme
 *  is removed also all the definition files will be deleted.
 *
 *
 *  @lib hspsDefRep.dll
 *  @since S60 5.0
 *  @ingroup group_hspspluginregistry
 */
class ChspsDefinitionRepository : public CBase
    {
    public:  // Constructors and destructor
        
        /**
        * Two-phased constructor.
        * 
        * @since S60 5.0
        */
        IMPORT_C static ChspsDefinitionRepository* NewL(
                CRepository& aCentralRepository );
        
        /**
        * Destructor.
        */
        virtual ~ChspsDefinitionRepository();

    public: // New functions
        
        /**
        * Saves the localisation file into repository. ODT must contain the valid 
        * header information, which is used to define the location for the file.
        * Updates resource object with the new location.
        * Leaves if original file does not exist or information is not valid.
        * 
        * @since S60 5.0        
        * @param aODT Contains the header information.
        * @param aResource The resource file to be saved.       
        */
        IMPORT_C TInt SetLocaleL( ChspsODT& aODT, ChspsResource& aResource );
                                
        /**
        * Creates a path for the ODT to be stored on repository.
        * ODT-header information works as a set mask.
        * 
        * @since S60 5.0
        * @param aODT Contains the header information. Works as a set mask. Leaves if invalid data.
        * @param aResource A ChspsResource-object filled with the new path is returned to caller.
        */       
        IMPORT_C void MakeODTPathL( ChspsODT& aODT, ChspsResource& aResource );
        
        /**
        * Saves the ODT data structure into the repository. The location of the data is
        * defined from the ODT's header information.
        * 
        * @since S60 5.0
        * @param aODT The actual data to be stored.
        */
        IMPORT_C TInt SetOdtL( const ChspsODT &aODT );
        
        /**
        * Retrieves the ODT from the repository. The ODT must contain the valid header 
        * information, which is used to locate the correct data. Leaves if ODT not found.
        * 
        * @since S60 5.0
        * @param aODT A reference to ODT object, which is filled with data from the repository.
        */
        IMPORT_C TInt GetOdtL( ChspsODT& aODT );
        
        /**
        * Retrieves the ODT header from the repository. The aPath variable must contain
        * the valid path. Leaves if path not found.
        * 
        * @since S60 5.0
        * @param aPath The path where the theme is located.
        * @param aLang Informs the correct language version.
        * @param aODT A reference to ODT header object, which is filled with data from the repository.
        */
        IMPORT_C TInt GetOdtHeaderL( TDes& aPath, TLanguage aLang, ChspsODT& aODT );
        
       
        /**
        * Retrieves the exact path of the repository object.
        * Does not check whether the object is really existing on repository or not.
        * 
        * @since S60 5.0
        * @param aODT Contains the header information. Works as a search mask. Leaves if invalid data.
        * @param aResourceType resource type to search for.
        * @param aFilePath A file path returned to caller.
        */       
        IMPORT_C void GetResourcePathL( const ChspsODT& aODT, ThspsResourceType aResourceType, TDes& aFilePath );
        
        
        /**
        * Retrieves the list of installed themes in an array. ODT header information works
        * as a search mask. If all the information is available in the header a function
        * can be used to query the theme's existence. This is useful when the accidental
        * overwrites needs to be avoided e.g. when installing a new theme.
        * 
        * @since S60 5.0
        * @param aThemeList A reference to an array, which is filled with the theme paths.
        * @param aODT Contains the header information. Works as a search mask. Leaves if invalid data.
        */
        IMPORT_C void GetThemePathListL( CDesCArraySeg& aThemeList, const ChspsODT& aODT );
        
        /**
        * Retrieves the list of installed themes as a streams in an array. ODT header information
        * works as a search mask.
        * 
        * @since S60 5.0
        * @param aThemeList A reference to an array, which contains internalized theme headers.
        * @param aODT Contains the header information. Works as a search mask. Leaves if invalid data.
        */
        IMPORT_C void GetThemeListAsStreamL( CArrayPtrSeg<HBufC8>& aThemeList, const ChspsODT& aODT );
        
        /**
        * Removes the theme specific ODT and definition files from the repository.
        * ODT header information works as a mask, which defines the level of removal. 
        * A single theme, all themes from a provider or all application specific
        * themes can be removed at once.
        * 
        * @since S60 5.0
        * @param aODT Contains the header information. Leaves if information is not valid.
        */
        IMPORT_C void RemoveThemeL( const ChspsODT& aODT );
        
        /**
        * Get a path for resource to be stored.
        * 
        * @since S60 5.0
        * @param aODT Contains the header information.
        * @param aResource resource where path is stored.
        * Leaves if path could not be defined.
        */
        IMPORT_C void MakeResourcePathL( const ChspsODT& aODT, ChspsResource& aResource );
        
        /**
        * Saves the resource into the repository.
        * 
        * @since S60 5.0
        * @param aODT Contains the header information.
        * @param aFbsBitmap bitmap to store to repository.
        * @param aResource resource where path is stored.
        * Leaves if original file does not exist.
        */
        IMPORT_C TInt SetResourceL( const ChspsODT& aODT, const CFbsBitmap& aFbsBitmap, ChspsResource& aResource );
        
        /**
        * Stores the resource list to the repository. Every registered resource has an
        * own entry in the array.
        * Leaves if the repository is corrupted. Use RemoveThemeL to remove the
        * corrupted repository.
        * 
        * @since S60 5.0
        * @param aODT Contains the header information.
        * @param aResourceList resource list to store to repository.
        */  
        IMPORT_C TInt SetResourceListL( ChspsODT& aODT, const CArrayPtrSeg<ChspsResource>& aResourceList );
      

        /**
        * Retrieves the resource list in an array. Every registered resource has an
        * own entry in the array.
        * Leaves if the repository is corrupted. Use RemoveThemeL to remove the
        * corrupted repository.
        * 
        * @since S60 5.0
        * @param aODT contains the theme header associated to this resourcelist.
        * @param aResourceList destination resource list where resource are read from repository.
        * @return a resource list.
        */
        IMPORT_C TInt GetResourceListL( const ChspsODT& aODT, CArrayPtrSeg<ChspsResource>& aResourceList );
               
        /**
        * GetRepositoryInfo
        * Register a ThspsRepositoryInfo-object to the repository. Repository's responsibility is
        * to notify its observers about the notification.
        * 
        * @since S60 5.0
        * @param aRepositoryInfo is aThspsRepositorynfo-object to be registered.
        */
        IMPORT_C void RegisterNotification( ThspsRepositoryInfo aRepositoryInfo );
        
        /**
        * RegisterObserverL (  )
        * Returns a refrence to a RArray-object containing changes in repository system.
        * It is a responsibilty of the caller to lock the repository for modifications.
        *    
        * @since S60 5.0
        * @param aObserver is a MhspsDefinitionRepositoryObserver-object to be registered.
        */
        IMPORT_C void RegisterObserverL( const MhspsDefinitionRepositoryObserver& aObserver );
        
        /**
        * UnregisterObserver
        * Returns a refrence to a RArray-object containing changes in repository system.
        * It is a responsibilty of the caller to lock the repository for modifications.
        *    
        * @since S60 5.0
        * @param aObserver is a MhspsDefinitionRepositoryObserver-object to be unregistered.
        */
        IMPORT_C void UnregisterObserver( const MhspsDefinitionRepositoryObserver& aObserver );

        /**
        * Increases the semaphore controlling repository locking.
        * Locking semaphore is increased explicitly by clients that
        * going to write data to repository..
        * Lockin semaphore revents reading repository as long as there
        * are writers. 
        * 
        * @since S60 5.0
        */
        IMPORT_C void Lock();
        
        /**
        * Decreases the semaphore controlling repository locking. 
        * Semaphore minimun value is 0 meaning unlocked situation.
        * 
        * @since S60 5.0
        */
        IMPORT_C void Unlock();
        
        /**
        * Returns repository locking status
        * 
        * @since S60 5.0
        * @returns ETrue if repository is locked otherwise EFalse
        */
        IMPORT_C TBool Locked() const;
                    
        /**
        * Creates a path from the header information. Results are stored into the iPath member.
        * 
        * @since S60 5.0
        * @param aODT An ODT whose path is being requested
        * @param aPath Relative path without a drive letter         
        */
        IMPORT_C void GetODTPathL( const ChspsODT& aODT, TDes& aPath );
        
        /** 
        * Set log bus.
        * Empty implementation if non HSPS_LOG_ACTIVE build.
        *  
        * @since S60 5.0
        * @param aLogBus Log bus to be set.
        */
        IMPORT_C void SetLogBus( void* aLogBus );        

        /** 
        * Restores a defined configuration from backup folder 
        * @since S60 5.0
        * @param aODT ODT header defining the configuration to be restored
        * @return Operation status 
        *         KErrNone if backup configuration is found and it is restored
        *         KErrNotFound if backup configuration is not found
        */
        IMPORT_C TInt RestoreBackupConfiguration( const ChspsODT& aODT );        

        /** 
        * Copies a defined configuration to theme backup folder 
        * @since S60 5.0
        * @param aODT ODT header defining the configuration to be copied
        */
        IMPORT_C void BackupConfigurationL( const ChspsODT& aODT );        

        /** 
        * Clears previous backup data
        * @since S60 5.0
        */
        IMPORT_C void ClearBackupsL();        

        /**
        * Retrieves the ODT from the repository. The aPath variable must contain
        * the valid path. Leaves if path not found.
        * @since S60 5.0
        * @param aPath The path where the theme is located.
        * @param aODT A reference to ODT object, which is filled with data from the repository.
        */
        IMPORT_C void GetOdtL( const TDes& aPath, ChspsODT& aODT );

     private:

        /**
        * C++ default constructor.
        */
        ChspsDefinitionRepository( CRepository& aCentralRepository );

        /**
        * By default Symbian 2nd phase constructor is private.
        */
        void ConstructL();
        
        void SetupPropertiesL( const TUint aKey );
        
        void SelectFilesFromPathL( CDesCArraySeg& aFileList, const TDesC& aPath, TEntryKey aSortFlag, const TDesC& aFileExtension );

        void GetPathL( const ChspsODT& aODT, ThspsResourceType aResourceType );
                
        void AppendDesCIntoPathL( TDes& aPath, const TDesC& aText );
        void AppendNumIntoPathL( TDes& aPath, const TUint aNum );
        void FileExtensionL( ThspsResourceType aResourceType, TInt aLanguage );
        void AddLocalePathL( TDes& aPath, TInt aLanguage );
        
        TInt WriteToFileL( const ChspsODT& aODT );        
                
        TInt ReadFromFileL( const TDesC& aPath, ChspsODT& aODT );
                
        HBufC8* StreamHeaderFromFileL( const TDesC& aPath );
        CFileStore* OpenFileStoreLC( const TDesC& aPath );
        
        void MatchingFoldersFromAllDrivesL(const TDesC& aWildName, const TDesC& aScanDir, CDesCArraySeg& aThemeList );
        TInt MatchingFoldersL(const TDesC& aWildName, const TDesC& aScanDir, CDesCArraySeg& aThemeList );
        
        TInt CopyFileL( const TDesC& aSourceFile );
        TInt CopyFileL( const TDesC& aSourceFile, const TDesC& aDestinationFile );
        void RemoveDirectoryL();
        void RemoveDirectoryL( TDesC& aDirPath );

    private:    // Data
    
        RArray<ThspsRepositoryInfo> iRepositoryInfoQueue;
        
        // Array of observers
        RPointerArray<MhspsDefinitionRepositoryObserver> iObservers;     
        
        //Handle to a file server session
        RFs iFs;
        
        //Holds the path information, owned
        HBufC* iPath;
         
        TBool iLicenseDefault;
        TInt iLockSemaphore;
                
        // Contains a temporary filepath
        TFileName iTempFileName1;
        
        // Contains a temporary filepath
        TFileName iTempFileName2;

#ifdef HSPS_LOG_ACTIVE        
        // Log bus. Not owned.
        ChspsLogBus* iLogBus;
#endif
       
        // Cached copy of last retrieved ODT.
        ChspsODT* iCacheLastODT;        
        
        // For indicating ODT updates
        RProperty iProperty;
        
        // Reference to central repository
        CRepository& iCentralRepository;
    };

#endif      // C_hspsDEFINITIONREPOSITORY_H   
            
// End of File
