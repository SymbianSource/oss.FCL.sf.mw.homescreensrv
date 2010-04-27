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
* Description:  Application list for settings listbox
*
*/

#include <aistrcnv.h>
#include <mcsmenuitem.h>
#include <mcsmenufilter.h>

#include "mcspluginsettingsapplist.h"
#include "mcspluginsettingsmodel.h" // For TSettingItem

_LIT( KMyMenuData, "matrixmenudata" );
_LIT( KMenuTypeShortcut, "menu:shortcut" );
_LIT( KMenuTypeMailbox, "menu:mailbox" );
_LIT( KMenuAttrParameter, "param" );
_LIT( KMenuAttrLocked, "locked" );
_LIT8( KItemLocked, "locked");
_LIT8( KProperValueFolder, "folder" );
_LIT( KMenuAttrUndefUid, "0x99999991" );
_LIT( KMenuItemLongName, "long_name" );

#define KMCSCmailMtmUidValue 0x2001F406

// ======== MEMBER FUNCTIONS ========

// ---------------------------------------------------------------------------
// First-phase construction
// ---------------------------------------------------------------------------
//
CMCSPluginSettingsAppList::CMCSPluginSettingsAppList()
    {
    }

// ---------------------------------------------------------------------------
// Second-phase construction
// ---------------------------------------------------------------------------
//
void CMCSPluginSettingsAppList::ConstructL()
    {
    iMsvSession = CMsvSession::OpenAsObserverL(*this);

    iMenu.OpenL( KMyMenuData );
    
    // Get "Undefined" icon and text
    CMenuFilter* filter = CMenuFilter::NewL();
    CleanupStack::PushL( filter );

    // 'Undefined' item
    filter->HaveAttributeL( KMenuAttrUid, KMenuAttrUndefUid );

    TMenuItem item;
    const TInt root = iMenu.RootFolderL();
    RArray<TMenuItem> items;
    CleanupClosePushL( items );
    iMenu.GetItemsL( items, root, filter, ETrue );

    if ( items.Count() > 0 )
        {
        iUndefinedItem = CMenuItem::OpenL( iMenu, items[ 0 ] );
        iUndefinedText = NULL;

        if ( iUndefinedItem )
            {
            TBool exists( KErrNotFound );//CleanupStack::PushL( undefinedItem );
            TPtrC undefined = iUndefinedItem->GetAttributeL( KMenuItemLongName, exists );

            if ( exists )
                {
                iUndefinedText = HBufC::NewMaxL( undefined.Length() );
                iUndefinedText->Des().Copy( undefined );
                }
            else
                {
                iUndefinedText = KNullDesC().Alloc();
                }
            }
        }

    CleanupStack::PopAndDestroy( &items );
    CleanupStack::PopAndDestroy( filter );
    }

// ---------------------------------------------------------------------------
// Two-phased constructor
// ---------------------------------------------------------------------------
//
CMCSPluginSettingsAppList* CMCSPluginSettingsAppList::NewL()
    {
    CMCSPluginSettingsAppList* self = new (ELeave) CMCSPluginSettingsAppList();
    CleanupStack::PushL(self);
    self->ConstructL();
    CleanupStack::Pop(self);
    return self;
    }

// ---------------------------------------------------------------------------
// Destructor
// ---------------------------------------------------------------------------
//
CMCSPluginSettingsAppList::~CMCSPluginSettingsAppList()
    {
    delete iMsvSession;

    iListItems.ResetAndDestroy();
    iMenu.Close();
    
    delete iUndefinedText;
    delete iUndefinedItem;
    }

// ---------------------------------------------------------------------------
// From MDesCArray
// Returns the number of descriptor elements in a descriptor array.
// ---------------------------------------------------------------------------
//
TInt CMCSPluginSettingsAppList::MdcaCount() const
    {
    return iListItems.Count();
    }

// ---------------------------------------------------------------------------
// From MDesCArray
// Indexes into a descriptor array.
// ---------------------------------------------------------------------------
//
TPtrC CMCSPluginSettingsAppList::MdcaPoint( TInt aIndex ) const
    {
    if ( aIndex < 0 || aIndex >= iListItems.Count() )
        {
        TPtrC ret( KNullDesC );
        return ret;
        }
    CMenuItem* item = iListItems[ aIndex ];

    TBool attrExists;
    TPtrC itm( KNullDesC );
    TRAP_IGNORE( 
        itm.Set( item->GetAttributeL( KMenuAttrLongName, attrExists ) );
        )

    return itm;
    }

