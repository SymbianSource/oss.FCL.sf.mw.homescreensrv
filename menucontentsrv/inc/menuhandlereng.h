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
* Description:  Definition of menu handler engine
*
*/


#ifndef __MENUHANDLERENG_H__
#define __MENUHANDLERENG_H__

#include <e32base.h>

class RMenu;
class CMenuHandler;
class CMenuOperation;
class CMenuItem;

/**
 *  Menu handler engine.
 *  Handler related menu functionality and objects (factory, handler list, ...)
 *  are collected in this class, to keep RMenu implementation and API clean.
 *  @lib mcsmenu.lib
 *  @since S60 v5.0
 */
NONSHARABLE_CLASS( CMenuHandlerEng ): public CBase
    {

public:

    /**
    * Destructor.
    */
    virtual ~CMenuHandlerEng();

    /**
    * Two-phased constructor.
    * @param aMenu Menu.
    * @return The created object.
    */
    static CMenuHandlerEng* NewL( RMenu& aMenu );

    /**
    * Handle command.
    * @param aItem Item.
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

protected:

    /**
    * Constructor.
    * @param aMenu Menu.
    */
    CMenuHandlerEng( RMenu& aMenu );

private:

    /**
    * Find loaded handler by type.
    * @param aType Type.
    * @return Loaded handler or NULL no handler is loaded for that type.
    */
    CMenuHandler* FindHandler( const TDesC& aType );

    /**
    * Load handler for given type.
    * @param aType Type.
    * @return Loaded handler or NULL no handler is found for that type.
    */
    CMenuHandler* LoadHandlerL( const TDesC& aType );

private:    // data

    RMenu& iMenu; ///<Menu.
    RPointerArray<CMenuHandler> iHandlers; ///< Own.

    };

#endif // __MENUHANDLERENG_H__
