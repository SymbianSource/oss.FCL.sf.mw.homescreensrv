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
* Description:  Application list for settings listbox
*
*/


#include <e32cmn.h>
#include <msvuids.h>        // For KMsvRootIndexEntryIdValue
#include <SenduiMtmUids.h>
#include <StringLoader.h>
#include <aistrcnv.h>
#include <featmgr.h>
#include <mcsmenuitem.h>
#include <mcsmenufilter.h>

#include "mcspluginsettingsapplist.h"
#include "mcspluginsettingsmodel.h"
#include "mcspluginwatcher.h"

_LIT( KMyMenuData, "matrixmenudata" );
_LIT( KMenuTypeShortcut, "menu:shortcut" );
_LIT( KMenuAttrRefcount, "ref_count" );
_LIT( KMenuParamMailbox, "mailbox:" );
_LIT( KMenuAttrParameter, "param" );
_LIT( KMenuAttrLocked, "locked" );
_LIT( KMenuIconFile, "aimcsplugin.mif" );
_LIT( KMenuIconId, "16388" );
_LIT( KMenuMaskId, "16389" );
_LIT( KMailboxUid, "0x100058c5" );
_LIT( KInitialRefCount, "1" );
_LIT( KMCSFolder, "mcsplugin_folder" );
_LIT8( KItemLocked, "locked");
_LIT8( KProperValueFolder, "folder" );

// ======== MEMBER FUNCTIONS ========

// ---------------------------------------------------------------------------
// First-phase construction
// ---------------------------------------------------------------------------
//
CMCSPluginSettingsAppList::CMCSPluginSettingsAppList()
    {
    iMCSPluginFolderId = 0;
    }