// ---------------------------------------------------------------------------
// From class MMsvSessionObserver.
// Handles an event from the message server.
// ---------------------------------------------------------------------------
//
void CMCSPluginSettingsAppList::HandleSessionEventL(
                                                  TMsvSessionEvent /*aEvent*/, 
                                                  TAny* /*aArg1*/, 
                                                  TAny* /*aArg2*/,
                                                  TAny* /*aArg3*/ )
    {

    }

// ---------------------------------------------------------------------------
// Starts the asynchronous application list initialization.
// ---------------------------------------------------------------------------
//
void CMCSPluginSettingsAppList::StartL()
    {
    iListItems.ResetAndDestroy();
    AddMailboxesL();
    AddStaticItemsL();
    }

// ---------------------------------------------------------------------------
// Iterates thru the application list and tries to find a menuitem which 
// matches given property map from HSPS
// ---------------------------------------------------------------------------
//
TSettingItem CMCSPluginSettingsAppList::FindItemL( 
        RPointerArray<HSPluginSettingsIf::CPropertyMap>& aProperties )
    {
    TBool attrExists( EFalse );
    TSettingItem settingItem = { KErrNotFound, EApplication, EFalse };
    TBool isFolder = EFalse;

    // check if the item is folder
    for ( TInt j = 0; j < aProperties.Count(); j++ )
        {
        if( aProperties[j]->Name() == KType )
            {
            if ( aProperties[j]->Value() == KProperValueFolder )
                {
                isFolder = ETrue;
                }
            break;
            }
        }

    TBool itemFound( EFalse );
    
    // compare name/value pairs with every menu item in the list
    for ( TInt i = 0; i < iListItems.Count() && !itemFound; i++ )
        {
        TBool match( ETrue );
        CMenuItem* item = iListItems[ i ];
        for ( TInt j = 0; j < aProperties.Count() && match; j++ )
            {
            // type and locked properties skipped
            if ( aProperties[j]->Name() != KType &&
                 aProperties[j]->Name() != KItemLocked )
                {
                HBufC* attrName( NULL );
                attrName = AiUtility::CopyToBufferL( attrName,
                        aProperties[j]->Name());
                CleanupStack::PushL( attrName );
                TPtrC attr = item->GetAttributeL( *attrName, attrExists );

                HBufC* attrValue( NULL );
                attrValue = AiUtility::CopyToBufferL( attrValue,
                        aProperties[j]->Value());
                CleanupStack::PushL( attrValue );

                // in case of folder, we just have to compare id
                // which is stored in param attribute
                if ( isFolder && *attrName == KMenuAttrParameter )
                    {
                    // convert id to integer
                    TInt id;
                    TLex16 lextmp( attrValue->Ptr() );
                    lextmp.Val( id );

                    if ( item->Id() != id )
                        {
                        match = EFalse;
                        }
                    CleanupStack::PopAndDestroy( attrValue );
                    CleanupStack::PopAndDestroy( attrName );
                    break;
                    }

                // otherwise, compare attributes from HSPS and from menuitem
                // if there is no match, move to the next item in the list
                if ( attr != *attrValue )
                    {
                    match = EFalse;
                    }
                CleanupStack::PopAndDestroy( attrValue );
                CleanupStack::PopAndDestroy( attrName );
                }
            }

        if ( match )
            {
            settingItem.id = i;
            settingItem.type = EApplication;
            itemFound = ETrue;
            }
        }
    return settingItem;
    }

// ---------------------------------------------------------------------------
// Returns menuitems at given index.
// ---------------------------------------------------------------------------
//
CMenuItem* CMCSPluginSettingsAppList::ItemL( const TInt& aIndex )
    {
    CMenuItem* menuItem( NULL );
    // check if index is within the list boundaries
    if ( aIndex >= 0 && aIndex < iListItems.Count() )
        {
        menuItem = iListItems[ aIndex ];
        }
    return menuItem;
    }

// ---------------------------------------------------------------------------
// Helper method for comparing names of two menuitems. Used to sort the list
// of items.
// ---------------------------------------------------------------------------
//
TInt CMCSPluginSettingsAppList::CompareNameL( const CMenuItem& aFirst,
                                              const CMenuItem& aSecond )
    {
    TBool exists( EFalse );
    CMenuItem& first = const_cast<CMenuItem&>(aFirst);
    CMenuItem& second = const_cast<CMenuItem&>(aSecond);
    
    TPtrC name1 = first.GetAttributeL( KMenuAttrLongName, exists );
    TPtrC name2 = second.GetAttributeL( KMenuAttrLongName, exists );
    
    return name1.CompareC( name2 );
    }

