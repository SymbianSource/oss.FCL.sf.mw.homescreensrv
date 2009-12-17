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
* Description:  Definition of asynchronous menu operation base class
*
*/


#ifndef __MCSMENUOPERATION_H__
#define __MCSMENUOPERATION_H__

#include <e32base.h>

class RMenu;

/**
 *  Asynchronous menu operation base class.
 *  Defines ownership and cancellation of asynchronous menu operations;
 *  does not define the operation itself.
 *  Concrete derived classes must provide:
 *  - A request function which sets the observer status to KRequesPending;
 *  - DoCancel() which cancels the operation;
 *  - RunL() which should, in addition to any other required functionality,
 *    always end by completing the observer request status.
 *  @lib -
 *  @since S60 v5.0
 */
class CMenuOperation : public CActive
    {

protected:

    /**
    * Constructor.
    * @param aMenu Menu.
    * @param aPriority Active Object priority.
    * @param aObserverStatus Completes when the operation is finished.
    * @capability None.
    * @throws None.
    * @panic None.
    */
    CMenuOperation(
        RMenu &aMenu,
        TInt aPriority,
        TRequestStatus &aObserverStatus )
        : CActive( aPriority ), iMenu( aMenu ), iObserverStatus( aObserverStatus ) {}

protected:  // data

    RMenu& iMenu; ///< Menu.
    TRequestStatus& iObserverStatus; ///< Observer request status.

    };

#endif // __MCSMENUOPERATION_H__
