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


//  CLASS HEADER
#include "UT_mcssettings.h"

//  EXTERNAL INCLUDES

#define protected public
#define private public
#include "mcspluginsettingsapplist.h"
#include "mcspluginsettingsbkmlist.h"
#include <gsbaseview.h>
#undef private
#undef protected

#include <msvuids.h>        // For KMsvRootIndexEntryIdValue
#include <senduimtmuids.h>

#include <digia/eunit/eunitmacros.h>
#include <GSPrivatePluginProviderIds.h>
#include <aknappui.h>
#include <aknviewappui.h>
#include <eikenv.h>
#include <CentralRepository.h>
#include <apgcli.h>         //RApaLsSession
#include <w32std.h>                 // For RWsSession

//  INTERNAL INCLUDES
#include "mcspluginsettings.h"
#include "mcspluginsettingsapplist.h"
#include "mcspluginsettingscontainer.h"
#include "mcspluginsettingsmodel.h"
#include "mcspluginsettingsbkmlist.h"

#include "caouserafter.h"
#include "mcspluginuids.hrh"
#include "mcspluginsettings.hrh"

#include <favouritesitemlist.h>
#include <mcsmenuitem.h>
#include <mcsmenufilter.h>
#include <hspluginsettings.h>
#include <apgtask.h>
#include <hlplch.h>


const TUid KCRUidAvkon = { 0x101F876E };
const TUint32 KAknMiddleSoftkeyEnabled = 0x0000000A;

const TInt KWait_05_secs =  0.5 * 1000000;
const TInt KWait_1_secs  =    1 * 1000000;
const TInt KWait_2_secs  =    2 * 1000000;
const TInt KWait_3_secs  =    3 * 1000000;

_LIT8( KEmpty, "" );
_LIT( KMyMenuData, "matrixmenudata" );
_LIT( KMenuUrl, "menu:url" );
_LIT8( KPluginId, "3" );
_LIT(KUid, "0x99999985");
_LIT(KName, "TEST");
_LIT( KUrl, "http://test" );
_LIT8( KUidMCSB8, "0xeeeeffff" );
_LIT( KUidMCSB16, "0xeeeeffff" );


// ----------------------------------------------------------------------------
// CleanupResetAndDestroy()
// ----------------------------------------------------------------------------
//
template<class T>
static void CleanupResetAndDestroy( TAny* aObj )
    {
    if( aObj )
        {
        static_cast<T*>( aObj )->ResetAndDestroy();
        }
    }

// ----------------------------------------------------------------------------
// CleanupResetAndDestroyPushL
// ----------------------------------------------------------------------------
//
template<class T>
static void CleanupResetAndDestroyPushL(T& aArray)
    {
    CleanupStack::PushL( TCleanupItem( &CleanupResetAndDestroy<T>, &aArray ) );
    }


// CONSTRUCTION

// ---------------------------------------------------------------------------
//
// ---------------------------------------------------------------------------
//
UT_mcssettings* UT_mcssettings::NewL()
{
    UT_mcssettings* self = UT_mcssettings::NewLC();
    CleanupStack::Pop();

    return self;
}

// ---------------------------------------------------------------------------
//
// ---------------------------------------------------------------------------
//
UT_mcssettings* UT_mcssettings::NewLC()
{
    UT_mcssettings* self = new(ELeave) UT_mcssettings();
    CleanupStack::PushL(self);

    self->ConstructL();

    return self;
}

// ---------------------------------------------------------------------------
// Destructor (virtual by CBase)
// ---------------------------------------------------------------------------
//
UT_mcssettings::~UT_mcssettings()
{
}

// ---------------------------------------------------------------------------
// Default constructor
// ---------------------------------------------------------------------------
//
UT_mcssettings::UT_mcssettings()
{
}

// ---------------------------------------------------------------------------
// Second phase construct
// ---------------------------------------------------------------------------
//
void UT_mcssettings::ConstructL()
{
    // The ConstructL from the base class CEUnitTestSuiteClass must be called.
    // It generates the test case table.
    CEUnitTestSuiteClass::ConstructL();
}

//  METHODS

// ---------------------------------------------------------------------------
//
// ---------------------------------------------------------------------------
//
void UT_mcssettings::SetupL()
    {
    //__UHEAP_MARK;
    iGs = CGSPluginInterface::NewL(TUid::Uid(AI_UID_ECOM_IMPLEMENTATION_SETTINGS_MCSPLUGIN), NULL);
    iSettingsView = static_cast<CGSBaseView*>(iGs);

    iAppUi = reinterpret_cast< CAknViewAppUi* >(CEikonEnv::Static()->EikAppUi());

    iModel = CMCSPluginSettingsModel::NewL(*(CMCSPluginSettings::NewL(NULL)),CEikonEnv::Static());
    }

