/*
* Copyright (c) 2002-2004 Nokia Corporation and/or its subsidiary(-ies).
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
*  ChspsRequestClient is a client-side implementation of  MhspsClientRequestService API-definition 
*  of Homescreen Plugin Configuration Management Services. 
*  ChspsRequestClient is intended to serve Rendering Engines with Application configuartion and plugin configurations
*  and their associated resources. ChspsRequestClient will provide access to Homescreen Plugin configuration 
*  database maintained by hspsThemeServer.
*  Typical use-case in using ChspsRequestClient is that there is HSPS SAPI user with a instance of 
*  CHspsConfigurationService. When Homescreen application is starting up, the provider of teh HSPS Sapi is created
*  with instance of ChspsConfigurationService.  ChspsConfigurationService calls hspsGetODT() and retrieves application 
*  configuartion. When request exists on server-side, the right one application configuration is delivered to client process. 
*  Theme may include resources. Resourses are listed on Resource List which is delivered too by
*  the same request. hspsThemeServer uses Symbian Central Repository to get match theme on the 
*  request. ChspsRequestClient serves also resource accesses. In using file handle passing introduced 
*  by Symbian Platform Security, it deliver a resource access to the client process.
*  
*  ChspsRequestClient includes asynchronous service for subscribing the changes of current theme. 
*  These changes could be for e.g. theme status change (another theme is activated for the 
*  application) or a theme update. Change-events must be subsribed by calling hspsGetODTUpdate(). 
*  CHspsConfigurationService who was initiated ChspsRequestClient, must listen to
*  the hspsThemeServer call-back events. Call-back events are delivered through 
*  MhspsClientRequestServiceObserver::HandlehspsRequestClientMessage() function. 
*  Subscribtion can be canceled by hspsCancelGetODTUpdate() at any time. ChspsRequestClient cancels 
*  subscription automaticly on some cases, for instance, on theme status change; there is no reason 
*  to continue subscription of changes of the theme which is not active any more - host application
*  must reload the theme and then initiate the subscription again.
*
*  For more information, see HSPS Theme Management Service APIs documentation 
*  and hspsThemeManagement.h, where MhspsClientRequestService API implemented by this class are defined.
*  
*  Version     : %version: 1 %
*
*/


#ifndef __ChspsREQUESTCLIENT_H__
#define __ChspsREQUESTCLIENT_H__

#include <e32base.h>

#include "hspsthememanagement.h"
#include "hspsclientsession.h"
#include "hspsresult.h"
#include "hspsreqnotifparam.h"

#ifdef HSPS_LOG_ACTIVE
class ChspsLogBus;
#endif

/** hspsRequestClient Panic Category. */
_LIT( KhspsRequestClient, "hspsRequestClient" );

/** hspsRequestClient panic codes: */
enum ThspsRequestClientPanics
    {
    EhspsRequestClientBadState
    };

class ChspsODT;
class ChspsResource;
class RhspsClientSession;
class MhspsClientRequestService;
class MhspsClientRequestServiceObserver;
class ChspsResult;
class ChspaRequestNotificationParams;

/**
 * Request client
 *
 * @lib hspsrequestclient.lib
 * @since S60 5.0
 * @ingroup group_hspsclients
 */
