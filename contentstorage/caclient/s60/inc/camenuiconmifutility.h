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

#ifndef MENUICONMIFUTILITY_H
#define MENUICONMIFUTILITY_H

// INCLUDES

// CLASS DECLARATION
#include <cadefs.h>

/**
 *  CaMenuIconMifUtility
 *
 */
class CaMenuIconMifUtility
{
public:
    
    /**
     * Create pixmap from filename
     * @param fileName icon filename.
     * @param icon size.
     * @param pixmap to fill.
     * @return void.
     */
    static void GetPixmapByFilenameL(TDesC& fileName, const QSize &size,
            QPixmap& pixmap);
};

#endif // MENUICONMIFUTILITY_H
