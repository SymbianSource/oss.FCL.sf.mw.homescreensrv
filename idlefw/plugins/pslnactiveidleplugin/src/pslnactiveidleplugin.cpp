/*
* Copyright (c) 2005-2007 Nokia Corporation and/or its subsidiary(-ies).
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
* Description:  View for Active Idle skinning.
*
*/


// This plugin specific.
#include <pslnactiveidlepluginrsc.rsg>
#include <pslnactiveidleplugin.mbg>
#include "pslnactiveidleplugin.h"
#include "pslnactiveidleplugincontainer.h"
#include "pslnactiveidlepluginengine.h"
#include "pslnactiveidlepluginuids.h"

// General services.
#include <stringloader.h>
#include <featmgr.h>
#include <gsfwviewuids.h>

// Psln Framework specific.
#include <pslnfwviewuids.h>
#include <psln.hrh>
#include <pslncommon.rsg>
#include <psln.rsg>


// Path to mbm file.
_LIT(KPslnActiveIdleIconFileName, "pslnactiveidleplugin.mbm");

// Path to compiled resource file.
_LIT(KPslnActiveIdlePluginResourceFileName, "z:pslnactiveidlepluginrsc.rsc");

// Path to common personalization resources. This resource file is meant for
// shared resources between application and plugins.
_LIT(KPslnCommonResourceFileName, "z:pslncommon.rsc");

// Path to Psln application resource file. Plugin uses some localized texts
// from Psln's resources.
_LIT(KPslnApplicationResourceFileName, "z:psln.rsc");

// Location of this plugin view within Psln's tab group.
const TInt KPslnActiveIdlePluginLocation = 2;

const TUid KPslnActiveIdlePluginUid = { KPslnActiveIdlePluginImplementationUID };

// Middle Softkey control ID.
const TInt KPslnMSKControlId = 3;

// ----------------------------------------------------------------------------
// CPslnActiveIdlePlugin::CPslnActiveIdlePlugin()
//
// Constructor
// ----------------------------------------------------------------------------
//
CPslnActiveIdlePlugin::CPslnActiveIdlePlugin(CAknViewAppUi* aAppUi)
    :
    iResourceLoader(*iCoeEnv),
    iResourceLoaderCommon(*iCoeEnv),
    iResourceLoaderPsln(*iCoeEnv),
    iAppUi(aAppUi)
{
}

// ---------------------------------------------------------------------------
// CPslnActiveIdlePlugin::NewL
//
// Symbian OS default constructor
// ---------------------------------------------------------------------------
CPslnActiveIdlePlugin* CPslnActiveIdlePlugin::NewL(TAny* aAppUi)
{
    CAknViewAppUi* appUi = reinterpret_cast<CAknViewAppUi*>(aAppUi);
    CPslnActiveIdlePlugin* self = new(ELeave) CPslnActiveIdlePlugin (appUi);

    CleanupStack::PushL(self);
    self->ConstructL();
    CleanupStack::Pop(self);

    return self;
}

// ---------------------------------------------------------------------------
// CPslnActiveIdlePlugin::ConstructL
//
// Symbian OS two-phased constructor
// ---------------------------------------------------------------------------
void CPslnActiveIdlePlugin::ConstructL()
{
#ifdef _MY_DEBUG
    RDebug::Print(_L("XAI: CPslnActiveIdlePlugin::ConstructL"));
#endif

    // Find the resource file:
    TParse* parse = new (ELeave) TParse;
    CleanupStack::PushL(parse);
    parse->Set(KPslnCommonResourceFileName, &KDC_APP_RESOURCE_DIR, NULL);
    TFileName* fileName = new (ELeave) TFileName(parse->FullName());
    CleanupStack::PushL(fileName);

    // Open resource file:
    iResourceLoaderCommon.OpenL(*fileName);
    CleanupStack::PopAndDestroy(fileName);

    // Find the resource file:
    parse->Set(KPslnApplicationResourceFileName, &KDC_APP_RESOURCE_DIR, NULL);
    fileName = new (ELeave) TFileName(parse->FullName());
    CleanupStack::PushL(fileName);

    // Open resource file:
    iResourceLoaderPsln.OpenL(*fileName);
    CleanupStack::PopAndDestroy(fileName);
    CleanupStack::PopAndDestroy(parse);

    // Open own resources for reading.
    OpenLocalizedResourceFileL(
        KPslnActiveIdlePluginResourceFileName,
        iResourceLoader);

    BaseConstructL(R_PSLN_ACTIVE_IDLE_VIEW);

    iEngine = new (ELeave) CPslnActiveIdlePluginEngine(this);
    iEngine->ConstructL( TRect() );
    iEngine->ManualGetSkinsRestartL(ETrue); // Force start

}

