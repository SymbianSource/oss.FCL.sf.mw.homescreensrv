/*
 * Copyright (c) 2009 Nokia Corporation and/or its subsidiary(-ies).
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
 * Description: causifscanner.cpp
 *
 */

#include <e32property.h>
#include <f32file.h>
#include <badesca.h>
#include <usif/sif/sif.h>
#include <usif/scr/scr.h>
#include <usif/sif/sifcommon.h>

#include "causifscanner.h"
#include "cainnerentry.h"
#include "cainnerquery.h"
#include "cadef.h"
#include "castorageproxy.h"
#include "caarraycleanup.inl"

using namespace Usif;

_LIT( KUsifPluginCollection, "Usif collection");
_LIT( KCaAttrComponentId, "component_id" );

// ---------------------------------------------------------------------------
//
// ---------------------------------------------------------------------------
//
CCaUsifScanner* CCaUsifScanner::NewL( TPluginParams* aPluginParams )
    {
    CCaUsifScanner* self = new ( ELeave )
            CCaUsifScanner( *aPluginParams->storageProxy );
    CleanupStack::PushL( self );
    self->ConstructL();
    CleanupStack::Pop( self );
    return self;
    }

// ---------------------------------------------------------------------------
//
// ---------------------------------------------------------------------------
//
CCaUsifScanner::CCaUsifScanner( CCaStorageProxy& aStorageProxy ) :
    iStorageProxy( aStorageProxy )
    {

    }

// ---------------------------------------------------------------------------
//
// ---------------------------------------------------------------------------
//
void CCaUsifScanner::ConstructL()
    {
    AddCollectionUsifL();
    AddNativeAppsL();
    }

// ---------------------------------------------------------------------------
//
// ---------------------------------------------------------------------------
//
CCaUsifScanner::~CCaUsifScanner()
    {
    }

// ---------------------------------------------------------------------------
//
// ---------------------------------------------------------------------------
//
void CCaUsifScanner::InstallL( const TDesC& aFileName )
    {
    RSoftwareInstall sif;
    CleanupClosePushL( sif );
    User::LeaveIfError( sif.Connect());

    // Install the component
    TRequestStatus status;
    sif.Install( aFileName, status, EFalse );
    User::WaitForRequest( status );
    User::LeaveIfError(status.Int());

    // Disconnect from the SIF server
    CleanupStack::PopAndDestroy( &sif );
    }

// ---------------------------------------------------------------------------
//
// ---------------------------------------------------------------------------
//
void CCaUsifScanner::AddCollectionUsifL()
    {
    if( GetCollectionUsifIdL() == 0 )
        {
        CCaInnerEntry *entry = CCaInnerEntry::NewLC();
        entry->SetRole( EGroupEntryRole );
        entry->SetEntryTypeNameL( KCaTypeCollection );
        entry->SetFlags( ERemovable | EVisible );
        entry->SetTextL( KUsifPluginCollection );
        iStorageProxy.AddL( entry );
        iCollectionUsifId = entry->GetId();
        CleanupStack::PopAndDestroy( entry );

        TCaOperationParams params =
            {
            TCaOperationParams::EPrepend, 2, 0
            };
        RArray<TInt> ids;
        CleanupClosePushL( ids );
        ids.AppendL( iCollectionUsifId );
        iStorageProxy.OrganizeL( ids, params );
        CleanupStack::PopAndDestroy( &ids );
        }
    }

// ---------------------------------------------------------------------------
//
// ---------------------------------------------------------------------------
//
TInt CCaUsifScanner::GetCollectionUsifIdL()
    {
    if( iCollectionUsifId == 0 )
        {
        RPointerArray<CCaInnerEntry> resultArray;
        CleanupResetAndDestroyPushL( resultArray );
        CCaInnerQuery* allAppQuery = CCaInnerQuery::NewLC();
        CDesC16ArrayFlat* appType = new ( ELeave ) CDesC16ArrayFlat( 1 );
        CleanupStack::PushL( appType );
        appType->AppendL( KCaTypeCollection );
        allAppQuery->SetEntryTypeNames( appType );
        allAppQuery->SetRole(CCaInnerQuery::Group);
        CleanupStack::Pop( appType );
        iStorageProxy.GetEntriesL( allAppQuery, resultArray );
        CleanupStack::PopAndDestroy( allAppQuery );
        for(TInt i(0); i< resultArray.Count(); i++ )
            {
            if(!resultArray[i]->GetText().Compare(KUsifPluginCollection))
                iCollectionUsifId = resultArray[i]->GetId();
            }
        CleanupStack::PopAndDestroy( &resultArray );
        }
    return iCollectionUsifId;
    }

