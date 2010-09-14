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
* Description:  MCS settings plug-in model.
*
*/

#include <eikenv.h>
#include <itemmap.h>
#include <mhomescreensettingsif.h>
#include <aistrcnv.h>
#include <StringLoader.h>
#include <uri16.h>
#include <mcspluginsettingsres.rsg>
#include <mcsmenuitem.h>

#include "mcspluginsettingsmodel.h"
#include "mcspluginsettingsapplist.h"
#include "mcspluginsettingsbkmlist.h"
#include "mcspluginsettingscontainer.h"


/**
 * Line format for the settings list box
 */
_LIT(KSettingListboxLineFormat, " \t%S\t\t%S");
_LIT( KMenuAttrParam, "param");
_LIT( KMenuAttrLocked, "locked");
_LIT8( KTrue, "true" );
_LIT8( KAppUid, "271012080" );
_LIT8( KProperNameType, "type" );
_LIT8( KProperNameParam, "param" );
_LIT8( KProperNameUid, "uid" );
_LIT8( KProperNameView, "view" );
_LIT8( KProperNameLocked, "locked" );
_LIT8( KProperValueFolder, "folder" );
_LIT8( KProperValueSuite, "suite" );
_LIT8( KProperValueBookmark, "bookmark" );
_LIT8( KProperValueAppl, "application" );
_LIT8( KProperValueMailbox, "mailbox" );
_LIT( KMenuTypeMailbox, "menu:mailbox" );
_LIT8( KMenuAttrInvokeSettingsUid, "0x99999990" );

using namespace HSPluginSettingsIf;

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

// -----------------------------------------------------------------------------
// Creates a formatted listbox line.
// -----------------------------------------------------------------------------
//
TPtrC CMCSPluginSettingsModel::ListBoxLineL( const TDesC& aCaption, TInt aIndex ) const
{
    HBufC* title = StringLoader::LoadLC(
            R_AI_MCS_SETTINGS_TXT_LINKN, aIndex + 1, iEnv );

   TPtrC caption;
   caption.Set(aCaption);

   TUriParser parser;
   TInt err = parser.Parse(aCaption);
   if (err == KErrNone)
   {
       // Remove scheme from the url.
       const TDesC& host = parser.Extract(EUriHost);
       if (host.Length())
       {
           caption.Set(host);
       }
   }

   // Format (" \t%S\t\t%S") without %S characters.
   TInt formatLength = KSettingListboxLineFormat().Length();

   HBufC* listBoxLine =
       HBufC::NewLC( title->Length() + 
                     caption.Length() + 
                     formatLength );

   TPtr ptr = listBoxLine->Des();
   ptr.Format(KSettingListboxLineFormat, title, &caption);

   delete iListBoxLine;
   iListBoxLine = NULL;
   iListBoxLine = listBoxLine;
   CleanupStack::Pop(listBoxLine);
   CleanupStack::PopAndDestroy(title);
   return *iListBoxLine;
}

// ======== MEMBER FUNCTIONS ========

// ---------------------------------------------------------------------------
// Two phased constructor
// ---------------------------------------------------------------------------
//
CMCSPluginSettingsModel* CMCSPluginSettingsModel::NewL( CMCSPluginSettings& aPlugin,
                                                        CCoeEnv* aEnv)
{
    CMCSPluginSettingsModel* self = new (ELeave) CMCSPluginSettingsModel(aPlugin, aEnv);

    CleanupStack::PushL(self);
    self->ConstructL();
    CleanupStack::Pop(self);

    return self;
}

// ---------------------------------------------------------------------------
// First phase construction
// ---------------------------------------------------------------------------
//
CMCSPluginSettingsModel::CMCSPluginSettingsModel( CMCSPluginSettings& aPlugin, 
                                                  CCoeEnv* aEnv )
                                                  : iPlugin( aPlugin ), 
                                                    iEnv( aEnv )
{
}

// ---------------------------------------------------------------------------
// Destructor
// ---------------------------------------------------------------------------
//
CMCSPluginSettingsModel::~CMCSPluginSettingsModel()
{
    delete iAppList;
    delete iBkmList;
    iSettings.Reset();    
    delete iPluginId;
    delete iListBoxLine;
    
    CHomescreenSettings::UnInitialize();
    iPluginSettings = NULL;
}

