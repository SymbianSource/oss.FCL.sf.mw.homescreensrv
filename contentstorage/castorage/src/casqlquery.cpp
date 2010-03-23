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
 * Description: CCASqlQuery implementation
 *
 */

#include <sqldb.h>
#include "casqlquery.h"
#include "casqlcommands.h"
#include "cainnerquery.h"
#include "caentryattribute.h"
#include "casqlquerycreator.h"
#include "caoperationparams.h"
#include "cadef.h"
#include "calocalizationentry.h"

// ---------------------------------------------------------------------------
//
// ---------------------------------------------------------------------------
//
CCaSqlQuery::CCaSqlQuery( RSqlDatabase& aSql ) :
    iSqlDb( aSql )
    {
    }

// ---------------------------------------------------------------------------
//
// ---------------------------------------------------------------------------
//
CCaSqlQuery::~CCaSqlQuery()
    {
    iStatement.Close();
    iQuery.Close();
    }

// ---------------------------------------------------------------------------
//
// ---------------------------------------------------------------------------
//
CCaSqlQuery* CCaSqlQuery::NewLC( RSqlDatabase& aSql )
    {
    CCaSqlQuery* self = new ( ELeave ) CCaSqlQuery( aSql );
    CleanupStack::PushL( self );
    self->ConstructL();
    return self;
    }

// ---------------------------------------------------------------------------
//
// ---------------------------------------------------------------------------
//
CCaSqlQuery* CCaSqlQuery::NewL( RSqlDatabase& aSql )
    {
    CCaSqlQuery* self = CCaSqlQuery::NewLC( aSql );
    CleanupStack::Pop( self );
    return self;
    }

// ---------------------------------------------------------------------------
//
// ---------------------------------------------------------------------------
//
void CCaSqlQuery::ConstructL()
    {
    }

// ---------------------------------------------------------------------------
//
// ---------------------------------------------------------------------------
//
void CCaSqlQuery::PrepareL()
    {
    User::LeaveIfError( iStatement.Prepare( iSqlDb, iQuery ) );
    }
// ---------------------------------------------------------------------------
//
// ---------------------------------------------------------------------------
//
void CCaSqlQuery::CloseStatement()
    {
    iStatement.Close();
    }
// ---------------------------------------------------------------------------
//
// ---------------------------------------------------------------------------
//
void CCaSqlQuery::ResetStatement()
    {
    iStatement.Reset();
    }
// ---------------------------------------------------------------------------
//
// ---------------------------------------------------------------------------
//
void CCaSqlQuery::BindValuesForAddL( const CCaInnerEntry* aEntry )
    {
    switch ( Type() )
        {
        case EEntryTable: // = 1
            {
            BindValuesForAddEntryL( aEntry );
            break;
            }
        case EIconTable:
            {
            BindValuesForAddIconL( aEntry );
            break;
            }
        case EAttributeTable:
            {
            break;
            }
        default:
            User::Leave( KErrNotSupported );
        }
    }

// ---------------------------------------------------------------------------
//
// ---------------------------------------------------------------------------
//
void CCaSqlQuery::BindValuesForAddEntryL( const CCaInnerEntry* aEntry )
    {
    if( iQuery.Find( KSQLEnText ) != KErrNotFound )
        {
        BindTextL( iStatement.ParameterIndex( KSQLEnText ),
                aEntry->GetText() );
        }
    if( iQuery.Find( KSQLEnDescription )!= KErrNotFound )
        {
        BindTextL( iStatement.ParameterIndex( KSQLEnDescription ),
                aEntry->GetDescription() );
        }    
    if( iQuery.Find( KSQLEnRole ) != KErrNotFound )
        {
        BindIntL( iStatement.ParameterIndex( KSQLEnRole ),
                aEntry->GetRole() );
        }
    if( iQuery.Find( KSQLEnTypeName ) != KErrNotFound )
        {
        BindTextL( iStatement.ParameterIndex( KSQLEnTypeName ),
                aEntry->GetEntryTypeName() );
        }
    if( iQuery.Find( KSQLEnIdIcon ) != KErrNotFound )
        {
        BindIntL( iStatement.ParameterIndex( KSQLEnIdIcon ),
                aEntry->GetIconId() );
        }
    if( iQuery.Find( KSQLEnCreationTime ) != KErrNotFound )
        {
        TTime currentTime( 0 );
        currentTime.UniversalTime();
        BindInt64L( iStatement.ParameterIndex( KSQLEnCreationTime ),
                currentTime.Int64() );
        }
    if( iQuery.Find( KSQLEnUid ) != KErrNotFound )
        {
        BindIntL( iStatement.ParameterIndex( KSQLEnUid ),
                aEntry->GetUid() );
        }
    if( iQuery.Find( KSQLEnFlags )!= KErrNotFound )
        {
        BindIntL( iStatement.ParameterIndex( KSQLEnFlags ),
                aEntry->GetFlags() );
        }
    if( iQuery.Find( KSQLEntryId ) != KErrNotFound )
        {
        BindIntL( iStatement.ParameterIndex( KSQLEntryId ),
                aEntry->GetId() );
        }
    }

