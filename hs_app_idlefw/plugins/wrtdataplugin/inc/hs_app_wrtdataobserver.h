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


#ifndef WRTDATAOBSERVER_H
#define WRTDATAOBSERVER_H

// INCLUDE FILES
#include <liwcommon.h>

// FORWARD DECLARATIONS
class CWrtData;

// CLASS DECLARATION
/**
 *  @ingroup group_wrtdataplugin
 *
 *  Wrt data observer
 *
 *  @since S60 v3.2
 */
class CWrtDataObserver : public CBase, public MLiwNotifyCallback
    {
    public:

        /**
        * Part of the two phased constuction
        *
        * @param aInterface reference of the interface 
        * @param aData reference of the wrt data object
        * @return none
        */
        static CWrtDataObserver* NewL( MLiwInterface* aInterface, CWrtData* aData );
    
        /**
        * Destructor
        *
        * @param none
        * @return none
        */   
        ~CWrtDataObserver();

    private :
    
        /**
        * Constructor
        *
        * @param none
        * @return none
        */
        CWrtDataObserver();
        
        /**
        * Part of the two phased construction
        *
        * @param aInterface reference of the interface
        * @param aData reference of the wrt data object
        * @return void
        */
        void ConstructL( MLiwInterface* aInterface, CWrtData* aData );
       
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
        void RegisterL( CLiwDefaultMap* aFilter );
    
        /**
        * Cancel all the registered notifications. 
        * @return void.
        */
        void ReleaseL();
            
    private:
            
        // Reference of 
        // Not owned
        MLiwInterface* iInterface;
        
        // Reference of the wrt data
        // Not owned
        CWrtData* iData;
        
        // Call back error code
        TInt iError;
        
    };

#endif /*WRTDATAOBSERVER_H*/