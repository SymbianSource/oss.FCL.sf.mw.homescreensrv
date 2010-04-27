/*
* Copyright (c) 2006 Nokia Corporation and/or its subsidiary(-ies).
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


#include <s32mem.h>
#include <liwcommon.h>
#include "cpclientactivenotifier.h"
#include "ccontentmap.h"
#include "cpdebug.h"
#include "cpglobals.h"
#include "cpserverdef.h"
#include "cdatasourceinterface.h"

using namespace LIW;

// ======== MEMBER FUNCTIONS ========

// ----------------------------------------------------------------------------
//
// ----------------------------------------------------------------------------
//
CCPActiveNotifier* CCPActiveNotifier::NewL( RCPServerClient& aServerClient )
    {
    CCPActiveNotifier* self = new( ELeave ) 
            CCPActiveNotifier ( aServerClient );
    CleanupStack::PushL( self );
    self->ConstructL( );
    CleanupStack::Pop( self );
    return self;
    }

// ---------------------------------------------------------------------------
// destructor
// ---------------------------------------------------------------------------
CCPActiveNotifier::~CCPActiveNotifier()
    {
    CP_DEBUG( _L8("CCPActiveNotifier::CCPActiveNotifier()" ) );
    Cancel( );
    iObservers.Close( );
    delete iObserverBuf;
    delete iSizeDes;
    }

// ----------------------------------------------------------------------------
//
// ----------------------------------------------------------------------------
//
void CCPActiveNotifier::RegisterL( MLiwNotifyCallback* aObserver, 
		TInt32 aTransactionId, CCPLiwMap* aMap )
    {
    CP_DEBUG( _L8("CCPActiveNotifier::RegisterL()" ) );
    TIpcArgs args;
    
   	if(iObservers.Find( aTransactionId ))
   		{
   		User::Leave( KErrInUse );
   		}
    
   	iObservers.InsertL( aTransactionId, aObserver );
	delete iObserverBuf;
	iObserverBuf = NULL;
 	iObserverBuf = aMap->PackForServerLC( );
   	CleanupStack::Pop( iObserverBuf );
   	args.Set( KDescriptorPosition, &*iObserverBuf );
	args.Set( KTransactionPosition, aTransactionId );
    
    if ( !IsActive( ) )
        {
        iSizeDes = new (ELeave) TPckgBuf<TInt>( );
        args.Set( KReturnPosition, iSizeDes );
        iServerClient.RegisterObserverL( iStatus, args );
        SetActive( );
        }
    else
        {
        iServerClient.AddObserver( args );
        }
    }


// ----------------------------------------------------------------------------
//
// ----------------------------------------------------------------------------
//
void CCPActiveNotifier::UnregisterL()
    {
    CP_DEBUG( _L8("CCPActiveNotifier::UnregisterL()" ) );
    if ( !IsActive( ) )
        {
        User::Leave( KErrNotFound );
        }
    else
        {
        Cancel( );
        delete iSizeDes;
        iSizeDes = NULL;
        }
    }

// ----------------------------------------------------------------------------
//
// ----------------------------------------------------------------------------
//
TBool CCPActiveNotifier::UnregisterL( TInt32 aTransactionId )
    {
    TBool last(EFalse);
    CP_DEBUG( _L8("CCPActiveNotifier::UnregisterL()" ) );

    MLiwNotifyCallback** observer = iObservers.Find(aTransactionId);
    if (observer)
        {
        CLiwGenericParamList* changeInfoList = 
                 CLiwGenericParamList::NewL( );
        CleanupStack::PushL( changeInfoList );
        CLiwGenericParamList* inParamList = CLiwGenericParamList::NewL( );
        CleanupStack::PushL( inParamList );
        
        changeInfoList->AppendL(TLiwGenericParam(EGenericParamError,
                TLiwVariant(CDataSourceInterface::ErrCodeConversion(KErrNone))));
        changeInfoList->AppendL(TLiwGenericParam(KErrorCode, TLiwVariant(
                CDataSourceInterface::ErrCodeConversion(KErrNone))));
        
		(*observer)->HandleNotifyL( aTransactionId, KLiwEventCanceled, *changeInfoList,
            *inParamList );     
        
        CleanupStack::PopAndDestroy( inParamList );
        CleanupStack::PopAndDestroy( changeInfoList );
        }
    else
        {
        User::Leave(KErrNotFound);
        }
    
    TIpcArgs args;
	args.Set( KTransactionPosition, aTransactionId );
    iServerClient.RemoveObserver( args );

    iObservers.Remove( aTransactionId );
        
	if( !iObservers.Count() )
		{
		UnregisterL();
		last = ETrue;
		}
	return last;
    }

// ----------------------------------------------------------------------------
//
// ----------------------------------------------------------------------------
//
void CCPActiveNotifier::UnregisterAllL(  )
    {
    CP_DEBUG( _L8("CCPActiveNotifier::UnregisterAllL()" ) );
    if (!iObservers.Count())
    	{
    	User::Leave( KErrNotFound );
    	}
    THashMapIter<TInt32, MLiwNotifyCallback*> iter( iObservers );
    const TInt32* transaction = iter.NextKey();
    while( transaction )
        {
        UnregisterL( *transaction );
        transaction = iter.NextKey( );
        }    
    }

// ----------------------------------------------------------------------------
//
// ----------------------------------------------------------------------------
//
void CCPActiveNotifier::RunL()
    {
    CP_DEBUG( _L8("CCPActiveNotifier::RunL()" ) );
    CLiwGenericParamList* eventParamList = CLiwGenericParamList::NewL();
    CleanupStack::PushL(eventParamList);

    TInt error = iStatus.Int();
    if (KErrNone == error)
        {
        RBuf8 outbuf;
        outbuf.CleanupClosePushL();
        outbuf.CreateL((*iSizeDes)());
        error = iServerClient.GetChangeInfoData(outbuf);
        RDesReadStream stream(outbuf);
        CleanupClosePushL(stream);
        eventParamList->InternalizeL(stream);
        CleanupStack::PopAndDestroy(&stream);
        delete iSizeDes;
        iSizeDes = NULL;
        CleanupStack::PopAndDestroy(&outbuf);
        }
    if (KErrNoMemory!=iStatus.Int())
        {
        RegisterAgainL();
        }
    NotifyObserversL(error, eventParamList);
    CleanupStack::PopAndDestroy(eventParamList);
    }

// ----------------------------------------------------------------------------
//
// ----------------------------------------------------------------------------
//
void CCPActiveNotifier::NotifyObserversL( TInt aErrorCode, 
		CLiwGenericParamList* aEventParamList  )
	{
    TInt event( KLiwEventInProgress );
    if ( aErrorCode == KErrCancel )
        {
        event = KLiwEventCanceled;
        }
    else if ( aErrorCode != KErrNone )
        {
        event = KLiwEventStopped;
        }	
 
    for ( TInt i = 0; i<aEventParamList->Count(); i++ )
    	{
    	TLiwGenericParam param;
    	param.PushL();
    	TLiwVariant mapsV;
    	mapsV.PushL();
    	aEventParamList->AtL( i, param );
    	mapsV = param.Value();
    	CLiwDefaultList* listOfMatchingMaps = CLiwDefaultList::NewLC( );
    	if (mapsV.Get( *listOfMatchingMaps ))
    		{
    		TLiwVariant transV;
    		transV.PushL();
    		TInt transIndex (listOfMatchingMaps->Count()-1);
    		listOfMatchingMaps->AtL(transIndex ,transV);
    		TInt32 transactionId ( KErrNotFound );
    		transV.Get( transactionId );
    		MLiwNotifyCallback** observer = iObservers.Find( transactionId );
    		
    		if ( observer )
    			{
    			listOfMatchingMaps->Remove( transIndex );
    			CLiwGenericParamList* changeInfoList = 
											CLiwGenericParamList::NewL( );
    			CleanupStack::PushL( changeInfoList );
    		    CLiwGenericParamList* inParamList = CLiwGenericParamList::NewL( );
    		    CleanupStack::PushL( inParamList );
    			
    			changeInfoList->AppendL( TLiwGenericParam( KChangeInfo,
    							TLiwVariant( listOfMatchingMaps ) ) );
    			
    			changeInfoList->AppendL( TLiwGenericParam( EGenericParamError,
    			    TLiwVariant( CDataSourceInterface::ErrCodeConversion( aErrorCode ) ) ) );
    			changeInfoList->AppendL( TLiwGenericParam( KErrorCode,
    			    TLiwVariant( CDataSourceInterface::ErrCodeConversion( aErrorCode ) ) ) );
    			(*observer)->HandleNotifyL( transactionId, event, *changeInfoList,
    			        *inParamList );
    			
    		    CleanupStack::PopAndDestroy( inParamList );
    		    CleanupStack::PopAndDestroy( changeInfoList );
    			}//if

    		CleanupStack::PopAndDestroy( &transV );
    		}//if
    	CleanupStack::PopAndDestroy( listOfMatchingMaps );
    	CleanupStack::PopAndDestroy( &mapsV );
    	CleanupStack::PopAndDestroy( &param );
    	}//for
	}


// ----------------------------------------------------------------------------
//
// ----------------------------------------------------------------------------
//
void CCPActiveNotifier::DoCancel()
    {
    CP_DEBUG( _L8("CCPActiveNotifier::DoCancel()" ) );
    iServerClient.UnregisterObserver( );
    }

// ----------------------------------------------------------------------------
//
// ----------------------------------------------------------------------------
//
TInt CCPActiveNotifier::RunError( TInt aError )
    {
    if (KErrNoMemory!=aError)
        {
        TRAP_IGNORE( RegisterAgainL( ) );
        }
    return KErrNone;
    }
// ----------------------------------------------------------------------------
//
// ----------------------------------------------------------------------------
//
CCPActiveNotifier::CCPActiveNotifier( RCPServerClient& aServerClient ) :
    CActive(EPriorityStandard)
    {
    iServerClient = aServerClient;
    CActiveScheduler::Add( this );
    }

// ----------------------------------------------------------------------------
//
// ----------------------------------------------------------------------------
//
void CCPActiveNotifier::ConstructL()
    {

    }

// ----------------------------------------------------------------------------
//
// ----------------------------------------------------------------------------
//
void CCPActiveNotifier::RegisterAgainL()
    {
    CP_DEBUG( _L8("CCPActiveNotifier::RegisterAgainL()" ) );

    if (!IsActive())
        {
        TIpcArgs args;
        args.Set(KTransactionPosition, KErrAlreadyExists);
        if (iSizeDes)
            {
            delete iSizeDes;
            iSizeDes = NULL;
            }
        iSizeDes = new (ELeave) TPckgBuf<TInt> ();
        args.Set(KReturnPosition, iSizeDes);
        iServerClient.RegisterObserverL(iStatus, args);
        SetActive();
        }
    }
