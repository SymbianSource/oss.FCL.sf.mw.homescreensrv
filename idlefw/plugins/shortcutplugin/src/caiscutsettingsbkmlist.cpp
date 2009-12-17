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
* Description:  Bookmark list for settings listbox
*
*/


#include <StringLoader.h>
#include <ActiveFavouritesDbNotifier.h> // For CActiveFavouritesDbNotifier

#include <aiscutsettingsres.rsg>
#include <aiscuttexts.rsg>

#include "caiscutsettingsmodel.h"
#include "caiscutsettingsapplist.h"
#include "caiscutsettingsbkmlist.h"
#include "taiscutparser.h"
#include "caiscutsettingsitem.h"
#include "aiscutdefs.h"

#include "debug.h"


_LIT( KText, "bkm=0x%x" );
const TInt KTBUF16 = 16;

// ======== MEMBER FUNCTIONS ========

CAiScutSettingsBkmList::CAiScutSettingsBkmList(
    CCoeEnv*                aEnv,
    CAiScutSettingsModel*   aModel,
    MAiScutListObserver&    aObserver
    )
    : iEnv(aEnv)
    , iModel(aModel)
    , iObserver(aObserver)
{
}

void CAiScutSettingsBkmList::ConstructL()
{
}

CAiScutSettingsBkmList* CAiScutSettingsBkmList::NewL(
    CCoeEnv* aEnv,
    CAiScutSettingsModel* aModel,
    MAiScutListObserver& aObserver)
{
    CAiScutSettingsBkmList* self = new (ELeave) CAiScutSettingsBkmList(
        aEnv, aModel, aObserver);
    CleanupStack::PushL(self);
    self->ConstructL();
    CleanupStack::Pop(self);
    return self;
}

CAiScutSettingsBkmList::~CAiScutSettingsBkmList()
{
    iListItems.ResetAndDestroy();
    delete iBookmarkDbObserver;
}

// ---------------------------------------------------------------------------
// From MDesCArray
// Returns the number of descriptor elements in a descriptor array.
// ---------------------------------------------------------------------------
//
TInt CAiScutSettingsBkmList::MdcaCount() const
{
    return iListItems.Count();
}

// ---------------------------------------------------------------------------
// From MDesCArray
// Indexes into a descriptor array.
// ---------------------------------------------------------------------------
//
TPtrC CAiScutSettingsBkmList::MdcaPoint(TInt aIndex) const
{
    if (aIndex < 0 || aIndex >= iListItems.Count())
    {
        TPtrC ret(KNullDesC);
        return ret;
    }
    return iListItems[aIndex]->Caption();
}

// ---------------------------------------------------------------------------
// Gets bookmark list.
// ---------------------------------------------------------------------------
//
void CAiScutSettingsBkmList::GetBookmarkListL()
{
    iListItems.ResetAndDestroy();

    iModel->ReadBookmarksL();

    TInt count = iModel->BookmarkCount();

    for (TInt i = count - 1; i >= 0; i--) // newest on top
    {
        CFavouritesItem* item = iModel->GetBookmark(i);

        TUid  uid  = TUid::Uid(item->Uid());
        TPtrC name = item->Name();

        AddBookmarkL(uid, name);
    }
}

// ---------------------------------------------------------------------------
// Tells the bookmark list to start or stop observing for changes.
// ---------------------------------------------------------------------------
//
void CAiScutSettingsBkmList::SetObservingL(TBool aObserving)
{
    delete iBookmarkDbObserver;
    iBookmarkDbObserver = NULL;

    if (aObserving)
    {

        iBookmarkDbObserver = new (ELeave) CActiveFavouritesDbNotifier(
            iModel->FavouritesDb(), *this);
        iBookmarkDbObserver->Start();
    }

    iObserving = aObserving;
}

// ---------------------------------------------------------------------------
// Finds the index of the given settings item in the bookmark list.
// ---------------------------------------------------------------------------
//
TInt CAiScutSettingsBkmList::FindIndex(CAiScutSettingsItem& aItem)
{
    TInt index = KErrNotFound;

    TAiScutParser parser;
    parser.Parse(aItem.Value());
    TUid uid = parser.ParseUid(parser.Get(EScutDefParamValue));

    for (TInt i = iListItems.Count() - 1; i >= 0; --i)
    {
        if (iListItems[i]->Uid() == uid)
        {
            index = i;
            break;
        }
    }

    return index;
}

