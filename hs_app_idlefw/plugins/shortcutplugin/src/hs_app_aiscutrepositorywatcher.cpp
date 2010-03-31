/*
* Copyright (c) 2005-2006 Nokia Corporation and/or its subsidiary(-ies).
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
* Description:  Shortcut definition parser
*
*/


#include "hs_app_aiscutrepositorywatcher.h"
#include <centralrepository.h>      // For CRepository



CAiScutRepositoryWatcher* CAiScutRepositoryWatcher::NewL(
    const TUid aUid,
    const TUint32 aKey,
    CCenRepNotifyHandler::TCenRepKeyType aKeyType,
    TCallBack aCallBack,
    CRepository* aRepository)
{
    CAiScutRepositoryWatcher* self =
        new (ELeave) CAiScutRepositoryWatcher(aUid, aKey, aCallBack, aRepository);

    CleanupStack::PushL(self);
    self->ConstructL(aKeyType);
    CleanupStack::Pop(self);

    return self;
}

CAiScutRepositoryWatcher* CAiScutRepositoryWatcher::NewL(
    const TUid aUid,
    TCallBack aCallBack,
    CRepository* aRepository)
{
    CAiScutRepositoryWatcher* self =
        new (ELeave) CAiScutRepositoryWatcher(
            aUid,
            NCentralRepositoryConstants::KInvalidNotificationId,
            aCallBack,
            aRepository);

    CleanupStack::PushL(self);
    self->ConstructL();
    CleanupStack::Pop(self);

    return self;
}

CAiScutRepositoryWatcher::~CAiScutRepositoryWatcher()
{
    if (iNotifyHandler)
    {
        iNotifyHandler->StopListening();
        delete iNotifyHandler;
    }
}

CAiScutRepositoryWatcher::CAiScutRepositoryWatcher(
    const TUid aUid,
    const TUint32 aKey,
    TCallBack aCallBack,
    CRepository* aRepository)
:
iUid(aUid), iKey(aKey), iCallBack(aCallBack), iRepository(aRepository)
{
}

void CAiScutRepositoryWatcher::ConstructL(
    CCenRepNotifyHandler::TCenRepKeyType aKeyType)
{
    iNotifyHandler = CCenRepNotifyHandler::NewL(
        *this, *iRepository, aKeyType, iKey);
}

void CAiScutRepositoryWatcher::ConstructL()
{
    iNotifyHandler = CCenRepNotifyHandler::NewL(*this, *iRepository);
}

void CAiScutRepositoryWatcher::StartListeningL()
{
    if (iNotifyHandler)
    {
        iNotifyHandler->StartListeningL();
    }
}

void CAiScutRepositoryWatcher::StopListening()
{
    if (iNotifyHandler)
    {
        iNotifyHandler->StopListening();
    }
}

TUint32 CAiScutRepositoryWatcher::ChangedKey()
{
    return iChangedKey;
}

void CAiScutRepositoryWatcher::HandleNotifyInt(TUint32 aKey, TInt /*aNewValue*/)
{
    iChangedKey = aKey;
    iCallBack.CallBack();
    iChangedKey = NCentralRepositoryConstants::KInvalidNotificationId;
}

void CAiScutRepositoryWatcher::HandleNotifyString(
    TUint32 aKey, const TDesC16& /*aNewValue*/)
{
    iChangedKey = aKey;
    iCallBack.CallBack();
    iChangedKey = NCentralRepositoryConstants::KInvalidNotificationId;
}

void CAiScutRepositoryWatcher::HandleNotifyGeneric(TUint32 aKey)
{
    iChangedKey = aKey;
    iCallBack.CallBack();
    iChangedKey = NCentralRepositoryConstants::KInvalidNotificationId;
}

void CAiScutRepositoryWatcher::HandleNotifyError(
    TUint32 /*aKey*/, TInt /*aError*/, CCenRepNotifyHandler* /*aHandler*/)
{
}

// End of File
