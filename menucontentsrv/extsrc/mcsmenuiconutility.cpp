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
*  Version     :  %version: ou1s60ui#12 % << Don't touch! Updated by Synergy at check-out.
*
*/


// INCLUDE FILES

#include <e32base.h>
#include <AknsUtils.h> 
#include <mcsmenu.h>
#include <mcsmenuitem.h>
#include <mcsmenuutils.h>
#include <satdomainpskeys.h>
#include <e32property.h>
#include <apgcli.h>
#include <AknInternalIconUtils.h>

#include "mcssathandler.h"

//#include "mcsextendedmenuitem.h"
#include "mcsmenuiconutility.h"
#include <mcsdef.h>

// Constants
_LIT( KMifIconPath, "\\resource\\apps\\" );
const TUint32 KMifIconPathLenght = 18;
const TInt KJavaIconWidth = 88;			// Java icon default width
const TInt KJavaIconHeight = 88;		// Java icon default height



// ================= LOCAL FUNCTIONS ========================


// -----------------------------------------------------------------------------
// CreateMbmIconLC2 - Creates bitmap
// -----------------------------------------------------------------------------
//    
LOCAL_C TBool CreateMbmIconLC2(CFbsBitmap*& aBitmap, CFbsBitmap*& aMask, const TDesC& aAppUid ) 
    {
    TBool ret = EFalse;
    TSize size( KJavaIconWidth, KJavaIconHeight );

    aBitmap = NULL;
    aMask = NULL;
    
    RApaLsSession lsSession;
    User::LeaveIfError( lsSession.Connect() );
    CleanupClosePushL( lsSession ); // lsSession (1)
    
    TUint appUidUint;
    
    // Gets app TUid from aAppUid descryptor
    User::LeaveIfError( MenuUtils::GetTUint( aAppUid, appUidUint ) );
    TUid appUid( TUid::Uid( appUidUint) );
    
    // Tries to find icon with default size.
    CApaMaskedBitmap* apaBmp = CApaMaskedBitmap::NewLC();
    TInt apaErr = lsSession.GetAppIcon( appUid, size, *apaBmp );
    if ( apaErr == KErrNotFound ) 
        {
        // Icon not found.
        // AppArc tries to find equal to or smaller than the specified size.
        // Get all icon sizes and search for next bigger icon size.
        CArrayFixFlat<TSize>* sizes = new(ELeave) CArrayFixFlat<TSize>( 5 );
        apaErr = lsSession.GetAppIconSizes(appUid, *sizes);
        if ( !apaErr )
            {
            if ( sizes->Count() ) 
                {
                // There are other icon sizes
                TInt idx = -1;
                TInt square( size.iWidth * size.iHeight );
                for ( TInt i = 0; i < sizes->Count(); i++ ) 
                    {
                    if ( ( sizes->At( i ).iWidth * sizes->At( i ).iHeight ) > square ) 
                        {
                        idx = i;
                        break;
                        }
                    }
                    
                if ( idx >= 0 ) 
                    {
                    // Next bigger icon.
                    size = sizes->At( idx );
                    }
                else 
                    {
                    // Just get the first available.
                    size = sizes->At( 0 );
                    }
                }
            
            // Gets icon with new size.
            apaErr = lsSession.GetAppIcon( appUid, size, *apaBmp );
            }

        // Reset and destroy.
        sizes->Reset();
        delete sizes;
        sizes = NULL;
        }
        
    // Some builds requires brackets
    // logical correction was also made
    if ( ( apaErr != KErrNone ) &&
         ( apaErr != KErrNotFound ) &&
         ( apaErr != KErrNotSupported ) ) 
        {
        // System wide error.
        User::LeaveIfError( apaErr );
        }
    
    if( !apaErr && 
        apaBmp && 
        apaBmp->Mask() &&
        apaBmp->Handle() && 
        apaBmp->Mask()->Handle()) 
        {
        // AIF-based icon
        CFbsBitmap* iconOwnedBitmap = new (ELeave) CFbsBitmap(); 
        CleanupStack::PushL( iconOwnedBitmap ); // iob (3)
        CFbsBitmap* iconOwnedMask = new (ELeave) CFbsBitmap();
        CleanupStack::PushL( iconOwnedMask ); // iom (4)
        User::LeaveIfError(
            iconOwnedBitmap->Duplicate( apaBmp->Handle() ) );
        User::LeaveIfError(
            iconOwnedMask->Duplicate( apaBmp->Mask()->Handle() ) );
        
        CAknIcon* tmpIcon = CAknIcon::NewL();

        // Ownership is transferred
        tmpIcon->SetBitmap( iconOwnedBitmap );
        tmpIcon->SetMask( iconOwnedMask );
        // Ownership of tmpIcon is transferred
        CAknIcon* appIcon = AknIconUtils::CreateIconL( tmpIcon );
        CleanupStack::Pop( 2 ); // iom, iob (2)

        aBitmap = appIcon->Bitmap();
        aMask = appIcon->Mask();
        AknInternalIconUtils::SetAppIcon(aBitmap);
        
        // Detach and delete
        appIcon->SetBitmap( NULL );
        appIcon->SetMask( NULL );
        delete appIcon;

        ret = ETrue;
        }
        
    CleanupStack::PopAndDestroy( 2 ); // lsSession, apaBmp (0)

    // These are both safe
    if ( aBitmap ) 
        {
        CleanupStack::PushL( aBitmap ); // (1)
        CleanupStack::PushL( aMask ); // (2)
        }
    
    return ret;
    }