// ---------------------------------------------------------------------------
// Second phase construction
// ---------------------------------------------------------------------------
//
void CMCSPluginSettingsModel::ConstructL()
    {
    CHomescreenSettings::InitializeL( KAppUid );
    
    iPluginSettings = CHomescreenSettings::Instance();
    if( iPluginSettings == NULL )
        {
        User::Leave( KErrUnknown );
        }
    }

// ---------------------------------------------------------------------------
// Gets the latest settings from HSPS and updates
// ---------------------------------------------------------------------------
//
void CMCSPluginSettingsModel::UpdateSettingsL()
    {
    iSettings.Reset();
    if( !iPluginId )
        {
        return;
        }
    
    if (iContainer)
        {
        iContainer->CloseChangeDialog();
        }
    
    RPointerArray<CItemMap> settingItems;
    CleanupResetAndDestroyPushL( settingItems );

    User::LeaveIfError( 
            iPluginSettings->GetSettingsL( *iPluginId, settingItems ) );

    for ( TInt i = 0; i < settingItems.Count(); i++ )
        {
        CItemMap* itemMap = settingItems[i];
        // get properties
        RPointerArray<HSPluginSettingsIf::CPropertyMap>& properties
            = itemMap->Properties();
        TSettingItem item = ItemL( properties );
        iSettings.AppendL( item );
        }
    
    CleanupStack::PopAndDestroy(); // settingItems
    
    if (iContainer)
        {
        iContainer->ResetCurrentListL(0);
        }
    }

// ---------------------------------------------------------------------------
// Tries to find menuitem which matches given HSPS entry
// ---------------------------------------------------------------------------
//
TSettingItem CMCSPluginSettingsModel::ItemL( 
                 RPointerArray<HSPluginSettingsIf::CPropertyMap>& aProperties )
    {
    TSettingItem setting = { KErrNotFound, EApplication , EFalse, EFalse };

    TSettingType type = SettingTypeL( aProperties );
    if ( type == EApplication || type == EMailbox )
        {
        setting = iAppList->FindItemL( aProperties );
        }
    else if ( type == EBookmark )
        {
        setting = iBkmList->FindItemL( aProperties );
        }
    
    setting.locked = SettingLockedL( aProperties );
    setting.empty = SettingEmptyL( aProperties );
    
    return setting;
    }

// ---------------------------------------------------------------------------
// Gets Type (application or bookmark) of given HSPS entry
// ---------------------------------------------------------------------------
//
TSettingType CMCSPluginSettingsModel::SettingTypeL( 
                RPointerArray<HSPluginSettingsIf::CPropertyMap>& aProperties )
    {
    TSettingType type( EApplication );

    for( TInt i = 0; i < aProperties.Count(); i++ )
        {
        TPtrC8 name = aProperties[i]->Name();
        if( name == KProperNameType )
            {
            TPtrC8 value = aProperties[i]->Value();
            if( value == KProperValueBookmark )
                {
                type = EBookmark;
                }
            else
                {
                type = EApplication;
                }
            }
        }

    return type;
    }

// ---------------------------------------------------------------------------
// Gets locking status of given HSPS entry
// ---------------------------------------------------------------------------
//
TBool CMCSPluginSettingsModel::SettingLockedL( 
                RPointerArray<HSPluginSettingsIf::CPropertyMap>& aProperties )
    {
        
    for( TInt i = 0; i <aProperties.Count(); i++ )
        {
        TPtrC8 name = aProperties[i]->Name();
        if( name == KProperNameLocked )
            {
            TPtrC8 value = aProperties[i]->Value();
            if( value == KTrue )
                {
                return ETrue;
                }
            }
        }   
    
    return EFalse;
    }