// ---------------------------------------------------------------------------
//
// ---------------------------------------------------------------------------
//
void CCaSqlQuery::BindValuesForAddIconL( const CCaInnerEntry* aEntry )
    {
    BindValuesForGetIconL( aEntry );
    }

// ---------------------------------------------------------------------------
//
// ---------------------------------------------------------------------------
//
void CCaSqlQuery::BindValuesForAddAttributeL( const CCaInnerEntry* aEntry,
        const CCaEntryAttribute* aAttr )
    {
    if( iQuery.Find( KSQLAttrEntryID ) != KErrNotFound )
        {
        BindIntL(iStatement.ParameterIndex( KSQLAttrEntryID ),
                aEntry->GetId() );
        }
    if( iQuery.Find( KSQLAttrName ) != KErrNotFound )
        {
        BindTextL( iStatement.ParameterIndex( KSQLAttrName ),
                aAttr->Name() );
        }
    if( iQuery.Find( KSQLAttrValue ) != KErrNotFound )
        {
        BindTextL( iStatement.ParameterIndex( KSQLAttrValue ),
                aAttr->Value() );
        }
    }

// ---------------------------------------------------------------------------
//
// ---------------------------------------------------------------------------
//
void CCaSqlQuery::BindValuesForOrganizeL( const RArray<TInt>& aEntryIds,
        TCaOperationParams& aParams, const TInt aEntryId )
    {
    if( aEntryIds.Count() > 0 )
        {
        BindIdsL( aEntryIds, KSQLEntryId );
        }
    if( iQuery.Find( KSQLGEIdGroup ) != KErrNotFound )
        {
        BindIntL( iStatement.ParameterIndex( KSQLGEIdGroup ),
                aParams.iGroupId );
        }
    if( iQuery.Find( KSQLEntryIdBefore ) != KErrNotFound )
        {
        BindIntL( iStatement.ParameterIndex( KSQLEntryIdBefore ),
                aParams.iBeforeEntryId );
        }
    if( iQuery.Find( KSQLGEEntryId ) != KErrNotFound )
        {
        BindIntL( iStatement.ParameterIndex( KSQLGEEntryId ), aEntryId );
        }
    if( iQuery.Find( KSQLShiftThreshold ) != KErrNotFound )
        {
        BindIntL( iStatement.ParameterIndex( KSQLShiftThreshold ),
                aEntryIds.Count() );
        }
    }

// ---------------------------------------------------------------------------
//
// ---------------------------------------------------------------------------
//
void CCaSqlQuery::BindValuesForLaunchL( const TInt aEntryId,
        const TInt64 aLaunchTime )
    {
    if( iQuery.Find( KSQLLaLaunchEntryId ) != KErrNotFound )
        {
        BindIntL( iStatement.ParameterIndex( 
                KSQLLaLaunchEntryId ), aEntryId );
        }
    if( iQuery.Find( KSQLLaLaunchTime ) != KErrNotFound )
        {
        BindInt64L( iStatement.ParameterIndex( 
                KSQLLaLaunchTime ), aLaunchTime );
        }
    if( iQuery.Find( KSQLEnFlags ) != KErrNotFound )
        {
        BindIntL( iStatement.ParameterIndex( KSQLEnFlags ), EUsed );
        }
    }

// ---------------------------------------------------------------------------
//
// ---------------------------------------------------------------------------
//
void CCaSqlQuery::BindValuesForRemoveL( const TInt aEntryId,
        const TInt aIconId )
    {
    if( iQuery.Find( KSQLEntryId )!= KErrNotFound )
        {
        BindIntL( iStatement.ParameterIndex( KSQLEntryId ), aEntryId );
        }
    if( iQuery.Find( KSQLIconId )!= KErrNotFound )
        {
        BindIntL( iStatement.ParameterIndex( KSQLIconId ), aIconId );
        }
    }

