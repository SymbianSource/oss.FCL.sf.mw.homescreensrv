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
* Description:  
 *
*/


#include <liwcommon.h>
#include <s32mem.h>
#include <escapeutils.h>

#include "cpserverdatamanager.h"
#include "ccontentmap.h"
#include "cpublisherregistrymap.h"
#include "cpstorage.h"
#include "cpdebug.h"
#include "cpstoragefactory.h"
#include "cpglobals.h"
#include "cpserversession.h"
#include "cpnotificationhandler.h"

using namespace LIW;

// ---------------------------------------------------------------------------
// C++ destructor
// ---------------------------------------------------------------------------
//
CCPDataManager::~CCPDataManager()
    {
    delete iActionDataCache;
    iNotificationsArray.Close( );
    delete iStorage;
    }

// ---------------------------------------------------------------------------
// 
// ---------------------------------------------------------------------------
//
CCPDataManager* CCPDataManager::NewL(TLiwVariant& aDataMapCache, TBool aBool)
    {
    CCPDataManager* self = CCPDataManager::NewLC( aDataMapCache, aBool );
    CleanupStack::Pop( self );
    return self;
    }

// ---------------------------------------------------------------------------
// 
// ---------------------------------------------------------------------------
//
CCPDataManager* CCPDataManager::NewLC(TLiwVariant& aDataMapCache, TBool aBool)
    {
    CCPDataManager* self = new( ELeave ) CCPDataManager();
    CleanupStack::PushL( self );
    self->ConstructL( aDataMapCache, aBool );
    return self;
    }

// ---------------------------------------------------------------------------
// C++ constructor.
// ---------------------------------------------------------------------------
//
CCPDataManager::CCPDataManager() 
    {

    }

// ---------------------------------------------------------------------------
// Standard 2nd phase constructor.
// ---------------------------------------------------------------------------
//
void CCPDataManager::ConstructL(TLiwVariant& aDataMapCache, TBool aBool)
    {
    CP_DEBUG( _L8("CCPDataManager::ConstructL()") );
    if ( !aBool )
        {
        iStorage = StorageFactory::NewDatabaseL( aDataMapCache );
        }
    iActionDataCache = CCPActionDataCache::NewL();
    }

// ---------------------------------------------------------------------------
// 
// ---------------------------------------------------------------------------
//
TUint CCPDataManager::AddDataL( CCPLiwMap& aMap )
    {
    CP_DEBUG( _L8("CCPDataManager::AddDataL()") );
    __ASSERT_DEBUG( iStorage , User::Panic( _L("cpserver"), 0 ) );

    CCPLiwMap* getlistmap = CreateMapForGetlistLC( aMap );

    iActionDataCache->HandleCacheRemoveL( &aMap );
    
    TBool activateMap = aMap.GetActivateInfo();
    TBool activateDB ( EFalse );
    TBool activateSupport =aMap.ActivateActionSupport( );
   	
   	if( activateSupport )
	    {
	    activateDB = GetActivateInfoL( getlistmap );
	    }   
  
    TInt id = iStorage->AddL( &aMap );

    if( activateSupport )
        {
        if ( activateMap && !activateDB )
            {
            // append action trigger for activate
            aMap.InsertL( KActionTrigger, TLiwVariant( KActivateTrigger ) ); 
            }
        if ( !activateMap && activateDB )
            {
            // append action trigger for deactivate
            aMap.InsertL( KActionTrigger, TLiwVariant( KDeactivateTrigger ) ); 
            }
        //remove flag from map ( needed because map will be used in getlist 
        //operation )                
        aMap.Remove( KFlag );  
        }
     
    CleanupStack::PopAndDestroy( getlistmap );

    return id;
    }

// ---------------------------------------------------------------------------
//
// ---------------------------------------------------------------------------
//
void CCPDataManager::AddNonPersistentDataL( const CCPLiwMap* aMap )
    {
    CLiwDefaultList* notificationList = CLiwDefaultList::NewLC( );
    BuildChangeInfoForAddL( aMap, notificationList );
    HandleChangeL( notificationList );
    CleanupStack::PopAndDestroy( notificationList );
    }

// ---------------------------------------------------------------------------
//
// ---------------------------------------------------------------------------
//
void CCPDataManager::GetListL( const CCPLiwMap& aMap,
    CLiwGenericParamList& aOutParamList )
    {
    CP_DEBUG( _L8("CCPDataManager::GetListL()") );
    __ASSERT_DEBUG( iStorage , User::Panic( _L("cpserver"), 0 ) );
    iStorage->GetListL( &aMap, aOutParamList );
    }
