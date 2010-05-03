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
#ifndef HSACTIVITYSTORAGE_H
#define HSACTIVITYSTORAGE_H
#include "hsactivitydbclientinterface.h"
class HsActivityStoragePrivate;

/**
 * Class implements HsActivityDbClientInterface and storage functionality
 */
class HsActivityStorage: public HsActivityDbClientInterface
{
public:
    /**
     * Constructor
     */
    HsActivityStorage();

    /**
     * Destructor
     */
    ~HsActivityStorage();

    /**
     * Interface implementation.
     * @see int HsActivityDbClientInterface::addActivity(const QVariantHash&)
     */
    int addActivity(const QVariantHash &activity);

    /**
     * Interface implementation.
     * @see int HsActivityDbClientInterface::updateActivity(const QVariantHash&)
     */
    int updateActivity(const QVariantHash &activity);

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
     * @see int HsActivityDbClientInterface::activities(QList<QVariantHash> &)
     */
    int activities(QList<QVariantHash>& result);

    /**
     * Interface implementation.
     * @see int HsActivityDbClientInterface::applicationActivities(QList<QVariantHash> &, const QVariantHash &)
     */
    int applicationActivities(QList<QVariantHash>& result,
                              const QVariantHash &activity);

    /**
     * Interface implementation.
     * @see int HsActivityDbClientInterface::waitActivity(const QVariantHash &)
     */
    int waitActivity(const QVariantHash &activity);

    /**
     * Interface implementation.
     * @see int HsActivityDbClientInterface::launchActivity(const QVariantHash &)
     */
    int launchActivity(const QVariantHash &activity);
private:
    /**
     * Private storage implementation
     * Own.
     */
    HsActivityStoragePrivate *d_ptr;
};
#endif //HSACTIVITYSTORAGE_H
