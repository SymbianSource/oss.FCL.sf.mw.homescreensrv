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
* Description:  Back Stepping Service Provider
 *
*/


#ifndef C_BS_SERVICE_PROVIDER_H
#define C_BS_SERVICE_PROVIDER_H

#include <liwserviceifbase.h>

/**
 *  Back Stepping Service Provider
 *  Serves as a LIW provider for the BS Service Interface.
 *
 *  @since S60 v3.2
 */
class CBSServiceProvider : public CLiwServiceIfBase
    {

public:

    /**
     * Two-phase constructor.
     */
    static CBSServiceProvider* NewL();

    /**
     * Destructor.
     */
    ~CBSServiceProvider();

    // from CLiwServiceIfBase
    /**
     * Called by the LIW framework to initialise necessary information 
     * from the Service Handler. This method is called when the consumer makes
     * the attach operation.
     *
     * @since S60 v3.2
     * @param aFrameworkCallback Framework provided callback
     * @param aInterest List of criteria items which invoked the provider
     */
    void InitialiseL( MLiwNotifyCallback& aFrameworkCallback,
        const RCriteriaArray& aInterest );

    /**
     * Executes generic service commands included in criteria.
     *
     * @param aCmdId Command to be executed
     * @param aInParamList Input parameters, can be an empty list
     * @param aOutParamList Output parameters, can be an empty list
     * @param aCmdOptions Options for the command
     * @param aCallback Callback for asynchronous command handling
     */
    void HandleServiceCmdL( const TInt& aCmdId,
        const CLiwGenericParamList& aInParamList,
        CLiwGenericParamList& aOutParamList, TUint aCmdOptions = 0,
        const MLiwNotifyCallback* aCallback = NULL );

private:

    /**
     * Constructor.
     */
    CBSServiceProvider();

    // error handling
    /**
     * Appends error code to output parameter list.
     *
     * @param aOutParamList output parameter list
     * @param aError error code
     */
    void HandleErrorL( CLiwGenericParamList& aOutParamList,
        const TInt aError );

private:
    // data

    };

#endif // C_BS_SERVICE_PROVIDER_H
