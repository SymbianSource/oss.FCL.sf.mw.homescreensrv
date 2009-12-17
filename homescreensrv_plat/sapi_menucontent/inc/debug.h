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

#ifndef DEBUG_H
#define DEBUG_H

#include <e32debug.h>

#ifdef _DEBUG

#define FLOG(aText)                 RDebug::Print(_L(aText));						
#define FLOGF(aText,aParam)          RDebug::Print(_L(aText),aParam);


#else   // _DEBUG not defined, no logging code will be included at all!

#define FLOG(a)
#define FLOGF(a,p)


#endif // _DEBUG

#endif // DEBUG_H

// End of File
