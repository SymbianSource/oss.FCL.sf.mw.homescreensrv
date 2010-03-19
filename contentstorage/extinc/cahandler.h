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

#ifndef C_CAHANDLER_H
#define C_CAHANDLER_H

#include <e32base.h>

class CCaInnerEntry;

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
class CCaHandler: public CBase
    {

public:

    /**
     * Destructor.
     */
    IMPORT_C virtual ~CCaHandler();

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
     * this status will be completed with the resulting error code.
     * @capability None.
     * @throws This implementation throws KErrNotSupported.
     * @panic None.
     */
    IMPORT_C virtual void HandleCommandL( CCaInnerEntry& aEntry,
            const TDesC8& aCommand );

protected:

    /**
     * Constructor.
     */
    IMPORT_C CCaHandler();

    /**
     * Base constructor.
     */
    IMPORT_C void BaseConstructL();

private:
    // data

    class TData;

    /**
     * Private data.
     */
    TData* iData;

    };

#endif // C_CAHANDLER_H
