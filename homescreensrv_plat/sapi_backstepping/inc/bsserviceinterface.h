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
* Description:  Back Stepping Service Interface
 *
*/


#ifndef C_BS_SERVICE_INTERFACE_H
#define C_BS_SERVICE_INTERFACE_H

#include <liwcommon.h>

#include "bsclient.h"

/**
 *  Back Stepping Service Interface.
 *  LIW interface for communication with BS Server.
 *
 *  @since S60 v3.2
 */
class CBSServiceInterface : public CBase, public MLiwInterface
    {

public:

    /**
     * Two-phase constructor.
     */
    static CBSServiceInterface* NewLC();

    /**
     * Destructor.
     */
    ~CBSServiceInterface();

    // from base class MLiwInterface
    /**
     * The consumer application should call this method to execute a service 
     * command directly on the interface. 
     *
     * @param aCmdName the name of the service command to invoke
     * @param aInParamList the input parameter list, can be empty list
     * @param [in,out] aOutParamList the output parameter list, can be empty.
     * @param aCmdOptions Options for the command
     * @param aCallback callback to be registered by consumer application
     *
     */
    void ExecuteCmdL( const TDesC8& aCmdName,
        const CLiwGenericParamList& aInParamList,
        CLiwGenericParamList& aOutParamList, TUint aCmdOptions = 0,
        MLiwNotifyCallback* aCallback = 0 );

    /**
     * The consumer application should call this method if there 
     * are no more service commands to be executed on the interface.
     */
    void Close();

private:

    // construction
    /**
     * Constructor.
     */
    CBSServiceInterface();

    /**
     * Symbian 2nd phace constructor.
     */
    void ConstructL();

    // service handling methods
    /**
     * Handles Initialize command.
     *
     * @param aInParamList input parameter list
     * @param aOutParamList output parameter list
     */
    void InitializeL( const CLiwGenericParamList& aInParamList,
        CLiwGenericParamList& aOutParamList );

    /**
     * Handles ForwardActivationEvent command.
     *
     * @param aInParamList input parameter list
     * @param aOutParamList output parameter list
     */
    void ForwardActivationEventL( const CLiwGenericParamList& aInParamList,
        CLiwGenericParamList& aOutParamList );

    /**
     * Handles HandleBackCommand command.
     *
     * @param aInParamList input parameter list
     * @param aOutParamList output parameter list
     */
    void HandleBackCommandL( const CLiwGenericParamList& aInParamList,
        CLiwGenericParamList& aOutParamList );

    // result handling
    /**
     * Appends error code to output parameter list.
     *
     * @param aOutParamList output parameter list
     * @param aError error code
     */
    void HandleErrorL( CLiwGenericParamList& aOutParamList,
        const TInt aError );

    /**
     * Appends service result to output parameter list.
     *
     * @param aOutParamList output parameter list
     * @param aError error code
     */
    void HandleResultL( CLiwGenericParamList& aOutParamList,
        const TInt aResult );

    // utility methods
    /**
     * Extracts a TPtrC8 param from input parameter list.
     *
     * @param aInParamList input parameter list
     * @param aParamName param name
     * @param aPtr returned value
     * @retutn Symbian error code
     */
    TInt GetParam( const CLiwGenericParamList& aInParamList,
        const TDesC8& aParamName, TPtrC8& aPtr ) const;

    /**
     * Extracts a TInt32 param from input parameter list.
     *
     * @param aInParamList input parameter list
     * @param aParamName param name
     * @param aNumber returned value
     * @retutn Symbian error code
     */
    TInt GetParam( const CLiwGenericParamList& aInParamList,
        const TDesC8& aParamName, TInt32& aNumber ) const;

    /**
     * Extracts a TBool param from input parameter list.
     *
     * @param aInParamList input parameter list
     * @param aParamName param name
     * @param aBool returned value
     * @retutn Symbian error code
     */
    TInt GetParam( const CLiwGenericParamList& aInParamList,
        const TDesC8& aParamName, TBool& aBool ) const;

private:
    // data

    /**
     * BS Server client.
     */
    RBSClient iBSClient;

    };

#endif // C_BS_SERVICE_INTERFACE_H
