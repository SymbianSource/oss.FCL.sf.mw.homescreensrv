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

// System includes
#include <ecom/ecom.h>
#include <ecom/implementationproxy.h>
#include <gulicon.h>
#include <mcsmenuitem.h>

// User includes
#include <aicontentobserver.h>
#include <aiutility.h>
#include <aistrcnv.h>
#include "mcspluginuids.hrh"
#include "mcsplugin.h"
#include "mcsplugindata.h"
#include "mcspluginengine.h"
#include "aipluginsettings.h"

// Constants
const TUint KPluginNameSeprator =  '/';

const TInt KImplUidMCSPlugin = AI_UID_ECOM_IMPLEMENTATION_CONTENTPUBLISHER_MCSPLUGIN;

_LIT( KEventNameLaunchByIndex,  "LaunchByIndex" );
_LIT( KEventNameShowSettings,   "ShowSettings" );
_LIT( KContentItemTypeText, "text" );
_LIT( KContentItemTypeImage, "image" );

const TImplementationProxy KImplementationTable[] =
    {
    IMPLEMENTATION_PROXY_ENTRY( KImplUidMCSPlugin, CMCSPlugin::NewL ) 
    };

// ======== LOCAL FUNCTIONS ========

// ======== MEMBER FUNCTIONS ========
// ----------------------------------------------------------------------------
// CMCSPlugin::NewL
//
// ----------------------------------------------------------------------------
//
CMCSPlugin* CMCSPlugin::NewL()
    {
    CMCSPlugin* self = new ( ELeave ) CMCSPlugin;
    CleanupStack::PushL( self );
    self->ConstructL();
    CleanupStack::Pop( self );
 
    return self;
    }
    
// ----------------------------------------------------------------------------
// CMCSPlugin::CMCSPlugin
//
// ----------------------------------------------------------------------------
//
CMCSPlugin::CMCSPlugin()
    {
    }
    
// ----------------------------------------------------------------------------
// CMCSPlugin::ConstructL
// 
// ----------------------------------------------------------------------------
//
void CMCSPlugin::ConstructL()
    {
    }
    
// ----------------------------------------------------------------------------
// Destructor
// Deletes all data created to heap
// ----------------------------------------------------------------------------
//
CMCSPlugin::~CMCSPlugin()
    {
     Release( iContent );

    delete iEngine;
    iObservers.Close();
    
    DeleteContentModel();
    }

