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
* Description:  MCS plugin publisher
*
*/


#include <ecom/ecom.h>
#include <ecom/implementationproxy.h>
#include <gulicon.h>
#include <aicontentobserver.h>
#include <aiutility.h>
#include <aistrcnv.h>
#include <mcsmenuitem.h>

#include <aknskinsinternalcrkeys.h> // For working with settings API
#include <centralrepository.h> // Headers Used for CRepository

#include "hs_app_mcspluginuids.hrh"
#include "hs_app_mcsplugin.h"
#include "hs_app_mcsplugindata.h"
#include "hs_app_mcspluginengine.h"
#include "aipluginsettings.h"


const TUint KPluginNameSeprator =  '/';

const TInt KImplUidMCSPlugin = AI_UID_ECOM_IMPLEMENTATION_CONTENTPUBLISHER_MCSPLUGIN;
// CONST CLASS VARIABLES
const TImplementationProxy KImplementationTable[] =
    {
    IMPLEMENTATION_PROXY_ENTRY( KImplUidMCSPlugin, CMCSPlugin::NewL ) 
    };

_LIT( KEventNameLaunchByIndex,  "LaunchByIndex" );
_LIT( KEventNameShowSettings,   "ShowSettings" );
_LIT( KContentItemTypeText, "text" );
_LIT( KContentItemTypeImage, "image" );

// ======== LOCAL FUNCTIONS ========

// ======== MEMBER FUNCTIONS ========

// ---------------------------------------------------------------------------
// Symbian 2nd phase constructor can leave
// ---------------------------------------------------------------------------
//
CMCSPlugin* CMCSPlugin::NewL()
    {
    CMCSPlugin* self = new (ELeave) CMCSPlugin;
    CleanupStack::PushL( self );
    self->ConstructL();
    CleanupStack::Pop( self );
 
    return self;
    }
    
// ---------------------------------------------------------------------------
// Default constructor
// ---------------------------------------------------------------------------
//
CMCSPlugin::CMCSPlugin()
    {
    }
    
// ---------------------------------------------------------------------------
// Symbian 2nd phase constructor can leave
// ---------------------------------------------------------------------------
//
void CMCSPlugin::ConstructL()
    { 
    iInfo.iUid.iUid = AI_UID_ECOM_IMPLEMENTATION_CONTENTPUBLISHER_MCSPLUGIN; 
    // We need to Query Central Repository
    iRepository  = CRepository::NewL( KCRUidPersonalisation );

    // Setting up watcher which calls HandleNotifyL method 
    // everytime the SkinUID changes in central repository
    iRepositoryWatcher = CMCSPluginWatcher::NewL( CMCSPluginWatcher::ENotify );
    iRepository->NotifyRequest( KPslnActiveSkinUid, iRepositoryWatcher->iStatus );
    iRepositoryWatcher->WatchNotify( this );
    }

// ---------------------------------------------------------------------------
// Handle Skin UID change
// ---------------------------------------------------------------------------
//
void CMCSPlugin::HandleNotifyL()
    {

    // Skin ID has changed. Set all MenuItems on Widget dirty
    // and re-publish to update icons
    if ( iEngine )
        {
        TInt dataCount = iEngine->MenuItemCount();
        for ( TInt i = 0; i < dataCount; i++ )
            {
            iEngine->MenuDataL( i ).SetDirty( ETrue );
            }
        PublishL();
        }

    // Skin ID Notification must be activated again
    iRepositoryWatcher->Cancel();
    iRepository->NotifyRequest( KPslnActiveSkinUid, iRepositoryWatcher->iStatus );
    iRepositoryWatcher->WatchNotify( this );
    }
    
// ---------------------------------------------------------------------------
// Destructor
// Deletes all data created to heap
// ---------------------------------------------------------------------------
//
CMCSPlugin::~CMCSPlugin()
    {
    Release( iContent );
    
    if ( iEngine )
        {
        TRAP_IGNORE( iEngine->CleanMCSItemsL() );
        }
    
    delete iEngine;
    iObservers.Close();
    
    DeleteContentModel();

    if ( iRepository )
        {
        delete iRepository;
        iRepository = NULL;
        }

    if ( iRepositoryWatcher )
        {
        iRepositoryWatcher->Cancel();
        delete iRepositoryWatcher;
        iRepositoryWatcher = NULL;
        }
    }

