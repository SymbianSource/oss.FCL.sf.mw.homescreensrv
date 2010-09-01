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

#ifndef __MENUURLHANDLER_H__
#define __MENUURLHANDLER_H__

#include "mcsmenuhandlerplugin.h"

#include <e32base.h>

// Constants
const TUid KUidBrowser = { 0x10008D39 };
_LIT( KBrowserPrefix, "4 " );

/**
* Handler for menu items of type "menu:app".
*/
NONSHARABLE_CLASS( CMenuUrlHandler ): public CMenuHandlerPlugin
    {

public:     // construction

    /**
    * Destructor.
    */
    virtual ~CMenuUrlHandler();

    /**
    * Two-phased constructor. Leaves on failure.
    * @param aMenu Menu.
    * @return The constructed object.
    */
    static CMenuUrlHandler* NewL( RMenu &aMenu );

protected:  // construction

    /**
    * Constructor.
    * @param aMenu Menu.
    */
    CMenuUrlHandler( RMenu &aMenu );

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
        
private:    // internal

    void LaunchUrlL( const TDesC& aUrl );

private:    // data
    
    };

#endif // __MENUURLHANDLER_H__
