/*
* Copyright (c) 2004 Nokia Corporation and/or its subsidiary(-ies).
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


#ifndef __CommonFramework_H
#define __CommonFramework_H

#include <e32base.h>
#include <e32cons.h>

const TUid KUidhspsThemeInstallerCons = { 0x054FDF26 };
const TInt KArrGranularity = 10;

void startupL();

enum TType
{
 EArgumentsNotDefined= -1,
 ENoFilesNotFound = -2,
 EErrorDeletingDir = -3,
 EErrorDirNotExists = -4
};



#endif

