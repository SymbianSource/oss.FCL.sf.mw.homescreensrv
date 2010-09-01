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
* Description:  Definition of menu handler interface
*
*/


#ifndef __MCSMENUHANDLER_H__
#define __MCSMENUHANDLER_H__

#include <e32base.h>

class RMenu;
class CMenuOperation;
class CMenuItem;

/**
 *  Menu handler interface.
 *  Menu handlers execute menu commands.
 *  @lib mcsmenu.lib
 *  @since S60 v5.0
 */
class CMenuHandler: public CBase
    {

public:

    /**
    * Destructor.
    * @since S60 v5.0
    * @capability None.
    * @throws None.
    * @panic None.
    */
    IMPORT_C virtual ~CMenuHandler();

    /**
    * Query support for item type.
    * @since S60 v5.0
    * @param aType Type.
    * @return ETrue if handler supports this type, EFalse otherwise.
    * @capability None.
    * @throws None.
    * @panic None.
    */
    IMPORT_C virtual TBool SupportsType( const TDesC& aType );

    /**
    * Handle command.
    * @since S60 v5.0
    * @param aItem Item of supported type.
    * @param aCommand Command.
    * @param aParams. Command parameters.
    * @param aStatus Observer request status. When the operation completes,
    * this status will be completed with the resulting error code.
    * @return Asynchronous operation. Owned by the caller.
    * @capability None.
    * @throws This implementation throws KErrNotSupported.
    * @panic None.
    */
    IMPORT_C virtual CMenuOperation* HandleCommandL(
        CMenuItem& aItem,
        const TDesC8& aCommand,
        const TDesC8& aParams,
        TRequestStatus& aStatus );

protected:

    /**
    * Constructor.
    * @since S60 v5.0
    * @param aMenu Menu.
    */
    IMPORT_C CMenuHandler( RMenu &aMenu );

    /**
    * Second phase constructor.
    * @since S60 v5.0
    */
    IMPORT_C void BaseConstructL();

protected:  // data

    RMenu& iMenu; ///< Menu.

private:    // data
    
    class TData;
    TData* iData; ///< Private data. Own.

    };

#endif // __MCSMENUHANDLER_H__
