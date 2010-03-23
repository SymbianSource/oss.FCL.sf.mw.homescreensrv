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

// INCLUDE FILES

#include <ecom/implementationproxy.h>
#include <sacls.h>

#include "chfactorysettingsinstallnotifier.h"
#include "cpdebug.h"
#include "chdbstatehandler.h"
#include "chfactorysettingsdomparser.h"
#include "chfactorysettingsplugin.h"

// LOCAL CONSTANTS AND MACROS
const TInt KADatFactorySettingsServerPluginUid( 0x102830EF);
// Dynamically loaded data storage handler

const TInt KDefaultGranularity = 5;

_LIT( KDirFSNew, "import\\fs\\" );
_LIT( KParsedDir, "parsed\\fs\\" );
_LIT( KParsed, "parsed\\" );
_LIT( KFs, "fs\\" );

_LIT( KDriveC, "C:" );
_LIT( KColen, ":" );

// Map the interface UIDs to implementation factory functions
const TImplementationProxy ImplementationTable[] =
    {
        {
            {
            KADatFactorySettingsServerPluginUid
            }, ( TProxyNewLPtr )CCHFactorySettings::NewL
        }
    };

// ----------------------------------------------------------------------------
// ImplementationGroupProxy
// Exported proxy for instantiation method resolution
// ----------------------------------------------------------------------------
//
EXPORT_C const TImplementationProxy*
ImplementationGroupProxy( TInt& aTableCount )
    {
    aTableCount = sizeof( ImplementationTable ) /
    sizeof( TImplementationProxy );
    return ImplementationTable;
    }

// ============================ MEMBER FUNCTIONS ===============================

// -----------------------------------------------------------------------------
// CCHFactorySettings::CCHFactorySettings
// C++ default constructor can NOT contain any code, that
// might leave.
// -----------------------------------------------------------------------------
//
CCHFactorySettings::CCHFactorySettings()
    {

    }

// -----------------------------------------------------------------------------
// CCHFactorySettings::ConstructL
// Symbian 2nd phase constructor can leave.
// -----------------------------------------------------------------------------
//
void CCHFactorySettings::ConstructL( MLiwInterface* aInterface )
    {
    CP_DEBUG(_L8("CCHFactorySettings::ConstructL" ));
    iCPSInterface = aInterface;
    iFileNewList = new (ELeave) CDesCArrayFlat(KDefaultGranularity);
    User::LeaveIfError( iFs.Connect( ) );
    iInstallNotifier = CCHFactorySettingsInstallNotifier::NewL( this,
        KUidSystemCategory, KSAUidSoftwareInstallKeyValue );

    iParser = CCHFactorySettingsDOMParser::NewL( iCPSInterface, iFs ); // mw

    iFileMan = CFileMan::NewL( iFs );
    iFilePath.CreateL( KMaxPath );

    iFs.CreatePrivatePath( EDriveC );
    User::LeaveIfError( iFs.PrivatePath( iFilePath ) );

    //create "parsed" directory if it doesn't exist yet;
    RBuf filePath;
    filePath.CreateL( KMaxPath );
    filePath.Append( KDriveC );
    filePath.Append( iFilePath );
    filePath.Append( KParsed );
    iFs.MkDir( filePath );
    filePath.Append( KFs );
    iFs.MkDir( filePath );
    filePath.Close( );
    iDBStateHandler = CCHDbStateHandler::NewL( this );
    }

// -----------------------------------------------------------------------------
// CCHFactorySettings::NewL
// Two-phased constructor.
// -----------------------------------------------------------------------------
//
CCHFactorySettings* CCHFactorySettings::NewL( MLiwInterface* aInterface )
    {
    CCHFactorySettings* self = NewLC( aInterface );
    CleanupStack::Pop( self );
    return self;
    }

// -----------------------------------------------------------------------------
// CCHFactorySettings::NewLC
// Two-phased constructor.
// -----------------------------------------------------------------------------
//
CCHFactorySettings* CCHFactorySettings::NewLC( MLiwInterface* aInterface )
    {
    CCHFactorySettings* self = new( ELeave ) CCHFactorySettings();
    CleanupStack::PushL( self );
    self->ConstructL( aInterface );
    return self;
    }

// -----------------------------------------------------------------------------
// Destructor
// -----------------------------------------------------------------------------
//
CCHFactorySettings::~CCHFactorySettings()
    {
    delete iFileNewList;
    delete iInstallNotifier;
    delete iFileMan;
    delete iParser;
    iFs.Close( );
    iFilePath.Close( );
    delete iDBStateHandler;
    }

// ----------------------------------------------------------------------------
// CCHFactorySettings::UpdateL
//
// ----------------------------------------------------------------------------
//
void CCHFactorySettings::UpdateL()
    {
    CP_DEBUG(_L8("CCHFactorySettings::UpdateL" ));
    CheckDrivesL( ); // fill in list with file names from all drives
    AddNewL( ); // if any of them isn't added to db then add
    UnInstallL( ); // if any of files from any drive was removed remove data
    // from DB

    }

