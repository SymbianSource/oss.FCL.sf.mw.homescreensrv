/*
* Copyright (c) 2009 Nokia Corporation and/or its subsidiary(-ies).
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


#ifndef __UT_MCSSETTINGS_H__
#define __UT_MCSSETTINGS_H__

//  EXTERNAL INCLUDES
#include <digia/eunit/ceunittestsuiteclass.h>
#include <apgcli.h>         //RApaLsSession
#include <w32std.h>                 // For RWsSession
#include <hscontentpublisher.h>
#include <mcsmenu.h>
#include <mcsmenuitem.h>

#include "mcspluginwatcher.h"

//  INTERNAL INCLUDES


//  FORWARD DECLARATIONS
class CAiScutSettingsItem;
class CMCSPluginSettings;
class TMCSPluginParser;
class MAiAppListObserver;
class CMCSPluginSettingsContainer;
class CMCSPluginSettingsAppList;
class CMCSPluginSettingsBkmList;
class CMCSPluginSettingsModel;
class CMCSPluginTargetShutter;
class CGSPluginInterface;
class CGSBaseView;
class CAknView;
class CAknViewAppUi;
class CMCSMenu;
class CActiveSchedulerWait;
      
#include <e32def.h>
#ifndef NONSHARABLE_CLASS
#define NONSHARABLE_CLASS(x) class x
#endif

//  CLASS DEFINITION
/**
 *
 * EUnitWizard generated test class.
 *
 */
NONSHARABLE_CLASS( UT_mcssettings ) : public CEUnitTestSuiteClass
{

enum TBookmarkType
	{
    EFavBookmark,
    EMCSBookmark
    };


public:     // Constructors and destructors

    /**
     * Two phase construction
     */
    static UT_mcssettings* NewL();
    static UT_mcssettings* NewLC();
    /**
     * Destructor
     */
    ~UT_mcssettings();

private:    // Constructors and destructors

    UT_mcssettings();
    void ConstructL();

public:     // From observer interface



private:    // New methods

    void SetupL();

    void SetupAppListL();

    void emptySetup();

    void Teardown();

    void TeardownAppList();

    void UT_Settings_FailL();

    void UT_Settings_NewLL();

    void UT_Settings_IdL();

    void UT_Settings_HandleCommandLL();

    void UT_Settings_DoDeactivateL();

    void UT_Settings_GetCaptionLL();

    void UT_Settings_PluginProviderCategoryL();

    void UT_Settings_VisibleL();

    void UT_SettingsAppList_NewLL();

    void UT_SettingsAppList_MdcaCountL();

    void UT_SettingsAppList_MdcaPointL();

    void UT_SettingsAppList_HandleSessionEventLL();

    void UT_mcssettings::SetupBkmListL();

    void UT_mcssettings::TeardownBkmList();

    void UT_mcssettings::UT_SettingsBkmList_NewLL();

    void UT_mcssettings::UT_SettingsBkmList_MdcaCountL();

    void UT_mcssettings::UT_SettingsBkmList_MdcaPointL();

    void UT_SettingsContainer_CMCSPluginSettingsContainerL();

    void UT_SettingsContainer_IsChangeDialogShowingL();

    void UT_SettingsContainer_CloseChangeDialogL();

    void UT_SettingsModel_NewLL();

    void UT_SettingsModel_MdcaCountL();

    void UT_SettingsModel_MdcaPointL();

    void UT_SettingsModel_ItemL();

    void UT_mcssettings::UT_SettingsModel_BkmListL();

    void UT_SettingsAppList_ItemL();

    void UT_SettingsAppList_FindItemL();

    void UT_SettingsBkmList_ItemL();

    void UT_SettingsBkmList_FindItemL();

    void UT_SettingsContainer_HandleNotifyL();

    void UT_SettingsContainer_ResetCurrrentListL();

    void UT_SettingsModel_BkmList();

    void UT_SettingsModel_AppList();

    void UT_SettingsModel_Item();

    void UT_SettingsModel_ItemId();

    void UT_SettingsModel_ReplaceItemL();

    void SetupModelL();

    void SetupContainerL();

    void TeardownContainer();

    void UT_SettingsBkmList_MCSMenuItemL();

    void UT_SettingsModel_UndefinedText();

    void UT_SettingsContainer_HandleHelpCommandL();


private:    // Data

    CAknViewAppUi*                      iAppUi;
    CGSPluginInterface*                 iGs;
    CGSBaseView*                        iSettingsView;
    CMCSPluginSettingsModel*            iModel;
    CMCSPluginSettingsContainer*        iContainer;
    CMCSPluginSettingsAppList*          iAppList;
    CMCSPluginSettingsBkmList*          iBkmList;
    TUid                                iId;
    RPointerArray<CAiScutSettingsItem>  iItemArray;
    CMCSPluginWatcher* iRemoveWatcher;
    CMCSPluginWatcher* iSaveWatcher;
    RMenu iMenu;

    EUNIT_DECLARE_TEST_TABLE;

};

#endif      //  __UT_MCSSETTINGS_H__

// End of file
