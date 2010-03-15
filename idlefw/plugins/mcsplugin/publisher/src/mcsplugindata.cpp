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
* Description:  
*
*/
#include <mcsmenuitem.h>
#include <mcsmenufilter.h>
#include <itemmap.h>
#include <aistrcnv.h>
#include <favouritesdbobserver.h>
#include <favouritesdb.h>        
#include <msvuids.h>        // For KMsvRootIndexEntryIdValue

#include <mcsmenuitem.h>
#include <mcsmenufilter.h>

#include "mcsplugindata.h"
#include "mcspluginengine.h"

using namespace HSPluginSettingsIf;

_LIT( KMenuAttrParam, "param" );
_LIT( KMenuAttrLocked, "locked" );
_LIT8( KProperNameType, "type" );
_LIT8( KProperNameParam, "param" );
_LIT8( KProperNameUid, "uid" );
_LIT8( KProperNameView, "view" );
_LIT8( KProperValueFolder, "folder" );
_LIT8( KProperValueBookmark, "bookmark" );
_LIT8( KProperValueAppl, "application" );

_LIT( KMailboxUid, "0x100058c5" );
_LIT8( KMailboxUid8, "0x100058c5" );
_LIT( KMenuMailboxIconId, "16388" );
_LIT( KMenuMailboxMaskId, "16389" );

_LIT( KUrl, "url" );
_LIT( KMenuIconFile, "aimcsplugin.mif" );
_LIT( KMenuBookmarkIconId, "16386" );
_LIT( KMenuBookmarkMaskId, "16387" );
_LIT( KInitialRefCount, "1" );
_LIT( KMenuAttrRefcount, "ref_count" );
_LIT( KMyMenuData, "matrixmenudata" );
_LIT( KMenuTypeShortcut, "menu:shortcut" );
_LIT( KMenuAttrParameter, "param" );

#define KMCSCmailMtmUidValue 0x2001F406

// ======== LOCAL FUNCTIONS ========

static void ItemMapArrayCleanupFunc( TAny* aPointerArray )
    {
    RPointerArray<CItemMap>* p = static_cast<RPointerArray<CItemMap>*>( aPointerArray ); 
    p->ResetAndDestroy();
    p->Close();
    }

// ======== MEMBER FUNCTIONS ========

void TMCSData::SetMenuItem( TMenuItem& aMenuItem )
    {
    iMenuItem = aMenuItem;
    }

TMenuItem& TMCSData::MenuItem()
    {
    return iMenuItem;
    }

void TMCSData::SetDirty( TBool aDirty )
    {
    iDirty = aDirty;
    }

TBool TMCSData::IsDirty() const
    {
    return iDirty;
    }
    
// ---------------------------------------------------------------------------
// Symbian 2nd phase constructor can leave
// ---------------------------------------------------------------------------
//
CMCSPluginData* CMCSPluginData::NewL( CMCSPluginEngine& aEngine, const TDesC8& aInstanceUid )
    {
    CMCSPluginData* self = new (ELeave) CMCSPluginData( aEngine, aInstanceUid );
    CleanupStack::PushL( self );
    self->ConstructL();
    CleanupStack::Pop( self );
 
    return self;
    }
    
// ---------------------------------------------------------------------------
// Default constructor
// ---------------------------------------------------------------------------
//
CMCSPluginData::CMCSPluginData( CMCSPluginEngine& aEngine, const TDesC8& aInstanceUid )
    : iEngine( aEngine ), iInstanceUid( aInstanceUid )
    {
    }
    
// ---------------------------------------------------------------------------
// Symbian 2nd phase constructor can leave
// ---------------------------------------------------------------------------
//
void CMCSPluginData::ConstructL()
    {
    iMsvSession = CMsvSession::OpenAsObserverL( *this );
    iPluginSettings = CHomescreenSettings::Instance();
    if( iPluginSettings == NULL )
        {
        User::Leave( KErrUnknown );
        }
    iPluginSettings->AddObserverL( this );

    iSaveWatcher = CMCSPluginWatcher::NewL( CMCSPluginWatcher::EOperation );

    iMenu.OpenL( KMyMenuData );

    UpdateDataL();
    }

