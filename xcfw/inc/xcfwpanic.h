/*
* Copyright (c) 2002-2005 Nokia Corporation and/or its subsidiary(-ies).
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
* Description:  Implements XCFW panic handling.
*
*/



#ifndef XCFWPANIC_H
#define XCFWPANIC_H

#define __ASSERT_PANIC(c, r) TXCFWPanic::CheckPanic(c ,r)
#define __ASSERT_LEAVE(c, r) TXCFWPanic::CheckLeaveL(c, r)

#ifdef __DEBUG
    #define __ASSERT_PANIC_DEBUG(c, r) TXCFWPanic::CheckPanic(c, r)
    #define __ASSERT_LEAVE_DEBUG(c, r) TXCFWPanic::CheckLeaveL(c, r)
#else
    #define __ASSERT_PANIC_DEBUG(c, r)
    #define __ASSERT_LEAVE_DEBUG(c, r)
#endif

//  INCLUDES
#include <e32std.h>

// DATA TYPES
enum TXCFWPanicCode
    {
    EXCFWPanicNoTypeIdentifier = 1
    };

/**
* Menu Engine panic
*/
class TXCFWPanic
    {
    public:

        /**
        * Panics if aOK == EFalse
        * @param aOK defines if we need to panic or not (EFalse => Panic)
        * @param aReason reason for panic
        */
        static void CheckPanic( TBool aOK, TInt aReason );
        
        /**
        * Leaves if aOK = EFalse
        * @param aOK defines if we need to panic or not (EFalse => Panic)
        * @param aReason reason for panic
        */
        static void CheckLeaveL( TBool aOK, TInt aReason );
    };

#endif //MENUMODELPANIC_H

// End of file
