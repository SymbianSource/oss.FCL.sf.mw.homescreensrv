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
#include <badesca.h>
#include <sacls.h>

#include "mcssuitehandler.h"
#include "menueng.h"
#include "mcsinstallnotifier.h"
#include "mcssuiteparser.h"
#include "mcssuiteobject.h"
#include "menusrvengutils.h"
#include "mcsmenuitem.h"

// ================= LOCAL FUNCTIONS =======================

/**
* Cleanup support method. Call ResetAndDestroy() on the array.
* @param RMenuItemArray* as TAny*
*/
LOCAL_C void ResetAndDestroy( TAny* aArray )
    {
    ((RPointerArray<CSuiteObject>*)aArray)->ResetAndDestroy();
    }

// ---------------------------------------------------------
// CleanupResetAndDestroyPushL
// ---------------------------------------------------------
//
LOCAL_C void CleanupResetAndDestroyPushL( RPointerArray<CSuiteObject>& aArray )
    {
    CleanupStack::PushL( TCleanupItem( ResetAndDestroy, &aArray ) );
    }

// ================= MEMBER FUNCTIONS =======================

// ---------------------------------------------------------
// CMcsSuiteHandler::NewL
// ---------------------------------------------------------
//
CMcsSuiteHandler* CMcsSuiteHandler::NewL( CMenuEng& aEng, const TDesC& aDirName )
	{
	CMcsSuiteHandler* self = new( ELeave ) CMcsSuiteHandler( aEng );
	CleanupStack::PushL( self );
	self->ConstructL(aDirName);
	CleanupStack::Pop( self );
	return self;
	}


// ---------------------------------------------------------
// CMcsSuiteHandler::~CMcsSuiteHandler
// ---------------------------------------------------------
//
CMcsSuiteHandler::~CMcsSuiteHandler()
	{
	iSuites.ResetAndDestroy();
	delete iMmcObserver;
	delete iFreeSpaceObserver;
	delete iInstallNotifier;
	iTimestamps.Close();
	iNewTimestamps.Close();
	iEng.DequeueOperation( *this );
	delete iInstalledFileList;
	delete iFileNewList;
	iDirPath.Close();
	iFs.Close();
	}


// ---------------------------------------------------------
// CMcsSuiteHandler::HaveSuite
// ---------------------------------------------------------
//
TBool CMcsSuiteHandler::HaveSuite(const TDesC& aSuiteName)
	{
	return iSuites.Exist(aSuiteName);
	}

void CMcsSuiteHandler::GetAttribute( const TDesC& aSuiteName, const TDesC& aAttrName, 
	TBool& aAttrExists, TDes& aAttrVal )
	{
	TInt suitePos = iSuites.FindLastSuite(aSuiteName);
	if (suitePos != KErrNotFound)
		{
		iSuites[suitePos]->GetAttribute(aAttrName, aAttrExists, aAttrVal);
		}
	}

// ---------------------------------------------------------
// CMcsSuiteHandler::CMcsSuiteHandler
// ---------------------------------------------------------
//
CMcsSuiteHandler::CMcsSuiteHandler( CMenuEng& aEng ) :
	iEng(aEng)
	{
	}

// ---------------------------------------------------------
// CMcsSuiteHandler::ConstructL
// ---------------------------------------------------------
//
void CMcsSuiteHandler::ConstructL(const TDesC& aDirName)
	{
	User::LeaveIfError( iFs.Connect( ) );
	iDirPath.CreateL( KMaxPath );
	User::LeaveIfError( iFs.PrivatePath( iDirPath ) );
	iDirPath.Append(KImportDir);
	iDirPath.Append(KBackSlash);
	iDirPath.Append(aDirName);
	iDirPath.Append(KBackSlash);
	iFileNewList = new (ELeave) CDesCArrayFlat(KDefaultGranularity);
	iInstalledFileList = new (ELeave) CDesCArrayFlat(KDefaultGranularity);
	iInstallNotifier = CMcsInstallNotifier::NewL(*this, KSAUidSoftwareInstallKeyValue);
	iMmcObserver = CMcsMmcObserver::NewL( *this );
	iFreeSpaceObserver = CMcsFreeSpaceObserver::NewL( *this );
	// Import suites form suite dir
	CheckSuiteDirL();
	iEvent = EInstOpRestore;
	iEng.QueueOperationL( *this );
	}
    

