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
* Description:  CFW source and type names
*
*/

#ifndef HGCONTEXTTYPES_H
#define HGCONTEXTTYPES_H

#include <e32base.h>
#include <hg/hgcontextdef.h> // pull in the public part

_LIT( KHgCFTypeFgApp, "FgApp" ); // published by fswserver

_LIT( KHgCFTypeUpdateAvail, "UpdateAvail" ); // for iad source plug-in

_LIT( KHgCFTypeMdsObjChanged, "MdsObjChanged" );
_LIT( KHgCFTypeMdsRelChanged, "MdsRelChanged" );

_LIT( KHgCFValueDuplicateMarker, "<dupl>" ); // may be used in PbkContactMulti

const TInt KHgCFValueLinkMarker = 2; // prefix when Contact holds a vpbk contact link

_LIT( KHgCFTypeCallState, "CallState" );
_LIT( KHgCFTypeCallSusp, "CallSusp" ); // see hgcfcallsourceplugin
_LIT( KHgCFTypeContactFromCall, "ContactFromCall" ); // contains "1" if contact is coming from a voice/video call

#endif // HGCONTEXTTYPES_H
