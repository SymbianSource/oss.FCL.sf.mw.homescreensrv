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

void CaBitmapAdapter::fromBitmapAndMaskToPixmapL(CFbsBitmap* fbsBitmap,
        CFbsBitmap* fbsMask, QPixmap& pixmap)
{
    pixmap = pixmap.fromSymbianCFbsBitmap(fbsBitmap);
    QPixmap mask;
    mask = mask.fromSymbianCFbsBitmap(fbsMask);
    pixmap.setAlphaChannel(mask);
}
