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
* Description:  MCS settings plug-in main class.
*
*/

#include <ecom/implementationproxy.h>
#include <e32std.h>
#include <eikmenup.h>
#include <eikbtgpc.h>
#include <StringLoader.h>
#include <gsfwviewuids.h>
#include <gsprivatepluginproviderids.h>
#include <bautils.h>
#include <pathinfo.h>
#include <featmgr.h>
#include <e32property.h>                // For RProperty
#include <activeidle2domainpskeys.h>    // For KPSUidActiveIdle2

#include <avkon.rsg>
#include <mcspluginsettingsres.rsg>
#include <aisystemuids.hrh>

#include "hs_app_mcspluginsettings.hrh"
#include "hs_app_mcspluginsettings.h"
#include "hs_app_mcspluginsettingscontainer.h"
#include "hs_app_mcspluginsettingsmodel.h"
#include "hs_app_mcspluginuids.hrh"

const TUid KUidScutSettingsPlugin =
{
    AI_UID_ECOM_IMPLEMENTATION_SETTINGS_MCSPLUGIN
};

const TUid KUidAiFwCommon =
{
        AI_UID3_AIFW_COMMON
};

_LIT(KMCSSettingsResourceFileName, "hs_app_mcspluginsettingsres.rsc");

/** Implementation table for MCS settings plug-in */
const TImplementationProxy KMCSPluginSettingsImplementationTable[] =
{
    IMPLEMENTATION_PROXY_ENTRY(
        AI_UID_ECOM_IMPLEMENTATION_SETTINGS_MCSPLUGIN, CMCSPluginSettings::NewL)
};


// ---------------------------------------------------------------------------
// Gate/factory function.
// ---------------------------------------------------------------------------
//
EXPORT_C const TImplementationProxy* ImplementationGroupProxy(TInt& aTableCount)
{
    aTableCount =
        sizeof(KMCSPluginSettingsImplementationTable) /
        sizeof(TImplementationProxy);
    return KMCSPluginSettingsImplementationTable;
}

// ======== MEMBER FUNCTIONS ========

// ---------------------------------------------------------------------------
// First phase construction
// ---------------------------------------------------------------------------
//
CMCSPluginSettings::CMCSPluginSettings()
{
}

// ---------------------------------------------------------------------------
// Second phase construction
// ---------------------------------------------------------------------------
//
void CMCSPluginSettings::ConstructL()
{
    FeatureManager::InitializeLibL();

    TParsePtrC driveParse(PathInfo::RomRootPath());
    TFileName resourceName(driveParse.Drive());
    // Find the resource file.
    TParse parse;
    parse.Set(KMCSSettingsResourceFileName, &KDC_RESOURCE_FILES_DIR, NULL);
    resourceName.Append(parse.FullName());
    // Open resource file.
    iResourceLoader.OpenL(resourceName);

    iModel = CMCSPluginSettingsModel::NewL(*this, iCoeEnv);

    BaseConstructL(R_AI_MCS_SETTINGS_VIEW);
}

// ---------------------------------------------------------------------------
// Two-phased constructor
// ---------------------------------------------------------------------------
//
CMCSPluginSettings* CMCSPluginSettings::NewL( TAny* /*aInitParams*/ )
{
    CMCSPluginSettings* self = new (ELeave) CMCSPluginSettings;
    CleanupStack::PushL(self);
    self->ConstructL();
    CleanupStack::Pop(self);
    return self;
}

// ---------------------------------------------------------------------------
// Destructor
// ---------------------------------------------------------------------------
//
CMCSPluginSettings::~CMCSPluginSettings()
{
    FeatureManager::UnInitializeLib();
    iResourceLoader.Close();
    delete iModel;
}

// ---------------------------------------------------------------------------
// From CAknView
// Returns view id.
// ---------------------------------------------------------------------------
//
TUid CMCSPluginSettings::Id() const
{
    return KUidScutSettingsPlugin;
}

