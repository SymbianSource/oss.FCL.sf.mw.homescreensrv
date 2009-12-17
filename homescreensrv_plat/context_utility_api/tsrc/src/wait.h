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
* Description:  CWait class declaration.
*
*/



#ifndef C_WAIT_H
#define C_WAIT_H

#include <e32base.h>

/**
*  Timed active scheduler wait.
*  This class wraps inside a timer and scheduler wait.
*  Wait will be automatically stopped after specified time.
*/
NONSHARABLE_CLASS( CWait ): public CTimer
    {
    public:  // Constructors and destructor

        // Two-phased constructor.
        static CWait* NewL();
        static CWait* NewLC();
               
        // Destructor.
        virtual ~CWait();
        
    public: // New methods
    
        /**
        * Starts waiting in the scheduler.
        * Wait will be automatically stopped after aInterval amount
        * of time has passed.
        * 
        * @param aInterval Interval after wait will be stopped.
        * @return None
        */
        void Start( const TTimeIntervalMicroSeconds32& aInterval );

        /**
        * Stops scheduler wait.
        * 
        * @param None
        * @return None
        */
        void Stop();
        
    private: // From base classes
    
        // @see CActive
        void RunL();
        
        // @see CActive
        void DoCancel();
        
    private:
    
        CWait();
        void ConstructL();

    private: // Data
    
        // Own: Active scheduler wait
        CActiveSchedulerWait* iWait;
    };

#endif