// ---------------------------------------------------------------------------
//
// ---------------------------------------------------------------------------
//
void UT_mcssettings::Teardown()
    {
    delete iSettingsView;
    iSettingsView = NULL;

    if ( iGs )
        {
        iGs = NULL;
        }

    if ( iAppList )
        {
        delete iAppList;
        iAppList = NULL;
        if ( iBkmList )
            {
            delete iBkmList;
            iBkmList = NULL;
            }
        
        iContainer = NULL;
        }
    else
        {
        if ( iBkmList )
            {
            delete iBkmList;
            iBkmList = NULL;
            iContainer = NULL;
            }
        else
            {
            if( iContainer )
                {
                delete iContainer;
                iContainer = NULL;
                }
            }
        }   

    if ( iModel )
        {
        delete iModel;
        iModel = NULL;
        }

    iItemArray.Reset();
   //__UHEAP_MARKEND;
    }

// ---------------------------------------------------------------------------
//
// ---------------------------------------------------------------------------
//
void UT_mcssettings::emptySetup()
{
}

//----------------------------------------------------------------------------
// Tests for CMCSPluginSettings
//----------------------------------------------------------------------------

// ---------------------------------------------------------------------------
//
// ---------------------------------------------------------------------------
//
void UT_mcssettings::UT_Settings_FailL()
{
    TVwsViewId dummyId;

    iSettingsView->DoActivateL(dummyId, KNullUid, KNullDesC8);
    CAOUserAfter::AfterLD(KWait_05_secs);

    iSettingsView->DoDeactivate();
    CAOUserAfter::AfterLD(KWait_05_secs);

    EUNIT_ASSERT(EFalse);
}

// ---------------------------------------------------------------------------
//
// ---------------------------------------------------------------------------
//
void UT_mcssettings::UT_Settings_NewLL()
{
    EUNIT_ASSERT(iSettingsView);
    iSettingsView->DoDeactivate();
    CAOUserAfter::AfterLD(KWait_05_secs);
}

// ---------------------------------------------------------------------------
//
// ---------------------------------------------------------------------------
//
void UT_mcssettings::UT_Settings_IdL()
{
    EUNIT_ASSERT_EQUALS(TUid::Uid(AI_UID_ECOM_IMPLEMENTATION_SETTINGS_MCSPLUGIN), iSettingsView->Id());
    
    iSettingsView->DoDeactivate();
    CAOUserAfter::AfterLD(KWait_05_secs);
}

// ---------------------------------------------------------------------------
//
// ---------------------------------------------------------------------------
//
void UT_mcssettings::UT_Settings_HandleCommandLL()
{
    TRAPD(err, iSettingsView->HandleCommandL(EAknSoftkeyExit));
    EUNIT_ASSERT_EQUALS(err, KErrNone);    
}

// ---------------------------------------------------------------------------
//
// ---------------------------------------------------------------------------
//
void UT_mcssettings::UT_Settings_DoDeactivateL()
{
    TVwsViewId dummyId;

    iSettingsView->DoActivateL(dummyId, KNullUid, KNullDesC8);
    CAOUserAfter::AfterLD(KWait_1_secs);

    iSettingsView->DoDeactivate();
    CAOUserAfter::AfterLD(KWait_1_secs);
    
}

// ---------------------------------------------------------------------------
//
// ---------------------------------------------------------------------------
//
void UT_mcssettings::UT_Settings_GetCaptionLL()
{
    TBuf<50> caption = _L("Shortcuts");
    TBuf<50> desc;

    iGs->GetCaptionL(desc);

    EUNIT_ASSERT_EQUALS(caption, desc);
    
    iSettingsView->DoDeactivate();
    CAOUserAfter::AfterLD(KWait_05_secs);
}

// ---------------------------------------------------------------------------
//
// ---------------------------------------------------------------------------
//
void UT_mcssettings::UT_Settings_PluginProviderCategoryL()
{
    EUNIT_ASSERT_EQUALS(KGSPluginProviderInternal, iGs->PluginProviderCategory());
    
    iSettingsView->DoDeactivate();
    CAOUserAfter::AfterLD(KWait_05_secs);
}

// ---------------------------------------------------------------------------
//
// ---------------------------------------------------------------------------
//
void UT_mcssettings::UT_Settings_VisibleL()
{
    EUNIT_ASSERT_EQUALS(EFalse,iGs->Visible());
    
    iSettingsView->DoDeactivate();
    CAOUserAfter::AfterLD(KWait_05_secs);
}

