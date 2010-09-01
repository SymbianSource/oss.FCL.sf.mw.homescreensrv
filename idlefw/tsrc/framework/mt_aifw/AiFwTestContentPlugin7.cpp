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

#include "AiFwTestContentPlugin7.h"
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
CAiFwTestContentPlugin7::CAiFwTestContentPlugin7()
{}

// ----------------------------------------------------------------------------
//
//
//
// ----------------------------------------------------------------------------
//
void CAiFwTestContentPlugin7::ConstructL()
{
    iInfo = new (ELeave) TAiPublisherInfo;
    iInfo->iUid.iUid = KImplementationUidAiTestContentPlugin4;
    iInfo->iName.Copy( _L8( "AiFwTestPlugin7" ) );
}

// ----------------------------------------------------------------------------
//
//
//
// ----------------------------------------------------------------------------
//
CAiFwTestContentPlugin7* CAiFwTestContentPlugin7::NewL()
{
    CAiFwTestContentPlugin7* self = new (ELeave) CAiFwTestContentPlugin7;

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
CAiFwTestContentPlugin7::~CAiFwTestContentPlugin7()
{
    delete iInfo;
}

// ----------------------------------------------------------------------------
//
//
//
// ----------------------------------------------------------------------------
//
TInt CAiFwTestContentPlugin7::Callback(TAny* /*aPtr*/)
{
    return KErrNone;
}

// ----------------------------------------------------------------------------
//
//
//
// ----------------------------------------------------------------------------
//
void CAiFwTestContentPlugin7::Resume(TAiTransitionReason /*aReason*/)
{
    
}

// ----------------------------------------------------------------------------
//
//
//
// ----------------------------------------------------------------------------
//
void CAiFwTestContentPlugin7::Suspend(TAiTransitionReason /*aReason*/)
{
    
}

// ----------------------------------------------------------------------------
//
//
//
// ----------------------------------------------------------------------------
//
void CAiFwTestContentPlugin7::Stop(TAiTransitionReason /*aReason*/)
{
    
}

// ----------------------------------------------------------------------------
//
//
//
// ----------------------------------------------------------------------------
//
void CAiFwTestContentPlugin7::SubscribeL(MAiContentObserver& /*aObserver*/)
{
   
}

// ----------------------------------------------------------------------------
//
//
//
// ----------------------------------------------------------------------------
//
void CAiFwTestContentPlugin7::ConfigureL(RAiSettingsItemArray& aSettings)
    {
    aSettings.ResetAndDestroy();
    }

// ----------------------------------------------------------------------------
//
//
//
// ----------------------------------------------------------------------------
//
TAny* CAiFwTestContentPlugin7::Extension(TUid aUid)
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
TAny* CAiFwTestContentPlugin7::GetPropertyL(TInt aProperty)
{
    if( aProperty == EAiPublisherInfo )
        {
        iLeaveOrNull = !iLeaveOrNull;
        if( !iLeaveOrNull )
            {
            User::Leave( KErrNotSupported );
            }
        }
    return NULL;
}

// ----------------------------------------------------------------------------
//
//
//
// ----------------------------------------------------------------------------
//
void CAiFwTestContentPlugin7::SetPropertyL(TInt /*aProperty*/, TAny* /*aValue*/)
    {
    
    }

// ----------------------------------------------------------------------------
//
//
//
// ----------------------------------------------------------------------------
//
void CAiFwTestContentPlugin7::HandleEvent(TInt /*aEvent*/, const TDesC& /*aParam*/)
    {
    
    }

// ========== OTHER EXPORTED FUNCTIONS ========================================

// End of File.
