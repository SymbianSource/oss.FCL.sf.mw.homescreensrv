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
* Description:  Class for shortcut setting items
*
*/



#include <StringLoader.h>
#include <centralrepository.h>
#include <msvuids.h>        // For KMsvRootIndexEntryIdValue
#include <SenduiMtmUids.h>
#include <uri16.h>                  // For TUriParser16
#include <aiscutsettingsres.rsg>
#include <aiscuttexts.rsg>
#include "caiscutsettingsitem.h"
#include "caiscutsettingsmodel.h"
#include "aiscutpluginprivatecrkeys.h"
#include "taiscutparser.h"
#include "aiscutdefs.h"
#include "aiscutsettings.hrh"
#include "aidefaultshortcut.h"
#include "debug.h"

const TInt KMaxBufSize = 250;
// ======== MEMBER FUNCTIONS ========
// ---------------------------------------------------------------------------
//
// ---------------------------------------------------------------------------
//

CAiScutSettingsItem::CAiScutSettingsItem(
    CAiScutSettingsModel&   aModel,
    TInt                    aIndex,
    TUint32                 aKey)
    : iModel(aModel)
        , iIndex(aIndex)
        , iKey(aKey)
{
}
// ---------------------------------------------------------------------------
//
// ---------------------------------------------------------------------------
//
void CAiScutSettingsItem::ReadL(CRepository* aRepository)
{
    if( !aRepository )
       {
          return;
       }

    HBufC* buffer = HBufC::NewLC(NCentralRepositoryConstants::KMaxUnicodeStringLength);
    TPtr bufferPtr = buffer->Des();

    // try user key first.
    TUint32 key = iKey & KScutBitMaskThemeDefault;
    TInt err = aRepository->Get(key, bufferPtr);

    if (err == KErrNone)
    {
        err = ParseValueL(bufferPtr);
    }

    if (err == KErrNone)
    {
        iKey = key;
    }
    else
    {
        // if no user key try default key then.
        key = iKey | KScutFlagBitThemeDefault;
        err = aRepository->Get(key, bufferPtr);
        if (err == KErrNone)
        {
            err = ParseValueL(bufferPtr);
        }

    }

    if (err != KErrNone ||
        (iType == EAiScutSettingTypeApplication && iModel.IsHidden(iUid)))
    {
        HBufC* buf = HBufC::NewLC(NCentralRepositoryConstants::KMaxUnicodeStringLength);        
        TPtr bufPtr = buf->Des();
        HBufC* caption = NULL;

        // Get the default shortcut uid for key 'key' 
        iType = EAiScutSettingTypeApplication;
        TAiDefaultShortcut::GetDefaultShortcut(key, iUid, bufPtr);

        err = iModel.GetAppCaption(iUid, bufPtr);
        if (err == KErrNone)
        {
            caption = bufPtr.AllocLC();
            CreateListBoxLineL(*caption);
            CleanupStack::PopAndDestroy(caption);
        }

        CleanupStack::PopAndDestroy(buf);

    }

    delete iValue;
    iValue = NULL;
    iValue = bufferPtr.AllocL();

    CleanupStack::PopAndDestroy(buffer);

 }

 // ---------------------------------------------------------------------------
 //
 // ---------------------------------------------------------------------------
 //
 TInt CAiScutSettingsItem::Save(CRepository* aRepository)
 {

    if( !aRepository )
    {
        return KErrGeneral;
    }

    iKey = iKey & KScutBitMaskThemeDefault;

    TInt err = aRepository->Set(iKey, Value());

    return err;
 }


 // ---------------------------------------------------------------------------
 // Constructs a new settings item leaving it on the cleanup stack.
 // ---------------------------------------------------------------------------
 //
 CAiScutSettingsItem* CAiScutSettingsItem::NewLC(
    CAiScutSettingsModel&   aModel,
    TInt                    aIndex,
    TUint32                 aKey)
 {
    CAiScutSettingsItem* self = new (ELeave) CAiScutSettingsItem(
        aModel, aIndex, aKey);

    CleanupStack::PushL(self);
    self->ConstructL();
    return self;
 }

 // ---------------------------------------------------------------------------
 //
 // ---------------------------------------------------------------------------
 //
 CAiScutSettingsItem* CAiScutSettingsItem::NewLC(
    CAiScutSettingsModel&   aModel,
    TInt                    aIndex,
    TUint32                 aKey,
    const TDesC&            aValue)
 {
    CAiScutSettingsItem* self = new (ELeave) CAiScutSettingsItem(
        aModel, aIndex, aKey);

    CleanupStack::PushL(self);
    self->ConstructL(aValue);
    return self;
 }

 // ---------------------------------------------------------------------------
 //
 // ---------------------------------------------------------------------------
 //
 void CAiScutSettingsItem::ConstructL()
 {

 }

 // ---------------------------------------------------------------------------
 //
 // ---------------------------------------------------------------------------
 //
 void CAiScutSettingsItem::ConstructL(const TDesC& aValue)
 {
     ParseValueL(aValue);
     iValue = aValue.AllocL();
 }

 // ---------------------------------------------------------------------------
 //
 // ---------------------------------------------------------------------------
 //
 CAiScutSettingsItem::~CAiScutSettingsItem()
 {
    delete iValue;
    delete iListBoxLine;
 }

 // ---------------------------------------------------------------------------
 //
 // ---------------------------------------------------------------------------
 //
 TInt CAiScutSettingsItem::ParseValueL(const TDesC& aValue)
 {
    HBufC* caption = NULL;
    TInt err = KErrNone;
    HBufC* buf = HBufC::NewLC(KMaxBufSize);
    TPtr bufPtr = buf->Des();

    TAiScutParser parser;
    parser.Parse(aValue);

    TShortcutType type = parser.Type();

    iUid = parser.Uid();
    iType = EAiScutSettingTypeApplication;

    switch (type)
    {
        case EScutApplication:
        case EScutApplicationWithParams:
        case EScutApplicationView:
        case EScutLogsMissedCallsView:
        case EScutLogsDialledCallsView:
        case EScutLogsReceivedCallsView:
        case EScutLogsMainView:
            err = iModel.GetAppCaption(iUid, bufPtr);
            if (err == KErrNone)
            {
                caption = bufPtr.AllocLC();
            }
            break;

        case EScutNewMessage:
            caption = StringLoader::LoadLC(
                R_SCUT_SETTINGS_NEW_MSG, iModel.Env());
            break;

        case EScutNewEmail:
            caption = StringLoader::LoadLC(
                R_SCUT_SETTINGS_NEW_EMAIL, iModel.Env());
            break;

        #ifdef __SYNCML_DS_EMAIL
            case EScutNewSyncMLMail:
                caption = StringLoader::LoadLC(
                    R_SCUT_SETTINGS_NEW_SYNCML_MAIL, iModel.Env());
                break;
        #endif

        case EScutNewPostcard:
            caption = StringLoader::LoadLC(
                R_SCUT_SETTINGS_NEW_POSTCARD, iModel.Env());
            break;

        case EScutNewAudioMsg:
            caption = StringLoader::LoadLC(
                R_SCUT_SETTINGS_NEW_AUDIO_MSG, iModel.Env());
            break;

        case EScutNewMsgType:
            caption = StringLoader::LoadLC(
                R_SCUT_SETTINGS_SELECT_MSG_TYPE, iModel.Env());
            break;

        case EScutChangeTheme:
            caption = StringLoader::LoadLC(
                R_SCUT_SETTINGS_CHANGE_THEME, iModel.Env());
            break;

        case EScutMailbox:
            {
            err = KErrNotFound;
            // Parse the mailbox id from the definition
            TLex lex(parser.Get(EScutDefParamValue));
            TInt mailboxId = KErrNone;
            lex.Val(mailboxId);

            CMsvSession* msvSession = CMsvSession::OpenAsObserverL(*iModel.AppList());
            CleanupStack::PushL(msvSession);

            CMsvEntry* rootEntry = msvSession->GetEntryL(KMsvRootIndexEntryIdValue);
            CleanupStack::PushL(rootEntry);          
            
            // Searching the mailbox name
            for (TInt i = rootEntry->Count() - 1; i >= 0; --i)
            {
                const TMsvEntry& tentry = (*rootEntry)[i];

                if ((tentry.iMtm == KSenduiMtmImap4Uid || tentry.iMtm == KSenduiMtmPop3Uid) &&
                    tentry.Id() == mailboxId)
                {
                    caption = tentry.iDetails.AllocL();
                    err = KErrNone;
                    break;
                }
            }
            CleanupStack::PopAndDestroy(rootEntry);
            CleanupStack::PopAndDestroy(msvSession);
            if ( caption )
                {
                CleanupStack::PushL( caption );
                }
            }
            break;

        case EScutNoEffect:
            caption = StringLoader::LoadLC(
                R_SCUT_SETTINGS_NO_EFFECT, iModel.Env());
            break;

        case EScutConnectivityStatusView:
            caption = StringLoader::LoadLC(
                R_SCUT_SETTINGS_CONNECTIVITY_STATUS, iModel.Env());
            break;

        case EScutApplicationManagerView:
            caption = StringLoader::LoadLC(
                R_SCUT_SETTINGS_APPMNGR, iModel.Env());
            break;

        case EScutBookmark:
            {
            // aValue = "localapp:0x102750fb?bkm=0x12345678"
            iUid = KScutSettingsDllUid;
            iType = EAiScutSettingTypeBookmark;
            TUid uid = parser.ParseUid(parser.Get(EScutDefParamValue));
            TRAP(err, iModel.GetBkmCaptionL(uid, bufPtr));
            if (err == KErrNone)
            {
                caption = bufPtr.AllocLC();

                __PRINT( __DBG_FORMAT( "XAI: CAiScutSettingsItem bkm uid = 0x%x caption = '%S' "),
                uid.iUid, caption);
            }
            }
            break;

        case EScutWebAddress:
        default:
            // treat unknown shortcuts as typed urls.
            iUid.iUid = 0;
            iType = EAiScutSettingTypeUrl;
        HBufC* tmp = HBufC::NewLC( aValue.Length() );
        TPtr tmpPtr = tmp->Des();
        TInt err = parser.CustomTitle( tmpPtr );

        if ( err != KErrNone || tmp->Length() <= 0 )
            {
            CleanupStack::PopAndDestroy( tmp );
            caption = aValue.AllocLC();
            }
        else
            {
            caption = tmp;
            }
            break;
        }

    if (caption)
    {
        CreateListBoxLineL(*caption);
        CleanupStack::PopAndDestroy(caption);
    }

    CleanupStack::PopAndDestroy(buf);
    return err;
 }

 // ---------------------------------------------------------------------------
 // Changes the setting item target application.
 // ---------------------------------------------------------------------------
 //
 void CAiScutSettingsItem::ChangeApplicationL(TUid aUid, const TDesC& aParams,
        const TDesC& aCaption)
 {
    iUid = aUid;
    iType = EAiScutSettingTypeApplication;

    HBufC* newValue = NULL;
    TAiScutParser parser;
    parser.ComposeL(newValue, iUid, aParams);

    delete iValue;
    iValue = NULL;
    iValue = newValue;

    CreateListBoxLineL(aCaption);
 }

 // -----------------------------------------------------------------------------
 // Changes the setting item target bookmark.
 // -----------------------------------------------------------------------------
 //
 void CAiScutSettingsItem::ChangeBookmarkL(const TDesC& aParams, const TDesC& aCaption)
 {
    iUid = KScutSettingsDllUid;
    iType = EAiScutSettingTypeBookmark;

    HBufC* newValue = NULL;
    TAiScutParser parser;
    parser.ComposeL(newValue, iUid, aParams);

    delete iValue;
    iValue = NULL;
    iValue = newValue;

    CreateListBoxLineL(aCaption);
 }

 // -----------------------------------------------------------------------------
 // Changes the setting item target url.
 // -----------------------------------------------------------------------------
 //
 void CAiScutSettingsItem::ChangeUrlL(const TDesC& aUrl)
 {
    iUid.iUid = 0;
    iType = EAiScutSettingTypeUrl;

    delete iValue;
    iValue = NULL;
    iValue = aUrl.AllocL();

    CreateListBoxLineL(*iValue);
 }

 // -----------------------------------------------------------------------------
 // Creates a formatted listbox line.
 // -----------------------------------------------------------------------------
 //
 void CAiScutSettingsItem::CreateListBoxLineL(const TDesC& aCaption)
 {
    HBufC* title = CreateItemTitleLC();

    TPtrC caption;
    caption.Set(aCaption);

    TUriParser parser;
    TInt err = parser.Parse(aCaption);
    if (err == KErrNone)
    {
        // Remove scheme from the url.
        const TDesC& host = parser.Extract(EUriHost);
        if (host.Length())
        {
            caption.Set(host);
        }
    }

    // Format (" \t%S\t\t%S") without %S characters.
    TInt formatLength = KSettingListboxLineFormat().Length();

    HBufC* listBoxLine =
        HBufC::NewLC(title->Length() + caption.Length() + formatLength);

    TPtr ptr = listBoxLine->Des();
    ptr.Format(KSettingListboxLineFormat, title, &caption);

    delete iListBoxLine;
    iListBoxLine = NULL;
    iListBoxLine = listBoxLine;
    CleanupStack::Pop(listBoxLine);

    TInt titlePos = ptr.Find(*title);
    if (titlePos < 0)
    {
        titlePos = 0;
    }
    iTitle.Set(ptr.Mid(titlePos, title->Length()));

    CleanupStack::PopAndDestroy(title);
 }

 // -----------------------------------------------------------------------------
 // Creates a setting item title.
 // -----------------------------------------------------------------------------
 //
 HBufC* CAiScutSettingsItem::CreateItemTitleLC()
 {
     HBufC* title = NULL;
     if (iKey & KScutFlagBitOptionallyVisible)
     {
        // Optionally visible shortcuts are either navigation keys or soft keys.
        title = CreateOptionallyVisibleKeyTitleLC();
     }

     // Make sure something gets loaded and put to cleanup stack.
     if (!title)
     {
        title = StringLoader::LoadLC(
            R_SCUT_SETTINGS_TXT_LINKN, iIndex+1, iModel.Env());
     }

     __PRINT( __DBG_FORMAT( "XAI: CreateItemTitleLC %d key = 0x%x (0x%x) title = '%S' "),
            iIndex+1, iKey, (iKey & 0xFFFF), title);

     return title;
 }

 // -----------------------------------------------------------------------------
 // Creates a key title for an optionally visible shortcut.
 // -----------------------------------------------------------------------------
 //
 HBufC* CAiScutSettingsItem::CreateOptionallyVisibleKeyTitleLC()
 {
    HBufC* title = NULL;
    TInt resourceId = 0;

    switch (iKey & 0xFFFF)
    {
        case EAiScutScrollKeyLeft:
            resourceId = R_SCUT_SETTINGS_SCROLL_LEFT;
            break;

        case EAiScutScrollKeyRight:
            resourceId = R_SCUT_SETTINGS_SCROLL_RIGHT;
            break;

        case EAiScutScrollKeyUp:
            resourceId = R_SCUT_SETTINGS_SCROLL_UP;
            break;

        case EAiScutScrollKeyDown:
            resourceId = R_SCUT_SETTINGS_SCROLL_DOWN;
            break;

        case EAiScutSelectionKey:
            resourceId = R_SCUT_SETTINGS_SELECTION_KEY;
            break;

        case EAiScutSoftKeyLeft:
            resourceId = R_SCUT_SETTINGS_SOFTKEY_LEFT;
            break;

        case EAiScutSoftKeyRight:
            resourceId = R_SCUT_SETTINGS_SOFTKEY_RIGHT;
            break;

        default:
            resourceId = 0;
            break;
        }

    if (resourceId)
    {
        title = StringLoader::LoadLC(resourceId, iModel.Env());
    }
    else
    {
        title = NULL;
    }

    return title;
 }

 // ---------------------------------------------------------------------------
 // Returns the setting item value.
 // ---------------------------------------------------------------------------
 //
 TPtrC CAiScutSettingsItem::Value() const
 {
     if(iValue)
         {
         return TPtrC( *iValue );
         }
     return TPtrC(KNullDesC);
 }

 // ---------------------------------------------------------------------------
 // Returns the formatted listbox line descriptor.
 // ---------------------------------------------------------------------------
 //
 TPtrC CAiScutSettingsItem::ListBoxLine() const
 {
     if(iListBoxLine)
         {
         return TPtrC( *iListBoxLine );
         }
     return TPtrC(KNullDesC);
 }
// End of File.
