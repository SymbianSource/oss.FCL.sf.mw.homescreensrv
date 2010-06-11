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
#include "afstorage.h"
#include "afqueries.h"
#include "afentry.h"
#include <bautils.h>
#include <s32mem.h>

_LIT(KDbName, "activity.db");
_LIT(KDbDrive, "c:");
const TInt KMaxPathLength = 256;

// -----------------------------------------------------------------------------
LOCAL_C void CleanupResetAndDestroy(TAny*  item)
{
    RPointerArray<CAfEntry> *array = static_cast< RPointerArray<CAfEntry>* >(item);
    array->ResetAndDestroy();
    array->Close();
}

// -----------------------------------------------------------------------------
/**
 * Constructor for performing 1st stage construction
 * @param session - initialized session to file system
 */
CAfStorage::CAfStorage(RFs& session)
:
mFsSession(session)
{
    // No implementation required
}

// -----------------------------------------------------------------------------
/**
 * Destructor.
 */
CAfStorage::~CAfStorage()
{
}

// -----------------------------------------------------------------------------
/**
 * Two-phased constructor.
 * @param session - initialized session to file system
 */
CAfStorage* CAfStorage::NewL(RFs& session)
{
    CAfStorage* self = new (ELeave) CAfStorage(session);
    CleanupStack::PushL(self);
    self->ConstructL();
    CleanupStack::Pop(); // self;
    return self;
}

// -----------------------------------------------------------------------------
/**
 * EPOC default constructor for performing 2nd stage construction
 */
void CAfStorage::ConstructL()
{
    RBuf path;
    CleanupClosePushL( path );
    path.CreateL(KMaxPathLength);
    User::LeaveIfError(mFsSession.PrivatePath(path ));
    path.Append(KDbName);
    path.Insert(0, KDbDrive);
    BaflUtils::EnsurePathExistsL(mFsSession, path);
    BaflUtils::FileExists(mFsSession, path) ? OpenDbL(path) : CreateDbL(path);
    CleanupStack::PopAndDestroy(&path);

    DeleteNonPersistentActivitiesL();
}

// -----------------------------------------------------------------------------
/**
 * Create database and its structure
 * @param databaseFile - database file path
 */
void CAfStorage::CreateDbL(const TDesC& databaseFile)
{
    mFileStore = CPermanentFileStore::ReplaceL(mFsSession,
                                               databaseFile,
                                               EFileRead|EFileWrite);
    mFileStore->SetTypeL(mFileStore->Layout());// Set file store type
    TStreamId id = mActDb.CreateL(mFileStore);// Create stream object
    mFileStore->SetRootL(id);// Keep database id as root of store
    mFileStore->CommitL();// Complete creation by commiting
    CreateTableL();
}

// -----------------------------------------------------------------------------
/**
 * Open database
 * @param databaseFile - database file path
 */
void CAfStorage::OpenDbL(const TDesC& databaseFile)
{
    mFileStore = CPermanentFileStore::OpenL(mFsSession,
                                            databaseFile,
                                            EFileRead|EFileWrite);
    mFileStore->SetTypeL(mFileStore->Layout()); /* Set file store type*/
    mActDb.OpenL(mFileStore,mFileStore->Root());
    CDbTableNames* tables = mActDb.TableNamesL();
    CleanupStack::PushL(tables);
    if (0 == tables->Count()) {
        CreateTableL();
    }
    CleanupStack::PopAndDestroy(tables);
}

// -----------------------------------------------------------------------------
/**
 * Create database structure
 */
void CAfStorage::CreateTableL()
{
    // Add the columns to column set
    CDbColSet* actColSet = CDbColSet::NewLC();

    TDbCol appName(KApplicationColumnName, EDbColInt64);
    appName.iAttributes = TDbCol::ENotNull;
    actColSet->AddL(appName);

    TDbCol actName(KActivityColumnName, EDbColText16);// Using default length
    actName.iAttributes = TDbCol::ENotNull;
    actColSet->AddL(actName);

    TDbCol actFlags(KFlagsColumnName, EDbColInt32);
    actFlags.iAttributes = TDbCol::ENotNull;
    actColSet->AddL(actFlags);

    actColSet->AddL(TDbCol(KDataColumnName, EDbColLongBinary));// Stream Data

    // Create the table
    User::LeaveIfError(mActDb.CreateTable(KActivityTableName,
                                         *actColSet));

    CleanupStack::PopAndDestroy(actColSet);
}

