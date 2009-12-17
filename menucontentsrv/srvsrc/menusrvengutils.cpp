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
#include <mcsmenuutils.h>
#include <AknIconUtils.h>
#include <avkon.mbg>
#include <javaregistry.h>
#include <javaregistryentry.h>
#include <drmrightsinfo.h>
#include <e32property.h>
#include <SATDomainPSKeys.h>
#include <AknTaskList.h> 
#include <mmf/common/mmfcontrollerpluginresolver.h>
#include <WidgetRegistryClient.h>

#include "menusrvengutils.h"

#include "mcsdef.h"
#include "mcsmenu.h"
#include "mcsmenuitem.h"
#include "mcsmenufilter.h"
#include "mcssathandler.h"
#include "menusrvappscanner.h"

_LIT( KMidletPostfix, ".fakeapp" );

// ================= LOCAL FUNCTIONS ========================

// ---------------------------------------------------------
// GetAppUid
// ---------------------------------------------------------
//
TInt CMenuSrvEngUtils::GetAppUid( const CMenuEngObject& aEngObj, TUid& aAppUid )
    {
    TInt err( 0 );
    TPtrC attrVal;
    TBool attrExists( EFalse );
    TBool localized( EFalse );//
    attrExists = aEngObj.FindAttribute(KMenuAttrUid, attrVal, localized);
    if ( !attrExists ) 
    	{
    	err = KErrNotFound;
    	}
    else
        {
        TUint appUid;
        err = MenuUtils::GetTUint( attrVal, appUid );
        if ( !err )
            {
            aAppUid = TUid::Uid( appUid );
            }
        }
    return err;
    }
    
// ---------------------------------------------------------
// GetApaAppInfo
// ---------------------------------------------------------
//
TInt CMenuSrvEngUtils::GetApaAppInfo( 
    const CMenuEngObject& aEngObj, 
    TApaAppInfo& aInfo ) const
    {
    TUid uid;
    TInt err;
    err = CMenuSrvEngUtils::GetAppUid( aEngObj, uid );
    if ( err == KErrNone )
        {
        err = GetApaAppInfo( uid, aInfo );
        }
        
    return err;
    }

// ---------------------------------------------------------
// GetApaAppInfo
// ---------------------------------------------------------
//
TInt CMenuSrvEngUtils::GetApaAppInfo( 
    TUid aUid, 
    TApaAppInfo& aInfo ) const
    {   
    return iApaLsSession.GetAppInfo( aInfo, aUid );
    }
// ---------------------------------------------------------
// CMenuSrvEngUtils::GetJavaContentIdL
// ---------------------------------------------------------
//
void CMenuSrvEngUtils::GetJavaContentIdL( TUid aUid, TDes& aContentId )
	{
	Java::CJavaRegistry* javaRegistry;
	Java::CJavaRegistryEntry* regEntry = NULL;

    javaRegistry = Java::CJavaRegistry::NewL();
    CleanupStack::PushL( javaRegistry );

    //regEntry will be NULL if there is no Java application 
    //corresponding to the specified aUid
    regEntry = javaRegistry->RegistryEntryL( aUid );
    
    //DrmContentId() will return reference to NULL when
    //Java application is not DRM protected
    if( regEntry && ( &regEntry->DrmContentId() ) )
    	{
   		aContentId.Copy( regEntry->DrmContentId() );
    	}
    else
    	{
    	aContentId.Copy( KNullDesC );
    	}
    
    delete regEntry;
    CleanupStack::PopAndDestroy( javaRegistry );
	}


// ================= MEMBER FUNCTIONS =======================

// ---------------------------------------------------------
// CMenuSrvEngUtils::~CMenuSrvEngUtils
// ---------------------------------------------------------
//
CMenuSrvEngUtils::~CMenuSrvEngUtils()
    {
    delete iSatHandler;
    iApaLsSession.Close();
    iWidgetSession.Close();
    iWsSession.Close();
    }

