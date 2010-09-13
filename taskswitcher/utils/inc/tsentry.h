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
class CFbsBitmap;
class MTsDataObserver;

typedef RPointerArray<CTsEntry> RTsFswArray;

/**
 * An entry in the task list.
 */
NONSHARABLE_CLASS( CTsEntry ) : public CBase,
                                private MImageReadyCallBack
{
public:
    static CTsEntry *NewL(const TTsEntryKey &key, MTsDataObserver &observer);
    static CTsEntry *NewLC(const TTsEntryKey &key, MTsDataObserver &observer);
    ~CTsEntry();

public:
    TUid AppUid() const;
    const TDesC &AppName() const;
    TBool CloseableApp() const;
    Visibility GetVisibility() const;    
    CFbsBitmap *AppIconBitmap() const;
    CFbsBitmap *Screenshot() const;
    const TTsEntryKey &Key() const;
    TTime Timestamp() const;    
    TTime LastUpdateTimestamp() const;
    
    void SetAppUid(const TUid &uid);
    void SetAppNameL(const TDesC &appName);
    void SetCloseableApp(TBool value);   
    void SetVisibility(Visibility visibility);
    void SetAppIcon(CFbsBitmap *aBitmap);
    void SetScreenshotL(const CFbsBitmap &bitmap, UpdatePriority priority, TInt angle);
    void RemoveScreenshotL();
    void SetTimestamp(const TTime &timestamp);
    void RefreshUpdateTimestamp();

private:
    CTsEntry(const TTsEntryKey &aKey, MTsDataObserver &observer);    
    
public: // from MImageReadyCallBack
    void ImageReadyCallBack(TInt error, const CFbsBitmap *bitmap);

private:
    TUid mAppUid;
    HBufC* mAppName;
    TBool mCloseableApp;
    Visibility mVisibility;
    CFbsBitmap* mAppIconBitmap;
    CFbsBitmap* mScreenshot;
    TTsEntryKey mKey;
    UpdatePriority mPriority;
    TTime mTimestamp;
    TTime mUpdateTimestamp;
    
private:     
    CTsGraphicFileScalingHandler *mImgTool;

private:    
    MTsDataObserver &mObserver;
    
};

#endif