// ---------------------------------------------------------------------------
// Destructor
// ---------------------------------------------------------------------------
//
CMCSPluginData::~CMCSPluginData()
    {
    if( iPluginSettings )
        {
        iPluginSettings->RemoveObserver( this );
        }

    iData.Close();
    iMenu.Close();

    delete iSaveWatcher;
    delete iMsvSession;
    }

// ---------------------------------------------------------------------------
// Gets the instance specific settings from HSPS and creates data items
// ---------------------------------------------------------------------------
//
void CMCSPluginData::UpdateDataL()
    {
    RPointerArray<CItemMap> settings;
    TCleanupItem settingsCleanupItem( ItemMapArrayCleanupFunc, &settings );
    CleanupStack::PushL( settingsCleanupItem );
    iPluginSettings->GetSettingsL( iInstanceUid, settings );
    TInt count = settings.Count();
    TBool wasEmpty = !iData.Count();
    
    for ( TInt i = 0; i < count; i++ )
       {
        CItemMap* itemMap = settings[ i ];
        RPointerArray<HSPluginSettingsIf::CPropertyMap>& properties
            = itemMap->Properties();
        TMenuItem item = CreateMenuItemL( properties );
        TMCSData data;

        if ( wasEmpty )
            {
            // list of shortcut slot was empty
            // we append the shortcut data slots one-by-one to the list
            data.SetMenuItem( item );
            data.SetDirty( ETrue );
            iData.AppendL( data );
            }
        else 
            {
            // check for updates in existing shortcut data slot
            // if menuitem id has changed, replace the item and 
            // set as dirty
            TInt id = -1;
            id = iData[ i ].MenuItem().Id();
            
            if ( item.Id() != id )
                {
                data.SetMenuItem( item );
                data.SetDirty( ETrue );
                iData.Remove( i );
                iData.InsertL( data, i );
                }
            }
        }
    
        CleanupStack::PopAndDestroy(); // settingsCleanupItem 
    }


// ---------------------------------------------------------------------------
// 
// ---------------------------------------------------------------------------
//
void CMCSPluginData::SettingsChangedL( const TDesC8& /*aEvent*/,  const TDesC8& /*aPluginName*/,
                           const TDesC8& /*aPluginUid*/, const TDesC8& aPluginId )
    {
    if( aPluginId.CompareF( iInstanceUid ) == 0 )
        {
        UpdateDataL();
        }   
    }

