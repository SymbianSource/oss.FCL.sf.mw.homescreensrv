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
#ifndef ACTIVITYSTORAGE_H
#define ACTIVITYSTORAGE_H

#define __E32SVR_H__

#include <e32std.h>
#include <e32base.h>
#include <d32dbms.h>    // RDbStoreDatabase
#include <s32file.h>    // CFileStore & CPermanentFileStore

/**
 *  CActivityStorage
 * 
 */
class CActivityStorage : public CBase
    {
public:
    /**
     * Destructor.
     */
    ~CActivityStorage();

    /**
     * Two-phased constructor.
     * @param session - initialized session to file system
     */
    static CActivityStorage* NewL(RFs& session);

    /**
     * Register new activity
     * @param appId - application id
     * @param actId - activity id
     * @param data - activity data
     */
    void AddActivityL(TInt appId,const TDesC8& actId, const TDesC8& data);

    /**
     * Update activity
     * @param appId - application id
     * @param actId - activity id
     * @param data - activity data
     */
    void UpdateActivityL(TInt appId,const TDesC8& actId, const TDesC8& data);

    /**
     * Delete activity
     * @param appId - application id
     * @param actId - activity id
     */
    void DeleteActivityL(TInt appId,const TDesC8& actId);

    /**
     * Delete activity
     * @param appId - application id
     */
    void DeleteActivitiesL(TInt appId);

    /**
     * Serialize all activity into the buffer
     * @param dst - destination buffer
     */
    void ActivitiesL(RBuf8 &dst);

    /**
     * Serialize application activity into the buffer
     * @param dst - destination buffer
     * @param appId - application id
     */
    void ActivitiesL(RBuf8 &dst, TInt appId);

private:
    /**
     * Constructor for performing 1st stage construction
     * @param session - initialized session to file system
     */
    CActivityStorage(RFs& session);

    /**
     * EPOC default constructor for performing 2nd stage construction
     */
    void ConstructL();

    /**
     * Create database and its structure
     * @param databaseFile - database file path
     */
    void CreateDbL(const TDesC& databaseFile);

    /**
     * Open database
     * @param databaseFile - database file path
     */
    void OpenDbL(const TDesC& databaseFile);

    /**
     * Create database structure
     */
    void CreateTableL();

    /**
     * Serialize activity into buffer
     * @param dst - destination buffer
     */
    void GetActivitiesL(const TDesC& dst);

    /**
     * Format query to select activity row
     * @param appId - application id
     * @param actId - activity id
     * @return formated sql query
     */
    HBufC* SelectRowLC(TInt appId, const TDesC8& actId) const;
    
    /**
     * Format query to select activities for application
     * @param appId - application id
     * @return formated sql query
     */
    HBufC* SelectRowsLC(TInt appId) const;

    /**
     * Format query to delete activity
     * @param appId - application id
     * @param actId - activity id
     * @return formated sql query
     */
    HBufC* DeleteRowLC(TInt appId, const TDesC8& actId) const;
    
    /**
     * Format query to delete activities for application
     * @param appId - application id
     * @return formated sql query
     */
    HBufC* DeleteRowsLC(TInt appId) const;

    /**
     * Format sql query
     * @format - sql format string
     * @param appId - application id
     * @param actId - activity id
     * @return formated sql query
     */
    HBufC* BuildQueryLC(const TDesC& format, TInt appId, const TDesC8& actId) const;

    /**
     * Count require buffer size to serialize activity query result
     * @param data - sql query result
     * @return reuired buffer size to seriazlie activity
     */
    TInt DataSizeL(RDbRowSet& data)const;

    /**
     * Serialize activity query result to preallocated buffer
     * @param dst - preallocated buffer
     * @param view - sql query result
     */
    void ExternalizeL(TDes8& dst, RDbRowSet& view)const;

    /**
     * Execute sql query and result serialize into buffer
     * @param dst - destination result buffer
     * @param query - sql activity query 
     */
    void ActivitiesL(RBuf8& dst, const TDesC& query);

    /**
     * Get activity for update
     * @param query - destination query result
     * @param appId - application id
     * @param actId - activity id
     */
    void GetActivityForUpdateL(RDbView& query, TInt appId, const TDesC8& actId);

private:
    RFs& mFsSession;
    RDbStoreDatabase mActDb;/* For database operations                 */
    CFileStore* mFileStore;  /* For creating and opening database files */
};

#endif // ACTIVITYSTORAGE_H
