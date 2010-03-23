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


#include <LiwCommon.h>

#include "ccontentmap.h"
#include "cpglobals.h"
#include "cpdebug.h"
#include "cpssqlcommands.h"
#include "cpsecuritypolicy.h"

// ---------------------------------------------------------------------------
// 
// ---------------------------------------------------------------------------
//

CContentMap::CContentMap()
    {

    }

// ---------------------------------------------------------------------------
// 
// ---------------------------------------------------------------------------
//
void CContentMap::ConstructL()
    {
    CCPLiwMap::ConstructL( );
    }

// ---------------------------------------------------------------------------
// 
// ---------------------------------------------------------------------------
//
EXPORT_C CContentMap* CContentMap::NewL( )
    {
    CContentMap* self = CContentMap::NewLC( );
    CleanupStack::Pop( self );
    return self;
    }

// ---------------------------------------------------------------------------
// 
// ---------------------------------------------------------------------------
//
EXPORT_C CContentMap* CContentMap::NewLC( )
    {
    CContentMap* self = new (ELeave) CContentMap();
    self->PushL();
    self->ConstructL( );
    return self;
    }

// ---------------------------------------------------------------------------
// 
// ---------------------------------------------------------------------------
//
CContentMap::~CContentMap()
    {
    
    }

// ---------------------------------------------------------------------------
// 
// ---------------------------------------------------------------------------
//
TInt32 CContentMap::AddL( RSqlDatabase aSqlDb,
    CLiwDefaultList* aNotificationList ) const
    {
    CP_DEBUG( _L8("CContentMap::AddL") );
    TInt32 id( 0);
    if ( !GetProperty( KId, id ) )
        {
        FetchIdL( aSqlDb, KSQLFetchId, id );
        }
    if ( id )
        {
        UpdateL( aSqlDb, id, aNotificationList );
        }
    else
        {
        id = AddToDbL( aSqlDb, aNotificationList );
        }
    return id;
    }

// ---------------------------------------------------------------------------
// 
// ---------------------------------------------------------------------------
//
TUint CContentMap::GetListL( RSqlDatabase aSqlDb,
    CLiwGenericParamList* aList, RArray<TInt32>& aItemsToDelete ) const
    {
    CP_DEBUG( _L8("CContentMap::GetListL") );
    aItemsToDelete.Reset( );
    return CCPLiwMap::GetListL( aSqlDb, KSQLGetList, aList, aItemsToDelete );
    }

// ---------------------------------------------------------------------------
// 
// ---------------------------------------------------------------------------
//
void CContentMap::RemoveL( RSqlDatabase aSqlDb,
    CLiwDefaultList* aNotificationList ) const
    {
    CP_DEBUG( _L8("CContentMap::RemoveL") );
    //get ids of items to be deleted 
    GetIdsOfRemovedEntriesL( aSqlDb, KSQLGetIds, aNotificationList );
    CCPLiwMap::RemoveL( aSqlDb, KSQLDelete );
    }

// ---------------------------------------------------------------------------
// 
// ---------------------------------------------------------------------------
//  
void CContentMap::SaveQueryResultsL( RSqlStatement& aStmt,
    CLiwGenericParamList& aList, RArray<TInt32>& aItemsToDelete ) const
    {
    CP_DEBUG( _L8("CContentMap::SaveQueryResultsL") );
    TInt idIndex = ColumnIndexL( aStmt,  KColumnId );
    TInt publisherIndex = ColumnIndexL( aStmt,  KColumnPublisher );
    TInt contentIndex = ColumnIndexL( aStmt,  KColumnContent_type );
    TInt content_idIndex = ColumnIndexL( aStmt,  KColumnContent_id );
    TInt expiration_dateIndex = ColumnIndexL( aStmt,  KColumnExpiration_date );
    TInt acl_Index = ColumnIndexL( aStmt,  KColumnAccessList );
    TInt data_Index = ColumnIndexL( aStmt,  KColumnData );
    TInt action_Index = ColumnIndexL( aStmt,  KColumnAction );
    
    TBool aclCheckFailed( EFalse );
    while ( aStmt.Next( ) == KSqlAtRow )
        {
        TBool securityOk( EFalse );
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
        if ( !iPolicy || securityOk )
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
            		content_id ) );
            CLiwDefaultMap* targetMap = CLiwDefaultMap::NewLC( );
            targetMap->InsertL( KId, TLiwVariant( id ) );
            targetMap->InsertL( KPublisherId, TLiwVariant( publisher ) );
            targetMap->InsertL( KContentType, TLiwVariant(content_type ) );
            targetMap->InsertL( KContentId, TLiwVariant( content_id ) );

            TInt64 expiration_date;
            expiration_date = aStmt.ColumnInt64( expiration_dateIndex );
            if ( ( expiration_date )
            		&&(IsOutdated( TTime( expiration_date ) )) )
            	{
            	// add id of item to be deleted to aItemsToDelete array
            	// do not continue getlist operation
            	aItemsToDelete.AppendL( id );
            	}//if
            else
            	{
            	// continue getlist operation
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
            	}//else        
            CleanupStack::PopAndDestroy( targetMap );
            }// if
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
void CContentMap::PrepareStmtforAddL( RSqlDatabase aSqlDb,
    RSqlStatement& aStmt, const TDesC& aQuery ) const
    {
    CP_DEBUG( _L8("CContentMap::PrepareStmtforAddL") );
    CCPLiwMap::PrepareStmtforAddL( aSqlDb, aStmt, aQuery );
    TTime expiration_date( NULL);
    GetExpirationDateL( expiration_date );
    TInt expiration_dateIndex = aStmt.ParameterIndex( KSQLExpiration_date );
    if ( expiration_dateIndex != KErrNotFound )
        {
        BindInt64L( aStmt, expiration_dateIndex, expiration_date.Int64() );
        }
    }

