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

#ifndef C_CAAPPHANDLERPLUGIN_H
#define C_CAAPPHANDLERPLUGIN_H

#include <e32base.h>

#include "cahandlerplugin.h"

class CEikonEnv;
class CCaUninstallOperation;

/**
 *  ?one_line_short_description
 *  ?more_complete_description
 *
 *  @code
 *   ?good_class_usage_example(s)
 *  @endcode
 *
 *  @lib ?library
 *  @since S60 ?S60_version *** for example, S60 v3.0
 */
NONSHARABLE_CLASS( CCaAppHandlerPlugin ): public CCaHandlerPlugin
    {

public:

    static CCaAppHandlerPlugin* NewL();

    /**
     * Destructor.
     */
    virtual ~CCaAppHandlerPlugin();

protected:

    CCaAppHandlerPlugin();

    void ConstructL();

public:

    // from CCaHandlerPlugin

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
    void HandleCommandL( CCaInnerEntry& aEntry, const TDesC8& aCommand );

private:

    void LaunchApplicationL( const TUid aUid, const TDesC8 &aParam,
            TInt aViewId );
    void CloseApplicationL( CCaInnerEntry& aEntry );

private:
    // data

    CEikonEnv* iEikEnv;
    CCaUninstallOperation* iUninstallOperation;
    };

#endif // C_CAAPPHANDLERPLUGIN_H
