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
* Description:  Definition of completed menu operation
*
*/


#ifndef __MENUCOMPLETEDOPERATION_H__
#define __MENUCOMPLETEDOPERATION_H__

#include "mcsmenuoperation.h"
#include <e32base.h>

class RMenu;

/**
 *  Completed CMenuOperation.
 *  Utility class to wrap synchronous functionality as asynchronous menu
 *  operation. Used in CMenuHandlers and other menu operations where the
 *  API requires an asynchronous operation but the concrete functionality
 *  is synchronous and the result is already available.
 *  @lib mcsmenu.lib
 *  @since S60 v5.0
 */
NONSHARABLE_CLASS( CMenuCompletedOperation ): public CMenuOperation
    {

public:

    /**
    * Destructor.
    */
    IMPORT_C virtual ~CMenuCompletedOperation();

    /**
    * Two-phased constructor. Constructs an operation that is already complete.
    * @param aMenu Menu.
    * @param aPriority Active Object priority.
    * @param aStatus Observer request status. Completes with aError.
    * @param aError Completion code (result of the operation).
    * @return Asynchronous operation. Owned by the caller.
    */
    IMPORT_C static CMenuCompletedOperation* NewL(
        RMenu &aMenu,
        TInt aPriority,
        TRequestStatus &aObserverStatus,
        TInt aError );

private:

    /**
    * Constructor
    * @param aMenu Menu.
    * @param aPriority Active Object priority.
    * @param aStatus Observer request status.
    */
    CMenuCompletedOperation(
        RMenu &aMenu,
        TInt aPriority,
        TRequestStatus &aObserverStatus );

    /**
    * Report completion.
    * @param aError Completion code (result of the operation).
    */
    void ReportCompletion( TInt aError );

// from base class CActive

    /**
    * Report completion to observer.
    */
    void RunL();

    /**
    * Cancel object (never called).
    */
    void DoCancel();

    /**
    * Handle error.
    * @param aError Error code.
    * @return KErrNone.
    */
    TInt RunError( TInt aError );

    };

#endif // __MENUCOMPLETEDOPERATION_H__
