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
* Description:  Shortcut settings plug-in container.
*
*/


#include <aknlists.h>
#include <AknWaitDialog.h>
#include <aknnotewrappers.h>
#include <eikbtgpc.h>
#include <gslistbox.h>
#include <gsfwviewuids.h>
#include <aknradiobuttonsettingpage.h>
#include <akntextsettingpage.h>
#include <centralrepository.h>
#include <StringLoader.h>

#include <csxhelp/ai.hlp.hrh>
#include <hlplch.h>

// For CActiveFavouritesDbNotifier
#include <activefavouritesdbnotifier.h> 
#include <mcspluginsettingsres.rsg>

#include "mcspluginsettingscontainer.h"
#include "mcspluginsettingsmodel.h"
#include "mcspluginsettingsbkmlist.h"
#include "mcspluginsettings.hrh"
#include "mcspluginuids.hrh"

_LIT( KMyMenuData, "matrixmenudata" );

// ======== MEMBER FUNCTIONS ========

// ---------------------------------------------------------------------------
// First-phase construction
// ---------------------------------------------------------------------------
//
CMCSPluginSettingsContainer::CMCSPluginSettingsContainer()
{
}

// ---------------------------------------------------------------------------
// Second-phase construction
// ---------------------------------------------------------------------------
//
void CMCSPluginSettingsContainer::ConstructL( const TRect& aRect )
    {
    iListBox = new (ELeave) CAknSettingStyleListBox;
    BaseConstructL(aRect, R_AI_MCS_SETTINGS_VIEW_TITLE, NULL);
    StartObservingL();
    CheckMiddleSoftkeyLabelL();
    }

// ---------------------------------------------------------------------------
// Tells the settings container to start observing for changes
// ---------------------------------------------------------------------------
//
void CMCSPluginSettingsContainer::StartObservingL()
    {
    // registering to bookmarks db. changes observing
    User::LeaveIfError( iBookmarkSession.Connect() );
    User::LeaveIfError( iBookmarkDb.Open( iBookmarkSession, KBrowserBookmarks ) );

    iBookmarkDbObserver = new (ELeave) CActiveFavouritesDbNotifier(
                                        iBookmarkDb, *this );
    iBookmarkDbObserver->Start();

    // registering to mailbox db. changes observing
    iMsvSession = CMsvSession::OpenAsObserverL( *this );
    
    // registering to matrix menu events observing
    iMenu.OpenL( KMyMenuData );
    TInt err = iNotifier.Open( iMenu ); 
    if ( err == KErrNone )
        {
        iNotifyWatcher = CMCSPluginWatcher::NewL( CMCSPluginWatcher::ENotify );
        iNotifier.Notify( 0,
            RMenuNotifier::EItemsAddedRemoved | 
            RMenuNotifier::EItemsReordered |
            RMenuNotifier::EItemAttributeChanged,
        iNotifyWatcher->iStatus );
        iNotifyWatcher->WatchNotify( this );
        }
    }

// ---------------------------------------------------------------------------
// Tells the settings container to stop observing for changes
// ---------------------------------------------------------------------------
//
void CMCSPluginSettingsContainer::StopObserving()
    {
    if( iBookmarkDbObserver )
        {
        delete iBookmarkDbObserver;
        iBookmarkDbObserver = NULL;
        }
    iBookmarkDb.Close();
    iBookmarkSession.Close();

    if( iMsvSession )
        {
        delete iMsvSession;
        iMsvSession = NULL;
        }
    
    iNotifier.Close();
    iMenu.Close();
    
    if ( iNotifyWatcher )
        {
        if ( iNotifyWatcher->IsActive() )
        	{
            iNotifyWatcher->Cancel();
        	}
        delete iNotifyWatcher;
        iNotifyWatcher = NULL;
        }
    }

// ---------------------------------------------------------------------------
// Destructor
// ---------------------------------------------------------------------------
//
CMCSPluginSettingsContainer::~CMCSPluginSettingsContainer()
    {
    StopObserving();
    }

// ---------------------------------------------------------------------------
// Sets pointer to settings plug-in model.
// ---------------------------------------------------------------------------
//
void CMCSPluginSettingsContainer::SetModel( CMCSPluginSettingsModel* aModel )
{
    iModel = aModel;
}

