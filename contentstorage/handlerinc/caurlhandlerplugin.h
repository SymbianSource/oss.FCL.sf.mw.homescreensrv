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
 * Description:  ?Description
 *
 */

#ifndef __CCAURLHANDLERPLUGIN_H__
#define __CCAURLHANDLERPLUGIN_H__

#include "cahandlerplugin.h"

#include <e32base.h>

// Constants
const TUid KUidBrowser =
    {
    0x10008D39
    };
_LIT( KBrowserPrefix, "4 " );

/**
 * Handler for entries with typenme "url".
 */
NONSHARABLE_CLASS( CCaUrlHandlerPlugin ): public CCaHandlerPlugin
    {

public:
    // construction

    /**
     * Destructor.
     */
    virtual ~CCaUrlHandlerPlugin();

    /**
     * Two-phased constructor. Leaves on failure.
     * @return The constructed object.
     */
    static CCaUrlHandlerPlugin* NewL();

protected:
    // construction

    /**
     * Constructor.
     * @param aMenu Menu.
     */
    CCaUrlHandlerPlugin();

    /**
     * Second-phase constructor.
     */
    void ConstructL();

public:

    /**
     * Query support for item type.
     * @param aType Type.
     * @return ETrue if handler supports this type, EFalse otherwise.
     */
    TBool SupportsType( const TDesC& aType );

    /**
     * Handle command.
     * @param aEntry The url entry to be processed.
     * @param aCommand Command.
     */
    void HandleCommandL( CCaInnerEntry& aEntry, const TDesC8& aCommand );

private:
    // internal

    /**
     * Launches the url.
     * @param aUrl The url to be launched.
     */
    void LaunchUrlL( const TDesC& aUrl );

    };

#endif // __CCAURLHANDLERPLUGIN_H__