// ---------------------------------------------------------------------------
// Gets empty status of given HSPS entry
// ---------------------------------------------------------------------------
//
TBool CMCSPluginSettingsModel::SettingEmptyL( 
                RPointerArray<HSPluginSettingsIf::CPropertyMap>& aProperties )
    {
        
    for( TInt i = 0; i <aProperties.Count(); i++ )
        {
        TPtrC8 name = aProperties[i]->Name();
        if( name == KProperNameUid )
            {
            TPtrC8 value = aProperties[i]->Value();
            if( value.Compare(KMenuAttrInvokeSettingsUid) == 0 )
                {
                return ETrue;
                }
            }
        }   
    
    return EFalse;
    }

// ---------------------------------------------------------------------------
// Saves menuitem to HSPS to the given shortcut index
// ---------------------------------------------------------------------------
//
void CMCSPluginSettingsModel::SaveSettingsL( const TInt& aIndex, 
                                             CMenuItem& aMenuItem )
    {    
    if( !iPluginId )
        {
        return;
        }

    RPointerArray<CItemMap> settingItems;    
    CleanupResetAndDestroyPushL( settingItems );
    User::LeaveIfError( iPluginSettings->GetSettingsL( *iPluginId, settingItems ) );

    if ( aIndex >= 0 && aIndex < settingItems.Count() )
        {
        TBool exists( EFalse );
        CItemMap* itemMap = settingItems[ aIndex ];
        
        // get properties
        RPointerArray<HSPluginSettingsIf::CPropertyMap>& properties
            = itemMap->Properties();

        const TInt KGranularity = 6;
        CDesC8Array* propertiesList = new ( ELeave ) CDesC8ArrayFlat( KGranularity );
        CleanupStack::PushL( propertiesList );
        propertiesList->AppendL( KProperNameType );
        propertiesList->AppendL( KProperNameParam );
        propertiesList->AppendL( KProperNameUid );
        propertiesList->AppendL( KProperNameView );
        // skip KProperNameLocked property, attribute may be missing. results into
		// leave with -1 when saving settings 
        
        // add missing properties
        for ( TInt i=0; i<propertiesList->Count(); i++ )
            {
            TBool found( EFalse );
            const TPtrC8 namePtr = propertiesList->MdcaPoint( i );
            for ( TInt j=0; j<properties.Count() && !found; j++ )
                {
                found = ( (namePtr == properties[ j ]->Name() ) ? ETrue : EFalse );
                }
            if ( !found )
                {
                CPropertyMap* property = CPropertyMap::NewLC();
                property->SetNameL( namePtr );
                property->SetValueL( KNullDesC8 );
                itemMap->AddPropertyMapL( property );
                CleanupStack::Pop( property );
                
                // get updated list
                properties = itemMap->Properties();
                }
            }
        CleanupStack::PopAndDestroy( propertiesList );

        for ( TInt i = 0; i < properties.Count(); i++ )
            {
            if ( properties[ i ]->Name() == KProperNameType )
                {
                TPtrC type = aMenuItem.Type();
                if ( type == KMenuTypeUrl )
                    {
                    properties[ i ]->SetValueL( KProperValueBookmark );
                    }
                else if ( type == KMenuTypeFolder )
                    {
                    properties[ i ]->SetValueL( KProperValueFolder );
                    }
                else if ( type == KMenuTypeSuite )
                    {
                    properties[ i ]->SetValueL( KProperValueSuite );
                    }
                else if( type == KMenuTypeMailbox )
                    {
                    properties[ i ]->SetValueL( KProperValueMailbox );
                    }
                else
                    {
                    properties[ i ]->SetValueL( KProperValueAppl );
                    }
                }
            else if ( properties[ i ]->Name() == KProperNameUid )
                {
                TPtrC uid = aMenuItem.GetAttributeL( KMenuAttrUid, exists );
                if ( exists && uid.Length() > 0 )
                    {
                    HBufC8* uid8( NULL );
                    uid8 = AiUtility::CopyToBufferL( uid8, uid );
                    CleanupStack::PushL( uid8 );
                    properties[ i ]->SetValueL( *uid8 );
                    CleanupStack::PopAndDestroy( uid8 );
                    }
                else
                    {
                    properties[ i ]->SetValueL( KNullDesC8 );
                    }
                }
            else if ( properties[ i ]->Name() == KProperNameView )
                {
                TPtrC type = aMenuItem.Type();
                TPtrC attr;
                if ( type == KMenuTypeUrl )
                    {
                    // In case of bookmark type item, bookmark URL is stored to view property
                    attr.Set( KMenuAttrUrl );
                    }
                else
                    {
                    attr.Set( KMenuAttrView );
                    }
                TPtrC view = aMenuItem.GetAttributeL( attr, exists );
                if( exists && view.Length() > 0 )
                    {
                    HBufC8* view8( NULL );
                    view8 = AiUtility::CopyToBufferL( view8, view );
                    CleanupStack::PushL( view8 );
                    properties[i]->SetValueL( *view8 );
                    CleanupStack::PopAndDestroy( view8 );
                    }
                else
                    {
                    properties[ i ]->SetValueL( KNullDesC8 );
                    }
                }
            else if ( properties[ i ]->Name() == KProperNameParam )
                {
                TPtrC param = aMenuItem.GetAttributeL( KMenuAttrParam, exists );
                TBool isFolder = EFalse;
                TPtrC type = aMenuItem.Type();

                // is the item folder or suite?

                if ( type == KMenuTypeFolder )
                    {
                    isFolder = ETrue;
                    }

                if ( (exists && param.Length() > 0) || (isFolder) )
                    {
                    // the folder id is stored 
                    // in param attribute in HSPS
                    if ( isFolder )
                        {
                        TBuf<8> number;
                        number.Num( aMenuItem.Id() );
                        param.Set( number );
                        }

                    HBufC8* param8( NULL );
                    param8 = AiUtility::CopyToBufferL( param8, param );
                    CleanupStack::PushL( param8 );
                    properties[ i ]->SetValueL( *param8 );
                    CleanupStack::PopAndDestroy( param8 );
                    }
                else
                    {
                    properties[ i ]->SetValueL( KNullDesC8 );
                    }
                }
            else if ( properties[ i ]->Name() == KProperNameLocked )
                {
                TPtrC locked = aMenuItem.GetAttributeL( KMenuAttrLocked, exists );
                if ( exists && locked.Length() > 0 )
                    {
                    HBufC8* locked8( NULL );
                    locked8 = AiUtility::CopyToBufferL( locked8, locked );
                    CleanupStack::PushL( locked8 );
                    properties[ i ]->SetValueL( *locked8 );
                    CleanupStack::PopAndDestroy( locked8 );
                    }
                else
                    {
                    properties[i]->SetValueL( KNullDesC8 );
                    }
                }
            }
        }

    // EFalse tells that modified settings are not stored to plugin reference
    User::LeaveIfError( iPluginSettings->SetSettingsL( *iPluginId, settingItems, EFalse ) );
    CleanupStack::PopAndDestroy(); // settingItems
    }

