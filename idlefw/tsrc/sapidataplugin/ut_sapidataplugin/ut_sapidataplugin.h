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


#ifndef __UT_SAPIDATAPLUGIN_H__
#define __UT_SAPIDATAPLUGIN_H__

//  EXTERNAL INCLUDES
#include <digia/eunit/ceunittestsuiteclass.h>
#include <msvapi.h>
#include <hscontentpublisher.h>
#include <aicontentmodel.h>
//  INTERNAL INCLUDES

//  FORWARD DECLARATIONS
class CSapiData;
class CSapiDataPlugin;
class Observer;

#include <e32def.h>
#ifndef NONSHARABLE_CLASS
#define NONSHARABLE_CLASS(x) class x
#endif

//  CLASS DEFINITION
/**
 *
 * Unit test for the mcspluginhandler
 *
 */
NONSHARABLE_CLASS( UT_SapiDataPlugin ) : 
    public CEUnitTestSuiteClass
    //public MMsvSessionObserver
    {
public:     // Constructors and destructors

    /**
     * Two phase construction
     */
    static UT_SapiDataPlugin* NewL();
    static UT_SapiDataPlugin* NewLC();
    /**
     * Destructor
     */
    ~UT_SapiDataPlugin();

private:    // Constructors and destructors

    UT_SapiDataPlugin();
    void ConstructL();

private:    // New methods

    void SetupL();

    void Teardown();
    
    void UT_SapiDataPlugin_Data();
    
    void UT_SapiDataPlugin_ConfigureL();
    
    void UT_SapiDataPlugin_Resume();
    
    void UT_SapiDataPlugin_NetworkStatus();
    
    void UT_SapiDataPlugin_GetProperty();
    
    void UT_SapiDataPlugin_GetTypeL();
    
    void UT_SapiDataPlugin_GetIdL();
    
    void UT_SapiDataPlugin_HasMenuItem();    
    
    void UT_SapiDataPlugin_Clean();

private:    // Data

    CSapiData* iData;
    
    CSapiDataPlugin* iPlugin;
    
    Observer* iObserver;
    
    EUNIT_DECLARE_TEST_TABLE;
    };

#endif      //  __UT_SAPIDATAPLUGIN_H__

// End of file
