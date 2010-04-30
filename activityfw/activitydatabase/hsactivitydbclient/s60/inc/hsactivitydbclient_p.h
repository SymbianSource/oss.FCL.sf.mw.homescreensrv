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

#ifndef HSACTIVITYDBCLIENTPRIVATE_H
#define HSACTIVITYDBCLIENTPRIVATE_H
#include "hsactivitydbclientinterface.h"
#include <e32base.h>
#include <hsactivitydbasyncrequestobserver.h>

class HsActivityDbAsyncRequestPrivate;
/**
 * Class implemets HsActivityDbClientInterface and is responsible for
 * activity data management on S60 enviroment.
 */
class HsActivityDbClientPrivate : protected RSessionBase,
                                  public HsActivityDbClientInterface
{
public:
    /**
     * Constructor
     */
    HsActivityDbClientPrivate(HsActivityDbAsyncRequestObserver &);

    /**
     * Destructor
     */
    ~HsActivityDbClientPrivate();

    /**
     * Function establish connection to activity server
     * @return 0 on succees, error code otherwise
     */
    int connect();

    /**
     * Interface implementation.
     * @see int HsActivityDbClientInterface::addActivity(const QVariantHash &)
     */
    int addActivity(const QVariantHash &);

    /**
     * Interface implementation.
     * @see int HsActivityDbClientInterface::updateActivity(const QVariantHash &)
     */
    int updateActivity(const QVariantHash &);

    /**
     * Interface implementation.
     * @see int HsActivityDbClientInterface::removeActivity(const QVariantHash &)
     */
    int removeActivity(const QVariantHash &activity);

    /**
     * Interface implementation.
     * @see int HsActivityDbClientInterface::removeApplicationActivities(const QVariantHash &)
    */
    int removeApplicationActivities(const QVariantHash &activity);

    /**
     * Interface implementation.
     * @see int HsActivityDbClientInterface::activities(QList<QVariantHash> &);
     */
    int activities(QList<QVariantHash> &);

    /**
     * Interface implementation.
     * @see int HsActivityDbClientInterface::applicationActivities(QList<QVariantHash> &, const QVariantHash &)
     */
    int applicationActivities(QList<QVariantHash>&, const QVariantHash &);

    /**
     * Interface implementation.
     * @see int HsActivityDbClientInterface::waitActivity(const QVariantHash &)
     */
    int waitActivity(const QVariantHash &activity);
    
    int getThumbnail(QSize size, QString imagePath, QString  mimeType, void *userDdata);

    /**
     * Interface implementation.
     * @see int HsActivityDbClientInterface::launchActivity(const QVariantHash &)
     */
    int launchActivity(const QVariantHash &);

    /**
     * Interface implementation.
     * @see int HsActivityDbClientInterface::cancelWaitActivity()
     */
    int cancelWaitActivity();
    
public:
    /**
     * Function get cached data from server
     * @param taskId - request task id
     * @param dst - destination, preallocated buffer
     */
    void getData(int taskId, RBuf8 &dst);

    /**
     * Function initialize aync request
     * @param func - requested function
     * @param data - request data
     * @param status - request status
     */
    void sendDataAsync(int func,const TIpcArgs &data, TRequestStatus &status);
    
//    void emitActivityRequested();
//    void emitThumbnailRequested();
    
public:
    void PushL(HsActivityDbAsyncRequestPrivate * task);
    void Pop(HsActivityDbAsyncRequestPrivate *task);

private:
    /**
     * Function start activity server process.
     * Function can leave on failure.
     */
    void startServerL();

    /**
     * Function establish connection to activity server.
     * Function can leave on failure
     */
    void connectL();

    /**
     * Function execute remote call request.
     * @param function - remote function identyfier
     * @param activity - remote function parameters
     */
    void execSimpleRequestL(int function, const QVariantHash &activity);

    /**
     * Function retrieve current activity name for application.
     * Function can leave on failure
     * @param  result - activity name
     * @param activity - request filtering rules
     */
    void requestedActivityNameL(QString &result,
                                const QVariantHash &activity);

    /**
     * Function retrieve all stored activity
     * Function can leave on failure
     * @param result - list of activity
     */
    void activitiesL(QList<QVariantHash>& result);

    /**
     * Function retrieve all stored activity
     * Function can leave on failure
     * @param result - list of activity
     * @param cond - request conditions
     */
    void applicationActivitiesL(QList<QVariantHash>& result,
                                const QVariantHash &cond);

    void waitActivityL(const QVariantHash &activity);
    
    void getThumbnailL(QSize size, QString imagePath, QString  mimeType, void *userDdata);
private:
    /**
     * Async request handler
     * Own
     */
    HsActivityDbAsyncRequestPrivate *mAsyncDataHandler;
    RPointerArray<HsActivityDbAsyncRequestPrivate> mAsyncTasks;
    HsActivityDbAsyncRequestObserver& mObserver;
};
#endif // HSACTIVITYDBCLIENTPRIVATE_H
