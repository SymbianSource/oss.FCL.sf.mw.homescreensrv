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
NONSHARABLE_CLASS( CHomescreenSettings ) :
    public CBase,
    public MLiwNotifyCallback,
    public MHomescreenSettingsIf
    {
public: // static methods
    /**
     * Get instance to settings API.
     * 
     * InitializeL must be called before calling this method.
     * Otherwise NULL is returned. InitializeL and Instance
     * must be called within same thread since TLS is used
     * to storage instance data.
     * 
     * @return CHomescreenSettings* Pointer to settings api.
     *                              Can return NULL in case of
     *                              error. 
     */
    IMPORT_C static CHomescreenSettings* Instance();

    /**
     * Initialize settings api.
     *
     * There must be one UnInitialize call for each Initialize call
     * in order to prevent memory leaking.
     * (Implementation contains reference counting)
     */
    IMPORT_C static void InitializeL( const TDesC8& aAppUid );

    /**
     * Uninitialize settings api.
     *
     * There must be one UnInitialize call for each Initialize call
     * in order to prevent memory leaking.
     * (Implementation contains reference counting)
     */
    IMPORT_C static void UnInitialize();    

public:
    /**
     * Add observer
     * 
     * @param aObserver     Observer to be added.
     */
    IMPORT_C void AddObserverL( MHomeScreenSettingsObserver* aObserver );        
    
    /**
     * Remove observer
     * 
     * @param aObserver     Observer to be removed.
     */    
    IMPORT_C void RemoveObserver( MHomeScreenSettingsObserver* aObserver );        
    
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
     * Two-phased constructor.
     * 
     * @param aAppUid Application uid in integer format
     */
    static CHomescreenSettings* NewL( const TDesC8& aAppUid );
    
    /**
     * Two-phased constructor.
     * 
     * @param aAppUid Application uid in integer format
     */
    static CHomescreenSettings* NewLC(
        const TDesC8& aAppUid );
    
    /**
    * Destructor.
    */
    virtual ~CHomescreenSettings(); 
    
    /**
     * Constructor.
     */
    CHomescreenSettings();
    
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

    /*
     * Asynchronous service request tarnsaction id
     */ 
    TInt iTransactionId;
    
    /*
     * List of observers. Items not owned!
     */ 
    RPointerArray<MHomeScreenSettingsObserver> iObservers;
    };

} //namespace HSPluginSettingsIf

#endif // C_CHomeScreenSettingsIf_H