// ---------------------------------------------------------------------------
// Publishes the all the items
// ---------------------------------------------------------------------------
//
void CMCSPlugin::PublishL()
    {
    
    if ( !iEngine )
        {
        iEngine = CMCSPluginEngine::NewL( *this, iInfo.iNamespace );
        }

    TInt err( KErrNone );
    TInt observers( iObservers.Count() );
    TInt transactionId = reinterpret_cast<TInt>( this );
    TInt menuItems ( iEngine->MenuItemCount() );

    for ( TInt i = 0; i < observers; i++ )
        {
        MAiContentObserver* observer = iObservers[ i ];
        err = observer->StartTransaction( transactionId );
        if ( err == KErrNotSupported )
            {
            return;
            }
        // Publish content to all items
        for ( TInt j = 0; j < menuItems; j++ )
            {
            // Index has to start from 1 ( j + 1 )
            PublishLItemL( *observer, iEngine->MenuDataL( j ), ( j + 1 ) );
            }// shortcut count
        if ( err == KErrNone )
            {
            err = observer->Commit( transactionId );
            if ( err == KErrNotSupported )
                {
                return;
                }
            }
        }//observers

    // Set all items not dirty.
    for ( TInt j = 0; j < menuItems; j++ )
        {
        iEngine->MenuDataL( j ).SetDirty( EFalse );
        }
    }

// ---------------------------------------------------------------------------
// Publishes one item to given index
// ---------------------------------------------------------------------------
//
void CMCSPlugin::PublishLItemL( MAiContentObserver& aObserver, TMCSData& aData, TInt aIndex )
    {

    if( !aData.IsDirty() )
        {
        return;
        }

    CMenuItem* item = iEngine->FetchMenuItemL( aData.MenuItem() );
    CleanupStack::PushL( item );
    // One widget item has iDataCount number of elements
    for ( TInt i = 0; i < iDataCount; i++ )
        {
        if ( iContentModel[i].type == KAiContentTypeBitmap )
            {
            //Publish image
            if ( aObserver.CanPublish( *this, i, aIndex ) )
                {
                CGulIcon* icon = iEngine->ItemIconL( *item, TPtrC16( ( const TText16* ) iContentModel[ i ].cid ) );
                aObserver.PublishPtr( *this, i, icon , aIndex );
                }
            }
        else if ( iContentModel[ i ].type == KAiContentTypeText )
            {
            //Publish  text
            if ( aObserver.CanPublish( *this, i, aIndex ) )
                {
                TPtrC name = iEngine->ItemTextL( *item, TPtrC16( ( const TText16* ) iContentModel[ i ].cid ) );
                aObserver.Publish( *this, i, name, aIndex );
                }
            }
        }//content items

    CleanupStack::PopAndDestroy( item );

    }

// ---------------------------------------------------------------------------
// From class CAiContentPublisher
// Plug-in is requested to unload its engines due backup operation
// ---------------------------------------------------------------------------
//
void CMCSPlugin::Stop( TAiTransitionReason aReason )
    {
    
    if ( aReason == EAiBackupRestoreStarted )
        {
        Suspend( aReason );
        }

    }

// ---------------------------------------------------------------------------
// From class CAiContentPublisher
// Plug-in is instructed that it is allowed to consume CPU resources
// ---------------------------------------------------------------------------
//
void CMCSPlugin::Resume( TAiTransitionReason aReason )
    {
    if( aReason == EAiIdleBackground )
        {
        return;
        }

    if ( aReason == EAiBackupRestoreEnded )
        {
        if ( iEngine )
            {
            TRAP_IGNORE( iEngine->ResumeL() );
            }
        }

    TRAP_IGNORE( DoResumeL( aReason ) );
    return;
    }

// ---------------------------------------------------------------------------
// From class CAiContentPublisher
// Plug-in is instructed that it is not allowed to consume CPU resources
// ---------------------------------------------------------------------------
//
void CMCSPlugin::Suspend( TAiTransitionReason aReason )
    {
    if ( aReason == EAiBackupRestoreStarted && iEngine )
        {
        iEngine->Suspend();
        }
    }

