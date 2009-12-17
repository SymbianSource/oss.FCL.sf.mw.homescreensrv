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
* Description:  CPS Client Service
 *
*/


#ifndef CPCLIENTSERVICE_H
#define CPCLIENTSERVICE_H

/**
 *  CPS Service Provider
 *
 *  This class implements new LIW Service Provider.
 *
 *  @lib cpclient.dll
 *  @since S60 v 5.0
 */
class CCPClientService : public CLiwServiceIfBase
    {
public:

    /**
     * Two-phase Constructor
     */    
    static CCPClientService* NewL();

    /**
     * Destructor.
     */    
    ~CCPClientService();

    // from base class CLiwServiceIfBase

    /**
     * From CLiwServiceIfBase.
     * Called by the LIW framework to initialise necessary information 
     * from the Service Handler. This method is called when the consumer makes
     * the attach operation.
     *
     * @since S60 v3.2
     * @param aFrameworkCallback Framework provided callback
     * @param aInterest List of criteria items which invoked the provider.
     */
    void InitialiseL( MLiwNotifyCallback& aFrameworkCallback,
        const RCriteriaArray& aInterest );

    /**
     * Executes generic service commands included in criteria.
     *
     * @param aCmdId Command to be executed.
     * @param aInParamList Input parameters, can be an empty list.
     * @param aOutParamList Output parameters, can be an empty list.
     * @param aCmdOptions Options for the command
     * @param aCallback Callback for asynchronous command handling, 
     * @leave KErrArgument Callback is missing when required.
     * @leave KErrNotSupported No provider supports service.
     */
    virtual void HandleServiceCmdL( const TInt& aCmdId,
        const CLiwGenericParamList& aInParamList,
        CLiwGenericParamList& aOutParamList, TUint aCmdOptions = 0,
        const MLiwNotifyCallback* aCallback = NULL );

private:

    /**
     * C++ default constructor.
     */     
    CCPClientService();

private:
    // data


    };

#endif // CPCLIENTSERVICE_H