// ----------------------------------------------------------------------------
// CPslnActiveIdlePlugin::~CPslnActiveIdlePlugin
//
// Destructor
// ----------------------------------------------------------------------------
CPslnActiveIdlePlugin::~CPslnActiveIdlePlugin()
{
    iResourceLoaderCommon.Close();
    iResourceLoaderPsln.Close();
    iResourceLoader.Close();
    delete iEngine;
}

// ---------------------------------------------------------------------------
// TUid CPslnActiveIdlePlugin::Id()
//
// Returns view's ID.
// ---------------------------------------------------------------------------
TUid CPslnActiveIdlePlugin::Id() const
{
    return KPslnActiveIdlePluginUid;
}

// ----------------------------------------------------------------------------
// CPslnActiveIdlePlugin::GetCaption
//
// Return application/view caption. 128
// ----------------------------------------------------------------------------
//
void CPslnActiveIdlePlugin::GetCaptionL(TDes& aCaption) const
{
    // the resource file is already opened.
    HBufC* result = StringLoader::LoadL(R_PSLN_AI_LIST_VIEW_CAPTION);
    if (aCaption.MaxLength() >= result->Length())
    {
        aCaption.Copy(*result);
    }
    delete result;
    }

// ----------------------------------------------------------------------------
// CPslnActiveIdlePlugin::GetTabTextL
//
// Return tab text. 128
// ----------------------------------------------------------------------------
//
void CPslnActiveIdlePlugin::GetTabTextL(TDes& aCaption) const
{
    // the resource file is already opened.
    HBufC* result = StringLoader::LoadL(R_PSLN_AI_TAB_NAME);
    if (aCaption.MaxLength() >= result->Length())
        {
        aCaption.Copy(*result);
        }
    delete result;
    }

// ----------------------------------------------------------------------------
// CPslnActiveIdlePlugin::CreateIconL
//
// Creates Main view icon.
// ----------------------------------------------------------------------------
//
CGulIcon* CPslnActiveIdlePlugin::CreateIconL()
{
    // Find the resource file:
    TParse* parse = new (ELeave) TParse;
    CleanupStack::PushL(parse);
    parse->Set(KPslnActiveIdleIconFileName, &KDC_APP_BITMAP_DIR, NULL);
    HBufC* fileName = parse->FullName().AllocLC();
    TPtr fileNamePtr = fileName->Des();

    CGulIcon* icon = AknsUtils::CreateGulIconL(
        AknsUtils::SkinInstance(),
        KAknsIIDQgnPropPslnAiSub,
        fileNamePtr,
        EMbmPslnactiveidlepluginQgn_prop_psln_ai_sub,
        EMbmPslnactiveidlepluginQgn_prop_psln_ai_sub_mask);

    CleanupStack::PopAndDestroy(2, parse); // fileName, parse

    return icon;
}

// -----------------------------------------------------------------------------
// CPslnActiveIdlePlugin::GetLocationTypeAndIndex()
//
//
// -----------------------------------------------------------------------------
//
void CPslnActiveIdlePlugin::GetLocationTypeAndIndex(
    TPslnFWLocationType& aType,
    TInt& aIndex) const
{
    aType = CPslnFWPluginInterface::EPslnFWSpecified;
    aIndex = KPslnActiveIdlePluginLocation;
}

