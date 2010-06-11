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

#ifndef CTSBACKSTEPPINGWAITINGDLE_H
#define CTSBACKSTEPPINGWAITINGDLE_H

#include <e32base.h>

class RProperty;
class MTsWindowGroupsMonitor;


NONSHARABLE_CLASS(CTsBacksteppingActivation) : public CActive
    {
public:
    /**
    * Two-phased constructor.
    * @param aPriority priority for the timer.
    * @return new instance of CTeleWaitingIdle.
    */
    static CTsBacksteppingActivation* NewL(MTsWindowGroupsMonitor &monitor);
    /**
    * destructor
    */
    virtual ~CTsBacksteppingActivation();

protected:
    /**
     * @see CActive.
     */
    void RunL();
    
    /**
     * @see CActive::RunError(TInt)
     */
    TInt RunError(TInt);
    
    /**
     * @see CActive.
     */
    void DoCancel();  

private:
    /**
    * constructor.
    * @return new instance of CTeleWaitingIdle.
    */
    CTsBacksteppingActivation(MTsWindowGroupsMonitor &monitor);
    
    void ConstructL();
    
    void Subscribe();

private:
    MTsWindowGroupsMonitor &mMonitor;
    
    // Central Repository.
    RProperty mProperty;
    
    // Backstepping engine. Own
    CBase* mEngine;
    };

#endif /* CTSBACKSTEPPINGWAITINGDLE_H */
