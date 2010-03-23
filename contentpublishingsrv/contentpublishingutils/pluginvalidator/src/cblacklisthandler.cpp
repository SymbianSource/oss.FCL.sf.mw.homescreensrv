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

#include <LiwCommon.h>
#include <s32mem.h>
#include "cpserverdef.h"

#include "cpglobals.h"
#include "cblacklisthandler.h"
#include "centralrepository.h"


//CONST
const TUint32 KOfficialBlacklistCRKey	= { 0x1028000A };
const TUint32 KOfficialBlacklistSizeCRKey	= { 0x1028000B };
const TUint32 KUnofficialBlacklistCRKey 	= { 0x1028000C };
const TUint32 KUnoficialBlacklistSizeCRKey	= { 0x1028000D };

// ----------------------------------------------------------------------------
//
// ----------------------------------------------------------------------------
//    
CBlacklistHandler* CBlacklistHandler::NewL(  )
	{
	CBlacklistHandler* self = CBlacklistHandler::NewLC( );
    CleanupStack::Pop( self );
    return self;	
	}

// ----------------------------------------------------------------------------
//
// ----------------------------------------------------------------------------
//
CBlacklistHandler* CBlacklistHandler::NewLC( )
	{
	CBlacklistHandler* self = new( ELeave ) CBlacklistHandler( );
    CleanupStack::PushL( self );
    self->ConstructL();
    return self;	
	}

// ----------------------------------------------------------------------------
//
// ----------------------------------------------------------------------------
//
CBlacklistHandler::~CBlacklistHandler()
	{
	delete iRepository;
	}

// ----------------------------------------------------------------------------
//
// ----------------------------------------------------------------------------
//
CBlacklistHandler::CBlacklistHandler(  )
	{
	
	}

// ----------------------------------------------------------------------------
//
// ----------------------------------------------------------------------------
//
void CBlacklistHandler::ConstructL()
	{
	iRepository = CRepository::NewL( KServerUid );
	}

// ----------------------------------------------------------------------------
//
// ----------------------------------------------------------------------------
//
void CBlacklistHandler::AppendL( TUid aUid )
	{
	RArray<TUid> blacklist;
	CleanupClosePushL( blacklist );
	GetArrayL( blacklist, EFalse ); //unofficial blacklist
	blacklist.AppendL( aUid );
	StoreArrayL( blacklist, EFalse );//unofficial blacklist
	CleanupStack::PopAndDestroy( &blacklist );	
	}

// ----------------------------------------------------------------------------
//
// ----------------------------------------------------------------------------
//
void CBlacklistHandler::RemoveL( TUid aUid )
	{
	RArray<TUid> blacklist;
	CleanupClosePushL( blacklist );
	GetArrayL( blacklist, EFalse );//from unofficial blacklist
	TInt index = blacklist.Find( aUid );
	if ( index != KErrNotFound )
	  	{
	   	blacklist.Remove( index );
	  	}
	StoreArrayL( blacklist, EFalse );//unofficial blacklist
	CleanupStack::PopAndDestroy( &blacklist ); 	
	}
      
// ----------------------------------------------------------------------------
//
// ----------------------------------------------------------------------------
//
TBool CBlacklistHandler::IsPresentL( TUid aUid )
	{
    TBool result(EFalse); 
    RArray<TUid> blacklist;
    CleanupClosePushL( blacklist );
    GetArrayL( blacklist, ETrue );// from official blacklist
    TInt index = blacklist.Find( aUid );
    if ( index != KErrNotFound )
    	{
    	result = ETrue;
    	}
    CleanupStack::PopAndDestroy( &blacklist );
    return result;	
	}
    
    
// ----------------------------------------------------------------------------
//
// ----------------------------------------------------------------------------
//
void CBlacklistHandler::GetArrayL( RArray<TUid>& aArray, TBool aOfficial )
	{
	TInt32 repositoryKey( NULL );
	TInt32 repositorySize( NULL );
	if( aOfficial )
		{
		repositorySize = KOfficialBlacklistSizeCRKey;
		repositoryKey = KOfficialBlacklistCRKey;
		}
	else
		{
		repositorySize = KUnoficialBlacklistSizeCRKey;
		repositoryKey = KUnofficialBlacklistCRKey;
		}
	
	TInt size(-1);
	if ( KErrNone == iRepository->Get( repositorySize, size ) )
		{
		RBuf8 serializedBlacklist;
		serializedBlacklist.Create( size );
		CleanupClosePushL( serializedBlacklist );
		if ( KErrNone == iRepository->Get( repositoryKey, serializedBlacklist ) )
			{
			RDesReadStream stream( serializedBlacklist );
			CleanupClosePushL( stream );
			TUint16 count = stream.ReadUint16L();
			for ( TInt i  = 0; i < count; i++ )
				{
				aArray.AppendL(TUid::Uid(stream.ReadInt32L()));
				}
			CleanupStack::PopAndDestroy( &stream );
			}
		CleanupStack::PopAndDestroy( &serializedBlacklist );
		}
	}

// ----------------------------------------------------------------------------
//
// ----------------------------------------------------------------------------
//
void CBlacklistHandler::StoreArrayL( const RArray<TUid>& aArray, 
		TBool aOfficial )
	{
	TInt32 repositoryKey( NULL );
	TInt32 repositorySize( NULL );
	if( aOfficial )
		{
		repositorySize = KOfficialBlacklistSizeCRKey;
		repositoryKey = KOfficialBlacklistCRKey;
		}
	else
		{
		repositorySize = KUnoficialBlacklistSizeCRKey;
		repositoryKey = KUnofficialBlacklistCRKey;
		}	
	
	RBuf8 serializedBlacklist;
	serializedBlacklist.Create( aArray.Count() * sizeof(TInt32) + 
			sizeof(TUint16) );
	CleanupClosePushL( serializedBlacklist );
	RDesWriteStream stream( serializedBlacklist );
	CleanupClosePushL( stream );
	stream.WriteUint16L( aArray.Count() );
	for ( TInt i = 0;  i< aArray.Count(); i++ )
		{
		stream.WriteInt32L( aArray[i].iUid );
		}
	stream.CommitL();	
	if ( KErrNone == iRepository->Set( repositoryKey, serializedBlacklist ) )
		{
		iRepository->Set( repositorySize, serializedBlacklist.Size()  );
		}
	CleanupStack::PopAndDestroy( &stream );
	CleanupStack::PopAndDestroy( &serializedBlacklist );
	}

// ----------------------------------------------------------------------------
//
// ----------------------------------------------------------------------------
//
void CBlacklistHandler::CopyBlacklistL( TBool aOfficialToUnofficial )
	{
	RArray<TUid> blacklist;
	CleanupClosePushL( blacklist );
	GetArrayL( blacklist, aOfficialToUnofficial );
	StoreArrayL( blacklist, !aOfficialToUnofficial );
	CleanupStack::PopAndDestroy( &blacklist );
	}

