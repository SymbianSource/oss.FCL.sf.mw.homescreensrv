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
 * Description: CASqlQueryCreator implementation
 *
 */

#include <e32cmn.h>
#include <badesca.h>

#include "casqlquerycreator.h"
#include "casqlcommands.h"
#include "cainnerentry.h"
#include "cainnerquery.h"
#include "caarraycleanup.inl"
#include "casqlquery.h"
#include "cadebug.h"
#include "cadef.h"
#include "caoperationparams.h"


// ---------------------------------------------------------------------------
//
// ---------------------------------------------------------------------------
//
void CaSqlQueryCreator::CreateAddQueryL( CCaInnerEntry* aEntry,
        RPointerArray<CCaSqlQuery>& aSqlQuery, RSqlDatabase& aSqlDb )
    {
    DEBUG(("_CA_:CASqlQueryCreator::CreateAddQueryL"));
    CCaSqlQuery* queryIcon = CCaSqlQuery::NewLC( aSqlDb );
    
    TIconType iconType = CaSqlQueryCreator::CheckIconType( aEntry );
    
    if( iconType == EProperIcon )
        {
        CreateAddIconQueryL( aEntry, queryIcon, aSqlDb );
        //add new icon to DB
        aSqlQuery.AppendL( queryIcon );
        CleanupStack::Pop( queryIcon );
        }
    else
        {
        // icon added to DB by another entry or not added
        CleanupStack::PopAndDestroy( queryIcon );
        queryIcon = NULL;
        }

    if( aEntry->GetId() != -1 )
        {
        // check if entry is new and has to be added to DB or just updated
        CCaSqlQuery* query = CCaSqlQuery::NewLC( aSqlDb );
        if( aEntry->GetId() > 0 )
            {
            //Update query is faster than replace query
            CreateUpdateQueryEntryL( aEntry, query );
            }
        else
            {
            // check if entry's Uid was set
            if( aEntry->GetUid() == 0 )
                {
                if( ( aEntry->GetIconId() == 0 ) && ( queryIcon == NULL ) )
                    {
                    query->SetQueryL( 
                            KSQLInsertToEntryUniqueUidWithoutIcon );
                    }
                else
                    {
                    query->SetQueryL( KSQLInsertToEntryUniqueUid );
                    }
                }
            else
                {
                if( ( aEntry->GetIconId() == 0 ) && ( queryIcon == NULL ) )
                    {
                    query->SetQueryL( KSQLInsertToEntryWithoutIcon );
                    }
                else
                    {
                    query->SetQueryL( KSQLInsertToEntry );
                    }
                }
            }
        query->SetTableType( CCaSqlQuery::EEntryTable );
        aSqlQuery.AppendL( query );
        CleanupStack::Pop( query );
        }
    // remove all entry's attributs from DB
    CCaSqlQuery* query = CCaSqlQuery::NewLC( aSqlDb );
    query->SetQueryL( KSQLDeleteAttribute );
    aSqlQuery.Append( query );
    query->SetTableType( CCaSqlQuery::EAttributeTable );
    CleanupStack::Pop( query );

    if( aEntry->GetAttributes().Count() > 0 )
        {
        CCaSqlQuery* query = CCaSqlQuery::NewLC( aSqlDb );
        query->SetQueryL( KSQLInsertToAttribute );
        aSqlQuery.AppendL( query );
        query->SetTableType( CCaSqlQuery::EAttributeTable );
        CleanupStack::Pop( query );
        }
    
    if( iconType == ENullIconToRemove )
        {
        RBuf queryRemoveIcon;
        queryRemoveIcon.CleanupClosePushL();
        queryRemoveIcon.CreateL( KSQLDeleteIconWhereIconId().Length() );
        queryRemoveIcon.Append( KSQLDeleteIconWhereIconId );
        queryRemoveIcon.ReAllocL( KSQLDeleteIconWhereIconId().Length() + KMaxIntNumLength );
        queryRemoveIcon.AppendNum( aEntry->GetIconId() );
        
        CCaSqlQuery* query = CCaSqlQuery::NewLC( aSqlDb );
        query->SetQueryL( queryRemoveIcon );
        aSqlQuery.AppendL( query );
        query->SetTableType( CCaSqlQuery::EIconTable );
        
        CleanupStack::Pop( query );
        CleanupStack::PopAndDestroy( &queryRemoveIcon );
        }
    }

