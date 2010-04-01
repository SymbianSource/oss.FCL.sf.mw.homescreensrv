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
* Description:  Shortcut settings plug-in main class.
*
*/


#include <e32std.h>
#include <eikmenup.h>
#include <eikbtgpc.h>
#include <StringLoader.h>
#include <gsfwviewuids.h>
#include <gsprivatepluginproviderids.h>
#include <bautils.h>
#include <pathinfo.h>
#include <hlplch.h>
#include <featmgr.h>
#include <e32property.h>                // For RProperty
#include <activeidle2domainpskeys.h>    // For KPSUidActiveIdle2

#include <avkon.rsg>
#include <aiscutsettingsres.rsg>
#include <aiscuttexts.rsg>

#include "hs_app_aiscutsettings.hrh"
#include "hs_app_caiscutsettings.h"
#include "hs_app_caiscutsettingsitem.h"
#include "hs_app_caiscutsettingscontainer.h"
#include "hs_app_caiscutsettingsmodel.h"
#include <platform/mw/aiscutuids.hrh>
#include "aiscutdefs.h"
#include <aisystemuids.hrh>

const TUid KUidScutSettingsPlugin =
{
    AI_UID_ECOM_IMPLEMENTATION_SETTINGS_SCUTPLUGIN
};

_LIT(KScutSettingsResourceFileName, "hs_app_aiscutsettingsres.rsc");
_LIT(KScutTextsResourceFileName,    "hs_app_aiscuttexts.rsc");
_LIT8(KScutDirectOpenTag,    "?open");


// ======== LOCAL FUNCTIONS ========


static void ParseKeysL( const TDesC8& aCustomMessage, RArray<TUint32>& aKeys, TBool& aOpen )
    {
    HBufC8* message = aCustomMessage.AllocLC();
    
    TPtr8 ptr( message->Des() );
            
    TInt pos;
    TInt openPos;
    TBool openTag=EFalse;
    
    do
        {                
        pos = ptr.Locate( ',' );
        
        if( pos != KErrNotFound )
            {
            const TDesC8& str1 = ptr.Left( pos );
            const TDesC8& str2 = str1.Right( str1.Length() - 2 );
            
            TUint32 value;
            TLex8 lex( str2 );
                        
            if( lex.Val( value, EHex ) == KErrNone )
                {
                aKeys.AppendL( value );
                }
                                    
            ptr = ptr.MidTPtr( pos + 1 );
            }
        else
            {
            const TDesC8& str1 = ptr;
            const TDesC8& str2 = str1.Right( str1.Length() - 2 );
            openPos = ptr.Find( KScutDirectOpenTag );
            TLex8 lex;
            if( openPos != KErrNotFound )
                {
                openTag=ETrue;
                const TDesC8& str3 = str2.Left( openPos -2 );
                lex.Assign( str3 );
                }
            else
                {
                lex.Assign( str2 );
                }
            TUint32 value;
            
            
            if( lex.Val( value, EHex ) == KErrNone )
                {
                aKeys.AppendL( value );
                }                        
            }                                           
        }
    while( pos != KErrNotFound );        
            
    if( openTag && aKeys.Count() == 1 )
        {
        aOpen = ETrue;
        }
    CleanupStack::PopAndDestroy();
    }
// ======== MEMBER FUNCTIONS ========

// ---------------------------------------------------------------------------
//
// ---------------------------------------------------------------------------
//
CAiScutSettings::CAiScutSettings() : iResourceLoaderTexts(*iCoeEnv)
{
}

// ---------------------------------------------------------------------------
//
// ---------------------------------------------------------------------------
//
void CAiScutSettings::ConstructL()
{
    FeatureManager::InitializeLibL();

    TParsePtrC driveParse(PathInfo::RomRootPath());
    TFileName resourceName(driveParse.Drive());

    // Find the resource file.
    TParse parse;
    parse.Set(KScutSettingsResourceFileName, &KDC_RESOURCE_FILES_DIR, NULL);
    resourceName.Append(parse.FullName());

    // Open resource file.
    iResourceLoader.OpenL(resourceName);

    resourceName.Copy(driveParse.Drive());
    parse.Set(KScutTextsResourceFileName, &KDC_RESOURCE_FILES_DIR, NULL);
    resourceName.Append(parse.FullName());

    // Open resource file.
    iResourceLoaderTexts.OpenL(resourceName);

    iModel = CAiScutSettingsModel::NewL(*this, iCoeEnv);
    
    iListBoxTimer = CIdle::NewL(CActive::EPriorityHigh);


    BaseConstructL(R_SCUT_SETTINGS_VIEW);
}

// ---------------------------------------------------------------------------
//
// ---------------------------------------------------------------------------
//
CAiScutSettings* CAiScutSettings::NewL(TAny* /*aInitParams*/)
{
    CAiScutSettings* self = new (ELeave) CAiScutSettings;
    CleanupStack::PushL(self);
    self->ConstructL();
    CleanupStack::Pop(self);
    return self;
}

// ---------------------------------------------------------------------------
//
// ---------------------------------------------------------------------------
//
CAiScutSettings::~CAiScutSettings()
{
    FeatureManager::UnInitializeLib();
    
    if( iListBoxTimer )
        {
        iListBoxTimer->Cancel();
        delete iListBoxTimer;
        }    

    iResourceLoader.Close();
    iResourceLoaderTexts.Close();

    delete iModel;
}

