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
* Description:  
*
*/

#ifndef HSCCPROVIDERCLIENT_H
#define HSCCPROVIDERCLIENT_H

// System includes
#include <e32base.h>
#include <hscontentcontrol.h>

// Forward declarations
#include "ccclientsession.h"
class MHsContentController;
class CCcSrvMsg;

/**
 *  Home screen content control server content provider client
 *
 *
 *  @code
 *
 *  @endcode
 *
 *  @lib hsccsrv.lib
 *  @since S60 v5.0
 */
class CHsCcProviderClient : public CActive, public MHsContentControl
    {
public: // Constructor and destructor

    /**
     * Two-phased constructor.
     * @param aController Controller interface 
    */
    IMPORT_C static CHsCcProviderClient* NewL(
        MHsContentController& aController );

    /**
     * Destructor
     */
    ~CHsCcProviderClient();

private: // Constructors and destructor

    /**
     * Constructor
     */
    CHsCcProviderClient(
        MHsContentController& aController );


    /** 
     * Second phase constructor 
     */
    void ConstructL();

private: // From CActive
    /**
     * 
     */
    void RunL();
    
    /**
     * 
     */
    void DoCancel();

public: // From MHsContentControl

    /**
     *
     */
    void NotifyWidgetListChanged();

    /**
     *
     */
    void NotifyViewListChanged();

    /**
     *
     */
    void NotifyAppListChanged();

private: // Functions
    /**
     * 
     */
    void WaitForApiReqL();

    /**
     * 
     */
    void HandleWidgetListReqL(
        TUint32 aSender,
        TUint32 aReceiver,
        CCcSrvMsg& aMessage );

    /**
     * 
     */
    void HandleRegisterObserverNtfL(
        TUint32 aSender,
        TUint32 aReceiver,
        CCcSrvMsg& aMessage );

    /**
     * 
     */
    void HandleUnregisterObserverNtfL(
        TUint32 aSender,
        TUint32 aReceiver,
        CCcSrvMsg& aMessage );

    /**
     * 
     */
    void HandleAddWidgetReqL(
        TUint32 aSender,
        TUint32 aReceiver,
        CCcSrvMsg& aMessage );

    /**
     * 
     */
    void HandleRemoveWidgetReqL(
        TUint32 aSender,
        TUint32 aReceiver,
        CCcSrvMsg& aMessage );

    /**
     * 
     */
    void HandleViewListReqL(
        TUint32 aSender,
        TUint32 aReceiver,
        CCcSrvMsg& aMessage );

    /**
     * 
     */
    void HandleAddViewReqL(
        TUint32 aSender,
        TUint32 aReceiver,
        CCcSrvMsg& aMessage );

    /**
     * 
     */
    void HandleRemoveViewReqL(
        TUint32 aSender,
        TUint32 aReceiver,
        CCcSrvMsg& aMessage );

    /**
     * 
     */
    void HandleActivateViewReqL(
        TUint32 aSender,
        TUint32 aReceiver,
        CCcSrvMsg& aMessage );

    /**
     * 
     */
    void HandleActiveViewReqL(
        TUint32 aSender,
        TUint32 aReceiver,
        CCcSrvMsg& aMessage );

    /**
     * 
     */
    void HandleAppListReqL(
        TUint32 aSender,
        TUint32 aReceiver,
        CCcSrvMsg& aMessage );

    /**
     * 
     */
    void HandleActivateAppReqL(
        TUint32 aSender,
        TUint32 aReceiver,
        CCcSrvMsg& aMessage );

    /**
     * 
     */
    void HandleActiveAppReqL(
        TUint32 aSender,
        TUint32 aReceiver,
        CCcSrvMsg& aMessage );

    /**
     * 
     */
    void HandleNotSupportedReqL(
        TUint32 aSender,
        TUint32 aReceiver,
        CCcSrvMsg& aMessage );

    /**
     * 
     */
    void SendNtfL(
        TUint32 aNtf );

    /**
     * 
     */
    void SendRespL(
        TUint32 aSender,
        TUint32 aReceiver,
        CCcSrvMsg& aMessage );

private: // Data
    /**
     * Session to Homescreen content control server
     */
    RCcClientSession iSession;

    /**
     * Homescreen content controller interface
     */
    MHsContentController& iController;

    /**
     * Registered address
     */
    TUint32 iAddress;

    /**
     * Sender of Api request
     */
    TPckgBuf<TUint32> iPckgSender;

    /**
     * receiver of Api request
     */
    TPckgBuf<TUint32> iPckgReceiver;

    /**
     * Received API request header buffer
     */
    HBufC8* iApiHeader;

    /**
     * Pointer to header buffer
     */
    TPtr8 iApiHeaderPtr;
    
    /**
     * Received API request data buffer
     */
    HBufC8* iApiData;
    
    /**
     * Pointer to data buffer
     */
    TPtr8 iApiDataPtr;

    /**
     * Observers
     */
    RArray<TUint32> iObservers;

    };


#endif // HSCCPROVIDERCLIENT_H

// End of file
