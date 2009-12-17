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
* Description:  Defination of class HspsServiceUtilities
*
*/


#ifndef _HSPSSERVICEUTILITIES_H
#define _HSPSSERVICEUTILITIES_H


// SYSTEM INCLUDES
#include <f32file.h>
#include <e32base.h>
#include <hspsthememanagement.h>

// USER INCLUDES

/**
 * request Notification parameters
 */
struct THspsParamsRequestNotification
    {
    TInt event;                     // event 
    TInt appUid;                    // uid of the application
    TInt appConfUid;                // uid of application configuration
    TInt pluginUid;                  // uid of the plugin configuration to be added
    TInt pluginId;                   // Plugin Id
    TInt parentPluginId;             // Id of the plugins parent plugin
    TBuf8<KMaxFileName> name;        // name of the plugin
    };
// FORWARD DECLARATIONS

 /**
  * @ingroup group_hsps_sapi 
  * This class provides common functionalties for HSPS services 
  */
class HspsServiceUtilities
    {
    public: // Static method

        /**
         * Converts a hex value string (0xXXXX) to TInt
         * @param aStr String containing hex value
         * @return corresponding TInt
         */
        static TInt HexString2IntL(
            const TDesC8& aStr );

        /**
           * Converts a decimal value string to TInt
           * @param aStr String containing hex value
           * @return corresponding TInt
           */
          static TInt DecString2Int(
              const TDesC8& aStr );

        /**
         * Converts a configuration type string to the corresponding
         * configuration type enumeration.
         * @param aTypeStr Configuration type string
         * @param aType
         */
        static void GetConfigurationTypeL(
            const TDesC8& aTypeStr,
            TUint& aType );

        /**
         * Converts a configuration state string to the corresponding
         * configuration state enumeration.
         * @param aStateStr Configuration state string
         * @param aState
         */
        static void GetConfigurationStateL(
            const TDesC8& aStateStr,
            ThspsConfigurationState& aState );

        /**
         * Converts a configuration state filter string to the corresponding
         * configuration state filter enumeration.
         * @param aFilterStr Configuration state filter
         * @param aFilter
         */
        static void GetConfigurationStateFilterL(
            const TDesC8& aFilterStr,
            ThspsConfStateChangeFilter& aFilter );

    };

#endif // C_HSPSSERVICEUTILITIES_H

// End of File.