// ---------------------------------------------------------------------------
//
// ---------------------------------------------------------------------------
//
void CCaSqlQuery::BindValuesForCustomSortL( const TInt aGroupId, const TInt aEntryId,
        const TInt aNewPostion )
    {
    if( iQuery.Find( KSQLGEIdGroup )!= KErrNotFound )
        {
        BindIntL( iStatement.ParameterIndex( KSQLGEIdGroup ), aGroupId );
        }
    if( iQuery.Find( KSQLGEEntryId )!= KErrNotFound )
        {
        BindIntL( iStatement.ParameterIndex( KSQLGEEntryId ), aEntryId );
        }
    if( iQuery.Find( KSQLGEPosition )!= KErrNotFound )
        {
        BindIntL( iStatement.ParameterIndex( KSQLGEPosition ), aNewPostion );
        }
    }

// ---------------------------------------------------------------------------
//
// ---------------------------------------------------------------------------
//
void CCaSqlQuery::BindTextL( TInt aParameterIndex,
        const TDesC& aParameterValue )
    {
    User::LeaveIfError( iStatement.BindText( aParameterIndex,
            aParameterValue ) );
    }

// ---------------------------------------------------------------------------
//
// ---------------------------------------------------------------------------
//
void CCaSqlQuery::BindIntL( TInt aParameterIndex,
        TInt aParameterValue )
    {
    User::LeaveIfError( aParameterIndex );
    User::LeaveIfError( iStatement.BindInt( aParameterIndex,
            aParameterValue ) );
    }

// ---------------------------------------------------------------------------
//
// ---------------------------------------------------------------------------
//
void CCaSqlQuery::BindInt64L( TInt aParameterIndex,
        TInt64 aParameterValue )
    {
    User::LeaveIfError( aParameterIndex );
    User::LeaveIfError( iStatement.BindInt64( aParameterIndex,
            aParameterValue ) );
    }

// ---------------------------------------------------------------------------
//
// ---------------------------------------------------------------------------
//
void CCaSqlQuery::SetQueryL( const TDesC& aQuery )
    {
    iQuery.Close();
    iQuery.CreateL( aQuery );
    }


// ---------------------------------------------------------------------------
//
// ---------------------------------------------------------------------------
//
void CCaSqlQuery::BindValuesForLocalizeL( CCaLocalizationEntry& aLocalization )
    {
    if( iQuery.Find( KSQLLocRowId ) != KErrNotFound )
        {
        BindIntL( iStatement.ParameterIndex( 
        		KSQLLocRowId ), aLocalization.GetRowId() );
        }
    if( iQuery.Find( KSQLLocTable ) != KErrNotFound )
        {
        BindTextL(iStatement.ParameterIndex( KSQLLocTable ),
                aLocalization.GetTableName() );
        }
    if( iQuery.Find( KSQLLocAttribute ) != KErrNotFound )
        {
        BindTextL(iStatement.ParameterIndex( KSQLLocAttribute ),
                aLocalization.GetAttributeName() );
        }
    if( iQuery.Find( KSQLLocString ) != KErrNotFound )
        {
        BindTextL(iStatement.ParameterIndex( KSQLLocString ),
                aLocalization.GetLocalizedString() );
        }
    }

// ---------------------------------------------------------------------------
//
// ---------------------------------------------------------------------------
//
void CCaSqlQuery::BindValuesForPropertyL( const TDesC& aProperty )
    {
    if( iQuery.Find( KSQLDbProperty ) != KErrNotFound )
        {
        BindTextL( iStatement.ParameterIndex( 
                KSQLDbProperty ), aProperty );
        }
    }

// ---------------------------------------------------------------------------
//
// ---------------------------------------------------------------------------
//
void CCaSqlQuery::BindValuesForPropertyUpdateL( const TDesC& aProperty, const TDesC& aPropertyValue )
    {
    BindValuesForPropertyL( aProperty );
    
    if( iQuery.Find( KSQLDbValue ) != KErrNotFound )
        {
        BindTextL( iStatement.ParameterIndex( 
                KSQLDbValue ), aPropertyValue );
        }
    }

