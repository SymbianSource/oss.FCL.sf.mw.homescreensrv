/*
* Copyright (c) 2005-2006 Nokia Corporation and/or its subsidiary(-ies).
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
* Description:  Platform default shortcuts.
*
*/


#ifndef AIDEFAULTSHORTCUT_H
#define AIDEFAULTSHORTCUT_H

#include <e32def.h> // for TInt
#include <e32cmn.h> // for TUid, TDes


class TAiDefaultShortcut
{
public:
    /**
     * Get S60 platform default shortcut uid and definition for index aIndex.
     *
     */
    static void GetDefaultShortcut(TInt aIndex, TUid& aUid, TDes& aDefinition);

};

#endif
