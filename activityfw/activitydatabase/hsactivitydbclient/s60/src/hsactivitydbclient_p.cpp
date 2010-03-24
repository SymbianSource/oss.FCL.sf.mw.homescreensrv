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

#include "hsactivitydbclient_p.h"
#include "hsactivitydbasyncrequest_p.h"
#include "hsactivityglobals.h"
#include "hsserializer.h"

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
//
HsActivityDbClientPrivate::HsActivityDbClientPrivate(HsActivityDbAsyncRequestObserver& observer)
{
    mAsyncDataHandler = HsActivityDbAsyncRequestPrivate::NewL(observer, *this);
}

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
//
HsActivityDbClientPrivate::~HsActivityDbClientPrivate()
{
    delete mAsyncDataHandler;
    Close();
}

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
//
int HsActivityDbClientPrivate::connect()
{
    TRAPD(errNo, connectL());
    return errNo;
}

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
//
int HsActivityDbClientPrivate::addActivity(const QVariantHash &activity)
{
    TRAPD( errNo, execSimpleRequestL(AddActivity, activity);)
    return errNo; 
}

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
//
int HsActivityDbClientPrivate::updateActivity(const QVariantHash &activity)
{
    TRAPD( errNo, execSimpleRequestL(UpdateActivity, activity);)
    return errNo; 
}

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
//
int HsActivityDbClientPrivate::removeActivity(const QVariantHash &activity)
{
    TRAPD( errNo, execSimpleRequestL(RemoveActivity, activity);)
    return errNo; 
}

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
//
int HsActivityDbClientPrivate::removeApplicationActivities(const QVariantHash &activity)
{
    TRAPD( errNo, execSimpleRequestL(RemoveApplicationActivities, activity);)
    return errNo; 
}

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
//
int HsActivityDbClientPrivate::activities(QList<QVariantHash>& result)
{
    TRAPD(errNo, activitiesL(result));
    return errNo;
}

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
//
int HsActivityDbClientPrivate::applicationActivities(QList<QVariantHash>& result, 
                                                      const QVariantHash & condition)
{
    TRAPD(errNo, applicationActivitiesL(result, condition));
    return errNo;
}


// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
//
int HsActivityDbClientPrivate::waitActivity(const QVariantHash &activity)
{
    TRAPD( errNo, waitActivityL(activity);)
    return errNo;
}

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
//
int HsActivityDbClientPrivate::launchActivity(const QVariantHash &activity)
{
    TRAPD( errNo, execSimpleRequestL(LaunchActivity, activity);)
    return errNo;
}

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
//
int HsActivityDbClientPrivate::cancelWaitActivity()
{
    return SendReceive(CancelWait, TIpcArgs());
}

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
//
void HsActivityDbClientPrivate::startServerL()
{
    RProcess server;
    const TUidType uid(KNullUid, KNullUid, KActivityServerUid);
    User::LeaveIfError(server.Create( KActivityServerName, KNullDesC, uid));
    TRequestStatus stat;
    server.Rendezvous( stat );
    if ( stat != KRequestPending ) {
        server.Kill(0);
    } else {
        server.Resume();
    }
    User::WaitForRequest(stat);
    int errNo = (EExitPanic == server.ExitType()) ? 
                KErrGeneral : stat.Int();
    if(KErrCancel == errNo) {
        errNo = KErrNone;
    }
    server.Close();
    User::LeaveIfError(errNo);
}

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
//
void HsActivityDbClientPrivate::connectL()
{
    const int asyncMessageSlots(4);
    const int maxRetry(4);
    
    TInt retry = maxRetry;
    TInt errNo(KErrNone);
    do {
        errNo = CreateSession( KActivityServerName, TVersion( 0, 0, 0 ), asyncMessageSlots );
        if( KErrNotFound != errNo && KErrServerTerminated != errNo) {
            retry =0;
        } else {
            TRAP(errNo, startServerL());
            if (KErrNone != errNo && KErrAlreadyExists != errNo) {
                retry =0;
            }
        }
    } while(--retry > 0);
    User::LeaveIfError(errNo);
}

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
//
void HsActivityDbClientPrivate::execSimpleRequestL(int function, const QVariantHash &activity)
{
    RBuf8 data;
    CleanupClosePushL(data);
    data.CreateL(256);
    data << activity;
    User::LeaveIfError(SendReceive(function, TIpcArgs(&data)));
    CleanupStack::PopAndDestroy(&data);
}

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
//
void HsActivityDbClientPrivate::requestedActivityNameL(QString& result, 
                                                const QVariantHash &activity)
{
    RBuf8 data;
    TPckgBuf<int> sizeBuf;
    CleanupClosePushL(data);
    data.CreateL(256);
    data << activity;
    User::LeaveIfError(SendReceive(RequestedActivityName, TIpcArgs(&data,&sizeBuf)));
    if(sizeBuf() > data.MaxSize()) {
       data.ReAlloc(sizeBuf()); 
    }
    User::LeaveIfError(SendReceive(GetData, TIpcArgs(&data)));
    result = QString::fromAscii(reinterpret_cast<const char*>(data.Ptr()), 
                                data.Length());
    CleanupStack::PopAndDestroy(&data);
}

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
//
void HsActivityDbClientPrivate::activitiesL(QList<QVariantHash>& result)
{
    RBuf8 data;
    TPckgBuf<int> sizeBuf;
    CleanupClosePushL(data);
    data.CreateL(256);
    User::LeaveIfError(SendReceive(Activities, TIpcArgs(&data, &sizeBuf)));
    if(sizeBuf() > data.MaxSize()) {
       data.ReAlloc(sizeBuf()); 
    }
    User::LeaveIfError(SendReceive(GetData, TIpcArgs(&data)));
    result << data;
    CleanupStack::PopAndDestroy(&data);
}

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
//
void HsActivityDbClientPrivate::applicationActivitiesL(QList<QVariantHash>& result, 
                                                     const QVariantHash & condition)
{
    RBuf8 data;
    TPckgBuf<int> sizeBuf;
    CleanupClosePushL(data);
    data.CreateL(256);
    data << condition;
    User::LeaveIfError(SendReceive(ApplicationActivities, TIpcArgs(&data, &sizeBuf)));
    if(sizeBuf() > data.MaxSize()) {
       data.ReAlloc(sizeBuf()); 
    }
    User::LeaveIfError(SendReceive(GetData, TIpcArgs(&data)));
    result << data;
    CleanupStack::PopAndDestroy(&data);
}

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
//
void HsActivityDbClientPrivate::waitActivityL(const QVariantHash &activity)
{
    if(mAsyncDataHandler->IsActive()) {
        User::Leave(KErrServerBusy);
    } else {
        mAsyncDataHandler->waitActivity(activity);
    }
}

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
//
void HsActivityDbClientPrivate::getData(RBuf8& data)
{
    SendReceive(GetData, TIpcArgs(&data));
}

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
//
void HsActivityDbClientPrivate::sendDataAsync(int func,
                                              const TIpcArgs &data, 
                                              TRequestStatus& status)
{
    SendReceive(func, data, status);
}
