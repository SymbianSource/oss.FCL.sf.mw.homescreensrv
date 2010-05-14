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
#ifndef ACTIVITYSESSION_H
#define ACTIVITYSESSION_H

#ifndef __E32SVR_H__
#define __E32SVR_H__
#endif 
// INCLUDES
#include <e32std.h>
#include <e32base.h>
#include <f32file.h>

#include "activitytaskstorage.h"

class CActivityStorage;
/**
 *  CActivitySession
 * 
 */
class CActivitySession : public CSession2,
                         public MActivityTaskStorage
{
public:
    // Constructors and destructor

    /**
     * Destructor.
     */
    ~CActivitySession();

    /**
     * Two-phased constructor.
     * @param fileSession - initialized file system session
     * @param taskStorage - global observers storage
     * @param storage - data storage
     */
    static CActivitySession* NewL(RFs& fileSession,
                                  MActivityTaskStorage& taskStorage, 
                                  CActivityStorage&storage);

public:
    /**
     * Interface implementation
     * @see MActivityTaskStorage::PushL(CActivityTask *)
     */
    void PushL(CActivityTask *);
    
    /**
     * Interface implementation
     * @see MActivityTaskStorage::Pop(CActivityTask *)
     */
    void Pop(CActivityTask *);
    
    /**
     * Interface implementation
     * @see MActivityTaskStorage::StorageData()
     */
    const RPointerArray<CActivityTask>& StorageData() const;
private:

    /**
     * Constructor for performing 1st stage construction
     * @param fileSession - initialized file system session
     * @param taskStorage - global observers storage
     * @param storage - data storage
     */
    CActivitySession(RFs& fileSession,
                     MActivityTaskStorage& taskStorage, 
                     CActivityStorage& storage);

    /**
     * EPOC default constructor for performing 2nd stage construction
     */
    void ConstructL();

private:
    /**
     * Implements interface
     * @see void CSession2::ServiceL(const RMessage2&)
     */
    void ServiceL(const RMessage2& message);

private:
    RFs& mFileSession; 
    MActivityTaskStorage& mTasksStorage;
    CActivityStorage& mStorage;
    RPointerArray<CActivityTask> mRunningTasks;
};

#endif // ACTIVITYSESSION_H
