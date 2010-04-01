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

#include <gslauncher.h>
#include <gulicon.h>
#include <viewcli.h>
#include <coemain.h>

#include <aiscutextdefs.h>

#include "hs_app_caiscutengineext.h"
#include "hs_app_caiscutextserver.h"
#include "hs_app_taiscutparser.h"
#include "hs_app_caiscutshortcut.h"
#include "hs_app_caiscutplugin.h"
#include "aiscutcontentmodel.h"
#include "hs_app_caiscutextdata.h"
#include "hs_app_caiscutextdatamodel.h"

// ======== LOCAL DEFINITIONS ========

namespace
    {
    //  LOCAL CONSTANTS
    /**
     * Target id hex format
     */
    _LIT( KTargetIdFormat, "0x%08x" );

    /**
     * Maximum length of target id
     */
    const TInt KTargetIdMaxLength = 11;
    }

// ======== MEMBER FUNCTIONS ========

// ---------------------------------------------------------------------------
//
// ---------------------------------------------------------------------------
//
CAiScutEngineExt* CAiScutEngineExt::NewL( CAiScutPlugin& aPlugin )
    {
    CAiScutEngineExt* self = new( ELeave ) CAiScutEngineExt( aPlugin );
    CleanupStack::PushL( self );
    self->ConstructL();
    CleanupStack::Pop( self );
    return self;
    }

// ---------------------------------------------------------------------------
//
// ---------------------------------------------------------------------------
//
CAiScutEngineExt::~CAiScutEngineExt()
    {
    delete iAiScutExtServer;
    delete iExtDataModel;
    }

// ---------------------------------------------------------------------------
//
// ---------------------------------------------------------------------------
//
CAiScutEngineExt::CAiScutEngineExt( CAiScutPlugin& aPlugin )
    : CAiScutEngine( aPlugin )
    {
    }

// ---------------------------------------------------------------------------
//
// ---------------------------------------------------------------------------
//
void CAiScutEngineExt::ConstructL()
    {
    CAiScutEngine::ConstructL();

    iExtDataModel = CAiScutExtDataModel::NewL();
    iAiScutExtServer = new( ELeave ) CAiScutExtServer( *this, Env()->WsSession() );
    iAiScutExtServer->ConstructL();
    TRAPD( err, iAiScutExtServer->StartL( KAiScutExtServerName ) );

    if( err != KErrNone && err != KErrAlreadyExists )
        {
        User::Leave( err );
        }
    
    //Send EAiScutEventGainFocus to the leftmost shortcut in order to put
    //its state machine in correct state
    if( iShortcuts.Count() > 0 )
    {
        static_cast< CAiScutShortcutExt* >( iShortcuts[ 0 ] )->HandleAIEventL(
            EAiScutEventGainFocus );
        }
    }

// ---------------------------------------------------------------------------
//
// ---------------------------------------------------------------------------
//
void CAiScutEngineExt::CheckAccessAndPublish( CAiScutShortcut& aShortcut )
    {
    TRAPD( err,
        RAiShortcutArray shortcutArray;
        CleanupClosePushL( shortcutArray );
        shortcutArray.AppendL( &aShortcut );
        aShortcut.SetToBePublished( ETrue );
        iPlugin.PublishShortcutsL( shortcutArray );
        CleanupStack::PopAndDestroy( &shortcutArray );
        );

    if( err != KErrNone )
        {
        //Delegate publishing to the original engine
        CAiScutEngine::CheckAccessAndPublish( EScutCheckAll, ETrue );
        }
    }

// ---------------------------------------------------------------------------
// Handles an event sent by the AI framework.
// ---------------------------------------------------------------------------
//
void CAiScutEngineExt::HandleAiEventL( TInt aEvent, const TDesC& aParam )
    {
    CAiScutShortcutExt* shortcut = FindShortcutById( aParam );
    if( shortcut )
        {
        shortcut->HandleAIEventL( aEvent );
        }
    CAiScutEngine::HandleAiEventL( aEvent, aParam );
    }

// ---------------------------------------------------------------------------
//
// ---------------------------------------------------------------------------
//
void CAiScutEngineExt::ResumeL( TBool aPublishAll, TAiTransitionReason aReason )
    {
    TArray<CAiScutExtData*> extDataArray = iExtDataModel->AiScutExtDataArray();
    const TInt count = extDataArray.Count();
    //Iterate extension data items and populate to matching shortcuts.
    //Remove the items that are not used in any shortcut.
    for( TInt i = 0; i < count; i++ )
        {
        CAiScutExtData* extData = extDataArray[ i ];
        const TDesC& targetDefinition = extData->TargetDefinition();
        if( !PopulateExtData( targetDefinition, extData ) )
            {
            iExtDataModel->RemoveAiScutExtData( targetDefinition );
            }
        }

    TAiTransitionReason resumeReason = iPlugin.ResumeReason();
    const TInt shortcutCount = iShortcuts.Count();
    for( TInt i = 0; i < shortcutCount; i++ )
        {
        CAiScutShortcutExt* shortcut = static_cast< CAiScutShortcutExt* >
            ( iShortcuts[ i ] );
        shortcut->HandleResumed( resumeReason );
        }

    iAiScutExtServer->ResumeL( aReason );

    CAiScutEngine::ResumeL( aPublishAll, aReason );
    }

