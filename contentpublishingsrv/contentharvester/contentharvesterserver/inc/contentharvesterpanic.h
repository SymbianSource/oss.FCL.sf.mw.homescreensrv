/*
* Copyright (c) 2007 Nokia Corporation and/or its subsidiary(-ies).
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
* Description:  Panics for cp active data server
 *
*/

 
#ifndef __CONTENTHARVESTERPANIC_H_
#define __CONTENTHARVESTERPANIC_H_

/** Panic Category */

/** Content Harvester panic codes */
enum TContentHarvesterPanic
    {
    EContentHarvesterBadRequest = 1,
    EContentHarvesterBadDescriptor = 2,
    EContentHarvesterSrvCreateServer = 3,
    EContentHarvesterMainSchedulerError = 4,
    EContentHarvesterCreateTrapCleanup = 5,
    EContentHarvesterSrvSessCreateTimer = 6,
    EContentHarvesterReqAlreadyPending = 7, //	A request is already pending
    EContentHarvesterEIkonEnv = 8
    };

#endif // __CONTENTHARVESTERPANIC_H_
