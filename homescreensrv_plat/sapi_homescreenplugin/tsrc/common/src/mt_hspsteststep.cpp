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
* Description:  Implementation of MT_CHSPSTeststep class
*
*/


#include <e32base.h>
#include <s32mem.h>
#include <eunitmacros.h>
#include <eunitdecorators.h>
#include "mt_hspsteststep.h"

#ifdef HSPS_LOG_ACTIVE
#include <hspslogbusfile.h>
#include <hspslogbus.h>
#endif

// ======== LOCAL CONSTANTS ====================================================

// Test step timeout
const TInt KTestStepTimeout = 5000000;

// Max input
const TInt KTestStepInputSizeMax = 1000;

// Max output
const TInt KTestStepOutputSizeMax = 10000;

// ======== LOCAL FUNCTIONS ====================================================

// ---------------------------------------------------------
// TimeoutCallback
// ---------------------------------------------------------
//
TInt TimeoutCallback( TAny* aPtr )
    {
    TRequestStatus* status = &( ( ( MT_CHSPSTeststep* )aPtr )->iStatus );

    if ( ( ( MT_CHSPSTeststep* )aPtr )->iOutParams )
        {
        delete ( ( MT_CHSPSTeststep* )aPtr )->iOutParams;
        ( ( MT_CHSPSTeststep* )aPtr )->iOutParams = NULL; 
        }
    if ( ( ( MT_CHSPSTeststep* )aPtr )->iTimeout )
        {
        delete ( ( MT_CHSPSTeststep* )aPtr )->iTimeout;
        ( ( MT_CHSPSTeststep* )aPtr )->iTimeout = NULL; 
        }
    
    User::RequestComplete( status, KErrNone );
 
    return KErrNone;
    }


// ======== MEMBER FUNCTIONS ===================================================

// -----------------------------------------------------------------------------
// Two-phased constructor.
// -----------------------------------------------------------------------------
//
MT_CHSPSTeststep* MT_CHSPSTeststep::NewL(
    MLiwInterface& aInterface,
    TUint8* aMethod,
    TUint8* aInParams,
    TUint8* aExpOutParams,
    TBool aAsync,
    TBool aMeasureExecTime )
    {
    MT_CHSPSTeststep* self = new( ELeave ) MT_CHSPSTeststep(
        aInterface,
        aMethod,
        aInParams,
        aExpOutParams,
        aAsync,
        aMeasureExecTime );

    return self;
    }

// -----------------------------------------------------------------------------
// Constructor
// -----------------------------------------------------------------------------
//
MT_CHSPSTeststep::MT_CHSPSTeststep(
    MLiwInterface& aInterface,
    TUint8* aMethod,
    TUint8* aInParams,
    TUint8* aExpOutParams,
    TBool aAsync,
    TBool aMeasureExecTime )
    :CActive( CActive::EPriorityStandard )
    ,iOutParams( NULL )
    ,iTimeout( NULL )
    ,iInterface( aInterface )
    ,iMethod( aMethod )
    ,iInParams( aInParams )
    ,iExpOutParams( aExpOutParams )
    ,iAsync( aAsync )
    ,iMeasureExecTime( aMeasureExecTime )
    {
    }

// -----------------------------------------------------------------------------
// Destructor.
// -----------------------------------------------------------------------------
//    
MT_CHSPSTeststep::~MT_CHSPSTeststep()
    {
    if ( iOutParams )
    	{
        delete iOutParams;
        }
    if ( iTimeout )
        {
        delete iTimeout;
        }
    }

// -----------------------------------------------------------------------------
// From CActive
// -----------------------------------------------------------------------------
//    
void MT_CHSPSTeststep::RunL()
    {
    // Stop test thread active scheduler
    CActiveScheduler::Stop();
    }

// -----------------------------------------------------------------------------
// From CActive
// -----------------------------------------------------------------------------
//    
TInt MT_CHSPSTeststep::RunError(
    TInt /*aError*/ )
    {
    if ( iOutParams )
        {
        delete iOutParams;
        iOutParams = NULL;
        }
    if ( iTimeout )
        {
        delete iTimeout;
        iTimeout = NULL;
        }
    
    return KErrNone; 
    }