// ---------------------------------------------------------------------------
// From CAknView
// Handles commands.
// ---------------------------------------------------------------------------
//
void CMCSPluginSettings::HandleCommandL(TInt aCommand)
{
    switch (aCommand)
    {
    case EAiScutSettingsCmdChange:
        Container()->HandleChangeCommandL();
        break;

    case EAknCmdHelp:
        Container()->HandleHelpCommandL();
        break;

    case EAknSoftkeyBack:
        if (iAppUi->View(KGSMainViewUid))
        {
            // if we are in GS activate parent plugin view (standby view)...
            iAppUi->ActivateLocalViewL(KGSPrslnPluginUid);
        }
        else
        {
            iAppUi->ActivateLocalViewL(iPrevViewId.iViewUid);
        }
        break;

    case EAknSoftkeyExit:
        iAppUi->HandleCommandL(EAknCmdExit);
        break;

    default:
        iAppUi->HandleCommandL(aCommand);
        break;
    }
}

// ----------------------------------------------------------------------------
// From CAknView
// First method called by the Avkon framwork
// ----------------------------------------------------------------------------
//
void CMCSPluginSettings::DoActivateL(const TVwsViewId& aPrevViewId, TUid aCustomMessageId, const TDesC8& aCustomMessage)
{
    CEikButtonGroupContainer* cba = Cba();

    if (cba)
    {
        if (aCustomMessageId == KUidAiFwCommon)
        {
            cba->SetCommandSetL(R_SCUT_SOFTKEYS_OPTIONS_EXIT_CHANGE);
        }
        else
        {
            cba->SetCommandSetL(R_SCUT_SOFTKEYS_OPTIONS_BACK_CHANGE);
        }
        cba->DrawDeferred();
    }

    CGSBaseView::DoActivateL(aPrevViewId, aCustomMessageId, aCustomMessage);
    
    iModel->UpdateSettingsContainerL( aCustomMessage );
}

// ----------------------------------------------------------------------------
// From CAknView
// Called by the Avkon view framework when closing.
// ----------------------------------------------------------------------------
//
void CMCSPluginSettings::DoDeactivate()
{
    CGSBaseView::DoDeactivate();

    iModel->SetContainer(Container());
}

// ----------------------------------------------------------------------------
// From MEikMenuObserver
// ----------------------------------------------------------------------------
//
void CMCSPluginSettings::DynInitMenuPaneL(
    TInt aResourceId, CEikMenuPane* aMenuPane)
{
    if (aMenuPane && aResourceId == R_AI_MCS_SETTINGS_MENUPANE)
    {
        if (!FeatureManager::FeatureSupported(KFeatureIdHelp))
        {
            // Disable help if not supported
            aMenuPane->SetItemDimmed(EAknCmdHelp, ETrue);
        }
        if (iModel->MdcaCount() == 0)
        {
            aMenuPane->SetItemDimmed(EAiScutSettingsCmdChange, ETrue);
        }
    }
}

// ---------------------------------------------------------------------------
// From CGSPluginInterface. 256
// ---------------------------------------------------------------------------
//
void CMCSPluginSettings::GetCaptionL(TDes& aCaption) const
{
    iCoeEnv->ReadResourceL(aCaption, R_AI_MCS_SETTINGS_VIEW_CAPTION);
}

// ---------------------------------------------------------------------------
// From CGSPluginInterface
// ---------------------------------------------------------------------------
//
TInt CMCSPluginSettings::PluginProviderCategory() const
{
    return KGSPluginProviderInternal;
}

// ---------------------------------------------------------------------------
// From CGSPluginInterface
// ---------------------------------------------------------------------------
//
TBool CMCSPluginSettings::Visible() const
{
    return EFalse;
}

// ---------------------------------------------------------------------------
// From CGSBaseView
// Returns view id.
// ---------------------------------------------------------------------------
//
CMCSPluginSettingsContainer* CMCSPluginSettings::Container()
{
    return static_cast<CMCSPluginSettingsContainer*>(iContainer);
}

// ---------------------------------------------------------------------------
// From CGSBaseView
// ---------------------------------------------------------------------------
//
void CMCSPluginSettings::NewContainerL()
{
    delete iContainer;
    iContainer = NULL;
    
    iContainer = new (ELeave) CMCSPluginSettingsContainer();
    Container()->SetModel(iModel);
    iModel->SetContainer(Container());
}

// ---------------------------------------------------------------------------
// From CGSBaseView
// ---------------------------------------------------------------------------
//
void CMCSPluginSettings::HandleListBoxSelectionL()
{
    Container()->HandleChangeCommandL();
}

// ---------------------------------------------------------------------------
// Returns if container exists or not
// ---------------------------------------------------------------------------
//
TBool CMCSPluginSettings::Activated() const
    {
    return iContainer ? ETrue : EFalse;
    }

// End of File.