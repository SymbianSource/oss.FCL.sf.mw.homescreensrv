/*
* Copyright (c) 2002-2004 Nokia Corporation and/or its subsidiary(-ies).
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


// ========== INCLUDE FILES ===================================================

#include "AiFwTestContentPlugin5.h"
#include "AiFwTestDefines.h"
#include "AiFwTestContentModel.h"

// ========== EXTERNAL DATA STRUCTURES ========================================

// ========== EXTERNAL FUNCTION PROTOTYPES ====================================

// ========== CONSTANTS =======================================================

// ========== MACROS ==========================================================

// ========== LOCAL CONSTANTS AND MACROS ======================================

// ========== MODULE DATA STRUCTURES ==========================================

// ========== LOCAL FUNCTION PROTOTYPES =======================================

// ========== FORWARD DECLARATIONS ============================================

// ========== LOCAL FUNCTIONS =================================================

// ========== MEMBER FUNCTIONS ================================================

// ----------------------------------------------------------------------------
//
//
//
// ----------------------------------------------------------------------------
//
CAiFwTestContentPlugin5::CAiFwTestContentPlugin5()
{}

// ----------------------------------------------------------------------------
//
//
//
// ----------------------------------------------------------------------------
//
void CAiFwTestContentPlugin5::ConstructL()
{
}

// ----------------------------------------------------------------------------
//
//
//
// ----------------------------------------------------------------------------
//
CAiFwTestContentPlugin5* CAiFwTestContentPlugin5::NewL()
{
    CAiFwTestContentPlugin5* self = new (ELeave) CAiFwTestContentPlugin5;

    CleanupStack::PushL(self);
    self->ConstructL();
    CleanupStack::Pop();

    return self;
}

// ----------------------------------------------------------------------------
//
//
//
// ----------------------------------------------------------------------------
//
CAiFwTestContentPlugin5::~CAiFwTestContentPlugin5()
{
}

// ----------------------------------------------------------------------------
//
//
//
// ----------------------------------------------------------------------------
//
TInt CAiFwTestContentPlugin5::Callback(TAny* /*aPtr*/)
{
    return KErrNone;
}

// ----------------------------------------------------------------------------
//
//
//
// ----------------------------------------------------------------------------
//
void CAiFwTestContentPlugin5::Resume(TAiTransitionReason /*aReason*/)
{
}

// ----------------------------------------------------------------------------
//
//
//
// ----------------------------------------------------------------------------
//
void CAiFwTestContentPlugin5::Suspend(TAiTransitionReason /*aReason*/)
{
}

// ----------------------------------------------------------------------------
//
//
//
// ----------------------------------------------------------------------------
//
void CAiFwTestContentPlugin5::Stop(TAiTransitionReason /*aReason*/)
{
}

// ----------------------------------------------------------------------------
//
//
//
// ----------------------------------------------------------------------------
//
void CAiFwTestContentPlugin5::SubscribeL(MAiContentObserver& /*aObserver*/)
{
}

// ----------------------------------------------------------------------------
//
//
//
// ----------------------------------------------------------------------------
//
void CAiFwTestContentPlugin5::ConfigureL(RAiSettingsItemArray& aSettings)
    {
    aSettings.ResetAndDestroy();
    }

// ----------------------------------------------------------------------------
//
//
//
// ----------------------------------------------------------------------------
//
TAny* CAiFwTestContentPlugin5::Extension(TUid aUid)
{
    if (aUid == KExtensionUidProperty && !iFirstExt)
        {
        iFirstExt = ETrue;
        return static_cast<MAiPropertyExtension*>(this);
        }
    return NULL;
}

// ----------------------------------------------------------------------------
//
//
//
// ----------------------------------------------------------------------------
//
TAny* CAiFwTestContentPlugin5::GetPropertyL(TInt /*aProperty*/)
{
    return NULL;
}

// ----------------------------------------------------------------------------
//
//
//
// ----------------------------------------------------------------------------
//
void CAiFwTestContentPlugin5::SetPropertyL(TInt /*aProperty*/, TAny* /*aValue*/)
    {
    User::Leave(KErrNotSupported);
    }

// ----------------------------------------------------------------------------
//
//
//
// ----------------------------------------------------------------------------
//
void CAiFwTestContentPlugin5::HandleEvent(TInt /*aEvent*/, const TDesC& /*aParam*/)
    {
    }

// ========== OTHER EXPORTED FUNCTIONS ========================================

// End of File.
