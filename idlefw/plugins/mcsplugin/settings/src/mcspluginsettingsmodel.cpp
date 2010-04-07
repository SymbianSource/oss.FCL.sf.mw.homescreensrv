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


#include "mcspluginsettings.h"
#include "mcspluginsettingsmodel.h"
#include "mcspluginsettingscontainer.h"


#include "debug.h"

#include <mcsmenuitem.h>

/**
 * Line format for the settings list box
 */
_LIT(KSettingListboxLineFormat, " \t%S\t\t%S");
_LIT( KMenuAttrParam, "param");
_LIT( KMenuAttrLocked, "locked");
_LIT8( KTrue, "true" );
// changed from 0x102750F0 = 271012080 to 0xEDFF45C4 = 3992929732 for standalone app
_LIT8( KAppUid, "3992929732" );
_LIT8( KProperNameType, "type" );
_LIT8( KProperNameParam, "param" );
_LIT8( KProperNameUid, "uid" );
_LIT8( KProperNameView, "view" );
_LIT8( KProperNameLocked, "locked" );
_LIT8( KProperValueFolder, "folder" );
_LIT8( KProperValueSuite, "suite" );
_LIT8( KProperValueBookmark, "bookmark" );
_LIT8( KProperValueAppl, "application" );



using namespace HSPluginSettingsIf;

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
       HBufC::NewLC(title->Length() + caption.Length() + formatLength);

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
    delete iPluginSettings;
    delete iPluginId;
    delete iListBoxLine;
}

// ---------------------------------------------------------------------------
// Second phase construction
// ---------------------------------------------------------------------------
//
void CMCSPluginSettingsModel::ConstructL()
{
    iAppList = CMCSPluginSettingsAppList::NewL();
    iAppList->StartL();
    iBkmList = CMCSPluginSettingsBkmList::NewL();
}


// ---------------------------------------------------------------------------
// Gets the latest settings from HSPS and updates
// ---------------------------------------------------------------------------
//
void CMCSPluginSettingsModel::UpdateSettingsL( const TDesC8& aPluginId )
    {
    if( !iPlugin.Activated() )
        {
        return;
        }
    if( !iPluginSettings )
        {
        // AILaunch uid in decimal format
        iPluginSettings = CHomescreenSettings::NewL( KAppUid, aPluginId, this );
        iPluginId = aPluginId.AllocL();
        }

    iSettings.Reset();
    RPointerArray<CItemMap> settingItems;
    CleanupClosePushL( settingItems );

    iPluginSettings->GetSettingsL( *iPluginId, settingItems );

    TInt count = settingItems.Count();
    for ( TInt i = 0; i < count; i++ )
        {
        CItemMap* itemMap = settingItems[i];
        RPointerArray<HSPluginSettingsIf::CPropertyMap> properties;
        properties = itemMap->Properties();
        TSettingItem item = ItemL( properties );
        iSettings.AppendL( item );
        }
    CleanupStack::Pop( &settingItems );
    settingItems.ResetAndDestroy();
    }