// ---------------------------------------------------------------------------
// Second-phase construction
// ---------------------------------------------------------------------------
//
void CMCSPluginSettingsAppList::ConstructL()
    {
    iMsvSession = CMsvSession::OpenAsObserverL(*this);

    iMenu.OpenL( KMyMenuData );
    iSaveWatcher = CMCSPluginWatcher::NewL( CMCSPluginWatcher::EOperation );
    iUpdateWatcher = CMCSPluginWatcher::NewL( CMCSPluginWatcher::EOperation );
    iRemoveWatcher = CMCSPluginWatcher::NewL( CMCSPluginWatcher::EOperation );
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
    delete iSaveWatcher;
    delete iUpdateWatcher;
    delete iRemoveWatcher;
    
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
    if (aIndex < 0 || aIndex >= iListItems.Count())
        {
        TPtrC ret(KNullDesC);
        return ret;
        }
    CMenuItem* item = iListItems[aIndex];
    TBool attrExists;
    TPtrC itm; 
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
// Returns menuitems at given index. Since this method is called during
// adding the item to the Desktop widget, we also have to increment
// ref_count attribute if the item is run-time generated (i.e. Mailbox)
// ---------------------------------------------------------------------------
//
CMenuItem& CMCSPluginSettingsAppList::ItemL( const TInt& aIndex )
    {

    CMenuItem* menuItem( NULL );
    
    // check if index in within the list boundaries
    if ( aIndex >= 0 && aIndex < iListItems.Count() )
        {

        menuItem = iListItems[ aIndex ];

        TBool hasParam = EFalse;
        CleanupStack::PushL( menuItem );
        TPtrC param = menuItem->GetAttributeL( KMenuAttrParameter, hasParam ); 
        CleanupStack::Pop( menuItem );

        // if item is a mailbox, add it to MCS
        // (if it is not already there)
        if ( hasParam && param.Find( KMenuParamMailbox ) != KErrNotFound )
            {

            // set up a filter for finding the mailbox
            // with given ID in MCS
            CMenuFilter* filter = CMenuFilter::NewL();
            CleanupStack::PushL( filter );

            filter->SetType( KMenuTypeShortcut );
            filter->HaveAttributeL( KMenuAttrParameter, param );

            // search menu from the Root folder with the filter
            const TInt rootId = iMenu.RootFolderL();
            RArray<TMenuItem> itemArray;
            CleanupClosePushL( itemArray );
            iMenu.GetItemsL( itemArray, rootId, filter, ETrue );
                
            // save the number of findings
            TInt count( itemArray.Count() );

            // if MenuItem does not exist in MCS
            if ( count == 0 )
                {
                // save the item into Matrixmenudata.xml
                // the "op" variable is cleaned up by iSaveWatcher when asynchronous 
                // operation finishes
                CleanupStack::PushL( menuItem );
                CMenuOperation* op = menuItem->SaveL( iSaveWatcher->iStatus );
                CleanupStack::Pop( menuItem );
                iSaveWatcher->Watch( op ); 
                }
            else
                {
                // Item already exists in MCS
                // If it has reference counter, increment it before returning.
                CMenuItem* itm = CMenuItem::OpenL( iMenu, itemArray[ 0 ] );

                TInt newRefCount = UpdateMenuItemsRefCountL( itm, 1 );
                if ( newRefCount > -1 )
                    {
                    CleanupStack::PushL( itm );
                    CMenuOperation* op = itm->SaveL( iSaveWatcher->iStatus );
                    CleanupStack::PopAndDestroy( itm );
                    iSaveWatcher->Watch( op );
                    }
                }
            CleanupStack::PopAndDestroy( &itemArray );
            CleanupStack::PopAndDestroy( filter );
            }
        }
    return *menuItem;
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

    // skip run-time generated items
    filter->DoNotHaveAttributeL( KMenuAttrRefcount );
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
        if ( type == KMenuTypeApp || type == KMenuTypeShortcut || 
             type == KMenuTypeFolder )
            {
            CMenuItem* menuItem = CMenuItem::OpenL( iMenu, itemArray[ i ] );
            CleanupStack::PushL( menuItem );

            // only non-hidden and non-missing items should be offered to the user
            if ( ( menuItem->Flags() & TMenuItem::EHidden ) == EFalse &&
                 ( menuItem->Flags() & TMenuItem::EMissing ) == EFalse )
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
    return iMsvSession->GetEntryL(KMsvRootIndexEntryIdValue);
    }

// ---------------------------------------------------------------------------
// Adds remote mailboxes to the application list.
// ---------------------------------------------------------------------------
//
void CMCSPluginSettingsAppList::AddMailboxesL()
    {
    
    iListItems.ResetAndDestroy();
    CMsvEntry* rootEntry = GetRootEntryL();
    CleanupStack::PushL(rootEntry);
    TBuf<255> mailboxId;
    
    for ( TInt i = rootEntry->Count() - 1; i >= 0; --i )
        {
        const TMsvEntry& tentry = (*rootEntry)[i];

        if (tentry.iMtm == KSenduiMtmImap4Uid || tentry.iMtm == KSenduiMtmPop3Uid)
            {
            mailboxId.Num( tentry.Id() );  
            AddMailboxL( tentry.iDetails, mailboxId );
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
        // prepare param value
        HBufC* params = HBufC::NewLC( KMenuParamMailbox().Length() + aMailboxId.Length() );
        params->Des().Copy( KMenuParamMailbox );
        params->Des().Append( aMailboxId );
        TPtrC paramValue( params->Des() );

        TLinearOrder<CMenuItem> sortMethod( CMCSPluginSettingsAppList::CompareNameL );
        CMenuItem* newItem = CMenuItem::CreateL( iMenu, 
                                                 KMenuTypeShortcut, 
                                                 GetMCSPluginFolderIdL(), 
                                                 0 );
        CleanupStack::PushL( newItem );

        // mailbox is a shortcut item with "mailbox:mailboxID" parameter
        newItem->SetAttributeL( KMenuAttrUid, KMailboxUid );
        newItem->SetAttributeL( KMenuAttrLongName, aMailbox );
        newItem->SetAttributeL( KMenuAttrParameter, paramValue );
        newItem->SetAttributeL( KMenuAttrRefcount, KInitialRefCount );

        // setting icon for the shortcut
        newItem->SetAttributeL( KMenuAttrIconFile, KMenuIconFile );
        newItem->SetAttributeL( KMenuAttrIconId, KMenuIconId );
        newItem->SetAttributeL( KMenuAttrMaskId, KMenuMaskId );

        // append the item into iListItems lists
        User::LeaveIfError( iListItems.InsertInOrderAllowRepeats( newItem, sortMethod ) );
        CleanupStack::Pop( newItem );
        CleanupStack::PopAndDestroy( params );
    }

// ---------------------------------------------------------------------------
// Removes run-time generated menuitem (i.e. Mailbox) from MCS
// If the item at given index is not run-time generated, return
// ---------------------------------------------------------------------------
//
void CMCSPluginSettingsAppList::RemoveMenuItemL( TInt aIndex )
    {

    if ( aIndex < 0 || aIndex > iListItems.Count() - 1 )
        {
        return;
        }

    CMenuItem* menuItem = iListItems[ aIndex ];
    
    TBool hasParam = ETrue;
    TPtrC param = menuItem->GetAttributeL( KMenuAttrParameter, hasParam );

    if ( !hasParam )
        {
        // nothing to do
        return;
        }

    // set up a filter for finding the mailbox
    // with given ID in MCS
    CMenuFilter* filter = CMenuFilter::NewL();
    CleanupStack::PushL( filter );

    filter->SetType( KMenuTypeShortcut );
    filter->HaveAttributeL( KMenuAttrParameter, param );

    // search menu from the Root folder with the filter
    const TInt rootId = iMenu.RootFolderL();
    RArray<TMenuItem> itemArray;
    CleanupClosePushL( itemArray );
    iMenu.GetItemsL( itemArray, rootId, filter, ETrue );

    // save the number of findings
    TInt count( itemArray.Count() );
    
    if ( count > 0 )
        {
        // Item already exists in MCS
        // If it has reference counter, increment it before returning.
        CMenuItem* itm = CMenuItem::OpenL( iMenu, itemArray[ 0 ] );
        
        // decrement ref_count attribute 
        TInt newRefCount = UpdateMenuItemsRefCountL( itm, -1 );
        if ( newRefCount > 0 )
            {
            CleanupStack::PushL( itm ); 
            CMenuOperation* op = itm->SaveL( iUpdateWatcher->iStatus );
            CleanupStack::Pop( itm );
            iUpdateWatcher->Watch( op );
            }
        else if ( newRefCount == 0 )
            {
            // counter reached 0 -> item is not referenced by any shortcut
            // so remove it from MCS
            if ( iRemoveWatcher->IsActive() )
               {
               return;
               }
            CMenuOperation* op = iMenu.RemoveL( itm->Id(), iRemoveWatcher->iStatus );
            iRemoveWatcher->Watch( op );
            }
            delete itm;
        }
    CleanupStack::PopAndDestroy( &itemArray );
    CleanupStack::PopAndDestroy( filter );
    }

// ---------------------------------------------------------------------------
// Gets MCS Plugin folder ID. This hidden folder in matrixmenudata.xml is used 
// for storing run-time generated menuitems
// ---------------------------------------------------------------------------
//
TInt CMCSPluginSettingsAppList::GetMCSPluginFolderIdL()
    {
    
    if ( iMCSPluginFolderId == 0 )
        {
        CMenuItem* item( NULL );
        CMenuFilter* filter = CMenuFilter::NewL();
        CleanupStack::PushL( filter );
        filter->SetType( KMenuTypeFolder );
        filter->HaveAttributeL( KMenuAttrLongName, KMCSFolder );
        const TInt rootId = iMenu.RootFolderL();
        RArray<TMenuItem> itemArray;
        CleanupClosePushL( itemArray );
        iMenu.GetItemsL( itemArray, rootId, filter, ETrue );
        if ( itemArray.Count() > 0 )
            {
            item = CMenuItem::OpenL( iMenu, itemArray[ 0 ] );
            iMCSPluginFolderId = item->Id();
            }
        else 
            {
            iMCSPluginFolderId = iMenu.RootFolderL();
            }
        CleanupStack::PopAndDestroy( &itemArray );
        CleanupStack::PopAndDestroy( filter ); 
        delete item; 
        }
    return iMCSPluginFolderId;
    
    }

// ---------------------------------------------------------------------------
// Helper method for updating ref_count attribute of run-time generated 
// menuitems
// ---------------------------------------------------------------------------
//
TInt CMCSPluginSettingsAppList::UpdateMenuItemsRefCountL( CMenuItem* aItem, 
                                                          TInt aValueToAdd )
    {
    
    TBool exists = EFalse;
    CleanupStack::PushL( aItem ); 
    TPtrC param = aItem->GetAttributeL( KMenuAttrRefcount, exists );
    CleanupStack::Pop( aItem );
    if ( exists )
        {
        TInt references;
        TLex16 lextmp( param );
        lextmp.Val( references );
        references += aValueToAdd;
        TBuf<128> buf;
        buf.NumUC( references );
        // set new ref_count
        CleanupStack::PushL( aItem ); 
        aItem->SetAttributeL( KMenuAttrRefcount, buf );
        CleanupStack::Pop( aItem );
        // return new ref_count
        return references;
        }
    return -1;
    }


// End of File.
