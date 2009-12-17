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
#ifndef TESTCLASS_H
#define TESTCLASS_H

//  INCLUDES
#include <StifParser.h>
#include <Stiftestinterface.h>
#include <StifLogger.h>
#include <TestScripterInternal.h>
#include <StifTestModule.h>
#include <e32property.h>


#include <mcsdef.h>

// CLASS DECLARATION

/**
*  CMenuSatInterfaceTest test class for STIF Test Framework TestScripter.
*  Class for testing CMenuSATInterface.
*
*  @lib ?library
*  @since ?Series60_version
*/
NONSHARABLE_CLASS(CMenuSatInterfaceTest) : public CScriptBase
    {
    public:  // Constructors and destructor
        
        /**
        * Two-phased constructor.
        */
        static CMenuSatInterfaceTest* NewL( CTestModuleIf& aTestModuleIf );
       
        /**
        * Destructor.
        */
        virtual ~CMenuSatInterfaceTest();

    public: // Functions from base classes

        /**
        * From CScriptBase Runs a script line.
        * @since ?Series60_version
        * @param aItem Script line containing method name and parameters
        * @return Symbian OS error code
        */
        virtual TInt RunMethodL( CStifItemParser& aItem );
                
    private:

        /**
        * C++ default constructor.
        */
        CMenuSatInterfaceTest( CTestModuleIf& aTestModuleIf );

        /**
        * By default Symbian 2nd phase constructor is private.
        */
        void ConstructL();
        
        /**
        * Test methods are listed below. 
        */
        TInt Test_SatUiVisibleL(CStifItemParser& /*aItem*/ );
        TInt Test_SatUiVisibleIconIdL(CStifItemParser& /*aItem*/ );
        TInt Test_SatInterfaceConstructL(CStifItemParser& /*aItem*/ );
        
        /**
        * Utility methods are listed below. 
        */
        void WaitL(TInt aMicroSec);
        
    private:    // Data
        /**
        * Data tosave enviroment statebefore testing. 
        */
    	TInt iIconId;
        
    };

#endif      // TESTCLASS_H
            
// End of File
