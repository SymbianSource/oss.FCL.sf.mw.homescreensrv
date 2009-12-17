/*
* Copyright (c) 2007 Nokia Corporation and/or its subsidiary(-ies).
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
* Description:  Definition of menu handler plugin interface
*
*/


#ifndef __MCSMENUHANDLERPLUGIN_H__
#define __MCSMENUHANDLERPLUGIN_H__

#include "mcsmenuhandler.h"
#include <e32base.h>
#include <ecom/ecom.h>

/**
 *  Menu handler plugin interface.
 *  @lib mcsmenu.lib
 *  @since S60 v5.0
 */
class CMenuHandlerPlugin : public CMenuHandler
    {

public:         

    /**
    * Instantiate Menu Handler plugin.
    * @since S60 v5.0
    * @param aImplementationUid Implementation UID.
    * @param aMenu Menu.
    * @return The created plugin.
    * @capability None.
    * @throws System-wide error codes if an error occurs.
    * @panic None.
    */
    inline static CMenuHandlerPlugin* NewL
        ( TUid aImplementationUid, RMenu& aMenu );

    /**
    * Constructor.
    * @since S60 v5.0
    * @param aMenu Menu.
    * @capability None.
    * @throws None.
    * @panic None.
    */
    inline CMenuHandlerPlugin( RMenu &aMenu );

    /**
    * Destructor.
    * @since S60 v5.0
    * @capability None.
    * @throws None.
    * @panic None.
    */
    inline virtual ~CMenuHandlerPlugin();

private:

    TUid iDtor_ID_Key; ///< Destructor ID key.

    };

#include "mcsmenuhandlerplugin.inl"

#endif // __MCSMENUHANDLERPLUGIN_H__