// -----------------------------------------------------------------------------
/**
 * Delete non-persistent activities
 */
void CAfStorage::DeleteNonPersistentActivitiesL()
{
    HBufC *query(BuildQueryLC(KDeleteNonPersistentActivities(), CAfEntry::Persistent, KNullDesC));
    User::LeaveIfError(mActDb.Execute(*query));
    CleanupStack::PopAndDestroy(query);
}

// -----------------------------------------------------------------------------
/**
 * Register new activity
 * @param appId - application id
 * @param actId - activity id
 * @param flags - activity flags
 * @param imgSrc - activity thumbnail source
 * @param privateData - activity private data
 * @param publicData - activity public data
 */
void CAfStorage::AddActivityL(CAfEntry& entry)
{
    //verify if row already exists
    TInt errNo(KErrNone);
    RDbView view;
    CleanupClosePushL(view);
    TRAP( errNo, GetActivityForUpdateL(view, entry.ApplicationId(), entry.ActivityId()));
    if (KErrNone == errNo) {
        User::Leave(KErrAlreadyExists);
    }
    CleanupStack::PopAndDestroy(&view);

    //write table
    RDbTable table;
    CleanupClosePushL(table);
    User::LeaveIfError(table.Open(mActDb, KActivityTableName, table.EUpdatable));
    CDbColSet *row = table.ColSetL();
    CleanupStack::PushL(row);

    table.InsertL();
    TRAP(errNo,
    table.SetColL(row->ColNo(KApplicationColumnName), TInt64(entry.ApplicationId()));
    table.SetColL(row->ColNo(KActivityColumnName), entry.ActivityId());
    table.SetColL(row->ColNo(KFlagsColumnName), entry.Flags());
    ExternalizeDataL(table, entry, row->ColNo(KDataColumnName));
    table.PutL();)
    if (KErrNone != errNo) {
        table.Cancel();
        User::Leave(errNo);
    }
    CleanupStack::PopAndDestroy(row);
    CleanupStack::PopAndDestroy(&table);
}

// -----------------------------------------------------------------------------
/**
 * Update activity
 * @param entry - activity data
 */
void CAfStorage::UpdateActivityL(CAfEntry& entry)
{
    RDbView view;
    CleanupClosePushL(view);
    GetActivityForUpdateL(view, entry.ApplicationId(), entry.ActivityId());
    view.UpdateL();
    TRAPD(errNo,
    CDbColSet* colSet = view.ColSetL();
    CleanupStack::PushL(colSet);

    view.SetColL(colSet->ColNo(KFlagsColumnName), entry.Flags());
    ExternalizeDataL(view, entry, colSet->ColNo(KDataColumnName));

    view.PutL();
    if (KErrNone != errNo) {
        view.Cancel();
        User::Leave(errNo);
    }
    CleanupStack::PopAndDestroy(colSet);)

    if (KErrNone != errNo) {
        view.Cancel();
        User::Leave(errNo);
    }
    CleanupStack::PopAndDestroy(&view);
}

// -----------------------------------------------------------------------------
/**
 * Delete activity
 * @param appId - application id
 * @param actId - activity id
 */
void CAfStorage::DeleteActivityL(CAfEntry& entry)
{
    HBufC *query(DeleteRowLC(entry.ApplicationId(), entry.ActivityId()));
    User::LeaveIfError(mActDb.Execute(*query));
    CleanupStack::PopAndDestroy(query);
}

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
//
void CAfStorage::DeleteActivitiesL(CAfEntry& entry)
{
    HBufC *query(DeleteRowsLC(entry.ApplicationId()));
    User::LeaveIfError(mActDb.Execute(*query));
    CleanupStack::PopAndDestroy(query);
}

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
//
void CAfStorage::ActivitiesL(RPointerArray<CAfEntry>& dst)
{
    ActivitiesL(dst, KSelectRows(), CAfEntry::Public);
}

