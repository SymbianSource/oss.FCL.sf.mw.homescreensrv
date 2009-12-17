/*
* Copyright (c) 2005-2008 Nokia Corporation and/or its subsidiary(-ies).
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
* Description:  Class for new message shortcut target
*
*/


#include <sendui.h>         // For CSendUi
#include <sendnorm.rsg>     // For settings not ok - error dialog
#include <gulicon.h>        // For CGulIcon
#include <SenduiMtmUids.h>
#include <cemailaccounts.h> // For finding out available email account counts
#include <AknsUtils.h>      // For AknsUtils

#include <avkon.rsg>

#include <data_caging_path_literals.hrh>
#include <AknGlobalNote.h>  // For error note
#include <MuiuMsvUiServiceUtilities.h>
#include <StringLoader.h>

#include "aiscutcontentmodel.h"
#include "caiscuttargetnewmsg.h"
#include "caiscutengine.h"
#include <aiscutplugin.mbg>
#include <e32property.h>
#include <connect/sbdefs.h>
#include <filemanagerbkupchecker.rsg>
#include <aiscuttexts.rsg>

#include "debug.h"

using namespace conn;
const TInt KMaxBufSize = 256;

// Status keys adopted from FileManager to be used when checking file backup status
const TUid KPSUidFileManagerStatus = { 0x101F84EB }; // File Manager SID
const TUint32 KFileManagerBkupStatus = 0x00000001;

enum TFileManagerBkupStatusType
    {
    EFileManagerBkupStatusUnset   = 0x00000000,
    EFileManagerBkupStatusBackup  = 0x00000001,
    EFileManagerBkupStatusRestore = 0x00000002
    };

TBool PhoneIsInBackupOrRestoreMode()
    {
    TBool backupOrRestore = EFalse;
    
    TInt status( EFileManagerBkupStatusUnset );
    TInt err( RProperty::Get( KPSUidFileManagerStatus, KFileManagerBkupStatus, status ) );
    if ( status == EFileManagerBkupStatusBackup )
    {
    	backupOrRestore = ETrue;
    	return backupOrRestore;
    }

    // Get the back-up restore key, return EFalse if we can't get the key
    TInt keyVal = 0;
    const TInt error = RProperty::Get( KUidSystemCategory, conn::KUidBackupRestoreKey, keyVal );
    if( error )
        {
        return backupOrRestore;
        }

    const conn::TBURPartType partType = static_cast< conn::TBURPartType >( keyVal & conn::KBURPartTypeMask );
    if  (keyVal != 0)
        {
        switch(partType)
            {
        case EBURUnset:
        case EBURNormal:
            break;
        case EBURBackupFull:
        case EBURBackupPartial:
        case EBURRestoreFull:
        case EBURRestorePartial:
            backupOrRestore = ETrue;
            break;
            }
        }
    //

    return backupOrRestore;
    }

// ======== MEMBER FUNCTIONS ========

// ---------------------------------------------------------------------------
//
// ---------------------------------------------------------------------------
//
CAiScutTargetNewMsg::CAiScutTargetNewMsg(CAiScutEngine& aEngine, TShortcutType aType)
    : CAiScutTarget(aEngine, aType)
{
}


// ---------------------------------------------------------------------------
//
// ---------------------------------------------------------------------------
//
void CAiScutTargetNewMsg::ConstructL(const TAiScutParser& aParser)
{
    iDefinition = aParser.Get(EScutDefComplete).AllocL();

    iAppUid       = KNullUid;
    iViewUid.iUid = -1;

    switch (aParser.Type())
    {
    case EScutNewMessage:
        iMtm          = KSenduiMtmUniMessageUid;
        iAppUid.iUid  = KScutUnifiedEditorUidValue;
        break;

    case EScutNewEmail:
        iMtm          = KSenduiMtmSmtpUid;
        iAppUid.iUid  = KScutEmailEditorUidValue;
        break;

#ifdef __SYNCML_DS_EMAIL
    case EScutNewSyncMLMail:
        iMtm          = KSenduiMtmSyncMLEmailUid;
        iAppUid.iUid  = KScutEmailEditorUidValue; // check that these uids are in sync with aiscuttexts.rss
        iViewUid.iUid = KScutSyncMlEmailUidValue;
        break;
#endif

    case EScutNewPostcard:
        iMtm          = KSenduiMtmPostcardUid;
        iAppUid.iUid  = KScutPostcardEditorUidValue;
        break;

    case EScutNewAudioMsg:
        iMtm          = KSenduiMtmAudioMessageUid;
        iAppUid.iUid  = KScutAmsEditorUidValue;
        break;

    case EScutNewMsgType:
        iAppUid.iUid  = KScutMessagingCenterUidValue; // check that these uids are in sync with aiscuttexts.rss
        iViewUid.iUid = KScutMessagingCenterUidValue;
        // fallthrough
    default:
        iMtm = KNullUid;
        break;
    }
}


