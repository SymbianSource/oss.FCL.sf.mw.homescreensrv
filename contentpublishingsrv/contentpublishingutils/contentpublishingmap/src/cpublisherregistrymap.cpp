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
* Description:  ?Description
 *
*/


#include <liwcommon.h>

#include "cpserverdef.h"
#include "cpublisherregistrymap.h"
#include "cpglobals.h"
#include "cpdebug.h"
#include "cpssqlcommands.h"
#include "cpsecuritypolicy.h"

// ---------------------------------------------------------------------------
// 
// ---------------------------------------------------------------------------
//

CPublisherRegistryMap::CPublisherRegistryMap()
    {

    }

// ---------------------------------------------------------------------------
// 
// ---------------------------------------------------------------------------
//
void CPublisherRegistryMap::ConstructL()
    {
    CCPLiwMap::ConstructL( );
    }

// ---------------------------------------------------------------------------
// 
// ---------------------------------------------------------------------------
//
EXPORT_C CPublisherRegistryMap* CPublisherRegistryMap::NewL( )
    {
    CPublisherRegistryMap* self = CPublisherRegistryMap::NewLC( );
    CleanupStack::Pop( self );
    return self;
    }

// ---------------------------------------------------------------------------
// 
// ---------------------------------------------------------------------------
//
EXPORT_C CPublisherRegistryMap* CPublisherRegistryMap::NewLC( )
    {
    CPublisherRegistryMap* self = new (ELeave) CPublisherRegistryMap();
    self->PushL();
    self->ConstructL( );
    return self;
    }

// ---------------------------------------------------------------------------
// 
// ---------------------------------------------------------------------------
//
CPublisherRegistryMap::~CPublisherRegistryMap()
    {
    }

// ---------------------------------------------------------------------------
// 
// ---------------------------------------------------------------------------
//
TInt32 CPublisherRegistryMap::AddL( RSqlDatabase aSqlDb, 
                                   CLiwDefaultList* aNotificationList ) const
    {
    CP_DEBUG( _L8("CPublisherRegistryMap::AddL") );
    TInt32 id( 0);
    if ( !GetProperty( KId, id ) )
        {
        FetchIdL( aSqlDb, KSQLFetchIdFromRegistry, id );
        }
    if ( id )
        {
        UpdateL( aSqlDb, id , aNotificationList );
        }
    else
        {
        id = AddToDbL( aSqlDb , aNotificationList );       
        }
    return id;

    }

// ---------------------------------------------------------------------------
// 
// ---------------------------------------------------------------------------
//
TUint CPublisherRegistryMap::GetListL( RSqlDatabase aSqlDb,
    CLiwGenericParamList* aList, RArray<TInt32>& aItemsToDelete ) const
    {
    CP_DEBUG( _L8("CPublisherRegistryMap::GetListL") );
    aItemsToDelete.Reset( ); //no items to delete
    return CCPLiwMap::GetListL( aSqlDb, KSQLGetListFromRegistry, aList,
        aItemsToDelete );
    }

// ---------------------------------------------------------------------------
// 
// ---------------------------------------------------------------------------
//
void CPublisherRegistryMap::RemoveL( RSqlDatabase aSqlDb, 
                                     CLiwDefaultList* aNotificationList ) const
    {
    CP_DEBUG( _L8("CContentMap::RemoveL") );
    //get ids of items to be deleted 
    GetIdsOfRemovedEntriesL( aSqlDb, KSQLGetIdsRegistry, aNotificationList );
    CCPLiwMap::RemoveL( aSqlDb, KSQLDeleteFromRegistry );
    }

