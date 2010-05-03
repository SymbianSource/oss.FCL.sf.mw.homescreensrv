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

#ifndef HSACTIVITYSERVERPRIVATE_H
#define HSACTIVITYSERVERPRIVATE_H

#include <e32base.h>
#include "hsactivitydbclientinterface.h"

/**
 * Class implemets server functionality for S60 enviroment
 */
class HsActivityServerPrivate : public CServer2
{
public:
    /**
     * Constructor
     */
    HsActivityServerPrivate(HsActivityDbClientInterface &storage);

    /**
     * Destructor
     */
    ~HsActivityServerPrivate();

    /**
     * Function establish server and initialize listening proces
     * @return true on succees, false otherwise
     */
    bool start();

    /**
     * Function look for active application session and complete pending message
     */
    void notifyL(int applicationId, const QString &activityName);

    /**
     * Cancel notification
     */
    void cancelNotify(int applicationId);

    /**
     * Function add item to pending messages
     */
    void waitNotification(int applicationId, const RMessage2 &msg);
private:
    /**
     * Function create new client session to handle its request
     * @param version - client implementation version
     * @param message - request message
     */
    CSession2 *NewSessionL(const TVersion &version,const RMessage2 &message)const;

private:
    /**
     * Reference to initialized storage client
     */
    HsActivityDbClientInterface &mStorage;

    QHash<int,RMessage2> mPendingMessage;
};

#endif //HSACTIVITYSERVERPRIVATE_H
