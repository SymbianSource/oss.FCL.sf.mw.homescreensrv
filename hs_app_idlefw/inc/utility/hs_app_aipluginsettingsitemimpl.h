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


#ifndef C_AIPLUGINSETTINGSITEMIMPL_H
#define C_AIPLUGINSETTINGSITEMIMPL_H

#include "aipluginsettings.h"

/**
 * Implementation of Active Idle framework settings item interface.
 *
 * @lib ?library
 * @since S60 3.2
 */
NONSHARABLE_CLASS(CAiPluginSettingsItem) : public CBase, public MAiPluginSettingsItem,
    public MAiPluginContentItem, public MAiPluginConfigurationItem 
    {
public:   // Constructors and destructor

    /**
     * Constructor.
     */
    CAiPluginSettingsItem();

    /**
     *
     */
    void ConstructL();

    /**
     * Destructor.
     */
    ~CAiPluginSettingsItem();

public:  // From MAiPluginSettingsItem

    /**
     * Get a pointer to the descriptor value of the setting.
     *
     * @return returns the value of the setting item.
     *
     * @since S60 3.2
     */
    TPtrC Value();

    /**
     * Sets the setting value.
     *
     * @param aValue       the descriptor value for the Setting.
     * @param aSaveToStore (this parameter is ignored)
     *
     * @since S60 3.2
     */
    void SetValueL(const TDesC& aValue, TBool aSaveToStore = ETrue);

    /**
     * Get publisher uid this setting item belongs to.
     *
     * @return publisher uid.
     *
     * @since S60 3.2
     */
    const TUid& PublisherId() const;

    /**
     * Set publisher uid this setting item belongs to.
     *
     * @param aUid publisher uid.
     *
     * @since S60 3.2
     */
    void SetPublisherId(const TUid& aUid);

    /**
     * Get setting item key.
     *
     * @return key value.
     *
     * @since S60 3.2
     */
    TInt32 Key() const;

    /**
     * Set setting item key.
     *
     * @param aKey setting item key.
     *
     * @since S60 3.2
     */
    void SetKey(TInt32 aKey);

    /**
     * Set storer for this setting item.
     *
     * @param aStorer pointer to instance of storer interface.
     *
     * @since S60 3.2
     */
    void SetStorer(MAiPluginSettingsStorer* aStorer);

    /**
     * Reads this setting item value from settings store.
     *
     * @since S60 3.2
     */
    virtual void ReadFromStoreL();

    /**
     * Saves this setting item value to settings store.
     *
     * @since S60 3.2
     */
    virtual void SaveToStoreL();

public: //From MAiPluginContentItem
    
    TPtrC Name();

    void SetNameL( const TDesC& aName );

    TPtrC Type();

    void SetTypeL( const TDesC& aType );
    
public: //From MAiPluginConfigurationItem

    TPtrC Owner();
    
    void SetOwnerL( const TDesC& aOwner );
    
    void SetValueL( const TDesC& aValue );

private:     // Data

    // Setting value.
    HBufC*                      iValue;

    // Uid of publisher plugin this setting item belongs to.
    TUid                        iPublisherId;

    // Setting item key.
    TInt32                      iKey;

    // Settings storer.
    MAiPluginSettingsStorer*    iStorer;

    HBufC*                      iName;
    
    HBufC*                      iType;
    
    HBufC*                      iOwner;
    };

#endif // C_AIPLUGINSETTINGSITEMIMPL_H

