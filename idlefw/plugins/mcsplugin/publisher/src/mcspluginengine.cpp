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


// System includes
#include <gulicon.h>
#include <AknsItemID.h>
#include <gslauncher.h>
#include <mcsmenuitem.h>
#include <mcsmenufilter.h>
#include <mcsmenuoperation.h>
#include <mcsmenuiconutility.h>
#include <activefavouritesdbnotifier.h>
#include <favouritesitemlist.h>
#include <bautils.h>
#include <StringLoader.h>
#include <aknnotedialog.h>
#include <AknsConstants.h>
#include <avkon.rsg>
#include <mcspluginres.rsg>
#include <apgtask.h>
#include <apgcli.h> 
#include <apacmdln.h>
#include <gfxtranseffect/gfxtranseffect.h>      
#include <akntranseffect.h>

// User includes
#include <aisystemuids.hrh>
#include "mcspluginengine.h"
#include "mcsplugin.h"
#include "mcsplugindata.h"
#include "mcspluginuids.hrh"

// Constants
_LIT( KMyMenuData, "matrixmenudata" );
_LIT( KSkin,         "skin" );
_LIT( KMif,          "mif" );
_LIT( KResourceDrive, "Z:" );
_LIT( KResourceFile, "mcspluginres.rsc" );
_LIT( KResPath, "\\resource\\" );
_LIT( KMenuAttrRefcount, "ref_count" );
_LIT( KMMApplication, "mm://" );
_LIT( KHideExit2, "&exit=hide" );
_LIT( KSetFocusString, "!setfocus?applicationgroup_name=" );
_LIT( KApplicationGroupName, "applicationgroup_name" );
_LIT( KIcon, "icon" );
_LIT( KMenuAttrUndefUid, "0x99999991" );

const TUid KHomescreenUid = { AI_UID3_AIFW_COMMON };
const TUid KMMUid = { 0x101F4CD2 };

// ======== LOCAL FUNCTIONS ========
// ----------------------------------------------------------------------------
// NextIdToken
//
// ----------------------------------------------------------------------------
//
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
// ----------------------------------------------------------------------------
// CMCSPluginEngine::CMCSPluginEngine
//
// ----------------------------------------------------------------------------
//
CMCSPluginEngine::CMCSPluginEngine( CMCSPlugin& aPlugin, 
    const TDesC8& aInstanceUid ) 
    : iPlugin( aPlugin ), iInstanceUid( aInstanceUid )    
    {
    }
    
// ----------------------------------------------------------------------------
// CMCSPluginEngine::NewL
// 
// ----------------------------------------------------------------------------
//
CMCSPluginEngine* CMCSPluginEngine::NewL( CMCSPlugin& aPlugin, 
    const TDesC8& aInstanceUid )
    {
    CMCSPluginEngine* self = 
        new( ELeave ) CMCSPluginEngine( aPlugin, aInstanceUid );
    
    CleanupStack::PushL( self );
    self->ConstructL();
    CleanupStack::Pop( self );

    return self;
    }

// ----------------------------------------------------------------------------
// CMCSPluginEngine::ConstructL
//
// ----------------------------------------------------------------------------
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
    
    // Get "Undefined" item
    CMenuFilter* filter = CMenuFilter::NewL();
    CleanupStack::PushL( filter );
    filter->HaveAttributeL( KMenuAttrUid, KMenuAttrUndefUid );
    iUndefinedItemHeader = FindMenuItemL( *filter );
    CleanupStack::PopAndDestroy( filter );
    filter = NULL;
    iUndefinedItem = CMenuItem::OpenL( iMenu, iUndefinedItemHeader );    
    }

// ----------------------------------------------------------------------------
// CMCSPluginEngine::~CMCSPluginEngine
//
// ----------------------------------------------------------------------------
//
CMCSPluginEngine::~CMCSPluginEngine()
    {
    StopObserving();

    delete iPluginData;
    
    iMenu.Close();
    //iWatcher->Cancel();
    delete iWatcher;
    

    CCoeEnv::Static()->DeleteResourceFile( iResourceOffset );

    if ( iUndefinedItem )
        {
        delete iUndefinedItem;
        iUndefinedItem = NULL;
        }
    }

