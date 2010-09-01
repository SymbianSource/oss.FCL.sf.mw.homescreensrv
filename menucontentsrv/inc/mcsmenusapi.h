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

#ifndef __MCSMENUSAPI_H__
#define __MCSMENUSAPI_H__

#include <e32base.h>

#include "mcsmenu.h"

/**
 *  Menu session.
 *  The menu is an ID-based tree structure of menu folders and items.
 *  @lib mcsmenu.lib
 *  @since S60 v5.0
 */
NONSHARABLE_CLASS( RMenuSapi ): public RMenu
    {
public:

    /**
    * GetList operation
	* @param aSerializedIn serialized list of input parameters
	* @param aSerializedOut serialized list with getlist output
    */
	IMPORT_C void GetListL( TDes8& aSerializedIn,
		RBuf8& aSerializedOut );


    };

#endif // __MCSMENUSAPI_H__
