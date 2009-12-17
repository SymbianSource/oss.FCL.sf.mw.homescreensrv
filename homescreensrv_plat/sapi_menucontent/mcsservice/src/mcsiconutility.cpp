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

#include <e32base.h>
#include <AknsUtils.h> 
#include <mcsmenu.h>
#include <mcsmenuitem.h>
#include <mcsmenuutils.h>
#include <SATDomainPSKeys.h>
#include <e32property.h>
#include <apgcli.h>
#include <AknInternalIconUtils.h>


#include <liwcommon.h>



//#include "mcsextendedmenuitem.h"
#include "mcsiconutility.h"
#include "mcssathandler.h"
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
LOCAL_C CAknIcon* GetSkinL( const CLiwDefaultMap* aMap )
	{
	CAknIcon* icon = NULL;
	TUint majorId( 0 );
    TUint minorId( 0 );
    TPtrC idString( KNullDesC );
    
    TLiwVariant variant;
    variant.PushL();
	TBuf8<KMenuMaxAttrNameLen> attrName;
	attrName.Copy( KMenuAttrIconSkinMajorId );
    
    if( aMap->FindL( attrName, variant))
    	{
    	variant.Get( idString );
		User::LeaveIfError( MenuUtils::GetTUint( idString, majorId ) );
		attrName.Copy( KMenuAttrIconSkinMinorId );
	    if( aMap->FindL( attrName, variant))
	    	{
	    	variant.Get( idString );
			User::LeaveIfError( MenuUtils::GetTUint( idString, minorId ) );
			icon = LoadSkinL( majorId, minorId );
			}
		}
	CleanupStack::PopAndDestroy( &variant );
	return icon;
	}

// ---------------------------------------------------------
// GetIconL
// ---------------------------------------------------------
//
LOCAL_C CAknIcon* GetIconL( const CLiwDefaultMap* aMap )
	{
	CAknIcon* icon = NULL;
    TUint iconId( 0 );
    TUint maskId( 0 );
	TPtrC attrStr( KNullDesC );
	TPtrC iconFilePath;
			
	TLiwVariant variantIconFile;
	TLiwVariant variantIconId;
	TLiwVariant variantMaskId;
	variantIconFile.PushL();
	variantIconId.PushL();
	variantMaskId.PushL();
	TBuf8<KMenuMaxAttrNameLen> attrName;
	attrName.Copy( KMenuAttrIconFile );
	    
	if( aMap->FindL( attrName, variantIconFile))
		{
		variantIconFile.Get( iconFilePath );
		attrName.Copy( KMenuAttrIconId );
		if( aMap->FindL( attrName, variantIconId) )
			{
			variantIconId.Get( attrStr );
			User::LeaveIfError( MenuUtils::GetTUint( attrStr, iconId ) );
			attrName.Copy( KMenuAttrMaskId );
			if( aMap->FindL( attrName, variantMaskId) )
				{
				variantMaskId.Get( attrStr );
				User::LeaveIfError( MenuUtils::GetTUint( attrStr, maskId ) );
				CFbsBitmap* bitmap( 0 );
				CFbsBitmap* mask( 0 );
				RBuf pathBuf;
				pathBuf.CleanupClosePushL();
				ParseIconFilePathL(iconFilePath, pathBuf);
				AknIconUtils::CreateIconLC( bitmap, mask, 
						pathBuf, iconId, maskId ); 
				icon = CAknIcon::NewL();
				icon->SetBitmap( bitmap );
				icon->SetMask( mask );
				CleanupStack::Pop( 2 );
				CleanupStack::PopAndDestroy( &pathBuf );			
				}
			}
		}
    CleanupStack::PopAndDestroy( &variantMaskId );
    CleanupStack::PopAndDestroy( &variantIconId );
    CleanupStack::PopAndDestroy( &variantIconFile );
	return icon;
	}




// ---------------------------------------------------------
// GetDefaultSkinL
// ---------------------------------------------------------
//
LOCAL_C CAknIcon* GetDefaultSkinL( const CLiwDefaultMap* aMap )
	{
	CAknIcon* icon = NULL;
	TPtrC attrValueUid(KNullDesC);
	TPtrC attrValueNative(KNullDesC);
	TPtrC attrValueChildren(KNullDesC);
	
	TLiwVariant variantUid;
	variantUid.PushL();
	TLiwVariant variantNative;
	variantNative.PushL();
	TLiwVariant variantChildren;
	variantChildren.PushL();
	TBuf8<KMenuMaxAttrNameLen> attrName;
	
	//APPLICATION
	attrName.Copy( KMenuAttrUid );
	if( aMap->FindL( attrName, variantUid))
		{
		variantUid.Get( attrValueUid );
	      
		if( attrValueUid == KMenuSatUiUid )
			{
			CMcsSatHandler* satHandler = CMcsSatHandler::NewL();
			CleanupStack::PushL( satHandler );
			icon = satHandler->LoadIconL();
			CleanupStack::PopAndDestroy( satHandler );
			}
		if( !icon )
			{
			attrName.Copy( KMenuAttrNative );
			if( aMap->FindL( attrName, variantNative) )
				{
				variantNative.Get( attrValueNative );
				if( attrValueNative == KMenuFalse )
					{
					CFbsBitmap* bitmap( 0 );
					CFbsBitmap* mask( 0 );
					//for native
					if( CreateMbmIconLC2( bitmap, mask, attrValueUid ) )
						{
						icon = CAknIcon::NewL();
						icon->SetBitmap( bitmap );
						icon->SetMask( mask );
						CleanupStack::Pop( 2 );
						}
					}
				}
			if( !icon )
				 {
				 TUint appUid;
				 TUid uid;
				 TInt err = MenuUtils::GetTUint( attrValueUid, appUid );
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
    
    //FOLDER
    TUint childrenCount;
   
	attrName.Copy( KChildrenCount );
	if( aMap->FindL( attrName, variantChildren))
		{
		variantChildren.Get( attrValueChildren );
		TInt err = MenuUtils::GetTUint( attrValueChildren, childrenCount );
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
	CleanupStack::PopAndDestroy( &variantChildren );
	CleanupStack::PopAndDestroy( &variantNative );
	CleanupStack::PopAndDestroy( &variantUid );
    return icon;
	}

// ================= MEMBER FUNCTIONS =======================


// ---------------------------------------------------------
// MenuIconUtility::GetItemIconL
// ---------------------------------------------------------
//
CAknIcon* McsIconUtility::GetItemIconL( const CLiwDefaultMap* aMap )
	{
	CAknIcon* icon = NULL;
	
	// Try to get the skin of the item
	TRAP_IGNORE(icon = GetSkinL( aMap ));
	if ( icon )
		{
		return icon;
		}

	// Try to get the icon of the item (skin failed)
	TRAP_IGNORE(icon = GetIconL( aMap ));
	if ( icon )
		{
		return icon;
		}

	// Return a default (skin and icon failed)
	icon = GetDefaultSkinL( aMap );
	return icon;
	}

