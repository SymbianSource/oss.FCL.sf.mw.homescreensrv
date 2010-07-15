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
* Description: 
*
*/


#ifndef INC_AIFWTESTDEFINES_H
#define INC_AIFWTESTDEFINES_H

// ========== INCLUDE FILES ===================================================

#include "aicontentmodel.h"
#include "AiFwTestContentPlugin.h"
#include "AiFwTestControllerPlugin.h"

// ========== CONSTANTS =======================================================

using namespace AiTestUiController;

const TInt KImplementationUidAiTestContentPlugin = 0x10170010;
const TInt KImplementationUidAiFwTestUiController = 0x10170011;
const TInt KImplementationUidAiFwTestUiController2 = 0x10170016;
const TInt KImplementationUidAiTestContentPlugin2 = 0x10170012;
const TInt KImplementationUidAiTestContentPlugin3 = 0x10170013;
const TInt KImplementationUidAiTestContentPlugin4 = 0x10170014;
const TInt KImplementationUidAiTestContentPlugin5 = 0x10170015;
const TInt KImplementationUidAiTestContentPlugin6 = 0x10170017;
const TInt KImplementationUidAiTestContentPlugin7 = 0x10170018;
const TInt KImplementationUidAiTestContentPlugin8 = 0x10170019;
const TInt KImplementationUidAiTestContentPlugin9 = 0x10170020;

_LIT( KTestEvent, "AiFwTestPlugin/test_event" );

// ========== MACROS ==========================================================

// ========== DATA TYPES ======================================================

struct TAiTestTLSStorage
    {
    CAiFwTestContentPlugin* iContentPlugin;
    CAiFwTestControllerPlugin* iControllerPlugin;
    
    TAiTestTLSStorage();
    static TAiTestTLSStorage* CreateInstanceL();
    static void DestroyInstance();
    };

// ========== FUNCTION PROTOTYPES =============================================

// ========== FORWARD DECLARATIONS ============================================

// ========== CLASS DECLARATION ===============================================

#endif

// End of File.
