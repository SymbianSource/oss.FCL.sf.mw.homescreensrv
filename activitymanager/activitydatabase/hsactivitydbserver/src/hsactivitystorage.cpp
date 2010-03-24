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
* Description: Application entry point
*
*/
#include "hsactivitystorage.h"
#include "hsactivitystorage_p.h"

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
//
HsActivityStorage::HsActivityStorage()
:
d_ptr(new HsActivityStoragePrivate())
{
}

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
//
HsActivityStorage::~HsActivityStorage()
{
    delete d_ptr;
}

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
//
int HsActivityStorage::addActivity(const QVariantHash& activity)
{
    return d_ptr->addActivity(activity);
}

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
//
int HsActivityStorage::updateActivity(const QVariantHash& activity)
{
    return d_ptr->updateActivity(activity);
}

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
//
int HsActivityStorage::removeActivity(const QVariantHash& activity)
{
    return d_ptr->removeActivity(activity);
}

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
//
int HsActivityStorage::removeApplicationActivities(const QVariantHash& activity)
{
    return d_ptr->removeApplicationActivities(activity);
}

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
//
int HsActivityStorage::activities(QList<QVariantHash>& result)
{   
    return d_ptr->activities(result);
}

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
//
int HsActivityStorage::applicationActivities(QList<QVariantHash>& result, 
                                             const QVariantHash &activity)
{
    return d_ptr->applicationActivities(result, activity);
}

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
//
int HsActivityStorage::waitActivity(const QVariantHash &activity)
{
    return d_ptr->waitActivity(activity);
}

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
//
int HsActivityStorage::launchActivity(const QVariantHash &activity)
{
    return d_ptr->launchActivity(activity);
}
