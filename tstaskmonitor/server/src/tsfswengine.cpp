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
* Description:  Task monitor engine
 *
*/


#include "tsfswengine.h"
#include "tsfswdatalist.h"


// --------------------------------------------------------------------------
// CTsFswEngine::NewL
// --------------------------------------------------------------------------
//
CTsFswEngine* CTsFswEngine::NewL(MHsDataObserver& observer)
{
    CTsFswEngine* self = NewLC(observer);
    CleanupStack::Pop(self);
    return self;
}

// --------------------------------------------------------------------------
// CTsFswEngine::NewLC
// --------------------------------------------------------------------------
//
CTsFswEngine* CTsFswEngine::NewLC(MHsDataObserver& observer)
{
    CTsFswEngine* self = new (ELeave) CTsFswEngine(observer);
    CleanupStack::PushL(self);
    self->ConstructL();
    return self;
}

// --------------------------------------------------------------------------
// CTsFswEngine::CTsFswEngine
// --------------------------------------------------------------------------
//
CTsFswEngine::CTsFswEngine(MHsDataObserver& observer) :
    mObserver(observer)
{
}

// --------------------------------------------------------------------------
// CTsFswEngine::ConstructL
// --------------------------------------------------------------------------
//
void CTsFswEngine::ConstructL()
{
    mDataList = CTsFswDataList::NewL(mObserver);
    CollectTasksL();
}

// --------------------------------------------------------------------------
// CTsFswEngine::~CTsFswEngine
// --------------------------------------------------------------------------
//
CTsFswEngine::~CTsFswEngine()
{
    delete mDataList;
}

// --------------------------------------------------------------------------
// CTsFswEngine::FswDataL
// --------------------------------------------------------------------------
//
const RTsFswArray& CTsFswEngine::FswDataL()
{
    return mDataList->FswDataL();
}

// --------------------------------------------------------------------------
// CTsFswEngine::UpdateTaskList
// Callback from appui
// --------------------------------------------------------------------------
//
void CTsFswEngine::DataChanged()
{
    TBool changed = EFalse;
    TRAPD( err, changed = CollectTasksL() );
    if (err == KErrNone && changed) {
        mObserver.DataChanged();
    }
}

// --------------------------------------------------------------------------
// CTsFswEngine::CollectTasksL
// --------------------------------------------------------------------------
//
TBool CTsFswEngine::CollectTasksL()
{
    TBool changed = mDataList->CollectTasksL();
    return changed;
}

// --------------------------------------------------------------------------
// CTsFswEngine::SetScreenshotL
// --------------------------------------------------------------------------
//
void CTsFswEngine::SetScreenshotL(const CFbsBitmap* bitmap, UpdatePriority priority, TInt wgId)
{
    TBool changed = mDataList->SetScreenshotL(bitmap, priority, wgId);
    if (!changed) {
        User::Leave(KErrAccessDenied);
    }
}

// --------------------------------------------------------------------------
// CTsFswEngine::SetScreenshotL
// --------------------------------------------------------------------------
//
void CTsFswEngine::RemoveScreenshotL(TInt wgId)
{
    TBool changed = mDataList->RemoveScreenshotL(wgId);
    if (changed) {
        mObserver.DataChanged();
    }
}
// end of file
