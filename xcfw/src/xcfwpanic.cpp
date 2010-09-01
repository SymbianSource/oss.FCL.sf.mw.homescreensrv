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



//  INCLUDES
#include "xcfwpanic.h"

// -----------------------------------------------------------------------------
// TXCFWPanic::CheckPanic
// Panic with given code if aOK == EFalse
// -----------------------------------------------------------------------------
//
void TXCFWPanic::CheckPanic( TBool aOK, TInt aReason )
    {
#ifdef __XCFW_MODULE_TEST
#pragma BullseyeCoverage off
#endif
    if ( !aOK )
        {
        _LIT( KPanicText, "XML Content Framework" );
        User::Panic( KPanicText, aReason );            
        }
#ifdef __XCFW_MODULE_TEST
#pragma BullseyeCoverage on
#endif
    }

// -----------------------------------------------------------------------------
// TXCFWPanic::CheckLeaveL
// Leave with given code if aOK == EFalse
// -----------------------------------------------------------------------------
//
void TXCFWPanic::CheckLeaveL( TBool aOK, TInt aReason )
    {
#ifdef __XCFW_MODULE_TEST
#pragma BullseyeCoverage off
#endif
    if ( !aOK )
        {
        User::Leave( aReason );
        }
#ifdef __XCFW_MODULE_TEST
#pragma BullseyeCoverage on
#endif
    }


// End of file
