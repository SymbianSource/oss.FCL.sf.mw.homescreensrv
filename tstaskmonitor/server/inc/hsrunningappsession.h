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
#ifndef RUNNINGAPPSESSION_H
#define RUNNINGAPPSESSION_H

// INCLUDES
#include <e32std.h>
#include <e32base.h>
#include "tsdataobserver.h"
#include "hsdataprovider.h"
#include "tsdatastorage.h"
/**
 *  CActivitySession
 * 
 */
class CRunningAppSession : public CSession2
{
public:
    // Constructors and destructor

    /**
     * Destructor.
     */
    ~CRunningAppSession();

    /**
     * Two-phased constructor.
     * @param fileSession - initialized file system session
     * @param taskStorage - global observers storage
     * @param storage - data storage
     */
    static CRunningAppSession* NewL(MHsDataObserverStorage& observerStorage, 
                                    const MHsDataProvider& dataProvider,
                                    MTsDataStorage& dataStorage);

private:

    /**
     * Constructor for performing 1st stage construction
     */
    CRunningAppSession(MHsDataObserverStorage& observerStorage, 
                       const MHsDataProvider& dataProvider,
                       MTsDataStorage& dataStorage);

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
    MHsDataObserverStorage& mObserverStorage;
    const MHsDataProvider& mDataProvider;
    MTsDataStorage& mDataStorage;
};

#endif // RUNNINGAPPSESSION_H
