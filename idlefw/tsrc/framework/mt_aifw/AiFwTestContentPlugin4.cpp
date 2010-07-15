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

#include "AiFwTestContentPlugin4.h"
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
CAiFwTestContentPlugin4::CAiFwTestContentPlugin4()
{}

// ----------------------------------------------------------------------------
//
//
//
// ----------------------------------------------------------------------------
//
void CAiFwTestContentPlugin4::ConstructL()
{
    iInfo = new (ELeave) TAiPublisherInfo;
    iInfo->iUid.iUid = KImplementationUidAiTestContentPlugin4;
    iInfo->iName.Copy( _L8( "AiFwTestPlugin4" ) );
}

// ----------------------------------------------------------------------------
//
//
//
// ----------------------------------------------------------------------------
//
CAiFwTestContentPlugin4* CAiFwTestContentPlugin4::NewL()
{
    CAiFwTestContentPlugin4* self = new (ELeave) CAiFwTestContentPlugin4;

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
CAiFwTestContentPlugin4::~CAiFwTestContentPlugin4()
{
    delete iInfo;
}

// ----------------------------------------------------------------------------
//
//
//
// ----------------------------------------------------------------------------
//
TInt CAiFwTestContentPlugin4::Callback(TAny* /*aPtr*/)
{
    return KErrNone;
}

// ----------------------------------------------------------------------------
//
//
//
// ----------------------------------------------------------------------------
//
void CAiFwTestContentPlugin4::Resume(TAiTransitionReason /*aReason*/)
{
    User::Leave( KErrNotFound );
}

// ----------------------------------------------------------------------------
//
//
//
// ----------------------------------------------------------------------------
//
void CAiFwTestContentPlugin4::Suspend(TAiTransitionReason /*aReason*/)
{
    
}

// ----------------------------------------------------------------------------
//
//
//
// ----------------------------------------------------------------------------
//
void CAiFwTestContentPlugin4::Stop(TAiTransitionReason /*aReason*/)
{
    
}

// ----------------------------------------------------------------------------
//
//
//
// ----------------------------------------------------------------------------
//
void CAiFwTestContentPlugin4::SubscribeL(MAiContentObserver& /*aObserver*/)
{
   
}

// ----------------------------------------------------------------------------
//
//
//
// ----------------------------------------------------------------------------
//
void CAiFwTestContentPlugin4::ConfigureL(RAiSettingsItemArray& aSettings)
    {
    aSettings.ResetAndDestroy();
    }

// ----------------------------------------------------------------------------
//
//
//
// ----------------------------------------------------------------------------
//
TAny* CAiFwTestContentPlugin4::Extension(TUid aUid)
{
    if (aUid == KExtensionUidProperty)
    {
        return static_cast<MAiPropertyExtension*>(this);
    }
    else if (aUid == KExtensionUidEventHandler)
        {
        return NULL;
        }
    return NULL;
}

// ----------------------------------------------------------------------------
//
//
//
// ----------------------------------------------------------------------------
//
TAny* CAiFwTestContentPlugin4::GetPropertyL(TInt aProperty)
{
    if( aProperty == EAiPublisherInfo )
        {
        return iInfo;
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
void CAiFwTestContentPlugin4::SetPropertyL(TInt /*aProperty*/, TAny* /*aValue*/)
    {
    
    }

// ----------------------------------------------------------------------------
//
//
//
// ----------------------------------------------------------------------------
//
void CAiFwTestContentPlugin4::HandleEvent(TInt /*aEvent*/, const TDesC& /*aParam*/)
    {
    
    }

// ========== OTHER EXPORTED FUNCTIONS ========================================

// End of File.
