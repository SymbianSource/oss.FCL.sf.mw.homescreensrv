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
     * 
     */
    TInt WidgetListL( CHsContentInfoArray& aArray );
    
    /**
     * 
     */
    TInt ViewListL( CHsContentInfoArray& aArray );
    
    /**
     * 
     */
    TInt AppListL( CHsContentInfoArray& aArray );
    
    /**
     * 
     */
    TInt AddWidgetL( CHsContentInfo& aInfo );
    
    /**
     * 
     */
    TInt RemoveWidgetL( CHsContentInfo& aInfo );
    
    /**
     * 
     */
    TInt AddViewL( CHsContentInfo& aInfo );
    
    /**
     * 
     */
    TInt RemoveViewL( CHsContentInfo& aInfo );
    
    /**
     * 
     */
    TInt ActivateViewL( CHsContentInfo& aInfo );
    
    /**
     * 
     */
    TInt ActivateAppL( CHsContentInfo& aInfo );

private: // Functions
    /**
     * 
     */
    void WaitForApiNtfL();
    
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
