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

#ifndef HSACTIVITYSERVER_H
#define HSACTIVITYSERVER_H

#include "hsactivitydbclientinterface.h"

class HsActivityServerPrivate;

/**
 * Class implemets server functionality to store and retrieve activity
 */
class HsActivityServer
{
public:
    /**
     * Constructor
     * @param storage - reference to initialized activity storage
     */
    HsActivityServer(HsActivityDbClientInterface &storage);

    /**
     * Destructor
     */
    ~HsActivityServer();

    /**
     * Function establish server and initialize listening proces
     * @return true on succees, false otherwise
     */
    bool start();
private:
    /**
     * Private server implementation
     */
    HsActivityServerPrivate *d_ptr;
};

#endif //HSACTIVITYSERVER_H
