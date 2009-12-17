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
* Description:  Psln Active Idle settings plugin.
*
*/


#include "gsactiveidleplugin.h"
#include "gsactiveidleplugincontainer.h"

#include <pslnfwplugininterface.h>
#include <pslnfwpluginhandler.h>
#include <pslnactiveidlepluginrsc.rsg>
#include "pslnactiveidlepluginuids.h"

#include <coeaui.h>
#include <gulicon.h>
#include <bautils.h>
#include <eikfrlbd.h>
#include <stringloader.h>
#include <aknviewappui.h>

#include <gsfwviewuids.h>
#include <gsprivatepluginproviderids.h>

// GS Active Idle Plugin impl. UID
const TUid KGSActiveIdlePluginUid = { KGSActiveIdlePluginImplementationUID };

// Psln Active Idle Plugin impl. UID
const TUid KPslnAIPluginUid       = { KPslnActiveIdlePluginImplementationUID };

_LIT(KGSActiveIdlePluginResourceFileName, "z:pslnactiveidlepluginrsc.rsc");


// ----------------------------------------------------------------------------
// CGSActiveIdlePlugin::CGSActiveIdlePlugin()
//
// Constructor
// ----------------------------------------------------------------------------
//
CGSActiveIdlePlugin::CGSActiveIdlePlugin() : iResourceLoader(*iCoeEnv), iPrevViewId()
{
}

// ---------------------------------------------------------------------------
// CGSActiveIdlePlugin::NewL()
//
// Symbian OS default constructor
// ---------------------------------------------------------------------------
CGSActiveIdlePlugin* CGSActiveIdlePlugin::NewL(TAny* /*aInitParams*/)
{
    CGSActiveIdlePlugin* self = new (ELeave) CGSActiveIdlePlugin();

    CleanupStack::PushL(self);
    self->ConstructL();
    CleanupStack::Pop(self);

    return self;
}

// ---------------------------------------------------------------------------
// CGSActiveIdlePlugin::ConstructL()
//
// Symbian OS two-phased constructor
// ---------------------------------------------------------------------------
void CGSActiveIdlePlugin::ConstructL()
{
#ifdef _MY_DEBUG
    RDebug::Print(_L("XAI: CGSActiveIdlePlugin::ConstructL"));
#endif

    // Find the resource file.
    TParse parse;
    parse.Set(KGSActiveIdlePluginResourceFileName, &KDC_RESOURCE_FILES_DIR, NULL);
    TFileName fileName(parse.FullName());

    // Open resource file.
    iResourceLoader.OpenL(fileName);

    BaseConstructL();

    // Psln first finds and loads all plugins that implement Psln plugin interface.
    iPluginLoader = CPslnFWPluginHandler::NewL(AppUi());
    CPslnFWPluginInterface* plugin = iPluginLoader->LoadPluginL(KPslnAIPluginUid);
    if (plugin)
    {
        // Add Psln Active Idle plugin to appui views. Plugin is deleted
        // when appui is destroyed.
        if (plugin->Id() == KPslnAIPluginUid)
        {
            CAknView* view = static_cast<CAknView*>(plugin);
            AppUi()->AddViewL(view);
        }
    }
}

// ----------------------------------------------------------------------------
// CGSActiveIdlePlugin::~CGSActiveIdlePlugin
//
// Destructor
// ----------------------------------------------------------------------------
CGSActiveIdlePlugin::~CGSActiveIdlePlugin()
{
    if (iContainer)
    {
        AppUi()->RemoveFromStack(iContainer);
        delete iContainer;
        iContainer = NULL;
    }

    iResourceLoader.Close();

    delete iPluginLoader;
}

// ---------------------------------------------------------------------------
// TUid CGSActiveIdlePlugin::Id()
//
// Returns view's ID.
// ---------------------------------------------------------------------------
TUid CGSActiveIdlePlugin::Id() const
{
    return KGSActiveIdlePluginUid;
}

// ----------------------------------------------------------------------------
// CGSActiveIdlePlugin::GetCaption
//
// Return application/view caption. 256
// ----------------------------------------------------------------------------
//
void CGSActiveIdlePlugin::GetCaptionL(TDes& aCaption) const
{
    iCoeEnv->ReadResourceL(aCaption, R_PSLN_GS_LIST_VIEW_CAPTION);
}

// ----------------------------------------------------------------------------
// CGSActiveIdlePlugin::PluginProviderCategory
//
// A means to identify the location of this plug-in in the framework.
// ----------------------------------------------------------------------------
//
TInt CGSActiveIdlePlugin::PluginProviderCategory() const
{
    // To identify internal plug-ins.
    return KGSPluginProviderInternal;
}

// ----------------------------------------------------------------------------
// CGSActiveIdlePlugin::Visible
//
// Provides the visibility status of self to framework.
// ----------------------------------------------------------------------------
//
TBool CGSActiveIdlePlugin::Visible() const
{
#ifdef _MY_DEBUG
    RDebug::Print(_L("XAI: CGSActiveIdlePlugin::Visible"));
#endif

    return ETrue;
}

// ---------------------------------------------------------------------------
// CGSActiveIdlePlugin::HandleCommandL(TInt aCommand)
//
// Handles commands directed to this class.
// ---------------------------------------------------------------------------
void CGSActiveIdlePlugin::HandleCommandL(TInt aCommand)
{
    // shouldn't get any commands to handle, but forward them to appui anyway...
    AppUi()->HandleCommandL(aCommand);
}

// ----------------------------------------------------------------------------
// CGSActiveIdlePlugin::DoActivateL
//
// First method called by the Avkon framwork to invoke a view.
// ----------------------------------------------------------------------------
//
void CGSActiveIdlePlugin::DoActivateL(
    const TVwsViewId& aPrevViewId,
    TUid /*aCustomMessageId*/,
    const TDesC8& /*aCustomMessage*/)
{
#ifdef _MY_DEBUG
    RDebug::Print(_L("XAI: CGSActiveIdlePlugin::DoActivateL"));
#endif

    if (!iContainer)
    {
        iContainer = new (ELeave) CGSActiveIdlePluginContainer();
        iContainer->SetMopParent(this);
        iContainer->ConstructL(ClientRect());
    }

    AppUi()->AddToStackL(*this, iContainer);

    if (iPrevViewId.iViewUid.iUid == 0)
    {
        iPrevViewId = aPrevViewId;
    }

	iContainer->MakeVisible(ETrue);
    AppUi()->ActivateLocalViewL(KPslnAIPluginUid);
}

// ----------------------------------------------------------------------------
// CGSActiveIdlePlugin::DoDeactivate
//
// Called by the Avkon view framework when closing.
// ----------------------------------------------------------------------------
//
void CGSActiveIdlePlugin::DoDeactivate()
{
    AppUi()->RemoveFromStack(iContainer);
    iContainer->MakeVisible(EFalse);
    iPrevViewId.iViewUid.iUid = 0;
}

// End of File.
