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
* Description:  Container for the Active Idle plugin.
*
*/


// From this plugin.
#include <aisystemuids.hrh>

#include "pslnactiveidlepluginengine.h"
#include "pslnactiveidleplugincontainer.h"
#include "pslnactiveidleplugin.h"
#include "activeidle2domainpskeys.h"
#include "activeidle2internalpskeys.h"

// from plugin side
#include <extrstandbyscrconst.h> // for KExtrStandbyScrFullScreenMode

// From PSLN framework.
#include <pslnfwiconhelper.h>
#include <psln.rsg>

// General services.
#include <stringloader.h>
#include <aknglobalnote.h>
#include <e32property.h>

// Application specific skins.
#include <pslnfwappthemehandler.h>

// XUIKON.
#include <xnodt.h>

// ECOM.
#include <ecom/ecom.h>
#include <ecom/implementationinformation.h>

#include <centralrepository.h>
#include <activeidle2domaincrkeys.h>

#include <AknSgcc.h>

// Granularity of plugin array.
const TInt KPslnPluginArrayGranularity = 4;

// ActiveIdle1 plugin ECOM interface uid
const TInt KAI1PluginInterfaceUid = 0x101F8700;
const TInt KAIThemeStatusPaneHidden = 0x00000001;

// AI1 cenrep
const TInt KCRUidActiveIdle =  0x10207467;
const TInt KCRPluginCountKey = 0x00000002;
const TInt KCRFirstPluginKey = 0x00000003;

const TInt KPSLNOneSecondInMicroSeconds = 1000*1000;

// ========== LOCAL FUNCTIONS =================================================

template<class Array>
void CleanupResetAndDestroy(TAny* aObj)
{
    if(aObj)
    {
        static_cast<Array*>(aObj)->ResetAndDestroy();
    }
}

template<class Array>
void CleanupResetAndDestroyPushL(Array& aPointerArray)
{
    CleanupStack::PushL(TCleanupItem(&CleanupResetAndDestroy<Array>, &aPointerArray));
}

// ========== MEMBER FUNCTIONS ================================================

// ---------------------------------------------------------------------------
// CPslnActiveIdlePluginEngine::ConstructL()
//
// Symbian OS two phased constructor
// ---------------------------------------------------------------------------
//
void CPslnActiveIdlePluginEngine::ConstructL(const TRect& /*aRect*/)
{
#ifdef _MY_DEBUG
    RDebug::Print(_L("XAI: CPslnActiveIdlePluginEngine::ConstructL"));
#endif

    // Create application theme handler and list for application themes.
    iMySkinList = new (ELeave)
        CArrayPtrFlat<CXnODT>(KPslnPluginArrayGranularity);
    iPslnFWThemeHandler = CPslnFWAppThemeHandler::NewL(
        *this,
        *iMySkinList);

    CheckCurrentUIControllerL();
    CheckCurrentAi1PluginL();
    iRestartTimer = CPeriodic::NewL(EPriorityNormal);
    iUiRefreshTimer = CPeriodic::NewL(EPriorityNormal);
}

// ---------------------------------------------------------------------------
// CPslnActiveIdlePluginEngine::CPslnActiveIdlePluginEngine
//
// Constructor
// ---------------------------------------------------------------------------
//
CPslnActiveIdlePluginEngine::CPslnActiveIdlePluginEngine(
    CPslnActiveIdlePlugin* aPlugin)
  :
    iFirstUpdateRound(ETrue),
    iFirstUpdateRoundInjected(EFalse),
    iCurrentUIController(EAiUICUnknown),
    iPlugin(aPlugin)
{
}

// ---------------------------------------------------------------------------
// CPslnActiveIdlePluginEngine::~CPslnActiveIdlePluginEngine()
//
// Destructor
// ---------------------------------------------------------------------------
//
CPslnActiveIdlePluginEngine::~CPslnActiveIdlePluginEngine()
{
    if (iRestartTimer)
    {
        iRestartTimer->Cancel();
    }
    delete iRestartTimer;
    if (iUiRefreshTimer)
    {
        iUiRefreshTimer->Cancel();
    }
    delete iUiRefreshTimer;

    if (iMySkinList)
    {
        iMySkinList->ResetAndDestroy();
    }
    delete iMySkinList;
    delete iPslnFWThemeHandler;
}

