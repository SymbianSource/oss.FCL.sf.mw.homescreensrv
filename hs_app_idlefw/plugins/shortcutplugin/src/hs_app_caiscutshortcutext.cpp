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
* Description:  Shortcut plug-in shortcut extensions
*
*/


#include <aicontentobserver.h>
#include <gulicon.h>
#include <fbs.h>
#include <e32property.h>

#include "hs_app_caiscutshortcutext.h"
#include "aiscutcontentmodel.h"
#include "hs_app_caiscuttarget.h"
#include "hs_app_caiscutengineext.h"
#include "hs_app_PopupFSM.h"
#include "hs_app_cpopupeventhandler.h"
#include "hs_app_maiscutextdata.h"
#include "activeidle2domainpskeys.h"

#include "debug.h"


// P&S access policies
_LIT_SECURITY_POLICY_C1( KAiScutReadPolicy, ECapabilityReadDeviceData );
_LIT_SECURITY_POLICY_C1( KAiScutWritePolicy, ECapabilityWriteDeviceData );


// ======== MEMBER FUNCTIONS ========

// ---------------------------------------------------------------------------
//
// ---------------------------------------------------------------------------
//
CAiScutShortcutExt* CAiScutShortcutExt::NewL(
    TInt aId, const TDesC& aTarget, CAiScutEngine& aEngine )
    {
    CAiScutShortcutExt* self = CAiScutShortcutExt::NewLC(
        aId, aTarget, aEngine );
    CleanupStack::Pop( self );
    return self;
    }

// ---------------------------------------------------------------------------
//
// ---------------------------------------------------------------------------
//
CAiScutShortcutExt* CAiScutShortcutExt::NewLC(
    TInt aId, const TDesC& aTarget, CAiScutEngine& aEngine )
    {
    CAiScutShortcutExt* self = new( ELeave ) CAiScutShortcutExt(
        aId, aEngine );
    CleanupStack::PushL( self );
    self->ConstructL( aTarget );
    return self;
    }

// ---------------------------------------------------------------------------
//
// ---------------------------------------------------------------------------
//
CAiScutShortcutExt::~CAiScutShortcutExt()
    {
    delete iPopupEventHandler;
    }

// ---------------------------------------------------------------------------
//
// ---------------------------------------------------------------------------
//
CAiScutShortcutExt::CAiScutShortcutExt( TInt aId, CAiScutEngine& aEngine )
    : CAiScutShortcut( aId, aEngine )//,
    //iPublishLineArray( EFalse )
    {
    }

// ---------------------------------------------------------------------------
//
// ---------------------------------------------------------------------------
//
void CAiScutShortcutExt::ConstructL( const TDesC& aTarget )
    {
    iPopupEventHandler = CPopupEventHandler::NewL( *this );
    CAiScutShortcut::ConstructL( aTarget );

    RProperty::Define(
        KPSUidAiInformation,
        KActiveIdleThemeSupportsXsp,
        RProperty::EInt,
        KAiScutReadPolicy,
        KAiScutWritePolicy );
    RProperty::Set( KPSUidAiInformation, KActiveIdleThemeSupportsXsp, EPSAiXspNotSupported );
    }


// ---------------------------------------------------------------------------
//
// ---------------------------------------------------------------------------
//
TPtrC CAiScutShortcutExt::ActiveTargetDefinition() const
    {
    if( iActiveTarget )
        {
        return iActiveTarget->Definition();
        }
    return KNullDesC();
    }

// ---------------------------------------------------------------------------
//
// ---------------------------------------------------------------------------
//
void CAiScutShortcutExt::HandleAIEventL( TInt aEvent )
    {
    switch ( aEvent )
        {
        case EAiScutEventLoseFocus:
            {
            iPopupEventHandler->PopupFSM().HandleLostFocus();
            break;
            }
        case EAiScutEventGainFocus:
            {
            iPopupEventHandler->PopupFSM().HandleGotFocus();
            break;
            }
        default:
            {
            break;
            }
        }
    }

// ---------------------------------------------------------------------------
//
// ---------------------------------------------------------------------------
//
void CAiScutShortcutExt::HandleResumed( TAiTransitionReason aResumeReason )
    {
    switch( aResumeReason )
        {
        case EAiIdleBackground:
            {
            iPopupEventHandler->PopupFSM().HandleBackground();
            break;
            }
        case EAiIdleForeground:
            {
            // Handleforeground() calls unnecessary publishes in case nothing
            // has changed. 
            if ( iTextChanged || iIconChanged )
                {
                iPopupEventHandler->PopupFSM().HandleForeground();
                }
            break;
            }
        default:
            {
            break;
            }
        }
    }