// ---------------------------------------------------------
// CMenuSrvEngUtils::NewL
// ---------------------------------------------------------
//
CMenuSrvEngUtils* CMenuSrvEngUtils::NewL( CMenuEng& aEng )
    {
    CMenuSrvEngUtils* utils = new (ELeave) CMenuSrvEngUtils( aEng );
    CleanupStack::PushL( utils );
    utils->ConstructL();
    CleanupStack::Pop( utils );
    return utils;
    }

// ---------------------------------------------------------
// CMenuSrvEngUtils::CMenuSrvEngUtils
// ---------------------------------------------------------
//
CMenuSrvEngUtils::CMenuSrvEngUtils( CMenuEng& aEng )
:   iEng( aEng )
    {
    }

// ---------------------------------------------------------
// CMenuSrvEngUtils::ConstructL
// ---------------------------------------------------------
//
void CMenuSrvEngUtils::ConstructL()
    {
    User::LeaveIfError( iApaLsSession.Connect() );
    User::LeaveIfError( iWidgetSession.Connect() );
    User::LeaveIfError( iWsSession.Connect() );
    iSatHandler = CMcsSatHandler::NewL();
    }

// ---------------------------------------------------------
// CMenuSrvEngUtils::GetAppInfo
// ---------------------------------------------------------
//
TInt CMenuSrvEngUtils::GetAppInfo( const CMenuEngObject& aEngObj, TApaAppInfo& aAppInfo ) const
    {
    TUid appUid;
    TInt err = GetAppUid( aEngObj, appUid );
    if ( !err )
        {
        if( appUid == KSatUid )
            {
            err = GetSatAppInfo( aAppInfo );
            }
        else
            {
            err = iApaLsSession.GetAppInfo( aAppInfo, appUid );
            }
        }
    return err;
    }

// ---------------------------------------------------------
// CMenuSrvEngUtils::GetAppInfo
// ---------------------------------------------------------
//
TInt CMenuSrvEngUtils::GetAppInfoL( TInt aItemId, TApaAppInfo& aAppInfo ) const
    {
    TInt err( 0 );
    const CMenuEngObject& obj = iEng.ObjectL( aItemId );
    err = GetAppInfo( obj, aAppInfo );
    return err;
    }


// ---------------------------------------------------------
// CMenuSrvEngUtils::GetFolderIcon
// Knowing that in case of folders there is no way to query 
// folder's icon file from APPARC, the responsibility of handling this is
// transferred to the caller of this method, so that he may show some default or
// display error etc.
// ---------------------------------------------------------
//
TInt CMenuSrvEngUtils::GetFolderIcon( const CMenuEngObject& /*aEngObj*/, 
                                 HBufC*& aFullFileName,
                                 TInt& /*aBitmapId*/,
                                 TInt& /*aMaskId*/ ) const
    {
    __ASSERT_ALWAYS( !aFullFileName, User::Invariant() );
    
    return KErrNotFound;
    }

// ---------------------------------------------------------
// CMenuSrvEngUtils::GetFolderIcon
// ---------------------------------------------------------
//
TInt CMenuSrvEngUtils::GetFolderIconL( TInt aItemId, 
                                 HBufC*& aFullFileName,
                                 TInt& aBitmapId,
                                 TInt& aMaskId ) const
    {
    __ASSERT_ALWAYS( !aFullFileName, User::Invariant() );

    TInt err( 0 );
    const CMenuEngObject& obj = iEng.ObjectL( aItemId );
    err = GetFolderIcon( obj, aFullFileName, aBitmapId, aMaskId );
    return err;
    }


// ---------------------------------------------------------
// CMenuSrvEngUtils::IsNative
// ---------------------------------------------------------
//
void CMenuSrvEngUtils::IsNative( const CMenuEngObject& aEngObj, 
    TBool& aAttrExists, TDes& aAttrVal )
    {
    aAttrExists = EFalse;
    if( aEngObj.Type().CompareF( KMenuTypeApp ) == KErrNone )
        {
        TInt err;
        TUid uid;
        err = GetAppUid( aEngObj, uid );
        if( err == KErrNone )
            {
            TBool native(EFalse);
            err = IsNative( uid, native );
        	if( !err )
        		{
        		aAttrExists = ETrue;
                if( native )
                    {
                    aAttrVal = KMenuTrue();
                    }
                else
                    {
                    aAttrVal = KMenuFalse();
                    }
        		}
            }
        }
    if( !aAttrExists )
        {
        aAttrVal = KNullDesC();
        }
    }

