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
* Description:
*
*/
#include <e32svr.h>
#include "contentobserver.h"


CContentObserver& CContentObserver::InstanceL()
	{
	CContentObserver* self = static_cast<CContentObserver*>( Dll::Tls() );
	if( self )
		{
		return *self;
		}

	self = new( ELeave )CContentObserver;
	CleanupStack::PushL( self );
	User::LeaveIfError( Dll::SetTls( self ) );
	CleanupStack::Pop( self );
	return *self;
	}


void CContentObserver::Release()
	{
	CContentObserver* self = static_cast<CContentObserver*>( Dll::Tls() );
	delete self;
	Dll::SetTls( NULL );
	}


void CContentObserver::Reset()
	{
	CContentObserver* self = static_cast<CContentObserver*>( Dll::Tls() );
	if( self )
		{
		self->iCache.ResetAndDestroy();
		}
	}


CContentCache* CContentObserver::GetContent( TInt aContentId, TBool aClean )
	{
	const TInt count( iCache.Count() );
	for( TInt i( 0 ); i < count; i++ )
		{
		if( iCache[i]->iId == aContentId )
			{
			if( !aClean && iCache[i]->iClean)
				{
				continue;
				}
			return iCache[i];
			}
		}

	return NULL;
	}


CContentCache* CContentObserver::GetLastContent( TInt aContentId, TBool aClean )
	{
	const TInt count( iCache.Count() );
	for( TInt i( count - 1 ); i >= 0; i-- )
		{
		if( iCache[i]->iId != aContentId )
			{
			return NULL;
			}
		if( !aClean && iCache[i]->iClean)
			{
			continue;
			}
		return iCache[i];
		}
	return NULL;
	}


CContentObserver::~CContentObserver()
	{
	iCache.ResetAndDestroy();
	}


TInt CContentObserver::StartTransaction(TInt /*aTxId*/)
	{
	return 0;
	}


TInt CContentObserver::Commit(TInt /*aTxId*/)
	{
	return 0;
	}


TInt CContentObserver::CancelTransaction(TInt /*aTxId*/)
	{
	return 0;
	}
	

TBool CContentObserver::CanPublish(CHsContentPublisher& /*aPlugin*/, TInt /*aContent*/, TInt /*aIndex*/ )
    {
    return EFalse;
    }


TInt CContentObserver::Publish(CHsContentPublisher& /*aPlugin*/, TInt aContent, TInt aResource, TInt aIndex )
	{
	RDebug::Print( _L("Publish(%d, %d, %d)"), aContent, aResource, aIndex );
	CContentCache* cache = new( ELeave )CContentCache;
	CleanupStack::PushL( cache );
	cache->iId = aContent;
	cache->iResource = aResource;
	cache->iIndex = aIndex;
	User::LeaveIfError( iCache.Append( cache ) );
	CleanupStack::Pop( cache );
	return 0;
	}

TInt CContentObserver::Publish(CHsContentPublisher& /*aPlugin*/, TInt aContent, const TDesC16& aText, TInt aIndex )
	{
	RDebug::Print( _L("Publish(%d, \"%S\", %d)"), aContent, &aText, aIndex );
	CContentCache* cache = new( ELeave )CContentCache;
	CleanupStack::PushL( cache );
	cache->iId = aContent;
	cache->iText = aText.AllocL();
	cache->iIndex = aIndex;
	User::LeaveIfError( iCache.Append( cache ) );
	CleanupStack::Pop( cache );
	return 0;
	}


TInt CContentObserver::Publish(CHsContentPublisher& /*aPlugin*/, TInt aContent, const TDesC8& aBuf, TInt aIndex )
	{
	RDebug::Print( _L("Publish(%d, Buf.Len=%d, %d)"), aContent, aBuf.Length(), aIndex );
	CContentCache* cache = new( ELeave )CContentCache;
	CleanupStack::PushL( cache );
	cache->iId = aContent;
	cache->iData = aBuf.AllocL();
	cache->iIndex = aIndex;
	User::LeaveIfError( iCache.Append( cache ) );
	CleanupStack::Pop( cache );
	return 0;
	}


TInt CContentObserver::Publish(CHsContentPublisher& /*aPlugin*/, TInt aContent, RFile& /*aFile*/, TInt aIndex )
	{
	CContentCache* cache = new( ELeave )CContentCache;
	CleanupStack::PushL( cache );
	cache->iId = aContent;
	cache->iIndex = aIndex;
	User::LeaveIfError( iCache.Append( cache ) );
	CleanupStack::Pop( cache );
	return 0;
	}


TInt CContentObserver::Clean(CHsContentPublisher& /*aPlugin*/, TInt aContent, TInt aIndex)
	{
	RDebug::Print( _L("Clean(%d, %d)"), aContent, aIndex );
	CContentCache* cache = new( ELeave )CContentCache;
	CleanupStack::PushL( cache );
	cache->iId = aContent;
	cache->iIndex = aIndex;
	cache->iClean = ETrue;
	User::LeaveIfError( iCache.Append( cache ) );
	CleanupStack::Pop( cache );
	return 0;
	}


TAny* CContentObserver::Extension(TUid /*aUid*/)
	{
	return NULL;
	}

TBool CContentObserver::RequiresSubscription( const THsPublisherInfo& /*aPublisherInfo*/ ) const
    {
    return EFalse;
    }
    
// ---------------------------------------------------------------------------
// SetProperty
// ---------------------------------------------------------------------------
//
TInt CContentObserver::SetProperty( CHsContentPublisher& /*aPlugin*/,
									          const TDesC8& /*aElementId*/,
									          const TDesC8& /*aPropertyName*/,
									          const TDesC8& /*aPropertyValue*/ )
{
	return KErrNone;
}				

// ---------------------------------------------------------------------------
// SetProperty
// ---------------------------------------------------------------------------
//
TInt CContentObserver::SetProperty( CHsContentPublisher& /*aPlugin*/,
									          const TDesC8& /*aElementId*/,
									          const TDesC8& /*aPropertyName*/,
									          const TDesC8& /*aPropertyValue*/,
									          MAiContentObserver::TValueType /*aValueType*/ )
{
	return KErrNone;
}				    

//  END OF FILE