class ChspsRequestClient : public CActive, public MhspsClientRequestService
    {

    public: // Constructors and destructors
        /**
        * NewL.
        * Two-phased constructor.
        * Creates a ChspsClient object using two phase construction,
        * and return a pointer to the created object.
        * 
        * @since S60 5.0
        * @param aObserver The object to be used to
        *                  handle updates from the server.
        */
        IMPORT_C static ChspsRequestClient* NewL(MhspsClientRequestServiceObserver& aObserver );

        /**
        * NewLC.
        * Two-phased constructor.
        * Creates a ChspsRequestClient object using two phase construction,
        * and return a pointer to the created object.
        * 
        * @since S60 5.0
        * @param aObserver The object to be used to
        *                  handle updates from the server.
        */
        IMPORT_C static ChspsRequestClient* NewLC( MhspsClientRequestServiceObserver& aObserver );

        /**
        * ~ChspsRequestClient.
        * 
        * Destructor.
        * Destroys the object and release all memory objects.
        */
        virtual ~ChspsRequestClient();

    public: // New functions

        /**
        * From MhspsClientRequestService.
        */
        IMPORT_C ThspsServiceCompletedMessage hspsGetODT(TInt aAppUid, ChspsODT& aODT);
        
        /**
        * From MhspsClientRequestService.
        */
        IMPORT_C ThspsServiceCompletedMessage hspsGetODTUpdate();
        
        /**
        * From MhspsClientRequestService.
        */
        IMPORT_C ThspsServiceCompletedMessage hspsCancelGetODTUpdate();   
        
        /**
        * From MhspsClientRequestService.
        */
        IMPORT_C ThspsServiceCompletedMessage hspsAccessResourceFile( 
        		const TDesC& aResourceFileName,
        		const ThspsConfiguration& aConfiguration,
                RFile& aFile);    
               
        /**
        * GethspsResult.
        * 
        * @since S60 5.0
        * @param aResult is a ChspsResult object containing additional information of query result.
        */
        IMPORT_C void GethspsResult(ChspsResult& aResult);

        /** 
        * Sets a log bus when debugging has been enabled.
        * Empty implementation if non HSPS_LOG_ACTIVE build.
        * 
        * @since S60 5.0 
        * @param aLogBus Log bus to be set.
        */
        IMPORT_C void SetLogBus( void* aLogBus );        

    protected: // Functions from base classes

        /**
        * From CActive, RunL.
        * Callback function.
        * Invoked to handle responses from the server.
        */
        void RunL();

        /**
        * From CActive, DoCancel.
        * Cancels any outstanding operation.
        */
        void DoCancel();
        
        /**
        * From CActive Called when asynchronous request has failed
        * @since S60 3.1
        */
        TInt RunError( TInt aError );
        
    protected:
        /**
        * Sends a request to the server, retrieves a file path, internalizes the file
        * and copies resources from the server to client's private folder
        * @since S60 5.0
        */
        void CallGetOdtInFileL(
        		const ThspsConfiguration& aConfiguration,
        		ChspsODT& aODT );
        
        /**
        * UpdatehspsResult. Updates iResult with latest result data coming on client request return.
        */        
        void UpdatehspsResult();
        
        /**
        * UpdatehspsReqNotifParams. Updates iReqNotifParams with latest notification data coming on client request return.
        */
        void UpdatehspsReqNotifParams();
        
    private: // Constructors and destructors

        /**
        * ChspsRequestClient.
        * Performs the first phase of two phase construction.
        * @param aObserver The object to be used to
        *                  handle updates from the server.
        */
        ChspsRequestClient( MhspsClientRequestServiceObserver& aObserver);

        /**
        * ConstructL.
        * Performs the second phase construction
        * ChspsRequestClient object.
        */
        void ConstructL();

    private: // Members
    
        /**
        * iSession, the time server session.
        */
        RhspsClientSession iSession;

        /**
        * iObserver, observer which handles updates from the server.
        */
        MhspsClientRequestServiceObserver& iObserver;    
    
    private: // Data
        TBuf8<KMaxHeaderDataLength8> iHeaderData;
        HBufC8* iElement;
        TBuf8<KMaxResultDataLength8> iResultData;
        ChspsResult* iResult;
        ChspsRequestNotificationParams* iReqNotifParams;
        TBuf8<KMaxReqNotifDataLength8> iReqNotifData;
        ChspsODT* iODT;
        TBool iODTValid;
        CArrayPtrSeg<ChspsResource>* iResourceList;          

#ifdef HSPS_LOG_ACTIVE
        // Log bus. Not owned.        
        ChspsLogBus* iLogBus;
#endif        
    };


#endif //__ChspsCLIENT_H__

// End of File
