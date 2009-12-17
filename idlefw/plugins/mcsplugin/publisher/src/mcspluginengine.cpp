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
* Description:  The engine class of MCS plugin. 
*
*/


// INCLUDE FILES
#include "mcspluginengine.h"

#include <gulicon.h>
#include <AknsItemID.h>
#include <gslauncher.h>
#include <aisystemuids.hrh>

#include <mcsmenuitem.h>
#include <mcsmenufilter.h>
#include <mcsmenuoperation.h>
#include <mcsmenuiconutility.h>
#include <ActiveFavouritesDbNotifier.h>
#include <FavouritesItemList.h>

#include <bautils.h>
#include <StringLoader.h>
#include <aknnotedialog.h>
#include <avkon.rsg>
#include <mcspluginres.rsg>
#include "mcsplugin.h"
#include "mcsplugindata.h"
#include "mcspluginuids.hrh"

#include <apgtask.h>
#include <apgcli.h> 
#include <apacmdln.h>
#include <gfxtranseffect/gfxtranseffect.h>      // For Transition effect
#include <akntranseffect.h>

#include <AknsConstants.h>
_LIT( KMyMenuData, "matrixmenudata" );
_LIT( KSkin,         "skin" );
_LIT( KMif,          "mif" );
_LIT( KResourceDrive, "Z:" );
_LIT( KResourceFile, "mcspluginres.rsc" );
_LIT( KResPath, "\\resource\\" );
_LIT( KMenuAttrRefcount, "ref_count" );
_LIT( KMMApplication, "mm://" );
_LIT( KHideExit, "?exit=hide" );
_LIT( KHideExit2, "&exit=hide" );
_LIT( KSetFocusString, "!setfocus?applicationgroup_name=" );
_LIT( KApplicationGroupName, "applicationgroup_name" );
_LIT( KSuiteName, "suite_name" );
_LIT( KIcon, "icon" );
_LIT( KMenuAttrUndefUid, "0x99999991" );

const TUid KHomescreenUid = { AI_UID3_AIFW_COMMON };
const TUid KMMUid = { 0x101F4CD2 };

// ======== LOCAL FUNCTIONS ========

static TPtrC NextIdToken( TLex& aLexer )
   {
   aLexer.SkipSpace();
   aLexer.Mark();
   while( !aLexer.Eos() && !aLexer.Peek().IsSpace() && aLexer.Peek() != ')' )
       {
       aLexer.Inc();
       }
   return aLexer.MarkedToken();
   }

// ============================ MEMBER FUNCTIONS ===============================
// ---------------------------------------------------------
// Default constructor
// ---------------------------------------------------------
//
CMCSPluginEngine::CMCSPluginEngine( CMCSPlugin& aPlugin, const TDesC8& aInstanceUid ) 
    : iPlugin( aPlugin ), iInstanceUid( aInstanceUid ),
      iSuspend( EFalse )
    {
    }
    
// ---------------------------------------------------------
// Two-phased constructor.
// Create instance of concrete ECOM interface implementation
// ---------------------------------------------------------
//
CMCSPluginEngine* CMCSPluginEngine::NewL( CMCSPlugin& aPlugin, const TDesC8& aInstanceUid )
    {
    CMCSPluginEngine* self = new( ELeave ) CMCSPluginEngine( aPlugin, aInstanceUid );
    CleanupStack::PushL( self );
    self->ConstructL();
    CleanupStack::Pop( self );

    return self;
    }

// ---------------------------------------------------------
// Symbian 2nd phase constructor can leave
// ---------------------------------------------------------
//
void CMCSPluginEngine::ConstructL()
    {
    TFullName resourceFile( KResourceDrive );
    resourceFile.Append( KResPath );
    resourceFile.Append( KResourceFile );
    BaflUtils::NearestLanguageFile(
        CCoeEnv::Static()->FsSession(),
        resourceFile );
    iResourceOffset =
        CCoeEnv::Static()->AddResourceFileL( resourceFile );
    InitL();
    StartObservingL();
    }
    

// ---------------------------------------------------------
// Destructor.
// ---------------------------------------------------------
//
CMCSPluginEngine::~CMCSPluginEngine()
    {
    StopObserving();
    
    delete iPluginData;
    iNotifier.Close();
    iMenu.Close();
    //iWatcher->Cancel();
    delete iWatcher;
    delete iNotifyWatcher;
    CCoeEnv::Static()->DeleteResourceFile( iResourceOffset );
    }


