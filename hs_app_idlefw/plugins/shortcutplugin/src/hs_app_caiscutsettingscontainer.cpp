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
* Description:  Shortcut settings plug-in container.
*
*/


#include <aknlists.h>
#include <AknWaitDialog.h>
#include <aknnotewrappers.h>
#include <eikbtgpc.h>
#include <gslistbox.h>
#include <gsfwviewuids.h>
//#include <csxhelp/ai.hlp.hrh>
#include <aknradiobuttonsettingpage.h>
#include <akntextsettingpage.h>
#include <centralrepository.h>
#include <StringLoader.h>

#include <aiscutsettingsres.rsg>
#include <aiscuttexts.rsg>

#include "hs_app_caiscutsettingscontainer.h"
#include "hs_app_caiscutsettingsmodel.h"
#include "hs_app_caiscutsettingsbkmlist.h"
#include "hs_app_caiscutsettingsitem.h"
#include "hs_app_aiscutpluginprivatecrkeys.h"
#include "aiscutdefs.h"

const TUid KUidAI = { 0x101F8701 }; // Active Standby Shortcuts help uid

// ======== MEMBER FUNCTIONS ========

// ---------------------------------------------------------------------------
//
// ---------------------------------------------------------------------------
//
CAiScutSettingsContainer::CAiScutSettingsContainer()
{
}

// ---------------------------------------------------------------------------
//
// ---------------------------------------------------------------------------
//
void CAiScutSettingsContainer::ConstructL(const TRect& aRect)
{
    iListBox = new (ELeave) CAknSettingStyleListBox;
    BaseConstructL(aRect, R_SCUT_SETTINGS_VIEW_TITLE, NULL);

    iOldType = EAiScutSettingTypeUndefined;

    CheckMiddleSoftkeyLabelL();

    iModel->ActivateObservers(ETrue);
}

// ---------------------------------------------------------------------------
//
// ---------------------------------------------------------------------------
//
CAiScutSettingsContainer::~CAiScutSettingsContainer()
{
    HideWaitNoteDialog();
}

// ---------------------------------------------------------------------------
// Sets pointer to settings plug-in model.
// ---------------------------------------------------------------------------
//
void CAiScutSettingsContainer::SetModel(CAiScutSettingsModel* aModel)
{
    iModel = aModel;
}

// ---------------------------------------------------------------------------
// Chandles a setting change command.
// ---------------------------------------------------------------------------
//
void CAiScutSettingsContainer::HandleChangeCommandL()
{       
    if ( iChangeProcessInProgress )
        {
        return;
        }
    TBool changed = EFalse;
    TInt current = iListBox->CurrentItemIndex();
    CAiScutSettingsItem* item = iModel->Item(current);
    if (!item)
    {
        return ;
    }

    TUint32 key = item->Key();
    TBool locked = (0 != (key & KScutFlagBitLocked));

    if (locked)
    {
        HBufC* text = iCoeEnv->AllocReadResourceLC(R_SCUT_SETTINGS_TXT_FIXED_ITEM);
        CAknInformationNote* note = new (ELeave) CAknInformationNote(ETrue);
        note->ExecuteLD(*text);
        CleanupStack::PopAndDestroy(text);
    }
    else
    {
        TAiScutSettingType oldType = item->Type();
        TInt newType = oldType;

        CDesCArrayFlat* array = iCoeEnv->ReadDesC16ArrayResourceL(
            R_SCUT_CHANGE_TO_PAGE_LBX);
        CleanupStack::PushL(array);

        CAknListQueryDialog* dialog = new(ELeave)CAknListQueryDialog(&newType);
        CleanupStack::PushL(dialog);
    	dialog->PrepareLC(R_SCUT_LISTQUERY_CHANGE_TO_PAGE);
    	CleanupStack::Pop(dialog);
        dialog->SetItemTextArray(array);
	    dialog->SetOwnershipType(ELbmDoesNotOwnItemArray);
	    iChangeProcessInProgress = ETrue;
        if (dialog->RunLD())
        {
            item = iModel->Item(current);
            if (newType == EAiScutSettingTypeUrl)
            {
                changed = HandleUrlChangeCommandL(*item, (newType != oldType));
            }
            else if (newType == EAiScutSettingTypeBookmark)
            {
                changed = HandleBookmarkChangeCommandL(*item, (newType != oldType));
            }
            else
            {
                changed = HandleAppListChangeCommandL(*item, (newType != oldType));
            }
            if ( changed )
                {
                ResetCurrentListL(current);    
                }

        }
        iChangeProcessInProgress = EFalse;
        CleanupStack::PopAndDestroy(array);
    }
}

// ---------------------------------------------------------------------------
//
// ---------------------------------------------------------------------------
//
TBool CAiScutSettingsContainer::IsChangeDialogShowing()
{
    return (iAppListDialog || iBkmListDialog || iEditDialog);
}

