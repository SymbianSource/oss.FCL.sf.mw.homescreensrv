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

#include "AiFwTestContentPlugin3.h"
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
CAiFwTestContentPlugin3::CAiFwTestContentPlugin3()
{}

// ----------------------------------------------------------------------------
//
//
//
// ----------------------------------------------------------------------------
//
void CAiFwTestContentPlugin3::ConstructL()
{
    iInfo = new (ELeave) TAiPublisherInfo;
    iInfo->iUid.iUid = KImplementationUidAiTestContentPlugin4;
    iInfo->iName.Copy( _L8( "AiFwTestPlugin3" ) );
}

// ----------------------------------------------------------------------------
//
//
//
// ----------------------------------------------------------------------------
//
CAiFwTestContentPlugin3* CAiFwTestContentPlugin3::NewL()
{
    CAiFwTestContentPlugin3* self = new (ELeave) CAiFwTestContentPlugin3;

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
CAiFwTestContentPlugin3::~CAiFwTestContentPlugin3()
{
    delete iInfo;
}

// ----------------------------------------------------------------------------
//
//
//
// ----------------------------------------------------------------------------
//
TInt CAiFwTestContentPlugin3::Callback(TAny* /*aPtr*/)
{
    return KErrNone;
}

// ----------------------------------------------------------------------------
//
//
//
// ----------------------------------------------------------------------------
//
void CAiFwTestContentPlugin3::Resume(TAiTransitionReason /*aReason*/)
{
}

// ----------------------------------------------------------------------------
//
//
//
// ----------------------------------------------------------------------------
//
void CAiFwTestContentPlugin3::Suspend(TAiTransitionReason /*aReason*/)
{
}

// ----------------------------------------------------------------------------
//
//
//
// ----------------------------------------------------------------------------
//
void CAiFwTestContentPlugin3::Stop(TAiTransitionReason /*aReason*/)
{
}

// ----------------------------------------------------------------------------
//
//
//
// ----------------------------------------------------------------------------
//
void CAiFwTestContentPlugin3::SubscribeL(MAiContentObserver& /*aObserver*/)
{
}

// ----------------------------------------------------------------------------
//
//
//
// ----------------------------------------------------------------------------
//
void CAiFwTestContentPlugin3::ConfigureL(RAiSettingsItemArray& aSettings)
    {
    aSettings.ResetAndDestroy();
    }

// ----------------------------------------------------------------------------
//
//
//
// ----------------------------------------------------------------------------
//
TAny* CAiFwTestContentPlugin3::Extension(TUid aUid)
{
    if (aUid == KExtensionUidProperty)
    {
        return static_cast<MAiPropertyExtension*>(this);
    }
    else if (aUid == KExtensionUidEventHandler)
        {
        User::Leave( KErrNotSupported );
        return static_cast<MAiEventHandlerExtension*>(this);
        }
    return NULL;
}

// ----------------------------------------------------------------------------
//
//
//
// ----------------------------------------------------------------------------
//
TAny* CAiFwTestContentPlugin3::GetPropertyL(TInt aProperty)
{
    if( aProperty == EAiPublisherInfo )
        {
        return iInfo;
        }
    return NULL;
}

// ----------------------------------------------------------------------------
//
//
//
// ----------------------------------------------------------------------------
//
void CAiFwTestContentPlugin3::SetPropertyL(TInt aProperty, TAny* aValue)
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
void CAiFwTestContentPlugin3::HandleEvent(TInt /*aEvent*/, const TDesC& /*aParam*/)
    {
    }

// ========== OTHER EXPORTED FUNCTIONS ========================================

// End of File.