// -----------------------------------------------------------------------------
/**
 * Serialize application activity into the buffer
 * @param dst - destination buffer
 * @param appId - application id
 */
void CAfStorage::ActivitiesL(RPointerArray<CAfEntry>& dst,TInt appId)
{
    HBufC *query(SelectRowsLC(appId));
    ActivitiesL(dst, *query, CAfEntry::Private);
    CleanupStack::PopAndDestroy(query);
}

// -----------------------------------------------------------------------------
/**
 * Serialize application activity into the buffer
 * @param dst - destination entry
 * @param src - condition pattern
 */
void CAfStorage::ActivityL(CAfEntry *&dst, CAfEntry& src)
{
    HBufC *query = SelectRowLC(src.ApplicationId(), src.ActivityId());
    RPointerArray<CAfEntry> array;
    CleanupStack::PushL(TCleanupItem(CleanupResetAndDestroy,&array));
    ActivitiesL(array, *query, CAfEntry::Private, 1);
    if (0 >= array.Count()) {
        User::Leave(KErrNotFound);
    }
    dst = array[0];
    array.Remove(0);
    CleanupStack::PopAndDestroy(&array);
    CleanupStack::PopAndDestroy(query);
}

// -----------------------------------------------------------------------------
/**
 * Provide initialized file system session
 * @return file system session
 */
RFs& CAfStorage::Fs()
{
    return mFsSession;
}

// -----------------------------------------------------------------------------
/**
 * Format query to select activity row
 * @param appId - application id
 * @param actId - activity id
 * @return formated sql query
 */
HBufC* CAfStorage::SelectRowLC(TInt appId, const TDesC& actId) const
{
    return BuildQueryLC(KSelectRow(),appId, actId);
}

// -----------------------------------------------------------------------------
/**
 * Format query to select activities for application
 * @param appId - application id
 * @return formated sql query
 */
HBufC* CAfStorage::SelectRowsLC(TInt appId) const
{
    return BuildQueryLC(KSelectAppRows(), appId, KNullDesC);
}

// -----------------------------------------------------------------------------
/**
 * Format query to delete activity
 * @param appId - application id
 * @param actId - activity id
 * @return formated sql query
 */
HBufC* CAfStorage::DeleteRowLC(TInt appId, const TDesC& actId) const
{
    return BuildQueryLC(KDeleteRow(),appId, actId);
}

// -----------------------------------------------------------------------------
/**
 * Format query to delete activities for application
 * @param appId - application id
 * @return formated sql query
 */
HBufC* CAfStorage::DeleteRowsLC(TInt appId) const
{
    return BuildQueryLC(KDeleteRows(),appId, KNullDesC);
}

// -----------------------------------------------------------------------------
/**
 * Format sql query
 * @format - sql format string
 * @param appId - application id
 * @param actId - activity id
 * @return formated sql query
 */
HBufC* CAfStorage::BuildQueryLC(const TDesC& format,
                                      TInt appId,
                                      const TDesC& actId) const
{
    TBuf<16> appName;
    appName.AppendNum(appId);
    RBuf actName;
    CleanupClosePushL(actName);
    actName.CreateL(actId.Length());
    actName.Copy(actId);
    HBufC* query = HBufC::NewL(format.Length() +
                               appName.Length() +
                               actName.Length() );
    query->Des().AppendFormat(format, &appName, &actName);
    CleanupStack::PopAndDestroy(&actName);
    CleanupStack::PushL(query);
    return query;
}

// -----------------------------------------------------------------------------
/**
 * Execute sql query and result serialize into buffer
 * @param dst - destination result buffer
 * @param query - sql activity query
 */
