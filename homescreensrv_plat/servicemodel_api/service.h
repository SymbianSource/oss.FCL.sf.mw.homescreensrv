/*
* Copyright (c) 2009 Nokia Corporation and/or its subsidiary(-ies).
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


#ifndef SERVICE_H
#define SERVICE_H

#include "servicemodel_global.h"
#include <QObject>
#include <QVariant>

class ServiceParameters;

/**
 * @ingroup group_hsservicemodel group_service_api_candidates
 * @brief Base class for all the services. 
 */
class HSSERVICEMODEL_EXPORT HsService : public QObject
{
	Q_OBJECT
	
public:

    /**
    * Constructor
    * @param aParent 
    */
    HsService( QObject* aParent = 0 ) : QObject( aParent ) {}
	  
    /**
     * destructor
     * @since S60 ?S60_version.
     */
    virtual ~HsService() {}
    
public:

    /**
     * Execute service command
     * @param aCommand service command 
     * @param aInParameters parameters. 
     * For example messaging service
     * needs to have at least the following pairs to send short message:
     * 'MessageType' - 'SMS'
     * 'To' - 'phone number here'
     * 'BodyText' - 'this is text to be sent'
     * So aInParameters should include QMap:
     * @code
     *   QMap<QString, QVariant> parameters;
     *   parameters["MessageType"]=QVariant(QString("SMS"));
     *   parameters["To"]=QVariant(QString("555555"));
     *   parameters["BodyText"]=QVariant(QString("this is bodytext"));
     *   QVariant inParameters;
     *   inParameters.setValue(parameters);
     * @endcode
     */
    virtual void executeCommand(const QByteArray &aCommand, const QVariant &aInParameters) = 0;
    
    /**
    * Cancels ALL then pending commands.
    */
    virtual void cancelCommands() = 0;
    
    /**
     * Get data after dataReceived signal has been received.
     * 
     * @param aData data buffer to be filled
     * TODO: error?
     */
    virtual void getData(QVariant &aData) = 0;
        
signals:

    /**
     * Signal emitted when data is received, client can then make query for the received data using getData function.
     */
    void dataReceived();
    
    /**
     * Signal emitted when data is received
     * @param aData (QMap<QString, QVariant>) data received from the service 
     */
    void dataReceived(const QVariant &aData);
    	    
};

#endif
