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
* Description:  
 *
*/


#ifndef C_ACTIONHANDLERINTERFACE_H
#define C_ACTIONHANDLERINTERFACE_H

#include <liwcommon.h>

class CPluginValidator;
class CLiwMap;

/**
 *  Class implements Action Handler Service Interface
 *
 *  Action Handler itself is a component which enables
 *  to execute different kind of actions.
 *
 *  @since S60 v3.2
 */
class CActionHandlerInterface : public CBase, public MLiwInterface
    {
public:

    static CActionHandlerInterface* NewL();

    static CActionHandlerInterface* NewLC();

    ~CActionHandlerInterface();

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
        CLiwGenericParamList& aOutParamList, TUint aCmdOptions,
        MLiwNotifyCallback* aCallback );

    /**
     * The consumer application should call this method if there 
     * are no more service commands to be executed on the interface.
     *
     */
    virtual void Close();

private:

    CActionHandlerInterface();

    void ConstructL();

    /**
     * Get uid of the required plugin from aInParamList
     *
     * @since S60 v3.2
     * @param aInParamList param received from service client
     * @param aUid source for the extracted uid of the plugin
     * @return errCode
     */
    TInt ExtractUid( const CLiwGenericParamList& aInParamList, TUid& aUid );

    /**
     * Get map from aInParamList
     *
     * @since S60 v3.2
     * @param aInParamList param received from service client
     * @param aMap source for the exctracted map
     * @return errCode
     */
    TInt ExtractMap( const CLiwGenericParamList& aInParamList, CLiwMap* aMap );

    TInt32 ErrCodeConversion( TInt code );

    void ExecuteActionL( const TUid aUid, const CLiwMap* aMap );

private:
    
    // data
    CPluginValidator* iPluginManager;

    };

#endif // C_ACTIONHANDLERINTERFACE_H