void CAfStorage::ActivitiesL(RPointerArray<CAfEntry>& dst, const TDesC& query, CAfEntry::AccessRights rights, TInt limit)
{
    RDbView view;// Create a view on the database
    CleanupClosePushL(view);
    User::LeaveIfError(view.Prepare(mActDb, TDbQuery(query), view.EReadOnly));
    User::LeaveIfError(view.EvaluateAll());
    ActivitiesL(dst, view, rights, limit);
    CleanupStack::PopAndDestroy(&view);
}

// -----------------------------------------------------------------------------
/**
 * Return view deserialisd into entries array
 * @param dst - destination result
 * @param query - view
 * @param rights - acess rights
 */
void CAfStorage::ActivitiesL(RPointerArray<CAfEntry>& dst, RDbView& src, CAfEntry::AccessRights rights, TInt limit)
{
    CDbColSet* row = src.ColSetL();
    CleanupStack::PushL(row);

    const TInt flagsOffset(row->ColNo(KFlagsColumnName)),
               applicationOffset(row->ColNo(KApplicationColumnName)),
               activityOffset(row->ColNo(KActivityColumnName)),
               dataOffset(row->ColNo(KDataColumnName));

    RBuf activityName;
    CleanupClosePushL(activityName);

    for (src.FirstL(); src.AtRow(); src.NextL()) {
        if(0 < limit && dst.Count() >= limit) {
            break;
        }
        src.GetL();
        ReadDataL(activityName, src, activityOffset);

        CAfEntry *entry = CAfEntry::NewLC(src.ColInt32(flagsOffset),
                                          src.ColInt64(applicationOffset),
                                          activityName,
                                          KNullDesC,
                                          KNullDesC8,
                                          KNullDesC8);
        if (CAfEntry::Public == rights && (entry->Flags() & CAfEntry::Invisible)) {
            CleanupStack::PopAndDestroy(entry);
            continue;
        }
        InternalizeDataL(*entry, src, dataOffset);
        
        if (CAfEntry::Public == rights || 0 >= limit) {
            entry->SetDataL(KNullDesC8(), CAfEntry::Private);
        }
        dst.AppendL(entry);
        CleanupStack::Pop(entry);
    }

    CleanupStack::PopAndDestroy(&activityName);
    CleanupStack::PopAndDestroy(row);
}

// -----------------------------------------------------------------------------
/**
 * Get activity for update
 * @param query - destination query result
 * @param appId - application id
 * @param actId - activity id
 */
void CAfStorage::GetActivityForUpdateL(RDbView& view, TInt appId, const TDesC& actId)
{
    HBufC* query(SelectRowLC(appId, actId));
    User::LeaveIfError(view.Prepare(mActDb, TDbQuery(*query), view.EUpdatable));
    CleanupStack::PopAndDestroy(query);
    User::LeaveIfError(view.EvaluateAll());
    if (!view.FirstL()) {
        User::Leave(KErrNotFound);
    }
}

// -----------------------------------------------------------------------------
void CAfStorage::ReadDataL(RBuf& dst, RDbRowSet& src, TInt offset) const
{
    const TInt length(src.ColLength(offset));
    CAfEntry::ReallocL(dst, length);
    RDbColReadStream srcStream;
    srcStream.OpenLC(src,offset);
    srcStream.ReadL(dst, src.ColLength(offset));
    CleanupStack::PopAndDestroy(&srcStream);
}

// -----------------------------------------------------------------------------
void CAfStorage::ExternalizeDataL(RDbRowSet& dst,const CAfEntry &src, TInt offset) const
{
    RDbColWriteStream dbStream;
    CleanupClosePushL(dbStream);
    dbStream.OpenL(dst, offset);
    src.ExternalizeDataOnlyL(dbStream);
    CleanupStack::PopAndDestroy(&dbStream);
}

// -----------------------------------------------------------------------------
void CAfStorage::InternalizeDataL(CAfEntry & dst, RDbRowSet& src, TInt offset) const
{
    RDbColReadStream dbStream;
    CleanupClosePushL(dbStream);
    dbStream.OpenL(src, offset);
    dst.InternalizeDataOnlyL(dbStream);
    CleanupStack::PopAndDestroy(&dbStream);
}
