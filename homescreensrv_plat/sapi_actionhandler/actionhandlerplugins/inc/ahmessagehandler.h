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


#ifndef C_ACTIVE_SPACE_MESSAGEHANDLER_H
#define C_ACTIVE_SPACE_MESSAGEHANDLER_H

#include <e32base.h>
#include "ahplugin.h"
// FORWARD DECLARATIONS
/**
 *  ActiveSpace Sms Handler Plugin
 *
 *
 *  @lib ?library
 *  @since S60 ?S60_version *** for example, S60 v3.0
 */
class CAHMessageHandler : public CAHPlugin
    {

public:

    /**
     * Two phase contruction
     */
    static CAHMessageHandler* NewL();

    /**
     * Two phase contruction
     */
    static CAHMessageHandler* NewLC();

    /**
     * Destructor
     */
    ~CAHMessageHandler();

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

    };

#endif // C_ACTIVE_SPACE_MESSAGEHANDLER_H
