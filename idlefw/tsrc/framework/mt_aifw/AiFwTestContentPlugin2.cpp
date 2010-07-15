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

#include "AiFwTestContentPlugin2.h"
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
CAiFwTestContentPlugin2::CAiFwTestContentPlugin2()
{}

// ----------------------------------------------------------------------------
//
//
//
// ----------------------------------------------------------------------------
//
void CAiFwTestContentPlugin2::ConstructL()
{
}

// ----------------------------------------------------------------------------
//
//
//
// ----------------------------------------------------------------------------
//
CAiFwTestContentPlugin2* CAiFwTestContentPlugin2::NewL()
{
    CAiFwTestContentPlugin2* self = new (ELeave) CAiFwTestContentPlugin2;

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
CAiFwTestContentPlugin2::~CAiFwTestContentPlugin2()
{
}

// ----------------------------------------------------------------------------
//
//
//
// ----------------------------------------------------------------------------
//
TInt CAiFwTestContentPlugin2::Callback(TAny* /*aPtr*/)
{
    return KErrNone;
}

// ----------------------------------------------------------------------------
//
//
//
// ----------------------------------------------------------------------------
//
void CAiFwTestContentPlugin2::Resume(TAiTransitionReason /*aReason*/)
{
}

// ----------------------------------------------------------------------------
//
//
//
// ----------------------------------------------------------------------------
//
void CAiFwTestContentPlugin2::Suspend(TAiTransitionReason /*aReason*/)
{
}

// ----------------------------------------------------------------------------
//
//
//
// ----------------------------------------------------------------------------
//
void CAiFwTestContentPlugin2::Stop(TAiTransitionReason /*aReason*/)
{
}

// ----------------------------------------------------------------------------
//
//
//
// ----------------------------------------------------------------------------
//
void CAiFwTestContentPlugin2::SubscribeL(MAiContentObserver& /*aObserver*/)
{
   User::Leave(KErrNotSupported);
}

// ----------------------------------------------------------------------------
//
//
//
// ----------------------------------------------------------------------------
//
void CAiFwTestContentPlugin2::ConfigureL(RAiSettingsItemArray& /*aSettings*/)
    {
    User::Leave(KErrNotSupported);
    }

// ----------------------------------------------------------------------------
//
//
//
// ----------------------------------------------------------------------------
//
TAny* CAiFwTestContentPlugin2::Extension(TUid aUid)
{
    if (aUid == KExtensionUidProperty && !iFirstExt)
        {
        iFirstExt = ETrue;
        return static_cast<MAiPropertyExtension*>(this);
        }
        User::Leave(KErrNotSupported);
    return NULL;
}

// ----------------------------------------------------------------------------
//
//
//
// ----------------------------------------------------------------------------
//
TAny* CAiFwTestContentPlugin2::GetPropertyL(TInt /*aProperty*/)
{
      User::Leave(KErrNotSupported);

    return NULL;
}

// ----------------------------------------------------------------------------
//
//
//
// ----------------------------------------------------------------------------
//
void CAiFwTestContentPlugin2::SetPropertyL(TInt aProperty, TAny* aValue)
    {
    if( aValue )
        {
        switch( aProperty )
            {
            case EAiPublisherInfo:
                {
                return;
                }
            }
        }
        User::Leave(KErrNotSupported);
    }

// ----------------------------------------------------------------------------
//
//
//
// ----------------------------------------------------------------------------
//
void CAiFwTestContentPlugin2::HandleEvent(TInt /*aEvent*/, const TDesC& /*aParam*/)
    {
    User::Leave(KErrNotFound);
    }

// ========== OTHER EXPORTED FUNCTIONS ========================================

// End of File.
