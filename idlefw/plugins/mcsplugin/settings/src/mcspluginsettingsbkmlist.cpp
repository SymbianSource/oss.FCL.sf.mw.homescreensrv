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
* Description:  Bookmark list for settings listbox
*
*/

#include <favouritesitemlist.h>
#include <aistrcnv.h>
#include <mcsmenufilter.h>
#include <mcsmenuitem.h>

#include "mcspluginsettingsmodel.h"
#include "mcspluginsettingsbkmlist.h"

_LIT( KMyMenuData, "matrixmenudata" );
_LIT( KMenuUrl, "menu:url" );
_LIT( KUrl, "url" );
_LIT8( KUid, "uid" );
_LIT( KMenuAttrParameter, "param" );

// ======== MEMBER FUNCTIONS ========


// ---------------------------------------------------------------------------
// First-phase construction
// ---------------------------------------------------------------------------
//
CMCSPluginSettingsBkmList::CMCSPluginSettingsBkmList()
    {
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
    TSettingItem settingItem = { KErrNotFound, EBookmark, EFalse };
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
        // if exists, add it
        if ( exists )
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
        menuItem = CreateMenuItemL( *listItem->iUid, *listItem->iCaption, *listItem->iUrl );
        }
    else
        {
        menuItem = MCSMenuItemL( *listItem->iUid, *listItem->iCaption, *listItem->iUrl );
        }
    return *menuItem;
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
    CMenuItem* newItem = CMenuItem::CreateL( iMenu, KMenuTypeUrl, 0, 0 );
    CleanupStack::PushL( newItem );
    newItem->SetAttributeL( KMenuAttrUid, aUid );
    newItem->SetAttributeL( KMenuAttrLongName, aName );
    newItem->SetAttributeL( KMenuAttrView, aUrl );
    newItem->SetAttributeL( KMenuAttrParameter, aName );
    iMenuItems.AppendL( newItem );
    CleanupStack::Pop( newItem );
    return newItem;
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


// End of File.