// ---------------------------------------------------------------------------
//
// ---------------------------------------------------------------------------
//
void UT_mcssettings::SetupAppListL()
{
    SetupL();

    TVwsViewId dummyId;
    iSettingsView->DoActivateL(dummyId, KNullUid, KNullDesC8);
    CAOUserAfter::AfterLD(KWait_05_secs);

    iContainer =
        static_cast<CMCSPluginSettingsContainer*>( iSettingsView->Container() );
    EUNIT_ASSERT(iContainer);

    iAppList = CMCSPluginSettingsAppList::NewL();

    iAppList->StartL();

    //while (!iAppList->IsReady())
    {
        CAOUserAfter::AfterLD(KWait_05_secs);
    }
}

// ---------------------------------------------------------------------------
//
// ---------------------------------------------------------------------------
//
void UT_mcssettings::TeardownAppList()
{
    iSettingsView->DoDeactivate();
    CAOUserAfter::AfterLD(KWait_05_secs);
    Teardown();
}

// ---------------------------------------------------------------------------
//
// ---------------------------------------------------------------------------
//
void UT_mcssettings::UT_SettingsAppList_NewLL()
{
    EUNIT_ASSERT(iAppList);
}

// ---------------------------------------------------------------------------
//
// ---------------------------------------------------------------------------
//
void UT_mcssettings::UT_SettingsAppList_MdcaCountL()
{
    TVwsViewId dummyId;
    TInt count;

    iSettingsView->DoActivateL(dummyId, KNullUid, KNullDesC8);
    CAOUserAfter::AfterLD(KWait_05_secs);

    iContainer =
        static_cast<CMCSPluginSettingsContainer*>( iSettingsView->Container() );
    EUNIT_ASSERT(iContainer);

    iAppList = CMCSPluginSettingsAppList::NewL();
    count = iAppList->MdcaCount();
    iAppList->StartL();

    //while (!iAppList->IsReady())
    {
        CAOUserAfter::AfterLD(KWait_05_secs);
    }

    EUNIT_ASSERT_NOT_EQUALS(count, iAppList->MdcaCount());

}

// ---------------------------------------------------------------------------
//
// ---------------------------------------------------------------------------
//
void UT_mcssettings::UT_SettingsAppList_MdcaPointL()
{
    TPtrC ret(KNullDesC);
    EUNIT_ASSERT_EQUALS(ret,iAppList->MdcaPoint(-1));
    
    EUNIT_ASSERT_EQUALS(ret,iAppList->MdcaPoint(208));
    
    TBufC<30> buf = iAppList->MdcaPoint(0);
    
    EUNIT_ASSERT(buf.Length() > 0);
}

// ---------------------------------------------------------------------------
//
// ---------------------------------------------------------------------------
//
void UT_mcssettings::UT_SettingsAppList_HandleSessionEventLL()
    {
    TAny* dummy = NULL;

    for (TInt i = 0; i < 10; i++)
        {
        TRAPD(err,
              iAppList->HandleSessionEventL(
                  MMsvSessionObserver::TMsvSessionEvent(i),
                  dummy,
                  dummy,
                  dummy) );
        EUNIT_ASSERT_EQUALS(err, KErrNone);
        }
    }


// ---------------------------------------------------------------------------
//
// ---------------------------------------------------------------------------
//
void UT_mcssettings::SetupBkmListL()
{
    SetupL();

    TVwsViewId dummyId;
    iSettingsView->DoActivateL(dummyId, KNullUid, KNullDesC8);
    CAOUserAfter::AfterLD(KWait_05_secs);

    iContainer =
        static_cast<CMCSPluginSettingsContainer*>( iSettingsView->Container() );
    EUNIT_ASSERT(iContainer);
    
    iBkmList = CMCSPluginSettingsBkmList::NewL();
    CAOUserAfter::AfterLD(KWait_05_secs);
    
}

// ---------------------------------------------------------------------------
//
// ---------------------------------------------------------------------------
//
void UT_mcssettings::TeardownBkmList()
{
    if ( iRemoveWatcher )
        {
        delete iRemoveWatcher;
        iRemoveWatcher = NULL;
        }
    if ( iSaveWatcher )
        {
        delete iSaveWatcher;
        iSaveWatcher = NULL;
        }

    iMenu.Close();
    iSettingsView->DoDeactivate();
    CAOUserAfter::AfterLD(KWait_05_secs);
    Teardown();
}

// ---------------------------------------------------------------------------
//
// ---------------------------------------------------------------------------
//
void UT_mcssettings::UT_SettingsBkmList_NewLL()
{
    EUNIT_ASSERT(iBkmList);
}