// ---------------------------------------------------------------------------
// Tries to find menuitem which matches given HSPS entry
// ---------------------------------------------------------------------------
//
TSettingItem CMCSPluginSettingsModel::ItemL( 
                 RPointerArray<HSPluginSettingsIf::CPropertyMap>& aProperties )
    {
    TSettingItem setting = { KErrNotFound, EApplication , EFalse };

    TSettingType type = SettingTypeL( aProperties );
    if( type == EApplication )
        {
        setting = iAppList->FindItemL( aProperties );
        }
    else if( type == EBookmark )
        {
        setting = iBkmList->FindItemL( aProperties );
        }
    
    setting.locked = SettingLockedL( aProperties );

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
// Saves menuitem to HSPS to the given shortcut index
// ---------------------------------------------------------------------------
//
void CMCSPluginSettingsModel::SaveSettingsL( const TInt& aIndex, 
                                             CMenuItem& aMenuItem )
    {

    if ( !iPluginSettings )
        {
        return;
        }
    RPointerArray<CItemMap> settingItems;
    CleanupClosePushL( settingItems );
    iPluginSettings->GetSettingsL( *iPluginId, settingItems );
    if ( aIndex >= 0 && aIndex < settingItems.Count() )
        {
        TBool exists( EFalse );
        CItemMap* itemMap = settingItems[ aIndex ];
        RPointerArray<HSPluginSettingsIf::CPropertyMap> properties;
        properties = itemMap->Properties();
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
                else
                    {
                    properties[ i ]->SetValueL( KProperValueAppl );
                    }
                }
            else if ( properties[ i ]->Name() == KProperNameUid )
                {
                TPtrC uid = aMenuItem.GetAttributeL( KMenuAttrUid, exists );
                if ( exists )
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
                TPtrC view = aMenuItem.GetAttributeL( KMenuAttrView, exists );
                if( exists )
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

                if ( exists || isFolder )
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
                if ( exists )
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
 // ETrue tells that modified settings are stored also to plugin reference
    iPluginSettings->SetSettingsL( *iPluginId, settingItems, ETrue );
    CleanupStack::Pop( &settingItems );
    settingItems.ResetAndDestroy();

    }

// ---------------------------------------------------------------------------
// Updates settings container.
// ---------------------------------------------------------------------------
//
void CMCSPluginSettingsModel::UpdateSettingsContainerL( const TDesC8& aPluginId )
{
    if (iContainer)
    {
        if (iContainer->IsChangeDialogShowing())
        {
            iContainer->CloseChangeDialog();
        }
    }

    UpdateSettingsL( aPluginId );

    if (iContainer)
    {
        iContainer->ResetCurrentListL(0);
    }
}

// ---------------------------------------------------------------------------
// From MHomeScreenSettingsObserver - handler for HSPS setting change
// ---------------------------------------------------------------------------
//
TInt CMCSPluginSettingsModel::SettingsChangedL( const TDesC8& /*aEvent*/,  
                                                const TDesC8& /*aPluginName*/,
                                                const TDesC8& /*aPluginUid*/, 
                                                const TDesC8& /*aPluginId*/ )
    {
    return KErrNone;
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
    if ( iSettings[aIndex].type == EApplication )
        {
        const TDesC& caption = iAppList->MdcaPoint( iSettings[aIndex].id );
        TPtrC line; 
        TRAP_IGNORE( line.Set( ListBoxLineL( caption, aIndex ) ) )
        return line; 
        }
    else
        {
        const TDesC& caption = iBkmList->MdcaPoint( iSettings[aIndex].id );
        TPtrC line;
        TRAP_IGNORE( line.Set( ListBoxLineL( caption, aIndex ) ) )
        return line;
        }
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
    TSettingItem setting = { KErrNotFound, EApplication, EFalse };

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
    if (aSettingIndex >= 0 && aSettingIndex < iSettings.Count())
        {
        // Old setting type is bookmark. Remove bookmark item from MCS 
        // if it was created in runtime.
        if( iSettings[aSettingIndex].type == EBookmark )
            {
                iBkmList->RemoveMenuItemL( iSettings[aSettingIndex].id );
            }

        // Old setting type is application.
        // Remove app item from MCS if it was created in runtime (mailbox).
        if ( iSettings[ aSettingIndex ].type == EApplication )
            {
                iAppList->RemoveMenuItemL( iSettings[ aSettingIndex ].id );
            }

        iSettings[aSettingIndex].id = aId;
        iSettings[aSettingIndex].type = aType;

        if ( aType == EApplication )
            {
            CMenuItem& item = iAppList->ItemL( aId );
            SaveSettingsL( aSettingIndex, item );
            }
        else
            {
            CMenuItem& item = iBkmList->ItemL( aId );
            SaveSettingsL( aSettingIndex, item );
            }

        return ETrue;
        }
    return EFalse;
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
void CMCSPluginSettingsModel::UpdateAppListL()
{
    iAppList->StartL();
}

// ---------------------------------------------------------------------------
// Updates bookmark list
// ---------------------------------------------------------------------------
//
void CMCSPluginSettingsModel::UpdateBkmListL()
{
    delete iBkmList;
    iBkmList = NULL;
    iBkmList = CMCSPluginSettingsBkmList::NewL();
}

// End of File.
