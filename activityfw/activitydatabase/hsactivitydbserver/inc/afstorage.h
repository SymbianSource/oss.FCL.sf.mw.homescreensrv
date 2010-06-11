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
#ifndef AFSTORAGE_H
#define AFSTORAGE_H

#define __E32SVR_H__

#include <e32std.h>
#include <e32base.h>
#include <d32dbms.h>    // RDbStoreDatabase
#include <s32file.h>    // CFileStore & CPermanentFileStore
#include "afentry.h"

class CAfStorage : public CBase
    {
public:
    ~CAfStorage();

    static CAfStorage* NewL(RFs& session);

    void AddActivityL(CAfEntry &entry);
    
    void UpdateActivityL(CAfEntry &entry);

    void DeleteActivityL(CAfEntry &entry);

    void DeleteActivitiesL(CAfEntry &entry);

    void ActivitiesL(RPointerArray<CAfEntry> &dst);

    void ActivitiesL(RPointerArray<CAfEntry> &dst, TInt appId);
    
    void ActivityL(CAfEntry *&dst, CAfEntry &src);

    RFs& Fs();

private:
    CAfStorage(RFs& session);

    void ConstructL();

    void CreateDbL(const TDesC& databaseFile);

    void OpenDbL(const TDesC& databaseFile);

    void CreateTableL();

    void DeleteNonPersistentActivitiesL();

    void GetActivitiesL(const TDesC& dst);

    HBufC* SelectRowLC(TInt appId, const TDesC& actId) const;

    HBufC* SelectRowsLC(TInt appId) const;

    HBufC* DeleteRowLC(TInt appId, const TDesC& actId) const;
    
    HBufC* DeleteRowsLC(TInt appId) const;

    HBufC* BuildQueryLC(const TDesC& format, TInt appId, const TDesC& actId) const;
    
    void ActivitiesL(RPointerArray<CAfEntry>& dst, 
                     const TDesC& query, 
                     CAfEntry::AccessRights rights,
                     TInt limit = 0);

    void ActivitiesL(RPointerArray<CAfEntry>& dst, 
                     RDbView& query, 
                     CAfEntry::AccessRights rights,
                     TInt limit = 0);

    void GetActivityForUpdateL(RDbView& query, TInt appId, const TDesC& actId);

    void ReadDataL(RBuf& dst, RDbRowSet& src, TInt offset) const;

    void ExternalizeDataL(RDbRowSet &dst,const CAfEntry & src, TInt offset) const;

    void InternalizeDataL(CAfEntry &dst, RDbRowSet& src, TInt offset) const;

private:
    RFs& mFsSession;
    RDbStoreDatabase mActDb;/* For database operations                 */
    CFileStore* mFileStore;  /* For creating and opening database files */
};

#endif //AFSTORAGE_H
