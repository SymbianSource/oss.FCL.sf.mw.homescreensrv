/*
* Copyright (c) 2005 Nokia Corporation and/or its subsidiary(-ies).
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


#include "hs_app_aipluginsettingsimpl.h"
#include "hs_app_aipluginsettingsitemimpl.h"

// ============================================================================
// Class CAiPluginSettings
// ============================================================================

CAiPluginSettings* CAiPluginSettings::NewL()
    {
    CAiPluginSettings* self = new (ELeave) CAiPluginSettings();

    CleanupStack::PushL(self);
    self->ConstructL();
    CleanupStack::Pop(self);

    return self;
    }

MAiPluginSettingsItem& CAiPluginSettings::AiPluginSettingsItem()
    {
    iItemType = 0;
    return *iSettingsItem;
    }

void CAiPluginSettings::ConstructL()
    {
    iSettingsItem = new (ELeave) CAiPluginSettingsItem();
    iSettingsItem->ConstructL();
    }

CAiPluginSettings::CAiPluginSettings(): iItemType( 0 )
    {
    }

CAiPluginSettings::~CAiPluginSettings()
    {
    delete iSettingsItem;
    }

TInt CAiPluginSettings::AiPluginItemType()
    {
    return iItemType;
    }

MAiPluginContentItem& CAiPluginSettings::AiPluginContentItem()
    {
    iItemType = 1;
    return *iSettingsItem;
    }

MAiPluginConfigurationItem& CAiPluginSettings::AiPluginConfigurationItem()
    {
    iItemType = 2;
    return *iSettingsItem;
    }

// ============================================================================
// Class CAiPluginSettingsItem
// ============================================================================

CAiPluginSettingsItem::CAiPluginSettingsItem()
  : iKey( KErrNotFound )
    {
    }

void CAiPluginSettingsItem::ConstructL()
    {
    iValue = HBufC::NewL(KMaxSettingsValueStringLength);
    }

CAiPluginSettingsItem::~CAiPluginSettingsItem()
    {
    delete iValue;
    delete iName;
    delete iType;
    delete iOwner;
    }

TPtrC CAiPluginSettingsItem::Value()
    {
    if (iValue)
        {
        return TPtrC(*iValue);
        }
    else
        {
        return TPtrC();
        }
    }

void CAiPluginSettingsItem::SetValueL(const TDesC& aValue, TBool /*aSaveToStore*/)
    {
  
    delete iValue;
    iValue = NULL;
    iValue = aValue.AllocL();
    }

const TUid& CAiPluginSettingsItem::PublisherId() const
    {
    return iPublisherId;
    }

void CAiPluginSettingsItem::SetPublisherId(const TUid& aUid)
    {
    iPublisherId = aUid;
    }

TInt32 CAiPluginSettingsItem::Key() const
    {
    return iKey;
    }

void CAiPluginSettingsItem::SetKey(TInt32 aKey)
    {
    iKey = aKey;
    }

void CAiPluginSettingsItem::SetStorer(MAiPluginSettingsStorer* aStorer)
    {
    iStorer = aStorer;
    }

void CAiPluginSettingsItem::ReadFromStoreL()
    {
    if (iStorer)
        {
        TPtr ptr = iValue->Des();
        iStorer->ReadL(iKey, ptr);
        }
    }

void CAiPluginSettingsItem::SaveToStoreL()
    {
    if (iStorer)
        {
        iStorer->SaveL(iKey, *iValue);
        }
    }


void CAiPluginSettingsItem::SetValueL( const TDesC& aValue )
    {
    delete iValue;
    iValue = NULL;
    iValue = aValue.AllocL();
    }

void CAiPluginSettingsItem::SetNameL( const TDesC& aName )
    {
    delete iName;
    iName = NULL;
    iName = aName.AllocL();
    }

void CAiPluginSettingsItem::SetTypeL( const TDesC& aType )
    {
    delete iType;
    iType = NULL;
    iType = aType.AllocL();
    }

TPtrC CAiPluginSettingsItem::Name()
    {
    if ( iName )
        {
        return TPtrC( *iName );
        }
    else
        {
        return TPtrC();
        }
    }

TPtrC CAiPluginSettingsItem::Type()
    {
    if ( iType )
        {
        return TPtrC( *iType );
        }
    else
        {
        return TPtrC();
        }
    }

TPtrC CAiPluginSettingsItem::Owner()
    {
    if ( iOwner )
        {
        return TPtrC( *iOwner );
        }
    else
        {
        return TPtrC();
        }
    }
    
void CAiPluginSettingsItem::SetOwnerL( const TDesC& aOwner )
    {
    delete iOwner;
    iOwner = NULL;
    iOwner = aOwner.AllocL();
    }
