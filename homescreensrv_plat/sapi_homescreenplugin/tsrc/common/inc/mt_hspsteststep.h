/*
* Copyright (c) 2008 Nokia Corporation and/or its subsidiary(-ies).
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
* Description:  Defination of class MT_CHSPSTeststep
*
*/


#ifndef C_MT_HSPSTESTSTEP_H
#define C_MT_HSPSTESTSTEP_H

#include <e32base.h>
#include <liwcommon.h>
#include <liwgenericparam.h>

/**
 * This class defines a HSPS test step used in HSPS testing
*/
NONSHARABLE_CLASS( MT_CHSPSTeststep )
	: public CActive, public MLiwNotifyCallback
    {
    
    public: // Constructors and destructors

        /**
         * Two phase construction
         */
        static MT_CHSPSTeststep* NewL(
            MLiwInterface& aInterface,
            TUint8* aMethod,
            TUint8* aInParams,
            TUint8* aExpOutParams,
            TBool aAsync,
            TBool aMeasureExecTime = EFalse );
        
        /**
         * Destructor
         */
        virtual ~MT_CHSPSTeststep();

    private: // Constructors and destructors

        MT_CHSPSTeststep(
            MLiwInterface& aInterface,
            TUint8* aMethod,
            TUint8* aInParams,
            TUint8* aExpOutParams,
            TBool aAsync,
            TBool aMeasureExecTime );
        
    public: // Functions from base classes

        /**
        * From CActive
        */
        void RunL();

        /**
         * From CActive
         */
        TInt RunError( TInt aError );

        /**
         * From CActive
         */
        void DoCancel();

        /**
        * From MLiwNotifyCallback
        * @param aCmdId                The service command associated to the event.
        * @param aEventId              Occured event, see LiwCommon.hrh.
        * @param aEventParamList       Event parameters, if any, as defined per
        *                              each event.
        * @param aInParamList          Input parameters, if any, given in the
        *                              related HandleCommmandL.
        * @return                      Error code for the callback.
        */       
        TInt HandleNotifyL(
            TInt aCmdId,
            TInt aEventId,
            CLiwGenericParamList& aEventParamList,
            const CLiwGenericParamList& aInParamList );
        
    public: // New methods
    
        /**
        * Calls service method
        */
        void CallServiceL();

        /**
        * Checks output parameters of method call
        * @return                      TRUE, if received output parameters 
        *                              match to expected output paramters
        *                              FALSE, if received output paramters
        *                              does not match to expected output parameters
        */
        TInt CheckOutParams();

    private: // New functions
        
        /**
        * Creates input parameter list from a descriptor stream
        * @param aInParams             Input parameter stream
        * @return                      Pointer to created input parameter list
        */
        CLiwGenericParamList* CreateInParamListL(
            TUint8* aInParams );

        /**
        * Creates output descriptor stream from output parameter list
        * @param aOutParamList         Output parameter list
        * @return                      Pointer to created output parameter stream
        */
        HBufC8* CreateOutParamsL(
                CLiwGenericParamList& aOutParamList );

        /**
        * Starts timing of SAPI method execution
        * @param aMethod               Called SAPI method
        * @return                      void
        */
        void StartTiming(
            TDesC8& aMethod );

        /**
        * Calculates SAPI method execution time
        * @return                      void
        */
        void StopTiming();
        
#ifdef HSPS_LOG_ACTIVE        
        /**
         * Debug output to a text file
         * @param aOutPtr An array of bytes from the SAPI call
         */
        void DebugL( const TUint8* aOutPtr );
#endif
        
    public: // Data

        // Output parameters
        HBufC8* iOutParams;
        // Test step timeout
        CPeriodic* iTimeout;
        
    private: // Data
    
        /** 
         * Service interface
         */
        MLiwInterface& iInterface;

        /**
         * Test step method 
         * ( Not owned )
         */
        TUint8* iMethod;
        
        /**
         * Test step input parameters as a binary stream
         * ( Not owned )
         */
        TUint8* iInParams;
        
        /**
         * Test step output parameters as a binary stream
         * ( Not owned )
         */
        TUint8* iExpOutParams;
        
        /** 
         * Test step asynchronous status
         */
        TBool iAsync;

        /** 
         * Execution time measurement status
         */
        TBool iMeasureExecTime;

        /** 
         * Test step method start time
         */
        TTime iStartTime;
        
    };

#endif      //  C_MT_HSPSTESTSTEP_H

// End of file
