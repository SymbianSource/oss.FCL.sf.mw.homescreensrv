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
* Description:  
*
*/
#include <mcsmenuitem.h>
#include <mcsmenufilter.h>
#include <itemmap.h>
#include <aistrcnv.h>

#include "mcsplugindata.h"
#include "mcspluginengine.h"

using namespace HSPluginSettingsIf;

_LIT( KMenuAttrParam, "param" );
_LIT( KMenuAttrLocked, "locked" );
_LIT8( KProperNameType, "type" );
_LIT8( KProperNameParam, "param" );
_LIT8( KProperNameUid, "uid" );
_LIT8( KProperNameView, "view" );
_LIT8( KProperNameLocked, "locked" );
_LIT8( KProperValueFolder, "folder" );
_LIT8( KProperValueBookmark, "bookmark" );
_LIT8( KProperValueAppl, "application" );
_LIT8( KProperValueMailbox, "mailbox" );
_LIT8( KMenuAttrUndefUid, "0x99999991" );

_LIT( KMyMenuData, "matrixmenudata" );
_LIT( KMenuTypeMailbox, "menu:mailbox" );


#define KMCSCmailMtmUidValue 0x2001F406

// ======== LOCAL FUNCTIONS ========

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

// ======== MEMBER FUNCTIONS ========

// ---------------------------------------------------------------------------
// Default constructor
// ---------------------------------------------------------------------------
//
CMCSData::CMCSData()
    {
    }

// ---------------------------------------------------------------------------
// Destructor
// ---------------------------------------------------------------------------
//
CMCSData::~CMCSData()
    {
    delete iName;
    delete iValue;
    }

// ---------------------------------------------------------------------------
// 
// ---------------------------------------------------------------------------
//
void CMCSData::SetMenuItem( TMenuItem& aMenuItem )
    {
    iMenuItem = aMenuItem;
    }

// ---------------------------------------------------------------------------
// 
// ---------------------------------------------------------------------------
//
TMenuItem& CMCSData::MenuItem()
    {
    return iMenuItem;
    }

// ---------------------------------------------------------------------------
// 
// ---------------------------------------------------------------------------
//
TDesC& CMCSData::Name()
    {
    return *iName;
    }
    
// ---------------------------------------------------------------------------
// 
// ---------------------------------------------------------------------------
//
void CMCSData::SetNameL( const TDesC& aName )
    {
	delete iName;
	iName = NULL;
    iName = aName.AllocL();
    }

// ---------------------------------------------------------------------------
// 
// ---------------------------------------------------------------------------
//
TDesC& CMCSData::Value()
    {
    return *iValue;
    }
    
// ---------------------------------------------------------------------------
// 
// ---------------------------------------------------------------------------
//
void CMCSData::SetValueL( const TDesC& aValue )
    {
	delete iValue;
	iValue = NULL;
    iValue = aValue.AllocL();
    }

// ---------------------------------------------------------------------------
// 
// ---------------------------------------------------------------------------
//
void CMCSData::SetDirty( TBool aDirty )
    {
    iDirty = aDirty;
    }

// ---------------------------------------------------------------------------
// 
// ---------------------------------------------------------------------------
//
TBool CMCSData::IsDirty() const
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
    iPluginSettings = CHomescreenSettings::Instance();
    if( iPluginSettings == NULL )
        {
        User::Leave( KErrUnknown );
        }
    iPluginSettings->AddObserverL( this );

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
    iData.ResetAndDestroy();
    iMenu.Close();
    }

