/*
* Copyright (c) 2005-2006 Nokia Corporation and/or its subsidiary(-ies).
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
* Description:  Shortcut plug-in engine extension
*
*/


#include "hs_app_caiscutextdata.h"
#include <fbs.h>
#include <gulicon.h>

// ======== LOCAL DEFINITIONS ========

namespace
    {
    }

// ======== MEMBER FUNCTIONS ========

// ---------------------------------------------------------------------------
//
// ---------------------------------------------------------------------------
//
CAiScutExtData* CAiScutExtData::NewL( const TDesC& aTargetDefinition )
    {
    CAiScutExtData* self = CAiScutExtData::NewLC( aTargetDefinition );
    CleanupStack::Pop( self );
    return self;
    }

CAiScutExtData* CAiScutExtData::NewLC( const TDesC& aTargetDefinition )
    {
    CAiScutExtData* self = new( ELeave ) CAiScutExtData;
    CleanupStack::PushL( self );
    self->ConstructL( aTargetDefinition );
    return self;
    }
// ---------------------------------------------------------------------------
//
// ---------------------------------------------------------------------------
//
CAiScutExtData::~CAiScutExtData()
    {
    delete iIcon;
    delete iPopupLineArray;
    delete iTargetDefinition;
    }

// ---------------------------------------------------------------------------
//
// ---------------------------------------------------------------------------
//
CAiScutExtData::CAiScutExtData()
    {
    }

// ---------------------------------------------------------------------------
//
// ---------------------------------------------------------------------------
//
void CAiScutExtData::ConstructL( const TDesC& aTargetDefinition )
    {
    iTargetDefinition = aTargetDefinition.AllocL();
    }

// ---------------------------------------------------------------------------
//
// ---------------------------------------------------------------------------
//
const TDesC& CAiScutExtData::TargetDefinition() const
    {
    return *iTargetDefinition;
    }

// ---------------------------------------------------------------------------
//
// ---------------------------------------------------------------------------
//
void CAiScutExtData::SwitchPopupLineArray( CDesCArray* aPopupLineArray )
    {
    delete iPopupLineArray;
    iPopupLineArray = aPopupLineArray;
    }
    
// ---------------------------------------------------------------------------
//
// ---------------------------------------------------------------------------
//
void CAiScutExtData::ResetPopupLineArray()
    {
    delete iPopupLineArray;
    iPopupLineArray = NULL;
    }
    
// ---------------------------------------------------------------------------
//
// ---------------------------------------------------------------------------
//
void CAiScutExtData::SwitchIcon( CGulIcon* aIcon )
    {
    delete iIcon;
    iIcon = aIcon;
    }
    
// ---------------------------------------------------------------------------
//
// ---------------------------------------------------------------------------
//
void CAiScutExtData::ResetIcon()
    {
    delete iIcon;
    iIcon = NULL;
    }

// ---------------------------------------------------------------------------
//
// ---------------------------------------------------------------------------
//
const MDesCArray* CAiScutExtData::PopupLineArray() const
    {
    return iPopupLineArray;
    }
    
// ---------------------------------------------------------------------------
//
// ---------------------------------------------------------------------------
//
const CGulIcon* CAiScutExtData::Icon() const
    {
    return iIcon;
    }

// ---------------------------------------------------------------------------
//
// ---------------------------------------------------------------------------
//
/*
CGulIcon* CAiScutExtData::DuplicateIconL() const
    {
    if( !iIcon )
        {
        return NULL;
        }

    CGulIcon* newIcon = CGulIcon::NewLC();
    
    CFbsBitmap* iconBitmap = iIcon->Bitmap();
    if( iconBitmap && iconBitmap->Handle() != 0 )
        {
        CFbsBitmap* newBitmap = new( ELeave ) CFbsBitmap();
        CleanupStack::PushL( newBitmap );
        User::LeaveIfError( newBitmap->Duplicate( iconBitmap->Handle() ) );
        CleanupStack::Pop( newBitmap );
        newIcon->SetBitmap( newBitmap );
        }

    CFbsBitmap* iconMask = iIcon->Mask();
    if( iconMask && iconMask->Handle() != 0 )
        {
        CFbsBitmap* newMask = new( ELeave ) CFbsBitmap();
        CleanupStack::PushL( newMask );
        User::LeaveIfError( newMask->Duplicate( iconMask->Handle() ) );
        CleanupStack::Pop( newMask );
        newIcon->SetMask( newMask );
        }

    CleanupStack::Pop( newIcon );
    return newIcon;
    }
*/
    
// End of File.
