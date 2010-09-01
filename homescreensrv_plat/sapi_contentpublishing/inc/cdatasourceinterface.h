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
* Description:  CPS Service Interface
 *
*/


#ifndef CDATASOURCEINTERFACE_H
#define CDATASOURCEINTERFACE_H

#include <liwcommon.h>

class CCPClient;

/**
 *  CPS Service Interface
 *
 *  This class implements interface to CPS Service.
 *
 *  @lib cpclient.dll
 *  @since S60 v 5.0
 */
class CDataSourceInterface : public CBase, public MLiwInterface
    {
public:

    /**
     * Two-phased constructor.
     */      
    static CDataSourceInterface* NewL();

    /**
     * Two-phased constructor.
     */  
    static CDataSourceInterface* NewLC();

    /**
     * Desctructor.
     */  
    ~CDataSourceInterface();

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

protected:

    /**
     * C++ default constructor.
     */     
    CDataSourceInterface();
    /**
     * Perform the second phase construction of a CCPClientInterface object.
     */    
    void ConstructL();
    
    /**
     * Proccess request issued by service client.
     *
     * @param aCmdName the name of the service command to invoke
     * @param aInParamList the input parameter list, can be empty list
     * @param [in,out] aOutParamList the output parameter list, can be empty.
     * @param aCmdOptions Options for the command
     * @param aCallback callback to be registered by consumer application
     *
     */
    virtual void ProcessCommandL( const TDesC8& aCmdName,
        const CLiwGenericParamList& aInParamList,
        CLiwGenericParamList& aOutParamList, TUint aCmdOptions,
        MLiwNotifyCallback* aCallback );

public:
    
    /**
     * Converse error code
     * @param code the error
     * @return TInt32 the error code after converse  
     */
    static TInt32 ErrCodeConversion( TInt code );

protected:
    // data

    /**
     * Client to CPS Server. Containing active object
     * to handle asynchornous requests.
     * Own.
     */
    CCPClient* iCPClient;
    };

#endif // CDATASOURCEINTERFACE_H
