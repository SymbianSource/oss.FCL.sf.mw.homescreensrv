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

#include "castorage.h"
#include "castorageproxy.h"
#include "castoragefactory.h"
#include "cainnerentry.h"
#include "cainnerquery.h"
#include "caarraycleanup.inl"
#include "calocalizationentry.h"
#include "casrvsession.h"



// ---------------------------------------------------------------------------
//
// ---------------------------------------------------------------------------
//
CCaStorageProxy::CCaStorageProxy()
    {
    }

// ---------------------------------------------------------------------------
//
// ---------------------------------------------------------------------------
//
void CCaStorageProxy::ConstructL()
    {
    iStorage = CaStorageFactory::NewDatabaseL();
    }

// ---------------------------------------------------------------------------
//
// ---------------------------------------------------------------------------
//
CCaStorageProxy* CCaStorageProxy::NewL()
    {
    CCaStorageProxy* self = CCaStorageProxy::NewLC();
    CleanupStack::Pop( self );
    return self;
    }

// ---------------------------------------------------------------------------
//
// ---------------------------------------------------------------------------
//
CCaStorageProxy* CCaStorageProxy::NewLC()
    {
    CCaStorageProxy* self = new ( ELeave ) CCaStorageProxy();
    CleanupStack::PushL( self );
    self->ConstructL();
    return self;
    }

// ---------------------------------------------------------------------------
//
// ---------------------------------------------------------------------------
//
CCaStorageProxy::~CCaStorageProxy()
    {
    delete iStorage;
    iHandlerNotifier.Close();
    }

// ---------------------------------------------------------------------------
//
// ---------------------------------------------------------------------------
//
EXPORT_C void CCaStorageProxy::GetEntriesL(const CCaInnerQuery* aQuery,
        RPointerArray<CCaInnerEntry>& aResultContainer )
    {
    iStorage->GetEntriesL( aQuery, aResultContainer );
    }

// ---------------------------------------------------------------------------
//
// ---------------------------------------------------------------------------
//
EXPORT_C void CCaStorageProxy::GetEntriesIdsL(const CCaInnerQuery* aQuery,
        RArray<TInt>& aResultIdArray)
    {
    iStorage->GetEntriesIdsL( aQuery, aResultIdArray );
    }

// ---------------------------------------------------------------------------
//
// ---------------------------------------------------------------------------
//
EXPORT_C void CCaStorageProxy::AddL( CCaInnerEntry* aEntry,
    TBool aUpdate,
    TItemAppearance aItemAppearanceChange )
    {
    TChangeType changeType = EAddChangeType;
    RArray<TInt> parentArray;
    CleanupClosePushL( parentArray );

    if( aEntry->GetId() > 0 )
        {
        changeType = EUpdateChangeType;
        RArray<TInt> id;
        CleanupClosePushL( id );
        id.AppendL( aEntry->GetId() );
        iStorage->GetParentsIdsL( id, parentArray );
        CleanupStack::PopAndDestroy( &id );
        }

    if( aItemAppearanceChange==EItemDisappeared )
        {
        changeType = ERemoveChangeType;
        }
    else if( aItemAppearanceChange==EItemAppeared )
        {
        changeType = EAddChangeType;
        }

    iStorage->AddL( aEntry, aUpdate );
    for( TInt i = 0; i < iHandlerNotifier.Count(); i++ )
        {
        iHandlerNotifier[i]->EntryChanged( aEntry, changeType, parentArray );
        }
    CleanupStack::PopAndDestroy( &parentArray );
    }

// ---------------------------------------------------------------------------
//
// ---------------------------------------------------------------------------
//
EXPORT_C void CCaStorageProxy::RemoveL( const RArray<TInt>& aEntryIds )
    {
    CCaInnerQuery* query = CCaInnerQuery::NewLC();
    query->SetIdsL( aEntryIds );
    RPointerArray<CCaInnerEntry> resultContainer;
    CleanupResetAndDestroyPushL( resultContainer );
    RArray<TInt> parentArray;
    CleanupClosePushL( parentArray );
    if( aEntryIds.Count() > 0 )
        {
        iStorage->GetEntriesL( query, resultContainer );
        iStorage->GetParentsIdsL( aEntryIds, parentArray );
        }
    iStorage->RemoveL( aEntryIds );
    for( TInt i( 0 ); i < resultContainer.Count(); i++ )
        {
        for( TInt j( 0 ); j < iHandlerNotifier.Count(); j++ )
            {
            iHandlerNotifier[j]->EntryChanged( resultContainer[i],
                    ERemoveChangeType, parentArray );
            }
        }
    CleanupStack::PopAndDestroy( &parentArray );
    CleanupStack::PopAndDestroy( &resultContainer );
    CleanupStack::PopAndDestroy( query );
    }

// ---------------------------------------------------------------------------
//
// ---------------------------------------------------------------------------
//
EXPORT_C void CCaStorageProxy::OrganizeL( const RArray<TInt>& aEntryIds,
        TCaOperationParams aParams )
    {
    iStorage->OrganizeL( aEntryIds, aParams );
    RArray<TInt> parentArray;
    CleanupClosePushL( parentArray );
    parentArray.AppendL( aParams.iGroupId );
    iStorage->GetParentsIdsL( parentArray, parentArray );
    for( TInt i = 0; i < iHandlerNotifier.Count(); i++ )
        {
        iHandlerNotifier[i]->GroupContentChanged( parentArray );
        }
    CleanupStack::PopAndDestroy( &parentArray );
    }

