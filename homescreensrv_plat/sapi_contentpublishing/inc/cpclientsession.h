/*
* Copyright (c) 2007 Nokia Corporation and/or its subsidiary(-ies).
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


#ifndef CPCLIENTSESSION_H
#define CPCLIENTSESSION_H

// INCLUDES

#include <e32base.h>

// FORWARD DECLARATIONS
class CLiwGenericParamList;
class CCPActive;
class CCPLiwMap;

// CLASS DECLARATION
/**
 * Client-side interface to CPS Server
 *
 * This class provides the client-side interface to the active data server session,
 * it just passes requests to the server.
 *
 */
class RCPServerClient : public RSessionBase
    {
public:

    /**
     * Construct the object.
     */
    RCPServerClient();

    /**
     * Connects to the server and create a session.
     * When the server is not running, the function starts the server.
     * @return error code
     */
    TInt Connect();

    /**
     * Pass Add request to the server
     *
     * @since S60 v 5.0
     * @param aInParamList const reference to the input list
     * @param aOutParamList reference to the output list
     */
    void AddL( const CCPLiwMap& aMap,
               CLiwGenericParamList& aOutParamList,
               TUint aCmdOptions );

    /**
     * Pass GetList request to the server
     *
     * @since S60 v 5.0
     * @param aInParamList const reference to the input list
     * @param aOutParamList reference to the output list
     */
    void GetListL( const CCPLiwMap& aMap,
        CLiwGenericParamList& aOutParamList );

    /**
     * Pass Delete request to the server
     *
     * @since S60 v 5.0
     * @param aInParamList const reference to the input list
     */
    void DeleteL( const CCPLiwMap& aMapt );

    /**
     * Pass RegisterObserver request to the server
     *
     * @since S60 v 5.0
     * @param aStatus  ?
     * @param aBuf ?
     * @param aSize ?
     * @return error code
     */
    void RegisterObserverL( TRequestStatus& aStatus, TIpcArgs& aArgs );
    
    /**
     * Pass AddObserver request to the server
     *
     * @since S60 v 5.0
     * @return error code
     */
    void AddObserver( TIpcArgs& aArgs );

    /**
     * Pass RemoveObserver request to the server
     *
     * @since S60 v 5.0
     * @return error code
     */
    void RemoveObserver( TIpcArgs& aArgs );
    
    /**
     * Pass ExecuteAction request to the server
     *
     * @since S60 v 5.0
     * @param aInParamList const reference to the input list
     * @param aOptions Command options.
     */
    void ExecuteActionL( const CCPLiwMap& aMap, TUint aOptions = 0 );

    /**
     * Pass GetChangeInfoData request to server
     * @param aBuf reference to the input list
     * @return error code
     */
    TInt GetChangeInfoData( TDes8& aBuf );
    /**
     * Pass Activate request to the server
     *
     * @since S60 v 5.0
     * @param aInParamList const reference to the input list
     */
    void ActivateL( const CCPLiwMap& aMap, TUint aOptions = 0 );

    /**
     * Pass UnregisterObserver request to the server
     *
     * @since S60 v 5.0
     * @return error code
     */
    void UnregisterObserver();

    /**
     * Closes session
     *
     * @since S60 v 5.0
     */
    void Close();

    /**
     * Return version information
     */
    TVersion Version() const;

private:
    //private methods

    };

#endif // CPCLIENTSESSION_H

// End of File
