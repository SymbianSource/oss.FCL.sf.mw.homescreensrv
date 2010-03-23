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
* Description: bagetmodel_global.h
*
*/


#ifndef BAGEMODEL_GLOBAL_H
#define BAGEMODEL_GLOBAL_H

#include <qglobal.h>

#ifdef BAGETMODEL_LIB
    #define BAGETMODEL_EXPORT Q_DECL_EXPORT
#else
    #ifdef BAGETMODEL_TEST
        #define BAGETMODEL_EXPORT
    #else
        #define BAGETMODEL_EXPORT Q_DECL_IMPORT
    #endif
#endif

#endif // BAGEMODEL_GLOBAL_H
