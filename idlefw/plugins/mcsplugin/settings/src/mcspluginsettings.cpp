/*
* Copyright (c) 2009 - 2010 Nokia Corporation and/or its subsidiary(-ies).
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
#include <eikmenup.h>
#include <eikbtgpc.h>
#include <gsfwviewuids.h>
#include <gsprivatepluginproviderids.h>
#include <pathinfo.h>
#include <featmgr.h>

#include <mcspluginsettingsres.rsg>
#include <aisystemuids.hrh>

#include "mcspluginsettings.hrh"
#include "mcspluginsettings.h"
#include "mcspluginsettingscontainer.h"
#include "mcspluginsettingsmodel.h"
#include "mcspluginuids.hrh"

const TUid KUidScutSettingsPlugin =
{
    AI_UID_ECOM_IMPLEMENTATION_SETTINGS_MCSPLUGIN
};

_LIT(KMCSSettingsResourceFileName, "mcspluginsettingsres.rsc");

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
    iTimer = CPeriodic::NewL( CActive::EPriorityUserInput );
    
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
    
    if (iTimer->IsActive())
        {
        iTimer->Cancel();
        }
    delete iTimer;
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
    // Parse the custom message
    TInt locate = aCustomMessage.Locate('/');
    TInt editIdx = -1;
    
    iModel->UpdateAppListL( EFalse );
    iModel->UpdateBkmListL( EFalse );
    
    if (locate > 0)
        {
        // if / is located in the custom mesage
        // upto / is the plugin id
        iModel->SetPluginIdL(aCustomMessage.Left(locate));

        // right most character is the edit index
        TLex8 lex(aCustomMessage.Right(1));
        lex.Val(editIdx);

        iModel->UpdateSettingsL();
        CGSBaseView::DoActivateL(aPrevViewId, aCustomMessageId,
                aCustomMessage.Left(locate));        

        if (editIdx > 0)
            {
            // set the current edit item
            Container()->SetCurrentItemIndex(editIdx - 1);
            
            // Set timer for handle the change of shortcut item
            // Otherwise status bar is not shown correctly
            if (iTimer->IsActive())
                {
                iTimer->Cancel();
                }
            iTimer->Start( 0, 0, TCallBack( TimerCallbackL, this ) );
            }
        }
    else
        { 
        // if '/' is not located, custommessage has only the plugin id 
        iModel->SetPluginIdL(aCustomMessage);
        iModel->UpdateSettingsL();
        CGSBaseView::DoActivateL(aPrevViewId, aCustomMessageId,
                aCustomMessage);
        }
    }

// ----------------------------------------------------------------------------
// From CAknView
// Called by the Avkon view framework when closing.
// ----------------------------------------------------------------------------
//
void CMCSPluginSettings::DoDeactivate()
    {
    CGSBaseView::DoDeactivate();
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
// From CGSPluginInterface
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
// Callback for direct settings change
// ---------------------------------------------------------------------------
//
TInt CMCSPluginSettings::TimerCallbackL( TAny *aPtr )
    {
    CMCSPluginSettings* self = reinterpret_cast< CMCSPluginSettings* >( aPtr );
    self->iTimer->Cancel();
    // Handle the change
    self->HandleListBoxSelectionL();
    
    return 0;
    }

// End of File.