// ---------------------------------------------------------------------------
//
// ---------------------------------------------------------------------------
//
void CaSqlQueryCreator::CreateUpdateQueryEntryL(
        const CCaInnerEntry* aEntry, CCaSqlQuery* aQuery )
    {
    DEBUG( ("_CA_:CASqlQueryCreator::CreateUpdateQueryEntryL") );
    RBuf query;
    query.CleanupClosePushL();
    query.CreateL( KSQLUpdateEntry().Length() );
    query.Append( KSQLUpdateEntry );
    // text
    if( aEntry->GetText().Length() != 0 )
        {
        query.ReAllocL( query.Length() + KSQLUpdateEntryText().Length()
                + KComma().Length() );
        query.Append( KSQLUpdateEntryText );
        query.Append( KComma );
        }
    // description
    query.ReAllocL( query.Length() + KSQLUpdateEntryDescription().Length()
            + KComma().Length() );
    query.Append( KSQLUpdateEntryDescription );
    query.Append( KComma );
    // role
    query.ReAllocL( query.Length() + KSQLUpdateEntryRole().Length()
            + KComma().Length() );
    query.Append( KSQLUpdateEntryRole );
    query.Append( KComma );
    // type_name
    if( aEntry->GetEntryTypeName().Length() != 0 )
        {
        query.ReAllocL( query.Length() + KSQLUpdateEntryTypeName().Length()
                + KComma().Length() );
        query.Append( KSQLUpdateEntryTypeName );
        query.Append( KComma );
        }
    // id_icon
    TIconType iconType = CaSqlQueryCreator::CheckIconType( aEntry );
    if( iconType == ENullIconToRemove || iconType == ENullIcon )
        {
        query.ReAllocL( query.Length( ) + 
                KSQLUpdateEntryIdIconNull().Length() + KComma().Length() );
        query.Append( KSQLUpdateEntryIdIconNull );
        }
    else
        { 
        query.ReAllocL( query.Length( ) + KSQLUpdateEntryIdIcon().Length()
                + KComma().Length() );
        query.Append( KSQLUpdateEntryIdIcon );
        }
    query.Append( KComma );
    // check if entry's Uid was set
    if( aEntry->GetUid() )
        {
        query.ReAllocL( query.Length() + KSQLUpdateEntryUid().Length()
                + KComma().Length() );
        query.Append( KSQLUpdateEntryUid );
        query.Append( KComma );
        }
    else
        {
        query.ReAllocL( query.Length() + KSQLUpdateEntryUidNULL().Length()
                + KComma().Length() );
        query.Append( KSQLUpdateEntryUidNULL );
        query.Append( KComma );
        }
    //flags
    query.ReAllocL( query.Length() + KSQLUpdateEntryFlags().Length()
            + KComma().Length() );
    query.Append( KSQLUpdateEntryFlags );

    query.ReAllocL( query.Length() + KSQLUpdateWhere().Length() );
    query.Append( KSQLUpdateWhere );

    aQuery->SetQueryL( query );
    CleanupStack::PopAndDestroy( &query );
    }
// ---------------------------------------------------------------------------
//
// ---------------------------------------------------------------------------
//
void CaSqlQueryCreator::CreateAddIconQueryForNewL( CCaInnerEntry* aEntry,
        CCaSqlQuery* aQuery, RSqlDatabase& aSqlDb )
    {
    DEBUG( ("_CA_:CASqlQueryCreator::CreateAddIconQueryForNewL") );
    CCaSqlQuery* query = CCaSqlQuery::NewLC( aSqlDb );
    query->SetQueryL( KSQLGetIconId );
    query->SetTableType( CCaSqlQuery::EIconTable );

    query->PrepareL();
    query->BindValuesForGetIconL( aEntry );
    TInt idIcon( 0 );
    query->ExecuteL( idIcon );
    CleanupStack::PopAndDestroy( query );

    if( idIcon == 0 )  
        {
        // entry's icon data is not in DB
        if( ( aEntry->GetIcon().iFileName.Length() != 0 ) ||
             ( aEntry->GetIcon().iBitmapId != 0 ) ||
             ( aEntry->GetIcon().iMaskId != 0 ) ||
             ( aEntry->GetIcon().iSkinMajorId != 0 ) ||
             ( aEntry->GetIcon().iSkinMinorId != 0 ) )
            {
            // icon is not null, so that insert it to storage
            aQuery->SetQueryL( KSQLInsertToIcon );
            aQuery->SetTableType( CCaSqlQuery::EIconTable );
            }
        else
            {
            // icon is null
            aQuery->SetTableType( CCaSqlQuery::ENoTableType );
            }
        }
    else if( idIcon > 0 )
        {
        aEntry->SetIconId( idIcon );
        aQuery->SetTableType( CCaSqlQuery::ENoTableType );
        }
    }
