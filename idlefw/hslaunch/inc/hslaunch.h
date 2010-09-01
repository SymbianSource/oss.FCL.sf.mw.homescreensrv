/*
* Copyright (c) 1020 Nokia Corporation and/or its subsidiary(-ies).
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
* Description: application class.
*/

#ifndef __HS_LAUNCH_
#define __HS_LAUNCH_

#include <e32base.h>
#include "hsprocessmonitor.h"
#include "hsshutdownmonitor.h"

/**
 * CHsLaunch
 * 
 * application class.
 * 
 * @since S60 5.2  
 */
class CHsLaunch : public CActive,
                         MHsProcessMonitorObserver,
                         MHsShutdownMonitorObserver
    {
    public:
        /**
         * Create new launcher
         *
         * @since S60 5.2     
         */    
        static CHsLaunch* NewL();
        
        /**
         * Destructor.
         *
         * @since S60 5.2     
         */
        virtual ~CHsLaunch();

        /**
         * Set active object to be ran
         * 
         * @since S60 5.2
         */        
        void Activate();        

        /**
         * Get application return value
         * 
         * @return TInt     Application return value
         * @since S60 5.2
         */        
        TInt ApplicationReturnValue();      
        
    protected:
        /**
         * C++ constructor.
         *
         * @since S60 5.2      
         */        
        CHsLaunch();    
        
        /**
         * Symbian 2nd phase constructor
         * 
         * @since S60 5.2
         */        
        void ConstructL();
    
    private: // From CActive.
        /**
         * From CActive
         */        
        void RunL();

        /**
         * From CActive
         * 
         * @since S60 5.2
         */        
        void DoCancel(); 

        /**
         * From CActive
         * 
         * @since S60 5.2
         */        
        TInt RunError( TInt aError );

    private: // From MHsProcessMonitorObserver
        /**
         * From MHsProcessMonitorObserver
         */
        void ProcessEnded( const TExitType& aExitType,
                           const TInt aExitReason,
                           const TExitCategoryName& aExitCategory );

        /**
         * From MHsProcessMonitorObserver
         */
        void ProcessMonitoringError( TInt aError );

    private: // From MHsShutdownMonitorObserver
        /**
         * From MHsShutdownMonitorObserver
         */
        void SystemShutdownEvent();

    private:
        /**
         * Initialize process monitor
         * 
         * @param aProcessId    Id of process to monitor 
         */
        void InitProcessMonitorL( const TInt aProcessId );
        
        /**
         * Shut down application 
         * 
         * @param aApplicationReturnValue   Application return value.
         */
        void ShutdownApp( const TInt aApplicationReturnValue );         
        
    private: // data.
        // Process monitor. Owned.
        CHsProcessMonitor* iProcessMonitor;
        
        // Shutdown monitor. Owned.
        CHsShutdownMonitor* iShutdownMonitor;
        
        // Application return value
        TInt iApplicationReturnValue;
    };

#endif // __HS_LAUNCH_
