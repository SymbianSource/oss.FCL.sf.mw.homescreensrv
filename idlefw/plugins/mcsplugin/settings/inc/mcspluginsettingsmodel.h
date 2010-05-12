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


#ifndef CMCSPLUGINSETTINGSMODEL_H
#define CMCSPLUGINSETTINGSMODEL_H

// External includes
#include <e32base.h>
#include <bamdesca.h>               // For MDesCArray
#include <hspluginsettings.h>
#include <propertymap.h>

// Forward declaration
class CCoeEnv;
class CMenuItem;
class CMCSPluginSettingsAppList;
class CMCSPluginSettingsBkmList;
class CMCSPluginSettings;
class CMCSPluginSettingsContainer;
class HSPluginSettingsIf::CItemMap;

/**
 * TSettingType
 */
enum TSettingType
    {
    EApplication,
    EBookmark,
    EMailbox
    };

/**
 * TSettingItem
 */
struct TSettingItem
    {
    TInt id;
    TSettingType type;
    TBool locked;
    };

/**
 *  @ingroup group_mcsplugin
 *
 * MCS settings plug-in model
 *
 * @lib mcspluginsettings.lib
 * @since S60 9.1
 */
class CMCSPluginSettingsModel : public CBase
    , public MDesCArray
{

public:

    /**
     * Two phase constructor
     * 
     * @param aPlugin
     * @param aEnv
     */
    static CMCSPluginSettingsModel* NewL( CMCSPluginSettings& aPlugin,
        CCoeEnv* aEnv);

    /**
     * Descructor
     */
    virtual ~CMCSPluginSettingsModel();

public: // from MDesCArray

    /**
     * Returns the number of descriptor elements in a descriptor array.
     *
     * @since S60 v3.2
     * @return The number of descriptor elements in a descriptor array.
     */
    TInt MdcaCount() const;

    /**
     * Indexes into a descriptor array.
     *
     * @since S60 v3.2
     * @param aIndex The position of the descriptor element within 
     *               a descriptor array.
     * @return A 16 bit non-modifiable pointer descriptor representing the 
     *         descriptor element located at position aIndex within a descriptor 
     *         array.
     */
    TPtrC MdcaPoint(TInt aIndex) const;

public:

    /**
     * Returns an ID of a setting item for the given index
     *
     * @since S60 v3.2
     * @param aIndex Index of the setting item
     * @return ID of a setting item
     */
    TInt ItemId( TInt aIndex ) const;

    /**
     * Returns a pointer to setting item for the given index
     *
     * @since S60 v9.1
     * @param aIndex Index of the setting item
     * @return setting item
     */
    const TSettingItem Item( TInt aIndex ) const;

    /**
     *
     */
    TBool ReplaceItemL( const TInt& aItemIndex, TInt aValue,
        TSettingType aType );

    /**
     * Sets pointer to settings plug-in container.
     *
     * @since S60 v3.2
     * @param aContainer Pointer to settings plug-in container.
     */
    void SetContainer(CMCSPluginSettingsContainer* aContainer);


    /**
     * Get application list
     */
    CMCSPluginSettingsAppList* AppList();

    /**
     * Get bookmark list
     */
    CMCSPluginSettingsBkmList* BkmList();

    /**
     * Update application list
     */
    void UpdateAppListL( TBool aUpdateSettings = ETrue );

    /**
     * Update bookmark list
     */
    void UpdateBkmListL( TBool aUpdateSettings = ETrue );

    /**
     * Update settings container
     * 
     * @param aPluginId
     */
    void SetPluginIdL( const TDesC8& aPluginId );
    
    /**
     * Read settings from HSPS and update settings list
     */
    void UpdateSettingsL();

private:

    /**
     * Private constructor
     * 
     * @param aPlugin
     * @param aEnv
     */
    CMCSPluginSettingsModel(CMCSPluginSettings& aPlugin, CCoeEnv* aEnv);

    /**
     * Leaving constructor
     */
    void ConstructL();

    /**
     * ListBoxLine for list
     * 
     * @param aCaption
     * @param aIndex
     * @return TPtrC
     */
    TPtrC ListBoxLineL( const TDesC& aCaption, TInt aIndex ) const;

    /**
     * Returns setting item based on properties.
     * 
     * @param aProperties
     * @return TSettingItem
     */
    TSettingItem ItemL( 
        RPointerArray<HSPluginSettingsIf::CPropertyMap>& aProperties );

    /**
     * Setting type
     * 
     * @param aProperties
     * @return TSettingType
     */
    TSettingType SettingTypeL( 
       RPointerArray<HSPluginSettingsIf::CPropertyMap>& aProperties );

    /**
     * Setting locked
     * 
     * @param aProperties
     * @return TBool
     */
    TBool SettingLockedL( 
        RPointerArray<HSPluginSettingsIf::CPropertyMap>& aProperties );

    

    /**
     * Save settings into HSPS
     * 
     * @param aIndex
     * @param aMenuItem
     */
    void SaveSettingsL( const TInt& aIndex, CMenuItem& aMenuItem );

private: // data

    /**
     * Array of setting items.
     * Own.
     */
    RArray<TSettingItem> iSettings;

    /**
    * Homescreen settings API. Not owned.
    */
    HSPluginSettingsIf::CHomescreenSettings* iPluginSettings;

    /**
     * HSPS settings id.
     */
    HBufC8* iPluginId;

    /**
     * Stores the text which is drawn by listbox
     * Listbox takes only reference, own.
     */
    mutable HBufC* iListBoxLine;

    /**
     * Settings plugin
     * Not own.
     */
    CMCSPluginSettings& iPlugin;

    /**
     * Coe environment.
     * Not own.
     */
    CCoeEnv* iEnv;

    /**
     * Application list.
     * Own.
     */
    CMCSPluginSettingsAppList* iAppList;

    /**
     * Bookmark list.
     * Own.
     */
    CMCSPluginSettingsBkmList* iBkmList;

    /**
     * Settings plugin container.
     * Not own.
     */
    CMCSPluginSettingsContainer* iContainer;
};

#endif // CMCSPLUGINSETTINGSMODEL_H

// End of File.
