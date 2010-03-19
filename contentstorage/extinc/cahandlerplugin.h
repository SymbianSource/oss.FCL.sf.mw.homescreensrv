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

#ifndef C_CAHANDLERPLUGIN_H
#define C_CAHANDLERPLUGIN_H

#include <e32base.h>
#include <ecom/ecom.h>

#include "cahandler.h"

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
class CCaHandlerPlugin: public CCaHandler
    {

public:

    /**
     * Two-phased constructor.
     * @param aImplementationUid  implementation uid
     * @return pointer to a newly created instance of CCaHandlerPlugin.
     */
    inline static CCaHandlerPlugin* NewL( TUid aImplementationUid );

    /**
     * Constructor.
     */
    inline CCaHandlerPlugin();

    /**
     * Destructor.
     */
    inline virtual ~CCaHandlerPlugin();

private:

    /**
     * Destructor ID key.
     */
    TUid iDtor_ID_Key;

    };

#include "cahandlerplugin.inl"

#endif // C_CAHANDLERPLUGIN_H
