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


#ifndef CAISCUTSETTINGSCONTAINER_H
#define CAISCUTSETTINGSCONTAINER_H

#include <gsbasecontainer.h>

#include "aiscutsettings.hrh"

class CAknRadioButtonSettingPage;
class CAknSettingPage;
class CAknWaitDialog;
class CAiScutSettingsModel;
class CAiScutSettingsBkmList;
class CAiScutSettingsItem;

/**
 * Shortcut settings plug-in container
 *
 * @lib aiscutsettings.lib
 * @since S60 v3.2
 */
class CAiScutSettingsContainer : public CGSBaseContainer
{

public:

    CAiScutSettingsContainer();

    virtual ~CAiScutSettingsContainer();

    /**
     * Sets pointer to settings plug-in model.
     *
     * @since S60 v3.2
     * @param aModel Pointer to settings plug-in model
     */
    void SetModel(CAiScutSettingsModel* aModel);

    /**
     * Chandles a setting change command
     *
     * @since S60 v3.2
     */
    void HandleChangeCommandL();

    /**
     * Chandles a setting type change command
     *
     * @since S60 v3.2
     */
    void HandleChangeShortcutTypeCommandL(TAiScutSettingType aType);

    /**
  	 * Dialog showing changed
     *
     * @since S60 v3.2
      */
    TBool IsChangeDialogShowing();

    /**
     * Close change dialog
     *
     * @since S60 v3.2
     */
    void CloseChangeDialog();

    /**
     * Reset current list
     *
     * @since S60 v3.2
     */
    void ResetCurrentListL(TInt aIndex);

    /**
     * Hides the wait note dialog.
     *
     * @since S60 v3.2
     */
    void HideWaitNoteDialog();
    
    /**
    * Dismisses the wait note dialog and possible
    * other dialogs and stops the change process.
    *
    * @since S60 v3.2
    */
    void StopShortcutChangeProcess();

private:

    void ConstructL(const TRect& aRect);

    // from base class CGSBaseContainer

    /**
     * From CGSBaseContainer
     * Constructs the settings listbox
     *
     * @since S60 v3.2
     * @param aResLbxId Listbox resource id
     */
    void ConstructListBoxL(TInt aResLbxId);

    /**
     * Chandles a setting change command to select application from a list
     *
     * @since S60 v3.2
     * @param aItem Applist item to modify
     * @param aNew ETrue if selecting an application for first time, EFalse if not
     * @return ETrue if shortcut was changed. EFalse if not
     */
    TBool HandleAppListChangeCommandL(CAiScutSettingsItem& aItem, TBool aNew = EFalse);

    /**
     * Chandles a setting change command to select bookmark from a list
     *
     * @since S60 v3.2
     * @param aItem Applist item to modify
     * @param aNew ETrue if selecting an application for first time, EFalse if not
     * @return ETrue if shortcut was changed. EFalse if not
     */
    TBool HandleBookmarkChangeCommandL(CAiScutSettingsItem& aItem, TBool aNew = EFalse);

    /**
     * Chandles a setting change command to edit an URL
     *
     * @since S60 v3.2
     * @param aItem Applist item to modify
     * @param aNew ETrue if typing a new url, EFalse if editing an existing one
     * @return ETrue if shortcut was changed. EFalse if not
     */
    TBool HandleUrlChangeCommandL(CAiScutSettingsItem& aItem, TBool aNew = EFalse);

    /**
     * Handles text editing in an CAknTextSettingPage
     *
     * @since S60 v3.2
     * @param aResId Resource id for the text setting page
     * @param aDes The text to edit
     * @return ETrue if user pressed OK, ETrue if user pressed Cancel
     */
    TBool EditTextL(TInt aResId, TDes& aDes);

    /**
     * Shows the wait note dialog.
     *
     * @since S60 v3.2
     */
    void ShowWaitNoteDialogL();

    /**
     * Required for help.
     *
     */
    void GetHelpContext(TCoeHelpContext& aContext) const;

    /**
     * 
     */
    TKeyResponse OfferKeyEventL(const TKeyEvent& aKeyEvent, TEventCode aType);

    /**
     * Checks if there is a need to update the middle softkey label.
     */
    void CheckMiddleSoftkeyLabelL();

private:  // data

    /**
     * Settings plug-in model.
     * Not own.
     */
    CAiScutSettingsModel*       iModel;

    /**
     * Application selection list page.
     * Own.
     */
    CAknRadioButtonSettingPage* iAppListDialog;

    /**
     * Bookmark selection list page.
     * Own.
     */
    CAknRadioButtonSettingPage* iBkmListDialog;

    /**
     * Text setting page.
     * Own.
     */
    CAknSettingPage*            iEditDialog;

    /**
     * Wait dialog that is shown when the application list is not ready.
     * Own.
     */
    CAknWaitDialog*             iWaitDialog;

    /**
     *
     */
    TAiScutSettingType          iOldType;
    
    /**
    * Is the change process stopped by outside "forces"
    */
    TBool                       iChangeProcessStopped;
    
    /**
     * Is the change process in progress
     */
    TBool                       iChangeProcessInProgress;

};

#endif // CAISCUTSETTINGSCONTAINER_H

// End of File.
