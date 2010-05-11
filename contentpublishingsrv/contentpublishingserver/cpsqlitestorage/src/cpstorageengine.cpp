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


#include "cpstorageengine.h"
#include "cpliwmap.h"
#include "ccontentmap.h"
#include "cpdebug.h"
#include "cpssqlconstants.h"
#include "cpserverdef.h"
#include <centralrepository.h>

// ---------------------------------------------------------------------------
// CCpStorageEngine::CCpStorageEngine()
//
// ---------------------------------------------------------------------------
//
CCpStorageEngine::CCpStorageEngine(TLiwVariant& aDataMapCache) :
    iDataMapCache(aDataMapCache)
    {
    }

// ---------------------------------------------------------------------------
// CCpStorageEngine::ConstructL()
//
// ---------------------------------------------------------------------------
//
void CCpStorageEngine::ConstructL()
    {
#ifdef CONTENT_PUBLISHER_DEBUG
    if(!CCPDebug::Enable())
        {
        iDebug = CCPDebug::NewL( KCPDebugFileName2 );
        }
#endif
    CP_DEBUG( _L8("CCpStorageEngine::ConstructL()") );
    
    iRepository = CRepository::NewL( KServerUid );
    TBool createNew( EFalse );
    TInt err = iSqlDb.Open( KCPdatabase, &KMdsSqlDbaConfig );
    if( err == KSqlErrCorrupt )
    	{
    	DeleteCorruptedDbL();
    	createNew = ETrue;
    	}
    else if( err/* == KErrNotFound */)
    	{
    	createNew = ETrue;
    	}
    
    if ( createNew )
        {
        CreateNewDbL();
        }
    }

// ---------------------------------------------------------------------------
// CCpStorageEngine::NewL()
//
// ---------------------------------------------------------------------------
//
CCpStorageEngine* CCpStorageEngine::NewL(TLiwVariant& aDataMapCache)
    {
    CCpStorageEngine* self = CCpStorageEngine::NewLC(aDataMapCache);
    CleanupStack::Pop( self );
    return self;
    }

// ---------------------------------------------------------------------------
// CCpStorageEngine::NewLC()
//
// ---------------------------------------------------------------------------
//
CCpStorageEngine* CCpStorageEngine::NewLC(TLiwVariant& aDataMapCache)
    {
    CCpStorageEngine* self = new (ELeave) CCpStorageEngine(aDataMapCache);
    CleanupStack::PushL( self );
    self->ConstructL( );
    return self;
    }

// ---------------------------------------------------------------------------
// CCpStorageEngine::~CCpStorageEngine() 
//
// ---------------------------------------------------------------------------
//
CCpStorageEngine::~CCpStorageEngine()
    {
    CP_DEBUG( _L8("CCpStorageEngine::~CCpStorageEngine()") );
    iSqlDb.Close( );
    delete iRepository;
#ifdef CONTENT_PUBLISHER_DEBUG
    delete iDebug;
#endif
    }

// ---------------------------------------------------------------------------
// CCpStorageEngine::CreateTablesL 
//
// ---------------------------------------------------------------------------
//
void CCpStorageEngine::CreateTablesL()
    {
    CP_DEBUG( _L8("CCpStorageEngine::CreateTablesL()") );

    //Main Table
    User::LeaveIfError( iSqlDb.Exec( KSQLCreateMainTable ) );
    User::LeaveIfError( iSqlDb.Exec( KSQLIndexOnMain_Publisher ) );
    User::LeaveIfError( iSqlDb.Exec( KSQLIndexOnMain_Content_type ) );
    //Publisher Table
    User::LeaveIfError( iSqlDb.Exec( KSQLCreatePublisherTable ) );
    User::LeaveIfError( iSqlDb.Exec( KSQLIndexOnPub_Content_type ) );
    User::LeaveIfError( iSqlDb.Exec( KSQLIndexOnPub_Content_type_Content_id ) );
    }

