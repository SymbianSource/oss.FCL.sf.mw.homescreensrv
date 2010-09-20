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

#ifndef ACTIVITYTSMODEL_H
#define ACTIVITYTSMODEL_H

#include <QObject>
#include <QVariantHash>

#include "activitytsentry.h"

class ActivityTsModel : public QObject
{
    Q_OBJECT

public:
    ActivityTsModel(QObject *parent = 0);
    virtual ~ActivityTsModel();

public slots:
    QList<QVariantHash> taskList() const;
    QList<QVariantHash> taskList(int limit) const;
    
    bool openTask(const QVariant &id);
    bool closeTask(const QVariant &id);
    
signals:
    void dataChanged();
    
private slots:
    void getActivities();    
    
public slots:    
    void convertScreenshotToThumbnail(const QPixmap &thumbnail, void *userData);
    void thumbnailCreated(const QPixmap &thumbnail, const void *userData);
    
signals:
    void createThumbnail(const QPixmap &source, int angle, const void *userData);
    
private:
    QObject *mAfManager;
    int mMaxItems;
    
    QList<ActivityTsEntry*> mData;
    
};

#endif // ACTIVITYTSMODEL_H