// ---------------------------------------------------------------------------
// 
// ---------------------------------------------------------------------------
//
TInt CCPDataManager::GetActionsL( const CCPLiwMap& aMap, TBool aEnableCache,
    CLiwGenericParamList& aOutParamList, CLiwDefaultList* aNotificationList )
    {
    CP_DEBUG( _L8("CCPDataManager::GetActionL()") );
    __ASSERT_DEBUG( iStorage , User::Panic( _L("cpserver"), 0 ) );

    CLiwGenericParamList* paramList = CLiwGenericParamList::NewLC( );
    TInt result( KErrNone );
    
    CLiwDefaultList* actionTriggers = aMap.GetActionTriggersLC();
    
    if ( actionTriggers )
        {
        TBool cacheable = iActionDataCache->IsCacheableL( &aMap );
        if (aEnableCache && cacheable && iActionDataCache->ExistL( &aMap ))
            {
            iActionDataCache->GetL( &aMap, paramList );
            }
        else 
            {
            TRAP( result, iStorage->GetListL( &aMap, *paramList ) );
            if ( aEnableCache && cacheable )
                {
                if ( result == KErrNone )
                    {   
                    iActionDataCache->AppendL(paramList);
                    }
                else if ( result == KErrNotFound )
                    {
                    iActionDataCache->AppendEmptyL(&aMap);
                    }
                }
            }
        
        const TLiwGenericParam* param;
    	TInt pos( 0);
    	
        param = paramList->FindFirst( pos, KListMap );
        //at least one param should be valid to preceed
        if ( !param || pos == KErrNotFound )
        	{
        	BuildDefaultChangeInfoL(&aMap, actionTriggers, aNotificationList);
        	}
        
        //iteration through paramList items
    	while ( param && pos != KErrNotFound )
    		{
    	    RBuf8 actionBinaries;
    	    actionBinaries.CleanupClosePushL();
    	    ExtractActionL( param, actionBinaries );
    	    RDesReadStream str(actionBinaries);
    	    CleanupClosePushL( str );
    	    CLiwDefaultMap* actionMap = CLiwDefaultMap::NewLC( str );
            BuildChangeInfoL( &aMap, actionTriggers, param, actionMap,
                    aNotificationList );
            FillActionParamListL( aOutParamList, actionMap, actionTriggers );
            param = paramList->FindNext( pos, KListMap );
            CleanupStack::PopAndDestroy( actionMap );
            CleanupStack::PopAndDestroy( &str );
            CleanupStack::PopAndDestroy( &actionBinaries );
    		}
        if (aNotificationList->Count() == 0)
            {
            User::Leave(KErrNotFound);
            }
        }
    else
        {
        User::Leave( KErrNotFound );
        }
    
    CleanupStack::PopAndDestroy( actionTriggers );
    CleanupStack::PopAndDestroy( paramList );
    return result;
    }


// ---------------------------------------------------------------------------
// 
// ---------------------------------------------------------------------------
//
void CCPDataManager::FillActionParamListL( 
		CLiwGenericParamList& aOutParamList, 
		const CLiwDefaultMap* aActionMap,
		const CLiwDefaultList* aActionTriggers)
	{
    CP_DEBUG( _L8("CCPDataManager::FillActionParamListL()") );
    __ASSERT_DEBUG( iStorage , User::Panic( _L("cpserver"), 0 ) );
    TLiwVariant trigger;
    trigger.PushL();
    TInt count = aActionTriggers->Count();
	for ( TInt i = 0; i<count; i++ )
	    {
	    trigger.Reset();
	    aActionTriggers->AtL( i,trigger );
	    ExtractTriggerL( aOutParamList, aActionMap, trigger.AsData());
	    }
    CleanupStack::PopAndDestroy(&trigger);
	}

// ---------------------------------------------------------------------------
// 
// ---------------------------------------------------------------------------
//
void CCPDataManager::GetActivePublishersL( CLiwGenericParamList* aResultList )
    {
    CCPLiwMap *map = CPublisherRegistryMap::NewLC();
    map->InsertL( KFlag, TLiwVariant( TUint( 0 | EActivate ) ) );
    iStorage->GetListL( map, *aResultList );
    CleanupStack::PopAndDestroy(map);
    }


