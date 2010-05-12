/*
* Copyright (c)  Nokia Corporation and/or its subsidiary(-ies).
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
* Description:  Content Publisher server client
 *
*/


#ifndef CPCLIENT_H
#define CPCLIENT_H

// INCLUDES
#include "cpclientsession.h"

// FORWARD DECLARATIONS
#ifdef CONTENT_PUBLISHER_DEBUG
class CCPDebug;
#endif
class CLiwGenericParamList;
class CCPActiveNotifier;
class CCPLiwMap;
class MLiwNotifyCallback;
class CLiwDefaultMap;

// CLASS DECLARATION

/**
 *  Client-side interface to Content Publisher
 *
 *  This class provides the client-side interface to the Content Publisher
 *  server.
 * 
 *  @lib cpclient.dll
 *  @since S6CCPActiveNotifierNotifier v 5.CCPActiveNotifierNotifier
 */
class CCPClient : public CBase
    {
public:

    /**
     * Two-phased constructor.
     */    
    static CCPClient* NewL();

    /**
     * Two-phased constructor.
     */    
    static CCPClient* NewLC();

    /**
     * Destructor.
     */
    ~CCPClient();

    /**
     * Send command to server to GetList of data from Content Publisher Database
     *
     * @since S6CCPActiveNotifierNotifier v 5.CCPActiveNotifierNotifier
     * @param aInParamList input parameter list (filter)
     * @param aOutParamList output parameter list (iterable of maps)
     */
    void GetListL( const CLiwGenericParamList& aInParamList,
        CLiwGenericParamList& aOutParamList );

    /**
     * Send command to server to Add data to Content Publisher Database
     *
     * @since S6CCPActiveNotifierNotifier v 5.CCPActiveNotifierNotifier
     * @param aInParamList input parameter list (data to add)
     * @param aOutParamList output parameter list (id of data or error code)
     * @param aCmdOptions options for the command
     */
    void AddL( const CLiwGenericParamList& aInParamList,
               CLiwGenericParamList& aOutParamList,
               TUint aCmdOptions );

    /**
     * Send command to server to Delete data from Content Publisher Database
     *
     * @since S6CCPActiveNotifierNotifier v 5.CCPActiveNotifierNotifier
     * @param aInParamList input parameter list (filter)
     */
    void DeleteL( const CLiwGenericParamList& aInParamList );

    /**
     * Send command to server to Register observer for notifications
     *
     * @since S6CCPActiveNotifierNotifier v 5.CCPActiveNotifierNotifier
     * @param aObserver 
     */
    void RegisterObserverL( MLiwNotifyCallback* aObserver,
        const CLiwGenericParamList& aInParamList, TInt32 aTransactionId );
   
    /**
     * Unregister observer if Transaction Id provided or all observers if not
     *
     * @since S6CCPActiveNotifierNotifier v 5.CCPActiveNotifierNotifier
     * @param aInParamList input parameter list (transactionId)
     */
    void UnregisterObserversL( const CLiwGenericParamList& aInParamList );

    /**
     * Send command to server to ExecuteAction
     *
     * @since S6CCPActiveNotifierNotifier v 5.CCPActiveNotifierNotifier
     * @param aInParamList input parameter list (filter)
     * @param aCmdOptions options for the command
     */
    void ExecuteActionL( const CLiwGenericParamList& aInParamList,
            TUint aCmdOptions );

    /**
     * Send command to server to ExecuteMultipleActions
     *
     * @param aInParamList input parameter list (filter)
     * @param aCmdOptions options for the command
     */
    void ExecuteMultipleActionsL(
            const CLiwGenericParamList& aInParamList, TUint aCmdOptions);
    
    /**
     * Check second param from IDataSource interface
     *
     * @since S6CCPActiveNotifierNotifier v 5.CCPActiveNotifierNotifier
     * @param aInParamList input parameter list 
     * @param aKey key with map
     */    
    void CheckMapL( const CLiwGenericParamList& aInParamList, 
            const TDesC8& aKey );
    
    /**
     * Check proper data is passed as parameter to ExecuteMultipleActions
     *
     * @since S6CCPActiveNotifierNotifier v 5.CCPActiveNotifierNotifier
     * @param aList generic list containing input parameters 
     */    
    void CheckMultiExecuteInputParamsL(const CLiwGenericParamList& aList);
    
private:

    /**
     * C++ default constructor.
     */
    CCPClient();

    /**
     * Perform the second phase construction of a CCPClient object.
     */
    void ConstructL();

private:
    //data

    /**
     * Client-server session
     */
    RCPServerClient iServerClient;
    /**
     * Pointer to an active object
     * Own.  
     */
    CCPActiveNotifier * iActiveNotifier;

#ifdef CONTENT_PUBLISHER_DEBUG
    CCPDebug* iDebug;
#endif

    };

#endif // CPCLIENT_H

// End of File
