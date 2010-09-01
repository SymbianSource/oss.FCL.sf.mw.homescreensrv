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
* Description:  Active Idle <-> Phone app synchronizer implementation.
*
*/


#ifndef C_AIIDLEAPPREGISTERIMPL_H
#define C_AIIDLEAPPREGISTERIMPL_H

#include "aiidleappregister.h"

/**
 * Idle application registration interface.
 */
NONSHARABLE_CLASS( CAiIdleAppRegisterImpl ) : public CAiIdleAppRegister
    {
public:
    static CAiIdleAppRegisterImpl* NewLC();
    ~CAiIdleAppRegisterImpl();

// from CAiIdleAppRegister
    void RegisterL();

private:    
// Construction
    CAiIdleAppRegisterImpl();
    };


#endif // C_AIIDLEAPPREGISTERIMPL_H

