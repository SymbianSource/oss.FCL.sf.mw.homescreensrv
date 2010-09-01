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
#include <javaregistry.h>
#include <javaregistryentry.h>
#include <drmrightsinfo.h>

#include "menusrvengutils.h"
#include "mcsdef.h"
#include "mcssathandler.h"

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
    iAppAttributes.ResetAndDestroy();
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
TInt CMenuSrvEngUtils::GetAppInfoL( const CMenuEngObject& aEngObj, TApaAppInfo& aAppInfo ) const
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
            aAppInfo.iUid = appUid;
            err = GetAppInfoFromArrayL( aAppInfo );
            if( err )
                {
                err = iApaLsSession.GetAppInfo( aAppInfo, appUid );
                }
            }
        }
    return err;
    }

// ---------------------------------------------------------
// CMenuSrvEngUtils::IsNativeL
// ---------------------------------------------------------
//

TInt CMenuSrvEngUtils::IsNativeL( TUid aAppUid, TBool& aIsNative )
	{
    TInt error( KErrNotFound );
    CMenuEngObject::TAppType aType;
    error = GetAppTypeFromArrayL( aAppUid, aType );
    if ( error == KErrNone && aType == CMenuEngObject::ENativeApp )
        {
        aIsNative = ETrue;
        }
    else if( error != KErrNone )
        {
        TApaAppInfo appInfo;
        error = iApaLsSession.GetAppInfo( appInfo, aAppUid );
        if( error == KErrNone )
            {
            error = iApaLsSession.IsProgram( appInfo.iFullName, aIsNative );
            }
        }
    else
        {
        aIsNative = EFalse;
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
        const TUid aAppUid, TDRMProtectionInfo& aProtectionInfo ) const
    {
    TInt err( KErrNone );

    TApaAppInfo info;

    aProtectionInfo = EDRMUnknownProtection;
    CMenuEngObject::TAppType aType;
    err = GetAppTypeFromArrayL( aAppUid, aType );
    if( ( !err && aType == CMenuEngObject::EJavaApp )
            || ( err && IsMiddlet( aAppUid ) ) )
        {
        TBuf<KMaxFileName> contentId;
        GetJavaContentIdL( aAppUid, contentId );
        if ( contentId != KNullDesC )
            {
            GetDrmProtectionByContentIdL( contentId, aProtectionInfo );
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
// CMenuSrvEngUtils::IsMiddlet
// ---------------------------------------------------------
//
TBool CMenuSrvEngUtils::IsMiddlet( const TUid aAppUid ) const
	{
    const TUid KMidletType = { 0x10210E26 };
    TUid typeuid = KNullUid;

    if ( KErrNone == iApaLsSession.GetAppType( typeuid, aAppUid  ) )
        {
        if ( typeuid == KMidletType )
            {
            return ETrue;
            }
        }
    return EFalse;
	}

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
    attrExists = aEngObj.FindAttribute( KMenuAttrUid, attrVal, localized );
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
// CMenuSrvEngUtils::GetJavaContentIdL
// ---------------------------------------------------------
//
void CMenuSrvEngUtils::GetJavaContentIdL( TUid aUid, TDes& aContentId )  const
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

// ---------------------------------------------------------
// CMenuSrvAppScanner::GetApaItemsL
// ---------------------------------------------------------
//
void CMenuSrvEngUtils::ReloadApaItemsL()
    {
    iAppAttributes.ResetAndDestroy();
    TApaAppInfo appInfo;
    TApaAppCapabilityBuf appCap;

    User::LeaveIfError( iApaLsSession.GetAllApps( 0 ) );
    // for every application get uid, hidden and missing attribute
    // and add to aArray.
    while ( KErrNone == iApaLsSession.GetNextApp( appInfo ) )
        {
        User::LeaveIfError(
                iApaLsSession.GetAppCapability( appCap, appInfo.iUid ) );
        // "Hidden" status according to AppArc.
        TBool appHidden = appCap().iAppIsHidden;

        CMenuSrvAppAttributes* appAtributes = CMenuSrvAppAttributes::NewLC( appCap, appInfo );
        if( IsMiddlet( appInfo.iUid ) )
            {
            appAtributes->SetAppType( CMenuEngObject::EJavaApp );
            }
        else if( iWidgetSession.IsWidget( appInfo.iUid ) )
            {
            appAtributes->SetAppType( CMenuEngObject::EWidgetApp );
            }
        else
            {
            //TODO: it is really necessary?
            TBool isNative( EFalse );
            iApaLsSession.IsProgram( appInfo.iFullName, isNative );
            if( isNative )
                {
                appAtributes->SetAppType( CMenuEngObject::ENativeApp );
                }
            }

        iAppAttributes.AppendL( appAtributes );
        CleanupStack::Pop( appAtributes );
        }
    }

// ---------------------------------------------------------
//
// ---------------------------------------------------------
//
TInt CMenuSrvEngUtils::GetAppInfoFromArrayL( TApaAppInfo& aAppInfo ) const
    {
    TApaAppCapabilityBuf capability;
    CMenuSrvAppAttributes* attribute = CMenuSrvAppAttributes::NewLC( capability, aAppInfo );
    TInt index = iAppAttributes.Find( attribute,
            TIdentityRelation<CMenuSrvAppAttributes>( CMenuSrvAppAttributes::MatchItems ) );
    if( index >= 0 )
        {
        aAppInfo.iCaption = iAppAttributes[index]->GetCaption();
        aAppInfo.iFullName = iAppAttributes[index]->GetFullName();
        aAppInfo.iShortCaption = iAppAttributes[index]->GetShortCaption();
        index = KErrNone;
        }
    CleanupStack::PopAndDestroy( attribute );
    return index;
    }

// ---------------------------------------------------------
//
// ---------------------------------------------------------
//
TInt CMenuSrvEngUtils::GetAppTypeFromArrayL( const TUid& aAppUid,
        CMenuEngObject::TAppType& aType  ) const
    {
    TApaAppCapabilityBuf capability;
    TApaAppInfo appInfo;
    appInfo.iUid = aAppUid;
    CMenuSrvAppAttributes* attribute = CMenuSrvAppAttributes::NewLC( capability, appInfo );
    TInt index = iAppAttributes.Find( attribute,
            TIdentityRelation<CMenuSrvAppAttributes>( CMenuSrvAppAttributes::MatchItems ) );
    if( index >= 0 )
        {
        aType = iAppAttributes[index]->GetAppType();
        index = KErrNone;
        }

    CleanupStack::PopAndDestroy( attribute );
    return index;
    }

// ---------------------------------------------------------
//
// ---------------------------------------------------------
//
RPointerArray<CMenuSrvAppAttributes>& CMenuSrvEngUtils::GetAppItemsL()
    {
    return iAppAttributes;
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
