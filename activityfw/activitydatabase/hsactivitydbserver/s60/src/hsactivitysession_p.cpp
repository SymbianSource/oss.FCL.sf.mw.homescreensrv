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
#include "hsserializer.h"
#include <qglobal.h>

const char ActivityPrimaryKeyFormat [] = "%1:%2";
// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
//
HsActivitySessionPrivate::HsActivitySessionPrivate(HsActivityDbClientInterface& storage)
: mStorage(storage), mAppId(KErrNotFound)
{
}

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
//
HsActivitySessionPrivate::~HsActivitySessionPrivate()
{
	 CancelNotify();
     mData.Close();
}

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
//
void HsActivitySessionPrivate::ServiceL(const RMessage2& message)
{
    mActivity.clear();
    mResult.clear();
    mName.clear();
    
    int errNo(KErrNone);
    switch(message.Function()) {
    case AddActivity: HandleAddActivityL(message); break;
    case UpdateActivity: HandleUpdateActivityL(message); break;
    case RemoveActivity: HandleRemoveActivityL(message); break;
    case RemoveApplicationActivities: HandleRemoveApplicationActivitiesL(message); break;
    case Activities: HandleActivitiesL(message);break;
    case ApplicationActivities: HandleApplicationActivitiesL(message);break;
    case LaunchActivity: HandleLaunchActivityL(message);break;
    case WaitActivity: HandleWaitActivityL(message);return; //!!!!! This message shouldn't be compleated !!!!!
    case GetData: HandleGetDataL(message); break;
    case CancelWait: HandleCancelWaitActivity(message); break;
    default:
        message.Panic(KErr400, CServer2::EBadMessageNumber);
    }
    message.Complete(KErrNone);
}

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
//
void HsActivitySessionPrivate::ServiceError(const RMessage2 &message, TInt error)
{
    message.Complete(error);
}

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
//
void HsActivitySessionPrivate::HandleAddActivityL(const RMessage2& message)
{
    ReadDataL(message);
    LeaveIfNotZero(mStorage.addActivity(mActivity));
}

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
//
void HsActivitySessionPrivate::HandleUpdateActivityL(const RMessage2& message)
{
    ReadDataL(message);
    LeaveIfNotZero(mStorage.updateActivity(mActivity));
}

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
//
void HsActivitySessionPrivate::HandleRemoveActivityL(const RMessage2& message)
{
    ReadDataL(message);
    LeaveIfNotZero(mStorage.removeActivity(mActivity));
}

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
//
void HsActivitySessionPrivate::HandleRemoveApplicationActivitiesL(const RMessage2& message)
{
    ReadDataL(message);
    LeaveIfNotZero(mStorage.removeApplicationActivities(mActivity));
}

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
//
void HsActivitySessionPrivate::HandleActivitiesL(const RMessage2& message)
{
    int errNo(KErrNone);
    QT_TRYCATCH_LEAVING(errNo = mStorage.activities(mResult));
    LeaveIfNotZero(errNo);
    mData << mResult;
    TPckgC<int> dataSize(mData.Size());
    message.WriteL(1, dataSize);
}

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
//
void HsActivitySessionPrivate::HandleApplicationActivitiesL(const RMessage2& message)
{
    int errNo(KErrNone);
    ReadDataL(message);
    QT_TRYCATCH_LEAVING(errNo = mStorage.applicationActivities(mResult, mActivity));
    LeaveIfNotZero(errNo);
    mData << mResult;
    TPckgC<int> dataSize(mData.Size());
    message.WriteL(1, dataSize);
}

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
//
void HsActivitySessionPrivate::HandleLaunchActivityL(const RMessage2& message)
{
    ReadDataL(message);
    LaunchActivityL();
}

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
//
void HsActivitySessionPrivate::HandleWaitActivityL(const RMessage2& message)
{
    ReadDataL(message);
    WaitActivityL(message);
}

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
//
void HsActivitySessionPrivate::HandleCancelWaitActivity(const RMessage2& /*message*/)
	{
	CancelNotify();
	}

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
//
void HsActivitySessionPrivate::HandleGetDataL(const RMessage2& message)
{
    message.WriteL(0, mData);
}

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
//
void HsActivitySessionPrivate::LaunchActivityL()
{
    CServer2* const server(const_cast<CServer2*>(Server()));
    if(mActivity.end() == mActivity.find(ActivityApplicationKeyword) || 
       mActivity.end() == mActivity.find(ActivityActivityKeyword)) {
        User::Leave(KErrCorrupt);
    }
    static_cast<HsActivityServerPrivate*>
        (server)->notifyL(mActivity.find(ActivityApplicationKeyword).value().toInt(), 
                         mActivity.find(ActivityActivityKeyword).value().toString());
}

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
//
void HsActivitySessionPrivate::LaunchActivityL(const RMessage2 &message, 
                                               const QString &name )
{
    if (mData.MaxSize() < name.toAscii().length()){
        mData.ReAlloc( name.toAscii().length());
    }
    mData.Copy(reinterpret_cast<const TUint8*>(name.toAscii().data()),
                                               name.toAscii().length());
    TPckgC<int> dataSize(mData.Size());
    message.WriteL(1, dataSize);
    message.Complete(KErrNone);
}
// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
//
void HsActivitySessionPrivate::WaitActivityL(const RMessage2& msg)
{
    CServer2* const server(const_cast<CServer2*>(Server()));
    if(mActivity.end() == mActivity.find(ActivityApplicationKeyword)) {
        User::Leave(KErrCorrupt);
    }
    mAppId = mActivity.find(ActivityApplicationKeyword).value().toInt();
    static_cast<HsActivityServerPrivate*>
        (server)->waitNotification(mAppId, msg);
}

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
//
void HsActivitySessionPrivate::CancelNotify()
	{
	if ( mAppId != KErrNotFound )
	    {
	    CServer2* const server(const_cast<CServer2*>(Server()));
	    static_cast<HsActivityServerPrivate*>(server)->cancelNotify(mAppId);
	    mAppId = KErrNotFound;
	    }
	}

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
//
void HsActivitySessionPrivate::CancelNotify(RMessage2& message)
	{
	message.Complete(KErrCancel);
	}

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
//
void HsActivitySessionPrivate::ReadDataL(const RMessage2& src)
{
    int srcLength = src.GetDesLength(0);
    User::LeaveIfError(srcLength);
    RBuf8 buffer;
    CleanupClosePushL(buffer);
    buffer.CreateL(srcLength);
    src.ReadL( 0, buffer, 0);
    if (buffer.Length() != srcLength) {
        User::Leave(KErrCorrupt);
    }
    mActivity << buffer;
    CleanupStack::PopAndDestroy(&buffer);
}

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
//
void HsActivitySessionPrivate::LeaveIfNotZero(int errNo)
{
    if (KErrNone != errNo) {
        User::LeaveIfError(errNo);
        User::Leave(KErrGeneral);
    }
}
