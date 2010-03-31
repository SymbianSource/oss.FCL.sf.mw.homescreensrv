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
* Description:  Custom active scheduler for Active Idle Framework startup.
*
*/


#ifndef C_AIFWSTARTUPSCHEDULER_H
#define C_AIFWSTARTUPSCHEDULER_H


#include <e32base.h>

/**
 *  Custom active scheduler for Active Idle Framework startup.
 */
NONSHARABLE_CLASS( CAiFwStartupScheduler ) : public CActiveScheduler
    {
public:
    static CAiFwStartupScheduler* NewLC();

    ~CAiFwStartupScheduler();

    /**
     * Returns result code from this scheduler's execution.
     *
     * @return - KErrNone if the scheduler loop executed succesfully
     *         - Any of the system-wide error codes in case of an error.
     */
    TInt Result();

private:    
// from base class CActiveScheduler
    void Error(TInt aError) const;

// Construction
    CAiFwStartupScheduler();
private: // data

    /**
     * Result code returned by Result().
     */
    mutable TInt iResult;
    };


#endif // ? C_AIFWSTARTUPSCHEDULER_H