// ---------------------------------------------------------
// CMcsSuiteHandler::HandleInstallNotifyL
// ---------------------------------------------------------
//
void CMcsSuiteHandler::HandleInstallNotifyL(TInt aEvent)
	{
	// Look for new files
    CheckSuiteDirL();
    iEvent = aEvent;
    iEng.QueueOperationL( *this );
    }

// ---------------------------------------------------------
// CMcsSuiteHandler::HandleMmcEventL
// ---------------------------------------------------------
//
void CMcsSuiteHandler::HandleMmcEventL(TInt aEvent)
    {
    CheckSuiteDirL();
    if (aEvent == EMmcInsert)
        {
        iEvent = EInstOpInstall;
        }
    else if (aEvent == EMmcRemove)
        {
        iEvent = EInstOpUninstall;
        }
    iEng.QueueOperationL( *this );
    }

// ---------------------------------------------------------
// CMcsSuiteHandler::HandleFreeSpaceEventL
// ---------------------------------------------------------
//
void CMcsSuiteHandler::HandleFreeSpaceEventL()
    {
    CheckSuiteDirL();
    iEvent = EInstOpUninstall;
    iEng.QueueOperationL( *this );
    }

// ---------------------------------------------------------
// CMcsSuiteHandler::CheckSuiteDirL
// ---------------------------------------------------------
//
void CMcsSuiteHandler::CheckSuiteDirL()
	{
	// Get file list
	iFileNewList->Reset();
	iNewTimestamps.Reset();

	TDriveList driveList;
	TChar driveLetter;
	User::LeaveIfError(iFs.DriveList(driveList));
	for (TInt driveNumber = EDriveA; driveNumber <= EDriveZ; driveNumber++)
		{
		if (driveList[driveNumber])
			{
			User::LeaveIfError(iFs.DriveToChar(driveNumber, driveLetter));
			RBuf filePath;
			filePath.CreateL(KMaxPath);
			CleanupClosePushL(filePath);
			filePath.Append(driveLetter);
			filePath.Append(KColen);
			filePath.Append(iDirPath);
			CDir* fileList;
			
			iFs.GetDir(filePath, KEntryAttMaskSupported,
					ESortByName, fileList);
			
			CleanupStack::PopAndDestroy(&filePath);
			if (fileList)
				{
				CleanupStack::PushL(fileList);
				for (TInt i = 0; i < fileList->Count(); i++)
					{
					// Not dir, xml file
					if (!(*fileList)[i].IsDir() && ValidExtansion(
							(*fileList)[i].iName))
						{
						// Don't add same file names more then once
						// Or else it will case trouble during update
						TInt dummy;
						if ( iFileNewList->Find((*fileList)[i].iName, dummy) )
							{
							iFileNewList->AppendL((*fileList)[i].iName);
							iNewTimestamps.AppendL((*fileList)[i].iModified);
							}
						}
					}
				CleanupStack::PopAndDestroy(fileList);
				}
			}
		}
	}

// ---------------------------------------------------------
// CMcsSuiteHandler::AddNewL
// ---------------------------------------------------------
//
void CMcsSuiteHandler::AddNew()
	{
	for ( TInt i = 0; i < iFileNewList->Count(); i++ )
		{
		TPtrC fileNewName = (*iFileNewList)[i];
		TTime fileTimestamp = iNewTimestamps[i];
		
		if ( !FileInstalled(fileNewName) )
			{
			// Not yet installed, install it
			TRAP_IGNORE( InstallFileL(fileNewName, fileTimestamp) );
			}
		else if (FileNeedUpdate(fileNewName, fileTimestamp) )
			{
			//Installed, but needs update
			TRAP_IGNORE( UpdateFileL( fileNewName, fileTimestamp) );
			}
		}
	}

