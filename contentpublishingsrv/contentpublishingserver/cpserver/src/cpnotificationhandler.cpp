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


#include <liwcommon.h>
#include <s32mem.h>

#include "cpnotificationhandler.h"
#include "cpdebug.h"
#include "cpliwmap.h"
#include "cpglobals.h"
#include "cpserverdef.h"

static const int KMaxKeyLength =  256;
using namespace LIW;
// ======== MEMBER FUNCTIONS ========

// ---------------------------------------------------------------------------
// 
// ---------------------------------------------------------------------------
//
CCPNotificationHandler::CCPNotificationHandler( TLiwVariant& aDataMapCache ):
        iDataMapCache(aDataMapCache)
    {

    }

// ---------------------------------------------------------------------------
// 
// ---------------------------------------------------------------------------
//
void CCPNotificationHandler::ConstructL( RPointerArray<CLiwDefaultList>& 
        aNotifications)
    {
    iChangeInfoList = CLiwGenericParamList::NewL( );
    for ( TInt i(0); i< aNotifications.Count( ); i++ )
        {
        iNotifications.AppendL(aNotifications[i]);
        aNotifications[i]->IncRef();
        }    
    }

// ---------------------------------------------------------------------------
// 
// ---------------------------------------------------------------------------
//
CCPNotificationHandler* CCPNotificationHandler::NewL(RPointerArray<
        CLiwDefaultList>& aNotifications, TLiwVariant& aDataMapCache)
    {
    CCPNotificationHandler* self = CCPNotificationHandler::NewLC(
            aNotifications, aDataMapCache);
    CleanupStack::Pop( self );
    return self;
    }

// ---------------------------------------------------------------------------
// 
// ---------------------------------------------------------------------------
//
CCPNotificationHandler* CCPNotificationHandler::NewLC(RPointerArray<
        CLiwDefaultList>& aNotifications, TLiwVariant& aDataMapCache)
    {
    CCPNotificationHandler* self = new (ELeave) CCPNotificationHandler(
            aDataMapCache);
    CleanupStack::PushL( self );
    self->ConstructL( aNotifications );
    return self;
    }

// ---------------------------------------------------------------------------
// 
// ---------------------------------------------------------------------------
//
CCPNotificationHandler::~CCPNotificationHandler()
    {
    THashMapIter<TInt32, CCPLiwMap*> iter( iFilters );

    const TInt32* transaction = iter.NextKey();
    while( transaction )
        {
        CCPLiwMap** filter = iFilters.Find( *transaction );// leaves if not found
        if ( filter )
        	{
        	(*filter)->Reset();
        	(*filter)->Close();
        	}
        transaction = iter.NextKey( );
        }  
    iFilters.Close();    
    iExtendedFlags.Close();
    Reset( );
    for ( TInt i(0); i< iNotifications.Count( ); i++ )
        {
        iNotifications[i]->Close( );
        }
    iNotifications.Close( );
    delete iChangeInfoList;
    }

// ---------------------------------------------------------------------------
//
// ---------------------------------------------------------------------------
//    
void CCPNotificationHandler::SaveMessageL( const RMessage2& aMessage )
    {
    iIsReadyToSend = ETrue;
    iMessage = aMessage;
    
    TInt32 transactionId = aMessage.Int3();
    if ( transactionId != KErrAlreadyExists )
    	{
    	TInt deslen = aMessage.GetDesLengthL( KDescriptorPosition );
    	if( deslen > 0 )
    		{
    		HBufC8* buffer = HBufC8::NewLC( deslen );
    		TPtr8 tempDes = buffer->Des( );
    		aMessage.Read( KDescriptorPosition, tempDes );
    		RDesReadStream datastrm( *buffer);
    		CleanupClosePushL( datastrm );
    		CCPLiwMap* filterMap = CCPLiwMap::NewL( datastrm );
    		CleanupStack::PushL( filterMap );
    		iFilters.InsertL( transactionId, filterMap );
    	    TUint options = static_cast<TUint> (aMessage.Int2()); // 2 == KOptionsPosition
            iExtendedFlags.InsertL(transactionId, options
                    & KExtendedNotifications);
    		CleanupStack::Pop( filterMap );
    		CleanupStack::PopAndDestroy( &datastrm );
    		CleanupStack::PopAndDestroy( buffer );
    		}
    	}
	if ( iNotifications.Count( ) )
		{
		SendNotificationL( NULL );
		}   
    }

