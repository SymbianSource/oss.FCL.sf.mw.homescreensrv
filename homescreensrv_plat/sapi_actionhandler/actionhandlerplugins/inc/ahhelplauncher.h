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
* Description:  ECOM Component
 *
*/


#ifndef C_ACTION_HANDLER_HELPLAUNCHER_H
#define C_ACTION_HANDLER_HELPLAUNCHER_H

#include "ahplugin.h"
// FORWARD DECLARATION
class CLiwMap;

/**
 *  Action Handler Help Launcher Plugin
 *
 *
 *  @lib ?library
 *  @since S60 v3.0
 */
class CAHHelpLauncher : public CAHPlugin
    {

public:

    /**
     * Two phase contruction
     */
    static CAHHelpLauncher* NewL();

    /**
     * Two phase contruction
     */
    static CAHHelpLauncher* NewLC();

    /**
     * Destructor
     */
    ~CAHHelpLauncher();

    /**
     * Executes provided action
     *
     * @return status code     
     */

    TInt ExecuteActionL( const CLiwMap* aMap );
private:

    /**
     * 2nd phase contructor
     */
    void ConstructL();

    TInt ExtractDesL( const CLiwMap* aMap, RBuf& aString,
        const TDesC8& aMapName );

    TInt ExtractUidL( const CLiwMap* aMap, TUid& aUid,
        const TDesC8& aMapName );

    };

#endif // C_ACTION_HANDLER_HELPLAUNCHER_H