// ---------------------------------------------------------------------------
//
// ---------------------------------------------------------------------------
//
void UT_mcssettings::UT_SettingsBkmList_MdcaCountL()
{
    TInt count1( -1 );
    iBkmList->GetBookmarkListL();
    count1 = iBkmList->MdcaCount();

    RFavouritesSession bookmarkSess;
    RFavouritesDb bookmarkDb;

    User::LeaveIfError( bookmarkSess.Connect() );
    User::LeaveIfError(bookmarkDb.Open( bookmarkSess, KBrowserBookmarks) );
    iMenu.OpenL( KMyMenuData );

    CMenuFilter* filter = CMenuFilter::NewL();
    CleanupStack::PushL( filter );
    filter->SetType( KMenuUrl );
    const TInt rootId = iMenu.RootFolderL();

    RArray<TMenuItem> itemArray;
    CleanupClosePushL( itemArray );
    iMenu.GetItemsL( itemArray, rootId, filter, ETrue );
    TInt countmenu( itemArray.Count() );

    CFavouritesItemList* favItems = new ( ELeave ) CFavouritesItemList();
    CleanupStack::PushL( favItems );
    TInt err = bookmarkDb.GetAll( *favItems, KFavouritesNullUid,
                                  CFavouritesItem::EItem );
    if( err != KErrNone )
       {
       EUNIT_ASSERT( 0 );
       }
    TInt countfav = favItems->Count();

    EUNIT_ASSERT_EQUALS( countfav + countmenu, count1 );

    CleanupStack::PopAndDestroy( favItems );  
    CleanupStack::PopAndDestroy( &itemArray );
    CleanupStack::PopAndDestroy( filter );  
         
    bookmarkDb.Close();
    bookmarkSess.Close();
}

// ---------------------------------------------------------------------------
//
// ---------------------------------------------------------------------------
//
void UT_mcssettings::UT_SettingsBkmList_MdcaPointL()
{
    iBkmList->GetBookmarkListL();
    
    TPtrC ret(KNullDesC);
    EUNIT_ASSERT_EQUALS(ret,iBkmList->MdcaPoint(-1));
    
    EUNIT_ASSERT_EQUALS(ret,iBkmList->MdcaPoint(200));
    
    if ( iBkmList->MdcaCount() > 0 )
        {
        TBufC<30> buf = iBkmList->MdcaPoint(0);
        EUNIT_ASSERT(buf.Length() > 0);
        }
}


// ---------------------------------------------------------------------------
//
// ---------------------------------------------------------------------------
//
void UT_mcssettings::UT_SettingsContainer_IsChangeDialogShowingL()
{
    TRAPD(err, iContainer = new (ELeave) CMCSPluginSettingsContainer);
    EUNIT_ASSERT_EQUALS(err, KErrNone);

    iSettingsView->DoDeactivate();
    CAOUserAfter::AfterLD(KWait_05_secs);
}

// ---------------------------------------------------------------------------
//
// ---------------------------------------------------------------------------
//
void UT_mcssettings::UT_SettingsContainer_CloseChangeDialogL()
{
    TRAPD(err, iContainer = new (ELeave) CMCSPluginSettingsContainer);
    EUNIT_ASSERT_EQUALS(err, KErrNone);

    iContainer->CloseChangeDialog();
    iSettingsView->DoDeactivate();
    CAOUserAfter::AfterLD(KWait_05_secs);
}


// ---------------------------------------------------------------------------
//
// ---------------------------------------------------------------------------
//

void UT_mcssettings::UT_SettingsModel_NewLL()
{
    EUNIT_ASSERT(iModel);
    iSettingsView->DoDeactivate();
    CAOUserAfter::AfterLD(KWait_05_secs);
}

// ---------------------------------------------------------------------------
//
// ---------------------------------------------------------------------------
//
void UT_mcssettings::UT_SettingsModel_MdcaCountL()
{
    EUNIT_ASSERT_NOT_EQUALS(NULL, iModel->MdcaCount());
    iSettingsView->DoDeactivate();
    CAOUserAfter::AfterLD(KWait_05_secs);
}

// ---------------------------------------------------------------------------
//
// ---------------------------------------------------------------------------
//
void UT_mcssettings::UT_SettingsModel_MdcaPointL()
    {
    TInt outtaBounds = 2;

    for ( TInt i = -1; i < iModel->MdcaCount() + outtaBounds; i++ )
    {
        if ( i < 0 || i >= iModel->MdcaCount() )
            {
            EUNIT_ASSERT_EQUALS( TPtrC( KNullDesC16 ),
                                 iModel->MdcaPoint( i ) );
            }
        else
            {
            TPtrC desc = iModel->MdcaPoint( i );
            EUNIT_ASSERT_NOT_EQUALS( desc.Length(), NULL );
            }
    }
    iSettingsView->DoDeactivate();
    CAOUserAfter::AfterLD( KWait_05_secs );
    }

// ---------------------------------------------------------------------------
//
// ---------------------------------------------------------------------------
//
void UT_mcssettings::UT_SettingsAppList_ItemL()
    {
    CMenuItem* menuItem;
    TInt maxIndex = iAppList->MdcaCount();

    if ( maxIndex > 0 )
        {
        menuItem = iAppList->ItemL( maxIndex - 1 );
        EUNIT_ASSERT( menuItem );
        }

    maxIndex = -1;
    menuItem = iAppList->ItemL( maxIndex );

    EUNIT_ASSERT_EQUALS( menuItem == NULL, ETrue );
    }