// ---------------------------------------------------------
// CMenuSrvEngUtils::IsNativeL
// ---------------------------------------------------------
//

TInt CMenuSrvEngUtils::IsNative( TUid aAppUid, TBool& aIsNative )
	{
	TInt error(KErrNotFound);
	TApaAppInfo appInfo;
	
	error = iApaLsSession.GetAppInfo( appInfo, aAppUid );
	if( error == KErrNone )
		{
		error = iApaLsSession.IsProgram( appInfo.iFullName, aIsNative );
		}
	
	return error;
	}

// ---------------------------------------------------------
// CMenuSrvEngUtils::GetDrmProtectionByContentIdL
// ---------------------------------------------------------
//
TInt CMenuSrvEngUtils::GetDrmProtectionByContentIdL(const TDesC& aContentId,
		TDRMProtectionInfo& aProtectionInfo) const
    {
    TInt err( KErrNone );
    DRM::CDrmRightsInfo* drmInfo = DRM::CDrmRightsInfo::NewL();
    CleanupStack::PushL( drmInfo );

    {
    DRM::TDrmRightsInfo drmRightInfo;
    TRAP( err, drmInfo->CheckRightsL( aContentId,
    		ContentAccess::EExecute,
    		drmRightInfo ); )
    if( err == KErrNone )
    	{
    	switch (drmRightInfo)
    		{
    		case (DRM::EURightsInfoValid)://1
    		aProtectionInfo = EDRMProtected;
    		break;

    		case (DRM::EURightsInfoExpired)://2
    		aProtectionInfo = EDRMRightsExpired;
    		break;

    		case (DRM::EURightsInfoMissing)://3
    		aProtectionInfo = EDRMRightsExpired;
    		break;
    		case (DRM::EURightsInfoFuture)://4

    		default:
    			aProtectionInfo = EDRMUnknownProtection;
    			break;
    		}
    	}
    }

    CleanupStack::PopAndDestroy( drmInfo );
    return err;    
    }

		
// ---------------------------------------------------------
// CMenuSrvEngUtils::GetDrmProtection
// ---------------------------------------------------------
//
TInt CMenuSrvEngUtils::GetDrmProtectionL( 
    const TUid aAppUid, 
    TDRMProtectionInfo& aProtectionInfo ) const
    {
    TInt err( KErrNone );
    
    TApaAppInfo info;
    
    aProtectionInfo = EDRMUnknownProtection;
    
    err = GetApaAppInfo( aAppUid, info );
    if ( err == KErrNone )
        {
        if( IsMiddlet( info ) )
        	{
        	TBuf<KMaxFileName> contentId;
        	GetJavaContentIdL( info.iUid, contentId );
        	if ( contentId != KNullDesC ) 
        		{
        		GetDrmProtectionByContentIdL( contentId, aProtectionInfo );
        		}
	        }
        }


    return err;
    }

// ---------------------------------------------------------
// CMenuSrvEngUtils::GetDrmProtection
// ---------------------------------------------------------
//
TInt CMenuSrvEngUtils::GetDrmProtectionL( 
    const CMenuEngObject& aEngObj, 
    TDRMProtectionInfo& aProtectionInfo ) const
    {
    TInt err( KErrNone );
    
    if ( aEngObj.Type().CompareF( KMenuTypeApp ) == 0 )
        {
        TUid uid;
        err = CMenuSrvEngUtils::GetAppUid( aEngObj, uid );
        if ( err == KErrNone )
            {
            err = GetDrmProtectionL( uid, aProtectionInfo );
            }
        }
    else
        {
        err = KErrNone;
        aProtectionInfo = EDRMUnknownProtection;
        }

    return err;
    }

