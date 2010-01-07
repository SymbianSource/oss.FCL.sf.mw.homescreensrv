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
* Description:  Bookmark list for settings listbox
*
*/


#include <StringLoader.h>
#include <activefavouritesdbnotifier.h> // For CActiveFavouritesDbNotifier
#include <aistrcnv.h>
#include <mcsmenufilter.h>
#include <mcsmenuitem.h>
#include <mcsmenuoperation.h>
#include <mcspluginsettingsres.rsg>

#include "mcspluginsettingsmodel.h"
#include "mcspluginsettingsapplist.h"
#include "mcspluginsettingsbkmlist.h"
#include "mcspluginwatcher.h"
#include "debug.h"

_LIT( KMyMenuData, "matrixmenudata" );
_LIT( KMenuUrl, "menu:url" );
_LIT( KMenuIconFile, "aimcsplugin.mif" );
_LIT( KMenuIconId, "16386" );
_LIT( KMenuMaskId, "16387" );
_LIT( KUrl, "url" );
_LIT8( KUid, "uid" );
_LIT( KMenuAttrRefcount, "ref_count" );
_LIT( KInitialRefCount, "1" );
_LIT( KMCSFolder, "mcsplugin_folder" );


// ======== MEMBER FUNCTIONS ========


// ---------------------------------------------------------------------------
// First-phase construction
// ---------------------------------------------------------------------------
//
CMCSPluginSettingsBkmList::CMCSPluginSettingsBkmList()
    {
    iMCSPluginFolderId = 0;
    }

// ---------------------------------------------------------------------------
// Second-phase construction
// ---------------------------------------------------------------------------
//
void CMCSPluginSettingsBkmList::ConstructL()
    {
    User::LeaveIfError(iBookmarkSess.Connect());
    User::LeaveIfError(iBookmarkDb.Open(iBookmarkSess, KBrowserBookmarks));
    iMenu.OpenL( KMyMenuData );
    GetBookmarkListL();

    iSaveWatcher = CMCSPluginWatcher::NewL( CMCSPluginWatcher::EOperation );
    iUpdateWatcher = CMCSPluginWatcher::NewL( CMCSPluginWatcher::EOperation );
    iRemoveWatcher = CMCSPluginWatcher::NewL( CMCSPluginWatcher::EOperation );
    }

// ---------------------------------------------------------------------------
// Two-phased constructor
// ---------------------------------------------------------------------------
//
CMCSPluginSettingsBkmList* CMCSPluginSettingsBkmList::NewL()
    {
    CMCSPluginSettingsBkmList* self = new (ELeave) CMCSPluginSettingsBkmList();
    CleanupStack::PushL(self);
    self->ConstructL();
    CleanupStack::Pop(self);
    return self;
    }

