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

#ifndef ACTIVITYSESSIONPRIVATE_H
#define ACTIVITYSESSIONPRIVATE_H
#include <e32base.h>
#include "hsactivitydbclientinterface.h"

/**
 * Class impolements client session for S60 enviroment.
 * Class is responsible for request handling and data translations
 */
class HsActivitySessionPrivate: public CSession2
{
public:
    /**
     * Constructor
     * @param storage - reference to initialized activity storage
     */
    HsActivitySessionPrivate(HsActivityDbClientInterface &storage);

    /**
     * Destructor
     */
    ~HsActivitySessionPrivate();

    /**
     * Implements interface
     * @see void CSession2::ServiceL(const RMessage2&)
     */
    void ServiceL(const RMessage2 &message);

    /**
     * Implements interface
     * @see void CSession2::ServiceError(const RMessage2 &, TInt)
     */
    void ServiceError(const RMessage2 &message, TInt error);

    void LaunchActivityL(const RMessage2 &message, const QString &name);

    void CancelNotify(RMessage2 &message);
private:
    /**
     * Function handle data translation and forward request to storage client
     * Function can leave on failure.
     * @param message - request message
     */
    void HandleAddActivityL(const RMessage2 &message);

    /**
     * Function handle data translation and forward request to storage client
     * Function can leave on failure.
     * @param message - request message
     */
    void HandleUpdateActivityL(const RMessage2 &message);

    /**
     * Function handle data translation and forward request to storage client
     * Function can leave on failure.
     * @param message - request message
     */
    void HandleRemoveActivityL(const RMessage2 &message);

    /**
     * Function handle data translation and forward request to storage client
     * Function can leave on failure.
     * @param message - request message
     */
    void HandleRemoveApplicationActivitiesL(const RMessage2 &message);

    /**
     * Function handle data translation and forward request to storage client
     * Function can leave on failure.
     * @param message - request message
     */
    void HandleActivitiesL(const RMessage2 &message);

    /**
     * Function handle data translation and forward request to storage client
     * Function can leave on failure.
     * @param message - request message
     */
    void HandleApplicationActivitiesL(const RMessage2 &message);

    /**
     * Function handle data translation and forward request to server
     * Function can leave on failure.
     * @param message - request message
     */
    void HandleLaunchActivityL(const RMessage2 &message);

    /**
     * Function handle data translation and forward request to server
     * Function can leave on failure.
     * @param message - request message
     */
    void HandleWaitActivityL(const RMessage2 &message);

    /**
     * Function cancels subscribtion
     * @param message - request message
     */
    void HandleCancelWaitActivity(const RMessage2 &message);
    
    /**
     * Function read thumbail
     * @param message - request message
     */
    void HandleGetThumbnailL(const RMessage2 &message);

    /**
     * Function handle data translation and forward request to storage client
     * Function can leave on failure.
     * @param message - request message
     */
    void HandleGetDataL(const RMessage2 &message);

    void LaunchActivityL();

    void WaitActivityL(const RMessage2 &message);
    
    void GetThumbnailL(const RMessage2 &message);

    void CancelNotify();


    /**
     * Function deserialize data for request message
     * Function can leave on failure.
     */
    void ReadDataL(const RMessage2 &src);

    /**
     * Function handle data translation and forward request to storage client
     * Function can leave on failure.
     * @param message - request message
     */
    void LeaveIfNotZero(int errNo);
private:
    /**
     * Reference to storage client
     */
    HsActivityDbClientInterface &mStorage;
    RBuf8 mData;
    QVariantHash mActivity;
    QList<QVariantHash> mResult;
    QString mName;
    int mAppId;
};
#endif // ACTIVITYSESSIONPRIVATE_H
