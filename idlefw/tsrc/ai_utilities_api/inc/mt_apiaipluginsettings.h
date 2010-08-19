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

#ifndef __MT_ApiAiPluginSettings_H__
#define __MT_ApiAiPluginSettings_H__

//  EXTERNAL INCLUDES
#include <digia/eunit/ceunittestsuiteclass.h>
#include <aipluginsettings.h>

//  INTERNAL INCLUDES


//  FORWARD DECLARATIONS
class MAiPluginSettings;
class MAiPluginSettingsItem;
class MAiPluginContentItem;
class MAiPluginConfigurationItem;


#include <e32def.h>
#ifndef NONSHARABLE_CLASS
    #define NONSHARABLE_CLASS(x) class x
#endif

//  CLASS DEFINITION
/**
 *
 * EUnitWizard generated test class. 
 *
 */
NONSHARABLE_CLASS( MT_ApiAiPluginSettings )
     : public CEUnitTestSuiteClass
    {
    public:     // Constructors and destructors

        /**
         * Two phase construction
         */
        static MT_ApiAiPluginSettings* NewL();
        static MT_ApiAiPluginSettings* NewLC();
        /**
         * Destructor
         */
        ~MT_ApiAiPluginSettings();

    private:    // Constructors and destructors

        MT_ApiAiPluginSettings();
        void ConstructL();

    public:     // From observer interface

        

    private:    // New methods

         void SetupL();
         
         void Teardown();
        
         void TestAiPluginSettingsItemL();
         
         void TestAiPluginContentItemL();
         
         void TestAiPluginConfigurationItemL();
         
         void TestAiPluginSettingsStorerBasicL();

    private:    // Data

        EUNIT_DECLARE_TEST_TABLE; 
        MAiPluginSettings* iAiPluginSettings;

    };

/**
 *
 * Additional test class.
 * Emulates basic storing functionality
 * 
 */
NONSHARABLE_CLASS( TStorer ): public MAiPluginSettingsStorer
    {
    public:    
        TStorer( );
        ~TStorer( );
        
        // derived from MAiPluginSettingsStorer
        void SaveL( TInt32 aKey, const TDesC& aValue );
        void ReadL( TInt32 aKey, TDes& aValue );
        // functions to set and read values
        void SetKey( TInt32 aKey );
        void SetValue( const TDesC aValue );
        void GetKey( TInt32& aKey );
        void GetValue( TDes& aValue );
    private:
        TInt32 iKey;
        TBuf<40> iValue;
    };

#endif      //  __MT_ApiAiPluginSettings_H__

// End of file
