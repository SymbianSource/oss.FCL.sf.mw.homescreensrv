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

#ifndef __CCAURLHANDLER_H__
#define __CCAURLHANDLER_H__

#include <e32base.h>

class CCaInnerEntry;

// Constants
const TUid KUidBrowser =
    {
    0x10008D39
    };
_LIT( KBrowserPrefix, "4 " );

/**
 *  Command handler for url entries.
 *
 *  @lib caclient.lib
 */
NONSHARABLE_CLASS( CCaUrlHandler ): public CBase
    {

public:
    // construction

    /**
     * Destructor.
     */
    virtual ~CCaUrlHandler();

    /**
     * Two-phased constructor. Leaves on failure.
     * @return The constructed object.
     */
    static CCaUrlHandler* NewL();

protected:
    // construction

    /**
     * Constructor.
     * @param aMenu Menu.
     */
    CCaUrlHandler();

    /**
     * Second-phase constructor.
     */
    void ConstructL();

public:


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

#endif // __CCAURLHANDLER_H__
