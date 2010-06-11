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

#include "s32mem.h"

#include <usif/usifcommon.h> 
#include <usif/scr/scr.h>
#include <usif/scr/screntries.h>

#include "caprogressnotifier.h"
#include "castorageproxy.h"
#include "cainnerquery.h"
#include "cainnerentry.h"
#include "caarraycleanup.inl"

using namespace Usif;

//fake constants
const TInt KMaxProgressValue = 100;
const TInt KDelayTimeBetweenNotifications(500000);


#ifdef COVERAGE_MEASUREMENT
#pragma CTC SKIP
#endif //COVERAGE_MEASUREMENT (fake notifier timer)

EXPORT_C CCaFakeProgressNotifier* CCaFakeProgressNotifier::NewL(
        MCaFakeProgressListener& aListener )
    {
    CCaFakeProgressNotifier* self = new ( ELeave ) CCaFakeProgressNotifier( aListener );
    CleanupStack::PushL( self );
    self->ConstructL( );
    CleanupStack::Pop( self );
    return self;
    }

CCaFakeProgressNotifier::~CCaFakeProgressNotifier()
    {
    Cancel();
    iTimer.Close();
    }

CCaFakeProgressNotifier::CCaFakeProgressNotifier( MCaFakeProgressListener& aListener ) :
    CActive( EPriorityNormal ), iListener( aListener ), iCount( 0 ) 
    {
    CActiveScheduler::Add( this );
    }

void CCaFakeProgressNotifier::ConstructL(  )
    {
    User::LeaveIfError( iTimer.CreateLocal() );
    }

void CCaFakeProgressNotifier::StartNotifying()
    {
    iCount = 0;
    Cancel();
    iTimer.After( iStatus, TTimeIntervalMicroSeconds32( KDelayTimeBetweenNotifications ) );
    SetActive();
    }

void CCaFakeProgressNotifier::DoCancel()
    {    
    iTimer.Cancel();
    }

void CCaFakeProgressNotifier::RunL()
    {
    User::LeaveIfError( iStatus.Int() );
    
    if ( iCount <= KMaxProgressValue )
        {
        if( iCount > 20 )
            {            
            iListener.HandleFakeProgressNotifyL(iCount); 
            }     
        iTimer.After( iStatus, TTimeIntervalMicroSeconds32( KDelayTimeBetweenNotifications ) );
        SetActive();
        iCount += 20;
        }
    }

TInt CCaFakeProgressNotifier::RunError( TInt /*aError*/)
    {
    // No need to do anything
    return KErrNone;
    }

#ifdef COVERAGE_MEASUREMENT
#pragma CTC ENDSKIP
#endif //COVERAGE_MEASUREMENT  (fake notifier)



// ---------------------------------------------------------------------------
//
// ---------------------------------------------------------------------------
//
EXPORT_C CCaProgressNotifier* CCaProgressNotifier::NewL(
        CCaStorageProxy& aCaStorageProxy )
    {
    CCaProgressNotifier* self = new ( ELeave ) CCaProgressNotifier(  );
    CleanupStack::PushL( self );
    self->ConstructL( aCaStorageProxy );
    CleanupStack::Pop( self );
    return self;
    }

// ---------------------------------------------------------------------------
//
// ---------------------------------------------------------------------------
//
CCaProgressNotifier::~CCaProgressNotifier()
    {
    // TODO: Commented out since USIF notifications do not 
    // work on MCL wk20
    /*
    iNotifier->CancelSubscribeL();
    delete iNotifier;
    */
    
    // needed for fake:
    delete iUsifUninstallNotifier;
    delete iJavaInstallNotifier;
    delete iFakeProgressNotifier;
    }

// ---------------------------------------------------------------------------
//
// ---------------------------------------------------------------------------
//
CCaProgressNotifier::CCaProgressNotifier()
    {
    }

// ---------------------------------------------------------------------------
//
// ---------------------------------------------------------------------------
//
void CCaProgressNotifier::ConstructL( CCaStorageProxy& aCaStorageProxy )
    {
    // TODO: Commented out since USIF notifications do not 
    // work on MCL wk20
    /*
    iNotifier = CSifOperationsNotifier::NewL(*this);
    */
    
    iStorageProxy = &aCaStorageProxy;
    
    iUsifUninstallNotifier = CCaInstallNotifier::NewL( *this,
            CCaInstallNotifier::EUsifUninstallNotification );

    iJavaInstallNotifier = CCaInstallNotifier::NewL( *this,
                CCaInstallNotifier::EJavaInstallNotification );

    iFakeProgressNotifier = CCaFakeProgressNotifier::NewL(*this);    
    }