// ---------------------------------------------------------------------------
// Adds the static list items to the application list.
// ---------------------------------------------------------------------------
//
void CMCSPluginSettingsAppList::AddStaticItemsL()
    {
    TLinearOrder<CMenuItem> sortMethod( CMCSPluginSettingsAppList::CompareNameL );
    
    CMenuFilter* filter = CMenuFilter::NewL();
    CleanupStack::PushL( filter );

    // skip locked items
    filter->DoNotHaveAttributeL( KMenuAttrLocked );
    const TInt rootId = iMenu.RootFolderL();
    RArray<TMenuItem> itemArray;
    CleanupClosePushL( itemArray );
    iMenu.GetItemsL( itemArray, rootId, filter, ETrue );
    
    TInt count( itemArray.Count() );
    
    for ( TInt i = 0; i < count; i++ )
        {
        TPtrC type = itemArray[ i ].Type();
        
        // we add applications, shortcuts and folders to the list
        if ( type == KMenuTypeApp || 
             type == KMenuTypeShortcut || 
             type == KMenuTypeFolder )
            {
            CMenuItem* menuItem = CMenuItem::OpenL( iMenu, itemArray[ i ] );
            CleanupStack::PushL( menuItem );

            // only non-hidden and non-missing items should be offered to the user
            if ( ( menuItem->Flags() & TMenuItem::EHidden ) == 0 &&
                 ( menuItem->Flags() & TMenuItem::EMissing ) == 0 )
                {
                User::LeaveIfError( iListItems.InsertInOrderAllowRepeats( menuItem, sortMethod ) );
                CleanupStack::Pop( menuItem );
                }
            else 
                {
                CleanupStack::PopAndDestroy( menuItem );
                }
            menuItem = NULL;
            }
        }

    CleanupStack::PopAndDestroy( &itemArray );
    CleanupStack::PopAndDestroy( filter );
    }


// ---------------------------------------------------------------------------
// Returns the root entry containing mailboxes.
// ---------------------------------------------------------------------------
//
CMsvEntry* CMCSPluginSettingsAppList::GetRootEntryL()
    {
    return iMsvSession->GetEntryL( KMsvRootIndexEntryIdValue );
    }

// ---------------------------------------------------------------------------
// Adds remote mailboxes to the application list.
// ---------------------------------------------------------------------------
//
void CMCSPluginSettingsAppList::AddMailboxesL()
    {
    CMsvEntry* rootEntry = GetRootEntryL();
    CleanupStack::PushL(rootEntry);
    TBuf<255> mailboxId;
    
    TInt cnt = rootEntry->Count();
    if ( cnt > 0 )
        {
        rootEntry->SetSortTypeL( TMsvSelectionOrdering( 
            KMsvGroupByType | KMsvGroupByStandardFolders, 
            EMsvSortByDetailsReverse, ETrue ) );
    
        for ( TInt i = rootEntry->Count(); --i >= 0; )
            {
            const TMsvEntry& tentry = (*rootEntry)[i];
    
            if (tentry.iMtm.iUid == KMCSCmailMtmUidValue )
                {
                mailboxId.Num( tentry.Id() );  
                AddMailboxL( tentry.iDetails, mailboxId );
                }
            }
        }
    CleanupStack::PopAndDestroy(rootEntry);
    }

// ---------------------------------------------------------------------------
// Adds a mailbox to the list.
// ---------------------------------------------------------------------------
//
void CMCSPluginSettingsAppList::AddMailboxL( const TDesC& aMailbox,
                                             const TDesC& aMailboxId )
    {
    TLinearOrder<CMenuItem> sortMethod( CMCSPluginSettingsAppList::CompareNameL );
    CMenuItem* newItem = CMenuItem::CreateL( iMenu, KMenuTypeMailbox, 0, 0 );
    CleanupStack::PushL( newItem );

    // mailbox is a shortcut item with "mailbox:mailboxID" parameter
    newItem->SetAttributeL( KMenuAttrUid, aMailboxId );
    newItem->SetAttributeL( KMenuAttrLongName, aMailbox );
    // Mailbox name is saved to settings into param field.
    newItem->SetAttributeL( KMenuAttrParameter, aMailbox );

    // append the item into iListItems lists
    User::LeaveIfError( iListItems.InsertInOrderAllowRepeats( newItem, sortMethod ) );
    CleanupStack::Pop( newItem );
    }

// End of File.
