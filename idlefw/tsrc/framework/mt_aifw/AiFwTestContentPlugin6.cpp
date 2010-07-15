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

#include "AiFwTestContentPlugin6.h"
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
CAiFwTestContentPlugin6::CAiFwTestContentPlugin6()
{}

// ----------------------------------------------------------------------------
//
//
//
// ----------------------------------------------------------------------------
//
void CAiFwTestContentPlugin6::ConstructL()
{
    iInfo = new (ELeave) TAiPublisherInfo;
    iInfo->iUid.iUid = KImplementationUidAiTestContentPlugin6;
    iInfo->iName.Copy( _L8( "erronouspluginname" ) );
}

// ----------------------------------------------------------------------------
//
//
//
// ----------------------------------------------------------------------------
//
CAiFwTestContentPlugin6* CAiFwTestContentPlugin6::NewL()
{
    CAiFwTestContentPlugin6* self = new (ELeave) CAiFwTestContentPlugin6;

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
CAiFwTestContentPlugin6::~CAiFwTestContentPlugin6()
{
delete iInfo;
}

// ----------------------------------------------------------------------------
//
//
//
// ----------------------------------------------------------------------------
//
TInt CAiFwTestContentPlugin6::Callback(TAny* /*aPtr*/)
{
    return KErrNone;
}

// ----------------------------------------------------------------------------
//
//
//
// ----------------------------------------------------------------------------
//
void CAiFwTestContentPlugin6::Resume(TAiTransitionReason /*aReason*/)
{
}

// ----------------------------------------------------------------------------
//
//
//
// ----------------------------------------------------------------------------
//
void CAiFwTestContentPlugin6::Suspend(TAiTransitionReason /*aReason*/)
{
}

// ----------------------------------------------------------------------------
//
//
//
// ----------------------------------------------------------------------------
//
void CAiFwTestContentPlugin6::Stop(TAiTransitionReason /*aReason*/)
{
}

// ----------------------------------------------------------------------------
//
//
//
// ----------------------------------------------------------------------------
//
void CAiFwTestContentPlugin6::SubscribeL(MAiContentObserver& /*aObserver*/)
{
}

// ----------------------------------------------------------------------------
//
//
//
// ----------------------------------------------------------------------------
//
void CAiFwTestContentPlugin6::ConfigureL(RAiSettingsItemArray& aSettings)
    {
    aSettings.ResetAndDestroy();
    }

// ----------------------------------------------------------------------------
//
//
//
// ----------------------------------------------------------------------------
//
TAny* CAiFwTestContentPlugin6::Extension(TUid aUid)
{
    if (aUid == KExtensionUidProperty && iExtCount < 2)
        {
        ++iExtCount;
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
TAny* CAiFwTestContentPlugin6::GetPropertyL(TInt /*aProperty*/)
{
    return iInfo;
}

// ----------------------------------------------------------------------------
//
//
//
// ----------------------------------------------------------------------------
//
void CAiFwTestContentPlugin6::SetPropertyL(TInt aProperty, TAny* aValue)
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
void CAiFwTestContentPlugin6::HandleEvent(TInt /*aEvent*/, const TDesC& /*aParam*/)
    {
    }

// ========== OTHER EXPORTED FUNCTIONS ========================================

// End of File.