// ---------------------------------------------------------------------------
// Updates settings container.
// ---------------------------------------------------------------------------
//
void CMCSPluginSettingsModel::SetPluginIdL( const TDesC8& aPluginId )
    {
    delete iPluginId;
    iPluginId = NULL;
    iPluginId = aPluginId.AllocL();    
    }

// ---------------------------------------------------------------------------
// From MDesCArray
// Returns the number of descriptor elements in a descriptor array.
// ---------------------------------------------------------------------------
//
TInt CMCSPluginSettingsModel::MdcaCount() const
{
    return iSettings.Count();
}

// ---------------------------------------------------------------------------
// From MDesCArray
// Indexes into a descriptor array.
// ---------------------------------------------------------------------------
//
TPtrC CMCSPluginSettingsModel::MdcaPoint( TInt aIndex ) const
    {
    if (aIndex < 0 || aIndex >= iSettings.Count())
        {
        TPtrC ret(KNullDesC);
        return ret;
        }
    
    TPtrC line( KNullDesC ); 
    if ( iSettings[aIndex].type == EApplication )
        {
        // first, we need to check if the item is missing 
        // (application uninstalled or mmc card removed)
        // If it is, we return "Undefined" application name instead
        // In case of empty item, it's own name is returned
        if ( iSettings[ aIndex ].id == KErrNotFound )
            {
            if ( iSettings[ aIndex ].empty )
                {
                const TDesC& caption = iAppList->EmptyText();
                TRAP_IGNORE( line.Set( ListBoxLineL( caption, aIndex ) ) )
                }
            else
                {
                const TDesC& caption = iAppList->UndefinedText();
                TRAP_IGNORE( line.Set( ListBoxLineL( caption, aIndex ) ) )
                }
            }
        else
            {
            const TDesC& caption = iAppList->MdcaPoint( iSettings[ aIndex ].id );
            TRAP_IGNORE( line.Set( ListBoxLineL( caption, aIndex ) ) )
            }
        }
    else
        {
        const TDesC& caption = iBkmList->MdcaPoint( iSettings[aIndex].id );
        TRAP_IGNORE( line.Set( ListBoxLineL( caption, aIndex ) ) )
        }

    return line;
    }