// ---------------------------------------------------------------------------
// 
// ---------------------------------------------------------------------------
//
void CCPDataManager::ExtractTriggerL( CLiwGenericParamList& aOutParamList,
    const CLiwDefaultMap* aMap, const TDesC8& aTrigger )
    {
    CP_DEBUG( _L8("CCPDataManager::GetActionL()") );
    CLiwDefaultMap* targetMap = CLiwDefaultMap::NewLC( );
    TLiwVariant variant;
    variant.PushL( );
    if ( aMap->FindL( aTrigger, variant ) )
        {
        targetMap->InsertL( KActionMap, variant );
        TLiwGenericParam result( KListMap, TLiwVariant( targetMap ));
        aOutParamList.AppendL( result );
        }
    CleanupStack::PopAndDestroy( &variant );
    CleanupStack::PopAndDestroy( targetMap );
    }

// ---------------------------------------------------------------------------
// 
// ---------------------------------------------------------------------------
//
void CCPDataManager::RemoveDataL( const CCPLiwMap& aMap )
    {
    CP_DEBUG( _L8("CCPDataManager::RemoveData()") );
    __ASSERT_DEBUG( iStorage , User::Panic( _L("cpserver"), 0 ) );
    iActionDataCache->HandleCacheRemoveL( &aMap );
    iStorage->RemoveL( &aMap );
    }

// ---------------------------------------------------------------------------
//
// ---------------------------------------------------------------------------
//
void CCPDataManager::AddObserverL(
    CCPNotificationHandler* aNotificationHandler )
    {
    CP_DEBUG( _L8("CCPDataManager::AddObserverL()") );
    __ASSERT_DEBUG( iStorage , User::Panic( _L("cpserver"), 0 ) );
    iNotificationsArray.AppendL( aNotificationHandler );
    if ( iNotificationsArray.Count( ) == 1 )
        {
        iStorage->SetCallback( this );
        }
    }

// ---------------------------------------------------------------------------
//
// ---------------------------------------------------------------------------
//
void CCPDataManager::RemoveObserver(
    CCPNotificationHandler* aNotificationHandler )
    {
    CP_DEBUG( _L8("CCPDataManager::RemoveObserver()") );
    TInt index = iNotificationsArray.Find( aNotificationHandler );
    if (index != KErrNotFound)
        {
        iNotificationsArray.Remove( index );
        if ( iNotificationsArray.Count( ) == 0 )
            {
            iStorage->SetCallback( 0 );
            }
        }
    }

// ---------------------------------------------------------------------------
//
// ---------------------------------------------------------------------------
//
void CCPDataManager::HandleChangeL( CLiwDefaultList* aListOfMaps )
    {
    CP_DEBUG( _L8("CCPDataManager::HandleChangeL()") );
    //for every session
    for ( TInt i = 0; i<iNotificationsArray.Count( ); i++ )
        {
        TRAPD(error, SendNotificationL(aListOfMaps, i));
        if ( error )
            {
            iNotificationsArray[i]->ErrorComplete( error );
            }
        }
    }

// ---------------------------------------------------------------------------
//
// ---------------------------------------------------------------------------
//
void CCPDataManager::CloseDatabase()
    {
    CP_DEBUG( _L8("CCPDataManager::CloseDatabase()") );
    delete iStorage;
    iStorage = NULL;
    }

// ---------------------------------------------------------------------------
//
// ---------------------------------------------------------------------------
//
void CCPDataManager::OpenDatabaseL(TLiwVariant& aDataMapCache)
    {
    CP_DEBUG( _L8("CCPDataManager::OpenDatabaseL()") );
    if ( !iStorage )
        {
        iStorage = StorageFactory::NewDatabaseL( aDataMapCache );
        
        // Restore storage observers
        if ( iNotificationsArray.Count() > 0 )
            {
            iStorage->SetCallback( this );
            }
        }
    }
// ---------------------------------------------------------------------------
//
// ---------------------------------------------------------------------------
//
void CCPDataManager::SendNotificationL( CLiwDefaultList* aListOfMaps,
    TInt aIndex )
    {
    CP_DEBUG( _L8("CCPDataManager::SendNotificationL()") );
    iNotificationsArray[aIndex]->SendNotificationL( aListOfMaps );
    }

