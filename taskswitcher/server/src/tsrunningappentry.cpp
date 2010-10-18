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

#include <s32strm.h>
#include <fbs.h>
#include <AknDef.h>
#include <apgwgnam.h>
#include <apgtask.h>
#include <apgcli.h>

#include "tsrunningappentry.h"
#include "tsiconprovider.h"
#include "tsdataobserver.h"
#include "tsthumbnailprovider.h"
#include "tsresourcemanager.h"

// --------------------------------------------------------------------------
CTsRunningAppEntry* CTsRunningAppEntry::NewL(const TTsEntryKey aKey,
                                             const MTsRunningApplication& aCacheEntry,
                                             MTsResourceManager& aResources,
                                             MTsDataObserver &aObserver, 
                                             QObject* aObj)
{
    CTsRunningAppEntry* self = NewLC(aKey, 
                                     aCacheEntry , 
                                     aResources, 
                                     aObserver, 
                                     aObj);
    CleanupStack::Pop(self);
    return self;
}

// --------------------------------------------------------------------------
CTsRunningAppEntry* CTsRunningAppEntry::NewLC(const TTsEntryKey aKey,
                                              const MTsRunningApplication& aCacheEntry,
                                              MTsResourceManager& aResources,
                                              MTsDataObserver &aObserver, 
                                              QObject* aObj)
{
    CTsRunningAppEntry* self = new (ELeave) CTsRunningAppEntry(aKey, 
                                                               aResources, 
                                                               aObserver);
    CleanupStack::PushL(self);
    self->ConstructL(aCacheEntry, aObj);
    return self;
}

// --------------------------------------------------------------------------
CTsRunningAppEntry::~CTsRunningAppEntry()
{
    delete iCaption;
    delete iThumbnail;
    delete iThumbnailProvider;
}

// --------------------------------------------------------------------------
CTsRunningAppEntry::CTsRunningAppEntry(const TTsEntryKey aKey,
                                       MTsResourceManager& aResources,
                                       MTsDataObserver &aObserver) 
:
    iResources(aResources),
    iObserver(aObserver),
    iKey(aKey)
{
}

// --------------------------------------------------------------------------
void CTsRunningAppEntry::ConstructL(const MTsRunningApplication& aCacheEntry, 
                                    QObject* aObject )
    {
    iUid = aCacheEntry.Uid();
    iIsSystem = aCacheEntry.IsSystem();
    iHideMode = aCacheEntry.HideMode();
    iWindowGroupId = aCacheEntry.WindowGroupId();
    iParentWindowGroupId = aCacheEntry.ParentWindowGroupId();
    iTimestamp.UniversalTime();
    iUpdateTimestamp.UniversalTime();
    CreateDisplayNameL(aCacheEntry);
    QT_TRYCATCH_LEAVING(
       iThumbnailProvider = new TsThumbnailProvider(*this, aObject);
    )
    }

// --------------------------------------------------------------------------
void CTsRunningAppEntry::CreateDisplayNameL(const MTsRunningApplication& aCacheEntry)
    {
    TApaAppInfo info;
    iResources.ApaSession().GetAppInfo( info, aCacheEntry.Uid() );
    TPtrC caption = info.iShortCaption;

    if (!caption.Length() ) // if not set - use thread name instead
        {
        if( aCacheEntry.DisplayName().Length() )
            {
            iCaption = aCacheEntry.DisplayName().AllocL();
            }
        else
            {
            TThreadId threadId;
            if(KErrNone == iResources.WsSession().GetWindowGroupClientThreadId( aCacheEntry.WindowGroupId(), threadId ) ) 
                {
                RThread thread;
                CleanupClosePushL( thread );
                if( KErrNone == thread.Open( threadId ) )
                    {
                    iCaption = thread.Name().AllocL();
                    }
                CleanupStack::PopAndDestroy( &thread );
                }
            }
        }
    else
        {
        iCaption = caption.AllocL();
        }
    }

// --------------------------------------------------------------------------
TUid CTsRunningAppEntry::Uid()const
    {
    return iUid;
    }
// --------------------------------------------------------------------------
MTsRunningApplication::ApplicationHideMode CTsRunningAppEntry::HideMode() const
    {
    return iHideMode;
    }

// --------------------------------------------------------------------------
void CTsRunningAppEntry::SetHidden( TBool aHidden )
    {
    iHideMode = aHidden ? MTsRunningApplication::Software : 
                          MTsRunningApplication::None;
    }

// --------------------------------------------------------------------------
TBool CTsRunningAppEntry::IsSystem() const
    {
    return iIsSystem;
    }

// --------------------------------------------------------------------------
TInt CTsRunningAppEntry::WindowGroupId() const
    {
    return iWindowGroupId;
    }