// ---------------------------------------------------------------------------
//
// ---------------------------------------------------------------------------
//
CAiScutTargetNewMsg* CAiScutTargetNewMsg::NewL(
    CAiScutEngine& aEngine, TShortcutType aType, const TAiScutParser& aParser)
{
    CAiScutTargetNewMsg* self = new (ELeave) CAiScutTargetNewMsg(aEngine, aType);

    CleanupStack::PushL(self);
    self->ConstructL(aParser);
    CleanupStack::Pop(self);

    return self;
}


// ---------------------------------------------------------------------------
//
// ---------------------------------------------------------------------------
//
CAiScutTargetNewMsg::~CAiScutTargetNewMsg()
{
    delete iCaption;
    delete iShortCaption;
    delete iDefinition;

}


// ---------------------------------------------------------------------------
// Returns the shortcut definition string.
// ---------------------------------------------------------------------------
//
TPtrC CAiScutTargetNewMsg::Definition() const
{
    return TPtrC(*iDefinition);
}


// ---------------------------------------------------------------------------
// Returns the shortcut target caption.
// ---------------------------------------------------------------------------
//
TInt CAiScutTargetNewMsg::GetCaption(TPtrC& aDes, TAiScutAppTitleType aTitleType) const
{
    TRAP_IGNORE(GetCaptionL(aTitleType));

    if (aTitleType == EAiScutSkeyTitle)
    {
        aDes.Set(iShortCaption ? *iShortCaption : KNullDesC());
    }
    else
    {
        aDes.Set(iCaption ? *iCaption : KNullDesC());
    }

    return 0;
}

// ---------------------------------------------------------------------------
// Returns the shortcut target caption.
// ---------------------------------------------------------------------------
//
void CAiScutTargetNewMsg::GetCaptionL(TAiScutAppTitleType aTitleType) const
{
    HBufC* titlePtr = HBufC::NewLC(KMaxBufSize);
    TPtr titlePtrP = titlePtr->Des();


    // Use lazy evaluation, create the caption only when it is first needed.
    if (aTitleType == EAiScutSkeyTitle)
    {
        if (!iShortCaption)
        {
            if (iEngine.GetAppTitle(iAppUid, iViewUid, titlePtrP, aTitleType))
            {
                iShortCaption = titlePtrP.AllocL();
            }
        }
    }
    else
    {
        if (!iCaption)
        {
            if (iEngine.GetAppTitle(iAppUid, iViewUid, titlePtrP, aTitleType))
            {
                iCaption = titlePtrP.AllocL();
            }
        }
    }

    CleanupStack::PopAndDestroy(titlePtr);
}

// ---------------------------------------------------------------------------
// Returns the shortcut target icon.
// ---------------------------------------------------------------------------
//
TInt CAiScutTargetNewMsg::GetIcon(CGulIcon*& aIcon) const
{
    if ( CAiScutTarget::GetIcon(aIcon) != KErrNone )
        {
        TRAP_IGNORE(GetIconL(aIcon));
        }


    return 0;
}

// ---------------------------------------------------------------------------
// Returns the shortcut target icon.
// ---------------------------------------------------------------------------
//
void CAiScutTargetNewMsg::GetIconL(CGulIcon*& aIcon) const
{

    CGulIcon* tempIcon = NULL;
    TBool useAppIcon = ETrue;
    TInt iconId = 0;
    TInt maskId = 0;

    if (iMtm == KNullUid)
    {
        useAppIcon = EFalse;
        iconId = EMbmAiscutpluginQgn_menu_mce_sel_mes;
        maskId = EMbmAiscutpluginQgn_menu_mce_sel_mes_mask;
    }
#ifdef __SYNCML_DS_EMAIL
    else if (iMtm.iUid == KSenduiMtmSyncMLEmailUidValue)
    {
        useAppIcon = EFalse;
        iconId = EMbmAiscutpluginQgn_menu_mce_syncmail;
        maskId = EMbmAiscutpluginQgn_menu_mce_syncmail_mask;
    }
#endif

    MAknsSkinInstance* skin = AknsUtils::SkinInstance();

    if (useAppIcon)
    {
        CFbsBitmap* bitmap = NULL;
        CFbsBitmap* mask   = NULL;

        AknsUtils::CreateAppIconLC(skin, iAppUid, EAknsAppIconTypeList, bitmap, mask);
        tempIcon = CGulIcon::NewL(bitmap, mask);
        CleanupStack::Pop(2); // Bitmap and mask. They have to be popped out by number
                              // because the order in which they are pushed in is undefined.
    }
    else
    {
        TFileName pluginIconFile(KDC_APP_BITMAP_DIR);
        pluginIconFile.Append(KBitmapFile);

        tempIcon = AknsUtils::CreateGulIconL(
            skin,
            KAknsIIDQgnPropAiShortcut,
            pluginIconFile,
            iconId,
            maskId
            );
    }

    //Do not need to sets the bitmap and mask to be owned externally


    aIcon = tempIcon;
}


