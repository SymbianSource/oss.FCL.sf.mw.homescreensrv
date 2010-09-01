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
    CP_EXTENDED_DEBUG( "GetListL()" , aInParamList );
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
    CP_EXTENDED_DEBUG( "Add()" , aInParamList );
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
    CP_EXTENDED_DEBUG( "Delete()" , aInParamList );
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
                                   const CLiwGenericParamList& aInParamList, 
                                   TInt32 aTransactionId, 
                                   TUint aCmdOptions )
    {
    CP_DEBUG( _L8("CCPClient::RegisterObserverL()") );
    CP_EXTENDED_DEBUG( "RegisterObserver()" , aInParamList );
    CheckMapL( aInParamList, KFilter );
    CCPLiwMap* inMapForServer = CCPLiwMap::NewL( aInParamList );
    inMapForServer->PushL( );
    inMapForServer->IsValidForNotificationL( );
    if ( !iActiveNotifier )
        {
        iActiveNotifier = CCPActiveNotifier::NewL( iServerClient );
        }
    iActiveNotifier->RegisterL( aObserver, aTransactionId, inMapForServer,
                                                           aCmdOptions );
    CleanupStack::PopAndDestroy( inMapForServer );
    }

// -----------------------------------------------------------------------------
// 
// -----------------------------------------------------------------------------
//     
void CCPClient::UnregisterObserversL( const CLiwGenericParamList& aInParamList )
    {
    CP_DEBUG( _L8("CCPClient::UnregisterObservers()") );
    CP_EXTENDED_DEBUG( "UnregisterObservers()" , aInParamList );
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
void CCPClient::ExecuteActionL( const CLiwGenericParamList& aInParamList,
        TUint aCmdOptions)
    {
    CP_DEBUG( _L8("CCPClient::ExecuteActionL()") );
    CP_EXTENDED_DEBUG( "ExecuteAction()" , aInParamList );
    CheckMapL( aInParamList, KFilter );
    CCPLiwMap* inMapForServer = CCPLiwMap::NewL( aInParamList );
    inMapForServer->PushL( );
    inMapForServer->IsValidForActionL( );
    iServerClient.ExecuteActionL( *inMapForServer, aCmdOptions );
    CleanupStack::PopAndDestroy( inMapForServer );
    }

// -----------------------------------------------------------------------------
// 
// -----------------------------------------------------------------------------
//    
void CCPClient::ExecuteMultipleActionsL(
        const CLiwGenericParamList& aInParamList, TUint aCmdOptions)
    {
    CP_DEBUG( _L8("CCPClient::ExecuteMultipleActionsL()") );
    CP_EXTENDED_DEBUG( "ExecuteMultipleActionsL()" , aInParamList );
    CheckMultiExecuteInputParamsL(aInParamList);
    iServerClient.ExecuteMultipleActionsL( aInParamList, aCmdOptions );
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

// -----------------------------------------------------------------------------
//
// --------------- --------------------------------------------------------------
//
void CCPClient::CheckMultiExecuteInputParamsL(
        const CLiwGenericParamList& aList)
    {
    const TLiwGenericParam* param = NULL;
    TInt pos(0);
    param = aList.FindFirst(pos, KFilters);
    User::LeaveIfError(pos); //leaves if not found
    if (param->Value().TypeId() != LIW::EVariantTypeList)
        {
        User::Leave(KErrBadName);
        }
    }
