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
 * Description:  The API supports requesting raw bitmaps
 *
 */

// INCLUDE FILES
#include <e32base.h>
#include <AknsUtils.h>
#include <SATDomainPSKeys.h>
#include <e32property.h>
#include <apgcli.h>
#include <AknInternalIconUtils.h>
#include "cadef.h"
#include "cautils.h"
#include "cainnerentry.h"
#include "camenuiconutility.h"
#include "caarraycleanup.inl"
#include "casathandler.h"
// Constants
_LIT( KMifIconPath, "\\resource\\apps\\" );
const TUint32 KMifIconPathLenght = 18;

// ================= LOCAL FUNCTIONS ========================


// ---------------------------------------------------------
// ParseIconFilePathL
// ---------------------------------------------------------
//

LOCAL_C void ParseIconFilePathL( const TDesC& aIconFilePath,
        RBuf& aIconFilePathBuf )
    {
    TParsePtrC fileParsePtrC( aIconFilePath );
    if( !fileParsePtrC.PathPresent() )
        {
        aIconFilePathBuf.CreateL( KMifIconPathLenght
                + aIconFilePath.Length() );
        aIconFilePathBuf.Append( KMifIconPath );
        }
    else
        {
        aIconFilePathBuf.CreateL( aIconFilePath.Length() );
        }
    aIconFilePathBuf.Append( aIconFilePath );
    }

// ---------------------------------------------------------
// LoadSkinL
// ---------------------------------------------------------
//
LOCAL_C CAknIcon* GetSkinL( TInt majorId, TInt minorId )
    {
    CAknIcon* icon = NULL;
    if( majorId >= 0 && minorId >= 0 )
        {
        CFbsBitmap* bitmap( 0 );
        CFbsBitmap* mask( 0 );

        TAknsItemID skinId;
        skinId.Set( majorId, minorId );

        MAknsSkinInstance* skinInstance = AknsUtils::SkinInstance();
        if( skinInstance )
            {
            AknsUtils::CreateIconLC( skinInstance, skinId, bitmap, mask,
                    KNullDesC, KErrNotFound, KErrNotFound );
            icon = CAknIcon::NewL();
            icon->SetBitmap( bitmap );
            icon->SetMask( mask );
            }

        // bitmap and icon, AknsUtils::CreateIconLC doesn't specify the order
        CleanupStack::Pop( 2 );
        }

    return icon;
    }

// ---------------------------------------------------------
// GetAppSkinL
// ---------------------------------------------------------
//
LOCAL_C CAknIcon* GetAppSkinL( TUid& aAppUid )
    {
    CAknIcon* icon = NULL;
    CFbsBitmap* bitmap( 0 );
    CFbsBitmap* mask( 0 );

    MAknsSkinInstance* skin = AknsUtils::SkinInstance();
    if( skin )
        {
        TRAPD( err,
                    {
                    AknsUtils::CreateAppIconLC( skin, aAppUid,
                            EAknsAppIconTypeList, bitmap, mask );
                    CleanupStack::Pop(2); //for trap
                    }
        );
        if( err == KErrNone )
            {
            icon = CAknIcon::NewL();
            icon->SetBitmap( bitmap );
            icon->SetMask( mask );
            }
        }
    return icon;
    }

// ---------------------------------------------------------
// GetIconL
// ---------------------------------------------------------
//
LOCAL_C CAknIcon* GetIconL( TFileName aFilename, int aBitmapId, int aMaskId )
    {
    CAknIcon* icon = NULL;
    if( aFilename.Length() > 0 && aBitmapId >= 0 && aMaskId >= 0 )
        {
        CFbsBitmap* bitmap( 0 );
        CFbsBitmap* mask( 0 );

        RBuf pathBuf;
        pathBuf.CleanupClosePushL();
        ParseIconFilePathL( aFilename, pathBuf );

        AknIconUtils::CreateIconLC( bitmap, mask, pathBuf, aBitmapId,
                aMaskId );
        icon = CAknIcon::NewL();
        icon->SetBitmap( bitmap );
        icon->SetMask( mask );
        CleanupStack::Pop( 2 );
        CleanupStack::PopAndDestroy( &pathBuf );
        }
    return icon;
    }

// ---------------------------------------------------------
// GetDefaultSkinL
// ---------------------------------------------------------
//
LOCAL_C CAknIcon* GetDefaultSkinL( const CCaInnerEntry &aItem )
    {
    CAknIcon* icon = NULL;
    if( aItem.GetEntryTypeName().Compare( KCaTypeApp ) == KErrNone )
        {
        TUid appUid( TUid::Uid( aItem.GetUid() ) );
        if( appUid == KSatUid )
            {
            CCaSatHandler* satHandler = CCaSatHandler::NewL();
            CleanupStack::PushL( satHandler );
            icon = satHandler->LoadIconL();
            CleanupStack::PopAndDestroy( satHandler );
            }
        if( !icon )
            {
            icon = GetAppSkinL( appUid );
            }
        if( !icon )
            {
            icon = GetSkinL( KAknsIIDQgnMenuUnknownLst.iMajor,
                    KAknsIIDQgnMenuUnknownLst.iMinor );
            }
        }
    else if( aItem.GetEntryTypeName().Compare( KCaTypeFolder ) == KErrNone
            || aItem.GetEntryTypeName().Compare( KCaTypeCollection )
                    == KErrNone || aItem.GetEntryTypeName().Compare(
            KCaTypeCollectionDownload ) == KErrNone )
        {
        icon = GetSkinL( KAknsIIDQgnPropFolderAppsMedium.iMajor,
                KAknsIIDQgnPropFolderAppsMedium.iMinor );
        }
    else if( aItem.GetEntryTypeName().Compare( KCaTypeUrl ) == KErrNone )
        {
        icon = GetSkinL( KAknsIIDQgnPropPbCommUrl.iMajor,
                KAknsIIDQgnPropPbCommUrl.iMinor );
        }
    else if( aItem.GetEntryTypeName().Compare( KCaTypeWidget ) == KErrNone )
        {
        icon = GetSkinL( KAknsIIDQgnIndiBrowserTbFeeds.iMajor,
                KAknsIIDQgnIndiBrowserTbFeeds.iMinor );
        }
    return icon;
    }

// ================= MEMBER FUNCTIONS =======================

// ---------------------------------------------------------
// MenuIconUtility::GetItemIcon
// ---------------------------------------------------------
//
EXPORT_C CAknIcon* CaMenuIconUtility::GetItemIcon(
        const CCaInnerEntry &aEntry )
    {
    CAknIcon* icon = NULL;
    CCaInnerEntry::TIconAttributes iconAttributes( aEntry.GetIcon() );
    // Try to get the skin of the item
    TRAP_IGNORE(icon = GetSkinL( iconAttributes.iSkinMajorId,
                iconAttributes.iSkinMinorId ) );
    if( !icon )
        {
        // Try to get the icon of the item (skin failed)
        TRAP_IGNORE( icon = GetIconL( iconAttributes.iFileName,
                            iconAttributes.iBitmapId,
                            iconAttributes.iMaskId ) );
        if( !icon )
            {
            // Return a default (skin and icon failed)
            TRAP_IGNORE( icon = GetDefaultSkinL( aEntry ) );
            }
        }
    return icon;
    }
