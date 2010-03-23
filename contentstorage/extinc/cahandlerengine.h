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

#ifndef C_CAHANDLERENGINE_H
#define C_CAHANDLERENGINE_H

#include <e32base.h>

class CCaHandler;
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
NONSHARABLE_CLASS( CCaHandlerEngine ): public CBase
    {

public:
    IMPORT_C static CCaHandlerEngine* NewL();
    IMPORT_C static CCaHandlerEngine* NewLC();

    /**
     * Destructor.
     */
    IMPORT_C virtual ~CCaHandlerEngine();

    /**
     * ?description
     *
     * @since S60 ?S60_version
     * @param ?arg1 ?description
     * @param ?arg2 ?description
     * @return ?description
     */
    IMPORT_C void HandleCommandL( CCaInnerEntry& aEntry,
            const TDesC8& aCommand );

private:

    CCaHandlerEngine();

    void ConstructL();

    /**
     * Find loaded handler by type.
     * @param aType Type.
     * @return Loaded handler or NULL no handler is loaded for that type.
     */
    CCaHandler* FindHandler( const TDesC& aType );

    /**
     * Load handler for given type.
     * @param aType Type.
     * @return Loaded handler or NULL no handler is found for that type.
     */
    CCaHandler* LoadHandlerL( const TDesC& aType );

private:
    // data

    /**
     * Array of Handlers.
     * Contents owned.
     */
    RPointerArray<CCaHandler> iHandlers;

    };

#endif // C_CAHANDLERENGINE_H