// ---------------------------------------------------------------------------
// From class CAiContentPublisher
// The plug-in MUST maintain a registry of subscribers and send 
// notification to all of them whenever the state changes or new content
// is available
// ---------------------------------------------------------------------------
//
void CMCSPlugin::SubscribeL( MAiContentObserver& aObserver )
    { 
    iObservers.AppendL( &aObserver );
    }
   
// ---------------------------------------------------------------------------
// Compare method to exclude the similar content items from array.
// ---------------------------------------------------------------------------
//
TInt CMCSPlugin::CompareItems( const MAiPluginSettings& aFirst,
        const MAiPluginSettings& aSecond )
    {
    MAiPluginSettings& first = const_cast<MAiPluginSettings&>(aFirst);
    MAiPluginSettings& second = const_cast<MAiPluginSettings&>(aSecond);
    return first.AiPluginContentItem().Name().CompareC(second.AiPluginContentItem().Name());    
    }

// ---------------------------------------------------------------------------
// From class CAiContentPublisher
// Plug-ins take ownership of the settings array, so it must either
// store it in a member or free it.
// Creates dynamic content model.
// ---------------------------------------------------------------------------
//
void CMCSPlugin::ConfigureL( RAiSettingsItemArray& aSettings )
    {
    
    TLinearOrder<MAiPluginSettings> sortMethod( CMCSPlugin::CompareItems );
    RAiSettingsItemArray contentItemsArr;

    TInt count = aSettings.Count();
    for ( TInt i = 0; i < count; i++ )
       {
       MAiPluginSettings* pluginSetting = aSettings[ i ];
       if( pluginSetting->AiPluginItemType() == EAiPluginContentItem )
           {
           MAiPluginContentItem& contItem = pluginSetting->AiPluginContentItem();
           TPtrC name = contItem.Name();
           TPtrC type = contItem.Type();
           contentItemsArr.InsertInOrder( pluginSetting, sortMethod );
           }

       }
    iDataCount = contentItemsArr.Count();
    if ( iDataCount > 0 )
        {
        // Create the dynamic content Model
        DeleteContentModel();
        iContentModel = new ( ELeave ) TAiContentItem[ iDataCount ];
        for ( TInt i = 0; i < iDataCount; i++ )
            {
            iContentModel[i].id = i;
            MAiPluginContentItem& contentItem = ( contentItemsArr[ i ] )->AiPluginContentItem();

            if( contentItem.Type() == KContentItemTypeText )
                {
                // text
                iContentModel[i].type = KAiContentTypeText;
                }
            if( contentItem.Type() == KContentItemTypeImage )
                {
                // image
                iContentModel[i].type = KAiContentTypeBitmap;
                }
            TInt pos = contentItem.Name().Locate( KPluginNameSeprator );
            
            HBufC* contentId = HBufC::NewL( contentItem.Name().Length() );
            CleanupStack::PushL( contentId );
            TPtr ptr = contentId->Des();
            ptr = contentItem.Name().Mid( pos + 1 );
            TInt sizeOfContentId = ptr.Size() +sizeof( wchar_t );
            iContentModel[i].cid = static_cast<const wchar_t*>( User::AllocL( sizeOfContentId ) );
            Mem::Copy((TAny*)iContentModel[i].cid, ptr.PtrZ(), sizeOfContentId);
            CleanupStack::PopAndDestroy( contentId );
            }
        iContent = AiUtility::CreateContentItemArrayIteratorL( iContentModel, iDataCount );
        }
    contentItemsArr.Reset();
    // We own the array so destroy it
    aSettings.ResetAndDestroy();
    }

// ---------------------------------------------------------------------------
// From class CAiContentPublisher
// Returns the extension interface. Actual type depends on the passed 
// aUid argument.
// ---------------------------------------------------------------------------
//
TAny* CMCSPlugin::Extension( TUid aUid )
    {    
    if (aUid == KExtensionUidProperty)
           {
        return static_cast<MAiPropertyExtension*>(this);
        }
    else if (aUid == KExtensionUidEventHandler)
        {
        return static_cast<MAiEventHandlerExtension*>(this);
        }
    else
        {    
        return NULL;
        }
    }