// ---------------------------------------------------------
// CMenuSrvEngUtils::GetDrmProtection
// ---------------------------------------------------------
//
TInt CMenuSrvEngUtils::GetDrmProtectionL(TInt aItemId,
		TDRMProtectionInfo& aProtectionInfo) const
    {
    TInt err( 0 );
    const CMenuEngObject& obj = iEng.ObjectL( aItemId );
    err = GetDrmProtectionL( obj, aProtectionInfo );
    return err;
    }
/*
// ---------------------------------------------------------
// CMenuSrvEngUtils::GetDrmProtection
// ---------------------------------------------------------
//
TBool CMenuSrvEngUtils::GetAppRunningL( TInt aItemId )
    {
    TInt err( 0 );
    const CMenuEngObject& obj = iEng.ObjectL( aItemId );
    err = GetAppRunningL( obj );
    return err;
    }

// ---------------------------------------------------------
// CMenuSrvEngUtils::GetDrmProtection
// ---------------------------------------------------------
//
TBool CMenuSrvEngUtils::GetAppRunningL( const CMenuEngObject& aEngObj )
    {
    TBool runningApp( EFalse );
    if ( aEngObj.Type().CompareF( KMenuTypeApp ) == KErrNone )
        {
        TUid appUid;
        TInt errorUid = GetAppUid( aEngObj, appUid );
        
        if( !errorUid )
            {
            CheckAppRunningL( appUid , runningApp );
            }
        else 
            {
            return EFalse;
            }
        }
    else if( aEngObj.Type().CompareF( KMenuTypeFolder ) == KErrNone )
        {
        GetChildrenRunningAppL( aEngObj.Id(), runningApp );
        }

    return runningApp;
    }

// ---------------------------------------------------------
// CMenuSrvEngUtils::GetChildrenRunningAppL
// ---------------------------------------------------------
//
void CMenuSrvEngUtils::GetChildrenRunningAppL( TInt id, 
        TBool& aRunningApp )
    {
    TMenuSrvTypeFilter filter;
    filter.SetType( KMenuTypeApp() );
    
    RArray<TMenuItem> aItemArray;
    CleanupClosePushL(aItemArray);
    iEng.GetItemsL( aItemArray , id , &filter, ETrue );
    for( TInt i = 0; i < aItemArray.Count(); i++)
        {
        const CMenuEngObject& obj = iEng.ObjectL( aItemArray[i].Id() );
        TUid appUid;
        TInt errorUid = GetAppUid( obj, appUid );
        
        if( !errorUid &&  
                !(obj.Flags() & TMenuItem::EHidden) )
            {
            CheckAppRunningL(  appUid , aRunningApp );
            }
        if( aRunningApp )
            {
            break;
            }
        }
    CleanupStack::PopAndDestroy( &aItemArray );
    }

// ---------------------------------------------------------
// CMenuSrvEngUtils::CheckAppRunning
// ---------------------------------------------------------
//
void CMenuSrvEngUtils::CheckAppRunningL( TUid aUid, TBool& aRunningApp )
    {
    if ( iWidgetSession.IsWidget( aUid ) )
        {
        aRunningApp = iWidgetSession.IsWidgetRunning( aUid );        
        }
    else
        {
        CAknTaskList* taskList = CAknTaskList::NewLC( iWsSession );
        
        TApaTask task = taskList->FindRootApp( aUid );
        aRunningApp = task.Exists();
        
        CleanupStack::PopAndDestroy( taskList );
        }
    }*/

// ---------------------------------------------------------
// CMenuSrvEngUtils::GetSatAppInfo
// ---------------------------------------------------------
//
TInt CMenuSrvEngUtils::GetSatAppInfo( TApaAppInfo& aAppInfo ) const
    {
    TBuf<KApaMaxAppCaption> aBuf;
    TInt err = iSatHandler->GetName( aBuf );
    if( err == KErrNone )
        {
        aAppInfo.iShortCaption = aBuf;
        aAppInfo.iCaption = aBuf;
        }
    return err;
    }

// ---------------------------------------------------------
// MenuSrvUtil::SetValueSkinMajorId
// ---------------------------------------------------------
//
void CMenuSrvEngUtils::SetValueSkinId( TDes& aAttrName, TInt aValue )
	{
	MenuUtils::SetTUint( aAttrName, aValue );
	}