// ---------------------------------------------------------------------------
// Gets the instance specific settings from HSPS and creates data items
// ---------------------------------------------------------------------------
//
void CMCSPluginData::UpdateDataL()
    {
    RPointerArray<CItemMap> settings;
    CleanupResetAndDestroyPushL( settings );

    iPluginSettings->GetSettingsL( iInstanceUid, settings );
    TInt count = settings.Count();
    TBool wasEmpty = !iData.Count();
    
    for ( TInt i = 0; i < count; i++ )
       {
        CItemMap* itemMap = settings[ i ];
        RPointerArray<HSPluginSettingsIf::CPropertyMap>& properties
            = itemMap->Properties();
        CMCSData* data = GetMenuDataL( properties );

        if ( wasEmpty )
            {
            // list of shortcut slot was empty
            // we append the shortcut data slots one-by-one to the list
            data->SetDirty( ETrue );
            iData.AppendL( data );
            }
        else 
            {
            // check for updates in existing shortcut data slot
            // if menuitem id has changed, replace the item and 
            // set as dirty
            TInt id = -1;
            id = iData[ i ]->MenuItem().Id();
            
            if ( data->MenuItem().Id() != id )
                {
                data->SetDirty( ETrue );
                CMCSData* oldData = iData[i];
                iData.Remove( i );
                delete oldData;
                iData.InsertL( data, i );
                }
            }
        }

    // Cleanup.
    CleanupStack::PopAndDestroy(); // settings
    }

// ---------------------------------------------------------------------------
// Removes obsolete data and saves 'Undefined' item to HSPS
// Used when active mailbox is deleted from system.
// ---------------------------------------------------------------------------
//
void CMCSPluginData::RemoveDataL( TInt aId )
    {
    TInt count = iData.Count();
    for( TInt i = 0; i < count; i++ )
        {
        CMCSData* data = iData[i];
        if( data->MenuItem().Type() == KMenuTypeMailbox &&
            data->MenuItem().Id() == aId )
            {
            iData[i]->MenuItem().SetId( KErrNotFound );
            iData[i]->SetDirty( ETrue );
            SaveUndefinedItemL( i );
            }
        }
    }

// ---------------------------------------------------------------------------
// Call back from Homescreen settings
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
CMCSData* CMCSPluginData::GetMenuDataL( RPointerArray<HSPluginSettingsIf::CPropertyMap>& aProperties )
    {
    TPtrC8 type;
    TPtrC8 uid;
    TPtrC8 view;
    TPtrC8 param;
    
    // first we need to check the item type
    for ( TInt i = 0; i < aProperties.Count(); i++ )
        {
        if ( aProperties[i]->Name() == KProperNameType )
            {
            type.Set( aProperties[i]->Value());
            }
        else if( aProperties[i]->Name() == KProperNameUid)
            {
            uid.Set( aProperties[i]->Value());
            }
        else if( aProperties[i]->Name() == KProperNameView)
            {
            view.Set( aProperties[i]->Value());
            }
        else if( aProperties[i]->Name() == KProperNameParam )
            {
            param.Set( aProperties[i]->Value());
            }
        }
    CMCSData* data = new ( ELeave ) CMCSData();
    CleanupStack::PushL( data );
    if( type == KProperValueBookmark )
        {
        GetBkmDataL( view, param, *data );
        }
    else if( type == KProperValueFolder )
        {
        GetFolderData( param, *data );
        }
    else if( type == KProperValueMailbox )
        {
        GetMailboxDataL( uid, param, *data );
        }
    else
        {
        GetMCSDataL( aProperties, *data );
        }
    CleanupStack::Pop( data );
    return data;
    }

// ---------------------------------------------------------------------------
// Creates bookmark data item.
// ---------------------------------------------------------------------------
//
void CMCSPluginData::GetBkmDataL( const TDesC8& aView, const TDesC8& aParam, CMCSData& aData )
    {
    TMenuItem item;
    item.SetType( KMenuTypeUrl );
    aData.SetMenuItem( item );

    HBufC* view( NULL );
    view = AiUtility::CopyToBufferL( view, aView );
    CleanupStack::PushL( view );
    aData.SetValueL( *view );
    CleanupStack::PopAndDestroy( view );
    
    HBufC* param( NULL );
    param = AiUtility::CopyToBufferL( param, aParam );
    CleanupStack::PushL( param );
    aData.SetNameL( *param );
    CleanupStack::PopAndDestroy( param );
    }
    
// ---------------------------------------------------------------------------
// Creates folder data item.
// ---------------------------------------------------------------------------
//
void CMCSPluginData::GetFolderData( const TDesC8& aParam, CMCSData& aData )
    {
    // In folder case, we have to extract id from 
    // param attribute and return item with this id
    // convert id to integer
    TInt id;
    TLex8 lextmp( aParam);
    lextmp.Val( id );
    
    TMenuItem item;
    item.SetType( KMenuTypeFolder );
    item.SetId( id );
    aData.SetMenuItem( item );
    }