// ---------------------------------------------------------------------------
// From CAknView
// Returns view id.
// ---------------------------------------------------------------------------
//
TUid CAiScutSettings::Id() const
{
    return KUidScutSettingsPlugin;
}

// ---------------------------------------------------------------------------
// From CAknView
// Handles commands.
// ---------------------------------------------------------------------------
//
void CAiScutSettings::HandleCommandL(TInt aCommand)
{
    switch (aCommand)
    {
    case EAiScutSettingsCmdChange:
        Container()->HandleChangeCommandL();
        break;

    case EAknCmdHelp:
	/*
        TUid fwUid = TUid::Uid( AI_UID3_AIFW_COMMON );
        TCoeContextName helpString;
        helpString.Copy( KSET_HLP_HOME_SCREEN_SHORTCUTS );
        
        CArrayFixFlat<TCoeHelpContext>* array = 
            new (ELeave) CArrayFixFlat<TCoeHelpContext>(1);
        CleanupStack::PushL( array );
        array->AppendL( TCoeHelpContext( fwUid, helpString ) );
        HlpLauncher::LaunchHelpApplicationL( CCoeEnv::Static()->WsSession(), array );
        CleanupStack::Pop( array );
	*/
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
void CAiScutSettings::DoActivateL(const TVwsViewId& aPrevViewId, TUid aCustomMessageId, const TDesC8& aCustomMessage)
{
#ifdef MY_DEBUG
    RDebug::Print(_L("XAI: CAiScutSettings::DoActivateL"));
    RDebug::Print(_L("XAI:   aCustomMessageId = 0x%08x"), aCustomMessageId.iUid);
    RDebug::Print(_L("XAI:   aPrevViewId      = 0x%08x"), aPrevViewId.iAppUid.iUid);
#endif

    CEikButtonGroupContainer* cba = Cba();

    if (cba)
    {
        if (aCustomMessageId == KScutActiveIdleUid)
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

    TBool open=EFalse;
    if( aCustomMessage != KNullDesC8 )
        {
        RArray<TUint32> keys;
        CleanupClosePushL( keys );

        ParseKeysL( aCustomMessage, keys, open );
        
        iModel->SetSettingsKeys( keys );
        
        CleanupStack::PopAndDestroy();
        }
    
    iModel->UpdateSettingsContainerL();

    if( open )
        {
        /* Let idle timer do this job. */
        iListBoxTimer->Cancel();
        iListBoxTimer->Start(TCallBack(DoHandleListBoxSelectionL, this));
        }
}

// ----------------------------------------------------------------------------
// From CAknView
// Called by the Avkon view framework when closing.
// ----------------------------------------------------------------------------
//
void CAiScutSettings::DoDeactivate()
{
    iModel->ActivateObservers(EFalse);

    // deletes iContainer.
    CGSBaseView::DoDeactivate();

    iModel->SetContainer(Container());
}

void CAiScutSettings::HandleForegroundEventL(TBool /*aForeground*/)
    {
    // No implementation required
    }

// ----------------------------------------------------------------------------
// From MEikMenuObserver
// ----------------------------------------------------------------------------
//
void CAiScutSettings::DynInitMenuPaneL(
    TInt aResourceId, CEikMenuPane* aMenuPane)
{
    if (aMenuPane && aResourceId == R_SCUTSETTINGS_MENUPANE)
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
void CAiScutSettings::GetCaptionL(TDes& aCaption) const
{
    iCoeEnv->ReadResourceL(aCaption, R_SCUT_SETTINGS_VIEW_CAPTION);
}

// ---------------------------------------------------------------------------
// From CGSPluginInterface
// ---------------------------------------------------------------------------
//
TInt CAiScutSettings::PluginProviderCategory() const
{
    return KGSPluginProviderInternal;
}

// ---------------------------------------------------------------------------
// From CGSPluginInterface
// ---------------------------------------------------------------------------
//
TBool CAiScutSettings::Visible() const
{
    return EFalse;
}

// ---------------------------------------------------------------------------
// From CGSBaseView
// Returns view id.
// ---------------------------------------------------------------------------
//
CAiScutSettingsContainer* CAiScutSettings::Container()
{
    return static_cast<CAiScutSettingsContainer*>(iContainer);
}

// ---------------------------------------------------------------------------
// From CGSBaseView
// ---------------------------------------------------------------------------
//
void CAiScutSettings::NewContainerL()
{
    delete iContainer;
    iContainer = NULL;
    
    iContainer = new (ELeave) CAiScutSettingsContainer();
    Container()->SetModel(iModel);
    iModel->SetContainer(Container());
}

// ---------------------------------------------------------------------------
// From CGSBaseView
// ---------------------------------------------------------------------------
//
void CAiScutSettings::HandleListBoxSelectionL()
{
    Container()->HandleChangeCommandL();
}

// ---------------------------------------------------------------------------
// ---------------------------------------------------------------------------
//
TBool CAiScutSettings::Activated() const
    {
    return iContainer ? ETrue : EFalse;
    }

// -----------------------------------------------------------------------------
// CActiveIdleState::DoUpdateSaSetting
// -----------------------------------------------------------------------------
//
TInt CAiScutSettings::DoHandleListBoxSelectionL(TAny* aAny)
    {
    CAiScutSettings* self = static_cast<CAiScutSettings*>(aAny);
    if(self)
        {
        self->iListBoxTimer->Cancel();
        self->HandleListBoxSelectionL();
        }
    return KErrNone;
    }


// End of File.
