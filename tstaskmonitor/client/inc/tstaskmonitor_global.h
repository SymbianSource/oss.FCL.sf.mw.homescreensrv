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
#ifndef TSTASKMONITOR_GLOBAL_H
#define TSTASKMONITOR_GLOBAL_H

#include <qglobal.h>

#ifdef TSTASKMONITOR_LIB
    #define TSTASKMONITOR_EXPORT Q_DECL_EXPORT
#else
    #ifdef TSTASKMONITOR_TEST
        #define TSTASKMONITOR_EXPORT
    #else
        #define TSTASKMONITOR_EXPORT Q_DECL_IMPORT
    #endif
#endif

#endif //TSTASKMONITOR_GLOBAL_H
