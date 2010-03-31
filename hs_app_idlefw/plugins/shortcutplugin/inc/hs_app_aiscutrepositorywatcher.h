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
* Description:  Shortcut definition parser.
*
*/


#ifndef AISCUTREPOSITORYWATCHER_H
#define AISCUTREPOSITORYWATCHER_H

// INCLUDES
#include <e32base.h>
#include <cenrepnotifyhandler.h>        // For CCenRepNotifyHandler

// CLASS DECLARATIONS

class CAiScutRepositoryWatcher : public CBase, public MCenRepNotifyHandlerCallback
{
public:
    static CAiScutRepositoryWatcher* NewL(
        const TUid aUid,
        const TUint32 aKey,
        CCenRepNotifyHandler::TCenRepKeyType aKeyType,
        TCallBack aCallBack,
        CRepository* aRepository);

    static CAiScutRepositoryWatcher* NewL(
        const TUid aUid,
        TCallBack aCallBack,
        CRepository* aRepository);

    ~CAiScutRepositoryWatcher();

    void StartListeningL();

    void StopListening();

    TUint32 ChangedKey();

public: // from MCenRepNotifyHandlerCallback
    void HandleNotifyInt    (TUint32 aKey, TInt aNewValue);
    void HandleNotifyString (TUint32 aKey, const TDesC16& aNewValue);
    void HandleNotifyGeneric(TUint32 aKey);
    void HandleNotifyError  (TUint32 aKey, TInt aError, CCenRepNotifyHandler* aHandler);

private:
    CAiScutRepositoryWatcher(
        const TUid aUid,
        const TUint32 aKey,
        TCallBack aCallBack,
        CRepository* aRepository);

    void ConstructL(CCenRepNotifyHandler::TCenRepKeyType aKeyType);

    void ConstructL();

private:
    TUid                    iUid;
    TUint32                 iKey;
    TUint32                 iChangedKey;
    TCallBack               iCallBack;
    CRepository*            iRepository;
    CCenRepNotifyHandler*   iNotifyHandler;

};

#endif // AISCUTREPOSITORYWATCHER_H

// End of File