// ---------------------------------------------------------------------------
//
// ---------------------------------------------------------------------------
//
void CaSqlQueryCreator::CreateAddIconQueryForUpdateL(CCaInnerEntry* aEntry,
        CCaSqlQuery* aQuery, RSqlDatabase& aSqlDb )
    {
    DEBUG( ("_CA_:CASqlQueryCreator::CreateAddIconQueryForUpdateL") );
    CCaSqlQuery* query = CCaSqlQuery::NewLC( aSqlDb );
    query->SetQueryL( KSQLGetIconDataByIdIcon );
    query->SetTableType( CCaSqlQuery::EIconTable );

    query->PrepareL();
    query->BindValuesForGetEntriesL( aEntry );
    CCaInnerEntry::TIconAttributes iconAttributs;
    query->ExecuteL( iconAttributs );
    query->CloseStatement();
    if( !( !iconAttributs.iFileName.Compare( aEntry->GetIcon().iFileName )
            && ( iconAttributs.iBitmapId == aEntry->GetIcon().iBitmapId )
            && ( iconAttributs.iMaskId == aEntry->GetIcon().iMaskId )
            && ( iconAttributs.iSkinMajorId
                    == aEntry->GetIcon().iSkinMajorId )
            && ( iconAttributs.iSkinMinorId
                    == aEntry->GetIcon().iSkinMinorId ) ) )
        {
        // entry's icon data is updated
        // if icon is used by another entry(s), new icon'll added to DB and
        // new Id_Icon'll set to entry, otherwise it'll just updated
        query->SetQueryL( KSQLGetIconsItems );
        query->SetTableType( CCaSqlQuery::EEntryTable );

        query->PrepareL();
        query->BindValuesForGetEntriesL( aEntry );
        RPointerArray<CCaInnerEntry> innerEntries;
        CleanupResetAndDestroyPushL( innerEntries );
        TInt count = query->ExecuteL( innerEntries, CCaSqlQuery::EEntry );
        query->CloseStatement();
        if( count > 1 )
            {
            //icon's data is used by another entry(s)
            aQuery->SetQueryL( KSQLInsertToIcon );
            aQuery->SetTableType( CCaSqlQuery::EIconTable );
            }
        else
            {
            // icon's data is used just by this entry
            CreateUpdateIconQueryL( aEntry, aQuery );
            }

        CleanupStack::PopAndDestroy( &innerEntries );
        }
    CleanupStack::PopAndDestroy( query );
    }
// ---------------------------------------------------------------------------
//
// ---------------------------------------------------------------------------
//
void CaSqlQueryCreator::CreateAddIconQueryL( CCaInnerEntry* aEntry,
        CCaSqlQuery* aQuery, RSqlDatabase& aSqlDb )
    {
    DEBUG( ("_CA_:CASqlQueryCreator::CreateAddIconQueryL") );
    // check if entry's icon id was set
    if( aEntry->GetIconId() == 0 )
        {
        // if entry doesn't have id_icon yet
        // check if entry's icon data doesn't exist in DB
        CreateAddIconQueryForNewL( aEntry, aQuery, aSqlDb );
        }
    else if( aEntry->GetIconId() > 0 )
        {
        // entry has id_icon set
        // check if entry's icon data are the same as in database
        CreateAddIconQueryForUpdateL( aEntry, aQuery, aSqlDb );
        }
    }
// ---------------------------------------------------------------------------
//
// ---------------------------------------------------------------------------
//
void CaSqlQueryCreator::CreateUpdateIconQueryL(
        const CCaInnerEntry* aEntry, CCaSqlQuery* aQuery )
    {
    DEBUG(("_CA_:CASqlQueryCreator::CreateUpdateIconQueryL"));
    RBuf query;
    query.CleanupClosePushL();
    query.CreateL( KSQLUpdateIcon().Length() );
    query.Append( KSQLUpdateIcon );
    if( aEntry->GetIcon().iFileName.Compare( KNullDesC ) )
        {
        query.ReAllocL( query.Length() + KSQLUpdateIconFileName().Length()
                + KComma().Length() );
        query.Append( KSQLUpdateIconFileName );
        query.Append( KComma );
        }
    if( aEntry->GetIcon().iBitmapId )
        {
        query.ReAllocL( query.Length() + KSQLUpdateIconBitmapId().Length()
                + KComma().Length() );
        query.Append( KSQLUpdateIconBitmapId );
        query.Append( KComma );
        }
    if( aEntry->GetIcon().iMaskId )
        {
        query.ReAllocL( query.Length() + KSQLUpdateIconMaskId().Length()
                + KComma().Length() );
        query.Append( KSQLUpdateIconMaskId );
        query.Append( KComma );
        }
    if( aEntry->GetIcon().iSkinMajorId )
        {
        query.ReAllocL( query.Length() + KSQLUpdateIconSkinMajorId().Length()
                + KComma().Length() );
        query.Append( KSQLUpdateIconSkinMajorId );
        query.Append( KComma );
        }
    if( aEntry->GetIcon().iSkinMinorId )
        {
        query.ReAllocL( query.Length() + KSQLUpdateIconSkinMinorId().Length()
                + KComma().Length() );
        query.Append( KSQLUpdateIconSkinMinorId );
        }
    if( !query.Right( KComma().Length() ).Compare( KComma ) )
        {
        query.Assign( query.Mid( query.Length() - 
                KComma().Length()).AllocL() );
        }
    // add WHERE expr
    query.ReAllocL( query.Length() + KSQLUpdateIconWhere().Length() );
    query.Append( KSQLUpdateIconWhere );

    aQuery->SetQueryL( query );
    aQuery->SetTableType( CCaSqlQuery::EIconTable );

    CleanupStack::PopAndDestroy( &query );
    }