// ---------------------------------------------------------------------------
//
// ---------------------------------------------------------------------------
//
void UT_mcssettings::UT_SettingsAppList_FindItemL()
    {
    RPointerArray<HSPluginSettingsIf::CPropertyMap> properties;
    CleanupResetAndDestroyPushL( properties );
    HSPluginSettingsIf::CPropertyMap* typePropertyMap = 
        HSPluginSettingsIf::CPropertyMap::NewL();
    CleanupStack::PushL( typePropertyMap );
    typePropertyMap->SetNameL( _L8( "type" ) );
    typePropertyMap->SetValueL( _L8( "application" ) );
    properties.AppendL( typePropertyMap );
    CleanupStack::Pop( typePropertyMap );

    HSPluginSettingsIf::CPropertyMap* uidPropertyMap = 
        HSPluginSettingsIf::CPropertyMap::NewL();
    CleanupStack::PushL( uidPropertyMap );
    uidPropertyMap->SetNameL(  _L8( "uid" ) );
    uidPropertyMap->SetValueL( _L8( "0xeeeebbbb" ) );
    properties.AppendL( uidPropertyMap );
    CleanupStack::Pop( uidPropertyMap );

    TSettingItem setting = iAppList->FindItemL( properties );
    EUNIT_ASSERT_EQUALS( setting.id == KErrNotFound, ETrue );

    CleanupStack::PopAndDestroy();    //properties
    }

// ---------------------------------------------------------------------------
//
// ---------------------------------------------------------------------------
//
void UT_mcssettings::UT_SettingsBkmList_ItemL()
    {
    TInt maxIndex = iBkmList->MdcaCount();
    CMenuItem* menu( NULL );
    if ( maxIndex > 0 )
        {
        menu = iBkmList->ItemL( maxIndex - 1 );
        EUNIT_ASSERT_EQUALS( menu != NULL, ETrue );
        }
    }

// ---------------------------------------------------------------------------
//
// ---------------------------------------------------------------------------
//
void UT_mcssettings::UT_SettingsBkmList_FindItemL()
    {
    RPointerArray<HSPluginSettingsIf::CPropertyMap> properties;
    CleanupResetAndDestroyPushL( properties );

    HSPluginSettingsIf::CPropertyMap* myCPropertyMap = 
        HSPluginSettingsIf::CPropertyMap::NewL();
    CleanupStack::PushL( myCPropertyMap );
    myCPropertyMap->SetNameL( _L8( "uid" ) );
    myCPropertyMap->SetValueL( _L8( "0xeeeebbbb" ) );
    properties.AppendL( myCPropertyMap );
    CleanupStack::Pop( myCPropertyMap );

    TSettingItem setting = iBkmList->FindItemL( properties );
    EUNIT_ASSERT_EQUALS( setting.id == KErrNotFound, ETrue );
    CleanupStack::PopAndDestroy();    //properties
    }

// ---------------------------------------------------------------------------
//
// ---------------------------------------------------------------------------
//
void UT_mcssettings::UT_SettingsContainer_ResetCurrrentListL()
    {
    TRAPD(err, iContainer->ResetCurrentListL( 0 ) );
    EUNIT_ASSERT_EQUALS( err, KErrNone );
    }

// ---------------------------------------------------------------------------
//
// ---------------------------------------------------------------------------
//
void UT_mcssettings::UT_SettingsModel_AppList()
    {
    EUNIT_ASSERT( iModel->AppList() );
    }

// ---------------------------------------------------------------------------
//
// ---------------------------------------------------------------------------
//
void UT_mcssettings::UT_SettingsModel_BkmList()
    {
    EUNIT_ASSERT( iModel->BkmList() );
    }

// ---------------------------------------------------------------------------
//
// ---------------------------------------------------------------------------
//
void UT_mcssettings::UT_SettingsModel_ItemId()
    {
    TInt indexMax = iModel->MdcaCount(); 
    TInt indexID( 0 );
    
    if ( indexMax > 0 )
        {
        indexID = iModel->ItemId( indexMax - 1 );
        EUNIT_ASSERT_EQUALS( indexID != KErrNotFound, ETrue );
        }
    }

// ---------------------------------------------------------------------------
//
// ---------------------------------------------------------------------------
//
void UT_mcssettings::UT_SettingsModel_Item()
    {
    TSettingItem setting;
    TInt indexMax = iModel->MdcaCount(); 
    if ( indexMax > 0 )
        {
        setting = iModel->Item( indexMax - 1 );    
        EUNIT_ASSERT_EQUALS( setting.id != KErrNotFound, ETrue );
        }
    }

