/*
* Copyright (c) 2007 Nokia Corporation and/or its subsidiary(-ies).
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



#ifndef GSWATCHDOG_INTERFACE_H
#define GSWATCHDOG_INTERFACE_H

// System includes
#include <e32base.h>

// CONSTANTS

// CLASS DECLARATION

/**
*  MGSWatchDog is responsible for quarantining plugins and updating blacklist
*  for crashing GS plugins. This is the class responsible for keeping GS
*  application functional even if some plugin to panics all the time.
*
*  This is the interface for GSWatchDog. Client has no need to know the actual
*  implementation as it is owned by GS application.
*
*  @since Series60_3.2
*/
class MGSWatchDog
    {
    public: // New

        /**
        * Adds plugin to quarantine. In case GS crashes, quarantined plugin
        * will be blacklisted so it is not loaded in next GS run. Same plugin
        * can be quarantined multiple times or even have several uid instances
        * in quarantine if needed as long as all these are removed
        * correspondingly.
        *
        * NOTE: Plugin must ALWAYS be removed from quarantine after plugin's
        * function execution.
        * 1. QuarantineL().
        * 2. Call quarantined plugin function.
        * 3. RemoveFromQuarantineL().
        *
        * @param aPluginUid uid of the quarantined plugin.
        */
        virtual void QuarantineL( TUid aPluginUid ) = 0;

        /**
        * Removes plugin from quarantine. In case plugin is not removed from
        * quarantine before GS exits, the plugin will be blacklisted during
        * next GS startup.
        *
        * @param aPluginUid uid of the plugin to be removed from quarantine.
        */
        virtual void RemoveFromQuarantineL( TUid aPluginUid ) = 0;

        /**
        * @return ETrue if GS exited cleanly last time.
        *         EFalse if GS crashed during last execution.
        */
        virtual TBool WasCleanExitL() = 0;

        /**
        * This must be called in all the cases GS exits normally. If GS panics,
        * this will not be called resulting in banning of all quarantined
        * plugins.Increments iRunsAfterCrash counter for blacklisted plugins.
        * Sets iGSAppRunning to EFalse.
        */
        virtual void ReportCleanExitL() = 0;

        /**
        * Checks if plugin is in blacklist. This can be used to determine if
        * plugin should be loaded.
        *
        * @return ETrue in case plugin is in blacklist and should not be loaded.
        *         EFalse in case plugin should not be loaded
        */
        virtual TBool IsInBlackList( TUid aPluginUid ) = 0;

    };

#endif // GSWATCHDOG_INTERFACE_H
// End of File
