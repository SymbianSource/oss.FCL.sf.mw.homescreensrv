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
    iNotificationsArray.Close( );
    delete iStorage;
    }

// ---------------------------------------------------------------------------
// 
// ---------------------------------------------------------------------------
//
CCPDataManager* CCPDataManager::NewL( TBool aBool )
    {
    CCPDataManager* self = CCPDataManager::NewLC( aBool );
    CleanupStack::Pop( self );
    return self;
    }

// ---------------------------------------------------------------------------
// 
// ---------------------------------------------------------------------------
//
CCPDataManager* CCPDataManager::NewLC( TBool aBool )
    {
    CCPDataManager* self = new( ELeave ) CCPDataManager;
    CleanupStack::PushL( self );
    self->ConstructL( aBool );
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
void CCPDataManager::ConstructL( TBool aBool )
    {
    CP_DEBUG( _L8("CCPDataManager::ConstructL()") );
    if ( !aBool )
        {
        iStorage = StorageFactory::NewDatabaseL( );
        }
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
TInt CCPDataManager::GetActionL( const CCPLiwMap& aMap,
    CLiwGenericParamList& aOutParamList, CLiwDefaultList* aNotificationList )
    {
    CP_DEBUG( _L8("CCPDataManager::GetActionL()") );
    __ASSERT_DEBUG( iStorage , User::Panic( _L("cpserver"), 0 ) );

    CLiwGenericParamList* paramList = CLiwGenericParamList::NewLC( );
    TInt result( KErrNone );
    TRAP( result, iStorage->GetListL( &aMap, *paramList ) );
    RBuf8 actionTrigger;
    actionTrigger.CleanupClosePushL();
    
    if ( aMap.GetPropertyL( KActionTrigger, actionTrigger ) )
        {
        const TLiwGenericParam* param;
    	TInt pos( 0);
    	
        param = paramList->FindFirst( pos, KListMap );
        //at least one param should be valid to preceed
        if ( !param || pos == KErrNotFound )
        	{
        	BuildDefaultChangeInfoL(&aMap, aNotificationList);
        	}
        
        //iteration through paramList items
    	while ( param && pos != KErrNotFound )
    		{
    		BuildChangeInfoL( &aMap, param, aNotificationList );
    		FillActionParamListL( aOutParamList, param, actionTrigger );
    		param = paramList->FindNext( pos, KListMap );
    		}
        }
    else
        {
        User::Leave( KErrNotFound );
        }
    CleanupStack::PopAndDestroy( &actionTrigger );
    CleanupStack::PopAndDestroy( paramList );
    return result;
    }


// ---------------------------------------------------------------------------
// 
// ---------------------------------------------------------------------------
//
void CCPDataManager::FillActionParamListL( 
		CLiwGenericParamList& aOutParamList, 
		const TLiwGenericParam* aParam,
		RBuf8 & aActionTrigger)
	{
    CP_DEBUG( _L8("CCPDataManager::FillActionParamListL()") );
    __ASSERT_DEBUG( iStorage , User::Panic( _L("cpserver"), 0 ) );
		
	RBuf8 actionBinaries;
	actionBinaries.CleanupClosePushL();
	ExtractActionL(  aParam, actionBinaries );

	RDesReadStream str(actionBinaries);
	CleanupClosePushL( str );
	CLiwDefaultMap* actionMap = CLiwDefaultMap::NewLC( str );
	ExtractTriggerL( aOutParamList, actionMap, aActionTrigger );

	CleanupStack::PopAndDestroy( actionMap );
	CleanupStack::PopAndDestroy( &str );
	CleanupStack::PopAndDestroy( &actionBinaries );
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
        }
    else
        {
        User::Leave( KErrNotFound );
        }

    TLiwGenericParam result( KListMap, TLiwVariant( targetMap ));
    aOutParamList.AppendL( result );
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
    __ASSERT_DEBUG( index >= 0 , User::Panic( _L("cpserver"), 0 ) );
    iNotificationsArray.Remove( index );
    if ( iNotificationsArray.Count( ) == 0 )
        {
        iStorage->SetCallback( 0 );
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
void CCPDataManager::OpenDatabaseL()
    {
    CP_DEBUG( _L8("CCPDataManager::OpenDatabaseL()") );
    if ( !iStorage )
        {
        iStorage = StorageFactory::NewDatabaseL( );
        
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
	CLiwDefaultMap* map = CLiwDefaultMap::NewLC( );
	if ( !aParam->Value().Get( *map ) )
		{
		User::Leave( KErrArgument );
		}
	
	TLiwVariant variant; variant.PushL( );
	TPtrC8 tempBuf( KNullDesC8 );
	if ( map->FindL( KActionMap, variant ) 
			&& variant.Get( tempBuf ) )
		{
		aAction.CreateL( tempBuf );
		}
	else
		{
		User::Leave( KErrArgument );
		}
	
	CleanupStack::PopAndDestroy( &variant );
	CleanupStack::PopAndDestroy( map );
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
		    CLiwDefaultMap *map = CLiwDefaultMap::NewLC();
   			variant.Get( *map );
   			if (map->FindL( KFlag, variant ))
       		    {
   	    		TUint flag;
   		    	variant.Get( flag );
   			   	result = flag & EActivate;
       			}
   	    	CleanupStack::PopAndDestroy( map );
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
		const TLiwGenericParam* aParam,	
		CLiwDefaultList* aChangeInfoList )
	{
	TLiwVariant resultVar = aParam->Value();
	resultVar.PushL();
	CLiwDefaultMap* changeInfoMap = CLiwDefaultMap::NewLC(); 
	CLiwDefaultMap* resultMap = CLiwDefaultMap::NewLC(); 
	resultVar.Get( *resultMap );
	
	CopyVariantL(KId, resultMap, changeInfoMap );
	CopyVariantL(KPublisherId, resultMap, changeInfoMap );
	CopyVariantL(KContentType, resultMap, changeInfoMap );
	CopyVariantL(KContentId, resultMap, changeInfoMap );
	CopyVariantL(KFlag, resultMap, changeInfoMap );
	CopyVariantL(KType, aMap, changeInfoMap );
	CopyVariantL(KActionTrigger, aMap, changeInfoMap );
	CopyActionTrigger16L( aMap, changeInfoMap );
	
	changeInfoMap->InsertL( KOperation, TLiwVariant( KOperationExecute ) );
	
	aChangeInfoList->AppendL( TLiwVariant( changeInfoMap ) );
	CleanupStack::PopAndDestroy( resultMap );
	CleanupStack::PopAndDestroy( changeInfoMap );
	CleanupStack::PopAndDestroy( &resultVar );
	}


// -----------------------------------------------------------------------------
// 
// -----------------------------------------------------------------------------
//    
void CCPDataManager::BuildDefaultChangeInfoL( const CCPLiwMap* aMap, 
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
	CopyActionTrigger16L( aMap, changeInfoMap );
	
	changeInfoMap->InsertL( KOperation, TLiwVariant( KOperationExecute ) );
	
	aChangeInfoList->AppendL( TLiwVariant( changeInfoMap ) );
	CleanupStack::PopAndDestroy( changeInfoMap );
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

		TPtrC8 result8( KNullDesC8 );
		if( variant.Get( result8 ) )
			{
			RBuf actionTrigger;
			actionTrigger.CleanupClosePushL();
			actionTrigger.Assign( 
					EscapeUtils::ConvertToUnicodeFromUtf8L( result8 ) );

			variant.Reset();
			variant.Set( actionTrigger );
			CleanupStack::PopAndDestroy( &actionTrigger );
			}
		
		aOutMap->InsertL( KActionTrigger16(), variant );
		}
	CleanupStack::PopAndDestroy( &variant );
	}