// ---------------------------------------------------------------------------
//
// ---------------------------------------------------------------------------
//
void CCaSqlQuery::BindValuesForGetEntriesL( const CCaInnerQuery* aQuery )
    {
    if( aQuery->GetIds().Count() > 0 )
        {
        BindIdsL( aQuery->GetIds(), KSQLEntryId );
        }
    else
        {
        if( aQuery->GetFlagsOn() != 0 && iQuery.Find( KSQLFlagsOn )
                != KErrNotFound )
            {
            BindIntL( iStatement.ParameterIndex( KSQLFlagsOn ),
                    aQuery->GetFlagsOn() );
            }
        if( aQuery->GetFlagsOff() != 0 && iQuery.Find( KSQLFlagsOff )
                != KErrNotFound )
            {
            BindIntL( iStatement.ParameterIndex( KSQLFlagsOff ),
                    aQuery->GetFlagsOff() );
            }
        if( aQuery->GetRole() != -1 && iQuery.Find( KSQLEnRole )
                != KErrNotFound )
            {
            BindIntL( iStatement.ParameterIndex( KSQLEnRole ),
                    aQuery->GetRole() );
            }
        if( aQuery->GetParentId() != -1 && iQuery.Find( KSQLGEIdGroup )
                != KErrNotFound )
            {
            BindIntL( iStatement.ParameterIndex( KSQLGEIdGroup ),
                    aQuery->GetParentId() );
            }
        if( aQuery->GetUid() != 0 && iQuery.Find( KSQLEnUid )
                != KErrNotFound )
            {
            BindIntL( iStatement.ParameterIndex( KSQLEnUid ),
                    aQuery->GetUid() );
            }
        }
    }

// ---------------------------------------------------------------------------
//
// ---------------------------------------------------------------------------
//
void CCaSqlQuery::BindValuesForGetEntriesL( const CCaInnerEntry* aEntry )
    {
    if( iQuery.Find( KSQLEntryId ) != KErrNotFound )
        {
        BindIntL(iStatement.ParameterIndex( KSQLEntryId ), 
                aEntry->GetId() );
        }
    if( iQuery.Find( KSQLEnText) != KErrNotFound )
        {
        BindTextL(iStatement.ParameterIndex( KSQLEnText ), 
                aEntry->GetText() );
        }
    if( iQuery.Find( KSQLEnDescription ) != KErrNotFound )
        {
        BindTextL(iStatement.ParameterIndex( KSQLEnDescription ),
                aEntry->GetDescription() );
        }    
    if( iQuery.Find( KSQLEnRole ) != KErrNotFound )
        {
        BindIntL( iStatement.ParameterIndex( KSQLEnRole ), 
                aEntry->GetRole() );
        }
    if( iQuery.Find( KSQLEnTypeName ) != KErrNotFound )
        {
        BindTextL( iStatement.ParameterIndex( KSQLEnTypeName ),
                aEntry->GetEntryTypeName() );
        }
    if( iQuery.Find( KSQLEnIdIcon ) != KErrNotFound )
        {
        BindIntL( iStatement.ParameterIndex( KSQLEnIdIcon ),
                aEntry->GetIconId() );
        }
    if( iQuery.Find( KSQLEnUid ) != KErrNotFound )
        {
        BindIntL( iStatement.ParameterIndex(KSQLEnUid), aEntry->GetUid() );
        }
    if( iQuery.Find( KSQLEnFlags ) != KErrNotFound )
        {
        BindIntL( iStatement.ParameterIndex( KSQLEnFlags ),
                aEntry->GetFlags() );
        }
    }
// ---------------------------------------------------------------------------
//
// ---------------------------------------------------------------------------
//
void CCaSqlQuery::BindValuesForGetIconL( const CCaInnerEntry* aEntry )
    {
    if( iQuery.Find( KSQLIconId ) != KErrNotFound )
        {
        BindIntL(iStatement.ParameterIndex( KSQLIconId ),
                aEntry->GetIcon().iId );
        }
    if( iQuery.Find( KSQLIcFileName ) != KErrNotFound )
        {
        BindTextL( iStatement.ParameterIndex( KSQLIcFileName ),
                aEntry->GetIcon().iFileName );
        }
    if( iQuery.Find( KSQLIcBitmapId ) != KErrNotFound )
        {
        BindIntL( iStatement.ParameterIndex( KSQLIcBitmapId ),
                aEntry->GetIcon().iBitmapId );
        }
    if( iQuery.Find(KSQLIcMaskId) != KErrNotFound )
        {
        BindIntL( iStatement.ParameterIndex( KSQLIcMaskId ),
                aEntry->GetIcon().iMaskId );
        }
    if( iQuery.Find( KSQLIcSkinMajorId ) != KErrNotFound )
        {
        BindIntL( iStatement.ParameterIndex( KSQLIcSkinMajorId ),
                aEntry->GetIcon().iSkinMajorId );
        }
    if( iQuery.Find( KSQLIcSkinMinorId ) != KErrNotFound )
        {
        BindIntL( iStatement.ParameterIndex( KSQLIcSkinMinorId ),
                aEntry->GetIcon().iSkinMinorId );
        }
    }

