/*
* Copyright (c) 2005-2007 Nokia Corporation and/or its subsidiary(-ies).
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
* Description:  Plug-in main class
*
*/


#ifndef SAPIDATAOBSERVER_H
#define SAPIDATAOBSERVER_H

// INCLUDE FILES
#include <liwcommon.h>

class CSapiData;

class CSapiDataObserver : public CBase, public MLiwNotifyCallback
    {
public:

    /**
    * Part of the two phased constuction
    *
    * @param aInterface reference of the interface 
    * @param aData reference of the sapi data object
    * @return none
    */
    static CSapiDataObserver* NewL( MLiwInterface* aInterface, CSapiData* aData );
    
    /**
    * Destructor
    *
    * @param none
    * @return none
    */   
    ~CSapiDataObserver();

private :
    
    /**
    * Constructor
    *
    * @param none
    * @return none
    */
    CSapiDataObserver();
    
    /**
    * Part of the two phased construction
    *
    * @param aInterface reference of the interface
    * @param aData reference of the sapi data object
    * @return void
    */
    void ConstructL( MLiwInterface* aInterface, CSapiData* aData );
       
public:  //from MLiwNotifyCallbackc
    
    /**
    * Handles notifications caused by an asynchronous Execute*CmdL call
    * or an event.
    *
    * @param aCmdId The service command associated to the event.
    * @param aEventId occurred event, see LiwCommon.hrh.
    * @param aEventParamList Event parameters, if any, as defined per
    *        each event.
    * @param aInParamList Input parameters, if any, given in the
    *        related HandleCommmandL.
    * @return Error code for the call back.
    */
    virtual TInt HandleNotifyL(
        TInt aCmdId,
        TInt /*aEventId*/,
        CLiwGenericParamList& aEventParamList,
        const CLiwGenericParamList& /*aInParamList*/);

public:
    
    /**
    * Registers to CPS for add, delete , update and execute notifications
    * @aFilter - filter for input parameter list
    * @return void.
    */
    void RegisterL( CLiwDefaultMap* aFilter, const TDesC& aRegistry );

    /**
    * Cancel all the registered notifications. 
    * @return void.
    */
    void ReleaseL();
        
private:
        
    // Reference of 
    // Not owned
    MLiwInterface* iInterface;
    
    // Reference of the sapi data
    // Not owned
    CSapiData* iData;
    
    // Call back error code
    TInt iError;
        
    };

#endif /*SAPIDATAOBSERVER_H*/