// -----------------------------------------------------------------------------
// CPslnActiveIdlePluginEngine::ActivateThemeL
// -----------------------------------------------------------------------------
//
void CPslnActiveIdlePluginEngine::ActivateThemeL()
{
    TInt skinIndex = 0;
    if( iPlugin->Container() )
        {
        skinIndex = iPlugin->Container()->iListBox->CurrentItemIndex();
        }
    else
        {
        // No selection possible yet
        return;
        }

    if (skinIndex >= 0 && skinIndex < iMySkinList->Count() &&
        !iRestartTimer->IsActive())
    {
        if (skinIndex >= iHSStartIndex &&
            skinIndex < iHSEndIndex)
        {
            CXnODT* selectedSkinODT = iMySkinList->At(skinIndex);
            UpdateStatusPaneVisibilityCenrepL(
                selectedSkinODT->Flags() & KAIThemeStatusPaneHidden);

            if (iCurrentAi1Plugin != selectedSkinODT->ThemeUid())
            {
                UpdateAi1PluginLoadCenrepL(selectedSkinODT->ThemeUid());
                CheckCurrentAi1PluginL();
            }
            if (iCurrentUIController == EAiUICXML)
            {
                UpdateUIControllersInCenrepL(EAiUICNative);
                CheckCurrentUIControllerL();

                iRestartTimer->Cancel();
                iRestartRetryCount = 0;
                iRestartTimer->Start(
                    KPSLNOneSecondInMicroSeconds,
                    KPSLNOneSecondInMicroSeconds,
                    TCallBack(RestartTimerCallback, this));
            }
            else
            {
            if( IsAiRunning() )
                {
                // Recycle this method to bring to foreground
                RestartTimerCallback( this );
                }
            }
            ManualGetSkinsRestartL();
        }
        else
        {
            if (iCurrentUIController == EAiUICNative)
            {
                UpdateStatusPaneVisibilityCenrepL(EFalse); // Always visible status pane
                UpdateUIControllersInCenrepL(EAiUICXML);
                CheckCurrentUIControllerL();
                }

            RProperty::Set( KPSUidAiInformation, KActiveIdleRestartAI2, KActiveIdleRestartCode );
            iRestartTimer->Cancel();
            iRestartRetryCount = 0;
            iRestartTimer->Start(
                KPSLNOneSecondInMicroSeconds,
                KPSLNOneSecondInMicroSeconds,
                TCallBack(RestartTimerCallback, this));
            CXnODT* selectedSkinODT = iMySkinList->At(skinIndex);
            TXnServiceCompletedMessage ret;

            ret = iPslnFWThemeHandler->SetApplicationSkinL(
                *selectedSkinODT );

            if (ret == EXnSetActiveThemeFailed)
            {
                // Show error note.
                HBufC* errorBuf = StringLoader::LoadLC(
                    R_PSLN_QTN_SKINS_ERROR_CORRUPTED);
                // Display global note.
                CAknGlobalNote* errorNote = CAknGlobalNote::NewLC();
                errorNote->ShowNoteL(EAknGlobalErrorNote, *errorBuf);
                CleanupStack::PopAndDestroy(2, errorBuf); // errorNote also
            }
            else
            {
                iRestartTimer->Cancel();
                iRestartRetryCount = 0;
                iRestartTimer->Start(
                    KPSLNOneSecondInMicroSeconds,
                    KPSLNOneSecondInMicroSeconds,
                    TCallBack(RestartTimerCallback, this));
            }
        }
    }
}

