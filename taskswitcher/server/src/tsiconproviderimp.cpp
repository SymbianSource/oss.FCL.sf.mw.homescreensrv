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
* Description:  Task list entry
*
*/
#include <fbs.h>
#include <apgcli.h>
#include <apgicnfl.h>
#ifdef SYMBIAN_ENABLE_SPLIT_HEADERS
#include <apaidpartner.h>
#else
#include <apaid.h>
#endif

#include <QPixmap>
#include <HbIcon>
#include <XQConversions>


#include "tsiconproviderimp.h"

// size for the created app icons
const TInt KAppIconWidth(128);
const TInt KAppIconHeight(128);
const TInt KImgExtLenght(3);
_LIT(KMbmExt, "mbm");

//------------------------------------------------------------------------------
CTsIconProvider* CTsIconProvider::NewL(MTsResourceManager& aResources)
    {
    CTsIconProvider* self = new(ELeave)CTsIconProvider(aResources);
    CleanupStack::PushL( self );
    self->ConstructL();
    CleanupStack::Pop( self );
    return self;
    }

//------------------------------------------------------------------------------
CTsIconProvider::CTsIconProvider(MTsResourceManager& aResources)
:
    iResources(aResources)
    {
    //No implementation required
    }

//------------------------------------------------------------------------------
void CTsIconProvider::ConstructL()
    {
    QT_TRYCATCH_LEAVING(
        iDefaultIcon = Bitmap( HbIcon("qtg_large_application").qicon() );)
    User::LeaveIfNull(iDefaultIcon);
    }

//------------------------------------------------------------------------------
CTsIconProvider::~CTsIconProvider()
    {
    delete iDefaultIcon;
    }

//------------------------------------------------------------------------------
void CTsIconProvider::LoadIconL(CFbsBitmap& aDst, TUid aUid)
    {
    const TIconSource sources [] = {ESkin, EApparc, EFile, EDefault};
    const TInt count ( sizeof(sources) / sizeof(TIconSource) );
    if(0 == count)
        {
        User::Leave(KErrCorrupt);
        }
    TInt errNo(KErrNotFound);
    for(TInt offset(0); KErrNone != errNo && offset < count; ++offset)
        {
        TRAP( errNo, LoadL( sources[offset], aDst, aUid ) );
        }
    User::LeaveIfError(errNo);
    }

//------------------------------------------------------------------------------
CFbsBitmap* CTsIconProvider::Bitmap( const QIcon& aIcon )
    {
    CFbsBitmap* retValue(0);
    QPixmap pixmap = aIcon.pixmap( QSize(KAppIconWidth, KAppIconHeight) );
    if( !pixmap.isNull() )
        {
        retValue = pixmap.toSymbianCFbsBitmap();
        }
    return retValue;
    }
//------------------------------------------------------------------------------
CFbsBitmap* CTsIconProvider::Bitmap( CFbsBitmap& aBitmap, 
                                     CFbsBitmap& aMask )
    {
    QPixmap ico(QPixmap::fromSymbianCFbsBitmap(&aBitmap));
    ico.setAlphaChannel(QPixmap::fromSymbianCFbsBitmap(&aMask));
    return ico.scaled(QSize(KAppIconWidth, KAppIconHeight),
                      Qt::KeepAspectRatioByExpanding,
                      Qt::SmoothTransformation).toSymbianCFbsBitmap();
    
    }

//------------------------------------------------------------------------------
void CTsIconProvider::LoadL(TIconSource aSrc, CFbsBitmap& aDst, TUid aUid)
    {
    switch(aSrc)
        {
        case ESkin:LoadIconFromSkinL(aDst, aUid); break;
        case EApparc:LoadIconFromApparcL(aDst, aUid); break;
        case EFile: LoadIconFromFileL(aDst, aUid); break;
        case EDefault: LoadIconDefaultL(aDst); break;
        }
    }

//------------------------------------------------------------------------------
void CTsIconProvider::LoadIconFromSkinL(CFbsBitmap& aDst, TUid aUid)
    {
    CApaAppServiceInfoArray* skin =
        iResources.ApaSession().GetAppServiceOpaqueDataLC(aUid, TUid::Uid(0x2002DCF3));
    TArray<TApaAppServiceInfo> info( skin->Array() );
    if( 0 >= info.Count() )
        {
        User::Leave(KErrNotFound);
        }
    TPtrC8 opaque(info[0].OpaqueData());
    const TPtrC16 iconName((TText16*) opaque.Ptr(),(opaque.Length()+1)>>1);
    
    CFbsBitmap* ico(0);
    QT_TRYCATCH_LEAVING( 
    ico = Bitmap(HbIcon(XQConversions:: s60DescToQString(iconName)).qicon());)
    CopyAndDestroyL(aDst, ico);
    
    CleanupStack::PopAndDestroy(skin);
    }

//------------------------------------------------------------------------------
void CTsIconProvider::LoadIconFromApparcL(CFbsBitmap& aDst, TUid aUid)
    {
    const TSize size(KAppIconWidth, KAppIconHeight);
    CApaMaskedBitmap* bitmap = CApaMaskedBitmap::NewLC();
    User::LeaveIfError( iResources.ApaSession().GetAppIcon(aUid,size,*bitmap) );
    User::LeaveIfNull(bitmap->Mask());
    TInt iconsCount(0);
    iResources.ApaSession().NumberOfOwnDefinedIcons( aUid, iconsCount);
    if(0 >= iconsCount)
        {
        User::Leave(KErrNotFound);
        }
    
    CFbsBitmap *ico(0);
    QT_TRYCATCH_LEAVING(
        ico = Bitmap(*bitmap, *bitmap->Mask());)
    CopyAndDestroyL(aDst, ico);
    
    CleanupStack::PopAndDestroy(bitmap);
    }

//------------------------------------------------------------------------------
void CTsIconProvider::LoadIconFromFileL(CFbsBitmap& aDst, TUid aUid)
    {
    HBufC* fileName( 0 );
    User::LeaveIfError(iResources.ApaSession().GetAppIcon(aUid,fileName));
    CleanupStack::PushL(fileName);
    fileName->Des().LowerCase();
    if(0 == fileName->Right(KImgExtLenght).Compare(KMbmExt))
        {
        User::LeaveIfError( aDst.Load(*fileName) );
        }
    else
        {
        CFbsBitmap* ico(0);
        QT_TRYCATCH_LEAVING(
            ico = Bitmap(QIcon(XQConversions::s60DescToQString(*fileName)));)
        CopyAndDestroyL(aDst, ico);
        }
        
    CleanupStack::PopAndDestroy(fileName);
    }

//------------------------------------------------------------------------------
void CTsIconProvider::LoadIconDefaultL(CFbsBitmap& aDst)
    {
    User::LeaveIfError( aDst.Duplicate( iDefaultIcon->Handle() ) );
    }

//------------------------------------------------------------------------------
void CTsIconProvider::CopyAndDestroyL(CFbsBitmap& aDst, CFbsBitmap* aSrc)
    {
    User::LeaveIfNull(aSrc);
    CleanupStack::PushL(aSrc);
    User::LeaveIfError(aDst.Duplicate(aSrc->Handle()));
    CleanupStack::PopAndDestroy(aSrc);
    }
