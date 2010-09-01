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
* Description:  Defination of class CHspsConfigurationService
*
*/


#ifndef C_HSPSCONFIGURATIONSERVICE_H
#define C_HSPSCONFIGURATIONSERVICE_H


// SYSTEM INCLUDES

#include <e32base.h>
#include <hspsthememanagement.h>
#include <hspsrequestclient.h>
#include <hspsodt.h>
#include <hspsdomlist.h>
#include <hspsdomdocument.h>
#include <e32property.h>

// FORWARD DECLARATIONS
class ChspsDomNode;
class ChspsLogBus;
class CHspsReqNotifCallback;

class ChspsRequestNotificationParams;

/**
 * @ingroup group_hsps_sapi
 * This class provides HSPS configuration services.
 * 
 * @since S60 5.0
 */
class CHspsConfigurationService : public CBase, public MhspsClientRequestServiceObserver
    {

    public:

        /**
         * Two-phased constructor.
         * 
         * @since S60 5.0
         */
        IMPORT_C static CHspsConfigurationService* NewL();

        /**
         * Destructor.
         */
        virtual ~CHspsConfigurationService();

    private:

        /**
         * Constructor
        */
        CHspsConfigurationService();

        /**
         * Symbian Constructor
        */
        void ConstructL();


    public: // From base class MXnClientRequestServiceObserver

        /**
         * Realisation of MXnClientRequestServiceObserver Interface
         * @see MXnClientRequestServiceObserver
         * @since S60 5.0
         * @param aMessage contains a service completion message returned from
         *        the XnClientRequestHandler to its observer.
         */
        void HandlehspsRequestClientMessageL( ThspsServiceCompletedMessage aMessage,
                ChspsRequestNotificationParams& aParams);

    public: // New methods
           
    	/**
    	 * Gets Dom document.
    	 * 
    	 * @since S60 5.0
    	 * @return Dom document.
    	 */
    	IMPORT_C ChspsDomDocument& GetDOML( );	
    	
        /**
         * Returns application configuration ODT for the defined application.
         * 
         * @since S60 5.0
         * @param aAppUid Application identifier.
         */
        IMPORT_C void GetODTL( const TInt aAppUid );

        /**
         * Gets application UID of active configuration.
         * 
         * @since S60 5.0
         * @param aAppUid requested application UID.
         */
        IMPORT_C void GetAppUidL( TInt& aAppUid );

        /* 
         * Register observer to get notification from the server.
         * 
         * @since S60 5.0
         * @param aCallBack Pointer to callback instance.
         */
        IMPORT_C TInt RegisterObserverL(CHspsReqNotifCallback* aCallBack );

        /* 
         * UnRegister observer to get notification from the server.
         * 
         * @since S60 5.0
         */
        IMPORT_C void UnRegisterObserverL( );
        
        /* 
         * Set log bus. Empty implementation in product builds.
         * 
         * @since S60 5.0
         * @param aLogBus   Log bus to be set.
         */
        IMPORT_C void SetLogBus( ChspsLogBus* aLogBus );
        
        /* 
         * Invalidate stored(cached) ODT.
         * 
         * If stored(cached) ODT is invalidate
         * then stored(cached) ODT will be refreshed
         * during next call to GetDOML. 
         * 
         * This method was implemented due to
         * performance improvements.
         * 
         * @since S60 5.0
         */
        IMPORT_C void InvalidateODT();

        /**
         * Gets active application configuration family
         * 
         * @since S60 5.0
         * @param aFamily requested application configuration family
         */
        IMPORT_C void GetFamilyL( TUint32& aFamily );

    private: // Methods
        
        TInt ParseNotificationDataL(ChspsRequestNotificationParams& aParams);
        
                           
        
    private: // Data        

        /**
         * Client-side implementation of MXnMaintenanceService API-definition.
         */
        ChspsRequestClient* iHspsRequestClient;

        /**
         * ODT
         */
        ChspsODT* iHspsODT;
        /**
         * CHspsReqNotifCallback
         */
        CHspsReqNotifCallback* iCallback;
        /**
         * HSPS notification parameters
         */
        ChspsRequestNotificationParams* iNotifyParams;
        /**
         * Notified PluginId array
         */
        RArray<TInt> iPluginIds;
#ifdef HSPS_LOG_ACTIVE        
        /**
         * Log bus. not owned.
         */  
        ChspsLogBus* iLogBus;
#endif        
        /**
         * Currently loaded version of the CHspsODT instance
         */  
        TInt iODTVersion;        
        
        // For validation of the iHspsODT member
        RProperty iProperty;
    };


#endif // C_HSPSCONFIGURATIONSERVICE_H

// End of File.
