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
* Description:
*
*/


#include "mcsdrmobservermanager.h"
#include "mcsdrmobserver.h"
#include "menusrvengutils.h"
#include <apaid.h>
#include <DRMEvent.h>


// ---------------------------------------------------------------------------
// destructor
// ---------------------------------------------------------------------------
RMcsDrmObserverManager::~RMcsDrmObserverManager()
	{
	}

// ---------------------------------------------------------------------------
// destructor
// ---------------------------------------------------------------------------
void RMcsDrmObserverManager::Close()
	{
	iObservers.ResetAndDestroy();
	}


// ---------------------------------------------------------------------------
// default constructor
// ---------------------------------------------------------------------------
RMcsDrmObserverManager::RMcsDrmObserverManager( CMenuSrvEngUtils& aUtils ):
    iUtils( aUtils )
	{
	}

// ---------------------------------------------------------------------------
//
// ---------------------------------------------------------------------------
void RMcsDrmObserverManager::AddL( CMcsDrmObserver* aObserver )
    {
    User::LeaveIfError( iObservers.Append( aObserver ) );
    }

// ---------------------------------------------------------------------------
//
// ---------------------------------------------------------------------------
void RMcsDrmObserverManager::Remove( CMcsDrmObserver* aObserver )
    {
    TInt ptrIndex( iObservers.Find( aObserver ) );
    if ( KErrNotFound != ptrIndex )
        {
        delete aObserver;
        iObservers.Remove( ptrIndex );
        }
    }

// ---------------------------------------------------------------------------
//
// ---------------------------------------------------------------------------
void RMcsDrmObserverManager::RemoveMissingObservers(
	const RArray<TUid>& aDrmProtectedArray )
    {
    TInt index = 0;
    while (index < iObservers.Count() )
        {
        if ( aDrmProtectedArray.Find( iObservers[index]->Uid() ) == KErrNotFound	)
            {
            delete iObservers[index];
            iObservers.Remove( index );
            }
        else
            {
            index++;
            }
        }
    }

// ---------------------------------------------------------------------------
//
// ---------------------------------------------------------------------------
TInt RMcsDrmObserverManager::FindUid( TUid aUid )
    {
    TInt ret( KErrNotFound );
    for (TInt i = 0; i < iObservers.Count(); i++)
        {
        if (iObservers[i]->Uid() == aUid)
            {
            ret = i;
            break;
            }
        }
    return ret;
    }

// ---------------------------------------------------------------------------
//
// ---------------------------------------------------------------------------
void RMcsDrmObserverManager::CreateObserversL(
        const RArray<TUid>& aDrmArray,
        TDRMEventType aMask,
        MMcsDrmObserver& aObserver )
    {
    for (TInt i = 0; i < aDrmArray.Count(); i++)
        {
        if (FindUid(aDrmArray[i]) == KErrNotFound)
            {
            CMcsDrmObserver* observer= NULL;
            if ( iUtils.IsMiddlet( aDrmArray[i] ) )
                {
                TBuf<KMaxFileName> contentId;
                iUtils.GetJavaContentIdL( aDrmArray[i], contentId );

                observer = CMcsDrmObserver::NewL(aObserver, contentId,
                                    aDrmArray[i], aMask);

                TInt err = iObservers.Append(observer);
                if( err != KErrNone)
                    {
                    delete observer;
                    User::Leave( err );
                    }
                }
            }
        }
    }

// ---------------------------------------------------------------------------
//
// ---------------------------------------------------------------------------
void RMcsDrmObserverManager::CreateObserversL(
	const RArray<TUid>& aDrmProtectedArray,
    MMcsDrmObserver& aObserver )
    {
    CreateObserversL(aDrmProtectedArray, KEventModify,
			aObserver);
	}

// ---------------------------------------------------------------------------
//
// ---------------------------------------------------------------------------
void RMcsDrmObserverManager::RefreshObserverL(
		const RArray<TUid>& aDrmProtectedArray,
		TUid aUid,
		MMcsDrmObserver& aObserver )
    {
    TInt index = FindUid( aUid );
    if ( index != KErrNotFound )
    	{
    	delete iObservers[index];
    	iObservers.Remove( index );
        CreateObserversL(aDrmProtectedArray, KEventModify,
    			aObserver);
    	}

	}
