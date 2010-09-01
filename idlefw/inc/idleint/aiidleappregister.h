/*
* Copyright (c) 2006 Nokia Corporation and/or its subsidiary(-ies).
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
* Description:  Idle application registration interface.
*
*/


#ifndef C_AIIDLEAPPREGISTER_H
#define C_AIIDLEAPPREGISTER_H

#include <e32base.h>

/**
 * @ingroup group_aiidleint
 *
 * Idle application registration interface.
 */
class CAiIdleAppRegister : public CBase
    {

public:

    IMPORT_C static CAiIdleAppRegister* NewLC();

    ~CAiIdleAppRegister() { }

    /**
     * Registers Active Idle as the system idle application.
     */
    virtual void RegisterL() = 0;
    };


#endif // C_AIIDLEAPPREGISTER_H

