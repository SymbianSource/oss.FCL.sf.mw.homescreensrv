/*
* Copyright (c) 2008 Nokia Corporation and/or its subsidiary(-ies).
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
* Description:  Observer interfaces to get notifications from appui and from engine
 *
*/


#ifndef TSFSWOBSERVERS_H
#define TSFSWOBSERVERS_H

/**
 * Interface to get notifications from appui when window groups
 * have changed.
 */
class MTsFswTaskListObserver
    {
public:
    /**
     * Function called by AppUi to notify about possible
     * changes in task list
     */
    virtual void UpdateTaskList() = 0;
    };
#endif
