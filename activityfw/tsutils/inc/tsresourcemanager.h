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
 * Description :
 *
 */
#ifndef TSRESOURCEMANAGER_H
#define TSRESOURCEMANAGER_H

#ifndef __E32SVR_H__
#define __E32SVR_H__
#endif
#include <e32base.h>
#include <e32event.h>
#include <w32std.h>
#include <apgcli.h>

/**
 * Interface decalre methods to access initialized OS resources
 */
class MTsResourceManager
{
public:
    /**
     * Access to initialized window server session
     */
    virtual RWsSession& WsSession() =0;
    
    /**
     * Access to initilaized APA session
     */
    virtual RApaLsSession& ApaSession() =0;
};

/**
 * Resource manager implemetatioin
 */
class CTsResourceManager: public CBase,
                          public MTsResourceManager
{
public:
    /**
     * Two phase constructor
     */
    IMPORT_C static CTsResourceManager* NewL();
    
    /**
     * Destructor
     */
    IMPORT_C ~CTsResourceManager();
    
    /**
     * @see MTsResourceManager::WsSession
     */
    IMPORT_C RWsSession& WsSession();
    
    /**
     * @see MTsResourceManager::ApaSession
     */
    IMPORT_C RApaLsSession& ApaSession();

private:
    /**
     * Second phase constructor
     */
    void ConstructL();

private:
   RWsSession mWsSession;
   RApaLsSession mApaSeesion;
};



#endif // TSRESOURCEMANAGER_H
