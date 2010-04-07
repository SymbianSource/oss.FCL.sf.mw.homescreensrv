/*
* Copyright (c) 2005-2006 Nokia Corporation and/or its subsidiary(-ies).
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
* Description:  Device status observer
*
*/


#ifndef M_AIDEVICESTATUSOBSERVER_H
#define M_AIDEVICESTATUSOBSERVER_H

#include "aipluginstatemachine.h"

/**
 *  description
 * 
 *  @lib aifw_hs
 *  @since S60 3.2
 */
class MAiDeviceStatusObserver
    {

public:

    virtual ~MAiDeviceStatusObserver(){}

    virtual TAiStateChanges Status() = 0;

    };

#endif // M_AIDEVICESTATUSOBSERVER_H

// End of File.
