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

#include <fbs.h>
#include <XQConversions>

#include "afthumbnailrequest_p.h"
#include "afstorageclient_p.h"
#include "afstorageclient.h"
#include "afentry.h"

// -----------------------------------------------------------------------------
/**
 * Create and initialize handler for activity thumbnail request
 * @param observer - request completion observer
 * @param session - activity client implementation
 * @param source - thumbnail source location
 * @param userData - rsponse user data
 */
void CAfThumbnailRequestPrivate::NewLD(MAfAsyncRequestObserver &observer, 
                                       RAfStorageClientImplementation & session,
                                       TSize resolution, 
                                       const TDesC& source,
                                       TAny* userData)
{
    CAfThumbnailRequestPrivate* self = new(ELeave)CAfThumbnailRequestPrivate(observer, session, userData);
    CleanupStack::PushL(self);
    self->ConstructL(resolution, source);
    session.PushL(self);
    CleanupStack::Pop(self);
}

// -----------------------------------------------------------------------------
/**
 * Constructor
 * @param observer - request completion observer
 * @param session - activity client implementation
 * @param userData - rsponse user data
 */
CAfThumbnailRequestPrivate::CAfThumbnailRequestPrivate(MAfAsyncRequestObserver & observer, 
                                                       RAfStorageClientImplementation & session,
                                                       TAny* userData)
:
CActive(EPriorityStandard),
mObserver(observer),
mSession(session),
mUserData(userData)
{
    CActiveScheduler::Add(this);
}

// -----------------------------------------------------------------------------
/**
 * Second phase constructor
 * @param resolution - requested thumbnail resolution
 * @param source - thumbnail source location
 */
void CAfThumbnailRequestPrivate::ConstructL(TSize resolution, const TDesC& source)
{
    mIds[0] = resolution.iWidth;
    mIds[1] = resolution.iHeight;
    CAfEntry::CopyL(mImgSrc, source);
    mSession.sendAsync(GetThumbnail, 
                       TIpcArgs(&mIds[0],&mIds[1], &mImgSrc),
                       iStatus);
    SetActive();
}

// -----------------------------------------------------------------------------
/**
 * Destuctor
 */
CAfThumbnailRequestPrivate::~CAfThumbnailRequestPrivate()
{
    Cancel();
    mImgSrc.Close();
}

// -----------------------------------------------------------------------------
/**
 * Cancel pending requst
 */
void CAfThumbnailRequestPrivate::DoCancel()
{
}

// -----------------------------------------------------------------------------
/**
 * Function handle request completion, copy data, forward information and destroy handler 
 */
void CAfThumbnailRequestPrivate::RunL()
{
    User::LeaveIfError(iStatus.Int());
    CFbsBitmap* bitmap = new (ELeave) CFbsBitmap();
    CleanupStack::PushL(bitmap);
    User::LeaveIfError(bitmap->Duplicate(mIds[0]()));
    mObserver.asyncRequestCompleated(iStatus.Int(), 
                                     GetThumbnail, 
                                     QPixmap::fromSymbianCFbsBitmap(bitmap), 
                                     mUserData);
    CleanupStack::PopAndDestroy(bitmap);
    mSession.Pop(this);
    delete this;
}

// -----------------------------------------------------------------------------
/**
 * Function handle request processing errors
 * @param error - error code 
 */
TInt CAfThumbnailRequestPrivate::RunError(TInt error)
{
    mObserver.asyncRequestCompleated(error, GetThumbnail, QPixmap(), mUserData);
    mSession.Pop(this);
    delete this;
    return KErrNone;
}

