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

// System includes
#include <liwcommon.h>

// User includes

// Forward declarations
class CSapiData;

/**
 * @ingroup group_sapidataplugin
 *
 * Sapi data observer
 *
 * @since S60 5.0
 */
NONSHARABLE_CLASS( CSapiDataObserver ) : public CBase, 
    public MLiwNotifyCallback
    {
public: // constructor and destructor
    
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

private:  // construtors
    
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
       
public: //from MLiwNotifyCallback
    
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

public:  // new functions
    
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
        
private: // data
    
    /** Interface Reference, not owned */    
    MLiwInterface* iInterface;   
    /** Reference of the sapi data, not owned */
    CSapiData* iData;    
    };

#endif // SAPIDATAOBSERVER_H

// End of file

