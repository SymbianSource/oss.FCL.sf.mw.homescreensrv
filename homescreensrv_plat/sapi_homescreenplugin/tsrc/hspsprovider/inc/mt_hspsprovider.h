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
* Description:  Defination of class MT_CHSPSProvider
*
*/


#ifndef C_MT_HSPSPROVIDER_H
#define C_MT_HSPSPROVIDER_H

#include <digia/eunit/eunitdecorators.h>
#include <liwservicehandler.h>
#include "mt_hsps.h"

/**
 * This class is a tester class for HSPS SAPI provider
*/
NONSHARABLE_CLASS( MT_CHSPSProvider ) : public MT_CHSPS
    {
    public:     // Constructors and destructors

        /**
         * Two phase construction
         */
        static MT_CHSPSProvider* NewL();
        /**
         * Destructor
         */
        virtual ~MT_CHSPSProvider();

    private:    // Constructors and destructors

    	MT_CHSPSProvider();
        void ConstructL();

    private:    // New methods

        void SetupL();
         
        void TestCase_AttachL();
                 
        void TestCase_AttachInvalidProviderL();

        void TestCase_AttachInvalidInterfaceL();

        void Teardown();

    private:    // Data

        EUNIT_DECLARE_TEST_TABLE; 

    };

#endif      //  C_MT_HSPSPROVIDER_H

// End of file