// ---------------------------------------------------------------------------
//
// ---------------------------------------------------------------------------
//
void UT_mcssettings::UT_SettingsModel_ReplaceItemL()
    {
    TInt indexMax = iModel->MdcaCount();
    TSettingItem setting;

    if ( indexMax > 0 )
        {
        setting = iModel->Item( indexMax - 1 );
        if ( setting.id != KErrNotFound )
            {
             TBool err = iModel->ReplaceItemL( indexMax - 1,
                                               setting.id,
                                               setting.type );
            EUNIT_ASSERT( err );
            }
        }
    }

// ---------------------------------------------------------------------------
//
// ---------------------------------------------------------------------------
//
void UT_mcssettings::SetupModelL()
    {
    SetupL();

    iModel->SetPluginIdL( KPluginId );
    iModel->UpdateAppListL();
    iModel->UpdateBkmListL();
    CAOUserAfter::AfterLD( KWait_05_secs );
    }

// ---------------------------------------------------------------------------
//
// ---------------------------------------------------------------------------
//
void UT_mcssettings::SetupContainerL()
    {
    SetupL();
    TVwsViewId dummyId;
    iSettingsView->DoActivateL(dummyId, KNullUid, KNullDesC8);
    CAOUserAfter::AfterLD( KWait_05_secs );

    iContainer =
        static_cast<CMCSPluginSettingsContainer*>( iSettingsView->Container() );
    EUNIT_ASSERT(iContainer);
    }

// ---------------------------------------------------------------------------
//
// ---------------------------------------------------------------------------
//
void UT_mcssettings::UT_SettingsContainer_HandleNotifyL()
    {
    iMenu.OpenL( KMyMenuData );
    CMenuItem* newItem = CMenuItem::CreateL( iMenu, KMenuTypeApp, 1, 0 );
    CleanupStack::PushL( newItem );
    newItem->SetAttributeL( KMenuAttrUid, KUid );
    newItem->SetAttributeL( KMenuAttrLongName, KName );
  
    iSaveWatcher = CMCSPluginWatcher::NewL( CMCSPluginWatcher::EOperation );
    //add menu item
    CMenuOperation* operationSave = newItem->SaveL( iSaveWatcher->iStatus );
    iSaveWatcher->Watch( operationSave );            
    CAOUserAfter::AfterLD( KWait_05_secs );
    //remove menu item 
    iRemoveWatcher = CMCSPluginWatcher::NewL( CMCSPluginWatcher::EOperation );
    CMenuOperation* operationRemove = iMenu.RemoveL( newItem->Id(),
        iRemoveWatcher->iStatus );
    iRemoveWatcher->Watch( operationRemove );
    CAOUserAfter::AfterLD( KWait_05_secs );

    CleanupStack::PopAndDestroy( newItem );
    }

// ---------------------------------------------------------------------------
//
// ---------------------------------------------------------------------------
//
void UT_mcssettings::TeardownContainer()
    {
    if ( iRemoveWatcher )
        {
        delete iRemoveWatcher;
        iRemoveWatcher = NULL;
        }
    if ( iSaveWatcher )
        {
        delete iSaveWatcher;
        iSaveWatcher = NULL;
        }

    iSettingsView->DoDeactivate();
    CAOUserAfter::AfterLD( KWait_05_secs );
    iContainer = NULL;
    iMenu.Close();
    Teardown();
    }

// ---------------------------------------------------------------------------
//
// ---------------------------------------------------------------------------
//
void UT_mcssettings::UT_SettingsBkmList_MCSMenuItemL()
    {
    iMenu.OpenL( KMyMenuData );
    CMenuItem* newItem = CMenuItem::CreateL( iMenu, KMenuTypeUrl, 1, 0 );
    CleanupStack::PushL( newItem );
    newItem->SetAttributeL( KMenuAttrUid, KUidMCSB16 );
    newItem->SetAttributeL( KMenuAttrLongName, KName );
    newItem->SetAttributeL( KMenuAttrUrl, KUrl );

    //add menu item - EMCSBookmark
    iSaveWatcher = CMCSPluginWatcher::NewL( CMCSPluginWatcher::EOperation );
    CMenuOperation* operationSave = newItem->SaveL( iSaveWatcher->iStatus );
    iSaveWatcher->Watch( operationSave );
    CAOUserAfter::AfterLD( KWait_05_secs );

    iBkmList->GetBookmarkListL();
    RPointerArray<HSPluginSettingsIf::CPropertyMap> properties;
    CleanupClosePushL( properties );
    HSPluginSettingsIf::CPropertyMap* myCPropertyMap = 
        HSPluginSettingsIf::CPropertyMap::NewL();

    CleanupStack::PushL( myCPropertyMap );
    myCPropertyMap->SetNameL( _L8( "uid" ) );
    myCPropertyMap->SetValueL( KUidMCSB8 ); 
    properties.AppendL( myCPropertyMap );
    CleanupStack::Pop( myCPropertyMap );

    TSettingItem setting = iBkmList->FindItemL( properties );
    CleanupStack::PopAndDestroy( &properties );
    CMenuItem* menuItem = iBkmList->ItemL( setting.id );

    EUNIT_ASSERT( menuItem );

    //remove menu item - EMCSBookmark 
    iRemoveWatcher = CMCSPluginWatcher::NewL( CMCSPluginWatcher::EOperation );
    CMenuOperation* operationRemove = iMenu.RemoveL( newItem->Id(),
        iRemoveWatcher->iStatus );
    iRemoveWatcher->Watch( operationRemove );
    CAOUserAfter::AfterLD( KWait_05_secs );

    CleanupStack::PopAndDestroy( newItem );
    }

