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
#include "activityserver.h"
#include "activitysession.h"
#include "activitystorage.h"
#include "activitytask.h"

#include <fbs.h>

_LIT( KActivityServerName, "hsactivitydbserver" );
_LIT(KObserverAlreadyExists, "Observer task exists");

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
//
CActivityServer::CActivityServer()
:
CServer2( EPriorityStandard )
{
    // No implementation required
}

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
//
CActivityServer::~CActivityServer()
{
    delete mStorage;
    mFsSession.Close();
    mObservers.ResetAndDestroy();
    RFbsSession::Disconnect();
}

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
//
CActivityServer* CActivityServer::NewLC()
{
    CActivityServer* self = new (ELeave) CActivityServer();
    CleanupStack::PushL(self);
    self->ConstructL();
    return self;
}

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
//
void CActivityServer::ConstructL()
{
    StartL(KActivityServerName);
    User::LeaveIfError(mFsSession.Connect());
    User::LeaveIfError(RFbsSession::Connect(mFsSession));
    mStorage = CActivityStorage::NewL(mFsSession);
    mObservers.Array();
}

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
//
CSession2* CActivityServer::NewSessionL(const TVersion &, const RMessage2&) const
{
    return CActivitySession::NewL(const_cast<CActivityServer*>(this)->mFsSession, 
                                  *const_cast<CActivityServer*>(this), 
                                  *mStorage);
}

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
//
void CActivityServer::PushL(CActivityTask * task)
{
    (KErrNotFound == mObservers.Find(task)) ?
     mObservers.AppendL(task) :
     User::Panic(KObserverAlreadyExists, KErrAlreadyExists);
}

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
//
void CActivityServer::Pop(CActivityTask *task)
{
    const TInt offset(mObservers.Find(task));
    if (KErrNotFound != offset) {
        mObservers.Remove(offset);
    }
}

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
//
const RPointerArray<CActivityTask>& CActivityServer::StorageData() const
{
    return mObservers;
}

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
//
void CActivityServer::RemoveNotValidTasks(const CSession2* session)
{
	for (TInt i=mObservers.Count()-1; i>=0; --i) {
		if( mObservers[i]->IsSessionTask(session) ) {
			delete mObservers[i];
			mObservers.Remove(i);
		}
	}
}