// -----------------------------------------------------------------------------
// From CActive
// -----------------------------------------------------------------------------
//    
void MT_CHSPSTeststep::DoCancel()
    {
    if ( iOutParams )
        {
        delete iOutParams;
        iOutParams = NULL;
        }
    if ( iTimeout )
        {
        delete iTimeout;
        iTimeout = NULL;
        }
    }

// -----------------------------------------------------------------------------
// MLiwNotifyCallback::HandleNotifyL
// -----------------------------------------------------------------------------
//    
TInt MT_CHSPSTeststep::HandleNotifyL(
    TInt aCmdId,
    TInt aEventId,
    CLiwGenericParamList& aEventParamList,
    const CLiwGenericParamList& /*aInParamList*/ )
    {
    aCmdId = aCmdId;
    aEventId = aEventId;
    
    // Copy event parameters as a descriptor stream
    if ( iOutParams )
        {
        delete iOutParams;
        iOutParams = NULL;
        }
    iOutParams = CreateOutParamsL( aEventParamList );
    
    TRequestStatus* status = &iStatus;
    User::RequestComplete( status, KErrNone );
    
    return KErrNone;
    }

// -----------------------------------------------------------------------------
// Calls service method
// -----------------------------------------------------------------------------
//    
void MT_CHSPSTeststep::CallServiceL()
    {

    CLiwGenericParamList* inParamList;
    CLiwGenericParamList* outParamList;
    TInt err;
    
    // Create input parameter list
    if ( iInParams )
        {
        // Create input parameter list from binary stream    
        inParamList = CreateInParamListL(
            iInParams );
        }
    else
        {
        // Create empty input parameter list
        inParamList = CLiwGenericParamList::NewL();
        }
    
    // Create empty output parameter list
    outParamList = CLiwGenericParamList::NewL();
    
    // Execute test step
    if ( !iAsync )
        {
        TPtrC8 method( iMethod );
        
        if ( iMeasureExecTime )
            {
            StartTiming( method );
            }
        
        TRAP( err, iInterface.ExecuteCmdL( 
            method, 
            *inParamList, 
            *outParamList ) );
        
        if ( iMeasureExecTime )
            {
            StopTiming();
            }
        
        if ( err == KErrNone )
            {
            // Store outparams as descriptor stream
            if ( iOutParams )
                {
                delete iOutParams;
                iOutParams = NULL;
                }
            iOutParams = CreateOutParamsL( *outParamList );
            }
        }
    else
        {
        TPtrC8 method( iMethod );
        
        TRAP( err, iInterface.ExecuteCmdL( 
            method, 
            *inParamList, 
            *outParamList,
            KLiwOptASyncronous,
            this ) );

        if ( err == KErrNone )
            {
            // Set test step to wait asynchronous response
            CActiveScheduler::Add( this );
            iStatus = KRequestPending;
            SetActive();
            
            // Start test step timeout
            iTimeout = CPeriodic::NewL( CActive::EPriorityStandard );
            iTimeout->Start( 
                TTimeIntervalMicroSeconds32( KTestStepTimeout ),
                TTimeIntervalMicroSeconds32( KTestStepTimeout ),
                TCallBack( TimeoutCallback, this ) );
            }
        }
    
    inParamList->Reset();
    delete inParamList;
    outParamList->Reset();
    delete outParamList;
    
    }

#ifdef HSPS_LOG_ACTIVE
// -----------------------------------------------------------------------------
// Debugging
// -----------------------------------------------------------------------------
//    
void MT_CHSPSTeststep::DebugL(
        const TUint8* aOutPtr )
    {    
    ChspsLogBus* logBus = ChspsLogBusFile::NewL( _L("LIW_test_comparison.log") );
    CleanupStack::PushL( logBus );
    
    logBus->LogText( _L( "index, expected(int, char), actual(int, char)" ) );
    
    TBool exitNow = EFalse;
    for ( TInt i = 0; 
          i < iOutParams->Length() && !exitNow; 
          i++ )
        {
        logBus->LogText( _L("[%5d], %3d, '%c', %3d, '%c'"),
                         i, // index
                         ( TUint16 )iExpOutParams[i],
                         ( TUint16 )iExpOutParams[i],
                         ( TUint16 )aOutPtr[i],
                         ( TUint16 )aOutPtr[i] );
                         
        if ( iExpOutParams[i] != aOutPtr[i] )
            {                    
            exitNow = ETrue;
            }                
        }            
    
    CleanupStack::PopAndDestroy( logBus );     
    }
