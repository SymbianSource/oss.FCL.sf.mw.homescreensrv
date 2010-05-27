/*
* Copyright (c) 2008 Nokia Corporation and/or its subsidiary(-ies).
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
* Description:  Monitor for server
 *
*/


#ifndef TSFSWMONITOR_H
#define TSFSWMONITOR_H
#define __E32SVR_H__

#include <e32event.h>
#include <e32debug.h>
#include <w32std.h>

class MTsFswTaskListObserver;
class MTsFswResourceObserver;

/**
 * Monitor for the fsw server.
 * Reimplements HandleWsEventL from base class to get notifications about
 * window group changes.
 */
class CTsFswMonitor : public CActive
    {
public:
    ~CTsFswMonitor();
    
    

static CTsFswMonitor* NewL(MTsFswTaskListObserver& aTaskListObserver);
static CTsFswMonitor* NewLC(MTsFswTaskListObserver& aTaskListObserver);

protected:
    CTsFswMonitor(MTsFswTaskListObserver& aTaskListObserver);
    void ConstructL();
    
    // from CActive
    void RunL();
    TInt RunError( TInt aError );
    void DoCancel();


private:
    void Subscribe();        
    void ConstractObserverL();
private:
    MTsFswTaskListObserver* iTaskListObserver; // not own
    
    // window server session
    RWsSession iWsSession;
    /**
       * Window group.
       */
   RWindowGroup iWg;
    };

#endif //TSFSWMONITOR_H