// ---------------------------------------------------------------------------
//
// ---------------------------------------------------------------------------
//
void CCaUsifScanner::CreateCaEntryFromEntryL(
        const CComponentEntry* aEntry, CCaInnerEntry* aCaEntry )
    {
    aCaEntry->SetRole( EItemEntryRole );
    aCaEntry->SetEntryTypeNameL(KCaTypeApp);
    if( aEntry->Name().Compare(KNullDesC))
        {
        aCaEntry->SetTextL(aEntry->Name());
        }
    if( !aEntry->IsHidden() )
        {
        aCaEntry->SetFlags( EVisible );
        }
    if( aEntry->IsRemovable() )
        {
        aCaEntry->SetFlags( aCaEntry->GetFlags() | ERemovable );
        }
    if( aEntry->GlobalId().Compare( KNullDesC ) )
        {
        TLex lex( aEntry->GlobalId() );
        TUint uint( 0 );
        User::LeaveIfError( lex.Val( uint, EHex ) );
        aCaEntry->SetUid( uint );
        }
    RBuf compIdDesc;
    CleanupClosePushL(compIdDesc);
    compIdDesc.CreateL(KCaMaxAttrValueLen);
    compIdDesc.Num(aEntry->ComponentId());
    aCaEntry->AddAttributeL( KCaAttrComponentId, compIdDesc );
    CleanupStack::PopAndDestroy( &compIdDesc );

    }

// ---------------------------------------------------------------------------
//
// ---------------------------------------------------------------------------
//
void CCaUsifScanner::AddNativeAppsL()
    {
    TLanguage locale = TLanguage(-1);

    CComponentFilter *filter = CComponentFilter::NewL();
    CleanupStack::PushL(filter);
    filter->SetSoftwareTypeL( KSoftwareTypeNative );
    filter->SetRemovable(ETrue);

    // Connect to the SCR server
    RSoftwareComponentRegistry *scr = new RSoftwareComponentRegistry();
    CleanupClosePushL(*scr);
    User::LeaveIfError( scr->Connect() );

    // Create an SCR view
    RSoftwareComponentRegistryView *scrView =
            new RSoftwareComponentRegistryView();
    CleanupClosePushL(*scrView);
    scrView->OpenViewL( *scr, filter );

    // Iterate over the matching components
    CComponentEntry* entry = CComponentEntry::NewLC();

    RPointerArray<CCaInnerEntry> entries;
    CleanupResetAndDestroyPushL( entries );
    GetCaAppEntriesL(entries);

    RArray<TInt> entryIds;
    CleanupClosePushL(entryIds);
    while( scrView->NextComponentL( *entry, locale ) )
        {
        CCaInnerEntry *caEntry = CCaInnerEntry::NewLC();
        CreateCaEntryFromEntryL( entry, caEntry );

        if( !AppExist(entries, caEntry))
            {
            iStorageProxy.AddL( caEntry );
            entryIds.Append( caEntry->GetId() );
            }
        CleanupStack::PopAndDestroy( caEntry );
        }
    TCaOperationParams params =
        {
        TCaOperationParams::EAppend, iCollectionUsifId, 0
        };
    iStorageProxy.OrganizeL( entryIds, params );

    CleanupStack::PopAndDestroy( &entryIds );
    CleanupStack::PopAndDestroy( &entries );
    CleanupStack::PopAndDestroy( entry );
    CleanupStack::PopAndDestroy(scrView);
    CleanupStack::PopAndDestroy(scr);
    CleanupStack::PopAndDestroy(filter);
    }

// ---------------------------------------------------------------------------
//
// ---------------------------------------------------------------------------
//
void CCaUsifScanner::GetCaAppEntriesL(
        RPointerArray<CCaInnerEntry>& aArray )
    {
    CCaInnerQuery* allAppQuery = CCaInnerQuery::NewLC();
    CDesC16ArrayFlat* appType = new ( ELeave ) CDesC16ArrayFlat( 1 );
    CleanupStack::PushL( appType );
    appType->AppendL( KCaTypeApp );
    allAppQuery->SetEntryTypeNames( appType );
    CleanupStack::Pop( appType );
    iStorageProxy.GetEntriesL( allAppQuery, aArray );
    CleanupStack::PopAndDestroy( allAppQuery );
    }

// ---------------------------------------------------------------------------
//
// ---------------------------------------------------------------------------
//
TInt CCaUsifScanner::AppExist( RPointerArray<CCaInnerEntry>& aArray,
            CCaInnerEntry* aEntry )
    {
    for (TInt i(0); i < aArray.Count(); i++)
        {
        if( aArray[i]->GetUid() == aEntry->GetUid())
            return KErrAlreadyExists;
        }
    return KErrNone;
    }