#endif    

// -----------------------------------------------------------------------------
// Checks method call output parameters
// -----------------------------------------------------------------------------
//    
TInt MT_CHSPSTeststep::CheckOutParams()
    {
    TInt err( KErrArgument );

    if ( iExpOutParams && iOutParams )
        {
        err = KErrNone;
        
        const TUint8* outPtr = ( iOutParams->Des() ).Ptr();        
        for ( TInt i = 0; 
              i < iOutParams->Length() && err == KErrNone; 
              i++ )
            {
            if ( *( iExpOutParams + i ) != *( outPtr + i ) )
                {
                EUNIT_PRINT( _L("TEST STEP FAILED: Output mismatch: output[%d] = %d, expected output[%d] = %d "), i, ( *( outPtr + i ) ), i, ( *( iExpOutParams + i ) ) );
                err = KErrArgument;
                }
            }
        
        // Dump comparison file.
#ifdef HSPS_LOG_ACTIVE
        if( err != KErrNone )
            {
            TRAP_IGNORE( DebugL( outPtr ) );
            }
#endif
        }
    
    return err;
    }

// -----------------------------------------------------------------------------
// Creates input parameter list from a descriptor stream
// -----------------------------------------------------------------------------
//    
CLiwGenericParamList* MT_CHSPSTeststep::CreateInParamListL(
    TUint8* aInParams )
    {
    HBufC8* inParamBuf = HBufC8::NewL( KTestStepInputSizeMax );
    CleanupStack::PushL( inParamBuf );
    
    TPtr8 inParamBufPtr = inParamBuf->Des();
    inParamBufPtr.Set( 
        ( ( TUint8* )aInParams ), 
        KTestStepInputSizeMax, 
        KTestStepInputSizeMax );

    RDesReadStream inParamStream;
    inParamStream.Open( inParamBufPtr );
    CLiwGenericParamList* inParamList = 
        CLiwGenericParamList::NewL( inParamStream );
    inParamStream.Release();

    CleanupStack::PopAndDestroy( inParamBuf );
    
    return inParamList;
    }

// -----------------------------------------------------------------------------
// Creates output descriptor stream from output parameter list
// -----------------------------------------------------------------------------
//    
HBufC8* MT_CHSPSTeststep::CreateOutParamsL(
    CLiwGenericParamList& aOutParamList )
    {
    HBufC8* outParams = HBufC8::NewL( KTestStepOutputSizeMax );
    CleanupStack::PushL( outParams );

    TPtr8 outParamBufPtr = outParams->Des();
    RDesWriteStream outParamStream;
    outParamStream.Open( outParamBufPtr );    
    aOutParamList.ExternalizeL( outParamStream );
    outParamStream.CommitL();
    outParamStream.Close();

    CleanupStack::Pop( outParams );
    
    return outParams;
    }

// -----------------------------------------------------------------------------
// Starts timing of SAPI method execution
// -----------------------------------------------------------------------------
//    
void MT_CHSPSTeststep::StartTiming( 
    TDesC8& aMethod )
    {
    iStartTime.HomeTime();
    EUNIT_PRINT( aMethod );
    }

// -----------------------------------------------------------------------------
// Calculates SAPI method execution time
// -----------------------------------------------------------------------------
//    
void MT_CHSPSTeststep::StopTiming()
    {
    TTime readyTime;
    readyTime.HomeTime();
    TTimeIntervalMicroSeconds delay = readyTime.MicroSecondsFrom( iStartTime );
    TTime transferTime( delay.Int64() );
    TBuf<64> timeString;
    transferTime.FormatL( timeString,_L( "Execution time: %S%C microseconds" ) );
    
    EUNIT_PRINT( timeString );
    
    }

//  END OF FILE
