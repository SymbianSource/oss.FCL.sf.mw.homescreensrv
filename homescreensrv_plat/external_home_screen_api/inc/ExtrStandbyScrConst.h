/*
* Copyright (c) 2005-2007 Nokia Corporation and/or its subsidiary(-ies).
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
* Description:  Define the API for a external standby screen
*
*/


#ifndef EXTRSTANDBYSCRCONST_H
#define EXTRSTANDBYSCRCONST_H

#define KExtrStandbyScrInterfaceUid 0x101F8700

// This constant should be used in source code to check
// if opaque_data field of ecom plugin registration
// file has value which identifies fullscreen mode
#define KExtrStandbyScrFullScreenMode _L8("fullscreen")

// This constant can be used in Ecom registration file
// to set opaque_data field correctly.
// e.g. opaque_data = KExtrStandbyScrFullScreenModeRss;
#define KExtrStandbyScrFullScreenModeRss "fullscreen"

#endif // EXTRSTANDBYSCRCONST_H
