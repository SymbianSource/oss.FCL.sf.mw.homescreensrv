/*
* Copyright (c) 2008 Nokia Corporation and/or its subsidiary(-ies).
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
* Description:  Encapsulates hsps liw service
*
*/
#ifndef C_CHomeScreenSettingsIf_H
#define C_CHomeScreenSettingsIf_H

#include <e32base.h>
#include <liwcommon.h>      // for MLiwNotifyCallback
#include <mhomescreensettingsif.h>
#include <mhomescreensettingsobserver.h>

class CLiwServiceHandler;
class MLiwInterface;
class CLiwCriteriaItem;
class CLiwMap;
class CLiwList;
class MDesC8Array;
class CLiwDefaultMap;
class CLiwDefaultList;
class MLiwNotifyCallback;
class CLiwGenericParamList;


namespace HSPluginSettingsIf{

class CHspsConfiguration;
class CItemMap;
class CPropertyMap;
class MHomescreenSettingsObserver;

/**
 *  Wrapper for hsps liw service
 *  
 *
 *  @code
 *   
 *  @endcode
 *
 *  @lib HomeScreenSettingsIf.lib
 *  @since S60 v5.0
 */
class CHomescreenSettings:
    public CBase,
    public MLiwNotifyCallback,
    public MHomescreenSettingsIf
    {
public:
    /**
     * Two-phased constructor.
     * 
     * @param aAppUid Application uid in integer format
     * @param aObserver Observer
     */
    IMPORT_C static CHomescreenSettings* NewL(
        const TDesC8& aAppUid,
        const TDesC8& aPluginId,
        MHomeScreenSettingsObserver* aObserver );
    
    /**
     * Two-phased constructor.
     * 
     * @param aAppUid Application uid in integer format
     * @param aObserver Observer
     */
    IMPORT_C static CHomescreenSettings* NewLC(
        const TDesC8& aAppUid,
        const TDesC8& aPluginId,
        MHomeScreenSettingsObserver* aObserver );
    
    /**
    * Destructor.
    */
    IMPORT_C virtual ~CHomescreenSettings();

public: 
    /**
     * From MHomescreenSettingsIf
     * Stores defined plugin’s settings. 
     * Plugin is identified with unique plugin ID used in application’s
     * configuration 
     *
     * @param aPluginId Pluging’s ID which settings are stored
     * @param aSettings List of item maps
     * @return Operation status. KErrNone (success), KErrNotFound
     */
    IMPORT_C TInt GetSettingsL(
        const TDesC8& aPluginId,
        RPointerArray<CItemMap>& aSettings );
    
    /**
     * From MHomescreenSettingsIf
     * Stores defined plugin’s settings. 
     * Plugin is identified with unique plugin ID used in application’s 
     * configuration 
     *
     * @param aPluginId Pluging’s ID which settings are stored
     * @param aSettings List of item maps
     * @param aStoringParam parameter to tell whether plugin configuration need to be saved also if parameter is ETrue.
     * @return Operation status. KErrNone (success), KErrNotFound
     */
    IMPORT_C TInt SetSettingsL(
        const TDesC8& aPluginId,
        const RPointerArray<CItemMap>& aSettings,
        const TBool aStoringParam );
        

protected:
    /**
     * Constructor.
     * 
     * @param aAppUid Application uid in integer format
     * @param aObserver Observer
     */
    CHomescreenSettings(
        MHomeScreenSettingsObserver* aObserver,
        const TDesC8& aPluginId );
    
    /**
     * Second phase constructor
     * 
     * @param aAppUid Application uid in integer format
     */
    void ConstructL( const TDesC8& aAppUid );
    
    /**
     * Processes all items configuration setting
     * @param aItemList item list
     * @param aTarget  output hsps configuration
     */
    void ProcessConfigurationSettingsL(
        const CLiwList& aItemList,
        CHspsConfiguration& aTarget );
    
    /**
     * Processes all items' properties configuration
     * 
     * @param aPropertyMapList input properties
     * @param aItemMap output item
     */
    void ProcessConfItemPropertiesL(
        const CLiwList& aPropertyMapList,
        CItemMap& aItemMap );
    
    /**
     * Fills liw map from given item
     * @param aMap liw item map to fill in
     * @param aItemMap item map 
     */
    void FillMapFromItemL( CLiwDefaultMap& aMap, const CItemMap& aItemMap );
    
    /**
     * Fills liw map from given item properties
     * @param aMap liw map to fill in
     * @param aItemMap item properiteis map 
     */
    void FillMapFromPropertiesL(
        CLiwDefaultList& aInPropertyMapList,
        const RPointerArray<CPropertyMap>& aProperties );
    
    /**
     * Sets plugin settings 
     * @param aPluginId plugin id
     * @param aSettings array of items to set
     * @param aStoringParam parameter to tell whether plugin configuration need to be saved.
     * @return 
     */
    TInt SetPluginSettingsL( 
        const TDesC8& aPluginId,
        const RPointerArray<CItemMap>& aSettings,
        const TDesC8& aStoringParam );
    
    
protected:  
    /**
     * From MLiwNotifyCallback
     * Handle HSPS notifications 
     *
     * @param aCmdId Command id
     * @param aEventId Event id
     * @param aEventParamList Event parameters
     * @param aInParamList In parameters
     */
    TInt HandleNotifyL(TInt aCmdId, TInt aEventId,                        
                       CLiwGenericParamList& aEventParamList,
                       const CLiwGenericParamList& aInParamList);
    
private: // data
    /**
     * Owned. SAPI service handler.
     */
    CLiwServiceHandler* iServiceHandler;
    /**
     * Owned. HSPS SAPI service.
     */
    CLiwCriteriaItem* iHspsService;   
    /**
     * Owned. Provides hsps services.
     */
    MLiwInterface* iHspsInterface;
    /**
     * Not owned. Wrapper observer
     */    
    MHomeScreenSettingsObserver* iObserver;
    /*
     * Asynchronous service request tarnsaction id
     */ 
    TInt iTransactionId;
    /**
     * Plugin id
     */
    const TDesC8& iPluginId;
    };

} //namespace HSPluginSettingsIf

#endif // C_CHomeScreenSettingsIf_H