// ---------------------------------------------------------------------------
//
// ---------------------------------------------------------------------------
//
void CAiScutSettingsContainer::CloseChangeDialog()
{
    TKeyEvent keyEvent;

    keyEvent.iCode      = EKeyEscape;
    keyEvent.iScanCode  = EStdKeyEscape;
    keyEvent.iModifiers = 0;
    keyEvent.iRepeats   = 0;

    CCoeControl* dialog = NULL;

    if (iAppListDialog)
    {
        dialog = static_cast<CCoeControl*>(iAppListDialog);
    }
    else if (iBkmListDialog)
    {
        dialog = static_cast<CCoeControl*>(iBkmListDialog);
    }
    else if (iEditDialog)
    {
        dialog = static_cast<CCoeControl*>(iEditDialog);
    }

    if (dialog)
    {
        TRAP_IGNORE(dialog->OfferKeyEventL(keyEvent, EEventKey));
    }
}

// ---------------------------------------------------------------------------
//
// ---------------------------------------------------------------------------
//
void CAiScutSettingsContainer::ResetCurrentListL(TInt aIndex)
{
    if (iModel->MdcaCount() == 0)
    {
        iListBox->HandleItemRemovalL();
    }
    else
    {
        iListBox->HandleItemAdditionL();
    }

    iListBox->SetCurrentItemIndex(aIndex);

    CheckMiddleSoftkeyLabelL();
}

// ---------------------------------------------------------------------------
// From CGSBaseContainer
// Constructs the settings listbox.
// ---------------------------------------------------------------------------
//
void CAiScutSettingsContainer::ConstructListBoxL(TInt /*aResLbxId*/)
{
    iListBox->ConstructL(this, EAknListBoxSelectionList);

    // Set empty listbox's text.
    HBufC* text = iCoeEnv->AllocReadResourceLC(R_SCUT_SETTINGS_TXT_ALL_FIXED);
    iListBox->View()->SetListEmptyTextL(*text);
    CleanupStack::PopAndDestroy(text);

    iListBox->Model()->SetItemTextArray(iModel);
    iListBox->Model()->SetOwnershipType(ELbmDoesNotOwnItemArray);
}

// ---------------------------------------------------------------------------
// Chandles a setting change command to select application from a list.
// ---------------------------------------------------------------------------
//
TBool CAiScutSettingsContainer::HandleAppListChangeCommandL(CAiScutSettingsItem& aItem,
    TBool /*aNew*/)
{
    
    TBool changed = EFalse;
    
    iChangeProcessStopped = EFalse;
    
    if (!iModel->AppList()->IsReady())
    {
        ShowWaitNoteDialogL(); // Blocks until applist reports that it is ready.
    }
    // If wait note was "killed" then don't continue the change process
    // If the process is not stopped here it could lead to situation where
    // self is deleted before the ShowWaitNoteDialogL() returns
    if ( iChangeProcessStopped )
        {
        return EFalse;
        }

    TUint32 key = aItem.Key();
    TBool optional  = (0 != (key & KScutFlagBitOptionallyVisible));
    TBool scrollkey = (0 != ((key & 0xFFFF) < EAiScutSoftKeyLeft));

    if (optional && scrollkey)
    {
        iModel->AppList()->AddExtraItemsL();
    }

    TInt index = iModel->AppList()->FindIndex(aItem);
    TInt oldIndex = index;

    iAppListDialog = new (ELeave) CAknRadioButtonSettingPage(
        R_SCUT_SETTINGS_APPLIST_PAGE, index, iModel->AppList());

    iAppListDialog->SetSettingTextL(aItem.Title());
    iAppListDialog->ConstructL();

    if (iAppListDialog->ExecuteLD(CAknSettingPage::EUpdateWhenChanged) &&
        index != oldIndex)
    {
        TUid uid;
        TPtrC params;
        TPtrC caption;
        TAiScutSettingType type;
        if (iModel->AppList()->GetDataByIndex(index, uid, params, caption, type) == KErrNone)
        {
            if ( type == EAiScutSettingTypeUrl )
                {
                aItem.ChangeUrlL( params );
                }
            else
                {                
                aItem.ChangeApplicationL(uid, params, caption);
                }
            iModel->SaveItemL(aItem);
            changed = ETrue;
        }
    }

    if (optional && scrollkey)
    {
        iModel->AppList()->RemoveExtraItemsL();
    }

    iAppListDialog = NULL;
    return changed;
}