// ---------------------------------------------------------
// ParseIconFilePathL
// ---------------------------------------------------------
//

LOCAL_C void ParseIconFilePathL( const TDesC& aIconFilePath, RBuf& aIconFilePathBuf )
	{
	TParsePtrC fileParsePtrC(aIconFilePath);
	if( !fileParsePtrC.PathPresent())
		{
		aIconFilePathBuf.CreateL(KMifIconPathLenght + aIconFilePath.Length());
		aIconFilePathBuf.Append(KMifIconPath);
		}
	else 
	    {
		aIconFilePathBuf.CreateL(aIconFilePath.Length());
	    }
	aIconFilePathBuf.Append(aIconFilePath);
	}

// ---------------------------------------------------------
// LoadSkinL
// ---------------------------------------------------------
//
LOCAL_C CAknIcon* LoadSkinL( TUint majorId, TUint minorId )
    {
    CAknIcon* icon = NULL;
    CFbsBitmap* bitmap( 0 );
    CFbsBitmap* mask( 0 );

    TAknsItemID skinId;
    skinId.Set( majorId, minorId );

    AknsUtils::CreateIconLC(
        AknsUtils::SkinInstance(),
        skinId,
        bitmap,
        mask,
        KNullDesC,
        KErrNotFound,
        KErrNotFound );

    icon = CAknIcon::NewL();
    icon->SetBitmap( bitmap );
    icon->SetMask( mask );

     // bitmap and icon, AknsUtils::CreateIconLC doesn't specify the order
    CleanupStack::Pop( 2 );

    return icon;
    }

// ---------------------------------------------------------
// LoadSkinL
// ---------------------------------------------------------
//
LOCAL_C CAknIcon* LoadAppSkinL( TUid& aAppUid )
    {
    CAknIcon* icon = NULL;
    CFbsBitmap* bitmap( 0 );
    CFbsBitmap* mask( 0 );
    
    MAknsSkinInstance* skin = AknsUtils::SkinInstance();
    
    TRAPD( err,  
        { 
        AknsUtils::CreateAppIconLC( skin, aAppUid, EAknsAppIconTypeList, bitmap, mask );            
        CleanupStack::Pop(2); //for trap
       }
        );
    if( err == KErrNone )
        {
        icon = CAknIcon::NewL();
        icon->SetBitmap( bitmap );
        icon->SetMask( mask );
        }
    return icon;
    }

// ---------------------------------------------------------
// GetSkinL
// ---------------------------------------------------------
//
LOCAL_C CAknIcon* GetSkinL( CMenuItem& aItem )
	{
	CAknIcon* icon = NULL;
	TUint majorId( 0 );
    TUint minorId( 0 );
    TPtrC idString( KNullDesC );
    TBool attrExists;
    
	idString.Set( aItem.GetAttributeL( KMenuAttrIconSkinMajorId, attrExists ) );
	if ( attrExists && idString.Length() )
		{
		User::LeaveIfError( MenuUtils::GetTUint( idString, majorId ) );
		}
	else
		{
		return NULL;
		}

	idString.Set( aItem.GetAttributeL( KMenuAttrIconSkinMinorId, attrExists ) );
	if ( attrExists && idString.Length() )
		{
		User::LeaveIfError( MenuUtils::GetTUint( idString, minorId ) );
		}
	else
		{
		return NULL;
		}

	icon = LoadSkinL( majorId, minorId );
	
	return icon;
	}