// ---------------------------------------------------------------------------
//
// ---------------------------------------------------------------------------
//
void UT_mcssettings::UT_SettingsModel_UndefinedText()
    {
    TPtrC undefinedT = iModel->AppList()->UndefinedText();
    EUNIT_ASSERT( &undefinedT != NULL );
    }

void UT_mcssettings::UT_SettingsContainer_HandleHelpCommandL()
    {
    iContainer->HandleHelpCommandL();
    CAOUserAfter::AfterLD( KWait_05_secs );

    TApaTaskList taskList( CCoeEnv::Static()->WsSession() );
    TApaTask task = taskList.FindApp( KHlpAppUid );

    EUNIT_ASSERT( task.Exists() );

    if ( task.Exists() )
        {
        task.EndTask();
        }
    }



//  TEST TABLE
EUNIT_BEGIN_TEST_TABLE
    (
    UT_mcssettings,
    "MCS plugin settings tests.",
    "UNIT"
    )

/*EUNIT_TEST
    (
    "Fail",
    "Settings",
    "Fail",
    "FUNCTIONALITY",
    SetupL, UT_Settings_FailL, Teardown
    )
*/

EUNIT_TEST
    (
    "NewL",
    "Settings",
    "NewL",
    "FUNCTIONALITY",
    SetupL, UT_Settings_NewLL, Teardown
    )

EUNIT_TEST
    (
    "Id",
    "Settings",
    "Id",
    "FUNCTIONALITY",
    SetupL, UT_Settings_IdL, Teardown
    )

EUNIT_TEST
    (
    "DoDeactivate",
    "Settings",
    "DoDeactivate",
    "FUNCTIONALITY",
    SetupL, UT_Settings_DoDeactivateL, Teardown
    )

EUNIT_TEST
    (
    "GetCaptionL",
    "Settings",
    "GetCaptionL",
    "FUNCTIONALITY",
    SetupL, UT_Settings_GetCaptionLL, Teardown
    )

EUNIT_TEST
    (
    "PluginProviderCategory",
    "Settings",
    "PluginProviderCategory",
    "FUNCTIONALITY",
    SetupL, UT_Settings_PluginProviderCategoryL, Teardown
    )

EUNIT_TEST
    (
    "Visible",
    "Settings",
    "Visible",
    "FUNCTIONALITY",
    SetupL, UT_Settings_VisibleL, Teardown
    )

EUNIT_TEST
    (
    "NewL",
    "SettingsAppList",
    "NewL",
    "FUNCTIONALITY",
    SetupAppListL, UT_SettingsAppList_NewLL, TeardownAppList
    )

EUNIT_TEST
    (
    "MdcaCount",
    "SettingsAppList",
    "MdcaCount",
    "FUNCTIONALITY",
    SetupL, UT_SettingsAppList_MdcaCountL, TeardownAppList
    )

EUNIT_TEST
    (
    "MdcaPoint",
    "SettingsAppList",
    "MdcaPointL",
    "FUNCTIONALITY",
    SetupAppListL, UT_SettingsAppList_MdcaPointL, TeardownAppList
    )

EUNIT_TEST
    (
    "HandleSessionEventL",
    "SettingsAppList",
    "HandleSessionEventL",
    "FUNCTIONALITY",
    SetupAppListL, UT_SettingsAppList_HandleSessionEventLL, TeardownAppList
    )

EUNIT_TEST
    (
    "BkmNewL",
    "SettingsBkmList",
    "NewL",
    "FUNCTIONALITY",
    SetupBkmListL, UT_SettingsBkmList_NewLL, TeardownBkmList
    )

EUNIT_TEST
    (
    "BkmMdcaCount",
    "SettingsBkmList",
    "MdcaCount",
    "FUNCTIONALITY",
    SetupBkmListL, UT_SettingsBkmList_MdcaCountL, TeardownBkmList
    )

