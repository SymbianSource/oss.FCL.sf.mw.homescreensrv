/*
 * Copyright (c)2008 Nokia Corporation and/or its subsidiary(-ies).
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
 * Description:  ?Description
 *
 */

#include <e32std.h>
#include <exception>
#include <e32base.h>
#include <badesca.h>

#include <fbs.h>
#include <bitdev.h>
#include <QPixmap>
#include <HbIcon>
#include <QBitmap>
#include <QDebug>
#include <XQConversions>

#include "cadef.h"
#include "cabitmapadapter.h"

// -----------------------------------------------------------------------------
// copying compressed bitmap
//----------------------------------------------------------------------------
CFbsBitmap *CaBitmapAdapter::copyBitmapLC(CFbsBitmap *input)
{
    CFbsBitmap *bmp = new(ELeave) CFbsBitmap();
    CleanupStack::PushL(bmp);
    bmp->Create(input->SizeInPixels(), input->DisplayMode());

    CFbsBitmapDevice *bitmapDevice = CFbsBitmapDevice::NewL(bmp);
    CleanupStack::PushL(bitmapDevice);
    CFbsBitGc *bmpGc;
    bitmapDevice->CreateContext(bmpGc);
    bmpGc->BitBlt(TPoint(0,0), input);
    delete bmpGc;
    CleanupStack::PopAndDestroy(bitmapDevice);
    return bmp;
}

void CaBitmapAdapter::fromBitmapAndMaskToPixmapL(CFbsBitmap* fbsBitmap,
        CFbsBitmap* fbsMask, QPixmap& pixmap)
{
    if (fbsBitmap->Header().iCompression == ENoBitmapCompression) {
        pixmap = pixmap.fromSymbianCFbsBitmap(fbsBitmap);
        QPixmap mask;
        mask = mask.fromSymbianCFbsBitmap(fbsMask);
        pixmap.setAlphaChannel(mask);
    } else { // we need special handling for icons in 9.2 (NGA)
        // let's hope that in future it will be in QT code
        CFbsBitmap *temp(NULL);
        temp = CaBitmapAdapter::copyBitmapLC(fbsBitmap);
        pixmap = pixmap.fromSymbianCFbsBitmap(temp);
        CleanupStack::PopAndDestroy();
        temp = CaBitmapAdapter::copyBitmapLC(fbsMask);
        QPixmap mask;
        mask = mask.fromSymbianCFbsBitmap(temp);
        CleanupStack::PopAndDestroy();
        pixmap.setAlphaChannel(mask);
    }
}