// ---------------------------------------------------------------------------
// Returns a setting ID for the given index.
// ---------------------------------------------------------------------------
//
TInt CMCSPluginSettingsModel::ItemId( TInt aIndex ) const
{
    if ( aIndex >= 0 && aIndex < iSettings.Count() )
    {
        return iSettings[ aIndex ].id;
    }

    return KErrNotFound;
}

// ---------------------------------------------------------------------------
// Returns a pointer to setting item for the given index
// ---------------------------------------------------------------------------
//
const TSettingItem CMCSPluginSettingsModel::Item( TInt aIndex ) const
    {
    TSettingItem setting = { KErrNotFound, EApplication, EFalse, EFalse };

    if ( aIndex >= 0 && aIndex < iSettings.Count() )
        {
        setting = iSettings[ aIndex ];
        }

    return setting;
    }

// ---------------------------------------------------------------------------
// Replaces shortcut item at given index by a new one.
// ---------------------------------------------------------------------------
//
TBool CMCSPluginSettingsModel::ReplaceItemL( const TInt& aSettingIndex, 
                                             TInt aId,
                                             TSettingType aType )
    {
    TBool replaced( EFalse );
    if (aSettingIndex >= 0 && aSettingIndex < iSettings.Count())
        {
        CMenuItem* item( NULL );
        if ( aType == EApplication )
            {
            item = iAppList->ItemL( aId );
            }
         else
            {
            item = iBkmList->ItemL( aId );
            }
        
        if ( item )
            {
            SaveSettingsL( aSettingIndex, *item );
            iSettings[ aSettingIndex ].id = aId;
            iSettings[ aSettingIndex ].type = aType;
            replaced = ETrue;
            }
        }
    return replaced;
    }

// ---------------------------------------------------------------------------
// Sets a container
// ---------------------------------------------------------------------------
//
void CMCSPluginSettingsModel::SetContainer(CMCSPluginSettingsContainer* aContainer)
{
    iContainer = aContainer;
}

// ---------------------------------------------------------------------------
// Returns application list
// ---------------------------------------------------------------------------
//
CMCSPluginSettingsAppList* CMCSPluginSettingsModel::AppList()
{
    return iAppList;
}

// ---------------------------------------------------------------------------
// Returns bookmarklist
// ---------------------------------------------------------------------------
//
CMCSPluginSettingsBkmList* CMCSPluginSettingsModel::BkmList()
{
    return iBkmList;
}

// ---------------------------------------------------------------------------
// Updates application list
// ---------------------------------------------------------------------------
//
void CMCSPluginSettingsModel::UpdateAppListL( TBool aUpdateSettings )
    {
    if( !iAppList )
        {
        iAppList = CMCSPluginSettingsAppList::NewL();
        }
    iAppList->StartL();
    if ( aUpdateSettings )
        {
        UpdateSettingsL();
        }
    }

// ---------------------------------------------------------------------------
// Updates bookmark list
// ---------------------------------------------------------------------------
//
void CMCSPluginSettingsModel::UpdateBkmListL( TBool aUpdateSettings )
    {
    delete iBkmList;
    iBkmList = NULL;
    iBkmList = CMCSPluginSettingsBkmList::NewL();
    if ( aUpdateSettings )
        {
        UpdateSettingsL();
        }
    }

// End of File.