// ---------------------------------------------------------------------------
//
// ---------------------------------------------------------------------------
//
void CCPDataManager::ExtractActionL( const TLiwGenericParam* aParam, RBuf8& aAction )
    {
    CP_DEBUG( _L8("CCPDataManager::ExtractActionL()") );
    if ( aParam->Value().TypeId() != EVariantTypeMap )
		{
		User::Leave( KErrArgument );
		}
	
	TLiwVariant variant; variant.PushL( );
	TPtrC8 tempBuf( KNullDesC8 );
	if ( aParam->Value().AsMap()->FindL( KActionMap, variant ) 
			&& variant.Get( tempBuf ) )
		{
		aAction.CreateL( tempBuf );
		}
	else
		{
		User::Leave( KErrArgument );
		}
	
	CleanupStack::PopAndDestroy( &variant );
    }

// -----------------------------------------------------------------------------
// 
// -----------------------------------------------------------------------------
//    
CCPLiwMap* CCPDataManager::CreateMapForGetlistLC( const CCPLiwMap& aMap )
    {
    CCPLiwMap *resultmap = CPublisherRegistryMap::NewLC();
    RBuf publisher;
    publisher.CleanupClosePushL();
    RBuf content_type;
    content_type.CleanupClosePushL();
    RBuf content_id;
    content_id.CleanupClosePushL();
    aMap.GetPropertyL( KContentType, content_type );
    aMap.GetPropertyL( KContentId, content_id ); 
    aMap.GetPropertyL( KPublisherId, publisher );
    resultmap->InsertL( KPublisherId, TLiwVariant( publisher ));
    resultmap->InsertL( KContentId, TLiwVariant( content_id ));
    resultmap->InsertL( KContentType, TLiwVariant( content_type ));
    CleanupStack::PopAndDestroy( &content_id );
    CleanupStack::PopAndDestroy( &content_type );
    CleanupStack::PopAndDestroy( &publisher );
    return resultmap;
    }

// -----------------------------------------------------------------------------
// 
// -----------------------------------------------------------------------------
//    
TBool CCPDataManager::GetActivateInfoL( const CCPLiwMap* aMap )
    {
    TBool result( EFalse );
    CLiwGenericParamList* outList = CLiwGenericParamList::NewLC();
	TRAPD(err,GetListL(*aMap,*outList));
	if (err == KErrNone)
	    {
	    TInt pos( 0 );
        outList->FindFirst( pos, KListMap );
        if( pos != KErrNotFound )
       	    {
   	    	TLiwVariant variant = (*outList)[pos].Value();
   	        if ( variant.TypeId() == EVariantTypeMap )
   	        	{
   	            if (variant.AsMap()->FindL( KFlag, variant ))
       	            {
   	    		    TUint flag;
   		    	    variant.Get( flag );
   			   	    result = flag & EActivate;
       			    }
   	            }
  		    variant.Reset();
   		    }
	    }
   	CleanupStack::PopAndDestroy( outList );
   	return result; 
    }	

// -----------------------------------------------------------------------------
// 
// -----------------------------------------------------------------------------
//    
void CCPDataManager::BuildChangeInfoL( const CCPLiwMap* aMap, 
        const CLiwDefaultList* aActionTriggers,
		const TLiwGenericParam* aParam,	
		const CLiwDefaultMap* aActionMap,
		CLiwDefaultList* aChangeInfoList )
	{
	TLiwVariant resultVar = aParam->Value();
    resultVar.PushL();
	if ( resultVar.TypeId() == EVariantTypeMap )
        {
        for ( TInt i = 0; i<aActionTriggers->Count(); i++ )
            {
            TLiwVariant dummyVariant;
            dummyVariant.PushL();
            TLiwVariant trigger;
            trigger.PushL();
            aActionTriggers->AtL(i,trigger);
            if (aActionMap->FindL( trigger.AsData(), dummyVariant ))
                {
                CLiwDefaultMap* changeInfoMap = CLiwDefaultMap::NewLC();
                CopyVariantL(KId, resultVar.AsMap(), changeInfoMap);
                CopyVariantL(KPublisherId, resultVar.AsMap(), changeInfoMap);
                CopyVariantL(KContentType, resultVar.AsMap(), changeInfoMap);
                CopyVariantL(KContentId, resultVar.AsMap(), changeInfoMap);
                CopyVariantL(KFlag, resultVar.AsMap(), changeInfoMap);
                CopyVariantL(KType, aMap, changeInfoMap);
                changeInfoMap->InsertL(KActionTrigger, trigger);
                CopyActionTrigger16L(trigger, changeInfoMap);
                changeInfoMap->InsertL(KOperation, TLiwVariant(
                        KOperationExecute));
                aChangeInfoList->AppendL(TLiwVariant(changeInfoMap));
                CleanupStack::PopAndDestroy(changeInfoMap);
                }
            CleanupStack::PopAndDestroy(&trigger);
            CleanupStack::PopAndDestroy(&dummyVariant);
            }
        }
    CleanupStack::PopAndDestroy( &resultVar );
	}