// ---------------------------------------------------------------------------
// Gets the menu item from engine using the setting properties as filter
// ---------------------------------------------------------------------------
//
TMenuItem CMCSPluginData::CreateMenuItemL( RPointerArray<HSPluginSettingsIf::CPropertyMap>& aProperties )
    {
    CMenuFilter* filter = CMenuFilter::NewLC();

    // Exclude 'view' and 'param' attributes from search criteria by default
    // Criterias will be added to filter if setting defines them
    filter->DoNotHaveAttributeL( KMenuAttrView );
    filter->DoNotHaveAttributeL( KMenuAttrParam );
    TBool isFolder = EFalse;
    
    // first, we need to check if the item is folder
    for ( TInt i = 0; i < aProperties.Count(); i++ )
        {
        if ( aProperties[i]->Name() == KProperNameType )
            {
            if ( aProperties[i]->Value() == KProperValueFolder )
                {
                isFolder = ETrue;
                }
            break;
            }
        }

    // then add all property/value pairs to the filter
    for ( TInt i = 0; i < aProperties.Count(); i++ )
        {

        // skip the type property
        if( aProperties[i]->Name() == KProperNameType )
            {
            continue;
            }

        HBufC* name( NULL );
        HBufC* value( NULL );
        name = AiUtility::CopyToBufferL( name, aProperties[i]->Name() );
        CleanupStack::PushL( name );
        value = AiUtility::CopyToBufferL( value, aProperties[i]->Value() );
        CleanupStack::PushL( value );

        if ( value->Length() != 0 )
            {
            // in case of folder, we just have to extract 
            // id from param attribute and return item with this id
            if ( aProperties[i]->Name() ==  KProperNameParam && isFolder )
                {
                TMenuItem item;
                // convert id to integer
                TInt id;
                TLex16 lextmp( value->Ptr() );
                lextmp.Val( id );
                item.SetType( KMenuTypeFolder );
                item.SetId( id );

                CleanupStack::PopAndDestroy( value );
                CleanupStack::PopAndDestroy( name );
                CleanupStack::PopAndDestroy( filter );

                return item;
                }
            else
                {
                // otherwise, we just add name/value into filter
                filter->HaveAttributeL( *name, *value );
                }
            }
        CleanupStack::PopAndDestroy( value );
        CleanupStack::PopAndDestroy( name );
        }
    
    // locked property excluded from search pattern 
    filter->DoNotHaveAttributeL( KMenuAttrLocked );
    
    TMenuItem item = iEngine.FindMenuItemL( *filter );
    CleanupStack::PopAndDestroy( filter );
    return item;
    }

// ---------------------------------------------------------------------------
// Returns menu item for given index
// ---------------------------------------------------------------------------
//
TMCSData& CMCSPluginData::DataItemL( TInt aIndex )
    {
    if( aIndex < 0 || aIndex >= iData.Count())
        {
        User::Leave( KErrArgument ); 
        }
    return iData[aIndex];
    }

// ---------------------------------------------------------------------------
// Replaces menuitem in data instance
// ---------------------------------------------------------------------------
//
void CMCSPluginData::ReplaceMenuItemL( const TInt& aIndex, TMenuItem& aMenuItem )
    {
    TMCSData& data = iData[aIndex];
    data.SetMenuItem( aMenuItem );
    data.SetDirty( ETrue );
    }

// ---------------------------------------------------------------------------
// Save the setting persistently to HSPS
// TODO HSPS setting api should be changed so that items and properties can 
// be added/removed dynamically. Now widgetconfiguration.xml must have all the
// properties for every item even though property is not used.
// It makes this function more compolicated.
// ---------------------------------------------------------------------------
//
void CMCSPluginData::SaveSettingsL( const TInt& aIndex, CMenuItem& aMenuItem )
    {
    RPointerArray<CItemMap> settingItems;
    CleanupClosePushL( settingItems );
    iPluginSettings->GetSettingsL( iInstanceUid, settingItems );
    if ( aIndex >= 0 && aIndex < settingItems.Count() )
        {
        TBool exists( EFalse );
        CItemMap* itemMap = settingItems[aIndex];
        RPointerArray<HSPluginSettingsIf::CPropertyMap> properties;
        properties = itemMap->Properties();
        for ( TInt i= 0; i < properties.Count(); i++ )
            {
            if ( properties[i]->Name() == KProperNameType )
                {
                TPtrC type = aMenuItem.Type();
                if ( type == KMenuTypeUrl )
                    {
                    properties[i]->SetValueL( KProperValueBookmark );
                    }
                else
                    {
                    properties[i]->SetValueL( KProperValueAppl );
                    }
                }
            else if ( properties[i]->Name() == KProperNameUid )
                {
                TPtrC uid = aMenuItem.GetAttributeL( KMenuAttrUid, exists );
                if ( exists )
                    {
                    HBufC8* uid8( NULL );
                    uid8 = AiUtility::CopyToBufferL( uid8, uid );
                    CleanupStack::PushL( uid8 );
                    properties[i]->SetValueL( *uid8 );
                    CleanupStack::PopAndDestroy( uid8 );
                    }
                else
                    {
                    properties[i]->SetValueL( KNullDesC8 );
                    }
                }
            else if ( properties[i]->Name() == KProperNameView )
                {
                TPtrC view = aMenuItem.GetAttributeL( KMenuAttrView, exists );
                if ( exists )
                    {
                    HBufC8* view8( NULL );
                    view8 = AiUtility::CopyToBufferL( view8, view );
                    CleanupStack::PushL( view8 );
                    properties[i]->SetValueL( *view8 );
                    CleanupStack::PopAndDestroy( view8 );
                    }
                else
                    {
                    properties[i]->SetValueL( KNullDesC8 );
                    }
                }
            else if ( properties[i]->Name() == KProperNameParam )
                {
                TPtrC param = aMenuItem.GetAttributeL( KMenuAttrParam, exists );
                if ( exists )
                    {
                    HBufC8* param8( NULL );
                    param8 = AiUtility::CopyToBufferL( param8, param );
                    CleanupStack::PushL( param8 );
                    properties[i]->SetValueL( *param8 );
                    CleanupStack::PopAndDestroy( param8 );
                    }
                else
                    {
                    properties[i]->SetValueL( KNullDesC8 );
                    }
                }
            }
        }
    // ETrue tells that changes are stored also to plugin reference 
    iPluginSettings->SetSettingsL( iInstanceUid, settingItems, ETrue );
    CleanupStack::PopAndDestroy( &settingItems );
    }