// ---------------------------------------------------------------------------
//
// ---------------------------------------------------------------------------
//
void CaSqlQueryCreator::CreateGetEntriesQueryL(
        const CCaInnerQuery* aQuery, CCaSqlQuery* aSqlQuery )
    {
    DEBUG( ("_CA_:CASqlQueryCreator::CreateGetEntriesQueryL") );
    if( aQuery->GetIds().Count() >= 1 )
        {
        //        just one or more ids so select just those items. easy case.
        CreateGetEntriesQueryByIdL( aQuery->GetIds().Count(), aSqlQuery );
        }
    else
        {
        //        create a sql query for a list of items to return.
        CreateGetEntriesQueryByQueryL( aQuery, aSqlQuery );
        }

    }

// ---------------------------------------------------------------------------
//
// ---------------------------------------------------------------------------
//
void CaSqlQueryCreator::CreateGetParentsIdsQueryL(
        const RArray<TInt>& aEntryIdArray, CCaSqlQuery* aSqlQuery,
        const RArray<TInt>& aParentIdArray )
    {
    DEBUG(("_CA_:CASqlQueryCreator::CreateGetParentsIdsQueryL"));
    RBuf entryIdList;
    entryIdList.CleanupClosePushL();
    CreateIdListL( aEntryIdArray.Count(), entryIdList, KSQLGEEntryId );
    RBuf parentIdList;
    parentIdList.CleanupClosePushL();
    CreateIdListL( aParentIdArray.Count(), parentIdList, KSQLGEIdGroup );

    RBuf query;
    query.CleanupClosePushL();
    query.CreateL( KSQLGetParentIds().Length() + entryIdList.Length() );
    query.AppendFormat( KSQLGetParentIds, &entryIdList );
    if( aParentIdArray.Count() > 0 )
        {
        query.ReAllocL( query.Length() + parentIdList.Length()
                + KSQLNotINIds().Length() );
        query.AppendFormat( KSQLNotINIds, &parentIdList );
        }

    aSqlQuery->SetQueryL( query );

    CleanupStack::PopAndDestroy( &query );
    CleanupStack::PopAndDestroy( &parentIdList );
    CleanupStack::PopAndDestroy( &entryIdList );
    }

// ---------------------------------------------------------------------------
//
// ---------------------------------------------------------------------------
//
void CaSqlQueryCreator::CreateGetAttributesQueryL(
        const TInt aIdEntryCount, CCaSqlQuery* aSqlQuery )
    {
    DEBUG(("_CA_:CASqlQueryCreator::CreateGetAttributesQueryL"));
    PopulateQueryWithIdListL( aIdEntryCount, aSqlQuery,
            KSQLGetAttributesByEntryId );
    }

// ---------------------------------------------------------------------------
//
// ---------------------------------------------------------------------------
//
void CaSqlQueryCreator::CreateRemoveQueryL(
        RPointerArray<CCaSqlQuery>& aSqlQuery, RSqlDatabase& aSqlDb )
    {
    DEBUG(("_CA_:CASqlQueryCreator::CreateRemoveQueryL"));

    CCaSqlQuery* query = CCaSqlQuery::NewLC( aSqlDb );
    query->SetQueryL( KSQLDeleteLaunch );
    aSqlQuery.Append( query );
    CleanupStack::Pop( query );

    query = CCaSqlQuery::NewLC( aSqlDb );
    query->SetQueryL( KSQLDeleteEntryFromGroup );
    aSqlQuery.Append( query );
    CleanupStack::Pop( query );

    query = CCaSqlQuery::NewLC( aSqlDb );
    query->SetQueryL( KSQLDeleteGroupFromGroup );
    aSqlQuery.Append( query );
    CleanupStack::Pop( query );

    query = CCaSqlQuery::NewLC( aSqlDb );
    query->SetQueryL( KSQLDeleteAttribute );
    aSqlQuery.Append( query );
    CleanupStack::Pop( query );

    query = CCaSqlQuery::NewLC( aSqlDb );
    query->SetQueryL( KSQLDeleteEntry );
    aSqlQuery.Append( query );
    CleanupStack::Pop( query );

    query = CCaSqlQuery::NewLC( aSqlDb );
    query->SetQueryL( KSQLDeleteIcon );
    query->SetTableType( CCaSqlQuery::EIconTable );
    aSqlQuery.Append( query );
    CleanupStack::Pop( query );
    }

