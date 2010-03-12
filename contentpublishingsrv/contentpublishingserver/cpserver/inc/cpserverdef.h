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
* Description:  Globals for Content Publisher  server
 *
*/


#ifndef C_CPSERVERDEF_H
#define C_CPSERVERDEF_H

// CONSTANTS
const TUint KCPServerMajorVersionNumber = 0;
const TUint KCPServerMinorVersionNumber = 1;
const TUint KCPServerBuildVersionNumber = 1;

// ENUMERATIONS
// Opcodes used in message passing between client and server
enum TCpServerRqst
    {
    ECpServerAddData = 1,
    ECpServerRemoveData = 2,
    ECpServerAddDataNonPersistent = 3,
    ECpServerGetListSize = 10,
    ECpServerGetListData = 11,
    ECpServerRegisterObserver = 20,
    ECpServerUnRegisterObserver = 21,
    ECpServerGetChangeInfoData = 22,
    ECpServerAddObserver = 23,
    ECpServerRemoveObserver = 24,
    ECpServerExecuteAction = 30,
    ECpServerInternal = 100
    };

_LIT( KCPServerThreadName, "cpserver_MainThread" );
_LIT( KCPServerName, "cpserver" );
_LIT( KCPServerFilename, "cpserver" );
_LIT( KActiveDes, "1" );
_LIT( KInActiveDes, "0" );
_LIT( KColon, ":" );
_LIT( KSemiColon, ";" );

_LIT( KCPServerPluginThreadName, "CPS plugin command execution thread %d" );
const TInt KCPServerPluginThreadMinHeapSize = 0x400; // 1kB
const TInt KCPServerPluginThreadMaxHeapSize = 0x100000; // 1MB

const TUid KServerUid3 =
    {
    0x20016B7B
    };

/** Panic Category */

/** Active Data Server panic codes */
enum TCPServerPanic
    {
    ECPServerBadRequest = 1,
    ECPServerBadDescriptor = 2,
    ECPServerSrvCreateServer = 3
    };

const TUint KDescriptorPosition( 0);
const TUint KReturnPosition( 1);
const TUint KOptionsPosition( 2);
const TUint KTransactionPosition( 3);
const TBool KActive( ETrue );
const TBool KInActive( EFalse );

// Uid of CP Server
const TUid KServerUid 			= { 0x20016B7B };
// Uid of cpstorage
const TUint32 KCPStorageUid 	= { 0x20016B7C };
//Values of KSQLDBStateKey key
const TInt KSQLDBStateNormal 	= 0x00000001;
const TInt KSQLDBStateRestored	= 0x00000002;//KSQLDBStateNormal << 1
// const ITnt NextState = LastState << 1;



#endif // C_CPSERVERDEF_H
