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
// INCLUDE FILES

#include <badesca.h>
#include "widgetscannerutils.h"
#include "cawidgetstoragehandler.h"
#include "cadef.h"
#include "cainnerentry.h"
#include "cainnerquery.h"
#include "castorageproxy.h"
#include "caarraycleanup.inl"

// ============================ MEMBER FUNCTIONS ===============================

// -----------------------------------------------------------------------------
// CCaWidgetStorageHandler::CCaWidgetStorageHandler
// C++ default constructor can NOT contain any code, that
// might leave.
// -----------------------------------------------------------------------------
//
CCaWidgetStorageHandler::CCaWidgetStorageHandler(
        CCaStorageProxy* aStorage, RFs& aFs )
    {
    iStorage = aStorage;
    iFs = aFs;
    }

// -----------------------------------------------------------------------------
// CCaWidgetStorageHandler::ConstructL
// Symbian 2nd phase constructor can leave.
// -----------------------------------------------------------------------------
//
void CCaWidgetStorageHandler::ConstructL()
    {
    }

// -----------------------------------------------------------------------------
// CCaWidgetStorageHandler::NewL
// Two-phased constructor.
// -----------------------------------------------------------------------------
//
CCaWidgetStorageHandler* CCaWidgetStorageHandler::NewL(
        CCaStorageProxy* aStorage, RFs& aFs )
    {
    CCaWidgetStorageHandler* self = NewLC( aStorage, aFs );
    CleanupStack::Pop( self );
    return self;
    }

// -----------------------------------------------------------------------------
// CCaWidgetStorageHandler::NewLC
// Two-phased constructor.
// -----------------------------------------------------------------------------
//
CCaWidgetStorageHandler* CCaWidgetStorageHandler::NewLC(
        CCaStorageProxy* aStorage, RFs& aFs )
    {
    CCaWidgetStorageHandler* self = new ( ELeave ) CCaWidgetStorageHandler(
            aStorage, aFs );
    CleanupStack::PushL( self );
    self->ConstructL();
    return self;
    }

// -----------------------------------------------------------------------------
// Destructor
// -----------------------------------------------------------------------------
//
CCaWidgetStorageHandler::~CCaWidgetStorageHandler()
    {
    iWidgets.ResetAndDestroy();
    iUpdatedIndexes.Close();
    }

// ----------------------------------------------------------------------------
// 
// ----------------------------------------------------------------------------
//  
void CCaWidgetStorageHandler::SynchronizeL( const RWidgetArray& aWidgets )
    {
    FetchWidgetsL();
    AddWidgetsL( aWidgets );
    RemoveWidgetsL();
    }

// ----------------------------------------------------------------------------
// 
// ----------------------------------------------------------------------------
//  
void CCaWidgetStorageHandler::AddL( const CCaWidgetDescription* aWidget )
    {
    CCaInnerEntry* entry = aWidget->GetEntryLC();
    iStorage->AddL( entry );
    if( ( entry->GetFlags() & ERemovable ) != 0 )
        {
        AddWidgetToDownloadCollectionL( entry );
        }
    CleanupStack::PopAndDestroy( entry );
    }

// ----------------------------------------------------------------------------
// 
// ----------------------------------------------------------------------------
//  
void CCaWidgetStorageHandler::UpdateL( const CCaWidgetDescription* aWidget,
        TUint aEntryId )
    {
    CCaInnerEntry* entry = aWidget->GetEntryLC();
    entry->SetId( aEntryId );
    if( !aWidget->IsMissing() && ( aWidget->GetLibrary() != KNoLibrary ) )
        {
        entry->SetFlags( entry->GetFlags() & ~EUsed );
        }
    else if(aWidget->IsUsed())
        {
        entry->SetFlags( entry->GetFlags() | EUsed );
        }
    entry->SetFlags( entry->GetFlags() & ~EMissing );
    iStorage->AddL( entry );
    if( !aWidget->IsMissing() )
        {
        AddWidgetToDownloadCollectionL( entry );
        }
    CleanupStack::PopAndDestroy( entry );
    }

// ----------------------------------------------------------------------------
// 
// ----------------------------------------------------------------------------
//  
void CCaWidgetStorageHandler::AddWidgetsL( const RWidgetArray& aWidgets )
    {
    iUpdatedIndexes.Reset();
    for( TInt i = 0; i < aWidgets.Count(); i++ )
        {
        TInt index = iWidgets.Find( 
                aWidgets[i], CCaWidgetDescription::Compare );
        if( index != KErrNotFound )
            {
            if( !iWidgets[index]->Compare( *aWidgets[i] ) || 
                    iWidgets[index]->IsMissing() )
                {
                aWidgets[i]->SetMissing( iWidgets[index]->IsMissing() );
                aWidgets[i]->SetUsed( iWidgets[index]->IsUsed() );
                UpdateL( aWidgets[i], iWidgets[index]->GetEntryId() );
                }
            iUpdatedIndexes.AppendL( index );
            }
        else
            {
            AddL( aWidgets[i] );
            }
        }
    }