// ---------------------------------------------------------------------------
//
// ---------------------------------------------------------------------------
//
void CAiScutEngineExt::HandleSetPopupLineArrayL( const TDesC& aDefinition,
    CDesCArray* aLineArray )
    {
    CAiScutExtData* extData = iExtDataModel->SetPopupLineArrayL(
        aDefinition, aLineArray );
    TBool shortcutsFound = PopulateExtData(
        aDefinition, extData );
    if( !shortcutsFound )
        {
        iExtDataModel->RemoveAiScutExtData( aDefinition );
        }
    }

// ---------------------------------------------------------------------------
//
// ---------------------------------------------------------------------------
//
void CAiScutEngineExt::HandleResetPopupLineArrayL( const TDesC& aDefinition )
    {
    CAiScutExtData* extData = iExtDataModel->ResetPopupLineArray( aDefinition );
    TBool shortcutsFound = PopulateExtData(
        aDefinition, extData );
    if( !shortcutsFound )
        {
        iExtDataModel->RemoveAiScutExtData( aDefinition );
        }
    }

// ---------------------------------------------------------------------------
//
// ---------------------------------------------------------------------------
//
void CAiScutEngineExt::HandleSetIconL( const TDesC& aDefinition, CGulIcon* aIcon )
    {
    CAiScutExtData* extData = iExtDataModel->SetIconL( aDefinition, aIcon );
    TBool shortcutsFound = PopulateExtData(
        aDefinition, extData );
    if( !shortcutsFound )
        {
        iExtDataModel->RemoveAiScutExtData( aDefinition );
        }
    }

// ---------------------------------------------------------------------------
//
// ---------------------------------------------------------------------------
//
void CAiScutEngineExt::HandleResetIconL( const TDesC& aDefinition )
    {
    CAiScutExtData* extData = iExtDataModel->ResetIcon( aDefinition );
    TBool shortcutsFound = PopulateExtData(
        aDefinition, extData );
    if( !shortcutsFound )
        {
        iExtDataModel->RemoveAiScutExtData( aDefinition );
        }
    }

// ---------------------------------------------------------------------------
//
// ---------------------------------------------------------------------------
//
TBool CAiScutEngineExt::HandleIsInShortcutsL( const TDesC& aDefinition ) const
    {
    const TInt shortcutCount = iShortcuts.Count();
    for( TInt i = 0; i < shortcutCount; i++ )
        {
        CAiScutShortcutExt* shortcut = static_cast< CAiScutShortcutExt* >
            ( iShortcuts[ i ] );
        TPtrC activeTargetDefinition = shortcut->ActiveTargetDefinition();
        if( activeTargetDefinition.CompareF( aDefinition ) == 0 )
            {
            return ETrue;
            }
        }

    return EFalse;
    }

// ---------------------------------------------------------------------------
//
// ---------------------------------------------------------------------------
//
void CAiScutEngineExt::HandleIssuePutInShortcutsL( const TDesC& /*aDefinition*/ )
    {
    CGSLauncher* l = CGSLauncher::NewLC();
    l->LaunchGSViewL ( TUid::Uid( AI_UID_ECOM_IMPLEMENTATION_SETTINGS_SCUTPLUGIN ),
                       KScutActiveIdleUid,
                       KNullDesC8 );    
    CleanupStack::PopAndDestroy( l );
    }

// ---------------------------------------------------------------------------
//
// ---------------------------------------------------------------------------
//
TBool CAiScutEngineExt::PopulateExtData( const TDesC& aDefinition,
    const MAiScutExtData* aAiScutExtData )
    {
    //Iterate shortcuts and update matching ones with new extData
    TBool shortcutsFound = EFalse;
    const TInt shortcutCount = iShortcuts.Count();
    for( TInt i = 0; i < shortcutCount; i++ )
        {
        CAiScutShortcutExt* shortcut = static_cast< CAiScutShortcutExt* >
            ( iShortcuts[ i ] );
        TPtrC activeTargetDefinition = shortcut->ActiveTargetDefinition();
        if( activeTargetDefinition.CompareF( aDefinition ) == 0 )
            {
            shortcut->SetAiScutExtData( aAiScutExtData );
            shortcutsFound = ETrue;
            }
        }

    return shortcutsFound;
    }

// ---------------------------------------------------------------------------
//
// ---------------------------------------------------------------------------
//
CAiScutShortcutExt* CAiScutEngineExt::FindShortcutById(
    const TDesC& aId ) const
    {
    const TInt shortcutCount = iShortcuts.Count();
    for( TInt i = 0; i < shortcutCount; i++ )
        {
        CAiScutShortcutExt* shortcut = static_cast< CAiScutShortcutExt* >
            ( iShortcuts[ i ] );

        TBuf< KTargetIdMaxLength > id;
        id.Format( KTargetIdFormat, shortcut->Id() );

        if( id.CompareF( aId ) == 0 )
            {
            return shortcut;
            }
        }
    return NULL;
    }

// End of File.