// ---------------------------------------------------------------------------
//
// ---------------------------------------------------------------------------
//
void CCaSqlQuery::BindEntryIdsL(
        const RPointerArray<CCaInnerEntry>& aEntryArray )
    {
    RArray<TInt> entryIdArray;
    CleanupClosePushL( entryIdArray );

    for( TInt i = 0; i < aEntryArray.Count(); i++ )
        {
        entryIdArray.AppendL( aEntryArray[i]->GetId() );
        }
    BindIdsL( entryIdArray, KSQLEntryId );
    CleanupStack::PopAndDestroy( &entryIdArray );
    }

// ---------------------------------------------------------------------------
//
// ---------------------------------------------------------------------------
//
void CCaSqlQuery::BindIdsL( const RArray<TInt>& aEntryIdArray,
        const TDesC& aColName )
    {
    RBuf bindKey;
    bindKey.CleanupClosePushL();
    bindKey.CreateL( aColName.Length() + KMaxIntNumLength );
    for( TInt i = 0; i < aEntryIdArray.Count() ; i++ )
        {
        bindKey.Copy( KNullDesC );
        bindKey.Append( aColName );
        bindKey.AppendNum( i );
        if( iQuery.Find( bindKey ) != KErrNotFound )
            {
            BindIntL( iStatement.ParameterIndex( bindKey ),
                    aEntryIdArray[i] );
            }
        }
    CleanupStack::PopAndDestroy( &bindKey );
    }

// ---------------------------------------------------------------------------
//
// ---------------------------------------------------------------------------
//
void CCaSqlQuery::BindEntryIdL( const TInt& aEntryId )
    {
    if( iQuery.Find( KSQLEntryId ) != KErrNotFound )
        {
        BindIntL( iStatement.ParameterIndex( KSQLEntryId ), aEntryId );
        }
    }

// ---------------------------------------------------------------------------
// CCaSqlQuery::BindValuesForRemoveFromLocalizationL(  const TInt aEntryId )
// ---------------------------------------------------------------------------
//
void CCaSqlQuery::BindValuesForRemoveFromLocalizationL(  const TInt aEntryId )
    {
    if( iQuery.Find( KSQLLocRowId ) != KErrNotFound )
        {
        BindIntL( iStatement.ParameterIndex( KSQLLocRowId ), aEntryId );
        }
    }

// ---------------------------------------------------------------------------
//
// ---------------------------------------------------------------------------
//
TInt CCaSqlQuery::ExecuteL()
    {
    User::LeaveIfError( iStatement.Exec() );
    TSqlScalarFullSelectQuery id_query( iSqlDb );
    return id_query.SelectIntL( KSQLGetLastInsertId );
    }

// ---------------------------------------------------------------------------
//
// ---------------------------------------------------------------------------
//
TInt CCaSqlQuery::ExecuteL( TInt& aId )
    {
    if( iStatement.Next() == KSqlAtRow )
        {
        aId = iStatement.ColumnInt( ColumnIndexL( iStatement,
                KColumnSingleData ) );
        return 1;
        }
    return 0;
    }

// ---------------------------------------------------------------------------
//
// ---------------------------------------------------------------------------
//
TInt CCaSqlQuery::ExecuteL( TInt64& aId )
    {
    if( iStatement.Next() == KSqlAtRow )
        {
        aId = iStatement.ColumnInt64(ColumnIndexL( iStatement,
                KColumnSingleData ) );
        return 1;
        }
    return 0;
    }

// ---------------------------------------------------------------------------
//
// ---------------------------------------------------------------------------
//
TInt CCaSqlQuery::ExecuteL( RPointerArray<CCaInnerEntry>& result,
        TQueryResultType aResultType )
    {
    TInt rowCount( 0 );
    if( aResultType==CCaSqlQuery::EEntry )
        {
        rowCount = ExecuteEntryL( result );
        }
    else // aResultType==CCaSqlQuery::EAttribute
        {
        rowCount = ExecuteAttributeL( result );
        }
    return rowCount;
    }