// ---------------------------------------------------------------------------
//
// ---------------------------------------------------------------------------
//    
void CCPNotificationHandler::AddObserverL( const RMessage2& aMessage )
    {
    TInt32 transactionId = aMessage.Int3();
    TInt deslen = aMessage.GetDesLength( KDescriptorPosition );
    if( deslen > 0 )
        {
        HBufC8* buffer = HBufC8::NewLC( deslen );
        TPtr8 tempDes = buffer->Des( );
        aMessage.Read( KDescriptorPosition, tempDes );
        RDesReadStream datastrm( *buffer);
        CleanupClosePushL( datastrm );
        CCPLiwMap* filterMap = CCPLiwMap::NewL( datastrm );
		CleanupStack::PushL( filterMap );
        iFilters.InsertL( transactionId, filterMap );
        TUint options = static_cast<TUint> (aMessage.Int2()); // 2 == KOptionsPosition
        iExtendedFlags.InsertL(transactionId, options
                & KExtendedNotifications);
		CleanupStack::Pop( filterMap );
        CleanupStack::PopAndDestroy( &datastrm );
        CleanupStack::PopAndDestroy( buffer );
        }    
    }

// ---------------------------------------------------------------------------
//
// ---------------------------------------------------------------------------
//    
void CCPNotificationHandler::RemoveObserverL( const RMessage2& aMessage )
    {
    TInt32 transactionId = aMessage.Int3();
    CCPLiwMap* filter = iFilters.FindL( transactionId );// leaves if not found
    filter->Reset();
    filter->Close();
    iFilters.Remove( transactionId );
    iExtendedFlags.Remove( transactionId );
    }

// -----------------------------------------------------------------------------
// 
// -----------------------------------------------------------------------------
//
TBool CCPNotificationHandler::IsProperForFilterL( const CLiwMap& aMap, 
		const CCPLiwMap& aFilter )
    {
    CP_DEBUG( _L8("CCPNotificationHandler::IsProperForFilter()") );
    TBool result( EFalse );
    if( CheckRegistryTypeL( aMap, aFilter ) && 
    		CheckOperationTypeL( aMap, aFilter ) )
        {
        if( CheckIdL( aMap, aFilter ) )
            {
            result= ETrue;
            }
        else if( CheckPropertiesL( aMap, aFilter ) )
            {
            result = ETrue;
            }
        }
    return result;
    }

// ----------------------------------------------------------------------------
//
// ----------------------------------------------------------------------------
//
void CCPNotificationHandler::SendNotificationL( CLiwDefaultList* aListOfMaps )
    {
    CP_DEBUG( _L8("CCPNotificationHandler::SendNotificationL()") );
    if ( iIsReadyToSend )
        {
        //notification can be send
        if ( iNotifications.Count( ) )
            {
            //at least one notification is waiting to be sent
            TBool sent;
            sent = SendChangeInfoListL( iNotifications[0] );
            iNotifications[0]->Close( );
            iNotifications.Remove( 0 );
            if ( !sent )
                {
                //if notification from array wasn't sent because it didn't 
                //match the filter, try to send the next one
                SendNotificationL( NULL );
                }
            else
                {
                //message was send
                iIsReadyToSend = EFalse;
                }
            }
        else
            {
            //no notification is waiting, so send the new one
            if ( aListOfMaps && SendChangeInfoListL( aListOfMaps ) )
                {
                iIsReadyToSend = EFalse;
                }
            }
        }
    else
        {
        //notification cannot be sent in this moment
        aListOfMaps->IncRef( );
        iNotifications.AppendL( aListOfMaps );
        }
    }

// ----------------------------------------------------------------------------
//
// ----------------------------------------------------------------------------
//

const CLiwGenericParamList* CCPNotificationHandler::GetPointerToChangeInfoList()
    {
    return iChangeInfoList;
    }

// ----------------------------------------------------------------------------
//
// ----------------------------------------------------------------------------
//
void CCPNotificationHandler::Reset()
    {
    iIsReadyToSend = EFalse;
    iChangeInfoList->Reset( );
    }