// ---------------------------------------------------------
// CMcsSuiteHandler::RemoveOldL
// ---------------------------------------------------------
//
void CMcsSuiteHandler::RemoveOld()
	{
	for ( TInt i = 0; i < iInstalledFileList->Count(); i++ )
		{
		TPtrC fileNewName = (*iInstalledFileList)[i]; 

		// File was removed, unistall it
		if ( FileUninstalled(fileNewName) )
			{
			TRAP_IGNORE( UninstallFileL(fileNewName) );
			}
		}
	iInstalledFileList->Compress();
	}

// ---------------------------------------------------------
// CMcsSuiteHandler::ScanSuitesL
// ---------------------------------------------------------
//
void CMcsSuiteHandler::ScanSuitesL()
	{
	RArray<TMenuItem> suiteItems; 
	CleanupClosePushL( suiteItems );
	GetMenuSuitesL( suiteItems, KMenuAttSuiteAdded(), KMenuTrue() );
	
	for ( TInt i = 0; i < suiteItems.Count(); i++ )
		{
		TBool dummy(EFalse);
		TPtrC suiteName(KNullDesC);
		iEng.ObjectL( suiteItems[i].Id() ).
			FindAttribute( KMenuAttSuiteName, suiteName, dummy );

		if ( !HaveSuite( suiteName ) )
			{
			iEng.RemoveL( suiteItems[i].Id() );
			}
		}
	CleanupStack::PopAndDestroy(&suiteItems);
	}

// ---------------------------------------------------------
// CMcsSuiteHandler::FileExistL
// ---------------------------------------------------------
//
TBool CMcsSuiteHandler::FileInstalled(const TDesC& aFileName)
	{
	TInt posInArray;
	// uses Folded Comparing
	if (iInstalledFileList->Find(aFileName, posInArray) == KErrNone)
		{
		return ETrue;
		}
	return EFalse;
	}

// ---------------------------------------------------------
// CMcsSuiteHandler::FileUninstalled
// ---------------------------------------------------------
//
TBool CMcsSuiteHandler::FileUninstalled(const TDesC& aFileName)
	{
	for (TInt j = 0 ; j < iFileNewList->Count(); j++)
		{
		if ( !(*iFileNewList)[j].Compare( aFileName ) )
			{
			// Wasn't removed but maybe needs to be updated (rom case)
			return FileNeedUpdate(aFileName, iNewTimestamps[j]);
			}
		}
	return ETrue;
	}

// ---------------------------------------------------------
// CMcsSuiteHandler::FileNeedUpdate
// ---------------------------------------------------------
//
TBool CMcsSuiteHandler::FileNeedUpdate(const TDesC& aFileName, const TTime& aTimestamp)
	{
	TInt posInArray;
	// uses Folded Comparing
	if (iInstalledFileList->Find(aFileName, posInArray) == KErrNone)
		{
		// If timestamps mach then it's the same file
		return (iTimestamps[posInArray] != aTimestamp);
		}
	return EFalse;
	}

// ---------------------------------------------------------
// CMcsSuiteHandler::ValidExtansion
// ---------------------------------------------------------
//
TBool CMcsSuiteHandler::ValidExtansion(const TDesC& aFileName)
	{
	TPtrC fileExt = 
		aFileName.Mid( aFileName.LocateReverse( KDot ) + 1 );

	return ( fileExt.CompareF(KXmlExt) == 0 );
	}