// ---------------------------------------------------------------------------
// CPslnActiveIdlePlugin::HandleCommandL(TInt aCommand)
//
// Handles commands directed to this class.
// ---------------------------------------------------------------------------
void CPslnActiveIdlePlugin::HandleCommandL(TInt aCommand)
{
    switch (aCommand)
    {
    case EPslnCmdAppActivate:
        iEngine->ActivateThemeL();
        break;

    case EAknSoftkeyBack:
        RemoveCommandFromMSK();
        if (iAppUi->View(KPslnMainViewUid))
        {
            // if we are in Psln activate Psln main view...
            iAppUi->ActivateLocalViewL(KPslnMainViewUid);
        }
        else if (iAppUi->View(KGSMainViewUid))
        {
            // ... else if we are in GS activate parent plugin view (standby view)...
            iAppUi->ActivateLocalViewL(KGSStandbyPluginUid);
        }
        else
        {
            iAppUi->HandleCommandL(aCommand);
        }
        break;

    case EAknSoftkeyExit:
        // This is here because we use different softkey setups
        iAppUi->HandleCommandL(EAknCmdExit);
        break;

    case EPslnCmdAppHelp:
    case EAknCmdHelp:// Fall trough
        CPslnFWBaseView::HandleCommandL(aCommand);
        break;

    default:
        iAppUi->HandleCommandL(aCommand);
        break;
    }
}

// ----------------------------------------------------------------------------
// CPslnActiveIdlePlugin::Container
//
// Return handle to container class.
// ----------------------------------------------------------------------------
//
CPslnActiveIdlePluginContainer* CPslnActiveIdlePlugin::Container()
{
    return static_cast<CPslnActiveIdlePluginContainer*>(iContainer);
}

// -----------------------------------------------------------------------------
// Checks is there a need to update the middle softkey label.
// This method should do nothing but MSK issues, since it is still called
// if the framework does not support MSK.
// -----------------------------------------------------------------------------
//
void CPslnActiveIdlePlugin::CheckMiddleSoftkeyLabelL()
{
    CPslnActiveIdlePluginContainer* container = Container();
    TInt highlightedItem = iContainer->iListBox->CurrentItemIndex();

#ifdef _MY_DEBUG
    RDebug::Print(_L("XAI: CPslnActiveIdlePlugin::CheckMiddleSoftkeyLabelL highl = %d, curr = %d"), highlightedItem, container->GetCurrentlySelectedIndex());
#endif

    // First remove any previous commands.
    RemoveCommandFromMSK();

    if (highlightedItem >= 0 &&
        highlightedItem != container->GetCurrentlySelectedIndex())
    {
        // Activate:
        CPslnFWBaseView::SetMiddleSoftKeyLabelL(
            R_PSLN_MSK_ACTIVATE,
            EPslnCmdAppActivate);
    }
    else
    {
        CPslnFWBaseView::SetMiddleSoftKeyLabelL(
            R_PSLN_MSK_DUMMY,
            EPslnCmdEmptyCommand );
    }
}

// -----------------------------------------------------------------------------
// Return engine instance.
// -----------------------------------------------------------------------------
//
CPslnActiveIdlePluginEngine* CPslnActiveIdlePlugin::Engine()
    {
    return iEngine;
    }

// ----------------------------------------------------------------------------
// CPslnActiveIdlePlugin::DoActivateL
//
// First method called by the Avkon framwork to invoke a view.
// ----------------------------------------------------------------------------
//
void CPslnActiveIdlePlugin::DoActivateL(
    const TVwsViewId& aPrevViewId,
    TUid aCustomMessageId,
    const TDesC8& aCustomMessage)
{
#ifdef MY_DEBUG
    RDebug::Print(_L("XAI: CPslnActiveIdlePlugin::DoActivateL"));
    RDebug::Print(_L("XAI:   aCustomMessageId = 0x%08x"), aCustomMessageId.iUid);
    RDebug::Print(_L("XAI:   aPrevViewId      = 0x%08x"), aPrevViewId.iAppUid.iUid);
#endif

    // If called from Psln - set tab group location.
    if (iAppUi->View(KPslnMainViewUid))
    {
        CPslnFWBaseView::SetNaviPaneL(KPslnActiveIdlePluginLocation);
    }

    CEikButtonGroupContainer* cba = Cba();

    if (cba)
    {
        if (aCustomMessageId == KUidActiveIdle)
        {
            cba->SetCommandSetL(R_SOFTKEYS_OPTIONS_EXIT_EMPTY);
        }
        else
        {
            cba->SetCommandSetL(R_SOFTKEYS_OPTIONS_BACK_EMPTY);
        }
        cba->DrawDeferred();
    }

    CPslnFWBaseView::DoActivateL(aPrevViewId, aCustomMessageId, aCustomMessage);

    CheckMiddleSoftkeyLabelL();
    
    iEngine->ManualGetSkinsRestartL();
}