// -----------------------------------------------------------------------------
// CPslnActiveIdlePluginEngine::HandleMessage
// -----------------------------------------------------------------------------
//
void CPslnActiveIdlePluginEngine::HandleMessage(
    TXnServiceCompletedMessage aMessage)
{
    switch (aMessage)
    {
    case EXnGetListHeadersEmpty:
        // Just destroy everything and use default items only.
    case EXnGetListHeadersFailed: // fallthrough
        if (iMySkinList)
        {
            iMySkinList->ResetAndDestroy();
        }
        break;
    case EXnServiceRequestCanceled: // fallthrough
    case EXnServiceRequestError: // fallthrough
        {
            TRAP_IGNORE(ManualGetSkinsRestartL();)
        }
        break;
    default:
        break;
    }
}

// -----------------------------------------------------------------------------
// CPslnActiveIdlePluginEngine::HandleMessage
// -----------------------------------------------------------------------------
//
void CPslnActiveIdlePluginEngine::HandleMessage(
    TXnServiceCompletedMessage aMessage,
    CArrayPtrFlat<CXnODT>& /*aAppThemeList*/)
{
    switch (aMessage)
    {
    case EXnGetListHeadersRestart:
    case EXnGetListHeadersUpdate: // fallthrough
        {
            TRAP_IGNORE(AddNewThemeL(aMessage));
            break;
        }
    default:
        {
            break;
        }
    }
}

// ---------------------------------------------------------------------------
// Adds new theme to the container / listbox.
// ---------------------------------------------------------------------------
//
void CPslnActiveIdlePluginEngine::AddThemesToContainerL()
    {
    TInt value  = 0;

    GetExtStandbyScreenState(value);

    /* value == 0  Only AI2 themes
       value != 0
         bit 31    AI2 themes
         bit 30-0  External themes
    */
    value |= 0x80000000;                               // Force AI2 themes on
    if ((value == 0) || ((value & 0x80000000) != 0))
        {
        if( iPlugin->Container() && iMySkinList )
            {
            // Remove themes. All.
            iPlugin->Container()->RemoveThemes( 0 );
            for ( TInt i = 0; i < iMySkinList->Count(); ++i )
                {
                if( i >= iHSStartIndex &&
                    i < iHSEndIndex )
                    {
                    iPlugin->Container()->AddNewItemToListL( i,
                                                             iMySkinList->At(i),
                                                             EAiUICNative );
                    }
                else
                    {
                    iPlugin->Container()->AddNewItemToListL( i,
                                                             iMySkinList->At(i),
                                                             EAiUICXML );
                    }
                }
            }
        }
    }

// ---------------------------------------------------------------------------
// Adds new theme to the listbox.
// ---------------------------------------------------------------------------
//
void CPslnActiveIdlePluginEngine::AddNewThemeL(
    TXnServiceCompletedMessage aMessage)
{
    // No themes available - do not add anything.
    if (!iMySkinList)
    {
        return;
    }

    // Theme activation causes restart of theme headers list.
    if (aMessage == EXnGetListHeadersRestart)
    {
    iFirstUpdateRound = EFalse;
    }

    TInt value  = 0;

    GetExtStandbyScreenState(value);

    /* value == 0  Only AI2 themes
       value != 0
         bit 31    AI2 themes
         bit 30-0  External themes
    */
    value |= 0x80000000;                               // Force AI2 themes on

    if ((value & 0x7FFFFFFF) > 0)
        {
    if ((iFirstUpdateRound && !iFirstUpdateRoundInjected) ||
        aMessage == EXnGetListHeadersRestart)
    {
        iFirstUpdateRoundInjected = ETrue;
        AddHomeScreenThemesL();
    }
        }

    UiTimerRestart();
}

//
// Get external Standby Screen state from Cenrep
//
TInt CPslnActiveIdlePluginEngine::GetExtStandbyScreenState(TInt& aValue )
    {
    TInt ret = 0;
    TUid uid    = { KCRUidActiveIdleLV }; // {0x10275102}; //

    //TRAP_IGNORE: leaving function called in non-leaving function
    TRAP_IGNORE
        (
        CRepository* cenRep = CRepository::NewLC( uid );

        ret = cenRep->Get(KAIExternalStatusScreen, aValue );

        CleanupStack::PopAndDestroy( cenRep );
        )

    return ret;
    }