// ---------------------------------------------------------
// CMcsSuiteHandler::InstallFile
// ---------------------------------------------------------
//
void CMcsSuiteHandler::InstallFileL(const TDesC& aFileName, const TTime& aTimestam)
	{	
	RPointerArray<CSuiteObject> suiteArray;
	CleanupResetAndDestroyPushL(suiteArray);
	
	// Get suite from suite file
	GetSuitesL(aFileName, suiteArray);
	
	//Add filename to installed suites files array
	//If Leave occured during GetSuitesL filename wont be added 
	iInstalledFileList->AppendL(aFileName);
	iTimestamps.AppendL(aTimestam);
	
	// Take the ownership of suite items
	for (TInt i = 0; i < suiteArray.Count(); i++ )
		{
		User::LeaveIfError( iSuites.Append( suiteArray[i] ) );
        TRAPD( err, AddSuiteL( suiteArray[i]->SuiteName() ) );
		if( err!=KErrNone )
		    {
		    if( i==0 )
                {
                iInstalledFileList->Delete( iInstalledFileList->Count()-1 );
                iTimestamps.Remove( iTimestamps.Count()-1 );
                }
            iSuites.Remove( iSuites.Count()-1 );
            User::Leave( err );
		    }
		suiteArray[i] = NULL;
		}
	CleanupStack::Pop(&suiteArray);
	suiteArray.Close();
	}

// ---------------------------------------------------------
// CMcsSuiteHandler::UninstallFile
// ---------------------------------------------------------
//
void CMcsSuiteHandler::UninstallFileL(const TDesC& aFileName)
	{
	TInt pos(KErrNotFound);
	if ( !iInstalledFileList->Find(aFileName, pos))
		{
		
		// Need to know what suites are beeing removed
		CDesCArrayFlat* removedSuites = 
			new (ELeave) CDesCArrayFlat(KDefaultGranularity);
		CleanupStack::PushL(removedSuites);
		for (TInt i = 0; i < iSuites.Count(); i++)
			{
			if ( aFileName.Compare( iSuites[i]->FileName() ) == KErrNone )
			    {
				removedSuites->AppendL( iSuites[i]->SuiteName() );
				}
			}
		
		// We know what suites are beeing removed, we can remove those safly
		iSuites.RemoveByFileName(aFileName);
		
		// Remove uninstalled file from our list
		iInstalledFileList->Delete(pos);
		iTimestamps.Remove(pos);
		
		// And reopen removed suites defined in other files
		for (TInt i = 0; i < removedSuites->Count(); i++)
			{
			TInt suitePos = iSuites.FindLastSuite( (*removedSuites)[i] );
			if (suitePos != KErrNotFound)
				{
				ReopenSuiteL(suitePos);
				}
			else
			    {
			    RemoveSuiteFromXmlL( (*removedSuites)[i] );
			    }
			}

		CleanupStack::PopAndDestroy(removedSuites);
		}
	}

// ---------------------------------------------------------
// CMcsSuiteHandler::UpdateFileL
// ---------------------------------------------------------
//
void CMcsSuiteHandler::UpdateFileL(const TDesC& aFileName, const TTime& aTimestam)
	{
	TInt pos;
	if (iInstalledFileList->Find(aFileName, pos) == KErrNone)
		{
		// Get rid of out of date suites
		iSuites.RemoveByFileName(aFileName);
		iInstalledFileList->Delete(pos);
		//iInstalledFileList->Compress();
		iTimestamps.Remove(pos);
		
		// Instal suites form file name 
		InstallFileL(aFileName, aTimestam);
		}
	}

