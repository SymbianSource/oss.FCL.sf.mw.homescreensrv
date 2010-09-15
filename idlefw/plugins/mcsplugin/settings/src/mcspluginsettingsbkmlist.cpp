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
_LIT8( KMenuAttrParameter8, "param" );

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
    TInt listItemsCount( 0 );
    for( TInt i = 0; i< iListItems.Count(); i++ )
        {
        if( !iListItems[i]->iHidden )
            listItemsCount++;
        }
    return listItemsCount;
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
// Checks with a given index if bookmark has been hidden from menulist.
// ---------------------------------------------------------------------------
//
TBool CMCSPluginSettingsBkmList::ItemHidden( TInt aIndex )
    {
    if ( aIndex < 0 || aIndex >= iListItems.Count( ))
        {
        return EFalse;
        }
    return iListItems[aIndex]->iHidden;
    }

// ---------------------------------------------------------------------------
// Iterates thru the bookmark list and tries to find a menuitem which 
// matches given property map from HSPS
// ---------------------------------------------------------------------------
//
TSettingItem CMCSPluginSettingsBkmList::FindItemL(
    RPointerArray<HSPluginSettingsIf::CPropertyMap>& aProperties )
    {
    HBufC* uid( NULL );
    HBufC* param( NULL );
    TInt index( KErrNotFound );
    TSettingItem settingItem = { KErrNotFound, EBookmark, EFalse, EFalse };
    
    // read property values
    for( TInt i= 0; i < aProperties.Count(); i++ )
        {
        if( aProperties[i]->Name() == KUid )
            {
            uid = AiUtility::CopyToBufferL(
                  uid, aProperties[i]->Value() );
            CleanupStack::PushL( uid );
            }
        else if( aProperties[i]->Name() == KMenuAttrParameter8 )
            {
            param = AiUtility::CopyToBufferL(
                    param, aProperties[i]->Value() );
            CleanupStack::PushL( param );
            }
        }
    
    // try to find a match
    for( TInt j = 0; j < iListItems.Count(); j++ )
        {
        TPtrC value = *iListItems[j]->iUid;
        if( value.Compare( *uid ) == 0 )
            {
            index = j;
            break;
            }
        }
    
    // menuitem not found, add a new one
    if( index == KErrNotFound && uid && param )
        {
        CBkmListItem* listItem = CBkmListItem::NewLC( *uid, *param );
        listItem->iType = EFavBookmark;
        listItem->iUrl = KNullDesC().AllocL();
        listItem->iHidden = ETrue;
        iListItems.Append( listItem );
        CleanupStack::Pop( listItem );
        
        TInt listItemsCount = iListItems.Count();
        TPtrC value = *iListItems[--listItemsCount]->iUid;
        if( value.Compare( *uid ) == 0 )
            index = listItemsCount;
        }
    
    if( param ) CleanupStack::PopAndDestroy( param );
    if( uid ) CleanupStack::PopAndDestroy( uid );
    
    settingItem.id = index;
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
        TBool uidExists( EFalse );
        TBool nameExists( EFalse );
        TBool urlExists( EFalse );
        CMenuItem* menuItem = CMenuItem::OpenL( iMenu, itemArray[i] );
        CleanupStack::PushL( menuItem );
        TPtrC uid = menuItem->GetAttributeL( KMenuAttrUid, uidExists );
        TPtrC name = menuItem->GetAttributeL( KMenuAttrLongName, nameExists );
        TPtrC url = menuItem->GetAttributeL( KUrl, urlExists );
        // if all attrib exists, add it ( url ignored )
        if ( uidExists && uid.Length() > 0 && 
            nameExists && name.Length() > 0 )
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
CMenuItem* CMCSPluginSettingsBkmList::ItemL( TInt aIndex )
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
    return menuItem;
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
    newItem->SetAttributeL( KMenuAttrUrl, aUrl );
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
        if ( item )
            {
            CleanupStack::PushL( item );
            iMenuItems.AppendL( item );
            CleanupStack::Pop( item );
            }
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
    else
        {
        listItem->iUrl = KNullDesC().AllocL();
        }

    TLinearOrder<CBkmListItem> sortMethod(CBkmListItem::CompareCaption);
    User::LeaveIfError(iListItems.InsertInOrderAllowRepeats(listItem, sortMethod));
    CleanupStack::Pop(listItem);
    }

// ---------------------------------------------------------------------------
//Nested class to store individual bookmark list items
// ---------------------------------------------------------------------------
//
CMCSPluginSettingsBkmList::CBkmListItem::CBkmListItem() : iHidden( EFalse )
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
