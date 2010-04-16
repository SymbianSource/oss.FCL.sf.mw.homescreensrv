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

#ifndef BITMAPADAPTER_H
#define BITMAPADAPTER_H

// INCLUDES

// CLASS DECLARATION
#include <QList>

class CFbsBitmap;

/**
 *  CaBitmapAdapter
 *
 */
class CaBitmapAdapter
{
public:

    //Static convertion methods between Symbian and Qt objects

    /**
     * Make bitmap copy, used to get rid of compression for extended bitmap
     * @param input input bitmap.
     * @return copied bitmap.
     */
    static CFbsBitmap *copyBitmapLC(CFbsBitmap *input);
    
    /**
     * Convert bitmap
     * @param input bitmap.
     * @return qPixmap.
     */
    static QPixmap fromSymbianCFbsBitmap(CFbsBitmap *aBitmap);
    
    /**
     * Convert from bitmap and mask to qpixmap
     * @param input bitmap.
     * @param pixmap to prepare.
     * @return void.
     */
    static void fromBitmapAndMaskToPixmapL(CFbsBitmap* fbsBitmap, CFbsBitmap* fbsMask, QPixmap& pixmap);

};

#endif // BITMAPADAPTER_H