// ---------------------------------------------------------------------------
// 
// ---------------------------------------------------------------------------
//  
void CPublisherRegistryMap::SaveQueryResultsL( RSqlStatement& aStmt,
    CLiwGenericParamList& aList, RArray<TInt32>& /*aItemsToDelete*/) const
    {
    CP_DEBUG( _L8("CPublisherRegistryMap::SaveQueryResultsL") );
    TInt idIndex = ColumnIndexL( aStmt,  KColumnId );
    TInt publisherIndex = ColumnIndexL( aStmt,  KColumnPublisher );
    TInt contentIndex = ColumnIndexL( aStmt,  KColumnContent_type );
    TInt content_idIndex = ColumnIndexL( aStmt,  KColumnContent_id );
    TInt flagIndex = ColumnIndexL( aStmt,  KColumnFlag );
    TInt acl_Index = ColumnIndexL( aStmt,  KColumnAccessList );
    TInt data_Index = ColumnIndexL( aStmt,  KColumnData );
    TInt action_Index = ColumnIndexL( aStmt,  KColumnAction );

    TBool aclCheckFailed( EFalse );
    while ( aStmt.Next( ) == KSqlAtRow )
        {
        TBool securityOk(EFalse);
        if( iPolicy )
            {
            TPtrC8 acl;
            User::LeaveIfError( aStmt.ColumnBinary( acl_Index, acl ) );
            securityOk = iPolicy->CheckSecurityL( acl , 
                     		CCPSecurityPolicy::ERead );
            if ( !securityOk )
             	{
             	aclCheckFailed = ETrue;
             	}
            }            
            
        if( !iPolicy || securityOk ) 
           	{
			TInt32 id = aStmt.ColumnInt( idIndex ) ;
			TPtrC publisher;
			User::LeaveIfError( aStmt.ColumnText( publisherIndex, 
					publisher ) );
			TPtrC content_type;
			User::LeaveIfError( aStmt.ColumnText( contentIndex, 
					content_type ) );
			TPtrC content_id;
			User::LeaveIfError( aStmt.ColumnText( content_idIndex,
					content_id ));
			TInt32 flag_db = aStmt.ColumnInt( flagIndex );
			TInt32 flag_map(0);
		
			TUint filter = GetFilterL();
			GetProperty( KFlag, flag_map );
				
			if (!(filter & EFlag) || ( (flag_db & flag_map)==flag_map))
				{
				CLiwDefaultMap* targetMap = CLiwDefaultMap::NewLC();
	            targetMap->InsertL( KType, TLiwVariant( KPublisher ) );
				targetMap->InsertL( KId , TLiwVariant( id ) );
				targetMap->InsertL( KPublisherId , TLiwVariant( publisher ) );
				targetMap->InsertL( KContentType , TLiwVariant( content_type ) );
				targetMap->InsertL( KContentId , TLiwVariant( content_id ) );
				targetMap->InsertL( KFlag , TLiwVariant( flag_db ) );
				TPtrC8 data;
				User::LeaveIfError( aStmt.ColumnBinary( data_Index, data ) );
				TPtrC8 action;
				User::LeaveIfError( aStmt.ColumnBinary( action_Index, action ) );
				if( data.Ptr() )
					{
					targetMap->InsertL( KDataMap , TLiwVariant( data ) );
					}
				if( action.Ptr() )
					{
					targetMap->InsertL( KActionMap , TLiwVariant( action ) );
					}
				TLiwGenericParam result( KListMap, TLiwVariant( targetMap ));
				aList.AppendL( result );
				CleanupStack::PopAndDestroy( targetMap );                
				}
            }//if
        }//while
    //leave only if ACL security check for all items failed
	if ( aclCheckFailed && !aList.Count())
		{
		User::Leave( KErrPermissionDenied );
		}    
    }