// ---------------------------------------------------------
// CMcsSuiteHandler::GetSuites
// ---------------------------------------------------------
//
void CMcsSuiteHandler::GetSuitesL(const TDesC& aFileName, 
		RPointerArray<CSuiteObject>& aSuiteArray)
	{
	TDriveList driveList;
    TChar driveLetter;
    User::LeaveIfError( iFs.DriveList( driveList ) );
    for ( TInt driveNumber = EDriveZ; driveNumber >= EDriveA; driveNumber-- )
        {
        if ( driveList[driveNumber] )
            {
            User::LeaveIfError( iFs.DriveToChar( driveNumber, driveLetter ) );
            RBuf file;
            file.CleanupClosePushL();
            file.CreateL( KMaxPath );
            file.Append( driveLetter );
            file.Append( KColen );
            file.Append( iDirPath );
            file.Append( aFileName );
            CSuiteParser* suiteParser = CSuiteParser::NewL(file);
            CleanupStack::PushL(suiteParser);
            TRAP_IGNORE( suiteParser->GetSuitsArrayL( aSuiteArray ) );
            CleanupStack::PopAndDestroy(suiteParser);
            CleanupStack::PopAndDestroy( &file );
            }
        }
	}

// ---------------------------------------------------------
// CMcsSuiteHandler::ReopenSuite
// ---------------------------------------------------------
//
void CMcsSuiteHandler::ReopenSuiteL(TInt aSuiteArrayPos)
	{
	RPointerArray<CSuiteObject> suiteArray;
	CleanupResetAndDestroyPushL(suiteArray);
	
	TPtrC suiteName = iSuites[aSuiteArrayPos]->SuiteName();
	TPtrC fileName = iSuites[aSuiteArrayPos]->FileName();
	
	// Get suite from suite file
	GetSuitesL(fileName, suiteArray);
	
	// Find our suite in suites from specified file
	CSuiteObject* neededSuite = NULL;
	for (TInt j = 0; j < suiteArray.Count(); j++)
		{
		if (suiteArray[j]->SuiteName().Compare(suiteName) == KErrNone)
			{
			// Take ownership of Suite Object
			neededSuite = suiteArray[j];
			suiteArray.Remove(j);
			break;
			}
		}
	
	if ( neededSuite )
		{
		// Replace Suite Object in suiteArray
		delete iSuites[aSuiteArrayPos];
		iSuites[aSuiteArrayPos] = neededSuite;
		}
	else
		{
		// Must be some error, delete that suite
		delete iSuites[aSuiteArrayPos];
		iSuites.Remove( aSuiteArrayPos );
		}
	
	CleanupStack::PopAndDestroy(&suiteArray);
	}

// ---------------------------------------------------------
// CMcsSuiteHandler::GetMenuSuitesL
// ---------------------------------------------------------
//
void CMcsSuiteHandler::GetMenuSuitesL( 
		RArray<TMenuItem>& aItemArray, 
		const TDesC& aAttrName, 
		const TDesC& aAttrValue )
    {
    TInt root( 0 );
    iEng.RootFolderL( root );
    TMenuSrvTypeAttrFilter suiteFilter;
    suiteFilter.SetType( KMenuTypeSuite() );   
    suiteFilter.SetAttr( aAttrName, aAttrValue );
    iEng.GetItemsL( aItemArray, root, &suiteFilter, ETrue );
    }

// ---------------------------------------------------------
// CMcsSuiteHandler::AddSuiteL
// ---------------------------------------------------------
//
void CMcsSuiteHandler::AddSuiteL( const TPtrC& aSuiteName )
    {
    RArray<TMenuItem> mcsItems; 
    CleanupClosePushL( mcsItems );
    GetMenuSuitesL( mcsItems, KMenuAttSuiteName(), aSuiteName );
    
    if( mcsItems.Count() == KErrNone )
        {
        AddSuiteToXmlL( aSuiteName );
        }
    else
        {
        UpdateItemsL( mcsItems );
        }
    CleanupStack::PopAndDestroy(&mcsItems);
    }


