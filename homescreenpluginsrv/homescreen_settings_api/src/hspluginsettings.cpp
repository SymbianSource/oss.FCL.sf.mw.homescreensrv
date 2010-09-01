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



#include <liwservicehandler.h>
#include <liwvariant.h>
#include <bamdesca.h>

#include <hspluginsettings.h>
#include "hspsconfiguration.h"
#include "itemmap.h"
#include "propertymap.h"
#include <mhomescreensettingsif.h>
#include <mhomescreensettingsobserver.h>

_LIT8( KHSPS, "Service.HSPS" );
_LIT8( KHSPSConfigurationIf, "IConfiguration" );

_LIT8( KHSPSCommandGetPluginConf, "GetPluginConf" );
_LIT8( KHSPSSetPluginSettings, "SetPluginSettings" );
_LIT8( KPluginConfKey, "pluginConf" );

_LIT8( KOutKeyStatus ,"status");
_LIT8( KHspsAppUid, "appUid" );
_LIT8( KKeyPluginId, "pluginId" );

_LIT8( KKeyStoringParams, "storingParams" );

_LIT8( KKeyStorePluginRefence, "storePluginConf" );
_LIT8( KKeyStoreAppConf, "storeAppConf" );

_LIT8( KKeyItemId, "itemId" );
_LIT8( KKeyName, "name" );
_LIT8( KKeyValue, "value" );
_LIT8( KKeyProperties, "properties" );
_LIT8( KKeySettings, "settings" );

_LIT8( KRequestNotification, "RequestNotification" );

_LIT8( KSettingsChanged, "PluginSettingsChanged" );