// ---------------------------------------------------------------------------
//
// ---------------------------------------------------------------------------
//
EXPORT_C void CCaStorageProxy::TouchL( CCaInnerEntry* aEntry )
    {
    CCaInnerQuery* touchQuery = CCaInnerQuery::NewLC();

    TInt entryId = aEntry->GetId();

    if ( entryId == 0 && aEntry->GetUid() != 0)
        {
        CCaInnerQuery* idQuery = CCaInnerQuery::NewLC();
        idQuery->SetUid( static_cast<TUint>( aEntry->GetUid()) );

        RArray<TInt> idArray;
        CleanupClosePushL( idArray );

        iStorage->GetEntriesIdsL( idQuery, idArray );

        if (idArray.Count() == 1 )
            {
            entryId = idArray[0];
            aEntry->SetId( entryId );
            }

        CleanupStack::PopAndDestroy( &idArray );
        CleanupStack::PopAndDestroy( idQuery );
        }

    RArray<TInt> id;
    CleanupClosePushL( id );
    id.AppendL( entryId );
    touchQuery->SetIdsL( id );
    RPointerArray<CCaInnerEntry> resultArray;
    CleanupResetAndDestroyPushL( resultArray );
    iStorage->GetEntriesL( touchQuery, resultArray );
    iStorage->TouchL( entryId );
    for( TInt i = 0; i < iHandlerNotifier.Count(); i++ )
        {
        iHandlerNotifier[i]->EntryTouched( entryId );
        }
    if( resultArray.Count() > 0 )
        {
        if( !( resultArray[0]->GetFlags() & EUsed ) )
            {
            RArray<TInt> parentArray;
            CleanupClosePushL( parentArray );
            iStorage->GetParentsIdsL( id, parentArray );
            for( TInt i = 0; i < iHandlerNotifier.Count(); i++ )
                {
                iHandlerNotifier[i]->EntryChanged( resultArray[0],
                        EUpdateChangeType,
                        parentArray );
                }
            CleanupStack::PopAndDestroy( &parentArray );
            }
        }
    CleanupStack::PopAndDestroy( &resultArray );
    CleanupStack::PopAndDestroy( &id );
    CleanupStack::PopAndDestroy( touchQuery );
    }

// ---------------------------------------------------------------------------
//
// ---------------------------------------------------------------------------
//
EXPORT_C void CCaStorageProxy::GetLocalizationsL(
        RPointerArray<CCaLocalizationEntry>& aResultArray )
    {
    iStorage->GetLocalizationsL( aResultArray );
    }

// ---------------------------------------------------------------------------
//
// ---------------------------------------------------------------------------
//
EXPORT_C void CCaStorageProxy::LocalizeEntryL(
        CCaLocalizationEntry& aLocalization )
    {
    iStorage->LocalizeEntryL( aLocalization );
    }

// ---------------------------------------------------------------------------
//
// ---------------------------------------------------------------------------
//
EXPORT_C void CCaStorageProxy::AddLocalizationL(
		const CCaLocalizationEntry& aLocalization)
	{
	iStorage->AddLocalizationL(aLocalization);
	}

// ---------------------------------------------------------------------------
//
// ---------------------------------------------------------------------------
//
EXPORT_C void CCaStorageProxy::DbPropertyL( const TDesC& aProperty,
        TDes& aPropertyValue )
    {
    iStorage->DbPropertyL( aProperty, aPropertyValue );
    }

// ---------------------------------------------------------------------------
//
// ---------------------------------------------------------------------------
//
EXPORT_C void CCaStorageProxy::SetDBPropertyL( const TDesC& aProperty,
        const TDesC& aPropertyValue )
    {
    iStorage->SetDBPropertyL( aProperty, aPropertyValue );
    }

// ---------------------------------------------------------------------------
//
// ---------------------------------------------------------------------------
//
EXPORT_C void CCaStorageProxy::CustomSortL( const RArray<TInt>& aEntryIds,
        const TInt aGroupId )
    {
    iStorage->CustomSortL( aEntryIds, aGroupId );

    RArray<TInt> parentArray;
    CleanupClosePushL( parentArray );
    parentArray.AppendL( aGroupId );
    for( TInt i = 0; i < iHandlerNotifier.Count(); i++ )
        {
        iHandlerNotifier[i]->GroupContentChanged( parentArray );
        }
    CleanupStack::PopAndDestroy( &parentArray );
    }

// ---------------------------------------------------------------------------
//
// ---------------------------------------------------------------------------
//
EXPORT_C void CCaStorageProxy::LoadDataBaseFromRomL()
    {
    iStorage->LoadDataBaseFromRomL();
    }

// ---------------------------------------------------------
//
// ---------------------------------------------------------
//
void CCaStorageProxy::AddSessionL( MCaSessionNorifier* aHandlerNotifier )
    {
    __ASSERT_ALWAYS(
            iHandlerNotifier.Find( aHandlerNotifier ) == KErrNotFound,
            User::Invariant() );
    iHandlerNotifier.AppendL( aHandlerNotifier );
    }

// ---------------------------------------------------------
//
// ---------------------------------------------------------
//
void CCaStorageProxy::RemoveSession( MCaSessionNorifier* aHandlerNotifier )
    {
    TInt i = iHandlerNotifier.Find( aHandlerNotifier );
    if( i != KErrNotFound )
        {
        iHandlerNotifier.Remove( i );
        }
    }