// ---------------------------------------------------------------------------
// Returns target bookmark data from the given index.
// ---------------------------------------------------------------------------
//
TInt CAiScutSettingsBkmList::GetDataByIndex(
    TInt aIndex, TPtrC& aParams, TPtrC& aCaption) const
{
    if (aIndex >= 0 && aIndex < iListItems.Count())
    {
        CBkmListItem* item = iListItems[aIndex];
        aParams.Set(item->Params());
        aCaption.Set(item->Caption());
        return KErrNone;
    }

    return KErrNotFound;
}

// ---------------------------------------------------------------------------
// Adds an bookmark to the list.
// ---------------------------------------------------------------------------
//
void CAiScutSettingsBkmList::AddBookmarkL(TUid aUid, const TDesC& aCaption)
{
    CBkmListItem* listItem = CBkmListItem::NewLC(aUid, aCaption);

    TBuf<KTBUF16> buf;
    buf.Format( KText, aUid.iUid );

    listItem->SetParamsL(buf);
    TLinearOrder<CBkmListItem> sortMethod(CBkmListItem::CompareCaption);
    User::LeaveIfError(iListItems.InsertInOrderAllowRepeats(listItem, sortMethod));
    CleanupStack::Pop(listItem);
}

// ---------------------------------------------------------------------------
// Updates the bookmark list.
// ---------------------------------------------------------------------------
//
void CAiScutSettingsBkmList::UpdateBkmListL()
{
    GetBookmarkListL();
    iObserver.HandleScutListEventL(MAiScutListObserver::EBkmListUpdated, EFalse);
}

// -----------------------------------------------------------------------------
// From class MFavouritesDbObserver.
// Handles database event.
// -----------------------------------------------------------------------------
//
void CAiScutSettingsBkmList::HandleFavouritesDbEventL(RDbNotifier::TEvent aEvent)
{
	__PRINT( __DBG_FORMAT( "XAI: CAiScutSettingsBkmList::HandleFavouritesDbEventL aEvent = %d"), aEvent);
	
    if (aEvent == RDbNotifier::ECommit)
    {
        UpdateBkmListL();
    }
}

// ---------------------------------------------------------------------------
//Nested class to store individual bookmark list items
// ---------------------------------------------------------------------------
//
CAiScutSettingsBkmList::CBkmListItem::CBkmListItem(TUid aUid) : iUid(aUid)
{
}

void CAiScutSettingsBkmList::CBkmListItem::ConstructL(const TDesC& aCaption)
{
    iCaption = aCaption.AllocL();
}

CAiScutSettingsBkmList::CBkmListItem* CAiScutSettingsBkmList::CBkmListItem::NewLC(
    TUid aUid, const TDesC& aCaption)
{
    CBkmListItem* self = new (ELeave) CBkmListItem(aUid);
    CleanupStack::PushL(self);
    self->ConstructL(aCaption);
    return self;
}

CAiScutSettingsBkmList::CBkmListItem::~CBkmListItem()
{
    delete iCaption;
    delete iParams;
}

// ---------------------------------------------------------------------------
// Compare method used to add the items to the list in sorted order.
// ---------------------------------------------------------------------------
//
TInt CAiScutSettingsBkmList::CBkmListItem::CompareCaption(const CBkmListItem& aFirst,
    const CBkmListItem& aSecond)
{
    return aFirst.iCaption->Des().CompareC(*aSecond.iCaption);
}

// ---------------------------------------------------------------------------
// Returns the item target bookmark uid.
// ---------------------------------------------------------------------------
//
TUid CAiScutSettingsBkmList::CBkmListItem::Uid() const
{
    return iUid;
}

// ---------------------------------------------------------------------------
// Returns the item target bookmark caption.
// ---------------------------------------------------------------------------
//
TPtrC CAiScutSettingsBkmList::CBkmListItem::Caption() const
{
    return TPtrC(*iCaption);
}

// ---------------------------------------------------------------------------
// Returns the possible parameters for item target.
// ---------------------------------------------------------------------------
//
TPtrC CAiScutSettingsBkmList::CBkmListItem::Params() const
{
    TPtrC params;
    if (iParams)
    {
        params.Set(*iParams);
    }
    return params;
}

// ---------------------------------------------------------------------------
// Sets the parameters for the item target.
// ---------------------------------------------------------------------------
//
void CAiScutSettingsBkmList::CBkmListItem::SetParamsL(const TDesC& aParams)
{
    HBufC* newParams = aParams.AllocL();
    delete iParams;
    iParams = newParams;
}

// End of File.
