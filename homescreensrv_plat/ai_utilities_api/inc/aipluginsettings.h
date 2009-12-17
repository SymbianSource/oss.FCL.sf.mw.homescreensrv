/*
* Copyright (c) 2005-2006 Nokia Corporation and/or its subsidiary(-ies).
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


#ifndef M_AIPLUGINSETTINGS_H
#define M_AIPLUGINSETTINGS_H

#include <e32base.h>

const TInt KMaxSettingsValueStringLength = 1024;

class MAiPluginSettings;
class MAiPluginSettingsItem;
class MAiPluginContentItem;
class MAiPluginConfigurationItem;

typedef RPointerArray<MAiPluginSettings> RAiSettingsItemArray;

enum TAiPluginItem
    {
    EAiPluginSettingsItem = 0,
    EAiPluginContentItem,
    EAiPluginConfigurationItem
    };

/**
 * Active Idle framework settings storing interface.
 *
 * @since S60 3.2
 */
class MAiPluginSettingsStorer
    {
public:
    /**
     * Saves setting item value identified by key to settings store.
     *
     * @param aKey   setting item key.
     * @param aValue setting item value.
     */
    virtual void SaveL(TInt32 aKey, const TDesC& aValue) = 0;

    /**
     * Reads setting item value identified by key from settings store.
     *
     * @param aKey   setting item key.
     * @param aValue received setting item value.
     */
    virtual void ReadL(TInt32 aKey, TDes& aValue) = 0;

    };

/**
 * Active Idle framework settings access interface.
 *
 * @since S60 3.2
 */
class MAiPluginSettings
    {
public:
    /**
     * Returns the settings item.
     */
    virtual MAiPluginSettingsItem& AiPluginSettingsItem() = 0;

    virtual ~MAiPluginSettings() { }
    
    virtual TInt AiPluginItemType() = 0;
    
    virtual MAiPluginContentItem& AiPluginContentItem() = 0;
    
    virtual MAiPluginConfigurationItem& AiPluginConfigurationItem() = 0;

    };

/**
 * Active Idle framework settings item interface.
 *
 * @since S60 3.2
 */
class MAiPluginSettingsItem
    {
public:

    /**
     * Get a pointer to the descriptor value of the setting.
     *
     * @return returns the value of the setting item.
     *
     * @since S60 3.2
     */
    virtual TPtrC Value() = 0;

    /**
     * Sets the setting value.
     *
     * @param aValue       the descriptor value for the Setting.
     * @param aSaveToStore (this parameter is ignored)
     *
     * @since S60 3.2
     */
    virtual void SetValueL(const TDesC& aValue, TBool aSaveToStore = ETrue) = 0;

    /**
     * Get publisher uid this setting item belongs to.
     *
     * @return publisher uid.
     *
     * @since S60 3.2
     */
    virtual const TUid& PublisherId() const = 0;

    /**
     * Set publisher uid this setting item belongs to.
     *
     * @param aUid publisher uid.
     *
     * @since S60 3.2
     */
    virtual void SetPublisherId(const TUid& aUid) = 0;

    /**
     * Get setting item key.
     *
     * @return key value.
     *
     * @since S60 3.2
     */
    virtual TInt32 Key() const = 0;

    /**
     * Set setting item key.
     *
     * @param aKey setting item key.
     *
     * @since S60 3.2
     */
    virtual void SetKey(TInt32 aKey) = 0;

    /**
     * Set storer for this setting item.
     *
     * @param aStorer pointer to instance of storer interface.
     *
     * @since S60 3.2
     */
    virtual void SetStorer(MAiPluginSettingsStorer* aStorer) = 0;

    /**
     * Reads this setting item value from settings store.
     *
     * @since S60 3.2
     */
    virtual void ReadFromStoreL() = 0;

    /**
     * Saves this setting item value to settings store.
     *
     * @since S60 3.2
     */
    virtual void SaveToStoreL() = 0;

protected:

    /**
     * Protected destructor prevents deletion through this interface.
     */
    ~MAiPluginSettingsItem() { }

    };


/**
 * Active Idle framework dynamic content model item interface.
 *
 * @since S60 5.0
 */
class MAiPluginContentItem
    {
public:

    /**
     * Get a pointer to the descriptor name of the content item.
     *
     * @return returns the value of the content item.
     *
     * @since S60 5.0
     */
    virtual TPtrC Name() = 0;

    /**
     * Sets the content item name.
     *
     * @param aName  the descriptor name for the content item.
     *
     * @since S60 5.0
     */
    virtual void SetNameL(const TDesC& aName) = 0;

    /**
     * Get a pointer to the content item type.
     *
     * @return returns content item type
     *
     * @since S60 5.0
     */
    virtual TPtrC Type() = 0;

    /**
     * Sets the type of the content item.
     *
     * @param aType
     *
     * @since S60 5.0
     */
    virtual void SetTypeL(const TDesC& aType) = 0;
    
protected:

    /**
     * Protected destructor prevents deletion through this interface.
     */
    ~MAiPluginContentItem() { }
    };


/**
 * Active Idle framework dynamic content source configuration interface.
 *
 * @since S60 5.0
 */
class MAiPluginConfigurationItem
    {
public:

    /**
     * Get a pointer to the configuration item owner.
     *
     * @return returns configuration item type
     *
     * @since S60 5.0
     */
    virtual TPtrC Owner() = 0;
    
    /**
     * Sets the owner of the configuration item.
     *
     * @param aOwner
     *
     * @since S60 5.0
     */
    virtual void SetOwnerL( const TDesC& aOwner ) = 0;

    /**
     * Get a pointer to the descriptor name of the configuration item.
     *
     * @return returns the name of the configuration item.
     *
     * @since S60 5.0
     */
    virtual TPtrC Name() = 0;

    /**
     * Sets the configuration item name.
     *
     * @param aName the descriptor name for the configuration item.
     *
     * @since S60 5.0
     */
    virtual void SetNameL( const TDesC& aName ) = 0;
    
    /**
     * Get a pointer to the configuration item value.
     *
     * @return returns configuration item value
     *
     * @since S60 5.0
     */
    virtual TPtrC Value() = 0;

    /**
     * Sets the value of the configuration item.
     *
     * @param aValue
     *
     * @since S60 5.0
     */
    virtual void SetValueL( const TDesC& aValue ) = 0;
    
protected:

    /**
     * Protected destructor prevents deletion through this interface.
     */
    ~MAiPluginConfigurationItem() { }
    };

#endif // M_AIPLUGINSETTINGS_H