// ---------------------------------------------------------------------------
//
// ---------------------------------------------------------------------------
//
void CAiScutShortcutExt::SetAiScutExtData(
    const MAiScutExtData* aAiScutExtData )
    {
    iAiScutExtData = aAiScutExtData;

    const MDesCArray* newPopupLineArray =
        aAiScutExtData ? aAiScutExtData->PopupLineArray() : NULL;
    const CGulIcon* newIcon =
        aAiScutExtData ? aAiScutExtData->Icon() : NULL;
        
    if( iPreviousPopupLineArray != newPopupLineArray &&
        newPopupLineArray )
        {
        iTextChanged = ETrue;
        iPopupEventHandler->PopupFSM().HandleUpdate();        
        }
    else if( iPreviousPopupLineArray && !newPopupLineArray )
        {
        iTextChanged = ETrue;
        iPopupEventHandler->PopupFSM().HandleReset();
        }

    if( iPreviousIcon != newIcon )
        {
        iIconChanged = ETrue;
        CAiScutEngineExt& engine = static_cast< CAiScutEngineExt& > ( iEngine );
        engine.CheckAccessAndPublish( *this );
        }

    iPreviousPopupLineArray = newPopupLineArray;
    iPreviousIcon = newIcon;
    }

// ---------------------------------------------------------------------------
//
// ---------------------------------------------------------------------------
//
void CAiScutShortcutExt::PublishL(
    MAiPropertyExtension& aPlugin, MAiContentObserver& aObserver )
    {
    if( iLastPublishedTarget && iActiveTarget &&
        iLastPublishedTarget->Definition() != iActiveTarget->Definition() )
        {
        iAiScutExtData = NULL;
        iPopupEventHandler->PopupFSM().HandleReset();
        }

    if( aObserver.CanPublish(
        aPlugin, KAiScutContent[EAiScutContentPopupTextFirstLine].id, iId) &&
        aObserver.CanPublish(
        aPlugin, KAiScutContent[EAiScutContentPopupTextSecondLine].id, iId) &&
        aObserver.CanPublish(
        aPlugin, KAiScutContent[EAiScutContentPopupTextThirdLine].id, iId) )
        {
        PublishPopupText( aPlugin, aObserver );
        RProperty::Set( KPSUidAiInformation, KActiveIdleThemeSupportsXsp, EPSAiXspIsSupported );
        }

    CAiScutShortcut::PublishL( aPlugin, aObserver );
    iTextChanged = EFalse;
    }

// ---------------------------------------------------------------------------
//
// ---------------------------------------------------------------------------
//
TInt CAiScutShortcutExt::PublishCaption(
    MAiPropertyExtension& aPlugin, MAiContentObserver& aObserver,
    TInt aCaptionContentId ) const
    {
    if( iPopupEventHandler->PublishCaption() )
        {
        if( !iPopupEventHandler->CaptionVisible() )
            {
            return aObserver.Clean( aPlugin, aCaptionContentId, iId );
            }
        else
            {
            return CAiScutShortcut::PublishCaption(
                aPlugin, aObserver, aCaptionContentId );
            }
        }
    else
        {
        return CAiScutShortcut::PublishCaption(
            aPlugin, aObserver, aCaptionContentId );
        }
    }