// ---------------------------------------------------------------------------
// 
// ---------------------------------------------------------------------------
//  
void CContentMap::PrepareStmtforUpdateL( RSqlDatabase aSqlDb,
    RSqlStatement& aStmt, TInt32 aId ) const
    {
    CP_DEBUG( _L8("CContentMap::PrepareStmtforUpdateL") );
    TTime expiration_date;
    TBool isExpirationDate = GetExpirationDateL( expiration_date );
    TBool isParameter(EFalse);
    RBuf query;
    query.CleanupClosePushL();
    query.CreateL( KSQLUpdate().Length( ) );
    query.Append( KSQLUpdate );
    RBuf8 data;
    data.CleanupClosePushL();
    RBuf8 action;
    action.CleanupClosePushL();
    if ( GetPropertyL( KDataMap, data ) )
        {
        query.ReAllocL( query.Length( ) + KSQLUpdateData().Length( ) );
        query.Append( KSQLUpdateData );
        isParameter = ETrue;
        }
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
    if ( isExpirationDate )
        {
        if ( isParameter )
            {
            query.ReAllocL( query.Length( ) + KComma().Length( ) );
            query.Append( KComma );
            }
        query.ReAllocL( query.Length( ) + KSQLUpdateExpirationDate().Length( ) );
        query.Append( KSQLUpdateExpirationDate );
        isParameter = ETrue;
        }
    if ( !isParameter )  User::Leave( KErrPathNotFound );
    query.ReAllocL( query.Length( ) + KSQLUpdateWhere().Length( ) );
    query.Append( KSQLUpdateWhere );

    User::LeaveIfError( aStmt.Prepare( aSqlDb, query ) );
    TInt dataId = aStmt.ParameterIndex( KSQLId );
    BindIntL( aStmt, dataId, aId );

    if ( data.Length( ) )
        {
        TInt dataIndex = aStmt.ParameterIndex( KSQLData );
        BindBinaryL( aStmt, dataIndex, data );
        }
    if ( action.Length( ) )
        {
        TInt actionIndex = aStmt.ParameterIndex( KSQLAction );
        BindBinaryL( aStmt, actionIndex, action );
        }
    if ( isExpirationDate )
        {
        TInt expiration_dateIndex =
                aStmt.ParameterIndex( KSQLExpiration_date );
        BindInt64L( aStmt, expiration_dateIndex,
            expiration_date.Int64( ) );

        }
    CleanupStack::PopAndDestroy( &action );
    CleanupStack::PopAndDestroy( &data );
    CleanupStack::PopAndDestroy( &query );
    }
// ---------------------------------------------------------------------------
// 
// ---------------------------------------------------------------------------
//  
void CContentMap::UpdateL( RSqlDatabase aSqlDb, TInt32 aId,
    CLiwDefaultList* aNotificationList ) const
    {
    CP_DEBUG( _L8("CContentMap::UpdateL") );
    RSqlStatement stmt;
    CleanupClosePushL( stmt );
    CLiwDefaultMap* changeInfoMap = CLiwDefaultMap::NewLC( );
    FetchIdsL( aId, KSQLFetchIds, changeInfoMap, aSqlDb );
    if ( aNotificationList )
        {
        changeInfoMap->InsertL( KType, TLiwVariant( KCpData ) );
        aNotificationList->AppendL( TLiwVariant( changeInfoMap ) );
        }
    CleanupStack::PopAndDestroy( changeInfoMap );
    PrepareStmtforUpdateL( aSqlDb, stmt, aId );
    if ( stmt.Exec( ) <= 0 )
        {
        User::Leave( KErrNotFound );
        }
    CleanupStack::PopAndDestroy( &stmt );
    }

