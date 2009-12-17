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

// INCLUDE FILES

#include "menuutil.h"
#include "menulogger.h"

#include <s32strm.h>
#include <e32property.h>
#include <connect/sbdefs.h>

// ================= MEMBER FUNCTIONS =======================

// ---------------------------------------------------------
// MenuUtil::ExternalizeL
// ---------------------------------------------------------
//
void MenuUtil::ExternalizeL
( const RArray<TInt>& aArray, RWriteStream& aStream )
    {
    aStream.WriteInt32L( aArray.Count() );
    for ( TInt i = 0; i < aArray.Count(); i++ )
        {
        aStream.WriteInt32L( aArray[i] );
        }
    }

// ---------------------------------------------------------
// MenuUtil::InternalizeL
// ---------------------------------------------------------
//
void MenuUtil::InternalizeL
( RArray<TInt>& aArray, RReadStream& aStream )
    {
    TInt count = aStream.ReadInt32L();
    for ( TInt i = 0; i < count; i++ )
        {
        aArray.AppendL( aStream.ReadInt32L() );
        }
    }

// ---------------------------------------------------------
// MenuUtil::ExternalizeL
// ---------------------------------------------------------
//
void MenuUtil::ExternalizeL
( const RArray<TUid>& aArray, RWriteStream& aStream )
    {
    aStream.WriteInt32L( aArray.Count() );
    for ( TInt i = 0; i < aArray.Count(); i++ )
        {
        aStream.WriteInt32L( aArray[i].iUid );
        }
    }

// ---------------------------------------------------------
// MenuUtil::InternalizeL
// ---------------------------------------------------------
//
void MenuUtil::InternalizeL
( RArray<TUid>& aArray, RReadStream& aStream )
    {
    TInt count = aStream.ReadInt32L();
    for ( TInt i = 0; i < count; i++ )
        {
        aArray.AppendL( TUid::Uid(aStream.ReadInt32L()) );
        }
    }

// ---------------------------------------------------------
// MenuUtil::ExternalizeL
// ---------------------------------------------------------
//
void MenuUtil::ExternalizeL
( const RArray<TMenuItem>& aArray, RWriteStream& aStream )
    {
    aStream.WriteInt32L( aArray.Count() );
    for ( TInt i = 0; i < aArray.Count(); i++ )
        {
        aArray[i].ExternalizeL( aStream );
        }
    }

// ---------------------------------------------------------
// MenuUtil::InternalizeL
// ---------------------------------------------------------
//
void MenuUtil::InternalizeL
( RArray<TMenuItem>& aArray, RReadStream& aStream )
    {
    TMenuItem hdr;
    TInt count = aStream.ReadInt32L();
    for ( TInt i = 0; i < count; i++ )
        {
        hdr.InternalizeL( aStream );
        aArray.AppendL( hdr );
        }
    }

// ---------------------------------------------------------
// MenuUtil::BackupInProgressL
// ---------------------------------------------------------
//
TBool MenuUtil::BackupInProgressL()
    {
    TBool backupInProgress( EFalse );
    TInt backupRestoreStatus( 0 );
    const TInt err = RProperty::Get( KUidSystemCategory, 
        					         conn::KUidBackupRestoreKey,
        					         backupRestoreStatus );

    if( err )
        {
        CLOG_WRITE_FORMAT8( "B&R error: %d", err );
        if( KErrNotFound != err )
            {
            User::Leave( err );
            }
        }
    else
    	{
    	CLOG_WRITE_FORMAT8( "B&R status: 0x%x", backupRestoreStatus );
	    if( backupRestoreStatus == conn::EBURUnset || 
	        backupRestoreStatus & conn::EBURNormal ||
	        (backupRestoreStatus & conn::KBackupIncTypeMask) == conn::ENoBackup)
	        {
	        backupInProgress = EFalse;
	        }
	    else
	        {
	        backupInProgress = ETrue;
	        }
    	}
    	
    return backupInProgress;
    }

//  End of File  
