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
* Description:  Context types
*
*/


#ifndef HGCONTEXTDEF_H
#define HGCONTEXTDEF_H

#include <e32base.h>

// context source
_LIT( KHgCFSource, "Hg" );

// context types
_LIT( KHgCFTypeContact, "Contact" );
_LIT( KHgCFTypePbkContact, "PbkContact" );
_LIT( KHgCFTypePbkContactMulti, "PbkContactMulti" );
_LIT( KHgCFTypeText, "Text" );
_LIT( KHgCFTypePhoto, "Photo" );
_LIT( KHgCFTypeActiveDate, "ActiveDate" );
_LIT( KHgCFTypeUrl, "Url" );
_LIT( KHgCFTypeDate, "Date" );

_LIT( KHgCFTypeMusicState, "MusicState" );
_LIT( KHgCFTypeMusicArtist, "MusicArtist" );
_LIT( KHgCFTypeMusicTitle, "MusicTitle" );
_LIT( KHgCFTypeMusicAlbum, "MusicAlbum" );
_LIT( KHgCFTypeMusicAlbumArt, "MusicAlbumArt" );
_LIT( KHgCFTypeMusicUri, "MusicUri" );
_LIT( KHgCFTypeMusicGenre, "MusicGenre" );
_LIT( KHgCFTypeMusicType, "MusicType" );

_LIT( KHgCFTypeMusicRadioName, "MusicRadioName" );
_LIT( KHgCFTypeMusicRadioUrl, "MusicRadioUrl" );
_LIT( KHgCFTypeMusicRadioFrequency, "MusicRadioFrequency" );
_LIT( KHgCFTypeMusicRadioRDSPI, "MusicRadioRDSPI" );

_LIT( KHgCFTypeVideoState, "VideoState" );
_LIT( KHgCFTypeVideoTitle, "VideoTitle" );
_LIT( KHgCFTypeVideoUri, "VideoUri" );
_LIT( KHgCFTypeVideoType, "VideoType" );

_LIT( KHgCFTypeTvChannelName, "TvChannelName" );
_LIT( KHgCFTypeTvProgramName, "TvProgramName" );
_LIT( KHgCFTypeTvProgramDesc, "TvProgramDesc" );
_LIT( KHgCFTypeTvProgramGenre, "TvProgramGenre" );

_LIT( KHgCFTypeGpsLatitude, "GpsLatitude" );
_LIT( KHgCFTypeGpsLongitude, "GpsLongitude" );

_LIT( KHgCFTypeOviId, "OviId" );

// some pre-defined values
_LIT( KHgCFValueUnknownContact, "<unknown>" ); // special value for PbkContact
_LIT( KHgCFValueUnknownInfo, "<unknown>" ); // e.g. for TV contexts, when information is not available
_LIT( KHgCFValueMusicTypePlayer, "MusicPlayer" );
_LIT( KHgCFValueMusicTypeRadio, "Radio" );
_LIT( KHgCFValueVideoTypeLocal, "VideoLocal" );
_LIT( KHgCFValueVideoTypeStream, "VideoStream" );

_LIT( KHgCFServiceIdPrefixOvi, "Ovi" ); // for CHgContextUtility::PublishServiceIdL

#endif // HGCONTEXTDEF_H