// ----------------------------------------------------------------------------
//
// ----------------------------------------------------------------------------
//
void CCPNotificationHandler::ErrorComplete( TInt aErrCode )
    {
    if( iIsReadyToSend && iMessage.IsNull() == EFalse )
        {
        iMessage.Complete( aErrCode );
        iIsReadyToSend = EFalse;
        }
    }

// ----------------------------------------------------------------------------
//
// ----------------------------------------------------------------------------
//
TBool CCPNotificationHandler::SendChangeInfoListL(
    const CLiwDefaultList* aListOfMaps )
    {
    CP_DEBUG( _L8("CCPNotificationHandler::SendChangeInfoListL()") );
	iChangeInfoList->Reset( );
    TBool sent(EFalse);
    //for every observer in session
    THashMapIter<TInt32, CCPLiwMap*> iter( iFilters );
    const CCPLiwMap*const* filter = iter.NextValue( );
    while( filter )
        {
        CLiwDefaultList* listOfMatchingMaps = CLiwDefaultList::NewLC( );
        TInt count = aListOfMaps->Count( );
        //for every item in the input list
		for ( TInt j = 0; j < count; j++ )
			{
	        TLiwVariant variant;
	        variant.PushL( );
			aListOfMaps->AtL( j, variant );
            if ( variant.TypeId() == EVariantTypeMap )
                {
                const CLiwMap* map = variant.AsMap();
                if ( IsProperForFilterL( *map, **filter ) )
                    {
                    if (iExtendedFlags.FindL(*iter.CurrentKey())
                            && (iDataMapCache.TypeId() == EVariantTypeDesC8))
                        {
                        //extended notifications - append data map
                        CLiwDefaultMap* extendedMap = CLiwDefaultMap::NewLC();
                        TInt count = map->Count();
                        for(TInt i=0; i<count;i++)
                            {
                            TLiwVariant temporary;
                            temporary.PushL();
                            TBuf8<KMaxKeyLength> key;
                            map->AtL(i,key);
                            map->FindL(key, temporary);
                            extendedMap->InsertL(key,temporary);
                            CleanupStack::PopAndDestroy(&temporary);
                            }
                        extendedMap->InsertL(KDataMap,iDataMapCache);
                        listOfMatchingMaps->AppendL( TLiwVariant(extendedMap) );
                        CleanupStack::PopAndDestroy(extendedMap);
                        }
                    else
                        {
                        listOfMatchingMaps->AppendL( variant );
                        }
                    }
                }
            CleanupStack::PopAndDestroy( &variant );
			}
		if ( listOfMatchingMaps->Count( ) )
			{
			//append transaction id
			listOfMatchingMaps->AppendL( TLiwVariant( *iter.CurrentKey() ));
			iChangeInfoList->AppendL( TLiwGenericParam( KChangeInfo,
				TLiwVariant( listOfMatchingMaps ) ) );
			}
		CleanupStack::PopAndDestroy( listOfMatchingMaps );
		filter = iter.NextValue( );
        }
    if( iChangeInfoList->Count( ) )
    	{
    	TPckgBuf<TInt> sizeDes( iChangeInfoList->Size( ) );
    	TInt err = iMessage.Write( KReturnPosition, sizeDes );
    	iMessage.Complete( err );
    	sent = ETrue;
    	}
	return sent;
    }

// ----------------------------------------------------------------------------
//
// ----------------------------------------------------------------------------
//
void CCPNotificationHandler::GetPropertyL( const CLiwMap& aMap,
    const TDesC8& aProperty, RBuf& aResult )
    {
    TLiwVariant value;
    value.PushL( );
    if ( aMap.FindL( aProperty, value ) )
        {
        TPtrC result( KNullDesC );
        value.Get( result );
        aResult.CreateL( result );
        }
    CleanupStack::PopAndDestroy( &value );
    }
    
// ----------------------------------------------------------------------------
//
// ----------------------------------------------------------------------------
//
TBool CCPNotificationHandler::CheckIdL( const CLiwMap& aMap, 
		const CCPLiwMap& aFilter )
    {
    TBool result = EFalse;
    TInt32 id( 0);
    if ( aFilter.GetProperty( KId, id ) )
        {
        TLiwVariant value;
        value.PushL( );
        if ( aMap.FindL( KId, value ) )
            {
            TUint idToCompare( 0);
            if ( value.Get( idToCompare ) )
                {
                if ( id == idToCompare )
                    {
                    result = ETrue;
                    }
                }
            }
        CleanupStack::PopAndDestroy( &value );
        }
    return result;
    }