// ---------------------------------------------------------------------------
// Creates mailbox data item.
// ---------------------------------------------------------------------------
//
void CMCSPluginData::GetMailboxDataL( const TDesC8& aUid, const TDesC8& aParam, CMCSData& aData )
    {
    TInt id( KErrNotFound );
    TLex8 lextmp( aUid);
    lextmp.Val( id );
    
    TMenuItem item;
    item.SetType( KMenuTypeMailbox );
    item.SetId( id );
    aData.SetMenuItem( item );
    
    HBufC* param( NULL );
    param = AiUtility::CopyToBufferL( param, aParam );
    CleanupStack::PushL( param );
    
    aData.SetNameL( *param );
    
    CleanupStack::PopAndDestroy( param );
    }
    
// ---------------------------------------------------------------------------
// Gets data item from MCS
// ---------------------------------------------------------------------------
//
void CMCSPluginData::GetMCSDataL( RPointerArray<HSPluginSettingsIf::CPropertyMap>& aProperties,
        CMCSData& aData)
    {
    CMenuFilter* filter = CMenuFilter::NewLC();

    // Exclude 'view' and 'param' attributes from search criteria by default
    // Criterias will be added to filter if setting defines them
    filter->DoNotHaveAttributeL( KMenuAttrView );
    filter->DoNotHaveAttributeL( KMenuAttrParam );
    
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
            filter->HaveAttributeL( *name, *value );  
            }
        CleanupStack::PopAndDestroy( value );
        CleanupStack::PopAndDestroy( name );
        }
    
    // locked property excluded from search pattern 
    filter->DoNotHaveAttributeL( KMenuAttrLocked );
    
    TMenuItem item = iEngine.FindMenuItemL( *filter );
    CleanupStack::PopAndDestroy( filter );
    aData.SetMenuItem( item );
    }

// ---------------------------------------------------------------------------
// Returns menu item for given index
// ---------------------------------------------------------------------------
//
CMCSData& CMCSPluginData::DataItemL( TInt aIndex )
    {
    if( aIndex < 0 || aIndex >= iData.Count())
        {
        User::Leave( KErrArgument ); 
        }
    return *iData[aIndex];
    }

// ---------------------------------------------------------------------------
// Save the undefined item.
// ---------------------------------------------------------------------------
//
void CMCSPluginData::SaveUndefinedItemL( const TInt& aIndex )
    {
    RPointerArray<CItemMap> settingItems;
    CleanupResetAndDestroyPushL( settingItems );
    
    iPluginSettings->GetSettingsL( iInstanceUid, settingItems );
    if ( aIndex >= 0 && aIndex < settingItems.Count() )
        {
        CItemMap* itemMap = settingItems[ aIndex ];
        RPointerArray<HSPluginSettingsIf::CPropertyMap> properties;
        properties = itemMap->Properties();
        for ( TInt i = 0; i < properties.Count(); i++ )
            {
            if ( properties[ i ]->Name() == KProperNameType )
                {
                properties[ i ]->SetValueL( KProperValueAppl );
                }
            else if ( properties[ i ]->Name() == KProperNameUid )
                {
                properties[ i ]->SetValueL( KMenuAttrUndefUid );
                }
            else if ( properties[ i ]->Name() == KProperNameView )
                {
                properties[ i ]->SetValueL( KNullDesC8 );
                }
            else if ( properties[ i ]->Name() == KProperNameParam )
                {
                properties[ i ]->SetValueL( KNullDesC8 );
                }
            else if ( properties[ i ]->Name() == KProperNameLocked )
                {
                properties[i]->SetValueL( KNullDesC8 );
                }
            }
        }
    // ETrue tells that modified settings are stored also to plugin reference
    iPluginSettings->SetSettingsL( iInstanceUid, settingItems, ETrue );
    CleanupStack::PopAndDestroy(); // settingItems
    }

// End of file
