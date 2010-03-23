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

#include <cadefs.h>

#include "caobjectadapter.h"
#include "caentry.h"
#include "caicondescription.h"


//----------------------------------------------------------------------------
//
//----------------------------------------------------------------------------
QPixmap CaObjectAdapter::makeIcon(const CaEntry &entry, const QSize &size)
{
    QPixmap icon;
    if (!entry.iconDescription().filename().isEmpty()) {
        icon = QPixmap(entry.iconDescription().filename()).scaled(size);
    }
    if (icon.isNull()) {
        if (entry.role() == GroupEntryRole) {
            icon = QPixmap("./resource/folder.png").scaled(size);
        }
        else {
            icon = QPixmap("./resource/application.png").scaled(size);
        }
    }
    return icon;
}

//----------------------------------------------------------------------------
//
//----------------------------------------------------------------------------
void CaObjectAdapter::setId(CaEntry &entry,
    int id)
{
    entry.setId(id);
}

//----------------------------------------------------------------------------
//
//----------------------------------------------------------------------------
void CaObjectAdapter::setId(CaIconDescription &iconDescription,
    int id)
{
    iconDescription.setId(id);
}