// ---------------------------------------------------------------------------
//
// ---------------------------------------------------------------------------
//
void CCaProgressNotifier::StartOperationHandler(TUint /*aKey*/, const CSifOperationStartData& /*aStartData*/)
    {
    // TODO: Commented out since USIF notifications do not 
    // work on MCL wk20
    /*
    iNotifier->SubscribeL( aKey, ETrue );
    */
    }

// ---------------------------------------------------------------------------
//
// ---------------------------------------------------------------------------
//
void CCaProgressNotifier::EndOperationHandler(const CSifOperationEndData& /*aEndData*/)
    {
    }

// ---------------------------------------------------------------------------
//
// ---------------------------------------------------------------------------
//
void CCaProgressNotifier::ProgressOperationHandler(const CSifOperationProgressData& /*aProgressData*/)
    {
    
    // TODO: Commented out since USIF notifications do not 
    // work on MCL wk20
    
    /* 
    // extract component ID
    TBuf8<20> globalIdBuf;
    globalIdBuf.Copy( aProgressData.GlobalComponentId() );
    
    RDesReadStream readStream( globalIdBuf );
    CleanupClosePushL( readStream );
    CGlobalComponentId *globalId = CGlobalComponentId::NewL( readStream );
    CleanupStack::PushL(globalId);
     
    RSoftwareComponentRegistry iScrSession;
    TComponentId componentId = iScrSession.GetComponentIdL( globalId->GlobalIdName(), 
            globalId->SoftwareTypeName() );

    CleanupStack::PopAndDestroy( globalId );
    CleanupStack::PopAndDestroy( &readStream );

    RBuf componentIdBuf;
    componentIdBuf.CleanupClosePushL();
    componentIdBuf.CreateL( sizeof(TComponentId) + 1 );
    componentIdBuf.AppendNum( componentId );

    // find entry by componentID
    CCaInnerQuery *innerQuery = CCaInnerQuery::NewLC();
    innerQuery->SetRole( CCaInnerQuery::Item );
    innerQuery->AddAttributeL( KCaAttrComponentId,
            componentIdBuf );

    RPointerArray<CCaInnerEntry> resultArrayItems;
    CleanupResetAndDestroyPushL( resultArrayItems );
    iStorageProxy->GetEntriesL( innerQuery, resultArrayItems );
   
    RBuf totalProgressBuf;
    totalProgressBuf.CleanupClosePushL();
    totalProgressBuf.CreateL( sizeof(TComponentId) + 1 );
    totalProgressBuf.AppendNum( aProgressData.CurrentProgess() );

    if (resultArrayItems.Count() && aProgressData.Phase() == EUninstalling)
        {
        CCaInnerEntry* appEntry = resultArrayItems[0];
        appEntry->AddAttributeL(KCaAppUninstallProgress, totalProgressBuf);

        iStorageProxy->AddL(appEntry, ETrue, EItemUninstallProgressChanged );
        }
    CleanupStack::PopAndDestroy( &totalProgressBuf );
    CleanupStack::PopAndDestroy( &resultArrayItems );
    CleanupStack::PopAndDestroy( innerQuery );
    CleanupStack::PopAndDestroy( &componentIdBuf );
    */
    }

void CCaProgressNotifier::HandleInstallNotifyL( TInt /*aUid*/)
    {
    // start sending fake notifications
    iFakeProgressNotifier->StartNotifying();
    }

void CCaProgressNotifier::HandleFakeProgressNotifyL(TInt aCurrentProgress)
    {
    //update storage with new info
    
    // find entry by componentID
    CCaInnerQuery *innerQuery = CCaInnerQuery::NewLC();
    innerQuery->SetRole( CCaInnerQuery::Item );
    innerQuery->SetFlagsOn(EUninstall);
    
    RPointerArray<CCaInnerEntry> resultArrayItems;
    CleanupResetAndDestroyPushL( resultArrayItems );
    iStorageProxy->GetEntriesL( innerQuery, resultArrayItems );
    
    
    RBuf totalProgressBuf;
    totalProgressBuf.CleanupClosePushL();
    totalProgressBuf.CreateL( sizeof( TInt ) + 1 );
    totalProgressBuf.AppendNum( aCurrentProgress );
    
    for ( TInt i = 0; i<resultArrayItems.Count(); i++ )
       {
       CCaInnerEntry* appEntry = resultArrayItems[i];
       appEntry->AddAttributeL(KCaAppUninstallProgress, totalProgressBuf);
       iStorageProxy->AddL(appEntry, ETrue, EItemUninstallProgressChanged );
       }
    
    CleanupStack::PopAndDestroy( &totalProgressBuf );
    CleanupStack::PopAndDestroy( &resultArrayItems );
    CleanupStack::PopAndDestroy( innerQuery );
    }
