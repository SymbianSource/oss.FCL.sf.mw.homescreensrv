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


// ========== INCLUDE FILES ===================================================

#include "AiFwTestControllerPlugin2.h"
#include <e32std.h>

#include "AiFwTestDefines.h"
#include "AiFwTestContentModel.h"

using namespace AiTestUiController2;


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
CAiFwTestControllerPlugin2::CAiFwTestControllerPlugin2()
	{
	}

// ----------------------------------------------------------------------------
//
//
//
// ----------------------------------------------------------------------------
//
void CAiFwTestControllerPlugin2::ConstructL()
	{
	User::Leave( KErrNotFound );
	}

// ----------------------------------------------------------------------------
//
//
//
// ----------------------------------------------------------------------------
//
CAiFwTestControllerPlugin2* CAiFwTestControllerPlugin2::NewL()
	{
    CAiFwTestControllerPlugin2* self = new (ELeave) CAiFwTestControllerPlugin2();
	
    CleanupStack::PushL( self );
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
CAiFwTestControllerPlugin2::~CAiFwTestControllerPlugin2()
	{
 	}

// ----------------------------------------------------------------------------
//
//
//
// ----------------------------------------------------------------------------
//
TInt CAiFwTestControllerPlugin2::StartTransaction(TInt /*aTxId*/)
	{
    return 0;
	}

// ----------------------------------------------------------------------------
//
//
//
// ----------------------------------------------------------------------------
//
TInt CAiFwTestControllerPlugin2::Commit(TInt /*aTxId*/)
	{
    return 0;
	}

// ----------------------------------------------------------------------------
//
//
//
// ----------------------------------------------------------------------------
//
TInt CAiFwTestControllerPlugin2::CancelTransaction(TInt /*aTxId*/)
	{
    return 0;
	}

TBool CAiFwTestControllerPlugin2::CanPublish( MAiPropertyExtension& /*aPlugin*/,
                                              TInt /*aContent*/,
                                              TInt /*aIndex*/ )
    {
    return EFalse;
    }

// ----------------------------------------------------------------------------
//
//
//
// ----------------------------------------------------------------------------
//
TInt CAiFwTestControllerPlugin2::Publish( MAiPropertyExtension& /*aPlugin*/,
							  TInt /*aContent*/, TInt /*aResource*/, TInt /*aIndex*/ )
	{
    return 0;
	}

// ----------------------------------------------------------------------------
//
//
//
// ----------------------------------------------------------------------------
//
TInt CAiFwTestControllerPlugin2::Publish( MAiPropertyExtension& /*aPlugin*/,
							  TInt /*aContent*/, const TDesC16& /*aBuf*/, TInt /*aIndex */)
	{
 	
    return 0;
	}

// ----------------------------------------------------------------------------
//
//
//
// ----------------------------------------------------------------------------
//
TInt CAiFwTestControllerPlugin2::Publish( MAiPropertyExtension& /*aPlugin*/,
							  TInt /*aContent*/, const TDesC8& /*aBuf*/, TInt /*aIndex */)
	{

    return 0;
	}
	
// ----------------------------------------------------------------------------
//
//
//
// ----------------------------------------------------------------------------
//
TInt CAiFwTestControllerPlugin2::Publish( MAiPropertyExtension& /*aPlugin*/,
							  TInt /*aContent*/, RFile& /*aFile*/, TInt /*aIndex*/ )
	{
    // TODO: get content from aFile.

    return 0;
	}

// ----------------------------------------------------------------------------
//
//
//
// ----------------------------------------------------------------------------
//
TInt CAiFwTestControllerPlugin2::Clean( MAiPropertyExtension& /*aPlugin*/, TInt /*aContent*/, TInt /*aIndex*/ )
	{
        // TODO: clean content.
	
    return 0;
	}

// ----------------------------------------------------------------------------
//
//
//
// ----------------------------------------------------------------------------
//
TAny* CAiFwTestControllerPlugin2::Extension( TUid /*aUid*/ )
	{
    return NULL;
	}

// ----------------------------------------------------------------------------
//
//
//
// ----------------------------------------------------------------------------
//
TBool CAiFwTestControllerPlugin2::RequiresSubscription( const TAiPublisherInfo& aPublisherInfo ) const
	{
	return EFalse;
	}

// ----------------------------------------------------------------------------
//
//
//
// ----------------------------------------------------------------------------
//
void CAiFwTestControllerPlugin2::LoadUIDefinitionL()
	{
	iDefinitionLoaded = ETrue;
	}

// ----------------------------------------------------------------------------
//
//
//
// ----------------------------------------------------------------------------
//
void CAiFwTestControllerPlugin2::GetPluginsL( RAiPublisherInfoArray& /*aPlugins */)
	{
	}
	
void CAiFwTestControllerPlugin2::GetSettingsL(const TAiPublisherInfo& /*aPubInfo*/, RAiSettingsItemArray& /*aSettings*/)
{
}

// ----------------------------------------------------------------------------
//
//
//
// ----------------------------------------------------------------------------
//
void CAiFwTestControllerPlugin2::RunApplicationL()
	{
	}

// ----------------------------------------------------------------------------
//
//
//
// ----------------------------------------------------------------------------
//
void CAiFwTestControllerPlugin2::ActivateUI()
	{
	iUiActivated = ETrue;
	}
	
// ----------------------------------------------------------------------------
//
//
//
// ----------------------------------------------------------------------------
//
MAiContentObserver& CAiFwTestControllerPlugin2::GetContentObserver()
	{
    return *this;
	}

// ----------------------------------------------------------------------------
//
//
//
// ----------------------------------------------------------------------------
//
void CAiFwTestControllerPlugin2::SetText( const TDesC8& /*aId*/, TPtrC8& /*aBuf*/ )
	{
	}

// ----------------------------------------------------------------------------
//
//
//
// ----------------------------------------------------------------------------
//
const TDesC* CAiFwTestControllerPlugin2::GetText( const TDesC8& /*aId*/ )
	{
    return NULL;
	}
	
void CAiFwTestControllerPlugin2::SetEventHandler(MAiFwEventHandler& aEventHandler)
    {
    iFwEventHandler = &aEventHandler;
    }

// ========== OTHER EXPORTED FUNCTIONS ========================================

// End of File.
