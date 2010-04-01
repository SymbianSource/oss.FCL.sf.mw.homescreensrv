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

#include "hs_app_mcsplugindata.h"
#include "hs_app_mcspluginengine.h"

using namespace HSPluginSettingsIf;

_LIT8( KAppUid, "271012080" );
_LIT( KMenuAttrParam, "param" );
_LIT( KMenuAttrLocked, "locked" );
_LIT8( KProperNameType, "type" );
_LIT8( KProperNameParam, "param" );
_LIT8( KProperNameUid, "uid" );
_LIT8( KProperNameView, "view" );
_LIT8( KProperValueFolder, "folder" );
_LIT8( KProperValueBookmark, "bookmark" );
_LIT8( KProperValueAppl, "application" );

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
     // AILaunch uid in decimal format
    iPluginSettings = CHomescreenSettings::NewL( KAppUid,
                                                 iInstanceUid,
                                                 this );
    UpdateDataL();
    }

// ---------------------------------------------------------------------------
// Destructor
// ---------------------------------------------------------------------------
//
CMCSPluginData::~CMCSPluginData()
    {   
    iData.Close();
    delete iPluginSettings;
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
    for( TInt i = 0; i < count; i++ )
       {
        CItemMap* itemMap = settings[i];
        RPointerArray<HSPluginSettingsIf::CPropertyMap>& properties
            = itemMap->Properties();
        TMenuItem item = CreateMenuItemL( properties );
        TMCSData data;
        TInt id = -1;
        
        if ( count == iData.Count() )
            {
            id = iData[i].MenuItem().Id();
            }
                   
        if ( id > 0 )
            {
            if ( item.Id() != id )
                {
                data.SetMenuItem( item );
                data.SetDirty( ETrue );
                iData.Remove( i );
                iData.InsertL( data, i );
                }
        	}
        else
            {
            data.SetMenuItem( item );
            data.SetDirty( ETrue );
            iData.AppendL( data );
        	}
        }
    
        CleanupStack::PopAndDestroy(); // settingsCleanupItem
    }

// ---------------------------------------------------------------------------
// 
// ---------------------------------------------------------------------------
//
TInt CMCSPluginData::SettingsChangedL( const TDesC8& /*aEvent*/,  const TDesC8& /*aPluginName*/,
                           const TDesC8& /*aPluginUid*/, const TDesC8& /*aPluginId*/ )
    {
    UpdateDataL();
    return KErrNone;
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