// ---------------------------------------------------------------------------
// 
// ---------------------------------------------------------------------------
//  
TInt32 CPublisherRegistryMap::AddToDbL( RSqlDatabase aSqlDb, 
    CLiwDefaultList* aNotificationList ) const
    {
    CP_DEBUG( _L8("CPublisherRegistryMap::AddToDbL") );
    RSqlStatement stmt;
    CleanupClosePushL( stmt );
    PrepareStmtforAddL( aSqlDb, stmt, KSQLInsertToRegistry );

    TInt dataIndex = stmt.ParameterIndex( KSQLData );
    TInt actionIndex = stmt.ParameterIndex( KSQLAction );
    TInt aclIndex = stmt.ParameterIndex( KSQLAccessList );
    
    RBuf8 data;
    data.CleanupClosePushL();
    GetPropertyL( KDataMap, data ) ;
    BindBinaryL( stmt, dataIndex, data );
    RBuf8 action;
    action.CleanupClosePushL();
    GetPropertyL( KActionMap, action ) ;
    BindBinaryL( stmt, actionIndex, action );
    
    RBuf8 acl;
    acl.CleanupClosePushL();
    if( !GetPropertyL( KAccessList , acl ) )
        {
        iPolicy->GetDefaultSecurityL( acl );
        }
    BindBinaryL( stmt, aclIndex, acl );
    
    if ( stmt.Exec( ) <= 0 )
        {
        User::Leave( KErrGeneral );
        }
    
    TSqlScalarFullSelectQuery id_query(aSqlDb);
    TInt32 ret( 0);
    ret = id_query.SelectIntL( KSQLGetLastInsertId );
    
    if ( aNotificationList )
        {
        RBuf publisher;
        publisher.CleanupClosePushL();   
        RBuf contentType;
        contentType.CleanupClosePushL();   
        RBuf contentId;
        contentId.CleanupClosePushL(); 
        GetPropertyL( KPublisherId, publisher ) ;
        GetPropertyL( KContentType, contentType );
        GetPropertyL( KContentId, contentId );
        CLiwDefaultMap* changeInfoMap = CLiwDefaultMap::NewLC( );
        changeInfoMap->InsertL( KId, TLiwVariant( ret ) );
        changeInfoMap->InsertL( KType, TLiwVariant( KPublisher ) );
        changeInfoMap->InsertL( KPublisherId, TLiwVariant( publisher ) );
        changeInfoMap->InsertL( KContentType, TLiwVariant( contentType ) );
        changeInfoMap->InsertL( KContentId, TLiwVariant( contentId ) );
        changeInfoMap->InsertL( KOperation, TLiwVariant( KOperationAdd ) );
        TInt32 flag( 0 );
        GetProperty( KFlag, flag );
        changeInfoMap->InsertL( KFlag, TLiwVariant( flag ) );
        aNotificationList->AppendL( TLiwVariant( changeInfoMap ) );
        CleanupStack::PopAndDestroy( changeInfoMap ) ;
        CleanupStack::PopAndDestroy( &contentId ) ;
        CleanupStack::PopAndDestroy( &contentType ) ;
        CleanupStack::PopAndDestroy( &publisher ) ;
        }

    CleanupStack::PopAndDestroy( &acl );  
    CleanupStack::PopAndDestroy( &action );  
    CleanupStack::PopAndDestroy( &data );  
    CleanupStack::PopAndDestroy( &stmt ) ;

    return ret;
    }

// ---------------------------------------------------------------------------
// 
// ---------------------------------------------------------------------------
// 
void CPublisherRegistryMap::AppendFilterL( RBuf& aQuery, 
                                           TUint& aFilter ) const
    {
    CP_DEBUG( _L8("CPublisherRegistryMap::AppendFilterL") );
    if( aFilter & EFlag )
        {
        aQuery.ReAllocL( aQuery.Length() + KSQLGetFlag().Length() + 
        		KWhere().Length() );
        aQuery.Append( KWhere );
        aQuery.Append( KSQLGetFlag );
        }
    else
        {
        CCPLiwMap::AppendFilterL( aQuery, aFilter );
        }
	} 
	   
