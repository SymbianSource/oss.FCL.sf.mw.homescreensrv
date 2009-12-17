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
* Description:  Globals for cp active data server
 *
*/


#ifndef C_CONTENTHARVESTERGLOBALS_H
#define C_CONTENTHARVESTERGLOBALS_H

// CONSTANTS
const TUint KContentHarvesterMajorVersionNumber = 0;
const TUint KContentHarvesterMinorVersionNumber = 1;
const TUint KContentHarvesterBuildVersionNumber = 1;

_LIT( KContentHarvesterThreadName, "contentharvester_MainThread" );
_LIT( KContentHarvesterName, "contentharvester" );
_LIT( KContentHarvesterFilename, "contentharvester" );

const TUid KServerUid3 =
    {
    0x10282E5A
    };

// ENUMERATIONS
// Opcodes used in message passing between client and server
enum KContentHarvesterRqst
    {
    EContentHarvesterUpdate = 100,
    EContentHarvesterStop
    };

enum KContentHarvesterRsp
    {
    EContentHarvesterUpdateComplete = 200,
    EContentHarvesterStopComplete
    };

#endif      // C_CONTENTHARVESTERGLOBALS_H
// End of File
