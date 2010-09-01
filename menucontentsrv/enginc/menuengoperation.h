/*
* Copyright (c) 2009 Nokia Corporation and/or its subsidiary(-ies).
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
* Description:
*
*/

#ifndef __MENUENGOPERATION_H__
#define __MENUENGOPERATION_H__

#include <e32base.h>

class CMenuEng;

/**
* Menu Engine Operation Interface - write-access engine functionality.
* Operations are executed by the engine, in the order they were added.
*/
NONSHARABLE_CLASS( MMenuEngOperation )
    {

public:     // from MMenuEngOperation

    /**
    * Perform operations on the menu. Write-access is possible.
    * After this method returns, the engine will commit (save) the
    * changes asynchronously, and call CompletedMenuEngOperation()
    * with the result.
    * No asynchronous functionality is allowed, the engine scheduler is
    * not reentrant! MMenuEngOperation-s should be short and synchronous.
    * If long-lasting or asynchronous functionality is required, perform the
    * long lasting and asynchronous part before scheduling the operation to
    * the engine.
    */
    virtual void RunMenuEngOperationL() = 0;

    /**
    * Operation complete (changes saved).
    * @param aErr Error code.
    */
    virtual void CompletedMenuEngOperation( TInt aErr ) = 0;

    };

#endif // __MENUENGOPERATION_H__
