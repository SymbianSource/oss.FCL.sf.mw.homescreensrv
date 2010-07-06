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
* Description:  Task list entry
 *
*/


#ifndef TSENTRY_H
#define TSENTRY_H

#include <e32base.h>
#include <e32cmn.h>
#include <tsgraphicfilescalinghandler.h>

#include "tsentrykey.h"
#include "tstaskmonitorglobals.h"

class CTsEntry;
class RReadStream;
class RWriteStream;
class CFbsBitmap;
class MHsDataObserver;

typedef RPointerArray<CTsEntry> RTsFswArray;

enum TAppType
{
    EApp = 0,
    EWidget,
    EJava
};

/**
 * An entry in the task list.
 */
NONSHARABLE_CLASS( CTsEntry ) : public CBase,
                                private MImageReadyCallBack
    {
public:
    static CTsEntry* NewL();

    static CTsEntry* NewLC();

    static CTsEntry* NewL(const TTsEntryKey& key, MHsDataObserver* observer =0);

    static CTsEntry* NewLC(const TTsEntryKey& key, MHsDataObserver* observer =0);

    ~CTsEntry();

    TUid AppUid() const;

    TAppType Type() const;

    const TDesC& AppName() const;

    TBool CloseableApp() const;

    Visibility GetVisibility() const;
    
    void SetVisibility(Visibility visibility);

    CFbsBitmap* AppIconBitmap() const;

    CFbsBitmap* AppIconMask() const;

    CFbsBitmap* Screenshot() const;

    UpdatePriority Priority() const;

    const TTsEntryKey& Key() const;

    void SetAppUid( const TUid& uid );

    void SetAppNameL( const TDesC& appName );

    void SetCloseableApp( TBool value );

    void SetScreenshotL(const CFbsBitmap& bitmap, UpdatePriority priority);

    void RemoveScreenshotL();

    void SetAppIcon( CFbsBitmap* aBitmap, CFbsBitmap* aMask );

    void ExternalizeL( RWriteStream& aStream ) const;

    void InternalizeL( RReadStream& aStream );

    static void ExternalizeArrayL( RWriteStream& stream,
        const RTsFswArray& array );

    static void InternalizeArrayL( RReadStream& stream,
        RTsFswArray& array );

private:
    CTsEntry();

    CTsEntry(const TTsEntryKey& aKey, MHsDataObserver* observer);

    void ImageReadyCallBack(TInt error, const CFbsBitmap *bitmap );

private:
    TUid mAppUid;
    HBufC* mAppName;
    TBool mCloseableApp;
    Visibility mVisibility;
    CFbsBitmap* mAppIconBitmap;
    CFbsBitmap* mAppIconMask;
    CFbsBitmap* mScreenshot;
    TTsEntryKey mKey;
    UpdatePriority mPriority;
    MHsDataObserver* mObserver;
    CTsGraphicFileScalingHandler* mImgTool;
    };

#endif