// ---------------------------------------------------------
// CMcsSuiteHandler::AddSuiteL
// ---------------------------------------------------------
//
void CMcsSuiteHandler::RemoveSuiteFromXmlL( const TPtrC& aSuiteName )
    {
    RArray<TMenuItem> mcsItems; 
    CleanupClosePushL( mcsItems );
    GetMenuSuitesL( mcsItems, KMenuAttSuiteName(), aSuiteName );

    for( TInt i( 0 ); i < mcsItems.Count(); i++ )
        {
        UpdateSuiteInXmlL( mcsItems, i );
        }
    
    CleanupStack::PopAndDestroy(&mcsItems);
    }

// ---------------------------------------------------------
// CMcsSuiteHandler::UpdateSuiteInXmlL
// ---------------------------------------------------------
//
void CMcsSuiteHandler::UpdateSuiteInXmlL( 
		RArray<TMenuItem> & aMcsItems, TInt aIndex )
	{
	TBool dummy(EFalse);
	TPtrC val(KNullDesC);
	if( iEng.ObjectL(aMcsItems[aIndex].Id()).
			FindAttribute( KMenuAttSuiteAdded, val, dummy ) )
		{
		if ( val.Compare(KMenuTrue) == KErrNone )
			{
			iEng.RemoveL(aMcsItems[aIndex].Id());
			}
		else if ( val.Compare(KMenuFalse) == KErrNone )
			{
			iEng.ModifiableObjectL(aMcsItems[aIndex].Id()).
				RemoveAttribute( KMenuAttSuiteAdded );
			}
		}
	}

// ---------------------------------------------------------
// CMcsSuiteHandler::RunMenuEngOperationL
// ---------------------------------------------------------
//
void CMcsSuiteHandler::RunMenuEngOperationL()
    {
    if ( iEvent & EInstOpInstall )
        {
        // Add new suites
        AddNew();
        }
    else if ( ( iEvent & EInstOpUninstall ) )
        {
        // Remove old suites, andd new if needed (stub sis case)
        RemoveOld();
        AddNew();
        }
    else if( ( iEvent & EInstOpRestore ) )
    	{
        AddNew();
        ScanSuitesL();
    	}
    }

// ---------------------------------------------------------
// CMcsSuiteHandler::CompletedMenuEngOperation
// ---------------------------------------------------------
//
void CMcsSuiteHandler::CompletedMenuEngOperation( TInt /*aErr*/ )
    {
    
    }
// ---------------------------------------------------------
// CMcsSuiteHandler::AddSuiteToXmlL
// ---------------------------------------------------------
//
void CMcsSuiteHandler::AddSuiteToXmlL( const TPtrC& aSuiteName )
    {
    TInt root( 0 );
    iEng.RootFolderL( root );
    
    CMenuEngObject* obj = iEng.NewObjectL( KMenuTypeSuite );
    CleanupStack::PushL( obj );
    obj->SetAttributeL( KMenuAttSuiteName, aSuiteName, EFalse );
    obj->SetAttributeL( KMenuAttSuiteAdded, KMenuTrue, EFalse );
    iEng.AddL( *obj, root, 0 );
    CleanupStack::Pop( obj );
    }

// ---------------------------------------------------------
// CMcsSuiteHandler::UpdateItemsL
// ---------------------------------------------------------
//
void CMcsSuiteHandler::UpdateItemsL( 
    const RArray<TMenuItem>& aItemArray )
    {
    for( TInt i( 0 ); i < aItemArray.Count(); ++i )
        {
        TPtrC val(KNullDesC);
        TBool dummy( EFalse );
        iEng.ObjectL( aItemArray[i].Id() ).
            FindAttribute( KMenuAttSuiteAdded, val, dummy );
        if( val.Compare( KMenuTrue ) != KErrNone )
            {
            iEng.ModifiableObjectL( aItemArray[i].Id() ).
                        SetAttributeL( KMenuAttSuiteAdded, KMenuFalse, EFalse);
            }
        else
        	{
        	iEng.ModifiableObjectL( aItemArray[i].Id() ).
        	            SetAttributeL( KMenuAttSuiteAdded, KMenuTrue, EFalse);
        	}
        }
    }

