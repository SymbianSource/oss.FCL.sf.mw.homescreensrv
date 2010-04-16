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

#ifndef C_CAAPPHANDLER_H
#define C_CAAPPHANDLER_H

#include <e32base.h>

// forward declarations
class CEikonEnv;
class CCaInnerEntry;
class CCaUninstallOperation;

/**
 *  Command handler for application entries.
 *
 *  @lib caclient.lib
 */
NONSHARABLE_CLASS( CCaAppHandler )
    {

public:
    /**
    * Allocates memory for and initializes CCaAppHandler object
    */
    static CCaAppHandler* NewL();

    /**
     * Destructor.
     */
    virtual ~CCaAppHandler();

protected:

    CCaAppHandler();

    void ConstructL();

public:

    /**
     * Handle command.
     * @param aItem Item of supported type.
     * @param aCommand Command.
     * @param aParams. Command parameters.
     * @param aStatus Observer request status. When the operation completes,
     * this status will be completed with the resulting error code.
     * @return Asynchronous operation. Owned by the caller.
     */
    void HandleCommandL( CCaInnerEntry& aEntry, const TDesC8& aCommand );

private:
    /**
     * Launches application
     * @param aUid UID of the application to launch
     * @param aParam command parameters
     * @param aViewId id of the view the application is to start in
     */
    void LaunchApplicationL( const TUid aUid, const TDesC8 &aParam,
            TInt aViewId );
    
    /**
     * Closes application
     * @param aEntry the entry represeting application to close
     */
    void CloseApplicationL( CCaInnerEntry& aEntry );

private:
    // data

    CEikonEnv* iEikEnv;
    CCaUninstallOperation* iUninstallOperation;
    };

#endif // C_CAAPPHANDLER_H
