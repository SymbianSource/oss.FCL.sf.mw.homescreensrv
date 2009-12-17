/*
* Copyright (c) 2007 Nokia Corporation and/or its subsidiary(-ies). 
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


#include <liwgenericparam.h>

#include "cpliwmap.h"
#include "cpclient.h"
#include "cpserverdef.h"
#include "cpdebug.h"
#include "cpclientactivenotifier.h"

// ======== MEMBER FUNCTIONS ========

// -----------------------------------------------------------------------------
// 
// -----------------------------------------------------------------------------
// 
CCPClient::CCPClient()
    {
    }

// -----------------------------------------------------------------------------
// 
// -----------------------------------------------------------------------------
// 
void CCPClient::ConstructL()
    {

#ifdef CONTENT_PUBLISHER_DEBUG
    if(!CCPDebug::Enable())
        {
        iDebug = CCPDebug::NewL( KCPDebugFileName );
        }
#endif
    CP_DEBUG( _L8("CCPClient::ConstructL()") );
    User::LeaveIfError( iServerClient.Connect( ) );
    }

// -----------------------------------------------------------------------------
// 
// -----------------------------------------------------------------------------
//
CCPClient* CCPClient::NewL()
    {
    CCPClient* self = CCPClient::NewLC( );
    CleanupStack::Pop( self );
    return self;
    }

// -----------------------------------------------------------------------------
// 
// -----------------------------------------------------------------------------
//    
CCPClient* CCPClient::NewLC()
    {
    CCPClient* self = new( ELeave ) CCPClient;
    CleanupStack::PushL( self );
    self->ConstructL( );
    return self;
    }

// -----------------------------------------------------------------------------
// 
// -----------------------------------------------------------------------------
//
CCPClient::~CCPClient()
    {
    CP_DEBUG( _L8("CCPClient::~CCPClient()") );
    delete iActiveNotifier;
    iServerClient.Close( );
#ifdef CONTENT_PUBLISHER_DEBUG
    delete iDebug;
#endif
    }

// -----------------------------------------------------------------------------
// 
// -----------------------------------------------------------------------------
//       
void CCPClient::GetListL( const CLiwGenericParamList& aInParamList,
    CLiwGenericParamList& aOutParamList )
    {
    CP_DEBUG( _L8("CCPClient::GetListL()") );
    CheckMapL( aInParamList, KFilter );
    CCPLiwMap* inMapForServer = CCPLiwMap::NewL( aInParamList );
    inMapForServer->PushL( );
    inMapForServer->IsValidForGetListL( );
    iServerClient.GetListL( *inMapForServer, aOutParamList );
    CleanupStack::PopAndDestroy( inMapForServer );
    }

// -----------------------------------------------------------------------------
// 
// -----------------------------------------------------------------------------
//     
void CCPClient::AddL( const CLiwGenericParamList& aInParamList,
                      CLiwGenericParamList& aOutParamList,
                      TUint aCmdOptions )
    {
    CP_DEBUG( _L8("CCPClient::AddL()") );
    CheckMapL( aInParamList, KItem );
    CCPLiwMap* inMapForServer = CCPLiwMap::NewL( aInParamList ) ;
    inMapForServer->PushL( );
    inMapForServer->IsValidForAddL( );
    iServerClient.AddL( *inMapForServer, aOutParamList, aCmdOptions );
    CleanupStack::PopAndDestroy( inMapForServer );
    }

// -----------------------------------------------------------------------------
// 
// -----------------------------------------------------------------------------
//     
void CCPClient::DeleteL( const CLiwGenericParamList& aInParamList )
    {
    CP_DEBUG( _L8("CCPClient::DeleteL()") );
    CheckMapL( aInParamList, KData );
    CCPLiwMap* inMapForServer = CCPLiwMap::NewL( aInParamList );
    inMapForServer->PushL( );
    inMapForServer->IsValidForDeleteL( );
    iServerClient.DeleteL( *inMapForServer );
    CleanupStack::PopAndDestroy( inMapForServer );
    }

// -----------------------------------------------------------------------------
// 
// -----------------------------------------------------------------------------
//    
void CCPClient::RegisterObserverL( MLiwNotifyCallback* aObserver,
    const CLiwGenericParamList& aInParamList, TInt32 aTransactionId )
    {
    CP_DEBUG( _L8("CCPClient::RegisterObserverL()") );
    CheckMapL( aInParamList, KFilter );
    CCPLiwMap* inMapForServer = CCPLiwMap::NewL( aInParamList );
    inMapForServer->PushL( );
    inMapForServer->IsValidForNotificationL( );
    if ( !iActiveNotifier )
        {
        iActiveNotifier = CCPActiveNotifier::NewL( iServerClient );
        }
    iActiveNotifier->RegisterL( aObserver, aTransactionId, inMapForServer );
    CleanupStack::PopAndDestroy( inMapForServer );
    }

// -----------------------------------------------------------------------------
// 
// -----------------------------------------------------------------------------
//     
void CCPClient::UnregisterObserversL( const CLiwGenericParamList& aInParamList )
    {
    CP_DEBUG( _L8("CCPClient::UnregisterObservers()") );
    if ( !iActiveNotifier )
        {
        User::Leave( KErrNotFound );
        }  
    TInt32 transactionId( -1 );
    const TLiwGenericParam* param = NULL;
    TInt pos( 0 );
    param = aInParamList.FindFirst( pos, KTransactionID );
    if( !param ||
		pos == KErrNotFound ||
		!param->Value().Get( transactionId ) ||
		transactionId < 0 )
    	{
    	iActiveNotifier->UnregisterAllL(  );
        delete iActiveNotifier;
        iActiveNotifier = NULL;
    	}
    else
    	{
    	if( iActiveNotifier->UnregisterL( transactionId ) )
    		{
    		//delete only if it was the last observer
    	    delete iActiveNotifier;
    	    iActiveNotifier = NULL;
    		}
    	}
    }

// -----------------------------------------------------------------------------
// 
// -----------------------------------------------------------------------------
//    
void CCPClient::ExecuteActionL( const CLiwGenericParamList& aInParamList )
    {
    CP_DEBUG( _L8("CCPClient::RegisterObserverL()") );
    CheckMapL( aInParamList, KFilter );
    CCPLiwMap* inMapForServer = CCPLiwMap::NewL( aInParamList );
    inMapForServer->PushL( );
    inMapForServer->IsValidForActionL( );
    iServerClient.ExecuteActionL( *inMapForServer );
    CleanupStack::PopAndDestroy( inMapForServer );
    }

// -----------------------------------------------------------------------------
// 
// -----------------------------------------------------------------------------
//    
void CCPClient::CheckMapL( const CLiwGenericParamList& aInParamList, 
        const TDesC8& aKey )
    {
    const TLiwGenericParam* param = NULL;
    TInt pos( 0 );
    param = aInParamList.FindFirst( pos, aKey );
    if( pos !=KErrNotFound )
        {
        if( param->Value().TypeId( ) != LIW::EVariantTypeMap )
            {
            User::Leave( KErrBadName );
            }
        }
    }
