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

#ifndef HSCCAPICLIENT_H
#define HSCCAPICLIENT_H

// System includes
#include <e32base.h>
#include <hscontentcontroller.h>

// Forward declarations
#include "ccclientsession.h"
class MHsContentControl;

/**
 *  Home screen content control server content control api client
 *
 *
 *  @code
 *
 *  @endcode
 *
 *  @lib hsccapiclient.lib
 *  @since S60 v5.0
 */
class CHsCcApiClient : public CActive, public MHsContentController
    {
public: // Constructor and destructor

    /**
     * Two-phased constructor.
     * @param aControlIf Content control observer interface 
     */
    IMPORT_C static CHsCcApiClient* NewL(
        MHsContentControl* aControlIf );

    /**
     * Destructor
     */
    ~CHsCcApiClient();

private: // Constructors and destructor

    /**
     * Constructor
     * @param aControlIf Content control observer interface 
     */
    CHsCcApiClient(
        MHsContentControl* aControlIf );

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

public: // From MHsContentController
    /**
     * See from API documentation
     */
    TInt WidgetListL( CHsContentInfoArray& aArray );

    /**
     * See from API documentation
     */
    TInt WidgetListL( CHsContentInfo& aInfo, CHsContentInfoArray& aArray );

    /**
     * See from API documentation
     */
    TInt ViewListL( CHsContentInfoArray& aArray );

    /**
     * See from API documentation
     */
    TInt ViewListL( CHsContentInfo& aInfo, CHsContentInfoArray& aArray );

    /**
     * See from API documentation
     */
    TInt AppListL( CHsContentInfoArray& aArray );
    
    /**
     * See from API documentation
     */
    TInt AddWidgetL( CHsContentInfo& aInfo );
    
    /**
     * See from API documentation
     */
    TInt RemoveWidgetL( CHsContentInfo& aInfo );
    
    /**
     * See from API documentation
     */
    TInt AddViewL( CHsContentInfo& aInfo );
    
    /**
     * See from API documentation
     */
    TInt RemoveViewL( CHsContentInfo& aInfo );
    
    /**
     * See from API documentation
     */
    TInt ActivateViewL( CHsContentInfo& aInfo );
    
    /**
     * See from API documentation
     */
    TInt ActivateAppL( CHsContentInfo& aInfo );

    /**
     * See from API documentation
     */
   TInt ActiveViewL( CHsContentInfo& aInfo );
   
   /**
    * See from API documentation
    */
   TInt ActiveAppL( CHsContentInfo& aInfo );

private: // Functions
    /**
     * Requests receiving of content change notification
     */
    void WaitForApiNtfL();
    
    /**
     * Internalize received response message
     */
    TInt InternalizeRespL( TPtr8& aResp, TUint32& aTrId, TUint32& aDataSize );

    /**
     * Internalize received CHsContentInfo type response message data
     */
    TInt InternalizeContentInfoL( CHsContentInfo& aInfo, TUint32 aTrId, TUint32 aDataSize );

    /**
     * Internalize received CHsContentInfoArray type response message data
     */
    TInt InternalizeContentInfoArrayL( CHsContentInfoArray& aInfo, TUint32 aTrId, TUint32 aDataSize );

private: // Data
    /**
     * Session to Homescreen content control server
     */
    RCcClientSession iSession;

    /**
     * Home screen content control API observer
     */
    MHsContentControl* iObserver;
    
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

    };


#endif // HSCCAPICLIENT_H

// End of file
