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
* Description: Application entry point
*
*/
#include "hsactivityserver_p.h"
#include "hsactivitysession_p.h"
#include "hsactivityglobals.h"

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
//
HsActivityServerPrivate::HsActivityServerPrivate(HsActivityDbClientInterface& storage)
:
    CServer2( EPriorityNormal, ESharableSessions),
    mStorage(storage)
{
}

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
//
HsActivityServerPrivate::~HsActivityServerPrivate()
{
}

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
//
bool HsActivityServerPrivate::start()
{
    return ( KErrNone == Start(KActivityServerName) );
}

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
//
void HsActivityServerPrivate::notifyL(int applicationId, 
                                     const QString& activityName)
{
    if(mPendingMessage.end() != mPendingMessage.find(applicationId)) {
        static_cast<HsActivitySessionPrivate*>(mPendingMessage.find(applicationId).value().Session())
            ->LaunchActivityL(mPendingMessage.find(applicationId).value(), activityName );
        mPendingMessage.erase(mPendingMessage.find(applicationId));
    }
}

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
//
void HsActivityServerPrivate::waitNotification(int applicationId, 
                                               const RMessage2& msg)
{
    if(mPendingMessage.find(applicationId) != mPendingMessage.end()) {
        msg.Kill(EBadMessageNumber);
    }
    mPendingMessage.insert(applicationId,msg);
}
// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
//
void HsActivityServerPrivate::cancelNotify(int applicationId)
	{
	if(mPendingMessage.end() != mPendingMessage.find(applicationId)) 
		{
		static_cast<HsActivitySessionPrivate*>(mPendingMessage.find(applicationId).value().Session())
			->CancelNotify(mPendingMessage.find(applicationId).value());
	    mPendingMessage.erase(mPendingMessage.find(applicationId));
		}
	}

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
//
CSession2* HsActivityServerPrivate::NewSessionL(const TVersion&,
                                               const RMessage2&)const
{
    return new (ELeave) HsActivitySessionPrivate(mStorage);
}
