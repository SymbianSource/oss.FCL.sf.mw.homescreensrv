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


#ifndef __MCSMENUICONUTILITY_H__
#define __MCSMENUICONUTILITY_H__

#include <AknIconUtils.h>
class CMenuItem;


/**
 *  Icon utility.
 *  This is a utilitiy class providing method(s) for querying raw bitmap data
 *  belonging to menu items 
 *  @lib mcsextendedmenu.lib
 *  @since S60 v5.0
 */
NONSHARABLE_CLASS( MenuIconUtility )
	{
public:

	/**
	* Creates the icon that belongs to a menu item.
	* @since S60 v5.0.
	* @param aItem The menu item the icon is querried for 
	* @return the icon of the specified menu item. The ownership of the icon
	* object is transferred to the caller. 
	*/
	
	IMPORT_C static CAknIcon* GetItemIconL( CMenuItem& aItem );
	};

#endif // __MENUICONUTILITY_H__