// ----------------------------------------------------------------------------
// 
// ----------------------------------------------------------------------------
//  
void CCaWidgetStorageHandler::RemoveWidgetsL()
    {
    RArray<TInt> widgetsToRemove;
    CleanupClosePushL( widgetsToRemove );
    for( TInt i = 0; i < iWidgets.Count(); i++ )
        {
        if( iUpdatedIndexes.Find( i ) == KErrNotFound )
            {
            if( iWidgets[i]->GetMmcId() && 
                    ( iWidgets[i]->GetMmcId() != 
                    WidgetScannerUtils::CurrentMmcId( iFs ) ) )
                {
                SetMissingFlagL( iWidgets[i] );
                }
            else
                {
                widgetsToRemove.AppendL( iWidgets[i]->GetEntryId() );
                }
            }
        }
    if( widgetsToRemove.Count() > 0 )
        {
        iStorage->RemoveL( widgetsToRemove );
        }
    CleanupStack::PopAndDestroy( &widgetsToRemove );
    }

// ----------------------------------------------------------------------------
// 
// ----------------------------------------------------------------------------
// 
void CCaWidgetStorageHandler::AddWidgetToDownloadCollectionL(
        const CCaInnerEntry* aEntry )
    {
    CCaInnerQuery* queryDownload = CCaInnerQuery::NewLC();
    CDesC16ArrayFlat* downloadType = new ( ELeave ) CDesC16ArrayFlat(
            KGranularityOne );
    CleanupStack::PushL( downloadType );
    downloadType->AppendL( KCaTypeCollectionDownload );
    queryDownload->SetEntryTypeNames( downloadType );//query takes ownership
    CleanupStack::Pop( downloadType );
    queryDownload->SetRole( EGroupEntryRole );

    RArray<TInt> idsGroup;
    CleanupClosePushL( idsGroup );
    iStorage->GetEntriesIdsL( queryDownload, idsGroup );

    TCaOperationParams operParams;
    operParams.iOperationType = TCaOperationParams::EPrepend;
    operParams.iGroupId = idsGroup[0];
    operParams.iBeforeEntryId = 0;
    CleanupStack::PopAndDestroy( &idsGroup );

    RArray<TInt> idsEntry;
    CleanupClosePushL( idsEntry );
    idsEntry.Append( aEntry->GetId() );
    iStorage->OrganizeL( idsEntry, operParams );

    CleanupStack::PopAndDestroy( &idsEntry );
    CleanupStack::PopAndDestroy( queryDownload );
    }

// ----------------------------------------------------------------------------
// 
// ----------------------------------------------------------------------------
//  
void CCaWidgetStorageHandler::FetchWidgetsL()
    {
    CCaInnerQuery* query = CCaInnerQuery::NewLC();
    CDesC16ArrayFlat* widgetType = new ( ELeave ) CDesC16ArrayFlat(
            KGranularityOne );
    CleanupStack::PushL( widgetType );
    widgetType->AppendL( KCaTypeWidget );
    query->SetEntryTypeNames( widgetType );//transfers ownership to query
    CleanupStack::Pop( widgetType );

    RPointerArray<CCaInnerEntry> entries;
    CleanupResetAndDestroyPushL( entries );
    iStorage->GetEntriesL( query, entries );
    iWidgets.ResetAndDestroy();
    for( TInt i = 0; i < entries.Count(); i++ )
        {
        CCaWidgetDescription* widget = CCaWidgetDescription::NewLC(
                entries[i] );
        iWidgets.AppendL( widget ); //iWidgets takes ownership
        CleanupStack::Pop( widget );
        }
    CleanupStack::PopAndDestroy( &entries );
    CleanupStack::PopAndDestroy( query );
    }

// ----------------------------------------------------------------------------
// 
// ----------------------------------------------------------------------------
//  
void CCaWidgetStorageHandler::SetMissingFlagL(
        const CCaWidgetDescription* aWidget )
    {
    CCaInnerEntry* entry = aWidget->GetEntryLC();
    entry->SetFlags( entry->GetFlags() | EMissing );
    if( aWidget->IsUsed() )
        {
        entry->SetFlags( entry->GetFlags() | EUsed );
        }
    iStorage->AddL( entry );
    CleanupStack::PopAndDestroy( entry );
    }

//  End of File  
