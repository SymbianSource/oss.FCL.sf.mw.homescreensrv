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
#include "activitystorage.h"
#include "activityqueries.h"
#include <bautils.h>
#include <s32mem.h>

_LIT(KDbName, "activity.db");
_LIT(KDbDrive, "c:");
const TInt KMaxPathLength = 256;

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
//
CActivityStorage::CActivityStorage(RFs& session)
:
mFsSession(session)
{
    // No implementation required
}

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
//
CActivityStorage::~CActivityStorage()
{
}

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
//
CActivityStorage* CActivityStorage::NewL(RFs& session)
{
    CActivityStorage* self = new (ELeave) CActivityStorage(session);
    CleanupStack::PushL(self);
    self->ConstructL();
    CleanupStack::Pop(); // self;
    return self;
}

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
//
void CActivityStorage::ConstructL()
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
}

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
//
void CActivityStorage::CreateDbL(const TDesC& databaseFile)
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
//
// -----------------------------------------------------------------------------
//
void CActivityStorage::OpenDbL(const TDesC& databaseFile)
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
//
// -----------------------------------------------------------------------------
//
void CActivityStorage::CreateTableL()
{
   // Add the columns to column set
   CDbColSet* actColSet = CDbColSet::NewLC();
   
   TDbCol appName(KApplicationColumnName, EDbColInt64);// Using default length
   appName.iAttributes = TDbCol::ENotNull;
   actColSet->AddL(appName);
   
   TDbCol actName(KActivityColumnName, EDbColText8);// Using default length
   actName.iAttributes = TDbCol::ENotNull;
   actColSet->AddL(actName);
   
   actColSet->AddL(TDbCol(KDataColumnName, EDbColLongText8));// Stream Data
   
   // Add the columns to index definition
   CDbKey* primaryKey = CDbKey::NewLC();
   primaryKey->AddL(TDbKeyCol(KApplicationColumnName));
   primaryKey->AddL(TDbKeyCol(KActivityColumnName));
   primaryKey->MakePrimary();

   // Create the table
   /*User::LeaveIfError(mActDb.CreateTable(KActivityTableName, 
                                         *actColSet, 
                                         *primaryKey));*/
   User::LeaveIfError(mActDb.CreateTable(KActivityTableName, 
                                            *actColSet));
   /*User::LeaveIfError(mActDb.CreateIndex(KActivityIndexName, 
                                         KActivityTableName, 
                                         *primaryKey));*/
   CleanupStack::PopAndDestroy(primaryKey);
   CleanupStack::PopAndDestroy(actColSet);
}

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
//
void CActivityStorage::AddActivityL(TInt appId, 
                                    const TDesC8& actId, 
                                    const TDesC8& data)
{
    //verify if row already exists
    RDbView view;
    CleanupClosePushL(view);
    TRAPD( errNo, GetActivityForUpdateL(view, appId, actId));
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
    table.SetColL(row->ColNo(KApplicationColumnName), TInt64(appId));
    table.SetColL(row->ColNo(KActivityColumnName), actId);
    
    //write blob data
    RDbColWriteStream stream;
    CleanupClosePushL(stream);
    stream.OpenL(table, row->ColNo(KDataColumnName));
    stream.WriteL(data);
    CleanupStack::PopAndDestroy(&stream);
    
    table.PutL();
    CleanupStack::PopAndDestroy(row);
    CleanupStack::PopAndDestroy(&table);
}

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
//
void CActivityStorage::UpdateActivityL(TInt appId, 
                                       const TDesC8& actId, 
                                       const TDesC8& data)
{
    RDbView view;
    CleanupClosePushL(view);
    GetActivityForUpdateL(view, appId, actId);
    view.UpdateL();
    CDbColSet* colSet = view.ColSetL();
    CleanupStack::PushL(colSet);
    RDbColWriteStream writeStream;
    CleanupClosePushL(writeStream);
    writeStream.OpenL(view, colSet->ColNo(KDataColumnName));
    writeStream.WriteL(data);
    CleanupStack::PopAndDestroy(&writeStream);
    view.PutL();
    CleanupStack::PopAndDestroy(colSet);
    CleanupStack::PopAndDestroy(&view);
}

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
//
void CActivityStorage::DeleteActivityL(TInt appId,const TDesC8& actId) 
{
    HBufC *query(DeleteRowLC(appId, actId));
    User::LeaveIfError(mActDb.Execute(*query));
    CleanupStack::PopAndDestroy(query);
}

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
//
void CActivityStorage::DeleteActivitiesL(TInt appId)
{
    HBufC *query(DeleteRowsLC(appId));
    User::LeaveIfError(mActDb.Execute(*query));
    CleanupStack::PopAndDestroy(query);
}

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
//
void CActivityStorage::ActivitiesL(RBuf8 &dst)
{
    ActivitiesL(dst, KSelectRows() );
}

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
//
void CActivityStorage::ActivitiesL(RBuf8 &dst,TInt appId)
{
    HBufC *query(SelectRowsLC(appId));
    ActivitiesL(dst, *query);
    CleanupStack::PopAndDestroy(query);
}

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
//
HBufC* CActivityStorage::SelectRowLC(TInt appId, const TDesC8& actId) const 
{
    return BuildQueryLC(KSelectRow(),appId, actId);
}

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
//
HBufC* CActivityStorage::SelectRowsLC(TInt appId) const
{
    return BuildQueryLC(KSelectAppRows(), appId, KNullDesC8);
}

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
//
HBufC* CActivityStorage::DeleteRowLC(TInt appId, const TDesC8& actId) const
{
    return BuildQueryLC(KDeleteRow(),appId, actId);
}

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
//
HBufC* CActivityStorage::DeleteRowsLC(TInt appId) const
{
    return BuildQueryLC(KDeleteRows(),appId, KNullDesC8);
}

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
//
HBufC* CActivityStorage::BuildQueryLC(const TDesC& format, 
                                      TInt appId, 
                                      const TDesC8& actId) const
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
//
// -----------------------------------------------------------------------------
//
TInt CActivityStorage::DataSizeL(RDbRowSet& data)const
{
    TInt dataSize(sizeof(TInt));
    CDbColSet* row = data.ColSetL();
    CleanupStack::PushL(row);
    const TInt dataOffset(row->ColNo(KDataColumnName));
    for (data.FirstL(); data.AtRow(); data.NextL()) {
        data.GetL();
        dataSize += (sizeof(TInt) + 
                     data.ColLength(dataOffset));
    }
    CleanupStack::PopAndDestroy(row);
    return dataSize;
}

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
//
void CActivityStorage::ExternalizeL(TDes8& dst, RDbRowSet& src)const
{
    RDesWriteStream dstStream(dst);
    CleanupClosePushL(dstStream);
    dstStream.WriteInt32L(src.CountL());
    RBuf8 data;
    CleanupClosePushL(data);
    CDbColSet* row = src.ColSetL();
    CleanupStack::PushL(row);
    const TInt dataOffset(row->ColNo(KDataColumnName));
    RDbColReadStream srcStream;
    CleanupClosePushL(srcStream);
    for (src.FirstL(); src.AtRow(); src.NextL()) {
        src.GetL();
        if (data.MaxLength() < src.ColLength(dataOffset)) {
            data.ReAllocL(src.ColLength(dataOffset));
        }
        data.SetLength(0);
        srcStream.OpenL(src,dataOffset);
        srcStream.ReadL(data, src.ColLength(dataOffset));
        srcStream.Close();
        dstStream.WriteInt32L(src.ColLength(dataOffset));
        if (0 < src.ColLength(dataOffset)) {
            dstStream.WriteL(data, src.ColLength(dataOffset));
        }
    }
    CleanupStack::PopAndDestroy(&srcStream);
    CleanupStack::PopAndDestroy(row);
    CleanupStack::PopAndDestroy(&data);
    CleanupStack::PopAndDestroy(&dstStream);
}

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
//
void CActivityStorage::ActivitiesL(RBuf8& dst, const TDesC& query)
{
    RDbView view;// Create a view on the database
    CleanupClosePushL(view);
    User::LeaveIfError(view.Prepare(mActDb, TDbQuery(query), view.EReadOnly));
    User::LeaveIfError(view.EvaluateAll());
    const TInt dataSize(DataSizeL(view));
    if(dst.MaxLength() < dataSize) {
        dst.ReAllocL(dataSize);
    }
    ExternalizeL(dst, view);
    CleanupStack::PopAndDestroy(&view);
}

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
//
void CActivityStorage::GetActivityForUpdateL(RDbView& view, TInt appId, const TDesC8& actId)
{
    HBufC* query(SelectRowLC(appId, actId));
    User::LeaveIfError(view.Prepare(mActDb, TDbQuery(*query), view.EUpdatable));
    CleanupStack::PopAndDestroy(query);
    User::LeaveIfError(view.EvaluateAll());
    if (!view.FirstL()) {
        User::Leave(KErrNotFound);
    }
}