// ---------------------------------------------------------------------------
// Add EXT HS themes to listing
// ---------------------------------------------------------------------------
//
void CPslnActiveIdlePluginEngine::AddHomeScreenThemesL()
{
    iHSStartIndex = iHSEndIndex = iMySkinList->Count();
    // Resolve AI1 plugins in ECOM
    RImplInfoPtrArray eComPlugins;
    CleanupResetAndDestroyPushL(eComPlugins);

    REComSession::ListImplementationsL(TUid::Uid(KAI1PluginInterfaceUid), eComPlugins);
    for (TInt i = 0; i < eComPlugins.Count(); ++i)
    {
        CXnODT* skinODT = CXnODT::NewL();
        skinODT->SetThemeUid(eComPlugins[i]->ImplementationUid().iUid);
        if (eComPlugins[i]->OpaqueData() == KExtrStandbyScrFullScreenMode)
        {
            skinODT->SetFlags(KAIThemeStatusPaneHidden);
        }
        else
        {
            skinODT->SetFlags(0);
        }
        CleanupStack::PushL(skinODT);
        iMySkinList->AppendL(skinODT);
        CleanupStack::Pop(skinODT);

        HBufC* item = HBufC::NewLC(KPslnItemMaxTextLength);
        TPtr itemPtr = item->Des();
        if (eComPlugins[i]->DisplayName().Length())
        {
            itemPtr = eComPlugins[i]->DisplayName();
        }
        else
        {
            itemPtr = _L("OperatorHomeScreen1");
        }
        skinODT->SetThemeFullNameL( itemPtr );
        if (iCurrentAi1Plugin == eComPlugins[i]->ImplementationUid().iUid &&
            iCurrentUIController == EAiUICNative)
        {
            itemPtr.Insert(0, KPslnFWActiveListItemFormat);
            TInt flags = skinODT->Flags();
            skinODT->SetFlags(flags | EXnThemeStatusActive );
        }
        else
        {
            itemPtr.Insert(0, KPslnFWNonActiveListItemFormat);
        }
        CleanupStack::PopAndDestroy(item);
        ++iHSEndIndex;
    }
    CleanupStack::PopAndDestroy();
}

// ---------------------------------------------------------------------------
// Check active AI2 UI Controller in cenrep keys
// ---------------------------------------------------------------------------
//
void CPslnActiveIdlePluginEngine::CheckCurrentUIControllerL()
{
    CRepository* cenRep = CRepository::NewL(TUid::Uid(KCRUidActiveIdleLV)); // KCRUidActiveIdleLV AI2 Cenrep!
    TInt value = 0;
    if (cenRep->Get(KAiMainUIController, value) == KErrNone)
    {
        if ((value == AI_UID_ECOM_IMPLEMENTATION_UICONTROLLER_NATIVE)
            || (value == AI3_UID_ECOM_IMPLEMENTATION_UICONTROLLER_NATIVE))
        {
            iCurrentUIController = EAiUICNative;
        }
        else if ((value == AI_UID_ECOM_IMPLEMENTATION_UICONTROLLER_XML)
            || (value == AI3_UID_ECOM_IMPLEMENTATION_UICONTROLLER_XML))
        {
            iCurrentUIController = EAiUICXML;
        }
    }
    delete cenRep;
}