// ---------------------------------------------------------------------------
//
// ---------------------------------------------------------------------------
//
TInt CCaSqlQuery::ExecuteEntryL(
        RPointerArray<CCaInnerEntry>& aResultInput )
    {
    TInt rowCount(0);
    while( iStatement.Next() == KSqlAtRow )
        {
        TInt entryId = iStatement.
            ColumnInt( ColumnIndexL( iStatement, KColumnEntryId ) );
        TInt role = iStatement.
            ColumnInt( ColumnIndexL( iStatement, KColumnEnRole ) );
        //        TODO get NULL ???
        TInt32 uid = 
        iStatement.ColumnInt( ColumnIndexL( iStatement, KColumnEnUid ) ) 
        ? iStatement.ColumnInt( ColumnIndexL( iStatement, KColumnEnUid ) )
        : 0;
        TUint flags = iStatement.ColumnInt( ColumnIndexL( iStatement,
                KColumnEnFlags ) );
        TInt idIcon = iStatement.ColumnInt( ColumnIndexL( iStatement,
                KColumnEnIdIcon ) );
        TInt bitmapId = iStatement.ColumnInt( ColumnIndexL( iStatement,
                KColumnIcBitmapId ) );
        TInt maskId = iStatement.ColumnInt( ColumnIndexL( iStatement,
                KColumnIcMaskId ) );
        TInt skinMajorId = iStatement.ColumnInt( ColumnIndexL( iStatement,
                KColumnIcSkinMajorId ) );
        TInt skinMinorId = iStatement.ColumnInt( ColumnIndexL( iStatement,
                KColumnIcSkinMinorId ) );

        TPtrC text;
        User::LeaveIfError( iStatement. ColumnText( ColumnIndexL(
                iStatement, KColumnEnText ), text ) );
        TPtrC description;
        User::LeaveIfError( iStatement.
                ColumnText( ColumnIndexL( iStatement, KColumnEnDescription ),
                description) );      
        TPtrC typeName;
        User::LeaveIfError( iStatement.ColumnText( 
                ColumnIndexL(iStatement, KColumnEnTypeName ), typeName) );
        TPtrC iconFilename;
        User::LeaveIfError( iStatement.ColumnText( ColumnIndexL(
                iStatement, KColumnIcFileName ), iconFilename ) );

        CCaInnerEntry* result = CCaInnerEntry::NewLC();
        result->SetId( entryId );
        result->SetRole( role );
        result->SetIconDataL( bitmapId, maskId, skinMajorId, skinMinorId,
                iconFilename );
        result->SetIconId( idIcon );
        result->SetFlags( flags );
        result->SetUid( uid );
        result->SetTextL( text );
        result->SetDescriptionL( description );
        result->SetEntryTypeNameL( typeName );

        aResultInput.AppendL( result );
        CleanupStack::Pop( result );
        rowCount++;
        }//while

    return rowCount;
    }

// ---------------------------------------------------------------------------
//
// ---------------------------------------------------------------------------
//
TInt CCaSqlQuery::ExecuteLocalizationsL(
        RPointerArray<CCaLocalizationEntry>& aResultInput )
    {
    TInt rowCount(0);
    while( iStatement.Next() == KSqlAtRow )
        {
        TInt locTextId = iStatement.ColumnInt(
                ColumnIndexL( iStatement, KColumnLocTextId )
                );
        
        TInt rowId = iStatement.ColumnInt(
                ColumnIndexL( iStatement, KColumnLocRowId ) 
                );
         
        TPtrC tableName;
        User::LeaveIfError(
                iStatement.ColumnText(
                        ColumnIndexL( iStatement, KColumnLocTableName ),
                        tableName )
                        );
        
        TPtrC attribute;
        User::LeaveIfError(
                iStatement.ColumnText(
                        ColumnIndexL( iStatement, KColumnLocAttrName ),
                        attribute )
                        );  
        
        TPtrC stringId;
        User::LeaveIfError(
                iStatement.ColumnText(
                        ColumnIndexL( iStatement, KColumnLocStringId ),
                        stringId )
                        );
        
        CCaLocalizationEntry* result = CCaLocalizationEntry::NewLC( );
        result->SetTextId( locTextId );
        result->SetAttributeNameL( attribute );
        result->SetStringIdL( stringId );
        result->SetRowId( rowId );
        result->SetTableNameL( tableName );
        
        aResultInput.AppendL( result );
        CleanupStack::Pop( result );
        rowCount++;
        }//while

    return rowCount;
    }