// ---------------------------------------------------------------------------
// Destructor
// ---------------------------------------------------------------------------
//
CMCSPluginSettingsBkmList::~CMCSPluginSettingsBkmList()
    {
    iListItems.ResetAndDestroy();
    iMenuItems.ResetAndDestroy();
    iBookmarkDb.Close();
    iBookmarkSess.Close();
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
TInt CMCSPluginSettingsBkmList::MdcaCount() const
    {
    return iListItems.Count();
    }

// ---------------------------------------------------------------------------
// From MDesCArray
// Indexes into a descriptor array.
// ---------------------------------------------------------------------------
//
TPtrC CMCSPluginSettingsBkmList::MdcaPoint(TInt aIndex) const
    {
    if (aIndex < 0 || aIndex >= iListItems.Count())
        {
        TPtrC ret(KNullDesC);
        return ret;
        }
    return iListItems[aIndex]->Caption();
    }

// ---------------------------------------------------------------------------
// Iterates thru the bookmark list and tries to find a menuitem which 
// matches given property map from HSPS
// ---------------------------------------------------------------------------
//
TSettingItem CMCSPluginSettingsBkmList::FindItemL( RPointerArray<HSPluginSettingsIf::CPropertyMap>& aProperties )
    {
    TInt index( KErrNotFound );
    TSettingItem settingItem = { KErrNotFound, EBookmark };
    for( TInt i= 0; i < aProperties.Count(); i++ )
        {
        if( aProperties[i]->Name() == KUid )
            {
            HBufC* value( NULL );
            value = AiUtility::CopyToBufferL( value, aProperties[i]->Value());
            for( TInt j = 0; j < iListItems.Count(); j++ )
                {
                TPtrC uid = *iListItems[j]->iUid;
                if( uid.Compare( *value ) == 0 )
                    {
                    index = j;
                    break;
                    }
                }
            delete value;
            }
        if( index != KErrNotFound )
            {
            settingItem.id = index;
            settingItem.type = EBookmark;
            break;
            }
        }
    return settingItem;
    }

// ---------------------------------------------------------------------------
// Gets bookmark list.
// ---------------------------------------------------------------------------
//
void CMCSPluginSettingsBkmList::GetBookmarkListL()
    {
    iListItems.ResetAndDestroy();

    GetBookmarksFromFavouritesL();
    GetBookmarksFromMCSL();
    }

// ---------------------------------------------------------------------------
// Gets bookmarks from Favourites server's bookmark database.
// ---------------------------------------------------------------------------
//
void CMCSPluginSettingsBkmList::GetBookmarksFromFavouritesL()
    {
    CFavouritesItemList* favItems = new (ELeave) CFavouritesItemList();
    CleanupStack::PushL( favItems );
    TInt err = iBookmarkDb.GetAll( *favItems, KFavouritesNullUid, CFavouritesItem::EItem);
    if( err != KErrNone )
        {
        ASSERT(0);
        }
    TInt count = favItems->Count();
    for ( TInt i = count - 1; i >= 0; i-- ) // newest on top
        {
        TUid  uid  = TUid::Uid(favItems->At(i)->Uid());
        TPtrC name = favItems->At(i)->Name();
        TPtrC url = favItems->At(i)->Url();
        AddBookmarkL( uid.Name(), name, url, EFavBookmark );
        }
    CleanupStack::PopAndDestroy( favItems );
    }

// ---------------------------------------------------------------------------
// Gets bookmarks from Menu Content Service.
// ---------------------------------------------------------------------------
//
void CMCSPluginSettingsBkmList::GetBookmarksFromMCSL()
    {
    TBool exists( EFalse );
    CMenuFilter* filter = CMenuFilter::NewL();
    CleanupStack::PushL( filter );
    filter->SetType( KMenuUrl );
    const TInt rootId = iMenu.RootFolderL();
    RArray<TMenuItem> itemArray;
    CleanupClosePushL( itemArray );
    iMenu.GetItemsL( itemArray, rootId, filter, ETrue );
    TInt count( itemArray.Count() );
    for ( TInt i = 0; i < count; i++ )
        {
        CMenuItem* menuItem = CMenuItem::OpenL( iMenu, itemArray[i] );
        CleanupStack::PushL( menuItem );
        TPtrC uid = menuItem->GetAttributeL( KMenuAttrUid, exists );
        TPtrC name = menuItem->GetAttributeL( KMenuAttrLongName, exists );
        TPtrC url = menuItem->GetAttributeL( KUrl, exists );

        // Check if bookmark is already present in Bookmark list.
        // This may happen in case of Favourite Bookmarks that were
        // previously added to MCS.
        // If it is, do not add it to Bookmark list anymore.
        
        TBool isRuntimeGenerated = EFalse;
        menuItem->GetAttributeL( KMenuAttrRefcount, isRuntimeGenerated );

        // if is not runtime generated and url exists, add it
        if ( !isRuntimeGenerated && exists )
            {
            AddBookmarkL( uid, name, url, EMCSBookmark );
            }

        CleanupStack::PopAndDestroy( menuItem );
        }

    CleanupStack::PopAndDestroy( &itemArray );
    CleanupStack::PopAndDestroy( filter );

    }

// ---------------------------------------------------------------------------
// If bookmark was selected amongst Favourites, new menu item is created into MCS.
// If predefined bookmark was selected, MCS menu item is retrieved
// ---------------------------------------------------------------------------
//
CMenuItem& CMCSPluginSettingsBkmList::ItemL( TInt aIndex )
    {
    CMenuItem* menuItem( NULL );
    CBkmListItem* listItem = iListItems[aIndex];
    if ( listItem->iType == EFavBookmark )
        {    
        TPtrC uid = *listItem->iUid;
        TPtrC name = *listItem->iCaption;
        TPtrC  url = *listItem->iUrl;
        menuItem = CreateMenuItemL( uid, name, url );
        }
    else
        {
        menuItem = MCSMenuItemL( *listItem->iUid, *listItem->iCaption, *listItem->iUrl );
        }
    return *menuItem;
    }

// ---------------------------------------------------------------------------
// Removes the menu item from MCS if it was created in runtime i.e. type is EFavBookmark.
// Favourite bookmarks have ref_count attribute, which is decremented everytime
// the bookmark is removed from some shortcut. When this counter reaches 0,
// its MenuItem is removed from MCS.
// ---------------------------------------------------------------------------
//
void CMCSPluginSettingsBkmList::RemoveMenuItemL( TInt aIndex )
    {

    if ( aIndex < 0 || aIndex > iListItems.Count() - 1 )
        {
        return;
        }

    CBkmListItem* listItem = iListItems[aIndex];
    if( listItem->iType == EFavBookmark )
        {
        CMenuItem* menuItem = MCSMenuItemL( *listItem->iUid, *listItem->iCaption, *listItem->iUrl );
        if ( !menuItem )
            {
            return;
            }
        // decrement ref_count attribute 
        TInt newRefCount = UpdateMenuItemsRefCountL( menuItem, -1 );
        if ( newRefCount > 0 )
            {
            CleanupStack::PushL( menuItem ); 
            CMenuOperation* op = menuItem->SaveL( iUpdateWatcher->iStatus );
            CleanupStack::Pop( menuItem );
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
            CMenuOperation* op = iMenu.RemoveL( menuItem->Id(), iRemoveWatcher->iStatus );
            iRemoveWatcher->Watch( op );
            }
        }
    }

// ---------------------------------------------------------------------------
// Tries to find menuitem with given UID, Name and Url in MCS,
// If it does not exist, it is created and saved there.
// If it exists already, ref_count attribute is incremented.
// ---------------------------------------------------------------------------
//
CMenuItem* CMCSPluginSettingsBkmList::CreateMenuItemL( const TDesC& aUid,
                                                       const TDesC& aName,
                                                       const TDesC& aUrl )
    {
    // try to search item in MCS
    CMenuItem* item = MCSMenuItemL( aUid, aName, aUrl );
    
    if ( item == NULL )
        {
        // Item does not exist in MCS yet. 
        // We will add a new one with reference counter set to 1.
        CMenuItem* newItem = CMenuItem::CreateL( iMenu, 
                                                 KMenuTypeUrl, 
                                                 GetMCSPluginFolderIdL(), 0 );
        CleanupStack::PushL( newItem );

        newItem->SetAttributeL( KMenuAttrUid, aUid );
        newItem->SetAttributeL( KMenuAttrLongName, aName );
        newItem->SetAttributeL( KMenuAttrIconFile, KMenuIconFile );
        newItem->SetAttributeL( KMenuAttrIconId, KMenuIconId );
        newItem->SetAttributeL( KMenuAttrMaskId, KMenuMaskId );
        newItem->SetAttributeL( KMenuAttrRefcount, KInitialRefCount );
        newItem->SetAttributeL( KUrl , aUrl );
        
        CMenuOperation* op = newItem->SaveL( iSaveWatcher->iStatus );
        iSaveWatcher->Watch( op );
        iMenuItems.AppendL( newItem );
        CleanupStack::Pop( newItem );
        return newItem;
        }
    else
        {
        // Item already exists in MCS
        // If it has reference counter, increment it before returning.
        TInt newRefCount = UpdateMenuItemsRefCountL( item, 1 );
        if ( newRefCount > -1 )
            {
            CleanupStack::PushL( item );
            CMenuOperation* op = item->SaveL( iSaveWatcher->iStatus );
            CleanupStack::Pop( item );
            iSaveWatcher->Watch( op );
            }
        }
    return item;
    }

// ---------------------------------------------------------------------------
// Finds menuitem with given UID, Name and URL in MCS and returns it
// ---------------------------------------------------------------------------
//
CMenuItem* CMCSPluginSettingsBkmList::MCSMenuItemL( const TDesC& aUid,
                                                    const TDesC& aName,
                                                    const TDesC& aUrl )
    {
    CMenuItem* item( NULL );
    CMenuFilter* filter = CMenuFilter::NewL();
    CleanupStack::PushL( filter );
    
    filter->SetType( KMenuUrl );
    filter->HaveAttributeL( KMenuAttrUid, aUid );
    filter->HaveAttributeL( KMenuAttrLongName, aName );
    filter->HaveAttributeL( KUrl , aUrl );
    const TInt rootId = iMenu.RootFolderL();
    RArray<TMenuItem> itemArray;
    CleanupClosePushL( itemArray );
    iMenu.GetItemsL( itemArray, rootId, filter, ETrue );
    if( itemArray.Count() > 0 )
        {
        item = CMenuItem::OpenL( iMenu, itemArray[0] );
        CleanupStack::PushL( item );
        iMenuItems.AppendL( item );
        CleanupStack::Pop( item );
        }
    CleanupStack::PopAndDestroy( &itemArray );
    CleanupStack::PopAndDestroy( filter );
    return item;
    }

// ---------------------------------------------------------------------------
// Adds an bookmark to the list.
// ---------------------------------------------------------------------------
//
void CMCSPluginSettingsBkmList::AddBookmarkL( const TDesC&  aUid, 
                                              const TDesC& aCaption, 
                                              const TDesC& aUrl, 
                                              TBookmarkType aType )
    {
    CBkmListItem* listItem = CBkmListItem::NewLC(aUid, aCaption);

    listItem->iType = aType;
    if( aUrl.Length() > 0 )
        {
        listItem->iUrl = aUrl.AllocL();
        }
    TLinearOrder<CBkmListItem> sortMethod(CBkmListItem::CompareCaption);
    User::LeaveIfError(iListItems.InsertInOrderAllowRepeats(listItem, sortMethod));
    CleanupStack::Pop(listItem);
    }

// ---------------------------------------------------------------------------
// Updates the bookmark list.
// ---------------------------------------------------------------------------
//
void CMCSPluginSettingsBkmList::UpdateBkmListL()
    {
    GetBookmarkListL();
    }

// ---------------------------------------------------------------------------
//Nested class to store individual bookmark list items
// ---------------------------------------------------------------------------
//
CMCSPluginSettingsBkmList::CBkmListItem::CBkmListItem()
    {
    }

// ---------------------------------------------------------------------------
// Second-phase construction
// ---------------------------------------------------------------------------
//
void CMCSPluginSettingsBkmList::CBkmListItem::ConstructL( const TDesC&  aUid, 
                                                          const TDesC& aCaption )
    {
    iUid = aUid.AllocL();
    iCaption = aCaption.AllocL();
    }

// ---------------------------------------------------------------------------
// Two-phased constructor
// ---------------------------------------------------------------------------
//
CMCSPluginSettingsBkmList::CBkmListItem* CMCSPluginSettingsBkmList::CBkmListItem::NewLC(
                                              const TDesC&  aUid, const TDesC& aCaption )
    {
    CBkmListItem* self = new (ELeave) CBkmListItem();
    CleanupStack::PushL(self);
    self->ConstructL( aUid, aCaption );
    return self;
    }

// ---------------------------------------------------------------------------
// Destructor
// ---------------------------------------------------------------------------
//
CMCSPluginSettingsBkmList::CBkmListItem::~CBkmListItem()
    {
    delete iUid;
    delete iCaption;
    delete iUrl;
    }

// ---------------------------------------------------------------------------
// Compare method used to add the items to the list in sorted order.
// ---------------------------------------------------------------------------
//
TInt CMCSPluginSettingsBkmList::CBkmListItem::CompareCaption( const CBkmListItem& aFirst,
                                                              const CBkmListItem& aSecond )
{
    return aFirst.iCaption->Des().CompareC(*aSecond.iCaption);
}

// ---------------------------------------------------------------------------
// Returns the item target bookmark caption.
// ---------------------------------------------------------------------------
//
TPtrC CMCSPluginSettingsBkmList::CBkmListItem::Caption() const
{
    return TPtrC(*iCaption);
}

// ---------------------------------------------------------------------------
// Gets MCS Plugin folder ID. This hidden folder in matrixmenudata.xml is used 
// for storing run-time generated menuitems
// ---------------------------------------------------------------------------
//
TInt CMCSPluginSettingsBkmList::GetMCSPluginFolderIdL()
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
            item = CMenuItem::OpenL( iMenu, itemArray[0] );
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
TInt CMCSPluginSettingsBkmList::UpdateMenuItemsRefCountL( CMenuItem* aItem, 
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
