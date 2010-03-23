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

#ifndef TIMEOUT_H
#define TIMEOUT_H

// INCLUDE FILES

#include <e32base.h>

// CLASS DECLARATION

/**
 * Timeout timer.
 */
NONSHARABLE_CLASS( CTimeout ): public CTimer
    {
public:
    // Construct / destruct

    /**
     * Two phase constructor. Leaves on failure.
     * @param aPriority Active object priority.
     * @param aCallback Callback to be called when timer expires.
     * This callback must return EFalse - it will NOT be called only again.
     * @return The constructed timeout timer.
     */
    static CTimeout* NewL( TInt aPriority, TCallBack aCallBack );

    /**
     * Destructor.
     */
    virtual ~CTimeout();

protected:
    // from CActive

    /**
     * Outstanding request completed (timed out).
     */
    virtual void RunL();

protected:
    // Construct / destruct

    /**
     * Constructor.
     * @param aPriority Active object priority.
     * @param aCallback Callback to be called when timer expires.
     */
    CTimeout( TInt aPriority, TCallBack aCallBack );

private:
    // data

    TCallBack iCallBack; ///< Callback.
    };

#endif // TIMEOUT_H
// End of File
