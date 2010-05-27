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
#include <usif/scr/screntries.h>
#include "widgetscannerutils.h"
#include "cawidgetstoragehandler.h"
#include "cadef.h"
#include "cainnerentry.h"
#include "cainnerquery.h"
#include "castorageproxy.h"
#include "caarraycleanup.inl"

using namespace Usif;

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
    User::LeaveIfError( iSoftwareRegistry.Connect() );
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
    iSoftwareRegistry.Close();
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
    UpdateComponentIdL( aWidget->GetManifestFilePathName(), *entry );
    iStorage->AddL( entry );
    if( entry->GetFlags() & ERemovable )
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
    UpdateComponentIdL( aWidget->GetManifestFilePathName(), *entry );
    entry->SetId( aEntryId );
    if( !aWidget->IsMissing() && aWidget->IsUsed() )
        {
        entry->SetFlags( entry->GetFlags() & ~EUsed );
        }
    TItemAppearance itemAppearanceChange = EItemAppearanceNotChanged;
    if( ( entry->GetFlags() & EMissing ) ||
        !( entry->GetFlags() & EVisible ) )
        {
        itemAppearanceChange = EItemAppeared;
        }
    entry->SetFlags( entry->GetFlags() & ~EMissing | EVisible );
    iStorage->AddL( entry, EFalse, itemAppearanceChange );
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
                aWidgets[i]->SetVisible( iWidgets[index]->IsVisible() );
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
    for( TInt i = 0; i < iWidgets.Count(); i++ )
        {
        if( iUpdatedIndexes.Find( i ) == KErrNotFound )
            {
            if( iWidgets[i]->GetMmcId() != KNullDesC )
                {
                RBuf currentMmcId;
                currentMmcId.CreateL( KMassStorageIdLength );
                currentMmcId.CleanupClosePushL();
                WidgetScannerUtils::CurrentMmcId( iFs, currentMmcId );
                if( iWidgets[i]->GetMmcId() == currentMmcId ||
                        ( iWidgets[i]->GetMmcId() == KCaMassStorage() &&
                        MassStorageNotInUse() ) )
                    {
                    //item was uninstalled so we remove its mmc id
                    iWidgets[i]->RemoveMmcId();
                    ClearVisibleFlagL( iWidgets[i] );
                    }
                else
                    {
                    SetMissingFlagL( iWidgets[i] );
                    }
                CleanupStack::PopAndDestroy(&currentMmcId);
                }
            else
                {
                //item was uninstalled so we remove its mmc id
                ClearVisibleFlagL( iWidgets[i] );
                }
            }
        }
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
    if( !aWidget->IsMissing() )
        {
        CCaInnerEntry* entry = aWidget->GetEntryLC();
        entry->SetFlags( entry->GetFlags() | EMissing );
        if( aWidget->IsUsed() )
            {
            entry->SetFlags( entry->GetFlags() | EUsed );
            }
        iStorage->AddL( entry, EFalse, EItemDisappeared );
        CleanupStack::PopAndDestroy( entry );
        }
    }

// ----------------------------------------------------------------------------
//
// ----------------------------------------------------------------------------
//
void CCaWidgetStorageHandler::ClearVisibleFlagL(
        const CCaWidgetDescription* aWidget )
    {
    if( aWidget->IsVisible() )
        {
        CCaInnerEntry* entry = aWidget->GetEntryLC();
        entry->SetFlags( entry->GetFlags() & ~EVisible & ~EMissing & ~EUsed );
        iStorage->AddL( entry, EFalse, EItemDisappeared );
        CleanupStack::PopAndDestroy( entry );
        }
    }

// ----------------------------------------------------------------------------
//
// ----------------------------------------------------------------------------
//
TBool CCaWidgetStorageHandler::MassStorageNotInUse()
    {
    TBool massStorageNotInUse( ETrue );
    TInt drive;
    if( DriveInfo::GetDefaultDrive( DriveInfo::EDefaultMassStorage, drive ) ==
        KErrNone )
        {
        TUint status;
        if( DriveInfo::GetDriveStatus( iFs, drive, status ) == KErrNone &&
            ( status & DriveInfo::EDriveInUse ) )
            {
            massStorageNotInUse = EFalse;
            }
        }
    return massStorageNotInUse;
    }

// ----------------------------------------------------------------------------
//
// ----------------------------------------------------------------------------
//

void CCaWidgetStorageHandler::UpdateComponentIdL( 
    const TDesC& aManifestFilePathName, 
    CCaInnerEntry& aEntry ) const
    {
    RArray<TComponentId> componentIds;
    CleanupClosePushL( componentIds );
    
    CComponentFilter* const fileNameFilter = CComponentFilter::NewLC();
    fileNameFilter->SetFileL( aManifestFilePathName );
    iSoftwareRegistry.GetComponentIdsL( componentIds, fileNameFilter );

    CleanupStack::PopAndDestroy( fileNameFilter );
    
    if ( componentIds.Count() == 1 )
        {
        RBuf newComponentId;
        newComponentId.CleanupClosePushL();
        newComponentId.CreateL( sizeof(TComponentId) + 1 );
        newComponentId.AppendNum( componentIds[0] );
        
        RBuf oldComponentId;
        oldComponentId.CleanupClosePushL();
        oldComponentId.CreateL( KCaMaxAttrValueLen );
        
        const TBool componentIdAttributeFound = 
            aEntry.FindAttribute( KCaComponentId, oldComponentId );
              
        if ( !componentIdAttributeFound 
            || oldComponentId.Compare( newComponentId ) != 0 )
            {
            // 'add' or 'update' the component id attribute value
            aEntry.AddAttributeL( KCaComponentId, 
                newComponentId );
            }
        
        CleanupStack::PopAndDestroy( &oldComponentId );
        CleanupStack::PopAndDestroy( &newComponentId );
        }

    CleanupStack::PopAndDestroy( &componentIds );
    }


//  End of File