// ---------------------------------------------------------------------------
// Chandles a setting change command to select bookmark from a list.
// ---------------------------------------------------------------------------
//
TBool CAiScutSettingsContainer::HandleBookmarkChangeCommandL(CAiScutSettingsItem& aItem,
    TBool /*aNew*/)
{

    TBool changed = EFalse;
    iModel->BkmList()->GetBookmarkListL();
    TInt index = iModel->BkmList()->FindIndex(aItem);
    TInt oldIndex = index;

    iBkmListDialog = new (ELeave) CAknRadioButtonSettingPage(
        R_SCUT_SETTINGS_APPLIST_PAGE, index, iModel->BkmList());

    iBkmListDialog->SetSettingTextL(aItem.Title());
    iBkmListDialog->ConstructL();

    if (iBkmListDialog->ExecuteLD(CAknSettingPage::EUpdateWhenChanged) &&
        index != oldIndex)
    {
        TPtrC params;
        TPtrC caption;
        if (iModel->BkmList()->GetDataByIndex(index, params, caption) == KErrNone)
        {
            aItem.ChangeBookmarkL(params, caption);
            iModel->SaveItemL(aItem);
            changed = ETrue;
        }
    }

    iBkmListDialog = NULL;
    return changed;
}

// ---------------------------------------------------------------------------
// Chandles a setting change command to edit an URL.
// ---------------------------------------------------------------------------
//
TBool CAiScutSettingsContainer::HandleUrlChangeCommandL(CAiScutSettingsItem& aItem,
    TBool aNew)
{
    TBool changed = EFalse;
    HBufC* newUrl = HBufC::NewLC(NCentralRepositoryConstants::KMaxUnicodeStringLength);
    TPtr urlPtr = newUrl->Des();

    if (!aNew)
    {
        urlPtr.Copy(aItem.Value());
    }

    if (EditTextL(R_SCUT_TYPE_URL_PAGE , urlPtr))
    {
        aItem.ChangeUrlL(urlPtr);
        iModel->SaveItemL(aItem);
        changed = ETrue;
    }

    CleanupStack::PopAndDestroy(newUrl);
    return changed;
}

// ---------------------------------------------------------------------------
//
// ---------------------------------------------------------------------------
//
TBool CAiScutSettingsContainer::EditTextL(TInt aResId, TDes& aDes)
{
    TBool ret = EFalse;

    iEditDialog = new (ELeave) CAknTextSettingPage(aResId, aDes);

    if (iEditDialog->ExecuteLD(CAknSettingPage::EUpdateWhenChanged))
    {
        if (aDes.Length())
        {
            ret = ETrue;
        }
    }

    iEditDialog = NULL;

    return ret;
}

// ---------------------------------------------------------------------------
// Shows the wait note dialog.
// ---------------------------------------------------------------------------
//
void CAiScutSettingsContainer::ShowWaitNoteDialogL()
{
    if (!iWaitDialog)
    {
        iWaitDialog = new (ELeave) CAknWaitDialog(
            (reinterpret_cast<CEikDialog**> (&iWaitDialog)),
            ETrue                                               // aVisibilityDelayOff
        );
        iWaitDialog->ExecuteLD(R_SCUT_WAIT_NOTE);
    }
}

// ---------------------------------------------------------------------------
// Hides the wait note dialog.
// ---------------------------------------------------------------------------
//
void CAiScutSettingsContainer::HideWaitNoteDialog()
{
    if (iWaitDialog)
    {
        TRAP_IGNORE(iWaitDialog->ProcessFinishedL()); // deletes the dialog.
        iWaitDialog = NULL;
    }
}

void CAiScutSettingsContainer::StopShortcutChangeProcess()
{
    iChangeProcessStopped = ETrue;
    HideWaitNoteDialog();
    if ( IsChangeDialogShowing() )
        {
        CloseChangeDialog();    
        }    
}

// ---------------------------------------------------------------------------
// Gets Help
// ---------------------------------------------------------------------------
//
void CAiScutSettingsContainer::GetHelpContext(TCoeHelpContext& aContext) const
{
    aContext.iMajor   = KUidAI;
    //aContext.iContext = KSET_HLP_PERSONAL_SHORTCUTS; // This is specified in HRH file
}

// ---------------------------------------------------------------------------
//
// ---------------------------------------------------------------------------
//
TKeyResponse CAiScutSettingsContainer::OfferKeyEventL(
    const TKeyEvent& aKeyEvent, TEventCode aType)
{
    switch (aKeyEvent.iCode)
    {
    case EKeyUpArrow:
    case EKeyDownArrow:
        {
            TKeyResponse resp = iListBox->OfferKeyEventL(aKeyEvent, aType);
            return resp;
        }

    case EKeyLeftArrow:
    case EKeyRightArrow:
        // Listbox takes all event even if it doesn't use them:
        return EKeyWasNotConsumed;
		
    default:
        break;
    }

    // Now it's iListBox's job to process the key event
    return iListBox->OfferKeyEventL(aKeyEvent, aType);
}

// ---------------------------------------------------------------------------
// Checks if there is a need to update the middle softkey label.
// ---------------------------------------------------------------------------
//
void CAiScutSettingsContainer::CheckMiddleSoftkeyLabelL()
{
    CEikButtonGroupContainer* cba = CEikButtonGroupContainer::Current();
    if (cba)
    {
        cba->MakeCommandVisible(EAiScutSettingsCmdChange, (iModel->MdcaCount() != 0));
    }
}

// End of File.