// ---------------------------------------------------------
// MenuSrvUtil::UidToStringL
// ---------------------------------------------------------
//
void MenuSrvUtil::UidToStringL( TUint aUid, TDes& aResult,
     TBool aLegacy, TRadix aRadix )
    {
    aResult.Zero();
    
    TBuf<KUidChars> number;
    if( !aLegacy )
        {
        if (aRadix == EHex)
            {
            aResult.Append( KHex );   
            }
        }
    number.AppendNum( aUid, aRadix );
    number.UpperCase();
  
    aResult.Append( number );
   
    }

// ---------------------------------------------------------
// TMenuSrvTypeFilter::MatchesObject
// ---------------------------------------------------------
//
TBool TMenuSrvTypeFilter::MatchesObject
( const CMenuEngObject& aObject ) const
    {
    return iType == aObject.Type();
    }

// ==================== MEMBER FUNCTIONS ====================

// ---------------------------------------------------------
// TMenuSrvTypeAttrFilter::MatchesObject
// ---------------------------------------------------------
//
TBool TMenuSrvTypeAttrFilter::MatchesObject
( const CMenuEngObject& aObject ) const
    {
    if ( TMenuSrvTypeFilter::MatchesObject( aObject ) )
        {
        TPtrC val;
        TBool dummy;
        if( aObject.FindAttribute( iAttrName, val, dummy ) )
            {
            if( !val.CompareF( iAttrValue ) )
                {
                return ETrue; // Match.
                }
            }
        }
    return EFalse;
    }

// ---------------------------------------------------------
// TMenuSrvAttrFilter::MatchesObject
// ---------------------------------------------------------
//
TBool TMenuSrvAttrFilter::MatchesObject
( const CMenuEngObject& aObject ) const
    {
    TPtrC val;
    TBool dummy;
    if( aObject.FindAttribute( iAttrName, val, dummy ) )
        {
        if( !val.CompareF( iAttrValue ) )
            {
            return ETrue; // Match.
            }
        }
    return EFalse;
    }

// ==================== MEMBER FUNCTIONS ====================

// ---------------------------------------------------------
// TMenuSrvTypeAttrFilter::MatchesObject
// ---------------------------------------------------------
//
TBool TMenuSrvTypeAttrExistsFilter::MatchesObject
( const CMenuEngObject& aObject ) const
    {
    if ( TMenuSrvTypeFilter::MatchesObject( aObject ) )
        {
        TPtrC val;
        TBool dummy;
        if( aObject.FindAttribute( iAttrName, val, dummy ) )
            {
            return ETrue; // Match.
            }
        }
    return EFalse;
    }    

// ---------------------------------------------------------
// TMenuSrvTypeAttrFilter::MatchesObject
// ---------------------------------------------------------
//
TBool TMenuSrvAttrExistsFilter::MatchesObject
( const CMenuEngObject& aObject ) const
    {
    TPtrC val;
    TBool dummy;
    if( aObject.FindAttribute( iAttrName, val, dummy ) )
        {
        return ETrue; // Match.
        }
    return EFalse;
    }    

// ---------------------------------------------------------
// TMenuSrvHiddenAppFilter::MatchesObject
// ---------------------------------------------------------
//
TBool TMenuSrvHiddenAppFilter::MatchesObject
( const CMenuEngObject& aObject ) const
    {
    if (KMenuTypeApp() == aObject.Type())
    	{
    	if( aObject.Flags() & TMenuItem::EHidden )
    		{
	    	return ETrue; // Match.
    		}
    	}
    return EFalse;
    } 

// ---------------------------------------------------------
// CMenuSrvEngUtils::IsMiddlet
// ---------------------------------------------------------
//
TBool CMenuSrvEngUtils::IsMiddlet( const TApaAppInfo& aInfo )
	{
	TBool ret( EFalse ); 
	if( aInfo.iFullName.Right( KMidletPostfix().Length() ).
		CompareF( KMidletPostfix ) == 0 )
		{
		ret = ETrue;
		}
	return ret;
	}

