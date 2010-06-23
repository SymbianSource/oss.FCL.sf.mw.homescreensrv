/*
 * Copyright (c) 2008 Nokia Corporation and/or its subsidiary(-ies).
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
 * Description:  Used for receive SIM Application name, icon or visibility information.
 *
 */

// INCLUDE FILES

#include <e32property.h>
#include <bitdev.h>

#include "ca2internalCRkeys.h"
#include "casathandler.h"


// ============================ MEMBER FUNCTIONS ===============================
// -----------------------------------------------------------------------------
// CCaSatHandler::NewL
// Two-phased constructor.
// -----------------------------------------------------------------------------
//
EXPORT_C CCaSatHandler* CCaSatHandler::NewL()
    {
    CCaSatHandler* self = new ( ELeave ) CCaSatHandler();
    CleanupStack::PushL( self );
    self->ConstructL();
    CleanupStack::Pop( self );
    return self;
    }

// -----------------------------------------------------------------------------
// Destructor
// -----------------------------------------------------------------------------
CCaSatHandler::~CCaSatHandler()
    {
    iSatIcon.Close();
    iSatSession.Close();
    }

// -----------------------------------------------------------------------------
// Destructor
// -----------------------------------------------------------------------------
EXPORT_C CAknIcon* CCaSatHandler::LoadIconL()
    {
    TInt iconId( KErrNone );
    User::LeaveIfError( RProperty::Get( KCRUidCa, KCaSatUIIconId, iconId ) );
    CAknIcon* icon = CAknIcon::NewL();
    CleanupStack::PushL( icon );
    if( iconId != KErrNone )
        {
        RIconEf iIconEf;
        iSatIcon.GetIconInfoL( TUint8( iconId ), iIconEf );
        CleanupClosePushL( iIconEf );
        CFbsBitmap* bitmap = GetBitmapL( iIconEf );
        if( !bitmap )
            {
            CFbsBitmap* mask( NULL );
            CleanupStack::PushL( mask );

            icon->SetBitmap( bitmap );
            // create and set mask
            User::LeaveIfError( mask->Create( bitmap->SizeInPixels(),
                    EGray256 ) );

            CFbsBitmapDevice* maskDevice = CFbsBitmapDevice::NewL( mask );
            CleanupStack::PushL( maskDevice );
            CFbsBitGc* maskGc;
            User::LeaveIfError( maskDevice->CreateContext( maskGc ) );
            CleanupStack::PushL( maskGc );
            maskGc->SetBrushStyle( CGraphicsContext::ESolidBrush );
            maskGc->SetDrawMode( CGraphicsContext::EDrawModePEN );
            maskGc->SetBrushColor( KRgbBlack );
            maskGc->Clear();
            maskGc->SetBrushColor( KRgbWhite );
            maskGc->DrawRect( TRect( TPoint(), bitmap->SizeInPixels() ) );
            icon->SetMask( mask );

            CleanupStack::PopAndDestroy( maskGc );
            CleanupStack::PopAndDestroy( maskDevice );
            CleanupStack::Pop( mask );
            }
        CleanupStack::PopAndDestroy( &iIconEf ); // iIconEf
        CleanupStack::Pop( icon );
        }
    else
        {
        CleanupStack::PopAndDestroy( icon );
        icon = NULL;
        }
    return icon;
    }

// ---------------------------------------------------------------------------
// CCaSatHandler::GetName
// ---------------------------------------------------------------------------
//
EXPORT_C TInt CCaSatHandler::GetName( TDes& aName )
    {
    return RProperty::Get( KCRUidCa, KCaSatUIName, aName );
    }

// ---------------------------------------------------------------------------
// CCaSatHandler::GetVisibility
// ---------------------------------------------------------------------------
//
EXPORT_C TBool CCaSatHandler::CheckVisibility()
    {
    TInt visibility( KErrNone );
    TInt err = RProperty::Get( KCRUidCa, KCaShowSatUI, visibility );
    if( err == KErrNone && visibility )
        return ETrue;
    else
        return EFalse;
    }

// -----------------------------------------------------------------------------
// CCaSatHandler::CMcsSatHandler
// C++ default constructor can NOT contain any code, that
// might leave.
// -----------------------------------------------------------------------------
//
CCaSatHandler::CCaSatHandler()
    {
    }

// -----------------------------------------------------------------------------
// CCaSatHandler::ConstructL
// Symbian 2nd phase constructor can leave.
// -----------------------------------------------------------------------------
//
void CCaSatHandler::ConstructL()
    {
    iSatSession.ConnectL();
    iSatIcon.OpenL( iSatSession );
    }

#ifdef COVERAGE_MEASUREMENT
#pragma CTC SKIP
#endif //COVERAGE_MEASUREMENT (Icon can be created only from SIM)
// ---------------------------------------------------------------------------
// CCaSatHandler::GetVisibility
// ---------------------------------------------------------------------------
//
CFbsBitmap* CCaSatHandler::GetBitmapL( const RIconEf& aIconEF )
    {
    TInt selectedIconIndex( KErrNotFound );
    TSize selectedIconSize( 0, 0 );
    CFbsBitmap* bitmap( NULL );
    for( TInt i = 0; i < aIconEF.Count(); ++i )
        {
        if( ( aIconEF[i].IconSize().iHeight * aIconEF[i].IconSize().iWidth )
                >= ( selectedIconSize.iHeight * selectedIconSize.iWidth ) )
            if( bitmap )
                {
                delete bitmap;
                bitmap = NULL;
                }
        // test and select index of iIcon which is not too big
        TRAPD( bitmapErr, bitmap = iSatIcon.GetIconL( aIconEF[ i ] ) );
        if( !bitmapErr && bitmap ) //!iBitmap if iIcon is too big
            {
            selectedIconSize = aIconEF[i].IconSize();
            selectedIconIndex = i;
            }
        else if( bitmapErr )
            {
            User::Leave( bitmapErr );
            }
        }
    if( selectedIconIndex != KErrNotFound )
        {
        if( bitmap )
            {
            delete bitmap;
            bitmap = NULL;
            }
        TRAPD( bitmapErr, bitmap = 
                iSatIcon.GetIconL( aIconEF[ selectedIconIndex ] ) );
        User::LeaveIfError( bitmapErr );
        return bitmap;
        }
    else
        {
        return NULL;
        }
    }
#ifdef COVERAGE_MEASUREMENT
#pragma CTC ENDSKIP
#endif //COVERAGE_MEASUREMENT

//  End of File
