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

static QImage::Format TDisplayMode2Format(TDisplayMode mode)
{
    QImage::Format format;
    switch (mode) {
    case EGray2:
        format = QImage::Format_MonoLSB;
        break;
    case EColor256:
    case EGray256:
        format = QImage::Format_Indexed8;
        break;
    case EColor4K:
        format = QImage::Format_RGB444;
        break;
    case EColor64K:
        format = QImage::Format_RGB16;
        break;
    case EColor16M:
        format = QImage::Format_RGB888;
        break;
    case EColor16MU:
        format = QImage::Format_RGB32;
        break;
    case EColor16MA:
        format = QImage::Format_ARGB32;
        break;
    case EColor16MAP:
        format = QImage::Format_ARGB32_Premultiplied;
        break;
    default:
        format = QImage::Format_Invalid;
        break;
    }
    return format;
}

QPixmap CaBitmapAdapter::fromSymbianCFbsBitmap(CFbsBitmap *aBitmap)
{
    aBitmap->BeginDataAccess();
    uchar *data = (uchar *)aBitmap->DataAddress();
    TSize size = aBitmap->SizeInPixels();
    TDisplayMode displayMode = aBitmap->DisplayMode();

    // QImage format must match to bitmap format
    QImage image(data, size.iWidth, size.iHeight, TDisplayMode2Format(
        displayMode));
    aBitmap->EndDataAccess();

    // No data copying happens because
    // image format matches native OpenVG format.
    // So QPixmap actually points to CFbsBitmap data.
    return QPixmap::fromImage(image);
}

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
        pixmap = CaBitmapAdapter::fromSymbianCFbsBitmap(fbsBitmap);
        QPixmap mask = CaBitmapAdapter::fromSymbianCFbsBitmap(fbsMask);
        pixmap.setAlphaChannel(mask);
    } else { // we need special handling for icons in 9.2 (NGA)
        // let's hope that in future it will be in QT code
        CFbsBitmap *temp(NULL);
        temp = CaBitmapAdapter::copyBitmapLC(fbsBitmap);
        pixmap = CaBitmapAdapter::fromSymbianCFbsBitmap(temp);
        CleanupStack::PopAndDestroy();
        temp = CaBitmapAdapter::copyBitmapLC(fbsMask);
        QPixmap mask = CaBitmapAdapter::fromSymbianCFbsBitmap(temp);
        CleanupStack::PopAndDestroy();
        pixmap.setAlphaChannel(mask);
    }
}
