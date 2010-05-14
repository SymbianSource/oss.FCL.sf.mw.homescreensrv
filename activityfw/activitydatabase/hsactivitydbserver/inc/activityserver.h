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
#ifndef ACTIVITYSERVER_H
#define ACTIVITYSERVER_H

#define __E32SVR_H__

// INCLUDES
#include <e32std.h>
#include <e32base.h>
#include <f32file.h>    // RFs

#include "activitytaskstorage.h"

class CActivityStorage;
/**
 *  CActivityServer
 * 
 */
class CActivityServer : public CServer2,
                        public MActivityTaskStorage
{
public:
    /**
     * Destructor.
     */
    ~CActivityServer();

    /**
     * Two-phased constructor.
     */
    static CActivityServer* NewLC();

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
     */
    CActivityServer();

    /**
     * Default constructor for performing 2nd stage construction
     */
    void ConstructL();

    /**
     * Interface implementation
     * @see CServer2::NewSessionL(const TVersion&, const RMessage2&)
     */
    CSession2* NewSessionL(const TVersion& version, const RMessage2& message) const;

private:
    RFs mFsSession;
    CActivityStorage* mStorage;
    RPointerArray<CActivityTask> mObservers;
};

#endif // ACTIVITYSERVER_H
