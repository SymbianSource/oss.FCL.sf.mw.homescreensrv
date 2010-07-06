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
#include "afstorageclient.h"
#include "afstorageclient_p.h"

// -----------------------------------------------------------------------------
/**
 * Constructor
 */
AfStorageClient::AfStorageClient(QObject *obj)
:
QObject(obj),
d_ptr(0)
{
    d_ptr = new AfStorageClientPrivate(*this);
}

// -----------------------------------------------------------------------------
/**
 * Destructor
 */
AfStorageClient::~AfStorageClient()
{
    delete d_ptr;
}

// -----------------------------------------------------------------------------
/**
 * Establish connection with activity server
 * @return 0 on succees, error code otherwise
 */
int AfStorageClient::connect()
{
    return d_ptr->connect();
}

// -----------------------------------------------------------------------------
/**
 * Interface implementation.
 * @see int MAfAsyncRequestObserver::asyncRequestCompleated(int, int, const QString&)
 */
void AfStorageClient::asyncRequestCompleated(int result,
                                             int requestType, 
                                             const QString& data)
{
    switch (requestType) {
    case WaitActivity:
        if (KErrCancel != result) {
            waitActivity();
        }            
        if (KErrNone == result) {
            emit activityRequested(data);
        }
        
        break;
    }
}

// -----------------------------------------------------------------------------
/**
 * Interface implementation.
 * @see int MAfAsyncRequestObserver::asyncRequestCompleated(int, int, QPixmap&, void*)
 */
void AfStorageClient::asyncRequestCompleated(int result,
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
/**
 * Interface implementation.
 * @see int MAfAsyncRequestObserver::asyncRequestCompleated(int, int)
 */
void AfStorageClient::asyncRequestCompleated(int result,int requestType)
{
    switch(requestType) {
    case NotifyChange:
        if (KErrCancel != result) {
            notifyDataChange();
        }
        if (KErrNone == result) {
            emit dataChanged();
        }
        break;
    }
}

// -----------------------------------------------------------------------------
/**
 * Function add new activity
 * @param entry - activity entry data structure
 * @param imageSource - source for activity thumbnail
 * @return 0 on success, error code otherwise
 */
int AfStorageClient::addActivity(const AfStorageEntry &entry, const QPixmap &imageSource)
{
    return d_ptr->addActivity(entry, imageSource);
}

// -----------------------------------------------------------------------------
/**
 * Function update existing activity
 * @param entry - activity entry data structure
 * @param imageSource - source for activity thumbnail
 * @return 0 on success, error code otherwise
 */
int AfStorageClient::updateActivity(const AfStorageEntry &entry, 
                                    const QPixmap &imageSource)
{
    return d_ptr->updateActivity(entry, imageSource);
}

// -----------------------------------------------------------------------------
/**
 * Function remove existing activity
 * @param entry - activity entry template
  * @return 0 on success, error code otherwise
 */
int AfStorageClient::removeActivity(const AfStorageEntry &entry)
{
    return d_ptr->removeActivity(entry);
}

// -----------------------------------------------------------------------------
/**
 * Function remove existing activities for application
 * @param entry - activity entry template
  * @return 0 on success, error code otherwise
 */
int AfStorageClient::removeApplicationActivities(const AfStorageEntry &entry)
{
    return d_ptr->removeApplicationActivities(entry);
}

// -----------------------------------------------------------------------------
/**
 * Function retreive public data for all activities
 * @param dst - list of results
 * @return 0 on success, error code otherwise 
 */
int AfStorageClient::activities(QList<AfStorageEntry> &dst)
{
    return d_ptr->activities(dst);
}

// -----------------------------------------------------------------------------
/**
 * Function retreive public data for all application activities
 * @param dst - list of results
 * @param entry - activity template
 * @return 0 on success, error code otherwise 
 */
int AfStorageClient::applicationActivities(QList<AfStorageEntry> &dst, 
                                           const AfStorageEntry &entry)
{
    return d_ptr->applicationActivities(dst, entry);
}

// -----------------------------------------------------------------------------
/**
 * Function retreive all data for requested activity
 * @param dst - activity entry
 * @param entry - activity template
 * @return 0 on success, error code otherwise 
 */
int AfStorageClient::activityData(AfStorageEntry &dst, const AfStorageEntry &entry)
{
    return d_ptr->activityData(dst, entry);
}

// -----------------------------------------------------------------------------
/**
 * Function subscribe application for notyfication about requested activity changes
 * @return 0 on success, error code otherwise 
 */
int AfStorageClient::waitActivity()
{
    return d_ptr->waitActivity();
}

// -----------------------------------------------------------------------------
/**
 * Function request activity change
 * @param entry - activity template 
 * @return 0 on success, error code otherwise 
 */
int AfStorageClient::launchActivity(const AfStorageEntry &entry)
{
    return d_ptr->launchActivity(entry);
}

// -----------------------------------------------------------------------------
int AfStorageClient::getThumbnail(QSize size, QString imagePath, void* userDdata)
{
    return d_ptr->getThumbnail(size, imagePath, userDdata);
}

// -----------------------------------------------------------------------------
/**
 * Function subscribe launcher for data model changes
 * @return 0 on success, error code otherwise 
 */
int AfStorageClient::notifyDataChange()
{
    return  d_ptr->notifyDataChange();
}
