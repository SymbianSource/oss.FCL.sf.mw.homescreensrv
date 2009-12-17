/*
* Copyright (c) 2006-2006 Nokia Corporation and/or its subsidiary(-ies).
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
* Description:  Active notifier class
 *
*/


#ifndef CPCLIENTACTIVENOTIFIER_H
#define CPCLIENTACTIVENOTIFIER_H

// INCLUDES
#include <e32hashtab.h> 
#include "cpclientsession.h"

class MLiwNotifyCallback;
/**
 *  Content publisher active notifier class
 *
 * 
 *  @lib cpclient.dll
 *  @since S60 v 5.0
 */
class CCPActiveNotifier : public CActive
    {
public:

    /**
     * Two-phased constructor.
     * 
     */     
    static CCPActiveNotifier* NewL( RCPServerClient& aServerClient );

    /**
     * Desctructor.
     */     
    ~CCPActiveNotifier();

    /**
     * Register observer
     *
     * @param aObserver Pointer for callback
     * @param aMap Map containing parameters
     * @param aTransactionId Integer transaction id
     */
    void RegisterL( MLiwNotifyCallback* aObserver, TInt32 aTransactionId, 
    		CCPLiwMap* aMap );

    /**
     * Unregister observer
     *
     * @since S60 v 5.0
     */
    void UnregisterL();

    /**
     * Unregister observer
     *
     * @since S60 v 5.0
     * @param aTransactionId Integer transaction id
     * @return returns ETrue if last observer was unregistered
     */
    TBool UnregisterL( TInt32 aTransactionId );

    
    /**
     * Unregister all observers
     * @since S60 v 5.0
     */   
    void UnregisterAllL(  );

protected:

    /**
     * From CActive, RunL.
     * Handles the active object’s request completion event
     */
    void RunL();

    /**
     * From CActive, DoCancel.
     * Implements cancellation of an outstanding request.
     */
    void DoCancel();

    /**
     * From CActive, RunError.
     * Implements cancellation of an outstanding request.
     */
    TInt RunError( TInt aError );

private:

    /**
     * C++ default constructor.
     */    
    CCPActiveNotifier( RCPServerClient& aServerClient );

    /**
     * Perform the second phase construction of a CCPActiveNotifier object.
     */    
    void ConstructL();

    /*
     * Notify all observers
     */
    void NotifyObserversL( TInt aErrorCode, 
    		CLiwGenericParamList* aEventParamList  );
    
    /*
     * Register observer
     */
    void RegisterAgainL( );

    
private:

    /**
     * Descriptor where server can write to when passing data
     * Own.
     */
    HBufC8 *iObserverBuf;

    /**
     * Size of the descriptor passed between server and client
     * Own.   
     */
    TPckgBuf<TInt>* iSizeDes;


    /**
     * Client-server session
     * Not Own.
     */
    RCPServerClient iServerClient;

    /*
     * Array containing all observers
     * Own
     */
    RHashMap<TInt32, MLiwNotifyCallback*> iObservers;

    };

#endif // CPCLIENTACTIVENOTIFIER_H
