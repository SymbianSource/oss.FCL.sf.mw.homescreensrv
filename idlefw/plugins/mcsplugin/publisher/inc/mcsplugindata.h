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
* Description:  Plug-in data class
*
*/


#ifndef CMCSPLUGINDATA_H
#define CMCSPLUGINDATA_H

// INCLUDE FILES
#include <e32base.h>
#include <badesca.h>
#include <mhomescreensettingsobserver.h>
#include <hspluginsettings.h>
#include <propertymap.h>

class TMenuItem;
class CMCSPluginEngine;
class CMCSPluginWatcher;


/**
 *  @ingroup group_mcsplugin
 *
 *  TMCData class
 *
 *  @since S60 v9.1
 */
class TMCSData
    {
public:
    /**
     * SetMenuItem
     * 
     * @param aMenuItem
     */
    void SetMenuItem( TMenuItem& aMenuItem );

    /**
     * MenuItem
     * 
     * @return TMenuItem
     */
    TMenuItem& MenuItem();

    /**
     * SetDirty
     * 
     * @param aDirty
     * @return TBool
     */
    void SetDirty( TBool aDirty );

    /**
     * IsDirty
     * 
     * @return TBool
     */
    TBool IsDirty() const;

private:
    /**
     * iMenuItem
     */
    TMenuItem iMenuItem;

    /**
     * iDirty
     */
    TBool iDirty;
    };

/**
 *  @ingroup group_mcsplugin
 *
 *  Plug-in data class
 *
 *  @since
 */
class CMCSPluginData : public CBase,
    public HSPluginSettingsIf::MHomeScreenSettingsObserver
    {

public:

    /**
    * Part of the two phased constuction
    *
    * @param aEngine
    * @param aInstanceUid
    * @return Pointer to the created CMCSPluginData object
    */
    static CMCSPluginData* NewL( CMCSPluginEngine& aEngine,
        const TDesC8& aInstanceUid );

    /**
    * Constructor
    *
    * @param aEngine
    * @param aInstanceUid
    * @return none
    */
    CMCSPluginData( CMCSPluginEngine& aEngine, const TDesC8& aInstanceUid );

    /**
    * Destructor
    *
    * @param none
    * @return none
    */
    ~CMCSPluginData();

    /**
     * DataItem
     * 
     * @param aIndex
     * @return TMCSData&
     */
    TMCSData& DataItemL( TInt aIndex );

    /**
     * ReplaceMenuItemL
     * 
     * @param aIndex
     * @param aMenuItem
     */
    void ReplaceMenuItemL( const TInt& aIndex, TMenuItem& aMenuItem );

    /**
     * SaveSettingsL
     * 
     * @param aIndex
     * @param aMenuItem
     */
    void SaveSettingsL( const TInt& aIndex, CMenuItem& aMenuItem );

    /**
     * DataCount
     * 
     * @return TInt
     */
    TInt DataCount(){ return iData.Count();};

    /**
     * UpdateDataL
     */
    void UpdateDataL();

    // From MHomeScreenSettingsObserver
    /**
     * SettingsChangedL
     * 
     * @param aEvent
     * @param aPluginName
     * @param aPluginUid
     * @param aPluginId
     * @return TInt
     */
    TInt SettingsChangedL( const TDesC8& aEvent,  const TDesC8& aPluginName,
                           const TDesC8& aPluginUid, const TDesC8& aPluginId );

private:

    /**
    * Part of the two phased construction
    *
    * @param void
    * @return void
    */
    void ConstructL();

    /**
     * CreateMenuItemL
     * @param aProperties
     * @return TMenuItem
     */
    TMenuItem CreateMenuItemL(
        RPointerArray<HSPluginSettingsIf::CPropertyMap>& aProperties );

private: // data

    // Menu items, which are defined in settings
    // Own
    RArray<TMCSData> iData;

    // Plugin settings
    HSPluginSettingsIf::CHomescreenSettings* iPluginSettings;

    // Reference to MCS plug-in engine
    CMCSPluginEngine& iEngine;

    // Reference to instance uid of HSPS widget
    const TDesC8& iInstanceUid;
    };

#endif // CMCSPLUGINDATA_H
