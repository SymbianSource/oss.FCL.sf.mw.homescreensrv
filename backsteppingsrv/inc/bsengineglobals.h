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
* Description:  Globals for BS engine
 *
*/


#ifndef C_BSENGINEGLOBALS_H
#define C_BSENGINEGLOBALS_H

// CONSTANTS
const TUint KBSEngineMajorVersionNumber = 0;
const TUint KBSEngineMinorVersionNumber = 1;
const TUint KBSEngineBuildVersionNumber = 1;

_LIT( KBSEngineThreadName, "bsengine" );
_LIT( KBSEngineName, "bsengine" );
_LIT( KBSEngineFilename, "bsengine" );

const TUid KServerUid3 =
    {
    0x2000F83E
    };

// ENUMERATIONS
// Opcodes used in message passing between client and server
enum TBSEngineRqst
    {
    EBSEngineInitialize = 100,
    EBSEngineHandleActivationEvent = 101,
    EBSEngineHandleBackCommand = 102
    };

enum TBSEngineRsp
    {
    EBSEngineCommandWasNotConsumed,
    EBSEngineCommandWasConsumed
    };

#endif      // C_BSENGINEGLOBALS_H
// End of File