namespace HSPluginSettingsIf{

NONSHARABLE_CLASS( CTlsEntry ): public CBase
    {
public:
    /**
     * Instance to home screen settings.
     */
    CHomescreenSettings* iInstance;
    
    /**
     * Reference count.
     */
    TInt iRefCount;
    };

// ---------------------------------------------------------------------------
// ---------------------------------------------------------------------------
//
EXPORT_C CHomescreenSettings* CHomescreenSettings::Instance()
    {
    CHomescreenSettings* instance = NULL;
    
    CTlsEntry* entry = static_cast<CTlsEntry*>( Dll::Tls() );
    if( entry )
        {
        instance = entry->iInstance;
        }
    
    return instance;
    }

// ---------------------------------------------------------------------------
// ---------------------------------------------------------------------------
//
EXPORT_C void CHomescreenSettings::InitializeL( const TDesC8& aAppUid )
    {
    CTlsEntry* entry = static_cast<CTlsEntry*>( Dll::Tls() );
    
    if( !entry )
        {
        entry = new (ELeave) CTlsEntry();
        entry->iInstance = NULL;
        entry->iRefCount = 1;
        
        CleanupStack::PushL( entry );
        entry->iInstance = CHomescreenSettings::NewL( aAppUid );
        CleanupStack::Pop( entry );                                
        
        Dll::SetTls( entry );
        }
    else
        {
        entry->iRefCount++;
        }
    }

// ---------------------------------------------------------------------------
// ---------------------------------------------------------------------------
//
EXPORT_C void CHomescreenSettings::UnInitialize()
    {
    CTlsEntry* entry = static_cast<CTlsEntry*>( Dll::Tls() );
    
    if( !entry )
        {
        return;
        }

    entry->iRefCount--;
    
    if( entry->iRefCount == 0 )
        {
        delete entry->iInstance;
        entry->iInstance = NULL;
        delete entry;
        Dll::SetTls( NULL );
        }
    }

// ---------------------------------------------------------------------------
// ---------------------------------------------------------------------------
//
EXPORT_C void CHomescreenSettings::AddObserverL( MHomeScreenSettingsObserver* aObserver )
    {
    if( iObservers.Find( aObserver ) == KErrNotFound )
        {
        iObservers.AppendL( aObserver );
        }
    }

// ---------------------------------------------------------------------------
// ---------------------------------------------------------------------------
//
EXPORT_C void CHomescreenSettings::RemoveObserver( MHomeScreenSettingsObserver* aObserver )
    {
    const TInt index = iObservers.Find( aObserver );
    if( index != KErrNotFound )
        {
        iObservers.Remove( index );
        }
    }

// ---------------------------------------------------------------------------
// ---------------------------------------------------------------------------
//
CHomescreenSettings::CHomescreenSettings()
    {
    }

// ---------------------------------------------------------------------------
// ---------------------------------------------------------------------------
//
void CHomescreenSettings::ConstructL( const TDesC8& aAppUid )
    {
    // Attach to HSPS:
    iServiceHandler = CLiwServiceHandler::NewL();
    
    iHspsService = CLiwCriteriaItem::NewL( 1, KHSPSConfigurationIf, KHSPS );
    iHspsService->SetServiceClass( TUid::Uid( KLiwClassBase ) );
    
    RCriteriaArray interestList;
    interestList.AppendL( iHspsService );
    iServiceHandler->AttachL( interestList );
    interestList.Reset();
    
    CLiwGenericParamList& inParamList = iServiceHandler->InParamListL();
    CLiwGenericParamList& outParamList = iServiceHandler->OutParamListL();
    
    TLiwGenericParam appUid;
    TLiwVariant uidVar;
    uidVar.Set( aAppUid );
    appUid.SetNameAndValueL( KHspsAppUid, uidVar );
    appUid.PushL();
    inParamList.AppendL( appUid );
    CleanupStack::Pop();
    appUid.Reset();
    
    iServiceHandler->ExecuteServiceCmdL( *iHspsService,inParamList,outParamList );
    inParamList.Reset();
    TInt pos = 0;
    outParamList.FindFirst( pos, KHSPSConfigurationIf );
    if( pos != KErrNotFound )
        {
        iHspsInterface = outParamList[ pos ].Value().AsInterface();    
        outParamList.Reset();        
        }
    else
        {
        outParamList.Reset();       
        User::Leave( KErrNotFound );
        }   
    
    inParamList.Reset();
    outParamList.Reset();
        
    iTransactionId = -1;
    iHspsInterface->ExecuteCmdL( KRequestNotification,
                                 inParamList,
                                 outParamList,
                                 KLiwOptASyncronous,
                                 this );
    
    const TLiwGenericParam* outParam( NULL );
            
    pos = 0;
    outParam = outParamList.FindFirst( pos, _L8("status") );
           
    if( outParam )
        {
        TInt retval;
        retval = outParam->Value().AsTInt32();
        if( retval == KErrNone )
            {
            pos = 0;
            outParam = outParamList.FindFirst( pos, _L8( "TransactionID" ) );
            if( outParam )
                {
                retval = outParam->Value().AsTInt32();
                iTransactionId = retval;
                }
            }      
        }
    }

// ---------------------------------------------------------------------------
// ---------------------------------------------------------------------------
//
CHomescreenSettings* CHomescreenSettings::NewL(
    const TDesC8& aAppUid )
    {
    CHomescreenSettings* self = CHomescreenSettings::NewLC( aAppUid );
    CleanupStack::Pop( self );
    return self;
    }

// ---------------------------------------------------------------------------
// ---------------------------------------------------------------------------
//
CHomescreenSettings* CHomescreenSettings::NewLC(
    const TDesC8& aAppUid )
    {    
    CHomescreenSettings* self = new( ELeave ) CHomescreenSettings();
    CleanupStack::PushL( self );
    self->ConstructL( aAppUid );   
    
    return self;
    }

// ---------------------------------------------------------------------------
// ---------------------------------------------------------------------------
//
CHomescreenSettings::~CHomescreenSettings()
    {
    iObservers.Reset();
    
    if( iHspsInterface )
        {
        // Close interface
        iHspsInterface->Close();       
        }
    
    if( iServiceHandler && iHspsService )
        {
        // Detach services from the handler
        RCriteriaArray interestList;
        TRAP_IGNORE( interestList.AppendL( iHspsService ) );
        TRAP_IGNORE( iServiceHandler->DetachL( interestList ) );   
        interestList.Reset();        
        }
    
    delete iHspsService;
    delete iServiceHandler;    
    }

// ---------------------------------------------------------------------------
// ---------------------------------------------------------------------------
//
TInt CHomescreenSettings::SetPluginSettingsL(
    const TDesC8& aPluginId, 
    const RPointerArray<CItemMap>& aSettings,
    const TDesC8& aStoringParam )
    {
    // Compose AddPlugin hsps LIW message to Service.HomeScreenPluginConfiguration
    CLiwGenericParamList& inParamList = iServiceHandler->InParamListL();
    CLiwGenericParamList& outParamList = iServiceHandler->OutParamListL();
    
    TLiwGenericParam pluginIdParam;
    pluginIdParam.SetNameAndValueL( KKeyPluginId, TLiwVariant(aPluginId) );
    pluginIdParam.PushL();
    inParamList.AppendL( pluginIdParam );
    CleanupStack::Pop( &pluginIdParam );
    pluginIdParam.Reset();    
       
    CLiwDefaultList* itemMapList = CLiwDefaultList::NewLC();
    for( int i = 0; i < aSettings.Count(); i++ )
        {
        CItemMap* itemMap = aSettings[i];
        
        CLiwDefaultMap* inItemMap = CLiwDefaultMap::NewLC();
        FillMapFromItemL(*inItemMap,*itemMap);
        itemMapList->AppendL( TLiwVariant(inItemMap) );
        CleanupStack::Pop(inItemMap);
        inItemMap->Close();
        }
       
    TLiwGenericParam settingsParam;
    settingsParam.SetNameAndValueL( KKeySettings, TLiwVariant(itemMapList) );
    settingsParam.PushL();
    inParamList.AppendL( settingsParam );
    CleanupStack::Pop( &settingsParam );
    settingsParam.Reset();
    
    TLiwGenericParam storingParams;
    storingParams.SetNameAndValueL( KKeyStoringParams, TLiwVariant(aStoringParam) );
    storingParams.PushL();
    inParamList.AppendL( storingParams );
    CleanupStack::Pop( &storingParams );
    storingParams.Reset();
    
    iHspsInterface->ExecuteCmdL( KHSPSSetPluginSettings, 
                                 inParamList, 
                                 outParamList ); 
    
    CleanupStack::PopAndDestroy( itemMapList );

    inParamList.Reset();
    
      
    // check success
    const TLiwGenericParam* outParam = NULL;
    TInt pos(0);
    outParam = outParamList.FindFirst( pos, KOutKeyStatus );
    TInt status(KErrGeneral);
    
    if ( outParam )
        {
        status = outParam->Value().AsTInt32();
        }
    outParamList.Reset();
    return status;
    }
// ---------------------------------------------------------------------------
// ---------------------------------------------------------------------------
//
EXPORT_C TInt CHomescreenSettings::SetSettingsL(
    const TDesC8& aPluginId, 
    const RPointerArray<CItemMap>& aSettings,
    const TBool aStoringParams)
    {
    TInt error = KErrNone;
    
    if( aStoringParams )
    	{
    	error = SetPluginSettingsL(aPluginId, aSettings, KKeyStorePluginRefence());
    	}
    else
    	{
    	error = SetPluginSettingsL(aPluginId, aSettings, KKeyStoreAppConf());	
    	}
   
    return error;
    }
// ---------------------------------------------------------------------------
// ---------------------------------------------------------------------------
//
EXPORT_C TInt CHomescreenSettings::GetSettingsL(
    const TDesC8& aPluginId, 
     RPointerArray<CItemMap>& aSettings)
    {
    TInt error = KErrNone;
    CHspsConfiguration* configuration = CHspsConfiguration::NewLC();
    
    // Compose AddPlugin hsps LIW message to Service.HomeScreenPluginConfiguration
    CLiwGenericParamList& inParamList = iServiceHandler->InParamListL();
    CLiwGenericParamList& outParamList = iServiceHandler->OutParamListL();
    
    TLiwGenericParam pluginIdParam;
    pluginIdParam.SetNameAndValueL( KKeyPluginId, TLiwVariant(aPluginId) );
    pluginIdParam.PushL();
    inParamList.AppendL( pluginIdParam );
    CleanupStack::Pop( &pluginIdParam );
    pluginIdParam.Reset();    
    
    
    iHspsInterface->ExecuteCmdL( KHSPSCommandGetPluginConf, 
                                     inParamList, 
                                     outParamList );
    
    inParamList.Reset();
        
    TInt index(0);
    const TLiwGenericParam* conf = outParamList.FindFirst(index,KPluginConfKey);
     
    if(conf)
        {
        const CLiwMap* confMap  = conf->Value().AsMap();
            // 2: Process Configuration map
        if(confMap)
            {
            TLiwVariant tempVariant;
            tempVariant.PushL();
            
            if( confMap->FindL( _L8("settings"), tempVariant ))
                {
                const CLiwList* settings_list( tempVariant.AsList() );
                if( settings_list )
                    {
                    aSettings.Reset();
                    ProcessConfigurationSettingsL(*settings_list,*configuration);
                    RPointerArray<CItemMap>& settings = configuration->Settings();
                    CItemMap* itemMapIn(0);
                    CPropertyMap* readProperty(0);
                    CPropertyMap* propertyIn(0); 
                    
                    for( TInt i = 0; i < settings.Count(); i++ )
                        {
                        CItemMap* readItem = settings[i];
                        itemMapIn = CItemMap::NewLC(); 
                        itemMapIn->SetItemIdL(readItem->ItemId());
                        const TDesC8& itemName = readItem->ItemName();
                        itemMapIn->SetItemNameL(itemName);
                                
                        RPointerArray<CPropertyMap>& readProperties = readItem->Properties();
                                
                        for( TInt j=0; j < readProperties.Count(); j++ )
                            {
                            readProperty = readProperties[j];       
                            propertyIn = CPropertyMap::NewLC();
                            propertyIn->SetNameL(readProperty->Name());
                            propertyIn->SetValueL(readProperty->Value());
                            // Dispatching is based on item name 
                            itemMapIn->AddPropertyMapL(propertyIn);
                            CleanupStack::Pop(propertyIn);
                            }
                                
                        aSettings.AppendL(itemMapIn);
                                
                        CleanupStack::Pop(itemMapIn);
                        }
                    }
                else
                    {
                    error=KErrNotFound;
                    }
                }
            else
                {
                error= KErrNotFound;
                }
            
            
            CleanupStack::Pop(&tempVariant);
            tempVariant.Reset();
            }
        }

    outParamList.Reset();
    CleanupStack::PopAndDestroy(configuration);
    
    return error;
    }
        


// ---------------------------------------------------------------------------
// ---------------------------------------------------------------------------
//
void CHomescreenSettings::ProcessConfigurationSettingsL(
    const CLiwList& aItemList, 
    CHspsConfiguration& aTarget)
    {
    TLiwVariant itemMapVariant;
    itemMapVariant.PushL();
    for( int i=0;i<aItemList.Count();++i )
        {
        if( aItemList.AtL(i,itemMapVariant) )
            {
            const CLiwMap* itemMap( itemMapVariant.AsMap() );
            TLiwVariant itemVariant;
            itemVariant.PushL();
            if( itemMap )
                {
                CItemMap* item = CItemMap::NewLC();
                if( itemMap->FindL( _L8("itemId"),itemVariant) )
                    {
                    item->SetItemIdL( itemVariant.AsData() );
                    }
                if( itemMap->FindL( _L8("name"),itemVariant) )
                    {
                    item->SetItemNameL( itemVariant.AsData() );
                    }
                if( itemMap->FindL( _L8("properties"),itemVariant) )
                    {
                    const CLiwList* properties( itemVariant.AsList() );
                    if(properties)
                        {
                        ProcessConfItemPropertiesL(*properties,*item);
                        }
                    }
                    
                aTarget.AddItemMapL(item);                    
                CleanupStack::Pop(item);
                }
            CleanupStack::Pop(&itemVariant);
            itemVariant.Reset();
            }
       
        }
    CleanupStack::Pop(&itemMapVariant);
    itemMapVariant.Reset();
    }


// ---------------------------------------------------------------------------
// ---------------------------------------------------------------------------
//
void CHomescreenSettings::ProcessConfItemPropertiesL(
    const CLiwList& aPropertyMapList,
    CItemMap& aItemMap)
    {
    TLiwVariant propertyMapVariant;
    propertyMapVariant.PushL();
    for( int i=0;i<aPropertyMapList.Count();++i )
       {
       if( aPropertyMapList.AtL(i,propertyMapVariant) )
           {
           const CLiwMap* propertyMap( propertyMapVariant.AsMap() );
           TLiwVariant propertyVariant;
           propertyVariant.PushL();
           if( propertyMap )
               {
               CPropertyMap* property = CPropertyMap::NewLC();
               if( propertyMap->FindL( _L8("name"),propertyVariant ) )
                   {
                   property->SetNameL( propertyVariant.AsData() );
                   }
               if( propertyMap->FindL( _L8("value"),propertyVariant ) )
                   {
                   property->SetValueL( propertyVariant.AsData() );
                   }
               aItemMap.AddPropertyMapL(property);                    
               CleanupStack::Pop(property);
               }
           CleanupStack::Pop(&propertyVariant);
           propertyVariant.Reset();
           }
      
       }
    CleanupStack::Pop(&propertyMapVariant);
    propertyMapVariant.Reset();
    }
    


// ---------------------------------------------------------------------------
// ---------------------------------------------------------------------------
//
void CHomescreenSettings::FillMapFromItemL( CLiwDefaultMap& aMap, const CItemMap& aItemMap )
    {
    aMap.InsertL(  KKeyItemId, TLiwVariant(aItemMap.ItemId()) );
    aMap.InsertL(  KKeyName, TLiwVariant(aItemMap.ItemName()) );
    
    CLiwDefaultList* inPropertyMapList = CLiwDefaultList::NewLC();
    FillMapFromPropertiesL(*inPropertyMapList,aItemMap.Properties());
    aMap.InsertL(  KKeyProperties, TLiwVariant(inPropertyMapList) );
    CleanupStack::Pop(inPropertyMapList);
    inPropertyMapList->Close();
    }

// ---------------------------------------------------------------------------
// ---------------------------------------------------------------------------
//
void CHomescreenSettings::FillMapFromPropertiesL( 
    CLiwDefaultList& aInPropertyMapList, 
    const RPointerArray<CPropertyMap>& aProperties )
    {
    
    for(int i=0; i<aProperties.Count(); ++i)
        {
        CLiwDefaultMap* inPropertyMap = CLiwDefaultMap::NewLC();
        inPropertyMap->InsertL(  KKeyName, TLiwVariant(aProperties[i]->Name()) );
        inPropertyMap->InsertL(  KKeyValue, TLiwVariant(aProperties[i]->Value()) );
        aInPropertyMapList.AppendL(inPropertyMap);
        CleanupStack::Pop(inPropertyMap);
        inPropertyMap->Close();
        }
    }

// ---------------------------------------------------------------------------
// ---------------------------------------------------------------------------
//    
TInt CHomescreenSettings::HandleNotifyL( TInt aCmdId, TInt aEventId,                        
    CLiwGenericParamList& aEventParamList,
    const CLiwGenericParamList& /*aInParamList*/ )
    {
    TInt retval( KErrNone );
   
    if( iTransactionId == aCmdId && iObservers.Count() > 0 )
        {                 
        const TLiwGenericParam* outParam( NULL );
        
        TInt pos( 0 );
        outParam = aEventParamList.FindFirst( pos, _L8("status") );
        
        if ( outParam )
            {    
            retval = outParam->Value().AsTInt32();
            }
        else
            {   
            pos = 0;
            
            retval = KErrNotFound;
                
            outParam = aEventParamList.FindFirst( pos, _L8("notification") );
            
            if( !outParam )
                {      
                // should never happen, but change status back to KErrNotFound 
                return retval;                
                }
                
            HBufC8* event( NULL );
            HBufC8* pluginUid( NULL );
            HBufC8* pluginName( NULL ); 
            TInt pushCount( 0 );
                                                                
            TLiwVariant variant;
            variant.PushL();
            pushCount++;
                
            variant = outParam->Value();
            
            const CLiwMap* notifMap( variant.AsMap() );
                                          
            if ( notifMap->FindL( _L8("event"), variant ) )
                {
                event = variant.AsData().AllocLC();
                pushCount++;
                }    
               
            variant.Reset();    
            
            if ( event && event->Des().Compare( KSettingsChanged ) == 0 )
                {
                if( notifMap->FindL( _L8("name"), variant ) )
                    {
                    pluginName = variant.AsData().AllocLC();            
                    pushCount++;
                    }
                            
                variant.Reset();
                            
                if( notifMap->FindL( _L8("pluginUid"), variant ) )
                    {                
                    pluginUid = variant.AsData().AllocLC();
                    pushCount++;
                    }        
                                
                           
                variant.Reset();
                
                if( notifMap->FindL( _L8("pluginIds"), variant ) )
                    { 
                    const CLiwList* pluginIdList( variant.AsList() );
                    
                    variant.Reset();
                    
                    TInt count( pluginIdList->Count() );
                    
                    retval = KErrNone;
                                            
                    for( TInt i = 0; i < count && retval == KErrNone; i++ )
                        {           
                        pluginIdList->AtL( i, variant );
                        HBufC8* pluginId( NULL );
                        pluginId = variant.AsData().AllocLC();
                        
                        for( TInt i = 0; i < iObservers.Count(); i++ )
                            {
                            iObservers[i]->SettingsChangedL( 
                                ( event ) ? *event : KNullDesC8(),  
                                ( pluginName ) ? *pluginName : KNullDesC8(), 
                                ( pluginUid ) ? *pluginUid : KNullDesC8(), 
                                ( pluginId ) ? *pluginId : KNullDesC8() );
                            }
       
                        CleanupStack::PopAndDestroy( pluginId );                                                      
                        
                        variant.Reset();                         
                        }                        
                    }
            
                }
            
            CleanupStack::PopAndDestroy( pushCount );
            }
        }
        
    if( retval != KErrNone && aEventId != KLiwEventInProgress)
        {
        CLiwGenericParamList& inParamList = iServiceHandler->InParamListL();
        CLiwGenericParamList& outParamList = iServiceHandler->OutParamListL();
        
        inParamList.Reset();
        outParamList.Reset();
        //cancel old notification request
        iHspsInterface->ExecuteCmdL( KRequestNotification,
                                     inParamList,
                                     outParamList,
                                     KLiwOptCancel,
                                     this ); 
        
        inParamList.Reset();
        outParamList.Reset();
        //request notification again
        iTransactionId = -1;
        iHspsInterface->ExecuteCmdL( KRequestNotification,
                                     inParamList,
                                     outParamList,
                                     KLiwOptASyncronous,
                                     this );
        
        const TLiwGenericParam* outParam( NULL );
                        
        TInt pos( 0 );
        outParam = outParamList.FindFirst( pos, _L8("status") );
                       
        if ( outParam )
            {
            TInt retval;
            retval = outParam->Value().AsTInt32();
            
            if(retval == KErrNone )
                {
                pos = 0;
                outParam = outParamList.FindFirst( pos, _L8("TransactionID") );
                       
                if( outParam )
                    {
                    retval = outParam->Value().AsTInt32();
                    iTransactionId = retval;
                    }
                }     
            }
        
        }   
    
    return retval;    
    }

}
//End of file