// ---------------------------------------------------------
// GetIconL
// ---------------------------------------------------------
//
LOCAL_C CAknIcon* GetIconL( CMenuItem& aItem )
	{
	CAknIcon* icon = NULL;
    TUint iconId( 0 );
    TUint maskId( 0 );
	TPtrC attrStr( KNullDesC );
	TBool attrExists;

	TPtrC iconFilePath( aItem.GetAttributeL( KMenuAttrIconFile, attrExists ) );
	
	if ( !attrExists )
		{
		return NULL;
		}
	
	attrStr.Set( aItem.GetAttributeL( KMenuAttrIconId, attrExists ) );
	if ( attrExists )
		{
		User::LeaveIfError( MenuUtils::GetTUint( attrStr, iconId ) );
		}
	else
		{
		return NULL;
		}

	attrStr.Set( aItem.GetAttributeL( KMenuAttrMaskId, attrExists ) );
	if ( attrExists )
		{
		User::LeaveIfError( MenuUtils::GetTUint( attrStr, maskId ) );
		}
	else
		{
		return NULL;
		}

	CFbsBitmap* bitmap( 0 );
	CFbsBitmap* mask( 0 );

	RBuf pathBuf;
	pathBuf.CleanupClosePushL();
	ParseIconFilePathL(iconFilePath, pathBuf);
	
	AknIconUtils::CreateIconLC( bitmap, mask, pathBuf, iconId, maskId ); 
	icon = CAknIcon::NewL();
	icon->SetBitmap( bitmap );
	icon->SetMask( mask );
	CleanupStack::Pop( 2 );
    
    CleanupStack::PopAndDestroy( &pathBuf );

	return icon;
	}




// ---------------------------------------------------------
// GetDefaultSkinL
// ---------------------------------------------------------
//
LOCAL_C CAknIcon* GetDefaultSkinL( CMenuItem& aItem )
	{
	CAknIcon* icon = NULL;

    if ( KErrNone == aItem.Type().Compare( KMenuTypeApp ) )
        {
        TBool attrExists( EFalse );
        TPtrC appUidDes( aItem.GetAttributeL( KMenuAttrUid, attrExists ) );
        if( appUidDes == KMenuSatUiUid )
            {
            CMcsSatHandler* satHandler = CMcsSatHandler::NewL();
            CleanupStack::PushL( satHandler );
            icon = satHandler->LoadIconL();
            CleanupStack::PopAndDestroy( satHandler );
            }
        if( !icon )
            {
            if( aItem.GetAttributeL( KMenuAttrNative, attrExists ) == KMenuFalse )
            	{
				CFbsBitmap* bitmap( 0 );
				CFbsBitmap* mask( 0 );
				//for native
				if( CreateMbmIconLC2( bitmap, mask, appUidDes ) )
					{
					icon = CAknIcon::NewL();
					icon->SetBitmap( bitmap );
					icon->SetMask( mask );
					CleanupStack::Pop( 2 );
					}
            	}
            if( !icon )
                 {
                 TUint appUid;
                 TUid uid;
                 TInt err = MenuUtils::GetTUint( appUidDes, appUid );
                 if ( !err )
                     {
                     uid = TUid::Uid( appUid );
                     icon = LoadAppSkinL( uid );
                     }
                 }
           if( !icon )
                {
                icon = LoadSkinL(
                    KAknsIIDQgnMenuUnknownLst.iMajor,
                    KAknsIIDQgnMenuUnknownLst.iMinor );
                }
            }
        }
    else if ( KErrNone == aItem.Type().Compare( KMenuTypeFolder ) )
        {
        TUint childrenCount;
        TBool attrExists( EFalse );
        TInt err = MenuUtils::GetTUint( 
        	aItem.GetAttributeL( KChildrenCount, attrExists ), childrenCount );
        
        if( childrenCount > 0 )
            {
            icon = LoadSkinL(
                KAknsIIDQgnPropFolderAppsMedium.iMajor,
                KAknsIIDQgnPropFolderAppsMedium.iMinor ); 
            }
        else
            {
            icon = LoadSkinL(
                KAknsIIDQgnMenuFolderEmpty.iMajor,
                KAknsIIDQgnMenuFolderEmpty.iMinor );

            }
               		
        }

    return icon;
	}

// ================= MEMBER FUNCTIONS =======================


// ---------------------------------------------------------
// MenuIconUtility::GetItemIconL
// ---------------------------------------------------------
//
EXPORT_C CAknIcon* MenuIconUtility::GetItemIconL( CMenuItem& aItem )
	{
	CAknIcon* icon = NULL;
	// Try to get the skin of the item
	TRAP_IGNORE(icon = GetSkinL( aItem ));
	if ( icon )
		{
		return icon;
		}

	// Try to get the icon of the item (skin failed)
	TRAP_IGNORE(icon = GetIconL( aItem ));
	if ( icon )
		{
		return icon;
		}

	// Return a default (skin and icon failed)
	icon = GetDefaultSkinL( aItem );
    
	return icon;
	}