// -----------------------------------------------------------------------------
// 
// -----------------------------------------------------------------------------
//    
void CCPDataManager::BuildDefaultChangeInfoL( const CCPLiwMap* aMap, 
        const CLiwDefaultList* aActionTriggers,
		CLiwDefaultList* aChangeInfoList )
	{
	for ( TInt i = 0; i<aActionTriggers->Count(); i++ )
	    {
	    CLiwDefaultMap* changeInfoMap = CLiwDefaultMap::NewLC(); 
	
	    CopyVariantL(KId, aMap, changeInfoMap );
        CopyVariantL(KPublisherId, aMap, changeInfoMap );
        CopyVariantL(KContentType, aMap, changeInfoMap );
        CopyVariantL(KContentId, aMap, changeInfoMap );
        CopyVariantL(KFlag, aMap, changeInfoMap );
        CopyVariantL(KType, aMap, changeInfoMap );
        
        TLiwVariant trigger;
        trigger.PushL();
        aActionTriggers->AtL(i,trigger);
        changeInfoMap->InsertL(KActionTrigger, trigger);
        CopyActionTrigger16L(trigger,changeInfoMap);
        CleanupStack::PopAndDestroy(&trigger);
        
        changeInfoMap->InsertL( KOperation, TLiwVariant( KOperationExecute ) );
        
        aChangeInfoList->AppendL( TLiwVariant( changeInfoMap ) );
        CleanupStack::PopAndDestroy( changeInfoMap );
	    }
	}

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
//
void CCPDataManager::BuildChangeInfoForAddL( const CCPLiwMap* aMap,
        CLiwDefaultList* aChangeInfoList )
    {
    CLiwDefaultMap* changeInfoMap = CLiwDefaultMap::NewLC();

    CopyVariantL(KId, aMap, changeInfoMap );
    CopyVariantL(KPublisherId, aMap, changeInfoMap );
    CopyVariantL(KContentType, aMap, changeInfoMap );
    CopyVariantL(KContentId, aMap, changeInfoMap );
    CopyVariantL(KFlag, aMap, changeInfoMap );
    CopyVariantL(KType, aMap, changeInfoMap );
    CopyVariantL(KActionTrigger, aMap, changeInfoMap );
    CopyVariantL(KDataMap, aMap, changeInfoMap );
    CopyActionTrigger16L( aMap, changeInfoMap );

    changeInfoMap->InsertL( KOperation, TLiwVariant( KOperationAdd ) );

    aChangeInfoList->AppendL( TLiwVariant( changeInfoMap ) );
    CleanupStack::PopAndDestroy( changeInfoMap );
    }

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
//
void CCPDataManager::CopyVariantL(const TDesC8& aKey,
		const CLiwMap* aInMap, CLiwDefaultMap* aOutMap )
	{
	TLiwVariant variant;
	variant.PushL();
	if ( aInMap->FindL( aKey, variant ) )
		{
		aOutMap->InsertL( aKey, variant );
		}
	CleanupStack::PopAndDestroy( &variant );
	}

// -----------------------------------------------------------------------------
// 
// -----------------------------------------------------------------------------
//   
void CCPDataManager::CopyActionTrigger16L( 
		const CLiwMap* aInMap, CLiwDefaultMap* aOutMap )
	{
	TLiwVariant variant;
	variant.PushL();
	if ( aInMap->FindL( KActionTrigger(), variant ) )
		{
		CopyActionTrigger16L(variant,aOutMap);
		}
	CleanupStack::PopAndDestroy( &variant );
	}

// -----------------------------------------------------------------------------
// 
// -----------------------------------------------------------------------------
//   
void CCPDataManager::CopyActionTrigger16L( const TLiwVariant& aVariant, 
        CLiwDefaultMap* aOutMap )
    {
    if( aVariant.TypeId() == EVariantTypeDesC8 )
        {
        TLiwVariant variant;
        variant.PushL();
        RBuf desc16;
        desc16.CleanupClosePushL();
        desc16.Assign( 
                EscapeUtils::ConvertToUnicodeFromUtf8L( aVariant.AsData()) );
        variant.Set( desc16 );
        aOutMap->InsertL(KActionTrigger16(),variant);
        CleanupStack::PopAndDestroy( &desc16 );
        CleanupStack::PopAndDestroy( &variant );
        }
    }