// ---------------------------------------------------------------------------
//
// ---------------------------------------------------------------------------
//
void CaSqlQueryCreator::CreateTouchQueryL(
        RPointerArray<CCaSqlQuery>& aSqlQuery, RSqlDatabase& aSqlDb )
    {
    DEBUG(("_CA_:CASqlQueryCreator::CreateTouchQueryL"));

    CCaSqlQuery* query = CCaSqlQuery::NewLC( aSqlDb );
    query->SetQueryL( KSQLInsertToLaunch );
    aSqlQuery.Append( query );
    CleanupStack::Pop( query );

    query = CCaSqlQuery::NewLC( aSqlDb );
    query->SetQueryL( KSQLUpdateEntryUsedFlag );
    aSqlQuery.Append( query );
    CleanupStack::Pop( query );
    }

// ---------------------------------------------------------------------------
//
// ---------------------------------------------------------------------------
//
void CaSqlQueryCreator::CreateLocalizeEntryQueryL( CCaSqlQuery* aSqlQuery )
    {
    DEBUG(("_CA_:CASqlQueryCreator::CreateLocalizeEntryQueryL"));
    RBuf query;
    
    query.CleanupClosePushL();
    query.CreateL( KSQLLocalizeEntry().Length() );
    query.Append( KSQLLocalizeEntry );
    
    aSqlQuery->SetQueryL( query );
    CleanupStack::PopAndDestroy( &query );
    }

// ---------------------------------------------------------------------------
//
// ---------------------------------------------------------------------------
//
void CaSqlQueryCreator::CreateGetLocalizationsQueryL( CCaSqlQuery* aSqlQuery )
    {
    DEBUG(("_CA_:CASqlQueryCreator::CreateGetLocalizationsQueryL"));
    RBuf query;
    query.CleanupClosePushL();
    query.CreateL( KSQLGetLocalizations().Length() );
    query.Append( KSQLGetLocalizations );
    
    aSqlQuery->SetQueryL( query );
    CleanupStack::PopAndDestroy( &query );
    }

// ---------------------------------------------------------------------------
//
// ---------------------------------------------------------------------------
//
void CaSqlQueryCreator::CreateGetEntriesQueryByIdL(
        const TInt aIdEntryCount, CCaSqlQuery* aSqlQuery )
    {
    DEBUG(("_CA_:CASqlQueryCreator::CreateGetEntriesQueryByIdL"));
    PopulateQueryWithIdListL( aIdEntryCount, aSqlQuery,
            KSQLGetListByEntryIds );
    }

// ---------------------------------------------------------------------------
//
// ---------------------------------------------------------------------------
//
void CaSqlQueryCreator::PopulateQueryWithIdListL( const TInt aIdEntryCount,
        CCaSqlQuery* aQuery, const TDesC& aSqlQueryText )
    {
    DEBUG(("_CA_:CASqlQueryCreator::PopulateQueryWithIdListL"));
    RBuf entryIdList;
    entryIdList.CleanupClosePushL();

    CreateIdListL( aIdEntryCount, entryIdList, KSQLEntryId );

    RBuf query;
    query.CleanupClosePushL();
    query.CreateL( aSqlQueryText.Length() + entryIdList.Length() );
    query.AppendFormat( aSqlQueryText, &entryIdList );

    aQuery->SetQueryL( query );

    CleanupStack::PopAndDestroy( &query );
    CleanupStack::PopAndDestroy( &entryIdList );
    }