// ---------------------------------------------------------------------------
// 
// ---------------------------------------------------------------------------
//
void CMCSPluginEngine::InitL()
    {
    iMenu.OpenL( KMyMenuData );
    iPluginData = CMCSPluginData::NewL( *this, iInstanceUid );
    iWatcher = CMCSPluginWatcher::NewL( CMCSPluginWatcher::EOperation );
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
// Tells the settings container to start observing for changes
// ---------------------------------------------------------------------------
//
void CMCSPluginEngine::StartObservingL()
    {
    // registering to bookmarks db. changes observing
    User::LeaveIfError( iBookmarkSession.Connect() );
    User::LeaveIfError( iBookmarkDb.Open( iBookmarkSession, KBrowserBookmarks ) );

    iBookmarkDbObserver = new (ELeave) CActiveFavouritesDbNotifier(
                                        iBookmarkDb, *this );
    iBookmarkDbObserver->Start();

    // registering to mailbox db. changes observing
    iMsvSession = CMsvSession::OpenAsObserverL( *this) ;
    }

// ---------------------------------------------------------------------------
// Tells the settings container to stop observing for changes
// ---------------------------------------------------------------------------
//
void CMCSPluginEngine::StopObserving()
    {
    if ( iBookmarkDbObserver )
        {
        delete iBookmarkDbObserver;
        iBookmarkDbObserver = NULL;
        }
    iBookmarkDb.Close();
    iBookmarkSession.Close();

    if ( iMsvSession )
        {
        delete iMsvSession;
        iMsvSession = NULL;
        }
    }

// ---------------------------------------------------------------------------
// 
// ---------------------------------------------------------------------------
//
TMCSData& CMCSPluginEngine::MenuDataL( const TInt& aIndex )
    {
    return iPluginData->DataItemL( aIndex );
    }

// ---------------------------------------------------------------------------
// 
// ---------------------------------------------------------------------------
//
TInt CMCSPluginEngine::MenuItemCount()
    {
    return iPluginData->DataCount();
    }

// ---------------------------------------------------------------------------
// Returns the menu item header, which matches the given filter.
// ---------------------------------------------------------------------------
//
TMenuItem CMCSPluginEngine::FindMenuItemL( CMenuFilter& aFilter )
    {    
    TMenuItem item;
    const TInt root = iMenu.RootFolderL();
    RArray<TMenuItem> items;
    CleanupClosePushL( items );
    iMenu.GetItemsL( items, root, &aFilter, ETrue );
    if( items.Count() > 0 )
        {

        item = items[0];
        }

    CleanupStack::PopAndDestroy( &items );
    return item;
    }

// ---------------------------------------------------------------------------
// Returns the actual menu item for the given header.
// ---------------------------------------------------------------------------
//
CMenuItem* CMCSPluginEngine::FetchMenuItemL( const TMenuItem& aMenuItem )
    {
    return CMenuItem::OpenL( iMenu, aMenuItem );
    }

// ---------------------------------------------------------------------------
// Returns MCS default icon if attribute is 'icon' else parses the
// skin definition from attribute and sets attributes to aMenuItem.
// ---------------------------------------------------------------------------
//
CGulIcon* CMCSPluginEngine::ItemIconL( CMenuItem& aMenuItem, const TDesC& aAttr )
    {
    CAknIcon* icon( NULL );
    CGulIcon* gIcon( NULL );
    TBool exists( ETrue );
    if( aAttr != KIcon )
        {
        // Resolve secondary icon definition from attribute
        TPtrC iconDef = aMenuItem.GetAttributeL( aAttr, exists );
        if( exists )
            {
            exists = ConstructMenuItemForIconL( iconDef, aMenuItem );
            }
        }
    if( exists )
        {
        icon = MenuIconUtility::GetItemIconL( aMenuItem );
        if( icon )
            {
            CleanupStack::PushL( icon );
            gIcon = CGulIcon::NewL(icon->Bitmap(), icon->Mask());
            // Detach and delete
            icon->SetBitmap( NULL );
            icon->SetMask( NULL );    
            CleanupStack::PopAndDestroy( icon );
            }
        }
    return gIcon;
    }

// ---------------------------------------------------------------------------
// Returns text string for the given attribute
// ---------------------------------------------------------------------------
//
TPtrC CMCSPluginEngine::ItemTextL( CMenuItem& aMenuItem, const TDesC& aAttr  )
    {    
    TBool exists( KErrNotFound );
    TPtrC name = aMenuItem.GetAttributeL( aAttr, exists );
    if( exists )
        {
        return name;
        }
    else
        {
        return KNullDesC();
        }
    }

// ---------------------------------------------------------------------------
// Calls the open command for the given menu item header
// ---------------------------------------------------------------------------
//
void CMCSPluginEngine::LaunchItemL( const TInt& aIndex )
    {

    if ( iSuspend )
        {
        HBufC* temp = StringLoader::LoadLC( R_MCS_DISABLE_OPEN_ITEM );

        CAknNoteDialog* dialog = new (ELeave) CAknNoteDialog(
            CAknNoteDialog::EConfirmationTone,
            CAknNoteDialog::ENoTimeout );
        dialog->SetTextL( temp->Des() );
        dialog->ExecuteDlgLD( R_MCS_DISABLE_OPEN_ITEM_DLG );
        CleanupStack::PopAndDestroy( temp );
        return;
        }
    if( iWatcher->IsActive())
        {
        return;
        }


    TMCSData& dataItem = iPluginData->DataItemL( aIndex );
    CMenuItem* item = CMenuItem::OpenL( iMenu, dataItem.MenuItem());
    CleanupStack::PushL( item );
    TPtrC type = item->Type();

    // run suite/folder
    if ( type == KMenuTypeSuite || type == KMenuTypeFolder )
        {

        // message for MM application
        HBufC8* message; 

        if ( type == KMenuTypeSuite )
            {
            // prepare message for launching suite
            TBool hasSuiteName = EFalse;
            TPtrC suiteName = item->GetAttributeL( KSuiteName, hasSuiteName );
            if ( !hasSuiteName )
                { 
                CleanupStack::PopAndDestroy( item );
                return;
                }
            message = HBufC8::NewLC( KMMApplication().Length() + 
                                     suiteName.Length() + 
                                     KHideExit().Length() );

            message->Des().Copy( KMMApplication );
            message->Des().Append( suiteName );
            message->Des().Append( KHideExit );
            }
        else 
            {
            // prepare message for launching folder
            TBool hasApplicationGroupName = EFalse;
            TPtrC applicationGroupName = item->GetAttributeL( KApplicationGroupName, 
                                                              hasApplicationGroupName );
            if ( !hasApplicationGroupName )
                {
                CleanupStack::PopAndDestroy( item );
                return;
                }
            message = HBufC8::NewLC( KMMApplication().Length() + 
                                     KSetFocusString().Length() +
                                     applicationGroupName.Length() + 
                                     KHideExit2().Length() );

            message->Des().Copy( KMMApplication );
            message->Des().Append( KSetFocusString );
            message->Des().Append( applicationGroupName );
            message->Des().Append( KHideExit2 );
            }

        // find MM application
        TApaTaskList taskList( CCoeEnv::Static()->WsSession() );
        TApaTask task = taskList.FindApp( KMMUid );

        if ( task.Exists() )
            {
            // MM is already running in background - send APA Message
            task.SendMessage( TUid::Uid( KUidApaMessageSwitchOpenFileValue ), *message );
            }
        else
            { 
            // MM not running yet - use Command Line Tail
            RApaLsSession appArcSession;
            CleanupClosePushL( appArcSession );
            User::LeaveIfError( appArcSession.Connect() );
            TApaAppInfo appInfo;
            TInt err = appArcSession.GetAppInfo( appInfo, KMMUid );
            if ( err == KErrNone )
                {
                CApaCommandLine* cmdLine = CApaCommandLine::NewLC();
                cmdLine->SetExecutableNameL( appInfo.iFullName );
                cmdLine->SetCommandL( EApaCommandRun );
                cmdLine->SetTailEndL( *message );
                appArcSession.StartApp( *cmdLine );
                CleanupStack::PopAndDestroy( cmdLine );
                }
            CleanupStack::PopAndDestroy( &appArcSession ); 
            }
        CleanupStack::PopAndDestroy( message );
        }
    else
        {
        TBool exists( EFalse );
        TPtrC desc( item->GetAttributeL( KMenuAttrUid, exists ) );
        
        if( exists )
            {      
            _LIT( KPrefix, "0x" );
            const TInt pos( desc.FindF( KPrefix ) );
            
            if( pos != KErrNotFound )
                {
                TLex lex( desc.Mid( pos + KPrefix().Length() ) );
                // Hex parsing needs unsigned int
                TUint32 value = 0;
                const TInt parseResult = lex.Val( value, EHex );
                
                if ( parseResult == KErrNone )
                    {
                    TUid uid( KNullUid );  
                    TInt32 value32( value );
                    uid.iUid = value32;   
                    
                    if( uid != KNullUid )
                        {
                        //start a full screen effect
                        GfxTransEffect::BeginFullScreen( 
                        AknTransEffect::EApplicationStart,
                        TRect(0,0,0,0), 
                        AknTransEffect::EParameterType, 
                        AknTransEffect::GfxTransParam( uid,
                        AknTransEffect::TParameter::EActivateExplicitContinue ));
                        }
                    }
                }
            }

        // run application/shortcut/bookmark
        CMenuOperation* operation = item->HandleCommandL(
            KMenuCmdOpen, KNullDesC8, iWatcher->iStatus );
        iWatcher->Watch( operation );  
        }
    CleanupStack::PopAndDestroy( item );
    }

// ---------------------------------------------------------------------------
// Handle the change in Menu Content
// ---------------------------------------------------------------------------
//
void CMCSPluginEngine::HandleNotifyL()
    {
    TInt count( iPluginData->DataCount());
    for( TInt i = 0; i < count; i++ )
        {
        TMCSData& data = iPluginData->DataItemL(i);
        // Check that all the data still exist is MCS, if flag is hidden or
        // missing, we have to remove data from UI
        CMenuItem* menuItem = CMenuItem::OpenL( iMenu, data.MenuItem().Id() );
        CleanupStack::PushL( menuItem );
        if ( !menuItem || 
             ( menuItem->Flags() & TMenuItem::EHidden ) ||
             ( menuItem->Flags() & TMenuItem::EMissing ) )
            {
            // Get the replacement for hidden data
            CMenuFilter* filter = CMenuFilter::NewL();
            CleanupStack::PushL( filter );
            // 'Undefined' item
            filter->HaveAttributeL( KMenuAttrUid, KMenuAttrUndefUid );
            TMenuItem undefItem = FindMenuItemL( *filter );
            iPluginData->ReplaceMenuItemL( i, undefItem );
            iPluginData->SaveSettingsL( i, *FetchMenuItemL(undefItem) );
            CleanupStack::PopAndDestroy( filter );
            }
        CleanupStack::PopAndDestroy( menuItem );
        menuItem = NULL;
        }
    // Notification must be activated again
    iNotifyWatcher->Cancel();
    iNotifier.Notify( 0,
                RMenuNotifier::EItemsAddedRemoved |
                RMenuNotifier::EItemsReordered |
                RMenuNotifier::EItemAttributeChanged,
                iNotifyWatcher->iStatus );

    iNotifyWatcher->WatchNotify( this );
    // Publish changed data
    iPlugin.PublishL();
    }

// ---------------------------------------------------------------------------
// From class MMsvSessionObserver.
// Handles an event from the message server.
// ---------------------------------------------------------------------------
//
void CMCSPluginEngine::HandleSessionEventL(
    TMsvSessionEvent aEvent, TAny* /*aArg1*/, TAny* /*aArg2*/, TAny* /*aArg3*/)
    {
    switch ( aEvent )
        {
    case EMsvEntriesDeleted:
        // fall-through intended here
    case EMsvEntriesChanged:
        {

        }
        break;
    default:
        break;
        }
    }

// -----------------------------------------------------------------------------
// From class MFavouritesDbObserver.
// Handles database event.
// -----------------------------------------------------------------------------
//
void CMCSPluginEngine::HandleFavouritesDbEventL( RDbNotifier::TEvent aEvent )
    {
    switch ( aEvent )
        {
        case RDbNotifier::ERollback:
            // fall-through intended here
        case RDbNotifier::ERecover:
            // fall-through intended here
        case RDbNotifier::ECommit:
            {
            // Get list of favourites bookmarks
            CFavouritesItemList* favItems =
                    new (ELeave) CFavouritesItemList();
            CleanupStack::PushL( favItems );
            TInt err = iBookmarkDb.GetAll( *favItems, KFavouritesNullUid,
                    CFavouritesItem::EItem );
            if ( err != KErrNone )
                {
                ASSERT(0);
                }
            TInt count_fav = favItems->Count();

            // Do for each plugin data (4x times)
            TInt count_data( iPluginData->DataCount() );
            for ( TInt item_index = 0; item_index < count_data; item_index++ )
                {
                // Get item ID and open its menu related item
                TMCSData& data = iPluginData->DataItemL( item_index );
                TInt itemID( data.MenuItem().Id() );
                CMenuItem* menuItem = CMenuItem::OpenL( iMenu, itemID );
                CleanupStack::PushL( menuItem );

                // Get URL aatribute
                TBool attrExists = EFalse;
                TPtrC url = menuItem->GetAttributeL( _L("url"), attrExists );
                // If bookmark...
                if ( attrExists )
                    {
                    // Get bookmark UID
                    TPtrC uid_item_ptr = menuItem->GetAttributeL(
                            KMenuAttrUid, attrExists );

                    // Compare with each item in fav. bookmarks list
                    TBool itemExists = EFalse;
                    for ( TInt fav_index = count_fav - 1; fav_index >= 0; fav_index-- ) // newest on top
                        {
                        // Get list item UID 
                        TUid uid_fav = TUid::Uid( favItems->At( fav_index )->Uid() );
                        if ( uid_fav.Name() == uid_item_ptr )
                            {
                            // Bookmark still exist in fav. bookmarks list
                            itemExists = ETrue;
                            break;
                            }
                        }

                    if ( !itemExists )
                        {
                        // If item not axist any more, replace it by undefined icon
                        CMenuFilter* filter = CMenuFilter::NewL();
                        CleanupStack::PushL( filter );
                        // 'Undefined' item
                        filter->HaveAttributeL( KMenuAttrUid,
                                KMenuAttrUndefUid );
                        TMenuItem undefItem = FindMenuItemL( *filter );
                        iPluginData->ReplaceMenuItemL( item_index, undefItem );
                        iPluginData->SaveSettingsL( item_index, *FetchMenuItemL(
                                undefItem ) );
                        CleanupStack::PopAndDestroy( filter );
                        }
                    }
                CleanupStack::PopAndDestroy( menuItem );
                }
            CleanupStack::PopAndDestroy( favItems );
            }
            break;
        default:
            break;
        }
    }

// ---------------------------------------------------------------------------
// Resumes the engine
// ---------------------------------------------------------------------------
//
void CMCSPluginEngine::ResumeL()
    {
    iSuspend = EFalse;
    }

// ---------------------------------------------------------------------------
// Suspends the engine
// ---------------------------------------------------------------------------
//
void CMCSPluginEngine::Suspend()
    {
    iSuspend = ETrue;
    }

// ---------------------------------------------------------------------------
// Launch General Settings plugin
// ---------------------------------------------------------------------------
//
void CMCSPluginEngine::ShowSettingsL()
    { 
    TUid uid = {AI_UID_ECOM_IMPLEMENTATION_SETTINGS_MCSPLUGIN};
    CGSLauncher* l = CGSLauncher::NewLC();
    l->LaunchGSViewL ( uid,
                       KHomescreenUid,
                       iInstanceUid );            
    CleanupStack::PopAndDestroy( l );

    }
// ---------------------------------------------------------------------------
// ResolveSkinItemId
// Syntax: skin(major minor):mif(filename bimapId maskId)
// ---------------------------------------------------------------------------
//
TBool CMCSPluginEngine::ConstructMenuItemForIconL( const TDesC& aPath, CMenuItem& aMenuItem )
   {
   TInt pos = aPath.Locate( ':' );
   if( pos == KErrNotFound )
       {
       pos = aPath.Length();
       }
   TPtrC skin = aPath.Left( pos );
   TInt sf = skin.FindF( KSkin );
   if( sf == KErrNotFound )
       {
       return EFalse;
       }
   TPtrC temp = skin.Mid( sf + KSkin().Length());
   TLex input( temp );
   input.SkipSpace();
   if( !input.Eos() && input.Peek() == '(')
       {
       input.Inc();
       }
   TPtrC majorId = NextIdToken( input );
   TPtrC minorId = NextIdToken( input );
   aMenuItem.SetAttributeL( KMenuAttrIconSkinMajorId, majorId );
   aMenuItem.SetAttributeL( KMenuAttrIconSkinMinorId, minorId );
      
   //TPtrC mif = aPath.Mid( pos + 1 );
   //TInt mf = mif.FindF( KMif );
   if( aPath.Length() > pos && (aPath.Mid( pos + 1 ).FindF( KMif ) != KErrNotFound ))
       {
       TPtrC mif = aPath.Mid( pos + 1 );
       TInt mf = mif.FindF( KMif );
       //TPtrC temp1 = mif.Mid( mf+ KMif().Length());
       TLex input1( mif.Mid( mf+ KMif().Length()) );
       input1.SkipSpace();
       if( !input1.Eos() && input1.Peek() == '(')
          {
          input1.Inc();
          }
       TPtrC file = NextIdToken( input1 );
       TPtrC bitmapId = NextIdToken( input1 );
       TPtrC maskId = NextIdToken( input1 );
       aMenuItem.SetAttributeL( KMenuAttrIconFile, file );
       aMenuItem.SetAttributeL( KMenuAttrIconId, bitmapId );
       aMenuItem.SetAttributeL( KMenuAttrMaskId, maskId );
       }
   return ETrue;
   }

// ---------------------------------------------------------------------------
// Called during plugin desctruction
// Decrements reference counters of all run-time generated items
// and deletes those which have reference counter == 0
// ---------------------------------------------------------------------------
//
void CMCSPluginEngine::CleanMCSItemsL()
    {
    const TInt count( iPluginData->DataCount() );
    for( TInt i = 0; i < count; i++ )
        {
        TMCSData& data = iPluginData->DataItemL(i);
        
        CMenuItem* menuItem = CMenuItem::OpenL( iMenu, data.MenuItem().Id() );        
        if( !menuItem )
            {
            continue;
            }        
        CleanupStack::PushL( menuItem );
        
        // check if ref_count attribute exists
        TBool exists = EFalse;
        TPtrC param = menuItem->GetAttributeL( KMenuAttrRefcount, exists );
        if( exists )
            {                
            const TInt references = UpdateMenuItemsRefCountL( menuItem, -1 );
            
            // Create a nested loop inside CActiveScheduler.
            CActiveSchedulerWait* wait = new (ELeave) CActiveSchedulerWait;
            CleanupStack::PushL( wait );
            
            if( references > 0 )
                {
                // if counter is still > 0, update its value in MCS 
                CMenuOperation* op = menuItem->SaveL( iWatcher->iStatus );
                iWatcher->StopAndWatch( op, wait );
                
                // Start the nested scheduler loop.
                wait->Start();
                }
            else
                {                     
                 // counter reached 0 -> item is not referenced by any shortcut
                 // so remove it from MCS
                 if( !iWatcher->IsActive() )
                     {
                     CMenuOperation* op = iMenu.RemoveL( menuItem->Id(), iWatcher->iStatus );
                     iWatcher->StopAndWatch( op, wait );
                     
                      // Start the nested scheduler loop.
                     wait->Start();
                     }
                }
            
            CleanupStack::PopAndDestroy( wait );
            wait = NULL;
            }
        
        CleanupStack::PopAndDestroy( menuItem );
        menuItem = NULL;
        }
    }


// ---------------------------------------------------------------------------
// Helper method. Adds a given constant to a value of reference counter  
// ---------------------------------------------------------------------------
//
TInt CMCSPluginEngine::UpdateMenuItemsRefCountL( CMenuItem* aItem, 
                                                 const TInt aValueToAdd )
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
        aItem->SetAttributeL( KMenuAttrRefcount, buf);
        CleanupStack::Pop( aItem );
        // return new ref_count
        return references;
        }
    return -1;
    }