// ---------------------------------------------------------------------------
// From class MAiPropertyExtension
// Read property of publisher plug-in.
// ---------------------------------------------------------------------------
//
TAny* CMCSPlugin::GetPropertyL( TInt aProperty )
    {
    TAny* property = NULL;
    
    switch ( aProperty )
        {
        case EAiPublisherInfo:
            {
             property = static_cast<TAiPublisherInfo*>( &iInfo );
            break;  
            }       
    
        case EAiPublisherContent:
            {
            property = static_cast<MAiContentItemIterator*>( iContent );
            break;    
            }
        }
    return property;
    }

// ---------------------------------------------------------------------------
// From class MAiPropertyExtension
// Write property value to optimize the content model.
// ---------------------------------------------------------------------------
//
void CMCSPlugin::SetPropertyL( TInt aProperty, TAny* aValue )
    {  
    switch ( aProperty )
        {
		case EAiPublisherInfo:
			{
			if( aValue )
			    {
    		    const TAiPublisherInfo* info = static_cast<const TAiPublisherInfo*>( aValue );
	    	    iInfo.iName.Copy(info->iName);
	    	    iInfo.iNamespace.Copy(info->iNamespace);
			    }
		    break;
		    }
		default:
		    break;         
        }
    }
 
// ---------------------------------------------------------------------------
// From class MAiEventHandlerExtension.
// Handles an event sent by the AI framework.
// ---------------------------------------------------------------------------
//
void CMCSPlugin::HandleEvent( TInt /*aEvent*/, const TDesC& /*aParam*/ )
    {
    // We have no way of reporting errors to framework so just ignore them.
    //TRAP_IGNORE( iEngine->HandleEventL(aEvent, aParam ));
    } 

// ---------------------------------------------------------------------------
// From class MAiEventHandlerExtension.
// Handles an event sent by the AI framework.
// ---------------------------------------------------------------------------
//
void CMCSPlugin::HandleEvent( const TDesC& aEventName, const TDesC& aParam )
    {
    if ( iEngine )
        {
        if( aEventName == KEventNameLaunchByIndex )
            {
            // We have no way of reporting errors to framework so just ignore them.
            TInt32 index;
            AiUtility::ParseInt( index, aParam );
            TRAP_IGNORE( iEngine->LaunchItemL( index - 1 ));
            }
        else if( aEventName == KEventNameShowSettings )
            {
            TRAP_IGNORE( iEngine->ShowSettingsL() );
            }
        }
    }

// ---------------------------------------------------------------------------
// From class CAiContentPublisher
// framework instructs plug-in that it is allowed to consume CPU resources
// ---------------------------------------------------------------------------
//
void CMCSPlugin::DoResumeL( TAiTransitionReason /*aReason*/ )
    {
    PublishL();
    }
    
// ---------------------------------------------------------------------------
// Frees engine resources
// ---------------------------------------------------------------------------
//    
void CMCSPlugin::FreeEngine()
    {
    delete iEngine;
    iEngine = NULL;
    }

// ---------------------------------------------------------------------------
// Delete content model
// ---------------------------------------------------------------------------
//    
void CMCSPlugin::DeleteContentModel()
    {
    if( iContentModel )
        {
        for ( TInt i = 0; i < iDataCount; i++ )
            {             
            if( iContentModel[i].cid )
                {
                TAny* cell = static_cast<TAny*>( const_cast<wchar_t*>( iContentModel[i].cid ) );
                User::Free( cell ); // Originally allocated with User::Alloc, so delete
                                    // with correlating method.
                iContentModel[i].cid = NULL;
                }
            }
        
        delete iContentModel;
        iContentModel = NULL;        
        }    
    }
  
// ======== GLOBAL FUNCTIONS ========
// ---------------------------------------------------------------------------
// Constructs and returns an application object.
// ---------------------------------------------------------------------------
//
EXPORT_C const TImplementationProxy* ImplementationGroupProxy( 
    TInt& aTableCount )
    {
    aTableCount = sizeof( KImplementationTable ) / 
        sizeof( TImplementationProxy );
    return KImplementationTable;
    }
