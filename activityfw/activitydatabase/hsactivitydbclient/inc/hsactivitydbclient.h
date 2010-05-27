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

#ifndef HSACTIVITYDBCLIENT_H
#define HSACTIVITYDBCLIENT_H
#include <QObject>
#include <hsactivitydbasyncrequestobserver.h>
#include "hsactivitydbclientinterface.h"

#ifdef ACTIVITY_LIB
    #define ACTIVITY_EXPORT Q_DECL_EXPORT
#else
    #define ACTIVITY_EXPORT Q_DECL_IMPORT
#endif

class HsActivityDbClientPrivate;

/**
 * Class implemets HsActivityDbClientInterface and is responsible for activity data management.
 */
class ACTIVITY_EXPORT HsActivityDbClient:   public QObject,
                                            public HsActivityDbClientInterface,
                                            public HsActivityDbAsyncRequestObserver
{
    Q_OBJECT
public:
    /**
     * Constructor
     */
    HsActivityDbClient(QObject* =0);

    /**
     * Destructor
     */
    ~HsActivityDbClient();

    /**
     * Establish connection with activity server
     * @return 0 on succees, error code otherwise
     */
    int connect();

    /**
     * Interface implementation.
     * @see int HsActivityDbAsyncRequestObserver::asyncRequestCompleated(int, int, const QString&)
     */
    void asyncRequestCompleated(int, int, const QString &);

    /**
     * Interface implementation.
     * @see int HsActivityDbAsyncRequestObserver::asyncRequestCompleated(int, int, QPixmap&)
     */
    void asyncRequestCompleated(int, int, const QPixmap&, void*);
    

    /**
     * Interface implementation.
     * @see int HsActivityDbAsyncRequestObserver::asyncRequestCompleated(int, int)
     */
    void asyncRequestCompleated(int result,
                                int requestType);

    /**
     * Interface implementation.
     * @see int HsActivityDbClientInterface::saveActivity(const QVariantHash&)
     */
    int addActivity(const QVariantHash &);

    /**
     * Interface implementation.
     * @see int HsActivityDbClientInterface::updateActivity(const QVariantHash&)
     */
    int updateActivity(const QVariantHash &);

    /**
     * Interface implementation.
     * @see int HsActivityDbClientInterface::deleteActivity(const QVariantHash &)
     */
    int removeActivity(const QVariantHash &activity);

    /**
     * Interface implementation.
     * @see int HsActivityDbClientInterface::deleteApplicationActivities(const QVariantHash &)
     */
    int removeApplicationActivities(const QVariantHash &activity);

    /**
     * Interface implementation.
     * @see int HsActivityDbClientInterface::activities(QList<QVariantHash>&)
     */
    int activities(QList<QVariantHash> &);

    /**
     * Interface implementation.
     * @see int HsActivityDbClientInterface::applicationActivities(QList<QVariantHash> &, const QVariantHash &)
     */
    int applicationActivities(QList<QVariantHash> &,
                              const QVariantHash &);

    /**
     * Interface implementation.
     * @see int HsActivityDbClientInterface::waitActivity(const QVariantHash &)
     */
    int waitActivity(const QVariantHash &activity);
    
    /**
     * Interface implementation.
     * @see int HsActivityDbClientInterface::launchActivity(const QVariantHash &)
     */
    int launchActivity(const QVariantHash &);
    
    /**
     */
    //getThumbnail(resolution, thumbnailPath, "image/png", data)
    int getThumbnail(QSize size, QString imagePath, QString mimeType, void* userDdata);
    
    int notifyDataChange();

signals:
    /**
     * Function notify about runtime activity change
     * @param activityId - requested activity name
     */
    void activityRequested(const QString &activityId);

    /**
     * Function notify about runtime activity change
     * @param thumbnailPixmap - requested thumbnail
     */
    void thumbnailRequested(QPixmap thumbnailPixmap, void *userData);
    
    void dataChanged();
    
private:
    /**
     * Private client implementation.
     * Own.
     */
    HsActivityDbClientPrivate *d_ptr;
    
    friend class HsActivityDbClientPrivate;
};

#endif //HSACTIVITYDBCLIENT_H