// ---------------------------------------------------------------------------
// CMCSPluginEngine::InitL
//
// ---------------------------------------------------------------------------
//
void CMCSPluginEngine::InitL()
    {
    iMenu.OpenL( KMyMenuData );
    iPluginData = CMCSPluginData::NewL( *this, iInstanceUid );
    iWatcher = CMCSPluginWatcher::NewL( CMCSPluginWatcher::EOperation );
    
    TInt err( iNotifier.Open( iMenu ) ); 
    
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
// CMCSPluginEngine::StartObservingL
//
// ---------------------------------------------------------------------------
//
void CMCSPluginEngine::StartObservingL()
    {
    // registering to mailbox db. changes observing
    iMsvSession = CMsvSession::OpenAsObserverL( *this );
    }

// ---------------------------------------------------------------------------
// CMCSPluginEngine::StopObserving
//
// ---------------------------------------------------------------------------
//
void CMCSPluginEngine::StopObserving()
    {       
    delete iMsvSession;
    iMsvSession = NULL;    
    }

// ---------------------------------------------------------------------------
// CMCSPluginEngine::MenuDataL
//
// ---------------------------------------------------------------------------
//
TMCSData& CMCSPluginEngine::MenuDataL( const TInt& aIndex )
    {
    return iPluginData->DataItemL( aIndex );
    }

// ---------------------------------------------------------------------------
// CMCSPluginEngine::MenuItemCount
//
// ---------------------------------------------------------------------------
//
TInt CMCSPluginEngine::MenuItemCount()
    {
    return iPluginData->DataCount();
    }

// ---------------------------------------------------------------------------
// CMCSPluginEngine::FindMenuItemL
// Returns the menu item header, which matches the given filter.
// ---------------------------------------------------------------------------
//
TMenuItem CMCSPluginEngine::FindMenuItemL( CMenuFilter& aFilter )
    {    
    TMenuItem item;
    const TInt root( iMenu.RootFolderL() );
    
    RArray<TMenuItem> items;
    CleanupClosePushL( items );
    
    iMenu.GetItemsL( items, root, &aFilter, ETrue );
    
    if ( items.Count() > 0 )
        {
        item = items[0];
        }

    CleanupStack::PopAndDestroy( &items );
    
    return item;
    }

// ---------------------------------------------------------------------------
// CMCSPluginEngine::FetchMenuItemL
// Returns the actual menu item for the given header.
// ---------------------------------------------------------------------------
//
CMenuItem* CMCSPluginEngine::FetchMenuItemL( const TMenuItem& aMenuItem )
    {
    return CMenuItem::OpenL( iMenu, aMenuItem );
    }

// ---------------------------------------------------------------------------
// CMCSPluginEngine::ItemIconL
// Returns MCS default icon if attribute is 'icon' else parses the
// skin definition from attribute and sets attributes to aMenuItem.
// ---------------------------------------------------------------------------
//
CGulIcon* CMCSPluginEngine::ItemIconL( CMenuItem& aMenuItem, 
    const TDesC& aAttr )
    {
    CMenuItem* menuItem;

    TInt id = aMenuItem.Id();

    // because the flags might have changed, we have
    // to get a fresh copy of menu item from Menu Server
    CMenuItem* mi = CMenuItem::OpenL( iMenu, id );
    TUint32 flags = mi->Flags();
    delete mi;

    TUint32 isHidden = flags & TMenuItem::EHidden;
    TUint32 isMissing = flags & TMenuItem::EMissing;

    if ( iUndefinedItem && ( isHidden || isMissing ) )
        {
        menuItem = iUndefinedItem;
        }
    else
        {
        menuItem = &aMenuItem;
        }

    CAknIcon* icon( NULL );
    CGulIcon* gIcon( NULL );
    TBool exists( ETrue );
    
    if ( aAttr != KIcon )
        {
        // Resolve secondary icon definition from attribute
        TPtrC iconDef( menuItem->GetAttributeL( aAttr, exists ) );
    
        if ( exists )
            {
            exists = ConstructMenuItemForIconL( iconDef, *menuItem );
            }
        }
    
    if ( exists )
        {
        icon = MenuIconUtility::GetItemIconL( *menuItem );
    
        if ( icon )
            {
            CleanupStack::PushL( icon );
            
            gIcon = CGulIcon::NewL( icon->Bitmap(), icon->Mask() );
            
            // Detach and delete
            icon->SetBitmap( NULL );
            icon->SetMask( NULL );    
            
            CleanupStack::PopAndDestroy( icon );
            }
        }
    
    return gIcon;
    }

// ---------------------------------------------------------------------------
// CMCSPluginEngine::ItemTextL
// Returns text string for the given attribute
// ---------------------------------------------------------------------------
//
TPtrC CMCSPluginEngine::ItemTextL( CMenuItem& aMenuItem, const TDesC& aAttr )
    {    
    // if item is hidden or missing (mmc card removed)
    // use "Undefined" text instead
    CMenuItem* menuItem;

    TInt id = aMenuItem.Id();

    // because the flags might have changed, we have
    // to get a fresh copy of the menu item from Menu Server
    CMenuItem* mi = CMenuItem::OpenL( iMenu, id );
    TUint32 flags = mi->Flags();
    delete mi;

    TUint32 isHidden = flags & TMenuItem::EHidden;
    TUint32 isMissing = flags & TMenuItem::EMissing;

    if ( iUndefinedItem && ( isHidden || isMissing ) )
        {
        menuItem = iUndefinedItem;
        }
    else
        {
        menuItem = &aMenuItem;
        }
        
    TBool exists( KErrNotFound );
    
    TPtrC name( menuItem->GetAttributeL( aAttr, exists ) );
    
    if ( exists )
        {
        return name;
        }

    return KNullDesC();
    }

// ---------------------------------------------------------------------------
// CMCSPluginEngine::LaunchItemL
// Calls the open command for the given menu item header
// ---------------------------------------------------------------------------
//
void CMCSPluginEngine::LaunchItemL( const TInt& aIndex )
    {
    if ( iBackupRestore )
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

    TMCSData& dataItem( iPluginData->DataItemL( aIndex ) );
    
    CMenuItem* item( CMenuItem::OpenL( iMenu, dataItem.MenuItem().Id() ) );
    CleanupStack::PushL( item );
    

    TBool attrExists = ETrue;

    TPtrC uid = item->GetAttributeL( KMenuAttrUid, attrExists );

    // trying to run non-existing application ( that was replaced by "Undefined" app )
    // OR trying to run hidden or missing application (e.g. unistalled app 
    // or app on MMC which was removed )
    // -> We display a note for a user that this is not possible¨
    TUint32 isHidden = item->Flags() & TMenuItem::EHidden;
    TUint32 isMissing = item->Flags() & TMenuItem::EMissing;

    if ( ( attrExists && uid == KMenuAttrUndefUid ) || isHidden || isMissing )
        {
        CleanupStack::PopAndDestroy( item );

        HBufC* temp = StringLoader::LoadLC( R_MCS_DISABLE_OPEN_ITEM_MISSING );

        CAknNoteDialog* dialog = new (ELeave) CAknNoteDialog(
            CAknNoteDialog::EConfirmationTone,
            CAknNoteDialog::ENoTimeout );
        dialog->SetTextL( temp->Des() );
        dialog->ExecuteDlgLD( R_MCS_DISABLE_OPEN_ITEM_DLG );
        CleanupStack::PopAndDestroy( temp );
        temp = NULL;

        return;
        }

    // run item based on its type
    TPtrC type( item->Type() );

    // run folder
    if ( type == KMenuTypeFolder )
        {
        // message for MM application
        HBufC8* message; 

        // prepare message for launching folder
        TBool hasApplicationGroupName( EFalse );
        
        TPtrC applicationGroupName( item->GetAttributeL(  
            KApplicationGroupName, hasApplicationGroupName ) );
                                                          
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

        // find MM application
        TApaTaskList taskList( CCoeEnv::Static()->WsSession() );
        TApaTask task( taskList.FindApp( KMMUid ) );

        if ( task.Exists() )
            {
            // MM is already running in background - send APA Message
            task.SendMessage( 
                TUid::Uid( KUidApaMessageSwitchOpenFileValue ), *message );
            }
        else
            { 
            // MM not running yet - use Command Line Tail
            RApaLsSession appArcSession;
            CleanupClosePushL( appArcSession );
            
            User::LeaveIfError( appArcSession.Connect() );
            
            TApaAppInfo appInfo;
            TInt err( appArcSession.GetAppInfo( appInfo, KMMUid ) );
            
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
        
        if ( exists )
            {      
            _LIT( KPrefix, "0x" );
            
            const TInt pos( desc.FindF( KPrefix ) );
            
            if ( pos != KErrNotFound )
                {
                TLex lex( desc.Mid( pos + KPrefix().Length() ) );
                
                // Hex parsing needs unsigned int
                TUint32 value( 0 );
                const TInt parseResult( lex.Val( value, EHex ) );
                
                if ( parseResult == KErrNone )
                    {
                    TUid uid( KNullUid );  
                    TInt32 value32( value );
                    uid.iUid = value32;   
                    
                    if ( uid != KNullUid )
                        {
                        //start a full screen effect
                        GfxTransEffect::BeginFullScreen( 
                            AknTransEffect::EApplicationStart,
                            TRect(), 
                            AknTransEffect::EParameterType, 
                            AknTransEffect::GfxTransParam( uid,
                            AknTransEffect::TParameter::EActivateExplicitContinue ) );
                        }
                    }
                }
            }

        // run application/shortcut/bookmark
        CMenuOperation* operation( item->HandleCommandL(
            KMenuCmdOpen, KNullDesC8, iWatcher->iStatus ) );
        
        iWatcher->Watch( operation );  
        }
    
    CleanupStack::PopAndDestroy( item );
    }

// ---------------------------------------------------------------------------
// CMCSPluginEngine::HandleNotifyL
// Handle the change in Menu Content
// ---------------------------------------------------------------------------
//
void CMCSPluginEngine::HandleNotifyL()
    {
    TInt count( iPluginData->DataCount() );
    
    for ( TInt i = 0; i < count; i++ )
        {
        TMCSData& data( iPluginData->DataItemL(i) );
        data.SetDirty( ETrue );
        
        // Check that all the data still exist is MCS, if flag is hidden or
        // missing, we have to remove data from UI
        CMenuItem* menuItem = NULL; 

        TRAP_IGNORE (
                menuItem = CMenuItem::OpenL( iMenu, data.MenuItem().Id() );
        )

        // item not found. Use "Undefined" item as a replacement
        if ( !menuItem )
            {
            CleanupStack::PushL( menuItem );
            iPluginData->ReplaceMenuItemL( i, iUndefinedItemHeader );
            iPluginData->SaveSettingsL( i, *iUndefinedItem );
            CleanupStack::Pop( menuItem );
            }

        delete menuItem;
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
// CMCSPluginEngine::HandleSessionEventL
// Handles an event from the message server.
// ---------------------------------------------------------------------------
//
void CMCSPluginEngine::HandleSessionEventL( TMsvSessionEvent aEvent, 
    TAny* /*aArg1*/, TAny* /*aArg2*/, TAny* /*aArg3*/)
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

// ---------------------------------------------------------------------------
// CMCSPluginEngine::SetBackupRestore
//
// ---------------------------------------------------------------------------
//
void CMCSPluginEngine::SetBackupRestore( TBool aBackupRestore )
    {
    iBackupRestore = aBackupRestore;
    }

// ---------------------------------------------------------------------------
// CMCSPluginEngine::ShowSettingsL
// Launch General Settings plugin
// ---------------------------------------------------------------------------
//
void CMCSPluginEngine::ShowSettingsL()
    { 
    TUid uid = {AI_UID_ECOM_IMPLEMENTATION_SETTINGS_MCSPLUGIN};
    
    CGSLauncher* launcher = CGSLauncher::NewLC();
    
    launcher->LaunchGSViewL ( uid, KHomescreenUid, iInstanceUid );
                                                        
    CleanupStack::PopAndDestroy( launcher );
    }

// ---------------------------------------------------------------------------
// CMCSPluginEngine::ConstructMenuItemForIconL
// Syntax: skin(major minor):mif(filename bimapId maskId)
// ---------------------------------------------------------------------------
//
TBool CMCSPluginEngine::ConstructMenuItemForIconL( const TDesC& aPath, 
    CMenuItem& aMenuItem )
   {
   TInt pos( aPath.Locate( ':' ) );
   
   if ( pos == KErrNotFound )
       {
       pos = aPath.Length();
       }
   
   TPtrC skin( aPath.Left( pos ) );
   TInt sf( skin.FindF( KSkin ) );
   
   if ( sf == KErrNotFound )
       {
       return EFalse;
       }
   
   TPtrC temp( skin.Mid( sf + KSkin().Length() ) );
   
   TLex input( temp );   
   input.SkipSpace();
   
   if ( !input.Eos() && input.Peek() == '(' )
       {
       input.Inc();
       }
   
   TPtrC majorId( NextIdToken( input ) );
   TPtrC minorId( NextIdToken( input ) );
   
   aMenuItem.SetAttributeL( KMenuAttrIconSkinMajorId, majorId );
   aMenuItem.SetAttributeL( KMenuAttrIconSkinMinorId, minorId );
      
   //TPtrC mif = aPath.Mid( pos + 1 );
   //TInt mf = mif.FindF( KMif );
   
   if ( aPath.Length() > pos && 
      ( aPath.Mid( pos + 1 ).FindF( KMif ) != KErrNotFound ) )
       {
       TPtrC mif( aPath.Mid( pos + 1 ) );
       TInt mf( mif.FindF( KMif ) );
       
       //TPtrC temp1 = mif.Mid( mf+ KMif().Length());
       
       TLex input1( mif.Mid( mf + KMif().Length() ) );
       input1.SkipSpace();
       
       if ( !input1.Eos() && input1.Peek() == '(' )
          {
          input1.Inc();
          }
       
       TPtrC file( NextIdToken( input1 ) );
       TPtrC bitmapId( NextIdToken( input1 ) );
       TPtrC maskId( NextIdToken( input1 ) );
       
       aMenuItem.SetAttributeL( KMenuAttrIconFile, file );
       aMenuItem.SetAttributeL( KMenuAttrIconId, bitmapId );
       aMenuItem.SetAttributeL( KMenuAttrMaskId, maskId );
       }
   
   return ETrue;
   }

// ---------------------------------------------------------------------------
// CMCSPluginEngine::CleanMCSItemsL
// Called during plugin desctruction
// Decrements reference counters of all run-time generated items
// and deletes those which have reference counter == 0
// ---------------------------------------------------------------------------
//
void CMCSPluginEngine::CleanMCSItemsL()
    {
    iNotifier.Close();
    delete iNotifyWatcher;
    iNotifyWatcher = NULL;

    const TInt count( iPluginData->DataCount() );
    
    for( TInt i = 0; i < count; i++ )
        {
        TMCSData& data( iPluginData->DataItemL(i) );
        
        CMenuItem* menuItem = CMenuItem::OpenL( iMenu, data.MenuItem().Id() );        
        
        if ( !menuItem )
            {
            continue;
            }
        
        CleanupStack::PushL( menuItem );
        
        // check if ref_count attribute exists
        TBool exists( EFalse );
        
        TPtrC param( menuItem->GetAttributeL( KMenuAttrRefcount, exists ) );
        
        if( exists )
            {                
            const TInt references( UpdateMenuItemsRefCountL( menuItem, -1 ) );
            
            // Create a nested loop inside CActiveScheduler.
            CActiveSchedulerWait* wait = 
                new ( ELeave ) CActiveSchedulerWait;
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
                     CMenuOperation* op = 
                         iMenu.RemoveL( menuItem->Id(), iWatcher->iStatus );
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
// CMCSPluginEngine::UpdateMenuItemsRefCountL
// Adds a given constant to a value of reference counter  
// ---------------------------------------------------------------------------
//
TInt CMCSPluginEngine::UpdateMenuItemsRefCountL( CMenuItem* aItem, 
    const TInt aValueToAdd )
    {
    TBool exists( EFalse );
    CleanupStack::PushL( aItem ); 
    TPtrC param( aItem->GetAttributeL( KMenuAttrRefcount, exists ) );
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

// ---------------------------------------------------------------------------
// Creates bookmark menu item if it does not exist
// ---------------------------------------------------------------------------
//
void CMCSPluginEngine::CreateBkmMenuItemsL()
    {
    iPluginData->CreateBkmMenuItemsL();
    }

// End of file

