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
* Description:  The API supports requesting raw bitmaps
 *
*/


#ifndef __CAMENUICONUTILITY_H__
#define __CAMENUICONUTILITY_H__

#include <akniconutils.h>

class CCaInnerEntry;

/**
 *  Icon utility.
 *  This is a utilitiy class providing method(s) for querying raw bitmap data
 *  belonging to menu items
 *  @lib caextendedmenu.lib
 *  @since S60 v5.0
 */
NONSHARABLE_CLASS( CaMenuIconUtility )
    {
public:
    /**
     * Creates the icon that belongs to a menu item.
     * @since S60 v5.0.
     * @param aEntry entry for which an icon is queried for
     * @return the icon of the specified entry. The ownership of the icon
     * object is transferred to the caller.
     */
    IMPORT_C static CAknIcon* GetItemIcon( const CCaInnerEntry &aEntry );
    };

#endif // __CAMENUICONUTILITY_H__
