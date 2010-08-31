/*
* Copyright (c) 2009 Nokia Corporation and/or its subsidiary(-ies).
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
* Description:
*
*/
#include "afapplicationsstorage.h"
//------------------------------------------------------------------------------
CAfApplicationsStorage* CAfApplicationsStorage::NewL(CAfStorage& storage, 
                                                     const MAfApplicationsRegistry& provider)
{
    CAfApplicationsStorage *self = new (ELeave)CAfApplicationsStorage(storage, provider);
    return self;
    
}

//------------------------------------------------------------------------------
CAfApplicationsStorage::CAfApplicationsStorage(CAfStorage& storage, 
                                               const MAfApplicationsRegistry& provider)
:
    mStorage(storage),
    mProvider(provider)
{
}

//------------------------------------------------------------------------------
CAfApplicationsStorage::~CAfApplicationsStorage()
{}

//------------------------------------------------------------------------------
void CAfApplicationsStorage::applicationsChanged()
{
    const TArray<TUid> removedApp(mProvider.removedApplications());
    for (TInt iter(0); iter < removedApp.Count(); ++iter) {
        TRAP_IGNORE(deleteActivityL(removedApp[iter]));
    }
}

//------------------------------------------------------------------------------
void CAfApplicationsStorage::deleteActivityL(TUid appId)
{
    CAfEntry *entry(CAfEntry::NewLC(0, 
                                    static_cast<TInt>(appId.iUid),
                                    KNullDesC,
                                    KNullDesC,
                                    KNullDesC8,
                                    KNullDesC8));
    mStorage.DeleteActivitiesL(*entry);
    CleanupStack::PopAndDestroy(entry);
}

