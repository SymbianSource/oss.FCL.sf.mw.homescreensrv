/*
* Copyright (c) 2010 Nokia Corporation and/or its subsidiary(-ies).
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


#ifndef CA_WIDGETSCANNERDEF_H
#define CA_WIDGETSCANNERDEF_H

// CONSTANTS
const TInt KChunkSize = 8192;
const TInt KDriveLetterLength = 2;
const TInt KModificationTimeLength = 17;
const TInt KNoId = -1;

// LIT
_LIT( KDoubleSlash, "\\" );
_LIT( KWidgetScannerUnderline, "_" );
_LIT( KWidgetScannerCaEntry, "CA_ENTRY" );
_LIT( KWidgetScannerEnText, "EN_TEXT" );
_LIT( KWidgetScannerEnDescription, "EN_DESCRIPTION" );


_LIT( KManifest, ".manifest");
_LIT( KColen, ":" );
_LIT( KImportDir, "\\private\\20022F35\\import\\widgetregistry\\" );
_LIT( KTrue, "true" );
_LIT8(KWidget, "widget" );
_LIT8(KUri, "uri" );
_LIT8(KLibrary, "library" );
_LIT8(KTitle, "title" );
_LIT8(KDescription, "description" );
_LIT8(KHidden, "hidden" );
_LIT8(KIconUri, "iconuri" );
_LIT8(KWidgetProvider, "widgetprovider");

_LIT8(KWidgetManifest, "hswidgetmanifest");
_LIT8(KIcon, "icon");
_LIT8(KServiceXml, "servicexml");


#endif // CA_WIDGETSCANNERDEF_H