// ----------------------------------------------------------------------------
//
// ----------------------------------------------------------------------------
//
TBool CCPNotificationHandler::CheckPropertiesL( const CLiwMap& aMap,
		const CCPLiwMap& aFilter )
    {
    TBool result( EFalse );
    TBool publisherName( ETrue );
    TBool contentType( ETrue );
    TBool contentId( ETrue );
    RBuf publisher;
    publisher.CleanupClosePushL();

    if ( aFilter.GetPropertyL( KPublisherId, publisher ) )
        {
        if ( publisher != KAll )
            {
            RBuf str2compare;
            str2compare.CleanupClosePushL();
            GetPropertyL( aMap, KPublisherId, str2compare );
            if ( publisher!=str2compare )
                {
                publisherName = EFalse;
                }
            CleanupStack::PopAndDestroy( &str2compare );
            }
        }
    RBuf content;
    content.CleanupClosePushL();
    if ( aFilter.GetPropertyL( KContentType, content ) )
        {
        if ( content != KAll )
            {
            RBuf str2compare;
            str2compare.CleanupClosePushL();
            GetPropertyL( aMap, KContentType, str2compare );
            if ( content!=str2compare )
                {
                contentType = EFalse;
                }
            CleanupStack::PopAndDestroy( &str2compare );
            }
        }
    RBuf content_id;
    content_id.CleanupClosePushL();
    if ( (publisher != KAll)||(content != KAll) )
        {
        if ( aFilter.GetPropertyL( KContentId, content_id ) )
            {
            if ( content_id != KAll )
                {
				RBuf str2compare;
				str2compare.CleanupClosePushL();
				GetPropertyL( aMap, KContentId, str2compare );
				if ( content_id!=str2compare )
					{
					contentId = EFalse;
					}
				CleanupStack::PopAndDestroy( &str2compare );
                }
            }
        }
    if ( publisherName && contentType && contentId )
        {
        result = ETrue;
        }
    CleanupStack::PopAndDestroy( &content_id );
    CleanupStack::PopAndDestroy( &content );
    CleanupStack::PopAndDestroy( &publisher );
    return result;
    }

// ----------------------------------------------------------------------------
//
// ----------------------------------------------------------------------------
//
TBool CCPNotificationHandler::CheckOperationTypeL( const CLiwMap& aMap,
		const CCPLiwMap& aFilter )
    {
    TBool result = ETrue;
    RBuf operation;
    operation.CleanupClosePushL();
    if ( aFilter.GetPropertyL( KOperation, operation ) )
        {
        result = EFalse;
        TLiwVariant value;
        value.PushL( );
        if ( aMap.FindL( KOperation, value ) )
            {
            TPtrC operationToCompare( KNullDesC );
            if ( value.Get( operationToCompare ) )
                {
                if ( operation.Find( operationToCompare ) != KErrNotFound )
                    {
                    result = ETrue;
                    }
                }
            }
        CleanupStack::PopAndDestroy( &value );
        }
    CleanupStack::PopAndDestroy( &operation );
    return result;
    }
    
// ----------------------------------------------------------------------------
//
// ----------------------------------------------------------------------------
//
TBool CCPNotificationHandler::CheckRegistryTypeL( const CLiwMap& aMap,
		const CCPLiwMap& aFilter )
    {
    TBool result = ETrue;
    RBuf type;
    type.CleanupClosePushL();
    if ( aFilter.GetPropertyL( KType, type ) )
        {
        result = EFalse;
        TLiwVariant value;
        value.PushL( );
        if ( aMap.FindL( KType, value ) )
            {
            TPtrC typeToCompare( KNullDesC );
            if ( value.Get( typeToCompare ) )
                {
                if ( type.Find( typeToCompare ) != KErrNotFound )
                    {
                    result = ETrue;
                    }
                }
            }
        CleanupStack::PopAndDestroy( &value );
        }
    CleanupStack::PopAndDestroy( &type );
    return result;
    }    
