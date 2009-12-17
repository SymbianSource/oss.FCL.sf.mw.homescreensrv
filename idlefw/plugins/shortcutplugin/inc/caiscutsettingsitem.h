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
* Description:  Class for shortcut setting items.
*
*/


#ifndef CAISCUTSETTINGSITEM_H
#define CAISCUTSETTINGSITEM_H

#include <e32base.h>
#include "aiscutsettings.hrh"

class CAiScutSettingsModel;
class CRepository;

/**
 * Line format for the settings list box
 */
_LIT(KSettingListboxLineFormat, " \t%S\t\t%S");

/**
 * Implements a shortcut setting item
 *
 * @since S60 v3.2
 */
class CAiScutSettingsItem : public CBase
{

public:

    /**
     * Constructs a new settings item leaving it on the cleanup stack
     *
     * @since S60 v3.2
     * @param aModel Reference to the plug-in model
     * @param aIndex Index of the setting in the setting list
     * @param aKey Setting item key
     * @param aValue Setting item value
     */
    static CAiScutSettingsItem* NewLC(
        CAiScutSettingsModel&   aModel,
        TInt                    aIndex,
        TUint32                 aKey);

    static CAiScutSettingsItem* NewLC(
        CAiScutSettingsModel&   aModel,
        TInt                    aIndex,
        TUint32                 aKey,
        const TDesC&            aValue);

    /**
     * Get item value from CRepository
     */
    void ReadL(CRepository* aRepository);

    /**
     * set item value into CRepository
     */
    TInt Save(CRepository* aRepository);

    /**
     *
     */
    virtual ~CAiScutSettingsItem();

    /**
     * Returns the setting item key
     *
     * @since S60 v3.2
     * @return Setting item key
     */
    inline TUint32 Key() const;

    /**
     * Returns the target shortcut uid
     *
     * @since S60 v3.2
     * @return Target shortcut uid
     */
    inline TUid Uid() const;

    /**
     * Returns the setting item type
     *
     * @since S60 v3.2
     * @return Setting item type
     */
    inline TAiScutSettingType Type() const;

    /**
     * Returns the setting item value
     *
     * @since S60 v3.2
     * @return Setting item value
     */
    TPtrC Value() const;

    /**
     * Returns the formatted listbox line descriptor
     *
     * @since S60 v3.2
     * @return Listbox line descriptor
     */
     TPtrC ListBoxLine() const;

    /**
     * Returns the setting item title
     *
     * @since S60 v3.2
     * @return setting item title
     */
    inline TPtrC Title() const;

    /**
     * Changes the setting item target application.
     * Can be used to change the setting item type to application setting
     *
     * @since S60 v3.2
     * @param aUid Target application uid
     * @param aParams Target application parameters, if any
     * @param aCaption Target application caption
     */
    void ChangeApplicationL(TUid aUid, const TDesC& aParams, const TDesC& aCaption);

    /**
     * Changes the setting item target bookmark.
     * Can be used to change the setting item type to bookmark setting
     *
     * @since S60 v3.2
     * @param aParams Target bookmark parameters
     * @param aCaption Target bookmark caption
     */
    void ChangeBookmarkL(const TDesC& aParams, const TDesC& aCaption);

    /**
     * Changes the setting item target url
     * Can be used to change the setting item type to an url setting
     *
     * @since S60 v3.2
     * @param aUrl Target url
     */
    void ChangeUrlL(const TDesC& aUrl);

private:

    /**
     *
     */
    CAiScutSettingsItem(
        CAiScutSettingsModel&   aModel,
        TInt                    aIndex,
        TUint32                 aKey);

    /**
     *
     */
    void ConstructL();

    /**
     *
     */
    void ConstructL(const TDesC& aValue);

    /**
     *
     */
    TInt ParseValueL(const TDesC& aValue);

    /**
     * Creates a formatted listbox line
     *
     * @since S60 v3.2
     * @param aCaption Caption to use in the listbox line
     */
    void CreateListBoxLineL(const TDesC& aCaption);

    /**
     * Creates a setting item title
     * Leaves the created descriptor on cleanup stack
     *
     * @since S60 v3.2
     * @return Key title
     */
    HBufC* CreateItemTitleLC();

    /**
     * Creates a key title for an optionally visible shortcut.
     * Leaves the created descriptor on cleanup stack
     *
     * @since S60 v3.2
     * @return Key title
     */
    HBufC* CreateOptionallyVisibleKeyTitleLC();

    /**
     * Creates a key title for a non-visible shortcut.
     * Leaves the created descriptor on cleanup stack
     *
     * @since S60 v3.2
     * @return Key title
     */
    //    HBufC* CreateNonVisibleKeyTitleLC();

protected:  // data

    /**
     * Reference to the plug-in model
     */
    CAiScutSettingsModel&   iModel;

    /**
     * Setting item index in the setting list
     */
    TInt                    iIndex;

    /**
     * Setting item key
     */
    TUint32                 iKey;

    /**
     * Setting item value
     * Own.
     */
    HBufC*                  iValue;

    /**
     * Target application uid
     */
    TUid                    iUid;

    /**
     * Setting item type
     */
    TAiScutSettingType      iType;

    /**
     * Formatted listbox line
     * Own.
     */
    HBufC*                  iListBoxLine;

    /**
     * Setting item title. Points to the iListBoxLine buffer
     */
    TPtrC                   iTitle;

};

#include "caiscutsettingsitem.inl"

#endif // CAISCUTSETTINGSITEM_H

// End of File.