// ---------------------------------------------------------------------------
// Handles a setting change command.
// ---------------------------------------------------------------------------
//
void CMCSPluginSettingsContainer::HandleChangeCommandL()
{       
    TInt current = iListBox->CurrentItemIndex();
    TInt appListIndex = iModel->ItemId( current );
    if ( appListIndex < -1 )
    {
        appListIndex = -1;
    }

    if ( iModel->Item( current ).locked )
        {
        HBufC* text = iCoeEnv->AllocReadResourceLC( R_AI_MCS_SETTINGS_TXT_FIXED_ITEM );
        CAknInformationNote* note = new ( ELeave ) CAknInformationNote( ETrue );
        note->ExecuteLD( *text );
        CleanupStack::PopAndDestroy( text );
        }
    else
        {
        TInt selection( KErrNotFound );
        TBool changed( EFalse );
        CDesCArrayFlat* array = iCoeEnv->ReadDesC16ArrayResourceL(
            R_AI_MCS_CHANGE_TO_PAGE_LBX );
        CleanupStack::PushL( array );

        CAknListQueryDialog* dialog = new ( ELeave ) CAknListQueryDialog( &selection );
        CleanupStack::PushL( dialog );
        dialog->PrepareLC( R_AI_MCS_LISTQUERY_CHANGE_TO_PAGE );
        CleanupStack::Pop( dialog );
        dialog->SetItemTextArray( array );
        dialog->SetOwnershipType( ELbmDoesNotOwnItemArray );
        if ( dialog->RunLD() )
            {

            TSettingItem itm = iModel->Item( current );

            // user wants to select bookmark
            if ( selection == 1 )
                {

                // if the last selected item was application, set selected item to -1
                if ( itm.type == EApplication )
                    {
                        appListIndex = -1;
                    }

                changed = HandleBookmarkChangeCommandL( appListIndex, current );

                }
            // user wants to select application
            else if ( selection == 0 )
                {

                // if the last selected item was bookmark, set selected item to -1
                if ( itm.type == EBookmark )
                    {
                        appListIndex = -1;
                    }

                changed = HandleAppListChangeCommandL( appListIndex, current );

                }
            if ( changed )
                {
                    ResetCurrentListL( current );
                }

            }
        CleanupStack::PopAndDestroy( array );
        }
    }

// ---------------------------------------------------------------------------
// Handles a help command.
// ---------------------------------------------------------------------------
//
void CMCSPluginSettingsContainer::HandleHelpCommandL()
{
    CArrayFixFlat<TCoeHelpContext>* array = 
        new ( ELeave ) CArrayFixFlat<TCoeHelpContext>( 1 );
    CleanupStack::PushL( array );
    array->AppendL( GetHelpContext() );
    HlpLauncher::LaunchHelpApplicationL( CCoeEnv::Static()->WsSession(), array );
    CleanupStack::Pop( array );
}

// ---------------------------------------------------------------------------
// Helper method which indicates if the Applist or Bkmlist is showing
// ---------------------------------------------------------------------------
//
TBool CMCSPluginSettingsContainer::IsChangeDialogShowing()
{
    return ( iAppListDialog || iBkmListDialog );
}

// ---------------------------------------------------------------------------
// Method for closing change dialog (app or bkm) if it is beeing shown
// ---------------------------------------------------------------------------
//
void CMCSPluginSettingsContainer::CloseChangeDialog()
{
    TKeyEvent keyEvent;

    keyEvent.iCode      = EKeyEscape;
    keyEvent.iScanCode  = EStdKeyEscape;
    keyEvent.iModifiers = 0;
    keyEvent.iRepeats   = 0;

    CCoeControl* dialog = NULL;

    if ( iAppListDialog )
    {
        dialog = static_cast< CCoeControl* >( iAppListDialog );
    }
    else if ( iBkmListDialog )
    {
        dialog = static_cast< CCoeControl* >( iBkmListDialog );
    }

    if ( dialog )
    {
        TRAP_IGNORE( dialog->OfferKeyEventL( keyEvent, EEventKey ) );
    }
}

// ---------------------------------------------------------------------------
// Refreshes current list..
// ---------------------------------------------------------------------------
//
void CMCSPluginSettingsContainer::ResetCurrentListL( TInt aIndex )
{
    if (iModel->MdcaCount() == 0)
    {
        iListBox->HandleItemRemovalL();
    }
    else
    {
        iListBox->HandleItemAdditionL();
    }

    iListBox->SetCurrentItemIndex( aIndex );
    CheckMiddleSoftkeyLabelL();
}

// ---------------------------------------------------------------------------
// From class MMsvSessionObserver.
// Handles an event from the message server.
// ---------------------------------------------------------------------------
//
void CMCSPluginSettingsContainer::HandleSessionEventL(
                                                     TMsvSessionEvent aEvent,
                                                     TAny* /*aArg1*/,
                                                     TAny* /*aArg2*/,
                                                     TAny* /*aArg3*/ )
    {
    switch ( aEvent )
        {
    case EMsvEntriesCreated:
        // fall-through intended here
    case EMsvEntriesDeleted:
        // fall-through intended here
    case EMsvEntriesChanged:
        {
        if ( IsChangeDialogShowing() )
            {
            CloseChangeDialog();
            }
        iModel->UpdateAppListL();
        }
        break;
    default:
        break;
        }
    }

// ---------------------------------------------------------------------------
// From CGSBaseContainer
// Constructs the settings listbox.
// ---------------------------------------------------------------------------
//
void CMCSPluginSettingsContainer::ConstructListBoxL(TInt /*aResLbxId*/)
{
    iListBox->ConstructL(this, EAknListBoxSelectionList);

    // Set empty listbox's text.
    HBufC* text = iCoeEnv->AllocReadResourceLC(R_AI_MCS_SETTINGS_TXT_ALL_FIXED);
    iListBox->View()->SetListEmptyTextL(*text);
    CleanupStack::PopAndDestroy(text);

    iListBox->Model()->SetItemTextArray(iModel);
    iListBox->Model()->SetOwnershipType(ELbmDoesNotOwnItemArray);
}