// -----------------------------------------------------------------------------
// Checks if the shortcut target is accessible.
// -----------------------------------------------------------------------------
//
TBool CAiScutTargetNewMsg::IsAccessibleL(TInt /*aCheckType*/)
{
    return ETrue;
}


// -----------------------------------------------------------------------------
// Launches the message editor to send an Sms, Mms or Email message.
// -----------------------------------------------------------------------------
//
void CAiScutTargetNewMsg::LaunchL()
    {
    if( PhoneIsInBackupOrRestoreMode() )
        {
        CAknGlobalNote* note = CAknGlobalNote::NewLC();

        HBufC* prompt = StringLoader::LoadLC( R_QTN_AI_SCUT_OPERATION_DISABLED );

        note->SetSoftkeys( R_AVKON_SOFTKEYS_OK_EMPTY );
        note->ShowNoteL( EAknGlobalInformationNote, *prompt );

        CleanupStack::PopAndDestroy( prompt );
        CleanupStack::PopAndDestroy( note );
        return;
        }

  CSendUi* sendUi = CSendUi::NewLC();

    if (iMtm == KNullUid)
        {
        TSendingCapabilities capabs(0, 0, TSendingCapabilities::ESupportsEditor);

        TUid uid = sendUi->ShowTypedQueryL(CSendUi::EWriteMenu, NULL, capabs, NULL, KNullDesC);
        if (uid != KNullUid)
            {
            sendUi->ServiceCapabilitiesL(uid, capabs);
            sendUi->CreateAndSendMessageL(uid, NULL, KNullUid, EFalse); // launch standalone
            }
        }
    else
        {
        if( iMtm == KSenduiMtmSmtpUid ) // pop, imap, smtp
            {
            RArray<TPopAccount> popAccounts;
            RArray<TImapAccount> imapAccounts;
            RArray<TSmtpAccount> smtpAccounts;
            CEmailAccounts* emailAccounts = CEmailAccounts::NewLC();
            // check that mailbox exists or else display error message
            emailAccounts->GetPopAccountsL(popAccounts);
            emailAccounts->GetImapAccountsL(imapAccounts);
            emailAccounts->GetSmtpAccountsL(smtpAccounts);
            CleanupStack::PopAndDestroy(emailAccounts);

            if( iMtm == KSenduiMtmSmtpUid &&
                (popAccounts.Count() + imapAccounts.Count() + smtpAccounts.Count() ) > 0 )
                {
                sendUi->CreateAndSendMessageL(iMtm, NULL, KNullUid, EFalse); // launch standalone
                }
            else
                {
                ShowErrorNote();
                }
            popAccounts.Reset();
            imapAccounts.Reset();
            smtpAccounts.Reset();
            }
        else if ( iMtm == KSenduiMtmSyncMLEmailUid ) // syncml
            {
            CMsvEntrySelection* sel =
                MsvUiServiceUtilities::GetListOfAccountsWithMTML( *(iEngine.MsvSession()), iMtm );
            TInt accounts = sel->Count();
            delete sel;
            if ( accounts > 0 )
                {
                sendUi->CreateAndSendMessageL(iMtm, NULL, KNullUid, EFalse); // launch standalone
                }
            else
                {
                ShowErrorNote();
                }
            }
        else
            {
            sendUi->CreateAndSendMessageL(iMtm, NULL, KNullUid, EFalse); // launch standalone
            }

    }
    CleanupStack::PopAndDestroy(sendUi);
}

void CAiScutTargetNewMsg::ShowErrorNote()
    {
     TRAP_IGNORE(
        // Display global error note.
        CAknGlobalNote* note = CAknGlobalNote::NewLC();
        HBufC* prompt = StringLoader::LoadLC( R_SENDUI_SETTINGS_NOT_OK );
        note->ShowNoteL( EAknGlobalErrorNote, *prompt );
        CleanupStack::PopAndDestroy( prompt );
        CleanupStack::PopAndDestroy( note );
    ); // end TRAP_IGNORE
    }

// ---------------------------------------------------------------------------
// Return application uid this target launches.
// ---------------------------------------------------------------------------
//
TUid CAiScutTargetNewMsg::AppUid() const
{
    return iAppUid;
}

// End of File.