// ---------------------------------------------------------------------------
// 
// ---------------------------------------------------------------------------
//  
TInt32 CContentMap::AddToDbL( RSqlDatabase aSqlDb,
    CLiwDefaultList* aNotificationList ) const
    {
    CP_DEBUG( _L8("CContentMap::AddToDbL") );
    RSqlStatement stmt;
    CleanupClosePushL( stmt );
    PrepareStmtforAddL( aSqlDb, stmt, KSQLInsert );

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
        changeInfoMap->InsertL( KType, TLiwVariant( KCpData ) );
        changeInfoMap->InsertL( KPublisherId, TLiwVariant( publisher ) );
        changeInfoMap->InsertL( KContentType, TLiwVariant( contentType ) );
        changeInfoMap->InsertL( KContentId, TLiwVariant( contentId ) );
        changeInfoMap->InsertL( KOperation, TLiwVariant( KOperationAdd ) );
        aNotificationList->AppendL( TLiwVariant( changeInfoMap ) );
        CleanupStack::PopAndDestroy( changeInfoMap ) ;
        CleanupStack::PopAndDestroy( &contentId ) ;
        CleanupStack::PopAndDestroy( &contentType ) ;
        CleanupStack::PopAndDestroy( &publisher ) ;
        }
    
    CleanupStack::PopAndDestroy( &acl ) ;
    CleanupStack::PopAndDestroy( &action ) ;
    CleanupStack::PopAndDestroy( &data ) ;
    CleanupStack::PopAndDestroy( &stmt ) ;
    return ret;
    }

// ---------------------------------------------------------------------------
// 
// ---------------------------------------------------------------------------
// 
void CContentMap::AppendSortL( RBuf& aQuery, const TCPSortOrder& aSort ) const
    {
    CP_DEBUG( _L8("CContentMap::AppendSortL") );
    switch ( aSort )
        {
        case ECPPublisher:
            aQuery.ReAllocL( aQuery.Length( ) + KSQLOrderPublisher().Length( ) );
            aQuery.Append( KSQLOrderPublisher );
            break;
        case ECPContent:
            aQuery.ReAllocL( aQuery.Length( ) + KSQLOrderContent().Length( ) );
            aQuery.Append( KSQLOrderContent );
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
TBool CContentMap::IsOutdated( const TTime aExpirationTime ) const
    {
    CP_DEBUG( _L8("CContentMap::IsOutdated") );
    TBool result(EFalse);
    TTime currentTime(0);
    currentTime.HomeTime( );
    if ( currentTime>aExpirationTime )
        {
        result = ETrue;
        }
    return result;
    }

// ---------------------------------------------------------------------------
// 
// ---------------------------------------------------------------------------
//
void CContentMap::IsDataMapL() const
    {
    TInt pos( 0 );
    const TLiwGenericParam* paramForValue = iMap->FindFirst( pos, KDataMap );
    if ( pos != KErrNotFound )
        { 
        if( paramForValue->Value().TypeId() != LIW::EVariantTypeDesC8  )
            {
            User::Leave( KErrBadName );
            }
        }
    else
    	{
    	User::Leave( KErrPathNotFound );
    	}
    }

// ---------------------------------------------------------------------------
// 
// ---------------------------------------------------------------------------
//
TBool CContentMap::GetExpirationDateL( TTime& aResult ) const
    {
    CP_DEBUG( _L8("CContentMap::GetExpirationDateL") );
    TInt pos( 0);
    TBool result(EFalse);
    const TLiwGenericParam* param= NULL;
    param = iMap->FindFirst( pos, KExpirationDate );
    if ( KErrNotFound != pos )
        {
        if ( !param->Value().Get( aResult ) )
            User::Leave( KErrArgument );
        result = ETrue;
        }
    return result;
    }

// ---------------------------------------------------------------------------
// 
// ---------------------------------------------------------------------------
//
void CContentMap::IsValidForDeleteL() const
    {
    CP_DEBUG( _L8("CContentMap::IsValidFilterL") );
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
    
    if( err == KErrNone && !IsId( ) && !( IsPublisherNameL( ) 
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
void CContentMap::IsValidForGetListL() const
    {
    CP_DEBUG( _L8("CContentMap::IsValidForGetListL") );
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
TBool CContentMap::ActivateActionSupport( ) const
    {
    return EFalse;
    }
    
// ---------------------------------------------------------------------------
// 
// ---------------------------------------------------------------------------
//                  
TBool CContentMap::GetActivateInfo( ) const
    {
    return EFalse;
    }
	
