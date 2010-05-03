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
#include <e32base.h>
#include <QBitmap>
#include <QDebug>
#include <AknIconUtils.h> // avkon
#include <APGICNFL.H> // fbsbitmap

#include "cabitmapadapter.h"
#include "camenuiconmifutility.h"


void CaMenuIconMifUtility::GetPixmapByFilenameL(TDesC& fileName,
        const QSize &size, QPixmap& pixmap)
{
    CFbsBitmap *bitamp(0);
    CFbsBitmap *mask(0);
    
    if ( AknIconUtils::IsMifFile(fileName)) {
        // SVG icon
        // SVG always has only one icon
        TInt bitmapIndex = 0;
        TInt maskIndex = 1;
        AknIconUtils::ValidateLogicalAppIconId( fileName,
                bitmapIndex, maskIndex );

        AknIconUtils::CreateIconLC( bitamp, mask, fileName,
                bitmapIndex, maskIndex );
        }

    AknIconUtils::DisableCompression(bitamp);
    AknIconUtils::SetSize(bitamp, TSize(size.width(), size.height()),
                    EAspectRatioPreservedAndUnusedSpaceRemoved);
    
    AknIconUtils::DisableCompression(mask);
    AknIconUtils::SetSize(mask, TSize(size.width(), size.height()),
                    EAspectRatioPreservedAndUnusedSpaceRemoved);
    
    CaBitmapAdapter::fromBitmapAndMaskToPixmapL(bitamp, mask, pixmap);
    
    // bitmap and icon, AknsUtils::CreateIconLC doesn't specify the order
    CleanupStack::Pop(2);
}