// ----------------------------------------------------------------------------
// CPslnActiveIdlePlugin::DoDeactivate
//
// Called by the Avkon view framework when closing.
// ----------------------------------------------------------------------------
//
void CPslnActiveIdlePlugin::DoDeactivate()
{
#ifdef _MY_DEBUG
    RDebug::Print(_L("XAI: CPslnActiveIdlePlugin::DoDeactivate"));
#endif
    CPslnFWBaseView::DoDeactivate();
    RemoveCommandFromMSK();
}

// ----------------------------------------------------------------------------
// CPslnActiveIdlePlugin::DynInitMenuPaneL
//
//
// ----------------------------------------------------------------------------
//
void CPslnActiveIdlePlugin::DynInitMenuPaneL(
    TInt aResourceId, CEikMenuPane* aMenuPane)
{
    if (aResourceId == R_PSLN_GEN_VIEW_MENUPANE &&
        aMenuPane)
    {
        // Since this is common resource it contains download,
        // set it off.
        aMenuPane->SetItemDimmed(EPslnCmdAppDownload, ETrue);

        CPslnActiveIdlePluginContainer* container = Container();
        TInt highlightedItem = iContainer->iListBox->CurrentItemIndex();
        // Remove Activate command, if highlighted item is already active.
        if (highlightedItem == container->GetCurrentlySelectedIndex())
        {
            aMenuPane->SetItemDimmed(EPslnCmdAppActivate, ETrue);
        }
    }
    else if (aResourceId == R_PSLN_AI_BASIC_MENUPANE &&
             aMenuPane)
    {
        if (!FeatureManager::FeatureSupported(KFeatureIdHelp))
        {
            // Disable help if not supported
            aMenuPane->SetItemDimmed(EPslnCmdAppHelp, ETrue);
        }
    }
}

// ---------------------------------------------------------------------------
// CPslnActiveIdlePlugin::NewContainerL()
//
// Creates new iContainer.
// ---------------------------------------------------------------------------
//
void CPslnActiveIdlePlugin::NewContainerL()
{
    if( !iContainer )
        {
        TBool isGSCalling = (NULL != iAppUi->View(KGSMainViewUid));
        iContainer = new (ELeave) CPslnActiveIdlePluginContainer(isGSCalling, this);
        iContainer->SetMiddleSoftkeyObserver(this);
        }

    RemoveCommandFromMSK();
}

// ---------------------------------------------------------------------------
// CPslnActiveIdlePlugin::HandleListBoxSelectionL()
//
// Handles events raised through a rocker key.
// ---------------------------------------------------------------------------
void CPslnActiveIdlePlugin::HandleListBoxSelectionL()
{
    TInt currentItem = iContainer->iListBox->CurrentItemIndex();
    CPslnFWBaseView::SetCurrentItem(currentItem);

    CPslnActiveIdlePluginContainer* container = Container();

    if (currentItem != container->GetCurrentlySelectedIndex())
    {
        HandleCommandL(EPslnCmdAppActivate);
    }
}

// -----------------------------------------------------------------------------
// Remove MSK command mappings.
// This method should do nothing but MSK issues.
// -----------------------------------------------------------------------------
//
void CPslnActiveIdlePlugin::RemoveCommandFromMSK()
{
    CEikButtonGroupContainer* cba = Cba();

    if (cba && iContainer)
    {
        cba->RemoveCommandFromStack(KPslnMSKControlId, EPslnCmdEmptyCommand);
        cba->RemoveCommandFromStack(KPslnMSKControlId, EPslnCmdAppActivate);
    }
}

// ---------------------------------------------------------------------------
// CPslnActiveIdlePlugin::SetTitlePaneL()
// 
// Gives resource ID to be used as plugin view title.
// ---------------------------------------------------------------------------
//  
void CPslnActiveIdlePlugin::SetTitlePaneL( TInt& aResourceId )
    {
#ifdef RD_CONTROL_PANEL
    aResourceId = R_PSLN_AI_LIST_VIEW_CAPTION;
#endif // RD_CONTROL_PANEL
    }

// End of File.
