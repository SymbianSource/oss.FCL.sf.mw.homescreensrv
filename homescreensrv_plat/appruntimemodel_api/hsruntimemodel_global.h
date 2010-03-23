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
* Description:  The dll header file.
*
*/


#ifndef HSRUNTIMEMODEL_GLOBAL_H
#define HSRUNTIMEMODEL_GLOBAL_H

#include <Qt/qglobal.h>


#ifdef HSRUNTIMEMODEL_LIB
	#define HSRUNTIMEMODEL_EXPORT Q_DECL_EXPORT
#else
    #ifdef HSRUNTIMEMODEL_TEST
        #define HSRUNTIMEMODEL_EXPORT
    #else
        #define HSRUNTIMEMODEL_EXPORT Q_DECL_IMPORT
    #endif	
#endif


#endif // HSRUNTIMEMODEL_GLOBAL_H