// ---------------------------------------------------------------------------
// 
// ---------------------------------------------------------------------------
// 
void CPublisherRegistryMap::AppendSortL( RBuf& aQuery,
    const TCPSortOrder& aSort ) const
    {
    CP_DEBUG( _L8("CPublisherRegistryMap::AppendSortL") );
    switch ( aSort )
        {
        case ECPContentId:
            aQuery.ReAllocL( aQuery.Length( ) + KSQLOrderContentId().Length( ) );
            aQuery.Append( KSQLOrderContentId );
            break;
        case ECPContent:
            aQuery.ReAllocL( aQuery.Length( ) + KSQLOrderContent().Length( ) );
            aQuery.Append( KSQLOrderContent );
            break;
        case ECPPublisher:
            aQuery.ReAllocL( aQuery.Length( ) + KSQLOrderPublisher().Length( ) );
            aQuery.Append( KSQLOrderPublisher );
            break;
        default:
            aQuery.ReAllocL( aQuery.Length( ) + KSQLOrderRecent().Length( ) );
            aQuery.Append( KSQLOrderRecent );
            break;
        }
    }

// ---------------------------------------------------------------------------
// 
// ---------------------------------------------------------------------------
//  
void CPublisherRegistryMap::PrepareStmtforAddL( RSqlDatabase aSqlDb, 
	                                  RSqlStatement& aStmt , 
                                      const TDesC& aQuery ) const
    {
    CP_DEBUG( _L8("CPublisherRegistryMap::PrepareStmtforAddL") );
    CCPLiwMap::PrepareStmtforAddL( aSqlDb,aStmt,aQuery);
    TInt32 flag( 0x00000000 );
    GetProperty( KFlag, flag );
    TInt flagIndex = aStmt.ParameterIndex( KSQLFlag );
    if (flagIndex != KErrNotFound)
        {
        BindIntL( aStmt, flagIndex , flag );	        
        }
    }

// ---------------------------------------------------------------------------
// 
// ---------------------------------------------------------------------------
//  
void CPublisherRegistryMap::PrepareStmtforUpdateL( RSqlDatabase aSqlDb,
    RSqlStatement& aStmt, TInt32 aId ) const
    {
    CP_DEBUG( _L8("CPublisherRegistryMap::PrepareStmtforUpdateL") );
    TInt32 flag; 
    TBool isFlagInfo = GetProperty( KFlag, flag );    
    
    TBool isParameter(EFalse);
    RBuf query;
    query.CleanupClosePushL();
    query.CreateL( KSQLUpdateRegistry().Length( ) );
    query.Append( KSQLUpdateRegistry );
    RBuf8 data;
    data.CleanupClosePushL();
    if ( GetPropertyL( KDataMap, data ) )
        {
        query.ReAllocL( query.Length( ) + KSQLUpdateData().Length( ) );
        query.Append( KSQLUpdateData );
        isParameter = ETrue;
        }
    RBuf8 action;
    action.CleanupClosePushL();
    if ( GetPropertyL( KActionMap, action ) )
        {
        if ( isParameter )
            {
            query.ReAllocL( query.Length( ) + KComma().Length( ) );
            query.Append( KComma );
            }
        query.ReAllocL( query.Length( ) + KSQLUpdateAction().Length( ) );
        query.Append( KSQLUpdateAction );
        isParameter = ETrue;
        }
    if( isFlagInfo )
       {
       if(isParameter)
           {
           query.ReAllocL( query.Length() + KComma().Length());
           query.Append( KComma );
           }        
       query.ReAllocL( query.Length() + KSQLUpdateFlag().Length());
       query.Append( KSQLUpdateFlag );
       isParameter = ETrue;
       } 
    if( !isParameter ) User::Leave( KErrPathNotFound );
    query.ReAllocL( query.Length( ) + KSQLUpdateWhere().Length( ) );
    query.Append( KSQLUpdateWhere );
    User::LeaveIfError( aStmt.Prepare( aSqlDb, query ) );
    TInt dataId = aStmt.ParameterIndex( KSQLId );
    BindIntL( aStmt, dataId, aId );

    if ( data.Length( ) )
        {
        TInt dataIndex = aStmt.ParameterIndex( KSQLData );
        BindBinaryL( aStmt,dataIndex, data );
        }
    if ( action.Length( ) )
        {
        TInt actionIndex = aStmt.ParameterIndex( KSQLAction );
        BindBinaryL( aStmt, actionIndex, action );
        }
    if( isFlagInfo )
        {
        TInt flagIndex = aStmt.ParameterIndex( KSQLFlag );
        BindIntL( aStmt, flagIndex , flag );	        
        }
    CleanupStack::PopAndDestroy( &action );
    CleanupStack::PopAndDestroy( &data );
    CleanupStack::PopAndDestroy( &query );
    }    	
    
