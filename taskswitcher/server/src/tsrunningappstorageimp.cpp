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
 * Description :
 *
 */
#include "tsutils.h"
#include "tsentrykey.h"
#include "tsrunningappstorageimp.h"

//------------------------------------------------------------------------------
CTsRunningAppStorage* CTsRunningAppStorage::NewLC()
    {
    CTsRunningAppStorage* self = new(ELeave) CTsRunningAppStorage();
    CleanupStack::PushL( self );
    self->iRunningApps = new (ELeave)CArrayPtrFlat<CTsRunningApp>(1);
    return self;
    }

//------------------------------------------------------------------------------
CTsRunningAppStorage* CTsRunningAppStorage::NewL()
    {
    CTsRunningAppStorage* self( CTsRunningAppStorage::NewLC() );
    CleanupStack::Pop( self );
    return self;
    }
//------------------------------------------------------------------------------
CTsRunningAppStorage::CTsRunningAppStorage()
    {
    //No implementation required
    }

//------------------------------------------------------------------------------
CTsRunningAppStorage::~CTsRunningAppStorage()
    {
    if( 0 != iRunningApps )
        {
        iRunningApps->ResetAndDestroy();
        }
    delete iRunningApps;
    iBlockedWindowGroups.Close();
    }

//------------------------------------------------------------------------------
void CTsRunningAppStorage::HandleWindowGroupChanged( 
               MTsResourceManager &aResources, 
               const TArray<RWsSession::TWindowGroupChainInfo> & aWindowGroups )
    {
    CTsRunningApp* app(0);
    iBlockedWindowGroups.Reset();
    TRAP_IGNORE(
    CArrayPtr<CTsRunningApp> *runningApps(new (ELeave)CArrayPtrFlat<CTsRunningApp>(aWindowGroups.Count()));
    TaskSwitcher::CleanupResetAndDestroyPushL(runningApps);
    for(TInt current(0); current < aWindowGroups.Count(); ++current)
        {
        app = 0;
        for(TInt old(0); 0 == app && old < iRunningApps->Count(); ++old)
            {
            if(iRunningApps->At(old)->WindowGroupId() == aWindowGroups[current].iId)
                {
                app = iRunningApps->At(old);
                iRunningApps->Delete(old);
                CleanupStack::PushL(app);
                app->RefreshDataL();
                }
            }
        if(0 == app)
            {
            app = CTsRunningApp::NewLC(aResources, aWindowGroups[current]);
            }
        runningApps->InsertL(current, app);
        CleanupStack::Pop(app);
        }
    iRunningApps->ResetAndDestroy();
    delete iRunningApps;
    iRunningApps = runningApps;
    CleanupStack::Pop(runningApps);
    )//TRAP_IGNORE
    }

//------------------------------------------------------------------------------
void CTsRunningAppStorage::HandleWindowGroupChanged( 
           MTsResourceManager &aResources, 
           const TArray<RWsSession::TWindowGroupChainInfo>& aFull,
           const TArray<RWsSession::TWindowGroupChainInfo>& aFiltered)
    {
    TInt filtered(0);
    HandleWindowGroupChanged(aResources, aFiltered);
    for(TInt full(0); full < aFull.Count(); ++full)
        {
        for(filtered = 0; filtered < aFiltered.Count(); ++filtered)
            {
            if(aFull[full].iId == aFiltered[filtered].iId)
                {
                break;
                }
            }
        if(aFiltered.Count() == filtered)
            {
            iBlockedWindowGroups.Append(aFull[full].iId);
            }
        }
    }
//------------------------------------------------------------------------------
MTsRunningApplication& CTsRunningAppStorage::operator[] (TInt aOffset) const
    {
    return *(*iRunningApps)[aOffset];
    }

//------------------------------------------------------------------------------
TInt CTsRunningAppStorage::Count() const
    {
    return iRunningApps->Count();
    }

//------------------------------------------------------------------------------
TInt CTsRunningAppStorage::ParentIndex( const MTsRunningApplication& aRunningApp ) const
    {
    const TInt applicationIndex(Find(aRunningApp.WindowGroupId()));
    return KErrNotFound == applicationIndex ? 
           applicationIndex : ParentIndex(applicationIndex);
    }

//------------------------------------------------------------------------------
TArray<TInt> CTsRunningAppStorage::BlockedWindowGroups() const
    {
    return iBlockedWindowGroups.Array();
    }

//------------------------------------------------------------------------------
TInt CTsRunningAppStorage::GenerateKey( TTsEntryKey& aReturnKey, 
                                        TInt aWindowGroupId) const
    {
    return GenerateKey(aReturnKey, aWindowGroupId, 0);
    }

//------------------------------------------------------------------------------
TInt CTsRunningAppStorage::GenerateKey( TTsEntryKey& aReturnKey, 
                                        TInt aWindowGroupId, 
                                        TInt aOffset) const
    {
    TInt retVal(Find(aWindowGroupId, aOffset));
    if( KErrNotFound != retVal )
        {
        CTsRunningApp &app(*iRunningApps->At(retVal));
        if(app.WindowGroupId() == app.ParentWindowGroupId())
            {
            retVal = KErrBadHandle;
            }
        else if( app.IsEmbeded() )
            {
            retVal = GenerateKey(aReturnKey, app.ParentWindowGroupId(), retVal);
            }
        else
            {
            aReturnKey = app.Key();
            retVal = KErrNone;
            }
        }
    return retVal;
    }

//------------------------------------------------------------------------------
TInt CTsRunningAppStorage::ParentIndex( TInt aOffset ) const
    {
    TInt retval(aOffset);
    
    if((*iRunningApps)[aOffset]->IsEmbeded())
        {
        const TInt parentIndex( Find((*iRunningApps)[aOffset]->ParentWindowGroupId(), 
                                aOffset + 1) );
        if( KErrNotFound != parentIndex )
            {
            retval = ParentIndex( parentIndex );
            }
        }
    return retval;
    }

//------------------------------------------------------------------------------
TInt CTsRunningAppStorage::Find(TInt aWindowGroupId, TInt aOffset) const
    {
    TInt retVal(KErrNotFound);
    for( TInt iter(aOffset); 
         KErrNotFound == retVal && iter < iRunningApps->Count(); 
         ++iter )
        {
        if( (*iRunningApps)[iter]->WindowGroupId() == aWindowGroupId )
            {
            retVal = iter;
            }
        }
    return retVal;
    }


