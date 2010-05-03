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


#include <s32mem.h>
#include <liwgenericparam.h>
#include <escapeutils.h>
#include <badesca.h>

#include "cpliwmap.h"
#include "cpdebug.h"
#include "cpssqlcommands.h"
#include "ccontentmap.h"
#include "cpublisherregistrymap.h"
#include "cpsecuritypolicy.h"

// ======== MEMBER FUNCTIONS ========

// ---------------------------------------------------------------------------
// 
// ---------------------------------------------------------------------------
//
EXPORT_C CCPLiwMap* CCPLiwMap::NewL( RDesReadStream& aStream )
    {
    CLiwGenericParamList* list = CLiwGenericParamList::NewLC( aStream );
    CCPLiwMap* map = CCPLiwMap::NewL( *list );
    CleanupStack::PopAndDestroy( list );
    return map;
    }

// ---------------------------------------------------------------------------
// 
// ---------------------------------------------------------------------------
//
EXPORT_C CCPLiwMap* CCPLiwMap::NewL( const CLiwGenericParamList& aList )
    {
    CP_DEBUG( _L8("CCPLiwMap::NewL") );
    const TLiwGenericParam* param = NULL;
    TInt pos( 0 );
    param = aList.FindFirst( pos, KType );
    CCPLiwMap* map( NULL );
    if( param && pos !=KErrNotFound )
        {
        RBuf typeBuf;
        typeBuf.CleanupClosePushL();
        TPtrC type( KNullDesC );
        if( !param->Value().Get( type ) )
            {
            TPtrC8 type8( KNullDesC8 );
            if( !param->Value().Get( type8 ) )
                {
                User::Leave( KErrBadName );
                }
            typeBuf.Assign( EscapeUtils::ConvertToUnicodeFromUtf8L( type8 ) );
            }
        else
            {
            typeBuf.CreateL( type );
            }
        if( typeBuf.Find( KCpData () ) != KErrNotFound )
            {
            map = CContentMap::NewLC();
            }
        else if ( typeBuf.Find( KPublisher () ) != KErrNotFound )
            {
            map = CPublisherRegistryMap::NewLC();
            }
        else User::Leave( KErrArgument );
        map->SetL( aList );
        CleanupStack::Pop( map );
        CleanupStack::PopAndDestroy( &typeBuf );
        }
    else
        {
        User::Leave( KErrPathNotFound );
        }
    return map;
    }

// ---------------------------------------------------------------------------
// 
// ---------------------------------------------------------------------------
//		
EXPORT_C HBufC8* CCPLiwMap::PackForServerLC() const
    {
    CP_DEBUG( _L8("CCPLiwMap::PackForServerLC") );
    HBufC8* datadesc = HBufC8::NewLC( Size() );
    TPtr8 ptr = datadesc->Des();
    RDesWriteStream datastrm( ptr );
    CleanupClosePushL(datastrm);
    ExternalizeL(datastrm);
    datastrm.CommitL();
    CleanupStack::PopAndDestroy(&datastrm);
    return datadesc;
    }

// ---------------------------------------------------------------------------
// 
// ---------------------------------------------------------------------------
//	
EXPORT_C void CCPLiwMap::Reset( ) const
    {
    iMap->Reset();    
    }