// ---------------------------------------------------------------------------
// 
// ---------------------------------------------------------------------------
//  
void CPublisherRegistryMap::UpdateL( RSqlDatabase aSqlDb, TInt32 aId,
    CLiwDefaultList* aNotificationList) const
    {
    CP_DEBUG( _L8("CPublisherRegistryMap::UpdateL") );
    RSqlStatement stmt;
    CleanupClosePushL( stmt );
    CLiwDefaultMap* changeInfoMap = CLiwDefaultMap::NewLC( );
    FetchIdsL( aId, KSQLFetchIdsFromRegistry, changeInfoMap, aSqlDb );
    if ( aNotificationList )
        {
        changeInfoMap->InsertL( KType, TLiwVariant( KPublisher ) );
        TInt32 flag( 0 );
        GetProperty( KFlag, flag );
        changeInfoMap->InsertL( KFlag, TLiwVariant( flag ) );
        aNotificationList->AppendL( TLiwVariant( changeInfoMap ) );
        }
    CleanupStack::PopAndDestroy( changeInfoMap ) ;        
    PrepareStmtforUpdateL( aSqlDb, stmt, aId );
    if ( stmt.Exec( ) <= 0 )
        {
        User::Leave( KErrNotFound );
        }
    CleanupStack::PopAndDestroy( &stmt ) ;
    }

// ---------------------------------------------------------------------------
// 
// ---------------------------------------------------------------------------
//
void CPublisherRegistryMap::IsValidForDeleteL() const
    {
    TInt err( KErrNone );
    TInt32 id( 0 );
    if( GetProperty( KId, id ) && ( id < 1 ) )
        {
        err = KErrArgument;
        }
    else if( !IsId() && PropertyExists( KId ) )
        {
        err = KErrBadName;
        }
    
    if( err == KErrNone && !IsFlag() && !IsId( ) && !( IsPublisherNameL( ) 
            || IsContentTypeL( ) || IsContentIdL() ) )
        {
        err = KErrPathNotFound;
        }
    
    if( err != KErrNone )
        {
        User::Leave( err );
        }
    }

// ---------------------------------------------------------------------------
// 
// ---------------------------------------------------------------------------
//
void CPublisherRegistryMap::IsValidForGetListL() const
    {
    TInt err( KErrNone );
    TInt32 id( 0 );
    if( GetProperty( KId, id ) && ( id < 1 ) )
        {
        err = KErrArgument;
        }
    else if( !IsId() && PropertyExists( KId ) )
        {
        err = KErrBadName;
        }
    
    if( err != KErrNone )
        {
        User::Leave( err );
        }
    }
// ---------------------------------------------------------------------------
// 
// ---------------------------------------------------------------------------
//                  
TBool CPublisherRegistryMap::ActivateActionSupport( ) const
    {
    return ETrue;
    }
    
// ---------------------------------------------------------------------------
// 
// ---------------------------------------------------------------------------
//                  
TBool CPublisherRegistryMap::GetActivateInfo( ) const
    {
    TInt32 flag;
    GetProperty( KFlag, flag );
    TBool activate = flag & EActivate;    
    return activate;
    }    

	
// ---------------------------------------------------------------------------
// 
// ---------------------------------------------------------------------------
//                  
void CPublisherRegistryMap::FillChangeInfoMapL( RSqlStatement& aStmt, 
		CLiwDefaultMap* aMap )const
	{
	CCPLiwMap::FillChangeInfoMapL(aStmt, aMap);
    TInt flagIndex = ColumnIndexL( aStmt,  KColumnFlag );
	TInt32 flag_db = aStmt.ColumnInt( flagIndex );
    aMap->InsertL( KFlag , TLiwVariant( flag_db ) ); 
	}

