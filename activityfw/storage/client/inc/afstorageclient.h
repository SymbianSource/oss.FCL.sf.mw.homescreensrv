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

#ifndef AFSTORAGECLIENT_H
#define AFSTORAGECLIENT_H
#include <QObject>
#include <afasyncrequestobserver.h>

#ifdef ACTIVITY_LIB
    #define ACTIVITY_EXPORT Q_DECL_EXPORT
#else
    #define ACTIVITY_EXPORT Q_DECL_IMPORT
#endif

class AfStorageClientPrivate;
class AfStorageEntry;

class ACTIVITY_EXPORT AfStorageClient: public QObject,
                                       public MAfAsyncRequestObserver
{
    Q_OBJECT
public:
    AfStorageClient(QObject* =0);

    ~AfStorageClient();

    int connect();

    int addActivity(const AfStorageEntry &entry, const QPixmap& imageSource = QPixmap());

    int updateActivity(const AfStorageEntry &entry, const QPixmap& imageSource = QPixmap());

    int removeActivity(const AfStorageEntry &entry);

    int removeApplicationActivities(const AfStorageEntry &entry);

    int activities(QList<AfStorageEntry> &dst);

    int applicationActivities(QList<AfStorageEntry> &dst, 
                              const AfStorageEntry &entry);

    int activityData(AfStorageEntry &dst, const AfStorageEntry &entry);

    int waitActivity();
    
    int launchActivity(const AfStorageEntry &entry);
    
    int getThumbnail(QSize size, QString imagePath, void* userDdata);
    
    int notifyDataChange();

private:
    void asyncRequestCompleated(int, int, const QString &);

    void asyncRequestCompleated(int, int, const QPixmap&, void*);

    void asyncRequestCompleated(int result,
                                int requestType);

signals:
    void activityRequested(const QString &activityId);

    void thumbnailRequested(QPixmap thumbnailPixmap, void *userData);
    
    void dataChanged();
    
private:
    /**
     * Private client implementation.
     * Own.
     */
    AfStorageClientPrivate *d_ptr;
    
    friend class AfStorageClientPrivate;
};

#endif //AFSTORAGECLIENT_H
