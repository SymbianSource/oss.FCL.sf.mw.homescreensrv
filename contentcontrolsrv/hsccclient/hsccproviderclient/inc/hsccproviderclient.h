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
        CCcSrvMsg& aMessage );

    /**
     * 
     */
    void HandleRegisterObserverNtfL(
        CCcSrvMsg& aMessage );

    /**
     * 
     */
    void HandleUnregisterObserverNtfL(
        CCcSrvMsg& aMessage );

    /**
     * 
     */
    void HandleAddWidgetReqL(
        CCcSrvMsg& aMessage );

    /**
     * 
     */
    void HandleRemoveWidgetReqL(
        CCcSrvMsg& aMessage );

    /**
     * 
     */
    void HandleViewListReqL(
        CCcSrvMsg& aMessage );

    /**
     * 
     */
    void HandleAddViewReqL(
        CCcSrvMsg& aMessage );

    /**
     * 
     */
    void HandleRemoveViewReqL(
        CCcSrvMsg& aMessage );

    /**
     * 
     */
    void HandleActivateViewReqL(
        CCcSrvMsg& aMessage );

    /**
     * 
     */
    void HandleActiveViewReqL(
        CCcSrvMsg& aMessage );

    /**
     * 
     */
    void HandleAppListReqL(
        CCcSrvMsg& aMessage );

    /**
     * 
     */
    void HandleActivateAppReqL(
        CCcSrvMsg& aMessage );

    /**
     * 
     */
    void HandleActiveAppReqL(
        CCcSrvMsg& aMessage );

    /**
     * 
     */
    void HandleNotSupportedReqL(
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
