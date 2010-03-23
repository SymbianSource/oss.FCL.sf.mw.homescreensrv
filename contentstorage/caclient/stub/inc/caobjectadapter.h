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

#ifndef OBJECTADAPTER_H
#define OBJECTADAPTER_H

// INCLUDES
#include <QPixmap>

class QSize;
class CaEntry;
class CaIconDescription;
class HbIcon;

/**
 *  CaObjectAdapter
 *
 */
class CaObjectAdapter
{
public:

    /**
     * Create an icon.
     * @param entry entry.
     * @param size size of the icon
     * @return QPixmap.
     */
    static HbIcon makeIcon(
        const CaEntry &entry,
        const QSize &size);

    /**
     * Set entry id.
     * @param entry entry.
     * @param id entry id.
     */
    static void setId(CaEntry &entry,
                      int id);

    /**
     * Set icon description id.
     * @param iconDescription icon description.
     * @param id icon description id.
     */
    static void setId(CaIconDescription &iconDescription,
                      int id);

};

#endif // OBJECTADAPTER_H