// ---------------------------------------------------------------------------
//
// ---------------------------------------------------------------------------
//
void CaSqlQueryCreator::CreateIdListL( const TInt aIdEntryCount,
        RBuf& aEntryIdList, const TDesC& aColName )
    {
    DEBUG(("_CA_:CASqlQueryCreator::CreateIdListL"));
    //    Create an list of ids seperated by commas
    if( aIdEntryCount > 0 )
        {
        aEntryIdList.CreateL( aIdEntryCount * ( aColName.Length()
                + KMaxIntNumLength ) - 1 );
        for( TInt i = 0; i < aIdEntryCount; i++ )
            {
            aEntryIdList.Append( aColName );
            aEntryIdList.AppendNum( i );
            if( i != aIdEntryCount - 1 )
                {
                aEntryIdList.Append( KComma );
                }
            }
        }
    }

// ---------------------------------------------------------------------------
//
// ---------------------------------------------------------------------------
//
void CaSqlQueryCreator::CreateGetEntriesQueryByQueryL(
        const CCaInnerQuery* aQuery, CCaSqlQuery* aSqlQuery )
    {
    DEBUG(("_CA_:CASqlQueryCreator::CreateGetEntriesQueryByQueryL"));
    RBuf whereStatement;
    whereStatement.CleanupClosePushL();

    TUint flagsOn = aQuery->GetFlagsOn();
    if( flagsOn != 0 )
        {
        whereStatement.ReAllocL( KAnd().Length() + whereStatement.Length()
                + KSQLGetEntryFlagsOn().Length() );
        whereStatement.Append( KAnd );
        whereStatement.Append( KSQLGetEntryFlagsOn );
        }

    TUint flagsOff = aQuery->GetFlagsOff();
    if( flagsOff != 0 )
        {
        whereStatement.ReAllocL( KAnd().Length() + whereStatement.Length()
                + KSQLGetEntryFlagsOff().Length() );
        whereStatement.Append( KAnd );
        whereStatement.Append( KSQLGetEntryFlagsOff );
        }

    TInt role = aQuery->GetRole();
    if( role != 0 )
        {
        whereStatement.ReAllocL( KAnd().Length() + whereStatement.Length()
                + KSQLGetEntryRole().Length() );
        whereStatement.Append( KAnd );
        whereStatement.Append( KSQLGetEntryRole );
        }

    TUint uid = aQuery->GetUid();
    if( uid != 0 )
        {
        whereStatement.ReAllocL( KAnd().Length() + whereStatement.Length()
                + KSQLGetEntryUid().Length() );
        whereStatement.Append( KAnd );
        whereStatement.Append( KSQLGetEntryUid );
        }

    const CDesC16ArrayFlat* typeNames = aQuery->GetEntryTypeNames();

    if( typeNames && typeNames->MdcaCount() > 0 )
        {
        RBuf typeNameWhereStatement;
        typeNameWhereStatement.CleanupClosePushL();
        for( TInt i = 0; i < typeNames->MdcaCount(); i++ )
            {
            typeNameWhereStatement.ReAllocL(
                    typeNameWhereStatement.Length()
                    + typeNames->MdcaPoint( i ).Length()
                    + KSingleQuotes().Length() * 2 + KComma().Length() );
            typeNameWhereStatement.Append( KSingleQuotes );
            typeNameWhereStatement.Append( typeNames->MdcaPoint( i ) );
            typeNameWhereStatement.Append( KSingleQuotes );
            if( i != typeNames->MdcaCount() - 1 )
                {
                typeNameWhereStatement.Append( KComma );
                }
            }
        whereStatement.ReAllocL( KAnd().Length()
                + KSQLGetEntryTypeNames().Length()
                + typeNameWhereStatement.Length()
                + whereStatement.Length() );
        whereStatement.Append( KAnd );
        whereStatement.AppendFormat( KSQLGetEntryTypeNames,
                &typeNameWhereStatement );
        CleanupStack::PopAndDestroy( &typeNameWhereStatement );
        }

    TInt groupId = aQuery->GetParentId();
    RBuf query;
    query.CleanupClosePushL();
    if( groupId > 0 )
        {
        query.CreateL( KSQLGetListByParentId().Length()
                + whereStatement.Length() - 2 );
        query.AppendFormat( KSQLGetListByParentId, &whereStatement );
        }
    else
        {
        query.CreateL( KSQLGetList().Length() );
        query.Append( KSQLGetList );
        if( whereStatement.Length() >= KAnd().Length() )
            {
            TPtrC ptrWhereStatement( whereStatement.Right(
                    whereStatement.Length() - KAnd().Length() ) );
            query.ReAllocL( KSQLGetList().Length() + KWhere().Length()
                    + ptrWhereStatement.Length() );
            query.Append( KWhere );
            query.Append( ptrWhereStatement );
            }
        }

    CCaInnerQuery::TSortAttribute sort = aQuery->GetSort();
    ModifyQueryForSortOrderL( sort, query, groupId > 0 );

    if( aQuery->GetCount() > 0 )
        {
        query.ReAllocL( query.Length() + KLimit().Length()
                + KMaxIntNumLength );
        query.Append( KLimit );
        TInt limitCount = aQuery->GetCount();
        query.AppendNum( limitCount );
        }

    aSqlQuery->SetQueryL( query );
    CleanupStack::PopAndDestroy( &query );
    CleanupStack::PopAndDestroy( &whereStatement );
    }


