/*
* Copyright (c) 2009 Nokia Corporation and/or its subsidiary(-ies).
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
#include <apgcli.h>
#include <swi/sisregistrysession.h>
#include <swi/sisregistryentry.h>
#include <javaregistry.h>
#include <javaregistrypackageentry.h>
#include <javaregistryapplicationentry.h>
#include <mcsmenuutils.h>
#include <utf.h>
#include "mcsdef.h"
#include "mcsmenu.h"
#include "mcsmenuitem.h"
#include "menuuninstalloperation.h"

_LIT8( KAppMimeType, "x-epoc/x-sisx-app" );
_LIT8( KMidletMimeType, "application/java-archive" );
_LIT8( KWidgetMimeType, "application/x-nokia-widget");

const TUid KMidletType = { 0x10210E26 };


// ========================= LOCAL FUNCTIONS ===============

// ---------------------------------------------------------
// CleanupResetAndDestroy()
// ---------------------------------------------------------
//
template<class T>
static void CleanupResetAndDestroy( TAny* aObj )
    {
    if( aObj )
        {
        static_cast<T*>( aObj )->ResetAndDestroy();
        }
    }

// ---------------------------------------------------------
// CleanupResetAndDestroyPushL
// ---------------------------------------------------------
//
template<class T>
static void CleanupResetAndDestroyPushL(T& aArray)
    {
    CleanupStack::PushL( TCleanupItem( &CleanupResetAndDestroy<T>, &aArray ) );
    }

// ========================= MEMBER FUNCTIONS ==============

// ---------------------------------------------------------
// CMenuUninstallOperation::~CMenuUninstallOperation
// ---------------------------------------------------------
//
CMenuUninstallOperation::~CMenuUninstallOperation()
    {
    Cancel();
    iUninstaller.Close();
    delete iRemoveOperation;
    iWidgetRegistry.Close();
    }

// ---------------------------------------------------------
// CMenuUninstallOperation::NewL
// ---------------------------------------------------------
//
CMenuUninstallOperation* CMenuUninstallOperation::NewL(
    RMenu &aMenu,
    TInt aPriority,
    TRequestStatus &aObserverStatus,
    CMenuItem& aItem )
    {
    CMenuUninstallOperation* self;

    self = new ( ELeave ) CMenuUninstallOperation( aMenu, aPriority, aObserverStatus );
    CleanupStack::PushL( self );
    self->ConstructL( aItem );
    CleanupStack::Pop( self );
    return self;
    }

// ---------------------------------------------------------
// CMenuUninstallOperation::CMenuUninstallOperation
// ---------------------------------------------------------
//
CMenuUninstallOperation::CMenuUninstallOperation
( RMenu &aMenu, TInt aPriority, TRequestStatus &aObserverStatus )
: CMenuOperation( aMenu, aPriority, aObserverStatus ),
iState( EIdle ),
iRemoveOperation( NULL )
    {
    CActiveScheduler::Add( this );
    }

// ---------------------------------------------------------
// CMenuUninstallOperation::ConstructL
// ---------------------------------------------------------
//
void CMenuUninstallOperation::ConstructL( CMenuItem& aItem )
    {
    User::LeaveIfError( iWidgetRegistry.Connect() );
    TBool exists;
    TPtrC uidAttr( aItem.GetAttributeL( KMenuAttrUid, exists ) );
    TUint uid;
    TUid packageUid = KNullUid;
    

    if( !exists )
        {
        User::Leave( KErrCorrupt );
        }

    if( aItem.Flags() & TMenuItem::ELockDelete )
    	{
        iObserverStatus = KRequestPending;
        SetActive();
        TRequestStatus* status = &iStatus;
        User::RequestComplete( status, KErrAccessDenied );
        return;
    	}

    User::LeaveIfError( iUninstaller.Connect() );

    iId = aItem.Id();
    
    // Prepare parameters
    MenuUtils::GetTUint( uidAttr, uid );
    
    
    RBuf8 mimeType;
    mimeType.CleanupClosePushL();
    AppInfoL( TUid::Uid( uid ), mimeType, packageUid );

    // Commence the uninstallations
    iUninstaller.Uninstall( iStatus, packageUid, mimeType );
    iObserverStatus = KRequestPending;
    if( mimeType == KMidletMimeType()
            && IsWidget( TUid::Uid( uid ) ) )	
		{
    	//we remove java type app(it will gain different uid
		//during next install) and widget type app(it MIGHT get
		//different uid during next install) from xml
    	iState = EUninstalling;
    	}
    else
    	{
    	//if its not java or widget app we leave it in xml,
    	//appscanner will hide it for later passible reinstallation 
    	iState = ERemoving;
    	}
    CleanupStack::PopAndDestroy( &mimeType );
    SetActive();
    }

// ---------------------------------------------------------
// CMenuUninstallOperation::AppInfo
// ---------------------------------------------------------
//
void CMenuUninstallOperation::AppInfoL( const TUid& aAppUid, 
        RBuf8& aMimeType, TUid& aPackageUid )
	{
	TUid typeUid;
	RApaLsSession apaLsSession;

	User::LeaveIfError( apaLsSession.Connect() );
	CleanupClosePushL( apaLsSession );
	User::LeaveIfError( apaLsSession.GetAllApps() );

    if( KErrNone == apaLsSession.GetAppType( typeUid, aAppUid  ) &&
    		typeUid == KMidletType )
        {
        GetJavaSuitUidL( aAppUid, aPackageUid );
        User::LeaveIfError( aMimeType.Create( KMidletMimeType() ) );
        }
    else if( IsWidget( aAppUid ) )
        {
        aPackageUid = aAppUid;
        CWidgetPropertyValue* widgetProperty( iWidgetRegistry.
                GetWidgetPropertyValueL( aAppUid, EMimeType ) );
        CleanupStack::PushL( widgetProperty );
        TPtrC mimeType( *widgetProperty );
        if (mimeType.Length() == 0)
            {
            User::LeaveIfError( aMimeType.Create( KWidgetMimeType() ) );
            }
        else 
            {
            HBufC8*  mimeType8 = CnvUtfConverter::ConvertFromUnicodeToUtf8L( mimeType );
            CleanupStack::PushL( mimeType8 );
            User::LeaveIfError( aMimeType.Create( *mimeType8 ) );
            CleanupStack::PopAndDestroy( mimeType8 );
            }
        CleanupStack::PopAndDestroy( widgetProperty );
        }
    else
        {
        TApaAppInfo appInfo;
        User::LeaveIfError( apaLsSession.GetAppInfo( appInfo, aAppUid ) );
        if( !GetInstallPkgUidL( appInfo.iFullName, aPackageUid ) )
        	{
        	aPackageUid = aAppUid;
        	}
        User::LeaveIfError( aMimeType.Create( KAppMimeType() ) );
        }

    CleanupStack::PopAndDestroy( &apaLsSession );
	}

// ---------------------------------------------------------
// CMenuUninstallOperation::GetInstallPkgUidL
// ---------------------------------------------------------
//
TBool CMenuUninstallOperation::GetInstallPkgUidL( const TDesC& aAppFullName, TUid& aPackageUid )
	{
	// Search for the full name of the application amongst every file name in
	// every installed packages.
	TBool found = EFalse;
	Swi::RSisRegistrySession iSisRegSession;

	// Get the array of ids of every installed packages
	if( KErrNone != iSisRegSession.Connect() )
		{
		return found;
		}
	CleanupClosePushL( iSisRegSession );
    
    RArray<TUid> packageIds;
    CleanupClosePushL( packageIds );
    
    iSisRegSession.InstalledUidsL( packageIds );
    
    RPointerArray< HBufC > packageFiles;
    CleanupResetAndDestroyPushL( packageFiles );
    
    for( TInt i = 0; i < packageIds.Count() && !found; ++i )
        {
        const TUid packageId = packageIds[i];
        Swi::RSisRegistryEntry packageEntry;

        // Get the array of file names in the current install package and look
        // if there is one suggesting that the application was installed from
        // the package.
        if( KErrNone == packageEntry.Open( iSisRegSession, packageId ) )
        	{
	        CleanupClosePushL( packageEntry );
	        packageEntry.FilesL( packageFiles );
	        for( TInt pf = 0; pf < packageFiles.Count() && !found; ++pf )
	            {
	            if( packageFiles[pf]->FindC( aAppFullName ) == 0 )
	                {
	                aPackageUid = packageId;
	                found = ETrue;
	                }
	            }
	        packageFiles.ResetAndDestroy();
	        CleanupStack::PopAndDestroy( &packageEntry );    
        	}
        }
        
    CleanupStack::PopAndDestroy( &packageFiles );
    CleanupStack::PopAndDestroy( &packageIds );
    CleanupStack::PopAndDestroy( &iSisRegSession );

    return found;
	}

// ---------------------------------------------------------
// CMenuUninstallOperation::GetJavaSuitUidL
// ---------------------------------------------------------
//
void CMenuUninstallOperation::GetJavaSuitUidL( const TUid& aAppUid, TUid& aPackageUid )
	{
	Java::CJavaRegistry* javaRegistry;
	javaRegistry = Java::CJavaRegistry::NewLC();

	Java::CJavaRegistryEntry* regEntry = 
		javaRegistry->RegistryEntryL( aAppUid );
	if( regEntry )
		{
	CleanupStack::PushL( regEntry );

	Java::TJavaRegistryEntryType entryType = regEntry->Type();
	
	if ( ( entryType >= Java::EGeneralPackage ) && 
			(entryType < Java::EGeneralApplication) )
		{
		//package entry
		aPackageUid = regEntry->Uid();
		}
	else 
		{
		//application entry
		Java::CJavaRegistryApplicationEntry* regApplicationEntry = 
			static_cast<Java::CJavaRegistryApplicationEntry*>( regEntry );
		Java::CJavaRegistryPackageEntry* regPackageEntry = 
			regApplicationEntry->PackageEntryL();
		aPackageUid = regPackageEntry->Uid();
		delete regPackageEntry;
		}
		CleanupStack::PopAndDestroy( regEntry );
		}
	else
		{
		aPackageUid = aAppUid;
		}
	CleanupStack::PopAndDestroy( javaRegistry );
	}

// ---------------------------------------------------------
// CMenuUninstallOperation::IsWidget
// ---------------------------------------------------------
//

TBool CMenuUninstallOperation::IsWidget( const TUid& aAppUid )
    {
    return iWidgetRegistry.IsWidget( aAppUid );
    }

// ---------------------------------------------------------
// CMenuUninstallOperation::RunL
// ---------------------------------------------------------
//
void CMenuUninstallOperation::RunL()
    {
    if( KErrNone == iStatus.Int() )
        {
        switch( iState )
            {
            case EUninstalling:
                {
                __ASSERT_DEBUG( NULL == iRemoveOperation, User::Invariant() );
                iState = ERemoving;
                iRemoveOperation = iMenu.RemoveL( iId, iStatus );
                SetActive();
                }
                break;

            case ERemoving:
                {
                delete iRemoveOperation;
                iRemoveOperation = NULL;
                iState = EIdle;
                TRequestStatus* status = &iObserverStatus;
                User::RequestComplete( status, iStatus.Int() );
                }
                break;

            default:
                // Object is in an unexpected state
                __ASSERT_DEBUG( 0, User::Invariant() );
            }
        }
    else if ( KErrNotFound == iStatus.Int() && iState == ERemoving )
    	{
    	// Most probably: After uninstalling the application the application
    	// scanner removed the corresponding item from the xml so not finding
    	// the menuitem is not an error here.
        delete iRemoveOperation;
        iRemoveOperation = NULL;
        iState = EIdle;
        TRequestStatus* status = &iObserverStatus;
        User::RequestComplete( status, KErrNone );
    	}
    else
        {
        // Report error
        delete iRemoveOperation;
        iRemoveOperation = NULL;
        iState = EIdle;
        TRequestStatus* status = &iObserverStatus;
        User::RequestComplete( status, iStatus.Int() );
        }
    }

// ---------------------------------------------------------
// CMenuUninstallOperation::DoCancel
// ---------------------------------------------------------
//
void CMenuUninstallOperation::DoCancel()
    {
    switch( iState )
        {
        case EUninstalling:
            {
            iState = EIdle;
            }
            break;

        case ERemoving:
            {
            delete iRemoveOperation;
            iRemoveOperation = NULL;
            iState = EIdle;
            }
            break;

        default:
            // Object is in an unexpected state
            __ASSERT_DEBUG( 0, User::Invariant() );
        }
    iUninstaller.CancelAsyncRequest( SwiUI::ERequestUninstall );
    // Complete the caller:
    TRequestStatus* status = &iObserverStatus;
    User::RequestComplete( status, KErrCancel );
    }

// ---------------------------------------------------------
// CMenuUninstallOperation::RunError
// ---------------------------------------------------------
//
TInt CMenuUninstallOperation::RunError( TInt aError )
    {
    TRequestStatus* status = &iObserverStatus;
    User::RequestComplete( status, aError );

    return KErrNone;
    }
