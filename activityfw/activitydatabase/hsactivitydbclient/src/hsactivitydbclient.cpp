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
#include "hsactivitydbclient.h"
#include "hsactivitydbclient_p.h"

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
//
HsActivityDbClient::HsActivityDbClient(QObject *obj)
    :
    QObject(obj),
    d_ptr(0)
{
    d_ptr = new HsActivityDbClientPrivate(*this);
}

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
//
HsActivityDbClient::~HsActivityDbClient()
{
    delete d_ptr;
}

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
//
int HsActivityDbClient::connect()
{
    return d_ptr->connect();
}

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
//
void HsActivityDbClient::asyncRequestCompleated(int result,
                                                int requestType, 
                                                const QString& data)
{
    switch (requestType) {
        case WaitActivity:
            if (KErrNone == result) {
                emit activityRequested(data);
            }
            break;
    }
}

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
//
void HsActivityDbClient::asyncRequestCompleated(int result,
                                                int requestType, 
                                                const QPixmap& pixmap,
                                                void* userData)
{
    switch (requestType) {
        case GetThumbnail:
            if (KErrNone == result) {
                emit thumbnailRequested(pixmap, userData);
            }
            break;
    }
}

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
//
int HsActivityDbClient::addActivity(const QVariantHash &activity)
{
    return d_ptr->addActivity(activity);
}

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
//
int HsActivityDbClient::updateActivity(const QVariantHash &activity)
{
    return d_ptr->updateActivity(activity);
}

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
//
int HsActivityDbClient::removeActivity(const QVariantHash &activity)
{
    return d_ptr->removeActivity(activity);
}

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
//
int HsActivityDbClient::removeApplicationActivities(const QVariantHash &activity)
{
    return d_ptr->removeApplicationActivities(activity);
}

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
//
int HsActivityDbClient::activities(QList<QVariantHash>& result)
{
    return d_ptr->activities(result);
}

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
//
int HsActivityDbClient::applicationActivities(QList<QVariantHash> & result,
        const QVariantHash &conditions)
{
    return d_ptr->applicationActivities(result, conditions);
}

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
//
int HsActivityDbClient::waitActivity(const QVariantHash &activity)
{
    return d_ptr->waitActivity(activity);
}

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
//
int HsActivityDbClient::launchActivity(const QVariantHash &activity)
{
    return d_ptr->launchActivity(activity);
}

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
//
//int HsActivityDbClient::getThumbnail(const QVariantHash &condition)
int HsActivityDbClient::getThumbnail(QSize size, QString imagePath, QString mimeType, void* userDdata)
{
    return d_ptr->getThumbnail(size, imagePath, mimeType, userDdata);
}