// ---------------------------------------------------------------------------
// Update active AI2 UI Controller in cenrep keys
// ---------------------------------------------------------------------------
//
void CPslnActiveIdlePluginEngine::UpdateUIControllersInCenrepL(TInt aControllerId)
{
    if (iCurrentUIController != aControllerId)
    {
        TUid uid = { KCRUidActiveIdleLV }; // KCRUidActiveIdleLV AI2 Cenrep!
        CRepository* cenRep = CRepository::NewL(uid);
        if (aControllerId == EAiUICNative)
        {
            cenRep->Delete(KAiFirstUIController);
            cenRep->Delete(KAiFirstUIController + 1);
            cenRep->Set(KAiMainUIController, AI_UID_ECOM_IMPLEMENTATION_UICONTROLLER_NATIVE);
        }
        else if (aControllerId ==  EAiUICXML)
        {
            cenRep->Create(KAiFirstUIController, AI_UID_ECOM_IMPLEMENTATION_UICONTROLLER_NATIVE);
            cenRep->Set(KAiFirstUIController, AI_UID_ECOM_IMPLEMENTATION_UICONTROLLER_NATIVE);
            cenRep->Delete(KAiFirstUIController + 1);
            cenRep->Set(KAiMainUIController, AI_UID_ECOM_IMPLEMENTATION_UICONTROLLER_XML);
        }
        delete cenRep;
    }
}

// ---------------------------------------------------------------------------
// Update status pane visibility for status pane cenrep key
// ---------------------------------------------------------------------------
//
void CPslnActiveIdlePluginEngine::UpdateStatusPaneVisibilityCenrepL(
    TBool aPaneHidden)
{
    TInt value = EAiStatusPaneLayoutIdleNormal;
    if (aPaneHidden)
    {
        value = EAiStatusPaneLayoutIdleHidden;
    }
    TUid uid = { KCRUidActiveIdleLV }; // KCRUidActiveIdleLV AI2 Cenrep!
    CRepository* cenRep = CRepository::NewL(uid);
    cenRep->Set(KAiStatusPaneLayout, value);
    delete cenRep;
}

// ---------------------------------------------------------------------------
// Update active AI1 plugin in cenrep
// ---------------------------------------------------------------------------
//
void CPslnActiveIdlePluginEngine::UpdateAi1PluginLoadCenrepL(TInt aNewUid)
{
    TUid uid = { KCRUidActiveIdle }; // KCRUidActiveIdle AI1 Cenrep!
    CRepository* cenRep = CRepository::NewL(uid);
    TInt currentCount = 0;
    cenRep->Get(KCRPluginCountKey, currentCount);

    // Delete old keys if any
    for (TInt i = 0; i < currentCount; ++i)
    {
        cenRep->Delete(KCRFirstPluginKey+i);
    }

    cenRep->Delete(KCRPluginCountKey);
    cenRep->Create(KCRPluginCountKey, 1);
    // Set only 1 new key, the selected one
    cenRep->Create(KCRFirstPluginKey, aNewUid);

    delete cenRep;
}

// ---------------------------------------------------------------------------
// Check the current AI1 plugin (ref. External opretor homescreen feature)
// ---------------------------------------------------------------------------
//
void CPslnActiveIdlePluginEngine::CheckCurrentAi1PluginL()
{
    TUid uid = { KCRUidActiveIdle }; // KCRUidActiveIdle AI1 Cenrep!
    TInt value = 0;
    TInt currentCount = 0;

    CRepository* cenRep = CRepository::NewL(uid);
    cenRep->Get(KCRPluginCountKey, currentCount);

    if (currentCount) // count must be set
    {
        cenRep->Get(KCRFirstPluginKey, value);
    }
    delete cenRep;

    if (value != 0)
    {
        iCurrentAi1Plugin = value;
    }
}

// ---------------------------------------------------------------------------
// Manually start updating theme/skin listing
// ---------------------------------------------------------------------------
//
void CPslnActiveIdlePluginEngine::ManualGetSkinsRestartL(TBool aForceStart)
{
    if( !aForceStart && iMySkinList->Count() > 0 &&
        iUiRefreshTimer->IsActive() )
        {
        return;
        }
        
    // Restart get skins manually!
    if( iPlugin->Container() )
        {
        iPlugin->Container()->RemoveThemes(0);
        }
    iMySkinList->ResetAndDestroy();
    iFirstUpdateRound = ETrue;
    iFirstUpdateRoundInjected = EFalse;
    iPslnFWThemeHandler->CancelGetApplicationSkins();
    iAddIndex = 0;
    iPslnFWThemeHandler->GetApplicationSkinsL(KUidActiveIdle);
}