// ---------------------------------------------------------------------------
//
// ---------------------------------------------------------------------------
//
TInt CCaSqlQuery::ExecutePropertyL( TDes& aPropertyValue )
    {
    TInt rowCount(0);
    while( iStatement.Next() == KSqlAtRow )
        {
        TPtrC propertyValue;
        User::LeaveIfError( iStatement. ColumnText(
                ColumnIndexL( iStatement, KColumnDbValue ), propertyValue ) );
        
        aPropertyValue.Copy( propertyValue );
        
        rowCount++;
        }//while

    return rowCount;
    }

// ---------------------------------------------------------------------------
//
// ---------------------------------------------------------------------------
//
TInt CCaSqlQuery::ExecuteAttributeL(
        RPointerArray<CCaInnerEntry>& aEntryArray )
    {
    TInt rowCount( 0 );
    while( iStatement.Next() == KSqlAtRow )
        {
        TInt idEntry = iStatement.ColumnInt(
                ColumnIndexL( iStatement, KColumnAttrEntryID ) );
        TPtrC name;
        User::LeaveIfError(iStatement.ColumnText(
                ColumnIndexL( iStatement, KColumnAttrName), name ) );
        TPtrC value;
        User::LeaveIfError(iStatement.ColumnText(
                ColumnIndexL( iStatement, KColumnAttrValue), value ) );
        for( TInt i = 0; i < aEntryArray.Count(); i++ )
            {
            if( idEntry == aEntryArray[i]->GetId() )
                {
                aEntryArray[i]->AddAttributeL( name, value );
                }
            }
        rowCount++;
        }
    return rowCount;
    }

// ---------------------------------------------------------------------------
//
// ---------------------------------------------------------------------------
//
TInt CCaSqlQuery::ExecuteL( RArray<TInt>& aResultIdArray,
        TTableType aType )
    {
    TInt rowCount( 0 );
    TPtrC columnId;
    if( aType == CCaSqlQuery::EEntryTable )
        {
        columnId.Set( KColumnEntryId );
        }
    else // aType==CCaSqlQuery::EGroupTable
        {
        columnId.Set( KColumnGEIdGroup );
        }
    while( iStatement.Next() == KSqlAtRow )
        {
        aResultIdArray.AppendL( iStatement.ColumnInt( ColumnIndexL(
                iStatement, columnId ) ) );
        rowCount++;
        }
    return rowCount;
    }

// ---------------------------------------------------------------------------
//
// ---------------------------------------------------------------------------
//
TInt CCaSqlQuery::ExecuteL( CCaInnerEntry::TIconAttributes& aIconAttributes )
    {
    TInt columnCount( 0 );
    while( iStatement.Next() == KSqlAtRow )
        {
        aIconAttributes.iId = iStatement.ColumnInt(
                ColumnIndexL( iStatement, KColumnIconId ) );
        User::LeaveIfError(iStatement.ColumnText(
                ColumnIndexL( iStatement, KColumnIcFileName),
                aIconAttributes.iFileName));
        aIconAttributes.iBitmapId = iStatement.ColumnInt(
                ColumnIndexL( iStatement, KColumnIcBitmapId ) );
        aIconAttributes.iMaskId = iStatement.ColumnInt(
                ColumnIndexL( iStatement, KColumnIcMaskId ) );
        aIconAttributes.iSkinMajorId = iStatement.ColumnInt(
                ColumnIndexL( iStatement, KColumnIcSkinMajorId ) );
        aIconAttributes.iSkinMinorId = iStatement.ColumnInt(
                ColumnIndexL( iStatement, KColumnIcSkinMinorId ) );
        columnCount++;
        }
    return columnCount;
    }

// ---------------------------------------------------------------------------
//
// ---------------------------------------------------------------------------
//
TInt CCaSqlQuery::ColumnIndexL( RSqlStatement& aStmt,
        const TDesC& aColumnName ) const
    {
    TInt ret( aStmt.ColumnIndex( aColumnName ) );
    User::LeaveIfError( ret );
    return ret;
    }
