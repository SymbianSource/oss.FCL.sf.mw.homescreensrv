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
            if (KErrCancel != result) {
                waitActivity(QVariantHash());
            }            
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
            emit thumbnailRequested(0 == result ? pixmap : QPixmap(), 
                                    userData);
            break;
    }
}

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
//
void HsActivityDbClient::asyncRequestCompleated(int result,int requestType)
{
    switch(requestType) {
    case NotifyChange:
        if (KErrCancel != result) {
            d_ptr->notifyDataChange();
        }
        if (KErrNone == result) {
            emit dataChanged();
        }
        break;
    }
}

// -----------------------------------------------------------------------------
/**
 * Interface implementation.
 * @see int HsActivityDbClientInterface::addActivity(const QVariantHash&)
 */
int HsActivityDbClient::addActivity(const QVariantHash &privateData, 
                                    const QVariantHash &publicData)
{
    return d_ptr->addActivity(privateData, publicData);
}

// -----------------------------------------------------------------------------
/**
 * Interface implementation.
 * @see int HsActivityDbClientInterface::updateActivity(const QVariantHash&)
 */
int HsActivityDbClient::updateActivity(const QVariantHash &privateData, 
                                       const QVariantHash &publicData)
{
    return d_ptr->updateActivity(privateData, publicData);
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
int HsActivityDbClient::activityData(QVariant &result, const QVariantHash &activity)
{
    return d_ptr->activityData(result, activity);
}

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
//
int HsActivityDbClient::waitActivity(const QVariantHash &activity)
{
    QVariantHash condition(activity);
    RProcess process;
    condition.insert(ActivityApplicationKeyword, 
                     static_cast<int>(process.SecureId().iId));
    return d_ptr->waitActivity(condition);
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
int HsActivityDbClient::getThumbnail(QSize size, QString imagePath, QString mimeType, void* userDdata)
{
    return d_ptr->getThumbnail(size, imagePath, mimeType, userDdata);
}

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
//
int HsActivityDbClient::notifyDataChange()
{
    return  d_ptr->notifyDataChange();
}