// ----------------------------------------------------------------------------
// CMCSPlugin::PublishL
// Publishes the all the items
// ----------------------------------------------------------------------------
//
void CMCSPlugin::PublishL()
    {    
    TInt err( KErrNone );
    TInt observers( iObservers.Count() );
    TInt transactionId( reinterpret_cast<TInt>( this ) );
    TInt menuItems ( iEngine->MenuItemCount() );

    for ( TInt i = 0; i < observers; i++ )
        {
        MAiContentObserver* observer( iObservers[ i ] );
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

// ----------------------------------------------------------------------------
// CMCSPlugin::PublishLItemL
// Publishes one item to given index
// ----------------------------------------------------------------------------
//
void CMCSPlugin::PublishLItemL( MAiContentObserver& aObserver, 
    TMCSData& aData, TInt aIndex )
    {
    if( !aData.IsDirty() )
        {
        return;
        }

    CMenuItem* item = NULL;
    TRAP_IGNORE ( item = iEngine->FetchMenuItemL( aData.MenuItem() ) );
    
    CleanupStack::PushL( item );
    
    // One widget item has iDataCount number of elements
    for ( TInt i = 0; i < iDataCount; i++ )
        {
        if ( iContentModel[i].type == KAiContentTypeBitmap )
            {
            //Publish image
            if ( aObserver.CanPublish( *this, i, aIndex ) )
                {
                CGulIcon* icon( iEngine->ItemIconL( item, 
                    TPtrC16( ( const TText16* ) iContentModel[ i ].cid ) ) );
                
                aObserver.PublishPtr( *this, i, icon , aIndex );
                }
            }
        else if ( iContentModel[i].type == KAiContentTypeText )
            {
            //Publish  text
            if ( aObserver.CanPublish( *this, i, aIndex ) )
                {
                TPtrC name( iEngine->ItemTextL( item, 
                    TPtrC16( ( const TText16* ) iContentModel[ i ].cid ) ) );
                
                aObserver.Publish( *this, i, name, aIndex );
                }
            }
        }//content items

    CleanupStack::PopAndDestroy( item );
    }

// ----------------------------------------------------------------------------
// CMCSPlugin::Start
// 
// ----------------------------------------------------------------------------
//
void CMCSPlugin::Start( TStartReason aReason )
    {
    if ( aReason == EPluginStartup )
        {
        TRAP_IGNORE( iEngine->CreateRuntimeMenuItemsL() );
        }    
    }

// ----------------------------------------------------------------------------
// CMCSPlugin::Stop
// 
// ----------------------------------------------------------------------------
//
void CMCSPlugin::Stop( TStopReason aReason )
    {
    if( aReason == EPluginShutdown )
        {
        TRAP_IGNORE( iEngine->CleanMCSItemsL() );
        }
    }

// ----------------------------------------------------------------------------
// CMCSPlugin::Resume
// 
// ----------------------------------------------------------------------------
//
void CMCSPlugin::Resume( TResumeReason aReason )
    {       
    if ( aReason == EForeground )
        {
        iEngine->SetBackupRestore( EFalse );
        
        if ( PublishRequired() )
            {
            TRAP_IGNORE( PublishL() );
            }        
        }
    }

// ----------------------------------------------------------------------------
// CMCSPlugin::Suspend
// 
// ----------------------------------------------------------------------------
//
void CMCSPlugin::Suspend( TSuspendReason aReason )
    {    
    if ( aReason == EGeneralThemeChange )
        {                   
        TInt dataCount( iEngine->MenuItemCount() );
        
        for ( TInt i = 0; i < dataCount; i++ )
            {
            TRAP_IGNORE( iEngine->MenuDataL( i ).SetDirty( ETrue ) );
            }                
        }
    else if ( aReason == EBackupRestore )
        {
        // Prevent item launching during backup / restore 
        iEngine->SetBackupRestore( ETrue );
        }
    }

// ----------------------------------------------------------------------------
// CMCSPlugin::SubscribeL
//
// ----------------------------------------------------------------------------
//
void CMCSPlugin::SubscribeL( MAiContentObserver& aObserver )
    { 
    iObservers.AppendL( &aObserver );
    }
   
// ----------------------------------------------------------------------------
// CMCSPlugin::ConfigureL
// 
// ----------------------------------------------------------------------------
//
void CMCSPlugin::ConfigureL( RAiSettingsItemArray& aSettings )
    {    
    iEngine = CMCSPluginEngine::NewL( *this, PublisherInfo().Namespace() );
    
    TLinearOrder<MAiPluginSettings> sortMethod( CMCSPlugin::CompareItems );
    RAiSettingsItemArray contentItemsArr;

    TInt count( aSettings.Count() );
    
    for ( TInt i = 0; i < count; i++ )
       {
       MAiPluginSettings* setting( aSettings[ i ] );
       
       if( setting->AiPluginItemType() == EAiPluginContentItem )
           {
           MAiPluginContentItem& contItem( setting->AiPluginContentItem() );
           TPtrC name( contItem.Name() );
           TPtrC type( contItem.Type() );
           
           contentItemsArr.InsertInOrder( setting, sortMethod );
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
        
            MAiPluginContentItem& contentItem( 
                contentItemsArr[ i ]->AiPluginContentItem() );

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
            
            TInt pos( contentItem.Name().Locate( KPluginNameSeprator ) );
            
            HBufC* contentId = HBufC::NewL( contentItem.Name().Length() );
            CleanupStack::PushL( contentId );
            
            TPtr ptr( contentId->Des() ); 
            ptr = contentItem.Name().Mid( pos + 1 );
            
            TInt sizeOfContentId( ptr.Size() + sizeof( wchar_t ) );
            
            iContentModel[i].cid = 
                static_cast<const wchar_t*>( User::AllocL( sizeOfContentId ) );
                
            Mem::Copy( ( TAny* )iContentModel[i].cid, 
                ptr.PtrZ(), sizeOfContentId );
            
            CleanupStack::PopAndDestroy( contentId );
            }
        
        iContent = AiUtility::CreateContentItemArrayIteratorL( 
                iContentModel, iDataCount );
        }
    
    contentItemsArr.Reset();
    // We own the array so destroy it
    aSettings.ResetAndDestroy();
    }

// ----------------------------------------------------------------------------
// CMCSPlugin::GetProperty
// 
// ----------------------------------------------------------------------------
//
TAny* CMCSPlugin::GetProperty( TProperty aProperty )
    {
    if( aProperty == EPublisherContent )
        {
        return static_cast< MAiContentItemIterator* >( iContent );        
        }
        
    return NULL;
    }
 
// ----------------------------------------------------------------------------
// CMCSPlugin::HandleEvent
// 
// ----------------------------------------------------------------------------
//
void CMCSPlugin::HandleEvent( const TDesC& aEventName, const TDesC& aParam )
    {       
    if( aEventName == KEventNameLaunchByIndex )
        {        
        TInt32 index;
        AiUtility::ParseInt( index, aParam );
        
        TRAP_IGNORE( iEngine->LaunchItemL( index - 1 ) );
        }
    else if( aEventName == KEventNameShowSettings )
        {
        TRAP_IGNORE( iEngine->ShowSettingsL() );
        }    
    }

// ----------------------------------------------------------------------------
// CMCSPlugin::CompareItems
//
// ----------------------------------------------------------------------------
//
TInt CMCSPlugin::CompareItems( const MAiPluginSettings& aFirst,
    const MAiPluginSettings& aSecond )
    {
    MAiPluginSettings& first = const_cast<MAiPluginSettings&>(aFirst);
    MAiPluginSettings& second = const_cast<MAiPluginSettings&>(aSecond);
    return first.AiPluginContentItem().Name().CompareC(second.AiPluginContentItem().Name());    
    }

// ----------------------------------------------------------------------------
// CMCSPlugin::DeleteContentModel
//
// ----------------------------------------------------------------------------
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

// ----------------------------------------------------------------------------
// CMCSPlugin::PublishRequired
//
// ----------------------------------------------------------------------------
//    
TBool CMCSPlugin::PublishRequired() const
    {
    TInt count( iEngine->MenuItemCount() );
    
    TBool retval( EFalse );
    
    for ( TInt i = 0; !retval && i < count; i++ )
        {               
        TRAP_IGNORE( retval = iEngine->MenuDataL( i ).IsDirty() );       
        }
        
    
    return retval;
    }

// ======== GLOBAL FUNCTIONS ========
// ----------------------------------------------------------------------------
// Constructs and returns an application object.
// ----------------------------------------------------------------------------
//
EXPORT_C const TImplementationProxy* ImplementationGroupProxy( 
    TInt& aTableCount )
    {
    aTableCount = sizeof( KImplementationTable ) / 
        sizeof( TImplementationProxy );
    return KImplementationTable;
    }

// End of file
