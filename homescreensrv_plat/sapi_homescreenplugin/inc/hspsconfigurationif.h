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
* Description:  Defination of class CHSPSConfigurationIf
*
*/


#ifndef C_CHSPSCONFIGURATIONIF_H
#define C_CHSPSCONFIGURATIONIF_H

#include <LiwCommon.h>
#include <LiwServiceIfBase.h>
#include <LiwBufferExtension.h>
#include "hspsconfigurationservice.h"
#include "hspspersonalisationservice.h"

//Forward declarations
class CHspsLiwUtilities;


#ifdef HSPS_LOG_ACTIVE
class ChspsLogBus;
class ChspsLiwDump;
#endif

/**
 * @ingroup group_hsps_sapi
 * This class provides the HSPS SAPI configuration interface to LIW Framework 
*/
NONSHARABLE_CLASS( CHSPSConfigurationIf ) : public CBase, public MLiwInterface
    {
    public:

        /**
         * Two-phased constructor.
         * @return CHSPSConfigurationIf* 
        */
        static CHSPSConfigurationIf* NewL( TDesC8& aAppUid );

        /**
        * Destructor.
        */
        virtual ~CHSPSConfigurationIf();

    private:

        /**
         * Constructor
        */
        CHSPSConfigurationIf();

        /**
         * Symbian Constructor
         * @param aAppUid Application identifier of a configuration.
        */
        void ConstructL( TDesC8& aAppUid );

    public: // From MLiwInterface class

        /**
         * Executes the SAPI as per params
         * @param aCmdName Command name defining which operation is to be performed
         * @param aInParamList  Input param list
         * @param aOutParamList Output list
         * @param aCmdOptions Command option
         * @param aCallback Callback object pointer
        */
        void ExecuteCmdL( 
            const TDesC8& aCmdName,
            const CLiwGenericParamList& aInParamList,
            CLiwGenericParamList& aOutParamList,
            TUint aCmdOptions = 0,
            MLiwNotifyCallback* aCallback = 0 );
        
        /**
         * Closes the interface 
        */
        void Close();

    
    private: // New functions

        /**
         * Calls SAPI method syncronously
         * @param aCmdName Command name defining which operation is to be performed
         * @param aInParamList Input param list
         * @param aOutParamList Output param list
         */   
        void SyncMethodCallL( 
            const TDesC8& aCmdName,
            const CLiwGenericParamList& aInParamList, 
            CLiwGenericParamList& aOutParamList );

        /**
         * Calls SAPI method asyncronously
         * @param aCmdName Command name defining which operation is to be performed
         * @param aInParamList Input param list
         * @param aOutParamList Output param list
         * @param aCmdOptions asynchronous sapi command options
         * @param aCallback Callback object pointer
         */   
        void AsyncMethodCallL( 
            const TDesC8& aCmdName,
            const CLiwGenericParamList& aInParamList, 
            CLiwGenericParamList& aOutParamList,
            TUint aCmdOptions,
            MLiwNotifyCallback* aCallback );

        /**
         * Returns active configuration of the application
         * @param aOutParamList Output param list
         */     
        void GetActiveAppConfL( 
            CLiwGenericParamList& aOutParamList );
        
        /**
         * Returns plugin configuration of defined plugin
         * @param aInParamList Input param list
         * @param aOutParamList Output param list
         */   
        void GetPluginConfL( 
            const CLiwGenericParamList& aInParamList, 
            CLiwGenericParamList& aOutParamList );

        /**
         * Returns list of references to installed plugins
         * @param aInParamList Input param list
         * @param aOutParamList Output param list
         */   
        void GetPluginsL( 
            const CLiwGenericParamList& aInParamList, 
            CLiwGenericParamList& aOutParamList );

        /**
         * Returns list of references to plugins in active application 
         * configuration
         * @param aInParamList Input param list
         * @param aOutParamList Output param list
         */   
        void GetPluginListL( 
            const CLiwGenericParamList& aInParamList, 
            CLiwGenericParamList& aOutParamList );
        
        /**
         * Adds a new plugin to the defined configuration
         * @param aInParamList Input param list
         * @param aOutParamList Output param list
         */   
        void AddPluginL( 
            const CLiwGenericParamList& aInParamList, 
            CLiwGenericParamList& aOutParamList );

        /**
         * Removes a plugin from the active configuration
         * @param aInParamList Input param list
         * @param aOutParamList Output param list
         */   
        void RemovePluginL( 
            const CLiwGenericParamList& aInParamList, 
            CLiwGenericParamList& aOutParamList );

        /**
         * Replaces a plugin from the active configuration
         * @param aInParamList Input param list
         * @param aOutParamList Output param list
         */
        void ReplacePluginL( 
            const CLiwGenericParamList& aInParamList, 
            CLiwGenericParamList& aOutParamList );
        
        /**
         * Sets plugin settings
         * @param aInParamList Input param list
         * @param aOutParamList Output param list
         */
        void SetPluginSettingsL( 
            const CLiwGenericParamList& aInParamList, 
            CLiwGenericParamList& aOutParamList );

        /**
         * Gets plugin settings
         * @param aInParamList Input param list
         * @param aOutParamList Output param list
         */        
        void GetPluginSettingsL( 
            const CLiwGenericParamList& aInParamList, 
            CLiwGenericParamList& aOutParamList );

        /**
         * Updates plugin positions
         * @param aInParamList Input param list
         * @param aOutParamList Output param list
         */
        void MovePluginsL(
            const CLiwGenericParamList& aInParamList, 
            CLiwGenericParamList& aOutParamList );


        /**
         * Returns list of installed application configurations
         * @param aInParamList Input param list
         * @param aOutParamList Output param list
         */   
        void GetAppConfsL( 
            const CLiwGenericParamList& aInParamList, 
            CLiwGenericParamList& aOutParamList );

        /**
         * Sets active application configuration
         * @param aInParamList Input param list
         * @param aOutParamList Output param list
         */   
        void SetActiveAppConfL( 
            const CLiwGenericParamList& aInParamList, 
            CLiwGenericParamList& aOutParamList );

        /**
          * Change Notification request
          * @param aInParamList Input param list
          * @param aCallback Callback object pointer
          * @param aTransactionId Output param for async request to store transaction id
          */
        void RequestNotificationL( const CLiwGenericParamList& aInParamList, 
                                         MLiwNotifyCallback* aCallback,
                                         TInt32& aTransactionId );

        /**
         * Updates the configuration's state
         * @param aInParamList Input param list
         * @param aOutParamList Output param list
         */   
        void SetConfStateL( 
            const CLiwGenericParamList& aInParamList, 
            CLiwGenericParamList& aOutParamList );

        /**
         * Activates a plugin in plugin list
         * @param aInParamList Input param list
         * @param aOutParamList Output param list
         */   
        void SetActivePluginL( 
            const CLiwGenericParamList& aInParamList, 
            CLiwGenericParamList& aOutParamList );

        /**
         * Sets an error value to output parameter.
         * @param aOutParamList Output param list
         * @param aErr an error
         * @return void
        */
        void SetErrorL( 
            CLiwGenericParamList& aOutParamList, 
            TInt32 aErr );
        
    private: // Data

        /**
         * CHspsConfigurationService class pointer
         */        
        CHspsConfigurationService* iHspsConfigurationService;

        /**
         * CHspsPersonalisationService class pointer
         */        
        CHspsPersonalisationService* iHspsPersonalisationService;

        /**
         * LIW handler
         */        
        CHspsLiwUtilities* iHspsLiwUtilities;
        /**
         * CHspsReqNotifCallback class pointer
         */        
                


#ifdef HSPS_LOG_ACTIVE        
        /**
         * Log bus.
         */        
        ChspsLogBus* iLogBus;
        
        /**
         * Liw dump object.
         */        
        ChspsLiwDump* iLiwDump;
#endif        
    };


#endif // C_CHSPSCONFIGURATIONIF_H
