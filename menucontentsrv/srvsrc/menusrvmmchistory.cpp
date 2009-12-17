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

#include <f32file.h>
#include <s32file.h>
#include <sysutil.h> 
#include "menusrvmmchistory.h"

// CONSTANTS

LOCAL_D const TInt KMenuMmcHistoryLength = 2;

// ==================== MEMBER FUNCTIONS ====================

// ---------------------------------------------------------
// CMenuSrvMmcHistory::LoadL
// ---------------------------------------------------------
//
void CMenuSrvMmcHistory::LoadL( RFs& aFs, const TDesC& aFname )
    {
    iMmcList.Reset();
    TUint att;
    if ( KErrNotFound != aFs.Att( aFname, att ) )
        {
        RFileReadStream stream;
        CleanupClosePushL( stream );
        User::LeaveIfError( stream.Open
            ( aFs, aFname, EFileStream | EFileShareReadersOnly) );
        TInt len = Min( KMenuMmcHistoryLength, stream.ReadInt32L() );
        while( len-- )
            {
            iMmcList.AppendL( stream.ReadUint32L() );
            }
        CleanupStack::PopAndDestroy( &stream );
        }
    }

// ---------------------------------------------------------
// CMenuSrvMmcHistory::SaveL
// Method do not work if there is no free space on C drive. 
// ---------------------------------------------------------
//
void CMenuSrvMmcHistory::SaveL( RFs& aFs, const TDesC& aFname )
    {
	if ( SysUtil::DiskSpaceBelowCriticalLevelL( &aFs, 0, EDriveC) )
		{
		return;
		}
    
    RFileWriteStream stream;
    CleanupClosePushL( stream );
    User::LeaveIfError( stream.Replace
        ( aFs, aFname, EFileStream | EFileShareExclusive ) );
    stream.WriteInt32L( iMmcList.Count() );
    for ( TInt i = 0; i < iMmcList.Count(); i++ )
        {
        stream.WriteUint32L( iMmcList[i] );
        }
    CleanupStack::PopAndDestroy( &stream );
    }

// ---------------------------------------------------------
// CMenuSrvMmcHistory::InsertL
// ---------------------------------------------------------
//
void CMenuSrvMmcHistory::InsertL( TUint aMmc )
    {
    TInt i = iMmcList.Find( aMmc );
    switch ( i )
        {
        case 0:
            {
            // aMmc is already first, do nothing.
            break;
            }

        case KErrNotFound:
            {
            // aMmc is not in the list. Insert as first.
            if ( iMmcList.Count() == KMenuMmcHistoryLength )
                {
                // List already full - remove last item to make space.
                iMmcList.Remove( KMenuMmcHistoryLength - 1 );
                }
            iMmcList.InsertL( aMmc, 0 );
            break;
            }

        default:
            {
            // aMmc is in the list, but not first. Move to first place.
            iMmcList.Remove( i );
            iMmcList.InsertL( aMmc, 0 );
            break;
            }
        }
    }

// ---------------------------------------------------------
// CMenuSrvMmcHistory::Find
// ---------------------------------------------------------
//
TInt CMenuSrvMmcHistory::Find( TUint aMmc )
    {
    return iMmcList.Find( aMmc );
    }

//  End of File  