// ---------------------------------------------------------------------------
// Chandles a setting change command to select application from a list.
// ---------------------------------------------------------------------------
//
TBool CMCSPluginSettingsContainer::HandleAppListChangeCommandL( const TInt& aIndex,
                                                                const TInt& aSettingIndex )
{
    
    TBool changed = EFalse;

    TInt index = aIndex;
    TInt oldIndex = index;

    iAppListDialog = new (ELeave) CAknRadioButtonSettingPage(
        R_AI_MCS_SETTINGS_APPLIST_PAGE, index, iModel->AppList());

    HBufC* title = StringLoader::LoadLC( R_AI_MCS_SETTINGS_TXT_LINKN, aSettingIndex + 1, iCoeEnv );
    iAppListDialog->SetSettingTextL( title->Des() );
    iAppListDialog->ConstructL();

    if (iAppListDialog->ExecuteLD(CAknSettingPage::EUpdateWhenChanged) &&
        index != oldIndex)
        {
        changed = iModel->ReplaceItemL( iListBox->CurrentItemIndex(), index , EApplication );
        }

    CleanupStack::PopAndDestroy( title );
    iAppListDialog = NULL;

    return changed;
}

// ---------------------------------------------------------------------------
// Chandles a setting change command to select bookmark from a list.
// ---------------------------------------------------------------------------
//
TBool CMCSPluginSettingsContainer::HandleBookmarkChangeCommandL( const TInt& aIndex, 
                                                                 const TInt& aSettingIndex )
    {
    TBool changed = EFalse;

    TInt index = aIndex;
    TInt oldIndex = index;

    iBkmListDialog = new (ELeave) CAknRadioButtonSettingPage(
        R_AI_MCS_SETTINGS_APPLIST_PAGE, index, iModel->BkmList());

    HBufC* title = StringLoader::LoadLC( R_AI_MCS_SETTINGS_TXT_LINKN, aSettingIndex + 1, iCoeEnv );
    iBkmListDialog->SetSettingTextL( title->Des() );
    iBkmListDialog->ConstructL();

    if (iBkmListDialog->ExecuteLD(CAknSettingPage::EUpdateWhenChanged) &&
        index != oldIndex)
        {
        changed = iModel->ReplaceItemL( iListBox->CurrentItemIndex(), index , EBookmark );
        }

    CleanupStack::PopAndDestroy( title );
    iBkmListDialog = NULL;
    return changed;
    }

// ---------------------------------------------------------------------------
// Gets Help
// ---------------------------------------------------------------------------
//
TCoeHelpContext CMCSPluginSettingsContainer::GetHelpContext() const
{
    TUid uid = TUid::Uid( AI_UID_ECOM_DLL_SETTINGS_MCSPLUGIN );
    TCoeContextName helpString;

    helpString.Copy( KSET_HLP_HOME_SCREEN_SHORTCUTS );

    // set UID and topic of Help to display
    return TCoeHelpContext( uid, helpString );
}

// ---------------------------------------------------------------------------
// Offers a key event.
// ---------------------------------------------------------------------------
//
TKeyResponse CMCSPluginSettingsContainer::OfferKeyEventL(
    const TKeyEvent& aKeyEvent, TEventCode aType)
{
    switch (aKeyEvent.iCode)
    {
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
void CMCSPluginSettingsContainer::CheckMiddleSoftkeyLabelL()
{
    CEikButtonGroupContainer* cba = CEikButtonGroupContainer::Current();
    if (cba)
    {
        cba->MakeCommandVisible(EAiScutSettingsCmdChange, (iModel->MdcaCount() != 0));
    }
}

// -----------------------------------------------------------------------------
// From class MFavouritesDbObserver.
// Handles database event.
// -----------------------------------------------------------------------------
//
void CMCSPluginSettingsContainer::HandleFavouritesDbEventL(RDbNotifier::TEvent aEvent)
    {
    switch ( aEvent )
        {
        case RDbNotifier::ECommit   :
            // fall-through intended here
        case RDbNotifier::ERecover  :
            // fall-through intended here
        case RDbNotifier::ERollback :
            {
            if ( IsChangeDialogShowing() )
                {
                CloseChangeDialog();
                }
            iModel->UpdateBkmListL();
            }
            break;
        default:
            break;
        }
    }

// -----------------------------------------------------------------------------
// From class MMCSPluginWatcherObserver.
// Handles matrix menu event.
// -----------------------------------------------------------------------------
//
void CMCSPluginSettingsContainer::HandleNotifyL()
    {
    if ( IsChangeDialogShowing() )
        {
        CloseChangeDialog();
        }
    iModel->UpdateAppListL();
    
    // Notification must be activated again
    iNotifyWatcher->Cancel();
    iNotifier.Notify( 0,
        RMenuNotifier::EItemsAddedRemoved | 
        RMenuNotifier::EItemsReordered |
        RMenuNotifier::EItemAttributeChanged,
        iNotifyWatcher->iStatus );
    iNotifyWatcher->WatchNotify( this );
    }

// End of File.