// ---------------------------------------------------------------------------
//
// ---------------------------------------------------------------------------
//
void CaSqlQueryCreator::CreateCustomSortQueryL(
        const RArray<TInt>& aEntryIds,
        RPointerArray<CCaSqlQuery>& aSqlQuery,
        RSqlDatabase& aSqlDb )
    {
    for( TInt i = 0; i < aEntryIds.Count(); i++ )
        {
        CCaSqlQuery* query = CCaSqlQuery::NewLC( aSqlDb );
        query->SetQueryL( KSQLUpdatePositionInGroup );
        aSqlQuery.Append( query );
        CleanupStack::Pop( query );
        }
    }


// ---------------------------------------------------------------------------
//
// ---------------------------------------------------------------------------
//
void CaSqlQueryCreator::ModifyQueryForSortOrderL(
        const CCaInnerQuery::TSortAttribute aSortType, RBuf& aQuery,
        TBool aSortByGroup )
    {
    DEBUG(("_CA_:CASqlQueryCreator::ModifyQueryForSortOrderL"));
    switch ( aSortType )
        {
        case CCaInnerQuery::Name:
            {
            aQuery.ReAllocL( aQuery.Length()
                    + KSQLSortOrderNameAsc().Length() );
            aQuery.Append( KSQLSortOrderNameAsc );
            break;
            }
        case CCaInnerQuery::NameDesc:
            {
            aQuery.ReAllocL( aQuery.Length()
                    + KSQLSortOrderNameDesc().Length() );
            aQuery.Append( KSQLSortOrderNameDesc );
            break;
            }
        case CCaInnerQuery::CreatedTimestamp:
            {
            aQuery.ReAllocL( aQuery.Length()
                    + KSQLSortOrderCreatedTimestampAsc().Length() );
            aQuery.Append( KSQLSortOrderCreatedTimestampAsc );
            break;
            }
        case CCaInnerQuery::CreatedTimestampDesc:
            {
            aQuery.ReAllocL( aQuery.Length()
                    + KSQLSortOrderCreatedTimestampDesc().Length() );
            aQuery.Append( KSQLSortOrderCreatedTimestampDesc );
            break;
            }
        case CCaInnerQuery::MostUsed:
        case CCaInnerQuery::MostUsedDesc:
        case CCaInnerQuery::LastUsed:
        case CCaInnerQuery::LastUsedDesc:
            {
            ModifyQueryForSortOrderLastMostUsedL( aSortType, aQuery );
            break;
            }
        case CCaInnerQuery::DefaultDesc:
            {
            if( aSortByGroup )
                {
                aQuery.ReAllocL( aQuery.Length()
                        + KSQLSortOrderDefaultDesc().Length() );
                aQuery.Append( KSQLSortOrderDefaultDesc );
                }
            else
                {
                aQuery.ReAllocL( aQuery.Length()
                        + KSQLSortOrderIdDesc().Length() );
                aQuery.Append( KSQLSortOrderIdDesc );
                }
            break;
            }
        case CCaInnerQuery::Default:
        default:
            {
            if( aSortByGroup )
                {
                aQuery.ReAllocL( aQuery.Length()
                        + KSQLSortOrderDefault().Length() );
                aQuery.Append( KSQLSortOrderDefault );
                }
            else
                {
                aQuery.ReAllocL( aQuery.Length()
                        + KSQLSortOrderIdAsc().Length() );
                aQuery.Append( KSQLSortOrderIdAsc );
                }
            break;
            }
        }
    }

