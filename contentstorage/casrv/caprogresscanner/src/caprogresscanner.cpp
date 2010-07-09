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

#include <usif/scr/scr.h>
#include <usif/scr/screntries.h>

#include "caprogresscanner.h"
#include "castorageproxy.h"
#include "cainnerquery.h"
#include "cainnerentry.h"
#include "caarraycleanup.inl"
#include "casrvplugin.h"

using namespace Usif;

// ---------------------------------------------------------------------------
//
// ---------------------------------------------------------------------------
//
EXPORT_C CCaProgresScanner* CCaProgresScanner::NewL(
        TPluginParams* aPluginParam )
    {
    CCaProgresScanner* self = new ( ELeave ) CCaProgresScanner(
            aPluginParam );
    CleanupStack::PushL( self );
    self->ConstructL();
    CleanupStack::Pop( self );
    return self;
    }

// ---------------------------------------------------------------------------
//
// ---------------------------------------------------------------------------
//
CCaProgresScanner::~CCaProgresScanner()
    {
    delete iNotifier;
    iResultArrayItems.ResetAndDestroy();
    }

// ---------------------------------------------------------------------------
//
// ---------------------------------------------------------------------------
//
CCaProgresScanner::CCaProgresScanner( TPluginParams* aPluginParam ) :
        iStorageProxy( *aPluginParam->storageProxy ),
        iSoftwareRegistry( *aPluginParam->softwareRegistry )
    {
    }

// ---------------------------------------------------------------------------
//
// ---------------------------------------------------------------------------
//
void CCaProgresScanner::ConstructL()
    {
    iNotifier = CSifOperationsNotifier::NewL( *this );
    }

// ---------------------------------------------------------------------------
//
// ---------------------------------------------------------------------------
//
void CCaProgresScanner::StartOperationHandler( TUint aKey,
        const CSifOperationStartData& aStartData )
    {
    TInt err( KErrNone );
    TComponentId componentId( 0 );
    iResultArrayItems.ResetAndDestroy();

    TRAP(err, componentId = iSoftwareRegistry.GetComponentIdL(
        aStartData.GlobalComponentId(), aStartData.SoftwareType() ));
    if ( !err )
        TRAP(err, MarkEntrysForUnistallL(aKey, componentId));

    }

// ---------------------------------------------------------------------------
//
// ---------------------------------------------------------------------------
//
void CCaProgresScanner::EndOperationHandler(
        const CSifOperationEndData& aEndData )
    {
    TInt err = KErrNone;
    TRAP( err, EndOperationL( aEndData.ErrorCode() ) );
    iResultArrayItems.ResetAndDestroy();
    }

// ---------------------------------------------------------------------------
//
// ---------------------------------------------------------------------------
//
void CCaProgresScanner::ProgressOperationHandler(
        const CSifOperationProgressData& aProgressData )
    {
    TInt err = KErrNone;
    TRAP( err, UpdateProgressL( aProgressData ) );
    }

// ---------------------------------------------------------------------------
//
// ---------------------------------------------------------------------------
//
void CCaProgresScanner::UpdateProgressL(
        const CSifOperationProgressData& aProgressData )
    {
    RBuf totalProgressBuf;
    totalProgressBuf.CleanupClosePushL();
    totalProgressBuf.CreateL( sizeof(TComponentId) + 1 );
    totalProgressBuf.AppendNum( aProgressData.CurrentProgess() );

    if ( iResultArrayItems.Count() && aProgressData.Phase() == EUninstalling )
        {
        for ( int i = 0; i < iResultArrayItems.Count(); i++ )
            {
            iResultArrayItems[i]->AddAttributeL( KCaAppUninstallProgress,
                    totalProgressBuf );
            iStorageProxy.AddL( iResultArrayItems[i], ETrue,
                    EItemUninstallProgressChanged );
            }
        }

    CleanupStack::PopAndDestroy( &totalProgressBuf );
    }

// ---------------------------------------------------------------------------
//
// ---------------------------------------------------------------------------
//
void CCaProgresScanner::EndOperationL( TInt aError )
    {
    if ( aError )
        {
        for ( int i = 0; i < iResultArrayItems.Count(); i++ )
            {
            iResultArrayItems[i]->SetFlags( iResultArrayItems[i]->GetFlags()
                    & ~EUninstall );
            iStorageProxy.AddL( iResultArrayItems[i] );
            }
        }
    iNotifier->CancelSubscribeL( iKey );
    }

// ---------------------------------------------------------------------------
//
// ---------------------------------------------------------------------------
//
void CCaProgresScanner::MarkEntrysForUnistallL( TUint aKey,
        TComponentId aComponentId )
    {
    RBuf componentIdBuf;
    componentIdBuf.CleanupClosePushL();
    componentIdBuf.CreateL( sizeof(TComponentId) + 1 );
    componentIdBuf.AppendNum( aComponentId );

    // find entry by componentID
    CCaInnerQuery *innerQuery = CCaInnerQuery::NewLC();
    innerQuery->SetRole( CCaInnerQuery::Item );
    innerQuery->AddAttributeL( KCaAttrComponentId, componentIdBuf );

    iStorageProxy.GetEntriesL( innerQuery, iResultArrayItems );

    if ( iResultArrayItems.Count() )
        {
        for ( int i = 0; i < iResultArrayItems.Count(); i++ )
            {
            iResultArrayItems[i]->SetFlags( iResultArrayItems[i]->GetFlags()
                    | EUninstall );
            iStorageProxy.AddL( iResultArrayItems[i] );
            }
        // subscribe for progress notifier
        iKey = aKey;
        iNotifier->SubscribeL( aKey, ETrue );
        }
    CleanupStack::PopAndDestroy( innerQuery );
    CleanupStack::PopAndDestroy( &componentIdBuf );
    }
