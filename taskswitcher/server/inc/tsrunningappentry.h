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

#ifndef TSRUNNINGAPPENTRY_H
#define TSRUNNINGAPPENTRY_H

#include <e32base.h>
#include <e32cmn.h>

#include "tsrunningapp.h"
#include "tsentrykey.h"
#include "tstaskmonitorglobals.h"
#include "tsthumbnailobserver.h"


class CTsRunningAppEntry;
class CFbsBitmap;
class QObject;
class MTsResourceManager;
class MTsDataObserver;
class TsThumbnailProvider;

typedef RPointerArray<CTsRunningAppEntry> RTsFswArray;

/**
 * An entry in the task list.
 */
NONSHARABLE_CLASS( CTsRunningAppEntry ) : public CBase,
                                          public MTsRunningApplication,
                                          private MTsThumbnailObserver
{
public:
    static CTsRunningAppEntry *NewL(const TTsEntryKey aKey, 
                                    const MTsRunningApplication& aCacheEntry,
                                    MTsResourceManager& aResources, 
                                    MTsDataObserver &observer, 
                                    QObject* obj);
    static CTsRunningAppEntry *NewLC(const TTsEntryKey aKey,
                                     const MTsRunningApplication& aCacheEntry,
                                     MTsResourceManager& aResources,
                                     MTsDataObserver &observer, 
                                     QObject* obj);
    ~CTsRunningAppEntry();

public://from MTsRunningApplication
    TUid Uid()const;
    ApplicationHideMode HideMode() const;
    void SetHidden( TBool aHidden );
    TBool IsSystem() const;
    TInt WindowGroupId() const;
    TInt ParentWindowGroupId() const;
    TBool IsEmbeded() const;
    
    const TDesC& DisplayName() const;
    TInt IconHandle() const;
    TTime Timestamp() const;
    TTime TimestampUpdate() const; 
    TTsEntryKey Key() const;
    TBool IsActive() const;
    TBool IsClosable() const;
    TBool Close() const;
    TBool Launch() const;
    TBool IsMandatory() const;

public:
    void SetScreenshotL(const CFbsBitmap &bitmap, UpdatePriority priority, TInt angle);
    void RemoveScreenshotL();
    void RefreshTimestamp();
    void RefreshUpdateTimestamp();
    void SetCloseableApp(TBool aClosable);

private:
    CTsRunningAppEntry(const TTsEntryKey aKey,
                       MTsResourceManager& aResources,
                       MTsDataObserver &aObserver);
    void ConstructL(const MTsRunningApplication& aCacheEntry, QObject* aObject);
    void CreateDisplayNameL(const MTsRunningApplication& aCacheEntry);

public://from MTsThumbnailObserver
    void ThumbnailCreated(const CFbsBitmap& aThumbnail);

private:
    MTsResourceManager& iResources;
    MTsDataObserver& iObserver;
    
    const TTsEntryKey iKey;
    UpdatePriority iPriority;
    TUid iUid;
    TBool iIsSystem;
    ApplicationHideMode iHideMode;
    TInt iWindowGroupId;
    TInt iParentWindowGroupId;
    TTime iTimestamp;
    TTime iUpdateTimestamp;
    HBufC *iCaption;
    mutable CFbsBitmap* iThumbnail;
    TsThumbnailProvider* iThumbnailProvider;

};

#endif //TSRUNNINGAPPENTRY_H