// ----------------------------------------------------------------------------
// CCHFactorySettings::AddNewL
// Add new entries after sis intallation.
// ----------------------------------------------------------------------------
//
void CCHFactorySettings::AddNewL()
    {
    CP_DEBUG(_L8("CCHFactorySettings::AddNewL" ));
    CDir* fileListInstalled= NULL;
    RBuf filePath;
    filePath.CreateL( KMaxPath );
    CleanupClosePushL( filePath );
    filePath.Append( KDriveC );
    filePath.Append( iFilePath );
    filePath.Append( KParsedDir );
    iFs.GetDir( filePath, KEntryAttMaskSupported, ESortByName,
        fileListInstalled );
    CleanupStack::PushL( fileListInstalled );

    for ( TInt i(0); i<iFileNewList->Count( ); i++ )
        {
        TBool parse(ETrue);
        TPtrC fileNewName =
            (*iFileNewList)[i].Mid( (*iFileNewList)[i].LocateReverse( '\\' ) + 1 );
        TInt count = fileListInstalled->Count( );
        for ( TInt j(0); j< count; j++ )
            {
            TPtrC existingFile = (*fileListInstalled)[j].iName;
            if ( fileNewName.Compare( existingFile ) )
                {
                parse = ETrue;
                }
            else
                {
                parse = EFalse;
                break;
                }
            }
        if ( parse )
            {
            //file wasn't found in afterinstallation dir
            //so should be copied and entries install
            TBool fileOpened(ETrue);
            iFs.IsFileOpen( (*iFileNewList)[i], fileOpened );

            if ( !fileOpened )
                {
                TInt error = iParser->Restore( (*iFileNewList)[i], EFalse );
                if ( error == KErrNone )
                    {
                    iFileMan->Copy( (*iFileNewList)[i], filePath );
                    }
                }

            }
        }
    CleanupStack::PopAndDestroy( fileListInstalled );
    CleanupStack::PopAndDestroy( &filePath );
    }

// ----------------------------------------------------------------------------
// CCHFactorySettings::UnInstallL
// Removes entries after uninstallation.
// ----------------------------------------------------------------------------
//
void CCHFactorySettings::UnInstallL()
    {
    CP_DEBUG(_L8("CCHFactorySettings::UnInstallL" ));
    TBool found(EFalse);
    CDir* fileListInstalled= NULL;
    RBuf filePath;
    filePath.CreateL( KMaxPath );
    CleanupClosePushL( filePath );
    filePath.Append( KDriveC );
    filePath.Append( iFilePath );
    filePath.Append( KParsedDir );
    iFs.GetDir( filePath, KEntryAttMaskSupported, ESortByName,
        fileListInstalled );
    CleanupStack::PushL( fileListInstalled );

    for ( TInt i(0); i<fileListInstalled->Count( ); i++ )
        {
        TPtrC fileInstalledName = (*fileListInstalled)[i].iName;
        found = EFalse;
        for ( TInt j(0); j<iFileNewList->Count( ); j++ )
            {
            TPtrC fileNewPath = (*iFileNewList)[j];
            TPtrC fileNewName =
                (*iFileNewList)[j].Mid( (*iFileNewList)[j].LocateReverse( '\\' ) + 1 );
            if ( fileInstalledName.Compare( fileNewName )==0 )
                {
                if ( !(*fileListInstalled)[i].IsDir( ) )
                    {
                    found = ETrue;
                    break;
                    }
                }
            }
        if ( !found ) //this file should be removed also entries from database
            {
            filePath.Zero( );
            filePath.Append( KDriveC );
            filePath.Append( iFilePath );
            filePath.Append( KParsedDir );
            filePath.Append( fileInstalledName );

            iParser->Restore( filePath, ETrue );
            iFileMan->Delete( filePath );
            }
        }
    CleanupStack::PopAndDestroy( fileListInstalled );
    CleanupStack::PopAndDestroy( &filePath );
    }

// ----------------------------------------------------------------------------
// CCHFactorySettings::CheckDrivesL
//
// ----------------------------------------------------------------------------
//
void CCHFactorySettings::CheckDrivesL()
    {
    CP_DEBUG(_L8("CCHFactorySettings::CheckDrivesL" ));
    iFileNewList->Reset( );
    TDriveList driveList;
    TChar driveLetter;
    User::LeaveIfError( iFs.DriveList( driveList ) );
    for ( TInt driveNumber=EDriveA; driveNumber <= EDriveZ; driveNumber++ )
        {
        if ( driveList[driveNumber] )
            {
            User::LeaveIfError( iFs.DriveToChar( driveNumber, driveLetter ) );
            RBuf filePath;
            filePath.CreateL( KMaxPath );
            CleanupClosePushL( filePath );
            filePath.Append( driveLetter );
            filePath.Append( KColen );
            filePath.Append( iFilePath );
            filePath.Append( KDirFSNew );
            CDir* fileList;
            iFs.GetDir( filePath, KEntryAttMaskSupported, ESortByName,
                fileList );
            CleanupStack::PopAndDestroy( &filePath );
            if ( fileList )
                {
                CleanupStack::PushL( fileList );
                for ( TInt i(0); i<fileList->Count( ); i++ )
                    {
                    RBuf fullFileName;
                    fullFileName.CreateL( KMaxPath );
                    CleanupClosePushL( fullFileName );
                    fullFileName.Append( driveLetter );
                    fullFileName.Append( KColen );
                    fullFileName.Append( iFilePath );
                    fullFileName.Append( KDirFSNew ) ;
                    fullFileName.Append( (*fileList)[i].iName );
                    if ( !(*fileList)[i].IsDir( ) )
                        {
                        //check if already exist
                        iFileNewList->AppendL( fullFileName );
                        }
                    CleanupStack::PopAndDestroy( &fullFileName );
                    }
                CleanupStack::PopAndDestroy( fileList );
                }
            }
        }
    }

// end of file
