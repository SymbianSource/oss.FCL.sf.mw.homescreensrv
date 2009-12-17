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

#ifndef __MENU_LINK_HANDLER_H__
#define __MENU_LINK_HANDLER_H__

#include <e32base.h>
#include "mcsmenuhandlerplugin.h"

/**
* Handler for menu items of type "menu:link".
*/
NONSHARABLE_CLASS( CMenuLinkHandler ): public CMenuHandlerPlugin
    {

public:     // construction

    /**
    * Destructor.
    */
    virtual ~CMenuLinkHandler();

    /**
    * Two-phased constructor. Leaves on failure.
    * @param aMenu Menu.
    * @return The constructed object.
    */
    static CMenuLinkHandler* NewL( RMenu &aMenu );

protected:  // construction

    /**
    * Constructor.
    * @param aMenu Menu.
    */
    CMenuLinkHandler( RMenu &aMenu );

    /**
    * Second-phase constructor.
    */
    void ConstructL();

public:     // from CMenuHandler

    /**
    * Query support for item type.
    * @param aType Type.
    * @return ETrue if handler supports this type, EFalse otherwise.
    */
    TBool SupportsType( const TDesC& aType );

    /**
    * Handle command.
    * @param aItem Item of supported type.
    * @param aCommand Command.
    * @param aParams. Command parameters.
    * @param aStatus Observer request status. When the operation completes,
    * this status will be completed with the resulting error code.
    * @return Asynchronous operation. Owned by the caller.
    */
    CMenuOperation* HandleCommandL(
        CMenuItem& aItem,
        const TDesC8& aCommand,
        const TDesC8& aParams,
        TRequestStatus& aStatus );

private:    // data
    
    };

#endif // __MENU_LINK_HANDLER_H__
