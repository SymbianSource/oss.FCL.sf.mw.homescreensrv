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
* Description:  Container for the Application Shell plugin
*
*/


// INCLUDE FILES

// From this plugin.
#include "hs_app_pslnactiveidleplugincontainer.h"
#include "hs_app_pslnactiveidlepluginengine.h"
#include "hs_app_pslnactiveidleplugin.h"

// From Psln application.
#include <csxhelp/skins.hlp.hrh>

// From PSLN framework
#include <pslnfwbaseview.h>
#include <pslnfwiconhelper.h>

// Resources
#include <psln.rsg>
#include <pslnactiveidlepluginrsc.rsg>

// General services
#include <Aknlists.h>

// XUIKON
#include <XnODT.h>

// CONSTANTS
// UID of Personlisation application
const TUid KPslnAppUid = { 0x10005A32 };

// ========================= MEMBER FUNCTIONS ================================

CPslnActiveIdlePluginContainer::CPslnActiveIdlePluginContainer(
                    TBool aGSCalling,
                    CPslnActiveIdlePlugin* aPlugin )
    :
    iPlugin( aPlugin ),
    iGSCalling( aGSCalling )
    {
    }
// ---------------------------------------------------------------------------
// Symbian 2nd phase constructor can leave.
// ---------------------------------------------------------------------------
//
void CPslnActiveIdlePluginContainer::ConstructL( const TRect& aRect )
    {
    iListBox = new( ELeave ) CAknSingleGraphicStyleListBox;
    TInt titleResId = 0;

    if (iGSCalling)
    {
        titleResId = R_PSLN_GS_LIST_VIEW_TITLE;
    }
    BaseConstructL(aRect, titleResId, R_PSLN_PLUGIN_DEFAULT_VIEW_LBX);
    // Do not show container immediately - to avoid seeing "No Data" first.
    iListBox->MakeVisible( EFalse );
    }

// ---------------------------------------------------------------------------
// Destructor 
// ---------------------------------------------------------------------------
//
CPslnActiveIdlePluginContainer::~CPslnActiveIdlePluginContainer()
    {
    if( iItemArray )
        {
        TInt count = iItemArray->Count();
        iItemArray->Delete(0, count);
        }

    delete iItems;
    }

// -----------------------------------------------------------------------------
// CPslnActiveIdlePluginContainer::GetCurrentlySelectedIndex
// -----------------------------------------------------------------------------
//
TInt CPslnActiveIdlePluginContainer::GetCurrentlySelectedIndex() const
    {
    return iCurrentlySelected;
    }

// ---------------------------------------------------------------------------
// Adds new item to the listbox.
// ---------------------------------------------------------------------------
//
void CPslnActiveIdlePluginContainer::AddNewItemToListL( 
    const TInt aItemIndex, CXnODT* aSkinODT, TInt aUiController )
    {
    HBufC* item = HBufC::NewLC( KPslnItemMaxTextLength );
    TPtr itemPtr = item->Des();
    TBool newActiveItem = EFalse;

    // Get theme ODT and full name. 
    // Index aItemIndex has been verified in view.
    itemPtr = aSkinODT->ThemeFullName();
    if ( aSkinODT->Flags() & EXnThemeStatusActive &&
        iPlugin->Engine()->CurrentUIController() == aUiController )
        {
        itemPtr.Insert( 0, KPslnFWActiveListItemFormat );

        // Set selected active.
        iCurrentlySelected = aItemIndex;
        newActiveItem = ETrue;
        }
    else
        {
        itemPtr.Insert( 0, KPslnFWNonActiveListItemFormat );
        }
    iItemArray->AppendL( itemPtr );
    CleanupStack::PopAndDestroy( item );

    if ( newActiveItem )
        {
        // Set highlight.
        iListBox->SetCurrentItemIndex( iCurrentlySelected );
        // Since we now have content, show container.
        iListBox->MakeVisible( ETrue );
        if ( iPlugin )
            {
            iPlugin->CheckMiddleSoftkeyLabelL();
            }
        }
    }

// ---------------------------------------------------------------------------
// Removes themes from listbox.
// ---------------------------------------------------------------------------
//
void CPslnActiveIdlePluginContainer::RemoveThemes( const TInt aDeleteFrom )
    {
    // Validate parameter first.
    if ( iItemArray &&
         ( aDeleteFrom >= 0 ) &&
         ( aDeleteFrom < iItemArray->Count() ) )
        {
        iItemArray->Delete( aDeleteFrom, iItemArray->Count() - aDeleteFrom );
        }
    }

// ---------------------------------------------------------------------------
// Construct the listbox from resource array.
// ---------------------------------------------------------------------------
//
void CPslnActiveIdlePluginContainer::ConstructListBoxL( TInt aResLbxId )
    {
    iListBox->ConstructL( this, EAknListBoxSelectionList );

    iItemArray = static_cast<CDesCArray*>
        ( iListBox->Model()->ItemTextArray() );

    iItems = iCoeEnv->ReadDesC16ArrayResourceL( aResLbxId );

    // Create pre- and post-text icons.
    AddIconsToListL();
    }

// ---------------------------------------------------------------------------
// Gets Help context.
// ---------------------------------------------------------------------------
//
void CPslnActiveIdlePluginContainer::GetHelpContext( 
    TCoeHelpContext& aContext ) const
    {
    aContext.iMajor = KPslnAppUid;
    aContext.iContext = KSKINS_HLP_APPSKINS_LIST;
    }

// ---------------------------------------------------------------------------
// Adds icons to setting list items.
// ---------------------------------------------------------------------------
//
void CPslnActiveIdlePluginContainer::AddIconsToListL()
    {
    CPslnFWIconHelper* iconHelper = CPslnFWIconHelper::NewL();
    CleanupStack::PushL( iconHelper );
    iconHelper->AddIconsToSettingItemsL(
        ETrue,
        KErrNotFound, // none, selected
        iListBox );
    CleanupStack::PopAndDestroy( iconHelper );
    }

void CPslnActiveIdlePluginContainer::RefreshList()
    {
    TRAP_IGNORE(iListBox->HandleItemAdditionL());
    iListBox->SetCurrentItemIndex(iCurrentlySelected);
    TRAP_IGNORE(iPlugin->CheckMiddleSoftkeyLabelL());
    DrawDeferred();
    // Since we now have content, show container.
    iListBox->MakeVisible( ETrue );
    }

// End of File
