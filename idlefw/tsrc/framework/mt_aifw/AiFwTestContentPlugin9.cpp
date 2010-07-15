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

#include "AiFwTestContentPlugin9.h"
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
CAiFwTestContentPlugin9::CAiFwTestContentPlugin9()
{}

// ----------------------------------------------------------------------------
//
//
//
// ----------------------------------------------------------------------------
//
void CAiFwTestContentPlugin9::ConstructL()
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
CAiFwTestContentPlugin9* CAiFwTestContentPlugin9::NewL()
{
    CAiFwTestContentPlugin9* self = new (ELeave) CAiFwTestContentPlugin9;

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
CAiFwTestContentPlugin9::~CAiFwTestContentPlugin9()
{
    delete iInfo;
}

// ----------------------------------------------------------------------------
//
//
//
// ----------------------------------------------------------------------------
//
TInt CAiFwTestContentPlugin9::Callback(TAny* /*aPtr*/)
{
    return KErrNone;
}

// ----------------------------------------------------------------------------
//
//
//
// ----------------------------------------------------------------------------
//
void CAiFwTestContentPlugin9::Resume(TAiTransitionReason /*aReason*/)
{
    
}

// ----------------------------------------------------------------------------
//
//
//
// ----------------------------------------------------------------------------
//
void CAiFwTestContentPlugin9::Suspend(TAiTransitionReason /*aReason*/)
{
    
}

// ----------------------------------------------------------------------------
//
//
//
// ----------------------------------------------------------------------------
//
void CAiFwTestContentPlugin9::Stop(TAiTransitionReason /*aReason*/)
{
    
}

// ----------------------------------------------------------------------------
//
//
//
// ----------------------------------------------------------------------------
//
void CAiFwTestContentPlugin9::SubscribeL(MAiContentObserver& /*aObserver*/)
{
   
}

// ----------------------------------------------------------------------------
//
//
//
// ----------------------------------------------------------------------------
//
void CAiFwTestContentPlugin9::ConfigureL(RAiSettingsItemArray& aSettings)
    {
    aSettings.ResetAndDestroy();
    }

// ----------------------------------------------------------------------------
//
//
//
// ----------------------------------------------------------------------------
//
TAny* CAiFwTestContentPlugin9::Extension(TUid /*aUid*/)
{
    return NULL;
}

// ----------------------------------------------------------------------------
//
//
//
// ----------------------------------------------------------------------------
//
TAny* CAiFwTestContentPlugin9::GetPropertyL(TInt /*aProperty*/)
{
    return NULL;
}

// ----------------------------------------------------------------------------
//
//
//
// ----------------------------------------------------------------------------
//
void CAiFwTestContentPlugin9::SetPropertyL(TInt /*aProperty*/, TAny* /*aValue*/)
    {
    
    }

// ----------------------------------------------------------------------------
//
//
//
// ----------------------------------------------------------------------------
//
void CAiFwTestContentPlugin9::HandleEvent(TInt /*aEvent*/, const TDesC& /*aParam*/)
    {
    
    }

// ========== OTHER EXPORTED FUNCTIONS ========================================

// End of File.
