/*
* Copyright (c) 2007 Nokia Corporation and/or its subsidiary(-ies).
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
* Description:  Class stores basic information about application
 *
*/


#include <e32base.h>

#include "bsapplicationinfo.h"

// ================= MEMBER FUNCTIONS =======================

// -----------------------------------------------------------------------------
// Constructor
// -----------------------------------------------------------------------------
//
CBSApplicationInfo::CBSApplicationInfo()
    {
    // No implementation required
    }

// -----------------------------------------------------------------------------
// Destructor.
// -----------------------------------------------------------------------------
//
CBSApplicationInfo::~CBSApplicationInfo()
    {
    delete iItemState;
    }

// -----------------------------------------------------------------------------
// Two-phased constructor.
// -----------------------------------------------------------------------------
//
CBSApplicationInfo* CBSApplicationInfo::NewLC()
    {
    CBSApplicationInfo* self = new (ELeave)CBSApplicationInfo();
    CleanupStack::PushL( self );
    self->ConstructL( );
    return self;
    }

// -----------------------------------------------------------------------------
// Two-phased constructor.
// -----------------------------------------------------------------------------
//
CBSApplicationInfo* CBSApplicationInfo::NewL()
    {
    CBSApplicationInfo* self = CBSApplicationInfo::NewLC( );
    CleanupStack::Pop( self );
    return self;
    }

// -----------------------------------------------------------------------------
// Symbian 2nd phase constructor can leave.
// -----------------------------------------------------------------------------
//
void CBSApplicationInfo::ConstructL()
    {
    }

// -----------------------------------------------------------------------------
// 
// -----------------------------------------------------------------------------
//
void CBSApplicationInfo::SetAppUid( const TUid& aApp )
    {
    iAppUid = aApp;
    }

// -----------------------------------------------------------------------------
// 
// -----------------------------------------------------------------------------
//
TUid CBSApplicationInfo::AppUid()
    {
    return iAppUid;
    }

// -----------------------------------------------------------------------------
// 
// -----------------------------------------------------------------------------
//
void CBSApplicationInfo::SetPrevAppUid( const TUid& aApp )
    {
    iPrevAppUid = aApp;
    }

// -----------------------------------------------------------------------------
// 
// -----------------------------------------------------------------------------
//
TUid CBSApplicationInfo::PrevAppUid()
    {
    return iPrevAppUid;
    }

// -----------------------------------------------------------------------------
// 
// -----------------------------------------------------------------------------
//
void CBSApplicationInfo::SetItemStateL( const TDesC8& aState )
    {
    delete iItemState;
    iItemState = NULL;
    iItemState = aState.AllocL( );
    }

// -----------------------------------------------------------------------------
// 
// -----------------------------------------------------------------------------
//
const TDesC8& CBSApplicationInfo::ItemState()
    {
    if ( iItemState )
        return *iItemState;
    else
        return KNullDesC8;
    }