// ---------------------------------------------------------------------------
// CCpStorageEngine::GetListL
//
// ---------------------------------------------------------------------------
//    
void CCpStorageEngine::GetListL( const CLiwMap* aMap,
    CLiwGenericParamList& aList )
    {
    CP_DEBUG( _L8("CCpStorageEngine::GetListL()") );
    const CCPLiwMap* map = static_cast<const CCPLiwMap*>(aMap);
    
    RArray<TInt32> itemsToDelete;
    CleanupClosePushL( itemsToDelete );
    
    TUint numberOfItems = map->GetListL( iSqlDb, &aList, itemsToDelete );
    //delete outdated items
    CContentMap* removeItemMap = CContentMap::NewLC( );
    for ( TInt i = 0; i<itemsToDelete.Count( ); i++ )
        {
        removeItemMap->InsertL( KId, TLiwVariant( itemsToDelete[i] ) );
        removeItemMap->InsertL( KType, TLiwVariant( KCpData ) );
        RemoveL( removeItemMap );
        removeItemMap->Remove( KId );
        }
    CleanupStack::PopAndDestroy( removeItemMap );
    CleanupStack::PopAndDestroy( &itemsToDelete );

    if ( numberOfItems == 0 )
        {
        User::Leave( KErrNotFound );
        }
    }

// ---------------------------------------------------------------------------
// CCpStorageEngine::AddL
//
// ---------------------------------------------------------------------------
//  
TInt32 CCpStorageEngine::AddL( const CLiwMap* aMap )
    {
    CP_DEBUG( _L8("CCpStorageEngine::AddL()") );
    TInt32 id(0);
    const CCPLiwMap* map = static_cast<const CCPLiwMap*>(aMap);
    if ( iCallback )
        {
        CLiwDefaultList* listOfMaps = CLiwDefaultList::NewLC( );
        id = map->AddL( iSqlDb, listOfMaps );
        iDataMapCache.Reset();
        map->FindL(KDataMap, iDataMapCache);
        iCallback->HandleChangeL( listOfMaps );
        CleanupStack::PopAndDestroy( listOfMaps ) ;
        }
    else
        {
        id = map->AddL( iSqlDb );
        }
    return id;
    }

// ---------------------------------------------------------------------------
// CCpStorageEngine::RemoveL
//
// ---------------------------------------------------------------------------
//  
void CCpStorageEngine::RemoveL( const CLiwMap* aMap )
    {
    CP_DEBUG( _L8("CCpStorageEngine::RemoveL()") );
    const CCPLiwMap* map = static_cast<const CCPLiwMap*>(aMap);
    if ( iCallback )
        {
        CLiwDefaultList* listOfMaps = CLiwDefaultList::NewLC( );
        map->RemoveL( iSqlDb, listOfMaps );
        iCallback->HandleChangeL( listOfMaps );
        CleanupStack::PopAndDestroy( listOfMaps ) ;
        }
    else
        {
        map->RemoveL( iSqlDb );
        }
    }

// ---------------------------------------------------------------------------
// CCpStorageEngine::SetCallback()
//
// ---------------------------------------------------------------------------
//  
void CCpStorageEngine::SetCallback( MCPChangeNotifier* aCallback )
    {
    iCallback = aCallback;
    }

// ---------------------------------------------------------------------------
// CCpStorageEngine::DeleteCorruptedDbL()
//
// ---------------------------------------------------------------------------
//  
void CCpStorageEngine::DeleteCorruptedDbL()
	{
	User::LeaveIfError( iSqlDb.Delete( KCPdatabase ) );
	TInt value( 0 );
	if( !iRepository->Get( KCPStorageUid, value ) )
		{
		value ^= KSQLDBStateNormal;
		value |= KSQLDBStateRestored;
		}
	else
		{
		value = KSQLDBStateRestored;
		}
	iRepository->Set( KCPStorageUid, value );
	}

// ---------------------------------------------------------------------------
// CCpStorageEngine::CreateNewDbL()
//
// ---------------------------------------------------------------------------
//  
void CCpStorageEngine::CreateNewDbL()
    {
    TSecureId id( KServerUid ); // Uid of CP Server - only this process has access rights
    TSecurityPolicy defaultPolicy( id );
    RSqlSecurityPolicy sqlSecurityPolicy;
    CleanupClosePushL( sqlSecurityPolicy );
    User::LeaveIfError( sqlSecurityPolicy.Create( defaultPolicy ) );
#ifdef CONTENT_PUBLISHER_STORAGE_MT
    iSqlDb.CreateL( KCPpathdatabase, &KMdsSqlDbaConfig );
#else
    iSqlDb.CreateL( KCPdatabase, sqlSecurityPolicy, &KMdsSqlDbaConfig );
#endif        
    CleanupStack::PopAndDestroy( &sqlSecurityPolicy );
    CreateTablesL( );
    
    TInt value( 0 );
    if( iRepository->Get( KCPStorageUid, value ) == KErrNotFound )
		{
		value = KSQLDBStateNormal;
		iRepository->Set( KCPStorageUid, value );
		}
    }