// ---------------------------------------------------------------------------
//
// ---------------------------------------------------------------------------
//
void CaSqlQueryCreator::ModifyQueryForSortOrderLastMostUsedL(
        const CCaInnerQuery::TSortAttribute aSortType, RBuf& aQuery )
    {
    DEBUG(("_CA_:CASqlQueryCreator::ModifyQueryForSortOrderLastMostUsedL"));

    RBuf descPart;
    descPart.CleanupClosePushL();
    if( aSortType == CCaInnerQuery::MostUsedDesc || 
         aSortType == CCaInnerQuery::LastUsedDesc )
        {
        descPart.CreateL( KDesc );
        }
    else
        {
        descPart.CreateL( KNullDesC );
        }

    RBuf usageDataQuery;
    usageDataQuery.CleanupClosePushL();
    if( aSortType == CCaInnerQuery::MostUsed || 
         aSortType == CCaInnerQuery::MostUsedDesc )
        {
        usageDataQuery.CreateL( KSQLGetMostUsed );
        }
    else
        {
        usageDataQuery.CreateL( KSQLGetLastUsed );
        }

    RBuf newQuery;
    newQuery.CleanupClosePushL();
    newQuery.CreateL( KSQLGetListByLaunchDataPart1().Length() +
            KSQLGetListByLaunchDataPart1().Length() +
            KSQLGetListByLaunchDataPart3().Length() +
            usageDataQuery.Length() +
            descPart.Length() +
            aQuery.Length() );
    newQuery.Append( KSQLGetListByLaunchDataPart1 );
    newQuery.Append( aQuery );
    newQuery.Append( KSQLGetListByLaunchDataPart2 );
    newQuery.Append( usageDataQuery );
    newQuery.Append( KSQLGetListByLaunchDataPart3 );
    newQuery.Append( descPart );
    aQuery.Close();
    aQuery.CreateL( newQuery );

    CleanupStack::PopAndDestroy( &newQuery );
    CleanupStack::PopAndDestroy( &usageDataQuery );
    CleanupStack::PopAndDestroy( &descPart );
    }

// ---------------------------------------------------------------------------
//
// ---------------------------------------------------------------------------
//
TBool CaSqlQueryCreator::CreateOrganizeQueryL(
        const RArray<TInt>& aEntryIds, TCaOperationParams aParams,
        RSqlDatabase& aSqlDb, RPointerArray<CCaSqlQuery>& aSqlQueries )
    {
    DEBUG(("_CA_:CASqlQueryCreator::CreateOrganizeQueryL"));
    TBool revereseInsertOrder = false;
    TInt idCount( aEntryIds.Count() );

    CCaSqlQuery * query = CCaSqlQuery::NewLC( aSqlDb );
    PopulateQueryWithIdListL( idCount, query,
            KSQLOrganizeRemoveEntriesFromGroup );
    query->SetTableType( CCaSqlQuery::EGroupTableRemove );
    aSqlQueries.AppendL( query );
    CleanupStack::Pop( query );

    switch( aParams.iOperationType )
        {
        case TCaOperationParams::EInsert:

            {
            if( aParams.iBeforeEntryId <= 0 )
                {
                User::Leave( KErrNotSupported );
                }

            query = CCaSqlQuery::NewLC( aSqlDb );
            query->SetQueryL( KSQLOrganizeInsertToGroupBasic );
            query->SetTableType( CCaSqlQuery::EGroupTableInsert );
            aSqlQueries.AppendL( query );
            CleanupStack::Pop( query );
            break;
            }
        case TCaOperationParams::EAppend:
            {
            CCaSqlQuery * query = CCaSqlQuery::NewLC( aSqlDb );
            query->SetQueryL( KSQLOrganizeAppendToGroup );
            query->SetTableType( CCaSqlQuery::EGroupTableInsert );
            aSqlQueries.AppendL( query );
            CleanupStack::Pop( query );
            break;
            }
        case TCaOperationParams::EPrepend:
            {
            query = CCaSqlQuery::NewLC( aSqlDb );
            query->SetQueryL( KSQLOrganizeInsertToGroupPrepend );
            query->SetTableType( CCaSqlQuery::EGroupTableInsert );
            aSqlQueries.AppendL( query );
            CleanupStack::Pop( query );

            revereseInsertOrder = true;

            break;
            }
        case TCaOperationParams::ERemove:
            {
            break;
            }
        default:
            {
            User::Leave( KErrNotSupported );
            }
        }
    return revereseInsertOrder;
    }

// ---------------------------------------------------------------------------
//
// ---------------------------------------------------------------------------
//
CaSqlQueryCreator::TIconType CaSqlQueryCreator::CheckIconType( const CCaInnerEntry* aEntry )
    {
    CaSqlQueryCreator::TIconType iconType;
    if( aEntry->GetIcon().iBitmapId == 0 &&
        aEntry->GetIcon().iMaskId == 0 &&
        aEntry->GetIcon().iSkinMajorId == 0 &&
        aEntry->GetIcon().iSkinMinorId == 0 &&
        aEntry->GetIcon().iFileName.Length() == 0 )
        {
        if( aEntry->GetIconId() > 0 )
            {
            iconType = ENullIconToRemove;
            }
        else
            {
            iconType = ENullIcon;
            }
        }
    else
        {
        iconType = EProperIcon;
        }
    return iconType;
    }