// --------------------------------------------------------------------------
TInt CTsRunningAppEntry::ParentWindowGroupId() const
    {
    return iParentWindowGroupId;
    }

// --------------------------------------------------------------------------
TBool CTsRunningAppEntry::IsEmbeded() const
    {
    return (0 >= iParentWindowGroupId);
    }

// --------------------------------------------------------------------------
const TDesC& CTsRunningAppEntry::DisplayName() const
    {
    return iCaption ? *iCaption : KNullDesC();
    }

// --------------------------------------------------------------------------
TInt CTsRunningAppEntry::IconHandle() const
    {
    if( 0 == iThumbnail )
        {
        TRAP_IGNORE( CFbsBitmap *icon(new(ELeave) CFbsBitmap());
                     CleanupStack::PushL(icon);
                     iResources.IconProvider().LoadIconL(*icon, Uid());
                     CleanupStack::Pop(icon);
                     iThumbnail = icon;)
        }
    return iThumbnail ? iThumbnail->Handle() : KErrNotFound;
    }
// --------------------------------------------------------------------------
TTime CTsRunningAppEntry::Timestamp() const
    {
    return iTimestamp; 
    }

// --------------------------------------------------------------------------
TTime CTsRunningAppEntry::TimestampUpdate() const
    {
    return iUpdateTimestamp;
    }

// --------------------------------------------------------------------------
TTsEntryKey CTsRunningAppEntry::Key() const
    {
    return iKey;
    }

// --------------------------------------------------------------------------
TBool CTsRunningAppEntry::IsActive() const
    {
    return true;
    }

// --------------------------------------------------------------------------
TBool CTsRunningAppEntry::IsClosable() const
    {
    return !iIsSystem;
    }

// --------------------------------------------------------------------------
TBool CTsRunningAppEntry::Close() const
    {
    TApaTask task( iResources.WsSession() );
    task.SetWgId( WindowGroupId() );
    task.EndTask();
    return ETrue;
    }

// --------------------------------------------------------------------------
TBool CTsRunningAppEntry::Launch() const
    {
    TApaTask task = TApaTaskList( iResources.WsSession() ).FindApp( iUid );
    task.BringToForeground();
    return task.Exists();
    }

// --------------------------------------------------------------------------
TBool CTsRunningAppEntry::IsMandatory() const
    {
    return ETrue;
    }

// --------------------------------------------------------------------------
void CTsRunningAppEntry::RefreshTimestamp()
    {
    iTimestamp.UniversalTime();
    RefreshUpdateTimestamp();
    }

// --------------------------------------------------------------------------
/**
 * Set new value of updates timestamp
 */
void CTsRunningAppEntry::RefreshUpdateTimestamp()
    {
    iUpdateTimestamp.UniversalTime();
    }

// --------------------------------------------------------------------------
void CTsRunningAppEntry::SetCloseableApp(TBool aClosable)
    {
    iIsSystem = !aClosable;
    }

// --------------------------------------------------------------------------
// CTsFswEntry::SetScreenshot
// --------------------------------------------------------------------------
//
void CTsRunningAppEntry::SetScreenshotL(const CFbsBitmap &bitmapArg, UpdatePriority priority, TInt angle)
    {
    if(Low == priority )
        {
        RefreshTimestamp();
        }
    if( priority < iPriority )
        {
        User::Leave(KErrAccessDenied);
        }
    
    CFbsBitmap* bitmap = new (ELeave) CFbsBitmap;
    CleanupStack::PushL(bitmap);
    User::LeaveIfError(bitmap->Duplicate(bitmapArg.Handle()));
    CleanupStack::Pop(bitmap);
    
    iPriority = priority;
    delete iThumbnail;
    iThumbnail = bitmap;
    RefreshUpdateTimestamp();
    
    iThumbnailProvider->createThumbnail( *iThumbnail, angle);
    }

// --------------------------------------------------------------------------
// CTsFswEntry::RemoveScreenshot
// --------------------------------------------------------------------------
//
void CTsRunningAppEntry::RemoveScreenshotL()
    {
    if (!iThumbnail) {
        User::Leave(KErrNotFound);
    }
    delete iThumbnail;
    iThumbnail = NULL;
    iPriority = Low;
    
    iObserver.DataChanged();
    RefreshUpdateTimestamp();
    }

// --------------------------------------------------------------------------
/**
 * Interface implementation
 * @see MTsThumbnailObserver::ThumbnailCreated( const CFbsBitmap& )
 */
void CTsRunningAppEntry::ThumbnailCreated(const CFbsBitmap& aThumbnail)
    {
    iThumbnail->Reset();
    iThumbnail->Duplicate(aThumbnail.Handle());
    RefreshUpdateTimestamp();
    iObserver.DataChanged();
    }

// end of file
