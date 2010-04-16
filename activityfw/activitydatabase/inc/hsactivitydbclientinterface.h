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
#ifndef HSACTIVITYDBCLIENTINTERFACE_H
#define HSACTIVITYDBCLIENTINTERFACE_H
#include <qvariant.h>

/**
 * Enumerate supported functions
 */
enum ClientFunctions {
    AddActivity =0,
    UpdateActivity,
    RemoveActivity,
    RemoveApplicationActivities,
    RequestedActivityName,
    Activities,
    ApplicationActivities,
    WaitActivity,
    LaunchActivity,
    GetThumbnail,
    GetData,
    CancelWait
};

const char ActivityApplicationKeyword [] = ":ApplicationId";
const char ActivityActivityKeyword [] = ":ActivityName";
const char ActivityScreenshotKeyword [] = "screenshot";

class HsActivityDbClientInterface
{
public:

    /**
     * Function save provided avtivity in activity storage
     * @param activity - activity to store
     * @return 0 on succees, error code otherwise
     */
    virtual int addActivity(const QVariantHash &activity) =0;

    /**
     * Function update exiting activity
     * @param activity - filtering rules
     * @return 0 on succees, error code otherwise
     */
    virtual int updateActivity(const QVariantHash &activity) =0;

    /**
     * Function delete activity
     * @param activity - filtering rules
     * @return 0 on succees, error code otherwise
     */
    virtual int removeActivity(const QVariantHash &activity) =0;

    /**
     * Function delete activity for application
     * @param activity - filtering rules
     * @return 0 on succees, error code otherwise
     */
    virtual int removeApplicationActivities(const QVariantHash &activity) =0;

    /**
     * Function return list of all activity
     * @param result - destination list
     * @return 0 on succees, error code otherwise
     */
    virtual int activities(QList<QVariantHash>& result) =0;

    /**
     * Function return list of all activity
     * @param result - destination list,
     * @param activity - filtering rules
     * @return 0 on succees, error code otherwise
     */
    virtual int applicationActivities(QList<QVariantHash>& result,
                                      const QVariantHash &activity) =0;

    /**
     * Function subscribe to activity
     * @param  activity - filtering rules
     * @return 0 on succees, error code otherwise
     */
    virtual int waitActivity(const QVariantHash &activity)=0;

    /**
     * Function launch application activity
     * @param activity - filtering rules
     * @return 0 on succees, error code otherwise
     */
    virtual int launchActivity(const QVariantHash &activity)=0;
};
#endif //HSACTIVITYDBCLIENTINTERFACE_H