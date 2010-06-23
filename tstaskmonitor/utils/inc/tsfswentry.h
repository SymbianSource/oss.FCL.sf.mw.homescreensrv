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


#ifndef TSFSWENTRY_H
#define TSFSWENTRY_H

#include <e32base.h>
#include <e32cmn.h>
#include <tsgraphicfilescalinghandler.h>

#include "tsentrykey.h"
#include "tstaskmonitorglobals.h"

class CTsFswEntry;
class RReadStream;
class RWriteStream;
class CFbsBitmap;
class MHsDataObserver;

typedef RPointerArray<CTsFswEntry> RTsFswArray;

enum TAppType
{
    EApp = 0,
    EWidget,
    EJava    
};

/**
 * An entry in the task list.
 */
NONSHARABLE_CLASS( CTsFswEntry ) : public CBase,
                                   private MImageReadyCallBack
    {
public:
    static CTsFswEntry* NewL();
    static CTsFswEntry* NewLC();
    
    static CTsFswEntry* NewL(const TTsEntryKey& key, MHsDataObserver* observer =0);
    static CTsFswEntry* NewLC(const TTsEntryKey& key, MHsDataObserver* observer =0);
    
    ~CTsFswEntry();

    /**
     * Window group id.
     * In case of wrt widgets this is negative and is not a valid window group id.
     * Use this when calling CloseApp or SwitchToApp. (these work with widgets too)
     */
    TInt WgId() const;
    
    /**
     * Application uid.
     */
    TUid AppUid() const;
    
    /**
     * Application type.
     */
    TAppType Type() const;
    
    /**
     * Application name.
     */
    const TDesC& AppName() const;    
    
    /**
     * ETrue if the application is closeable
     */
    TBool CloseableApp() const;
    
    /**
     * Application icon bitmap
     */
    CFbsBitmap* AppIconBitmap() const;
    
    /**
     * Application icon mask.
     */
    CFbsBitmap* AppIconMask() const;
    
    /**
     * Application screenshot.
     */
    CFbsBitmap* Screenshot() const;
    
    /**
     * Priority.
     */
    UpdatePriority Priority() const;
    
    /**
     * Entry's key
     */
    const TTsEntryKey& Key() const;


    void SetWgId( TInt wgId );
    void SetAppUid( const TUid& uid );
    void SetAppNameL( const TDesC& appName );
    void SetCloseableApp( TBool value );
    void SetScreenshotL(const CFbsBitmap* bitmap, UpdatePriority priority);
    void RemoveScreenshotL();
    
    // takes ownership of bitmaps
    void SetAppIcon( CFbsBitmap* aBitmap, CFbsBitmap* aMask );
    void ExternalizeL( RWriteStream& aStream );
    void InternalizeL( RReadStream& aStream );

    static void ExternalizeArrayL( RWriteStream& aStream,
        const RTsFswArray& aArray );
    static void InternalizeArrayL( RReadStream& aStream,
        RTsFswArray& aArray );

private:
    //constructors
    CTsFswEntry();
    CTsFswEntry(const TTsEntryKey& aKey, MHsDataObserver* observer);
    void ImageReadyCallBack(TInt error, const CFbsBitmap *bitmap );
    
private:
    TInt mWgId;
    TUid mAppUid;
    HBufC* mAppName;
    TBool mCloseableApp;
    CFbsBitmap* mAppIconBitmap;
    CFbsBitmap* mAppIconMask;
    CFbsBitmap* mScreenshot;
    TTsEntryKey mKey;
    UpdatePriority mPriority;
    MHsDataObserver* mObserver;
    CTsGraphicFileScalingHandler* mImgTool;
    };

#endif
