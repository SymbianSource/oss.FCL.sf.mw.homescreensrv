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
* Description:  Panic enumeration 
 *
*/


#ifndef __BSPANIC_H__
#define __BSPANIC_H__

/** Panic Category */

/** Active Data Server panic codes */
enum TBSEnginePanic
    {
    EBSEngineBadRequest = 1,
    EBSEngineBadDescriptor = 2,
    EBSEngineSrvCreateServer = 3,
    EBSEngineCreateTrapCleanup = 4,
    EBSEngineEIkonEnv = 5
    };

#endif // __BSPANIC_H__
