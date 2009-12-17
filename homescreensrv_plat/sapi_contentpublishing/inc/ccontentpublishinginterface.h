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
* Description:  CPS Service Content Interface
 *
*/


#ifndef CCONTENTPUBLISHINGINTERFACE_H
#define CCONTENTPUBLISHINGINTERFACE_H

#include "cdatasourceinterface.h"

/**
 *  CPS Service Interface
 *
 *  This class implements interface to CPS Service.
 *
 *  @lib cpclient.dll
 *  @since S60 v 5.0
 */
class CContentPublishingInterface : public CDataSourceInterface
    {
public:

    /**
     * Two-phased constructor.
     */      
    static CContentPublishingInterface* NewL();

    /**
     * Two-phased constructor.
     */  
    static CContentPublishingInterface* NewLC();

    /**
     * Desctructor.
     */  
    ~CContentPublishingInterface();

private:

    /**
     * C++ default constructor.
     */     
    CContentPublishingInterface();
    
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

private:
    // data

    };

#endif // CCONTENTPUBLISHINGINTERFACE_H
