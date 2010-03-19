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


#ifndef HSSTATEMODEL_GLOBAL_H
#define HSSTATEMODEL_GLOBAL_H

#include <Qt/qglobal.h>


#ifdef HSSTATEMODEL_LIB
	#define HSSTATEMODEL_EXPORT Q_DECL_EXPORT
#else
    #ifdef HSSTATEMODEL_TEST
        #define HSSTATEMODEL_EXPORT
    #else
        #define HSSTATEMODEL_EXPORT Q_DECL_IMPORT
    #endif	
#endif


#endif // HSSTATEMODEL_GLOBAL_H