// ---------------------------------------------------------------------------
// Restart AI2 process
// ---------------------------------------------------------------------------
//
TInt CPslnActiveIdlePluginEngine::RestartAifw()
{
    if (!IsAiRunning())
    {
        _LIT(KAiExeName, "z:\\sys\\bin\\ailaunch_hs.exe");
        RProcess process;
        TInt ret = process.Create(KAiExeName, KNullDesC);
        process.Resume();
        process.Close();
        return ret;
    }
    return KErrNone;
}

// ---------------------------------------------------------------------------
// Checks if AI2 process is running
// ---------------------------------------------------------------------------
//
TBool CPslnActiveIdlePluginEngine::IsAiRunning()
{
    TApaTaskList taskList(CCoeEnv::Static()->WsSession());
    TApaTask startTask = taskList.FindApp(TUid::Uid(0x100058f4));

    if (!startTask.Exists()) // if first boot queries are done continue bringing ai2 to foreground.
    {
        TApaTask aiTask = taskList.FindApp(TUid::Uid(AI_UID3_AIFW_EXE));
        if (aiTask.Exists())  // App open
        {
            return ETrue;
        }
    }
    return EFalse;
}

// ---------------------------------------------------------------------------
// start ui refresh timer
// ---------------------------------------------------------------------------
//
void CPslnActiveIdlePluginEngine::UiTimerRestart()
    {
    iUiRefreshTimer->Cancel();
    iUiRefreshTimer->Start(
        KPSLNOneSecondInMicroSeconds/4, // quarter second delay
        KPSLNOneSecondInMicroSeconds/4,
        TCallBack(UiRefreshTimerCallback, this));
    }
    
// ---------------------------------------------------------------------------
// Call back for restart timer
// ---------------------------------------------------------------------------
//
TInt CPslnActiveIdlePluginEngine::RestartTimerCallback(TAny* aSelf)
{
    CPslnActiveIdlePluginEngine* self =
        static_cast<CPslnActiveIdlePluginEngine*>(aSelf);

    if (self)
    {
        ++(self->iRestartRetryCount);
        if ( self->RestartAifw() == KErrNone)
        {
            self->iRestartTimer->Cancel();
            TApaTaskList taskList(CCoeEnv::Static()->WsSession());
            TApaTask startTask = taskList.FindApp(TUid::Uid(0x100058f4));

            if (!startTask.Exists()) // if first boot queries are done continue bringing ai2 to foreground.
            {
                TApaTask aiTask = taskList.FindApp(TUid::Uid(AI_UID3_AIFW_EXE));
                if (aiTask.Exists())  // App open
                {
                    //aiTask.BringToForeground();
                    CAknSgcClient::MoveApp(aiTask.WgId(), ESgcMoveAppToForeground);
                }
            }
            CAknEnv::RunAppShutter();
            return 0;
        }
        if (self->iRestartRetryCount > 3)
        {
            self->iRestartTimer->Cancel();
            TRAP_IGNORE(self->ManualGetSkinsRestartL());
            return 0;
        }
    }
    return 1;
}

// ---------------------------------------------------------------------------
// Call back for UI refresh timer timer
// ---------------------------------------------------------------------------
//
TInt CPslnActiveIdlePluginEngine::UiRefreshTimerCallback(TAny* aSelf)
{
    CPslnActiveIdlePluginEngine* self =
        static_cast<CPslnActiveIdlePluginEngine*>(aSelf);

    if (self)
    {
        self->iUiRefreshTimer->Cancel();
        TRAP_IGNORE(self->AddThemesToContainerL());
        if( self->iPlugin->Container() )
            {
            self->iPlugin->Container()->RefreshList();
            }
    }
    return 0;
}

// End of File.