// ---------------------------------------------------------------------------
// 
// ---------------------------------------------------------------------------
//
void CCPLiwMap::IsValidForAddL( ) const
    {
    CP_DEBUG( _L8("CCPLiwMap::IsValidForAddL") );
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

    CheckIdentifiersL();
    
    if( err == KErrNone && !IsId() && !( IsPublisherNameL() 
            && IsContentTypeL() && IsContentIdL() ) )
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
EXPORT_C void CCPLiwMap::IsValidForActionL( )const
    {
    CP_DEBUG( _L8("CCPLiwMap::IsValidForActionL") );
    CCPLiwMap::IsValidForNotificationL();
    if( !IsTriggerL() ) User::Leave( KErrPathNotFound );
    }

// ---------------------------------------------------------------------------
// 
// ---------------------------------------------------------------------------
//
EXPORT_C void CCPLiwMap::IsValidForNotificationL( ) const
    {
    CP_DEBUG( _L8("CCPLiwMap::IsValidForAddL") );
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
    
    CheckIdentifiersL();
    
    if( err == KErrNone && !IsId() && !IsPublisherNameL() 
            && !IsContentTypeL() && !IsContentIdL() )
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
EXPORT_C TBool CCPLiwMap::GetPropertyL( const TDesC8& aProperty ,
        RBuf& aResult ) const
    {
    //change name
    CP_DEBUG( _L8("CCPLiwMap::GetProperty - TPtrC") );
    TBool result( EFalse );
    TInt pos( 0 );
    TPtrC result16( KNullDesC);
    TPtrC8 result8( KNullDesC8);
    const TLiwGenericParam* paramForValue = iMap->FindFirst( pos, aProperty );
    if ( pos != KErrNotFound )
        {
        if( paramForValue->Value().Get( result16 ) )
            {
            aResult.CreateL( result16 );
            result = ETrue;
            }
        else if( paramForValue->Value().Get( result8 ) )
            {
            aResult.Assign( EscapeUtils::ConvertToUnicodeFromUtf8L( result8 ) );
            result = ETrue;
            }
        else
        	{
        	User::Leave( KErrBadName );
        	}
        }
    return result;
    }

// ---------------------------------------------------------------------------
// 
// ---------------------------------------------------------------------------
//
EXPORT_C TBool CCPLiwMap::GetPropertyL( const TDesC8& aProperty ,
        RBuf8& aResult ) const
    {
    CP_DEBUG( _L8("CCPLiwMap::GetProperty - TPtrC8") );
    TBool result( EFalse );
    TInt pos( 0 );
    TPtrC8 result8( KNullDesC8 );
    TPtrC result16( KNullDesC );
    const TLiwGenericParam* paramForValue = iMap->FindFirst( pos, aProperty );
    if ( pos != KErrNotFound )
        {
        if( paramForValue->Value().Get( result8 ) )
            {
            aResult.CreateL( result8 );
            result = ETrue;
            }
        else if( paramForValue->Value().Get( result16 ) )
            {
            aResult.Assign( EscapeUtils::ConvertFromUnicodeToUtf8L( result16 ) );
            result = ETrue;
            }
        else
        	{
        	User::Leave( KErrBadName );
        	}
        }
    return result;
    }

// ---------------------------------------------------------------------------
// 
// ---------------------------------------------------------------------------
//
EXPORT_C TBool CCPLiwMap::GetProperty( const TDesC8& aProperty ,
		TInt32& aResult ) const
    {
    CP_DEBUG( _L8("CCPLiwMap::GetProperty - TInt32") );
    TBool result( EFalse );
    TInt pos( 0 );
    const TLiwGenericParam* paramForValue = iMap->FindFirst( pos, aProperty );
    if ( pos != KErrNotFound )
        {
        if( paramForValue->Value().Get( aResult ) )
            {
            result = ETrue;
            }
        }
    return result;
    }


// ---------------------------------------------------------------------------
// 
// ---------------------------------------------------------------------------
//
EXPORT_C void CCPLiwMap::SetSecurityL(  const RMessage2& aMessage )
    {
    delete iPolicy;
    iPolicy = NULL;    
    iPolicy = CCPSecurityPolicy::NewL( aMessage );
    }
    
// ---------------------------------------------------------------------------
// 
// ---------------------------------------------------------------------------
//
CCPLiwMap::~CCPLiwMap()
    {
    delete iMap;
    delete iPolicy;
    }

// ---------------------------------------------------------------------------
// 
// ---------------------------------------------------------------------------
//
CCPLiwMap::CCPLiwMap()
    {

    }

// ---------------------------------------------------------------------------
// 
// ---------------------------------------------------------------------------
//
void CCPLiwMap::InsertL( const TDesC8& aKey, const TLiwVariant& aValue )
    {
    CP_DEBUG( _L8("CCPLiwMap::InsertL") );
    Remove( aKey );
    TLiwGenericParam mp;
    mp.SetNameAndValueL( aKey, aValue );
    iMap->AppendL( mp );
    mp.Reset( );
    }

// ---------------------------------------------------------------------------
// 
// ---------------------------------------------------------------------------
//
TBool CCPLiwMap::FindL( const TDesC8& aKey, TLiwVariant& aValue ) const
    {
    CP_DEBUG( _L8("CCPLiwMap::FindL") );
    TInt pos = 0;
    TBool result(EFalse);
    const TLiwGenericParam* tempParam = iMap->FindFirst( pos, aKey );
    if ( tempParam )
        {
        aValue.SetL( tempParam->Value( ) );
        result = ETrue;
        }
    return result;
    }

// ---------------------------------------------------------------------------
// 
// ---------------------------------------------------------------------------
//
TInt CCPLiwMap::Count() const
    {
    return iMap->Count( );
    }

// ---------------------------------------------------------------------------
// 
// ---------------------------------------------------------------------------
//
TBool CCPLiwMap::AtL( TInt aIndex, TDes8& aKey ) const
    {
    CP_DEBUG( _L8("CCPLiwMap::AtL") );
    TBool result(EFalse);
    if ( 0 <= aIndex && aIndex < iMap->Count( ) )
        {
        aKey = ((*iMap)[aIndex]).Name( );
        result = ETrue;
        }
    return result;
    }

// ---------------------------------------------------------------------------
// 
// ---------------------------------------------------------------------------
//
void CCPLiwMap::Remove( const TDesC8& aKey )
    {
    CP_DEBUG( _L8("CCPLiwMap::Remove") );
    TInt pos = 0;
    iMap->FindFirst( pos, aKey );
    if ( pos != KErrNotFound )
        {
        iMap->Remove( aKey );
        }
    }

// ---------------------------------------------------------------------------
// 
// ---------------------------------------------------------------------------
//
TInt CCPLiwMap::Size() const
    {
    return iMap->Size( );
    }

// ---------------------------------------------------------------------------
// 
// ---------------------------------------------------------------------------
//	
void CCPLiwMap::ExternalizeL( RWriteStream& aStream ) const
    {
    CP_DEBUG( _L8("CCPLiwMap::ExternalizeL") );
    iMap->ExternalizeL( aStream );
    }

// ---------------------------------------------------------------------------
// 
// ---------------------------------------------------------------------------
//                  
void CCPLiwMap::FillChangeInfoMapL( RSqlStatement& aStmt, 
		CLiwDefaultMap* aMap )const
	{
    TInt idIndex = ColumnIndexL( aStmt, KColumnId );
    TInt publisherIndex = ColumnIndexL( aStmt, KColumnPublisher );
    TInt contentIndex = ColumnIndexL( aStmt, KColumnContent_type );
    TInt content_idIndex = ColumnIndexL( aStmt, KColumnContent_id );
    TInt acl_Index = ColumnIndexL( aStmt, KColumnAccessList );
	
    TInt32 id = aStmt.ColumnInt( idIndex ) ;
    TPtrC publisher;
    User::LeaveIfError(aStmt.ColumnText( publisherIndex,publisher) );
    TPtrC content_type;
    User::LeaveIfError(aStmt.ColumnText( contentIndex,content_type ) );
    TPtrC content_id;
    User::LeaveIfError(aStmt.ColumnText( content_idIndex,content_id) );
    aMap->InsertL( KId, TLiwVariant( id ) );
    aMap->InsertL( KPublisherId, TLiwVariant( publisher ) );
    aMap->InsertL( KContentType, TLiwVariant( content_type ) );
    aMap->InsertL( KContentId, TLiwVariant( content_id ) );
    aMap->InsertL( KOperation, TLiwVariant( KOperationDelete ) );
    RBuf type;
    type.CleanupClosePushL();
    if( GetPropertyL( KType, type ) )
         {
         aMap->InsertL( KType , TLiwVariant( type ) );
         }        
    CleanupStack::PopAndDestroy( &type );  
	}


// ---------------------------------------------------------------------------
// 
// ---------------------------------------------------------------------------
//
void CCPLiwMap::SetL( const CLiwGenericParamList& aInParamList )
    {
    
    CP_DEBUG( _L8("CCPLiwMap::SetL") );
    for ( TInt i = 0; i < aInParamList.Count( ); i++ )
        {
        const TLiwGenericParam& param = aInParamList[i];
        if ( param.Value().TypeId( ) == LIW::EVariantTypeMap )
            {
            const CLiwMap* map = param.Value().AsMap( );
            for ( TInt i = 0; i <map->Count( ); i++ )
                {
                TBuf8<128> key;
                map->AtL( i, key );
                TLiwVariant value;
                value.PushL( );
                map->FindL( key, value );
                if ( key == KOperation )
                	{
                	IsProperOperationL( value );
                	}
                RBuf8 datadesc;
                datadesc.CleanupClosePushL();
                if ( value.TypeId( ) == LIW::EVariantTypeMap )
                    {
                    const CLiwMap* internalMap = value.AsMap( );
                    datadesc.CreateL( internalMap->Size( ) );
                    RDesWriteStream datastrm(datadesc);
                    CleanupClosePushL( datastrm );
                    internalMap->ExternalizeL( datastrm );
                    datastrm.CommitL( );
                    CleanupStack::PopAndDestroy( &datastrm );
                    value.Reset( );
                    value.Set( datadesc );
                    }
                TLiwGenericParam data( key, value);
                iMap->AppendL( data );
                CleanupStack::PopAndDestroy( &datadesc );
                CleanupStack::PopAndDestroy( &value );
                }
            }
        else
            {
            iMap->AppendL( param );
            }
        }
    }

// ---------------------------------------------------------------------------
// 
// ---------------------------------------------------------------------------
// 
void CCPLiwMap::IsProperOperationL( const TLiwVariant& aValue ) const
    {
    CP_DEBUG( _L8("CContentMap::IsProperOperationL") );
	TPtrC valDesc( KNullDesC );
	TPtrC8 valDesc8( KNullDesC8 );
	RBuf desc;
	desc.CleanupClosePushL();

    if( aValue.Get( valDesc ) == EFalse )
    	{
    	if( aValue.Get( valDesc8 ) != EFalse )
			{
			desc.Assign( EscapeUtils::ConvertToUnicodeFromUtf8L( valDesc8 ) );
			valDesc.Set( desc );
			}
    	else
    		{
    		User::Leave( KErrBadName );
    		}
    	}
    
    TLex input( valDesc );
    
    CDesC16Array* array = new (ELeave) CDesC16ArrayFlat(4);
    CleanupStack::PushL( array );
    
    TLexMark startMark;
    input.Mark( startMark );
    TBool notEmpty = EFalse;
    while ( !input.Eos() )
        {
        if( input.Peek() == ':')
            {
            array->AppendL( input.MarkedToken( startMark ) );
            input.Inc();
            input.Mark( startMark );
            }
        input.Inc();     
        notEmpty = ETrue;       
        }
    if ( notEmpty )
        {
        array->AppendL( input.MarkedToken( startMark ) );
        }        
    
    for( TInt i= 0; i < array->MdcaCount(); i++ )
    	{
    	TPtrC operValue( array->MdcaPoint(i) );
    	
    	if( operValue.Compare( KOperationAdd ) &&
    		operValue.Compare( KOperationUpdate ) &&
    		operValue.Compare( KOperationDelete ) &&
    		operValue.Compare( KOperationExecute ) )
    		{
    		User::Leave( KErrArgument );
    	    }
    	}
    CleanupStack::PopAndDestroy( array );
	CleanupStack::PopAndDestroy( &desc );
    }


// ---------------------------------------------------------------------------
// 
// ---------------------------------------------------------------------------
// 
void CCPLiwMap::AppendGetListFilterL( RBuf& aQuery, TUint& aFilter ) const
    {
    CP_DEBUG( _L8("CContentMap::AppendGetListFilterL") );
    if( !IsGetListAllL( aFilter ) )
    	{
    	AppendFilterL( aQuery, aFilter );
    	}
    }

// ---------------------------------------------------------------------------
// 
// ---------------------------------------------------------------------------
// 
void CCPLiwMap::AppendRemoveFilterL( RBuf& aQuery, TUint& aFilter ) const
    {
    CP_DEBUG( _L8("CContentMap::AppendRemoveFilterL") );
	AppendFilterL( aQuery, aFilter );
    }
// ---------------------------------------------------------------------------
// 
// ---------------------------------------------------------------------------
// 
void CCPLiwMap::AppendFilterL( RBuf& aQuery, TUint& aFilter ) const
    {
    CP_DEBUG( _L8("CContentMap::AppendFilterL") );

    TBool nextFilter( EFalse );
    
	RemoveAllAttributeL( aFilter ); //remove "all" attribute from publisher, 
						   //cont_type or cont_id
    
   	aQuery.ReAllocL( aQuery.Length( ) + KWhere().Length( ) );
   	aQuery.Append( KWhere );
    	
   	if( aFilter & EContentType )
   		{
   		aQuery.ReAllocL( aQuery.Length( ) + KSQLGetContentType().Length( ) );
		aQuery.Append( KSQLGetContentType );
		nextFilter = ETrue;    
   		}
   	if( aFilter & EPublisher )
   		{
   		if( nextFilter )
   			{
   			aQuery.ReAllocL( aQuery.Length( ) + KAnd().Length( ) + 
   					KSQLGetPublisher().Length( ) );
			aQuery.Append( KAnd );
			aQuery.Append( KSQLGetPublisher );
   			}
   		else
   			{
   			aQuery.ReAllocL( aQuery.Length( ) + 
   					KSQLGetPublisher().Length( ) );
   			aQuery.Append( KSQLGetPublisher );
   			}
   		nextFilter = ETrue;  
   		}
   	if( aFilter & EContentId )
   		{
   		if( nextFilter )
   			{
   			aQuery.ReAllocL( aQuery.Length( ) + 
   					KAnd().Length( ) + KSQLGetContentId().Length( ) );
            aQuery.Append( KAnd );
            aQuery.Append( KSQLGetContentId );
  			}
		else
			{
			aQuery.ReAllocL( aQuery.Length( ) + KSQLGetContentId().Length( ) );
            aQuery.Append( KSQLGetContentId );
			}
   		nextFilter = ETrue;  
   		}
    }

// ---------------------------------------------------------------------------
// 
// ---------------------------------------------------------------------------
//  
void CCPLiwMap::PrepareStmtforAddL( RSqlDatabase aSqlDb,
    RSqlStatement& aStmt, const TDesC& aQuery ) const
    {
    CP_DEBUG( _L8("CCPLiwMap::PrepareStmtforAddL") );
    RBuf publisher;
    publisher.CleanupClosePushL();
    RBuf content_type;
    content_type.CleanupClosePushL();  
    RBuf content_id;
    content_id.CleanupClosePushL();  
    if ( GetPropertyL( KPublisherId, publisher )
        && GetPropertyL( KContentType, content_type )
        && GetPropertyL( KContentId, content_id ) )
        {
        User::LeaveIfError( aStmt.Prepare( aSqlDb, aQuery ) );
        TInt publisherIndex = aStmt.ParameterIndex( KSQLPublisher );
        TInt content_typeIndex = aStmt.ParameterIndex( KSQLContent_type );
        TInt content_idIndex = aStmt.ParameterIndex( KSQLContent_id );
        
        BindTextL( aStmt, publisherIndex, publisher );
        BindTextL( aStmt, content_typeIndex, content_type );
        BindTextL( aStmt, content_idIndex, content_id );
        }
    else
        {
        User::Leave( KErrArgument );
        }
    CleanupStack::PopAndDestroy( &content_id );
    CleanupStack::PopAndDestroy( &content_type );
    CleanupStack::PopAndDestroy( &publisher );
    }

// ---------------------------------------------------------------------------
// 
// ---------------------------------------------------------------------------
//
TUint CCPLiwMap::GetFilterL( )const
    {
    CP_DEBUG( _L8("CCPLiwMap::GetFilterL") );
    TUint result (0);
    if(IsPublisherNameL())
        {
        result |= EPublisher;
        }
    if(IsContentTypeL())
        {
        result |= EContentType;
        }
    if(IsContentIdL())
        {
        result |= EContentId;
        }
    if(IsFlag())
        {
        result |= EFlag;
        }		
    return result;
	}

// ---------------------------------------------------------------------------
// 
// ---------------------------------------------------------------------------
//
CCPLiwMap::TCPSortOrder CCPLiwMap::GetSortL() const
    {
    CP_DEBUG( _L8("CCPLiwMap::GetSortL") );
    TCPSortOrder result(ECPRecent);
    RBuf sort;
    sort.CleanupClosePushL();  
    if ( GetPropertyL( KSortOrder, sort ) )
        {
        if ( !sort.Compare( KSortRecent ) )
            {
            result = ECPRecent;
            }
        else if ( !sort.Compare( KSortPublisher ) )
            {
            result = ECPPublisher;
            }
        else if ( !sort.Compare( KSortContentType ) )
            {
            result = ECPContent;
            }
        else if ( !sort.Compare( KSortContentId ) )
            {
            result = ECPContentId;
            }
        }
    CleanupStack::PopAndDestroy( &sort );
    return result;
    }

// ---------------------------------------------------------------------------
// 
// ---------------------------------------------------------------------------
//
TBool CCPLiwMap::IsPublisherNameL() const
    {
    TBool result( EFalse );
    RBuf buffer;
    buffer.CleanupClosePushL();
    result = GetPropertyL( KPublisherId, buffer );
    if( result && buffer.Length() == KErrNone )
        {
        User::Leave( KErrArgument );
        }
    CleanupStack::PopAndDestroy( &buffer );
    return result;
    }

// ---------------------------------------------------------------------------
// 
// ---------------------------------------------------------------------------
//
TBool CCPLiwMap::IsContentTypeL() const
    {
    TBool result( EFalse );
    RBuf buffer;
    buffer.CleanupClosePushL();
    result = GetPropertyL( KContentType, buffer );
    if( result && buffer.Length() == KErrNone )
        {
        User::Leave( KErrArgument );
        }
    CleanupStack::PopAndDestroy( &buffer );
    return result;
    }

// ---------------------------------------------------------------------------
// 
// ---------------------------------------------------------------------------
//
TBool CCPLiwMap::IsContentIdL() const
    {
    TBool result( EFalse );
    RBuf buffer;
    buffer.CleanupClosePushL();
    result = GetPropertyL( KContentId, buffer );
    if( result && buffer.Length() == KErrNone )
        {
        User::Leave( KErrArgument );
        }
    CleanupStack::PopAndDestroy( &buffer );
    return result;
    }

// ---------------------------------------------------------------------------
// 
// ---------------------------------------------------------------------------
//
TBool CCPLiwMap::IsTriggerL( ) const
    {
    TBool result( EFalse );
    RBuf8 buffer;
    buffer.CleanupClosePushL();
    result = GetPropertyL( KActionTrigger, buffer );
    CleanupStack::PopAndDestroy( &buffer );
    return result;
    }

// ---------------------------------------------------------------------------
// 
// ---------------------------------------------------------------------------
//
TBool CCPLiwMap::IsFlag( ) const
    {
    TInt32 buffer( 0 );
    return GetProperty( KFlag, buffer );
    }
		
// ---------------------------------------------------------------------------
// 
// ---------------------------------------------------------------------------
//
TBool CCPLiwMap::IsId() const
    {
    TInt32 id( 0);
    return GetProperty( KId, id );
    }

// ---------------------------------------------------------------------------
// 
// ---------------------------------------------------------------------------
//
TBool CCPLiwMap::PropertyExists( const TDesC8& aProperty ) const
    {
    TBool result( EFalse );
    TInt pos( 0 );
    iMap->FindFirst( pos, aProperty );
    if ( pos != KErrNotFound )
        {
        result = ETrue;
        }
    return result;
    }


// ---------------------------------------------------------------------------
// 
// ---------------------------------------------------------------------------
//
TUint CCPLiwMap::GetListL( RSqlDatabase aSqlDb, const TDesC& aQuery,
    CLiwGenericParamList* aList, RArray<TInt32>& aItemsToDelete ) const
    {
    CP_DEBUG( _L8("CCPLiwMap::GetListL") );
    RBuf sqlquery;
    sqlquery.CleanupClosePushL();
    sqlquery.CreateL( aQuery );
    TInt32 id( 0);
    if ( GetProperty( KId, id ) )
        {
        ConstructSelectIdL( sqlquery );
        }
    else
        {
        ConstructGetListSelectL( sqlquery );
        }
    RSqlStatement stmt;
    CleanupClosePushL( stmt );
    User::LeaveIfError( stmt.Prepare( aSqlDb, sqlquery ) );
    BindValueL( sqlquery, stmt );
    SaveQueryResultsL( stmt, *aList, aItemsToDelete );
    CleanupStack::PopAndDestroy( &stmt ) ;
    CleanupStack::PopAndDestroy( &sqlquery );
    return aList->Count( );
    }

// ---------------------------------------------------------------------------
// 
// ---------------------------------------------------------------------------
//
void CCPLiwMap::RemoveL( RSqlDatabase aSqlDb, const TDesC& aQuery ) const
    {
    CP_DEBUG( _L8("CCPLiwMap::RemoveL") );
    RBuf sqlquery;
    sqlquery.CleanupClosePushL();
    sqlquery.CreateL( aQuery );
    TInt32 id( 0);
    if ( GetProperty( KId, id ) )
        {
        ConstructSelectIdL( sqlquery );
        }
    else
        {
        ConstructRemoveSelectL( sqlquery, EFalse );
        }

    RSqlStatement stmt;
    CleanupClosePushL( stmt );
    User::LeaveIfError( stmt.Prepare( aSqlDb, sqlquery ) );

    BindValueL( sqlquery, stmt );
    if ( stmt.Exec( ) <= 0 )
        {
        User::Leave( KErrNotFound );
        }
    CleanupStack::PopAndDestroy( &stmt ) ;
    CleanupStack::PopAndDestroy( &sqlquery );
    }
// ---------------------------------------------------------------------------
// 
// ---------------------------------------------------------------------------
//  
void CCPLiwMap::FetchIdsL( TInt32 aId, const TDesC& aQuery,
    CLiwDefaultMap* aMap, RSqlDatabase aSqlDb ) const
    {
    CP_DEBUG( _L8("CCPLiwMap::FetchIdsL") );    
    RSqlStatement stmt;
    CleanupClosePushL( stmt );
    //prepare statment for "Get Ids" (Publisher, ContentType,ContentId)
    User::LeaveIfError( stmt.Prepare( aSqlDb, aQuery ) );
    TInt idIndex = stmt.ParameterIndex( KSQLId );
    User::LeaveIfError( stmt.BindInt( idIndex, aId ) );

    TInt publisherIndex = ColumnIndexL( stmt, KColumnPublisher );
    TInt contentTypeIndex = ColumnIndexL( stmt, KColumnContent_type );
    TInt contentIdIndex = ColumnIndexL( stmt, KColumnContent_id );
    TInt acl_Index = ColumnIndexL( stmt, KColumnAccessList );
    
    if ( stmt.Next( ) == KSqlAtRow )
        {
        if( iPolicy )
            {
            TPtrC8 acl;
            User::LeaveIfError( stmt.ColumnBinary( acl_Index, acl ) );
            if( !iPolicy->CheckSecurityL( acl , CCPSecurityPolicy::EWrite ) )
            	{
            	User::Leave( KErrPermissionDenied );
            	}
            }
        TPtrC publisher;
        User::LeaveIfError( stmt.ColumnText( publisherIndex, publisher ) );
        TPtrC content_type;
        User::LeaveIfError( stmt.ColumnText( contentTypeIndex, content_type ) );
        TPtrC content_id;
        User::LeaveIfError( stmt.ColumnText( contentIdIndex, content_id ) );
        aMap->InsertL( KId, TLiwVariant( aId ) );
        aMap->InsertL( KPublisherId, TLiwVariant( publisher ) );
        aMap->InsertL( KContentType, TLiwVariant( content_type ) );
        aMap->InsertL( KContentId, TLiwVariant( content_id ) );
        aMap->InsertL( KOperation, TLiwVariant( KOperationUpdate ) );
        }
    CleanupStack::PopAndDestroy( &stmt ) ;
    }

// ---------------------------------------------------------------------------
// 
// ---------------------------------------------------------------------------
//
void CCPLiwMap::FetchIdL( RSqlDatabase aSqlDb, const TDesC& aQuery, 
		TInt32& aId ) const
    {
    CP_DEBUG( _L8("CCPLiwMap::FetchIdL") );
    aId = 0;
    RSqlStatement stmt;
    CleanupClosePushL( stmt );
    PrepareStmtforAddL( aSqlDb, stmt, aQuery );
    TInt idIndex = ColumnIndexL( stmt, KColumnId );
    if ( stmt.Next( ) == KSqlAtRow )
        {
        aId = stmt.ColumnInt( idIndex );
        }
    CleanupStack::PopAndDestroy( &stmt ) ;
    }

// ---------------------------------------------------------------------------
// 
// ---------------------------------------------------------------------------
//  
void CCPLiwMap::GetIdsOfRemovedEntriesL( RSqlDatabase aSqlDb,
    const TDesC& aQuery, CLiwDefaultList* aList ) const
    {
    CP_DEBUG( _L8("CCPLiwMap::GetIdsOfRemovedEntriesL") );
    RBuf sqlquery;
    sqlquery.CleanupClosePushL();
    sqlquery.CreateL( aQuery );
    TInt32 id( 0);
    if ( GetProperty( KId, id ) )
        {
        ConstructSelectIdL( sqlquery );
        }
    else
        {
        ConstructRemoveSelectL( sqlquery );
        }
    RSqlStatement stmt;
    CleanupClosePushL( stmt );
    User::LeaveIfError( stmt.Prepare( aSqlDb, sqlquery ) );
    BindValueL( sqlquery, stmt );
    SaveQueryResultsIdsL( stmt, aList );
    if( aList && aList->Count()==0 )
        {
        User::Leave( KErrNotFound );
        }
    CleanupStack::PopAndDestroy( &stmt ) ;
    CleanupStack::PopAndDestroy( &sqlquery );
    }

// ---------------------------------------------------------------------------
// 
// ---------------------------------------------------------------------------
//  
void CCPLiwMap::SaveQueryResultsIdsL( RSqlStatement& aStmt,
    CLiwDefaultList* aList ) const
    {
    CP_DEBUG( _L8("CCPLiwMap::SaveQueryResultsIdsL") );
    TInt acl_Index = ColumnIndexL( aStmt, KColumnAccessList );

    while ( aStmt.Next( ) == KSqlAtRow )
        {
        if( iPolicy )
            {
            TPtrC8 acl;
            User::LeaveIfError( aStmt.ColumnBinary( acl_Index, acl ) );
            if ( !iPolicy->CheckSecurityL( acl , 
            		CCPSecurityPolicy::EDelete ) )
              	{
        		User::Leave( KErrPermissionDenied );
               	}            
            }
        if ( aList )
            {
            CLiwDefaultMap* targetMap = CLiwDefaultMap::NewLC( );
            FillChangeInfoMapL( aStmt, targetMap );
            aList->AppendL( TLiwVariant( targetMap ) );
            CleanupStack::PopAndDestroy( targetMap );    
            }
        }
    }

// ---------------------------------------------------------------------------
// 
// ---------------------------------------------------------------------------
// 
void CCPLiwMap::ConstructSelectIdL( RBuf& aQuery ) const
    {
    aQuery.ReAllocL( aQuery.Length( ) + KSQLGetId().Length( ) + 
    		KWhere().Length() );
    aQuery.Append( KWhere );
    aQuery.Append( KSQLGetId );
    }

// ---------------------------------------------------------------------------
// 
// ---------------------------------------------------------------------------
// 
void CCPLiwMap::ConstructGetListSelectL( RBuf& aQuery, TBool aGet ) const
    {
    TUint filter = GetFilterL( );
    AppendGetListFilterL( aQuery, filter );
    if ( aGet )
        {
        AppendSortL( aQuery, GetSortL( ) );
        }
    }

// ---------------------------------------------------------------------------
// 
// ---------------------------------------------------------------------------
// 
void CCPLiwMap::ConstructRemoveSelectL( RBuf& aQuery, TBool aGet ) const
    {
    TUint filter = GetFilterL( );
    AppendRemoveFilterL( aQuery, filter );
    if ( aGet )
        {
        AppendSortL( aQuery, GetSortL( ) );
        }
    }

// ---------------------------------------------------------------------------
// 
// ---------------------------------------------------------------------------
//
void CCPLiwMap::BindValueL( const RBuf& aQuery, RSqlStatement& aStmt ) const
    {
    CP_DEBUG( _L8("CCPLiwMap::BindValueL") );
    if ( aQuery.Find( KSQLGetPublisher ) != KErrNotFound )
        {
        TInt param = aStmt.ParameterIndex( KSQLPublisher );
        RBuf publisher;
        publisher.CleanupClosePushL();
        User::LeaveIfError( GetPropertyL( KPublisherId, publisher ));
        BindTextL( aStmt, param, publisher );
        CleanupStack::PopAndDestroy( &publisher );  
        }
    if ( aQuery.Find( KSQLGetContentType ) != KErrNotFound )
        {
        TInt param = aStmt.ParameterIndex( KSQLContent_type );
        RBuf content_type;
        content_type.CleanupClosePushL();
        User::LeaveIfError( GetPropertyL( KContentType, content_type ));
        BindTextL( aStmt, param, content_type );
        CleanupStack::PopAndDestroy( &content_type );  
        }
    if ( aQuery.Find( KSQLGetContentId ) != KErrNotFound )
        {
        TInt param = aStmt.ParameterIndex( KSQLContent_id );
        RBuf content_id;
        content_id.CleanupClosePushL();
        User::LeaveIfError( GetPropertyL( KContentId, content_id ));
        BindTextL( aStmt, param, content_id );
        CleanupStack::PopAndDestroy( &content_id );  
        }
    if ( aQuery.Find( KSQLGetId ) != KErrNotFound )
        {
        TInt param = aStmt.ParameterIndex( KSQLId );
        TInt32 id( 0);
        User::LeaveIfError( GetProperty( KId, id ));
        BindIntL( aStmt, param, id );
        }
    }

// ---------------------------------------------------------------------------
// 
// ---------------------------------------------------------------------------
//
TBool CCPLiwMap::IsGetListAllL( TUint aFilter ) const
    {
    TBool result( EFalse );
    if ( !( aFilter & EContentType ) &&
    		!( aFilter & EContentId ) &&
    		!( aFilter & EPublisher ) )
    	{
    	result = ETrue;
    	}
    else if ( ( aFilter & EContentType ) &&
				( aFilter & EContentId ) &&
				( aFilter & EPublisher ) )
    	{
        RBuf publisher;
        publisher.CleanupClosePushL();
        RBuf content_type;
        content_type.CleanupClosePushL();  
        RBuf content_id;
        content_id.CleanupClosePushL();  
        GetPropertyL( KPublisherId, publisher );
        GetPropertyL( KContentType, content_type );
        GetPropertyL( KContentId, content_id );    
        
        if( ( publisher == KAll ) &&
        		( content_type == KAll ) &&
        		( content_id == KAll) )
        	{
        	result = ETrue;
        	}
        CleanupStack::PopAndDestroy( &content_id );
        CleanupStack::PopAndDestroy( &content_type );
        CleanupStack::PopAndDestroy( &publisher );
    	}
    return result;
    }

// ---------------------------------------------------------------------------
// 
// ---------------------------------------------------------------------------
//
void CCPLiwMap::RemoveAllAttributeL( TUint& aFilter ) const
    {
    RBuf publisher;
    publisher.CleanupClosePushL();
    RBuf content_type;
    content_type.CleanupClosePushL();  
    RBuf content_id;
    content_id.CleanupClosePushL();  
    GetPropertyL( KPublisherId, publisher );
    GetPropertyL( KContentType, content_type );
    GetPropertyL( KContentId, content_id );    
    
    if( publisher == KAll )
    	{
    	iMap->Remove( KPublisherId );
    	aFilter = aFilter & ~EPublisher;
    	}
    if( content_type == KAll )
    	{
    	iMap->Remove( KContentType );
    	aFilter = aFilter & ~EContentType;
    	}
    if( content_id == KAll )
    	{
    	iMap->Remove( KContentId );
    	aFilter = aFilter & ~EContentId;    	
    	}

    CleanupStack::PopAndDestroy( &content_id );
    CleanupStack::PopAndDestroy( &content_type );
    CleanupStack::PopAndDestroy( &publisher );
    
    }

// ---------------------------------------------------------------------------
// 
// ---------------------------------------------------------------------------
//
void CCPLiwMap::ConstructL()
    {
    iMap = CLiwGenericParamList::NewL( );
    }

// ---------------------------------------------------------------------------
// 
// ---------------------------------------------------------------------------
//
void CCPLiwMap::CheckIdentifiersL() const
    {
    IsPublisherNameL();
    IsContentTypeL();
    IsContentIdL();
    }

// ---------------------------------------------------------------------------
// 
// ---------------------------------------------------------------------------
//
void CCPLiwMap::BindTextL(RSqlStatement& aStmt, TInt aParameterIndex,
    const TDesC& aParameterText) const
    {
    User::LeaveIfError( aParameterIndex );
    User::LeaveIfError( aStmt.BindText( aParameterIndex, aParameterText ) );
    }
    
// ---------------------------------------------------------------------------
// 
// ---------------------------------------------------------------------------
//
void CCPLiwMap::BindBinaryL(RSqlStatement& aStmt, TInt aParameterIndex,
    const TDesC8& aParameterData) const
    {
    User::LeaveIfError(aParameterIndex);
    User::LeaveIfError(aStmt.BindBinary(aParameterIndex, aParameterData));
    }

// ---------------------------------------------------------------------------
// 
// ---------------------------------------------------------------------------
//
void CCPLiwMap::BindInt64L(RSqlStatement& aStmt, TInt aParameterIndex,
    TInt64 aParameterValue) const
    {
    User::LeaveIfError(aParameterIndex);
    User::LeaveIfError(aStmt.BindInt64(aParameterIndex, aParameterValue));
    }

// ---------------------------------------------------------------------------
// 
// ---------------------------------------------------------------------------
//
void CCPLiwMap::BindIntL(RSqlStatement& aStmt, TInt aParameterIndex,
    TInt aParameterValue) const
    {
    User::LeaveIfError(aParameterIndex);
    User::LeaveIfError(aStmt.BindInt(aParameterIndex, aParameterValue));
    }

// ---------------------------------------------------------------------------
// 
// ---------------------------------------------------------------------------
//
TInt CCPLiwMap::ColumnIndexL(RSqlStatement& aStmt, const TDesC& aColumnName )
	const
    {
    TInt ret( aStmt.ColumnIndex( aColumnName ) );
	User::LeaveIfError( ret );
    return ret;
    }


    
