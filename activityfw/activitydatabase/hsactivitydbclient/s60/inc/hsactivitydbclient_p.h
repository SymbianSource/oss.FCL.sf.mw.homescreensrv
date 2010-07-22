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
    HsActivityDbClientPrivate(HsActivityDbAsyncRequestObserver &);

    ~HsActivityDbClientPrivate();

    int connect();

    int addActivity(const QVariantHash &privateData, 
                    const QVariantHash &publicData);

    int updateActivity(const QVariantHash &privateData, 
                       const QVariantHash &publicData);

    int removeActivity(const QVariantHash &activity);

    int removeApplicationActivities(const QVariantHash &activity);

    int activities(QList<QVariantHash> &);

    int applicationActivities(QList<QVariantHash>&, const QVariantHash &);

    int activityData(QVariant &result, const QVariantHash &activity);
    
    int waitActivity(const QVariantHash &activity);
    
    int getThumbnail(QSize size, QString imagePath, QString  mimeType, void *userDdata);
    
    int notifyDataChange();

    int launchActivity(const QVariantHash &);

    int cancelWaitActivity();

    int cancelNotifyDataChange();

public:
    void getData(int taskId, RBuf8 &dst);

    void sendDataAsync(int func,const TIpcArgs &data, TRequestStatus &status);

public:
    void PushL(HsActivityDbAsyncRequestPrivate * task);

    void Pop(HsActivityDbAsyncRequestPrivate *task);

private:
    void startServerL();

    void connectL();

    void execSimpleRequestL(int function, 
                            const QVariantHash &privateData, 
                            const QVariantHash &publicData);


    void activitiesL(QList<QVariantHash>& result);

    void applicationActivitiesL(QList<QVariantHash>& result,
                                const QVariantHash &cond);

    void activityDataL(QVariant &result, const QVariantHash &activity);

    void launchActivityL(const QVariantHash &activity);

    void getThumbnailL(QSize size, QString imagePath, QString  mimeType, void *userDdata);

    int checkDataConstraint(int func, const QVariantHash &data);

    int execute(int func, const QVariantHash &privateData, const QVariantHash &publicData);

    int execute(int func, QList<QVariantHash>&dst, const QVariantHash &src);

private:
    RPointerArray<HsActivityDbAsyncRequestPrivate> mAsyncTasks;
    HsActivityDbAsyncRequestObserver& mObserver;
};
#endif // HSACTIVITYDBCLIENTPRIVATE_H