EUNIT_TEST
    (
    "BkmMdcaPoint",
    "SettingsBkmList",
    "MdcaPointL",
    "FUNCTIONALITY",
    SetupBkmListL, UT_SettingsBkmList_MdcaPointL, TeardownBkmList
    )

EUNIT_TEST
    (
    "IsChangeDialogShowing",
    "SettingsContainer",
    "IsChangeDialogShowing",
    "FUNCTIONALITY",
    SetupL, UT_SettingsContainer_IsChangeDialogShowingL, Teardown
    )

EUNIT_TEST
    (
    "CloseChangeDialog",
    "SettingsContainer",
    "CloseChangeDialog",
    "FUNCTIONALITY",
    SetupL, UT_SettingsContainer_CloseChangeDialogL, Teardown
    )

EUNIT_TEST
    (
    "NewL",
    "SettingsModel",
    "NewL",
    "FUNCTIONALITY",
    SetupL, UT_SettingsModel_NewLL, Teardown
    )

EUNIT_TEST
    (
    "MdcaPoint",
    "SettingsModel",
    "MdcaPoint",
    "FUNCTIONALITY",
    SetupModelL, UT_SettingsModel_MdcaPointL, Teardown
    )

EUNIT_TEST
    (
    "App ItemL",
    "SettingsAppList",
    "ItemL",
    "FUNCTIONALITY",
    SetupAppListL, UT_SettingsAppList_ItemL, TeardownAppList
    )

EUNIT_TEST
    (
    "App FindItemL",
    "SettingsAppList",
    "FindItemL",
    "FUNCTIONALITY",
    SetupAppListL, UT_SettingsAppList_FindItemL, TeardownAppList
    )

EUNIT_TEST
    (
    "Bkm ItemL",
    "SettingsBkmList",
    "ItemL",
    "FUNCTIONALITY",
    SetupBkmListL, UT_SettingsBkmList_ItemL, TeardownBkmList
    )

EUNIT_TEST
    (
    "Bkm FindItemL",
    "SettingsBkmList",
    "FindItemL",
    "FUNCTIONALITY",
    SetupBkmListL, UT_SettingsBkmList_FindItemL, TeardownBkmList
    )

EUNIT_TEST
    (
    "ResetCurrrentListL",
    "SettingsContainer",
    "ResetCurrrentListL",
    "FUNCTIONALITY",
    SetupContainerL, UT_SettingsContainer_ResetCurrrentListL, TeardownContainer
    )

EUNIT_TEST
    (
    "HandleCommandL",
    "Settings",
    "HandleCommandL",
    "FUNCTIONALITY",
    SetupL, UT_Settings_HandleCommandLL, Teardown
    )

EUNIT_TEST
    (
    "AppList",
    "SettingsModel",
    "AppList",
    "FUNCTIONALITY",
    SetupModelL, UT_SettingsModel_AppList, Teardown
    )

EUNIT_TEST
    (
    "BkmList",
    "SettingsModel",
    "BkmList",
    "FUNCTIONALITY",
    SetupModelL, UT_SettingsModel_BkmList, Teardown
    )

EUNIT_TEST
    (
    "ItemId",
    "SettingsModel",
    "ItemId",
    "FUNCTIONALITY",
    SetupModelL, UT_SettingsModel_ItemId, Teardown
    )

EUNIT_TEST
    (
    "Item",
    "SettingsModel",
    "Item",
    "FUNCTIONALITY",
    SetupModelL, UT_SettingsModel_Item, Teardown
    )

EUNIT_TEST
    (
    "ReplaceItemL",
    "SettingsModel",
    "ReplaceItemL",
    "FUNCTIONALITY",
    SetupModelL, UT_SettingsModel_ReplaceItemL, Teardown
    )

EUNIT_TEST
    (
    "HandleNotifyL",
    "SettingsContainer",
    "HandleNotifyL",
    "FUNCTIONALITY",
    SetupContainerL, UT_SettingsContainer_HandleNotifyL, TeardownContainer
    )

EUNIT_TEST
    (
    "MCSMenuItemL",
    "SettingsBkmList",
    "MCSMenuItemL",
    "FUNCTIONALITY",
    SetupBkmListL, UT_SettingsBkmList_MCSMenuItemL, TeardownBkmList
    )
    
EUNIT_TEST
    (
    "UndefinedText",
    "SettingsModel",
    "UndefinedText",
    "FUNCTIONALITY",
    SetupModelL, UT_SettingsModel_UndefinedText, Teardown
    )
    
EUNIT_TEST
    (
    "HandleHelpCommandL",
    "SettingsModel",
    "HandleHelpCommandL",
    "FUNCTIONALITY",
    SetupContainerL, UT_SettingsContainer_HandleHelpCommandL, TeardownContainer
    )

EUNIT_END_TEST_TABLE

//  END OF FILE