// ---------------------------------------------------------------------------
//
// ---------------------------------------------------------------------------
//
void CAiScutShortcutExt::PublishPopupText(
    MAiPropertyExtension& aPlugin, MAiContentObserver& aObserver ) const
    {
    if( iPopupEventHandler->PublishPopup() )
        {
        TAiScutContentIds popupTextContentIds[] =
            {
            EAiScutContentPopupTextFirstLine,
            EAiScutContentPopupTextSecondLine,
            EAiScutContentPopupTextThirdLine
            };
        const TInt idCount =
            sizeof( popupTextContentIds ) / sizeof( TAiScutContentIds );

        TInt err = KErrNone;

        //iLineArray is set or reset via xSP API. iPublishLineArray is controlled
        //by state machine in iPopupEventHandler
        if( iAiScutExtData && iAiScutExtData->PopupLineArray() &&
            iPopupEventHandler->PopupVisible() )
            {
            //Publish caption line in popup text box
            TPtrC captionDes;
            TInt captionResId = iActiveTarget->GetCaption( captionDes, EAiScutLongTitle );

            if ( captionResId == 0 )
                {
                // Publish descriptor
                err = aObserver.Publish( aPlugin,
                    KAiScutContent[ EAiScutContentPopupTextCaptionLine ].id,
                    captionDes, iId );
                if( err != KErrNone )
                    {
                    __PRINT( __DBG_FORMAT( "XAI:   publish Popup caption text err = %d" ), err );
                    }
                }
            else if ( captionResId > 0 )
                {
                // Publish resource
                err = aObserver.Publish( aPlugin,
                    KAiScutContent[ EAiScutContentPopupTextCaptionLine ].id,
                    captionResId, iId );
                if( err != KErrNone )
                    {
                    __PRINT( __DBG_FORMAT( "XAI:   publish Popup caption text (resource) err = %d" ), err );
                    }
                }

            const MDesCArray& lineArray = *iAiScutExtData->PopupLineArray();
            TInt numberOfLines = lineArray.MdcaCount();
            //Iterate each popup text content and either publish or clean it
            //depending on the number or lines in iLineArray
            for( TInt i = 0; i < idCount; i++ )
                {
                if( numberOfLines > i )
                    {
                    err = aObserver.Publish( aPlugin,
                        KAiScutContent[ popupTextContentIds[ i ] ].id,
                        lineArray.MdcaPoint( i ), iId );
                    if( err != KErrNone )
                        {
                        __PRINT( __DBG_FORMAT( "XAI:   publish Popup text err = %d" ), err );
                        }
                    }
                else
                    {
                    err = aObserver.Clean( aPlugin,
                        KAiScutContent[ popupTextContentIds[ i ] ].id, iId );
                    if( err != KErrNone )
                        {
                        __PRINT( __DBG_FORMAT( "XAI:   clean Popup text err = %d" ), err );
                        }
                    }
                }
            }
        else
            {
            //Clean each popup text content
            for( TInt i = 0; i < idCount; i++ )
                {
                err = aObserver.Clean( aPlugin,
                    KAiScutContent[ popupTextContentIds[ i ] ].id, iId );
                if ( err != KErrNone )
                    {
                    __PRINT( __DBG_FORMAT( "XAI:   clean Popup text err = %d" ), err );
                    }
                }
            }
        }
    }

// ---------------------------------------------------------------------------
//
// ---------------------------------------------------------------------------
//
TInt CAiScutShortcutExt::PublishIcon( MAiPropertyExtension& aPlugin,
    MAiContentObserver& aObserver, TInt aIconContentId )
    {
    if( aIconContentId == KAiScutContent[EAiScutContentShortcutIcon].id && 
		iAiScutExtData && iAiScutExtData->Icon() )
        {
        const CGulIcon& icon = *iAiScutExtData->Icon();
        CGulIcon* duplicatedIcon = NULL;
        TRAP_IGNORE(
            CFbsBitmap* bitmap = new( ELeave ) CFbsBitmap();
            CleanupStack::PushL( bitmap );
            CFbsBitmap* mask   = new( ELeave ) CFbsBitmap();
            CleanupStack::PushL( mask );

            User::LeaveIfError( bitmap->Duplicate( icon.Bitmap()->Handle() ) );
            User::LeaveIfError( mask->Duplicate( icon.Mask()->Handle() ) );

            duplicatedIcon = CGulIcon::NewL( bitmap, mask );

            CleanupStack::Pop( 2, bitmap );
            );

        if( duplicatedIcon )
            {
            TInt err = aObserver.PublishPtr( aPlugin,
                aIconContentId, duplicatedIcon, iId );
            if( err != KErrNone )
                {
                delete duplicatedIcon;
                }
            return err;
            }
        }
    
    TInt err = CAiScutShortcut::PublishIcon( aPlugin, aObserver, aIconContentId );
    iIconChanged = EFalse;
    return err;
    }

// ---------------------------------------------------------------------------
//
// ---------------------------------------------------------------------------
//
void CAiScutShortcutExt::IssuePublishShortcut()
    {
//    iPublishLineArray = aVisiblePopup;
    CAiScutEngineExt& engine = static_cast< CAiScutEngineExt& >( iEngine );
    engine.CheckAccessAndPublish( *this );
    }


// End of File.
