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
* Description:  Definition of menu server constants
*
*/


#ifndef __MENUSRVDEF_H_
#define __MENUSRVDEF_H_

#include <e32std.h>

/// Major version number.
LOCAL_D const TInt KMenuMajorVersion = 1;
/// Minor version number.
LOCAL_D const TInt KMenuMinorVersion = 0;
/// Build number.
LOCAL_D const TInt KMenuBuild = 0;

/// Menu Content Service Server name.
_LIT( KMenuSrvName, "!MCSSRV" );
/// Menu Content Service Server executable.
_LIT( KMenuSrvExe, "mcsmenuserver" );
/// Exit delay in microseconds (10 sec).
LOCAL_D const TInt KMenuSrvExitDelay = 10000000;
/// Stream buffer size.
LOCAL_D const TInt KMenuStreamBufSize = 512;

/**
 *  Stream buffer data container.
 */
NONSHARABLE_CLASS( TMenuBuf )
    {

public:     // Data

    TInt iLen; ///< Data length.
    TUint8 iData[KMenuStreamBufSize]; ///< Data buffer.

    };

#endif // __MENUSRVDEF_H_