// ---------------------------------------------------------------------------
// Gets MCS Plugin folder ID. This hidden folder in matrixmenudata.xml is used 
// for storing run-time generated menuitems
// ---------------------------------------------------------------------------
//
TInt CMCSPluginData::GetMCSPluginFolderIdL()
    {
    TInt folderId;
    
    _LIT( KMCSFolder, "mcsplugin_folder" );

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
        folderId = item->Id();
        }
    else 
        {
        folderId = iMenu.RootFolderL();
        }
    CleanupStack::PopAndDestroy( &itemArray );
    CleanupStack::PopAndDestroy( filter ); 

    delete item; 

    return folderId;
    }

// ---------------------------------------------------------------------------
// Creates runtime generated menuitems (bookmarks/mailboxes if they 
// it does not exist yet in MCS. If they do, their ref_count is incremented.
// Called during Plugin startup.
// ---------------------------------------------------------------------------
//
void CMCSPluginData::CreateRuntimeMenuItemsL()
    {
    
    // start mailboxes observing and get the number of
    // mailboxes defined in the device

    TMsvId entryID = KMsvRootIndexEntryIdValue;
    CMsvEntry* rootEntry = iMsvSession->GetEntryL( entryID );
    TInt mailboxCount = rootEntry->Count();
    CleanupStack::PushL( rootEntry );
    
    RPointerArray<CItemMap> settings;
    TCleanupItem settingsCleanupItem( ItemMapArrayCleanupFunc, &settings );
    CleanupStack::PushL( settingsCleanupItem );
    iPluginSettings->GetSettingsL( iInstanceUid, settings );

    RFavouritesSession bookmarkSess;
    RFavouritesDb bookmarkDb;

    User::LeaveIfError( bookmarkSess.Connect() );
    User::LeaveIfError( bookmarkDb.Open( bookmarkSess, KBrowserBookmarks ));

    TInt count = settings.Count();
    for( TInt i = 0; i < count; i++ )
       {
        CItemMap* itemMap = settings[i];
        RPointerArray<HSPluginSettingsIf::CPropertyMap>& properties
            = itemMap->Properties();

        TPtrC8 uid8, type, param8;

        for( TInt j = 0; j < properties.Count(); j++ )
            {

            if( properties[j]->Name() == KProperNameType )
                {
                type.Set( properties[j]->Value() );
                }
            else if ( properties[j]->Name() == KProperNameUid )
                {
                uid8.Set( properties[j]->Value() );
                }
            else if ( properties[j]->Name() ==  KProperNameParam )
                {
                param8.Set( properties[j]->Value() );
                }
            }

        if( type == KProperValueBookmark )
            {
            
            // The shortcut is a bookmark
            TMenuItem menuItem = CreateMenuItemL( properties );

            CActiveSchedulerWait* wait = 
                            new ( ELeave ) CActiveSchedulerWait;
            CleanupStack::PushL( wait );
            
            if( menuItem.Id() == 0 )
                {
                TLex8 uidLex( uid8.Mid( 1, uid8.Length() - 2 ) );
                TUint32 id;
                uidLex.Val(id, EHex);

                CFavouritesItem* bkmItem = CFavouritesItem::NewLC();
                TInt bcount = bookmarkDb.Get( TInt32( id ), *bkmItem );

                HBufC *uid( NULL );
                uid = AiUtility::CopyToBufferL( uid, uid8 );
                CleanupStack::PushL( uid );

                CMenuItem* newItem = CMenuItem::CreateL( iMenu, 
                                                         KMenuTypeUrl, 
                                                         GetMCSPluginFolderIdL(),
                                                         0 );
                CleanupStack::PushL( newItem );

                newItem->SetAttributeL( KMenuAttrUid, *uid );
                newItem->SetAttributeL( KMenuAttrLongName, bkmItem->Name() );
                newItem->SetAttributeL( KMenuAttrIconFile, KMenuIconFile );
                newItem->SetAttributeL( KMenuAttrIconId, KMenuBookmarkIconId );
                newItem->SetAttributeL( KMenuAttrMaskId, KMenuBookmarkMaskId );
                newItem->SetAttributeL( KMenuAttrRefcount, KInitialRefCount );
                newItem->SetAttributeL( KUrl , bkmItem->Url() );

                CMenuOperation* op = newItem->SaveL( iSaveWatcher->iStatus );

                iSaveWatcher->StopAndWatch( op, wait );

                // Start the nested scheduler loop.
                wait->Start();

                SaveSettingsL( i, *newItem );

                CleanupStack::PopAndDestroy( newItem );
                CleanupStack::PopAndDestroy( uid );
                CleanupStack::PopAndDestroy( bkmItem );
                }
            else
                {
                CMenuItem* item = CMenuItem::OpenL( iMenu, menuItem );
                CleanupStack::PushL( item );
                if ( iEngine.UpdateMenuItemsRefCountL( item, 1 ) > 0 )
                    {
                    CMenuOperation* op = item->SaveL( iSaveWatcher->iStatus );
                    iSaveWatcher->StopAndWatch( op, wait );
                    // Start the nested scheduler loop.
                    wait->Start();
                    SaveSettingsL( i, *item );
                    }
                CleanupStack::PopAndDestroy( item );
                }

            CleanupStack::PopAndDestroy( wait );
            wait = NULL;
            }

        else if ( uid8 == KMailboxUid8 && mailboxCount > 0 )
             {
             // The shortcut is a Mailbox
             
             TMenuItem menuItem = CreateMenuItemL( properties );

             CActiveSchedulerWait* wait = 
                             new ( ELeave ) CActiveSchedulerWait;
             CleanupStack::PushL( wait );

             if ( menuItem.Id() == 0 )
                {
                // mailbox menuitem does not exist yet. We have to create it
                // first, we try to find its ID among existing mailboxes:bì 

                // extract Mailbox ID from HSPS
                TInt pos = param8.Locate( TChar( ':' ) ) + 1;
                TPtrC8 mailboxId8 = param8.Mid( pos );
                
                HBufC *mailboxId( NULL );
                mailboxId = AiUtility::CopyToBufferL( mailboxId, mailboxId8 );
                CleanupStack::PushL( mailboxId );
                
                // compare ID with existing mailboxes
                rootEntry->SetSortTypeL( TMsvSelectionOrdering( 
                            KMsvGroupByType | KMsvGroupByStandardFolders, 
                            EMsvSortByDetailsReverse, ETrue ) );
                
                TBuf<255> boxId;
                TBool found = EFalse;
                TInt index = rootEntry->Count();

                while ( !found && --index >= 0 )
                    {
                    const TMsvEntry& tentry = ( *rootEntry )[ index ];
                    if ( tentry.iMtm.iUid == KMCSCmailMtmUidValue )
                        {
                        boxId.Num( tentry.Id() );
                        if ( boxId == *mailboxId )
                            {
                            found = ETrue;
                            }
                        }
                    }

                CleanupStack::PopAndDestroy( mailboxId );
                
                // mailbox still exists in Mail application
                // we have to create a new menuitem
                if ( found )
                    {

                    // get the mailbox name
                    const TMsvEntry& tentry = ( *rootEntry )[ index ];
                    TPtrC name = tentry.iDetails;

                    HBufC *param( NULL );
                    param = AiUtility::CopyToBufferL( param, param8 );
                    CleanupStack::PushL( param );

                    // create a new menuitem with ref_count 1
                    CMenuItem* newItem = CMenuItem::CreateL( iMenu, 
                                                         KMenuTypeShortcut, 
                                                         GetMCSPluginFolderIdL(), 
                                                         0 );
                    CleanupStack::PushL( newItem );

                    // mailbox is a shortcut item with "mailbox:mailboxID" parameter
                    newItem->SetAttributeL( KMenuAttrUid, KMailboxUid );
                    newItem->SetAttributeL( KMenuAttrLongName, name );
                    newItem->SetAttributeL( KMenuAttrParameter, *param );
                    newItem->SetAttributeL( KMenuAttrRefcount, KInitialRefCount );

                    // setting icon for the shortcut
                    newItem->SetAttributeL( KMenuAttrIconFile, KMenuIconFile );
                    newItem->SetAttributeL( KMenuAttrIconId, KMenuMailboxIconId );
                    newItem->SetAttributeL( KMenuAttrMaskId, KMenuMailboxMaskId );

                    CMenuOperation* op = newItem->SaveL( iSaveWatcher->iStatus );
                    iSaveWatcher->StopAndWatch( op, wait );

                    // Start the nested scheduler loop.
                    wait->Start();

                    SaveSettingsL( i, *newItem );

                    CleanupStack::PopAndDestroy( newItem );
                    CleanupStack::PopAndDestroy( param );
                    }
                }
             else 
                {
                // mailbox menu item already exists -> increment ref_count by 1
                CMenuItem* item = CMenuItem::OpenL( iMenu, menuItem );
                CleanupStack::PushL( item );
                if ( iEngine.UpdateMenuItemsRefCountL( item, 1 ) > 0 )
                    {
                    CMenuOperation* op = item->SaveL( iSaveWatcher->iStatus );
                    iSaveWatcher->StopAndWatch( op, wait );

                    // Start the nested scheduler loop.
                    wait->Start();
                    SaveSettingsL( i, *item );
                    }
                 CleanupStack::PopAndDestroy( item );
                }
             CleanupStack::PopAndDestroy( wait );
             wait = NULL;
             
             }
        }
    CleanupStack::PopAndDestroy(); // settingsCleanupItem
    CleanupStack::PopAndDestroy( rootEntry );
    }

// ---------------------------------------------------------------------------
// From class MMsvSessionObserver.
// Handles an event from the message server.
// ---------------------------------------------------------------------------
//
void CMCSPluginData::HandleSessionEventL(
                                     TMsvSessionEvent /*aEvent*/, 
                                     TAny* /*aArg1*/, 
                                     TAny* /*aArg2*/,
                                     TAny* /*aArg3*/ )
    {

    }
