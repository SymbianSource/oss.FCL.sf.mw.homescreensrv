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


#ifndef __UT_SAPIDATAOBSERVER_H__
#define __UT_SAPIDATAOBSERVER_H__

//  EXTERNAL INCLUDES
#include <digia/eunit/ceunittestsuiteclass.h>
#include <msvapi.h>
#include <liwcommon.h>
#include <liwservicehandler.h>

//  INTERNAL INCLUDES
#include "interface.h"

//  FORWARD DECLARATIONS
class CSapiData;
class CSapiDataObserver;
class CSapiDataPlugin;
class CLiwServiceHandler;

#include <e32def.h>

_LIT8( KCPInterface, "IDataSource" );
_LIT8( KCPService, "Service.ContentPublishing" );

//  CLASS DEFINITION
/**
 *
 * Unit test for the sapidataobserver
 *
 */
NONSHARABLE_CLASS( UT_SapiDataObserver ) : 
    public CEUnitTestSuiteClass
    {
public:     // Constructors and destructors

    /**
     * Two phase construction
     */
    static UT_SapiDataObserver* NewL();
    static UT_SapiDataObserver* NewLC();
    /**
     * Destructor
     */
    ~UT_SapiDataObserver();

private:    // Constructors and destructors

    UT_SapiDataObserver();
    void ConstructL();

private:    // New methods

    void SetupL();

    void Teardown();
    
    MLiwInterface* GetMessagingInterfaceL();

    void ConstructTestL();
    void RegisterTestL();
    void ReleaseTestL();
    void HandleNotifyTestL();

private:    // Data

    CSapiData* iData;
    MLiwInterface* iInterface;
    CSapiDataPlugin* iPlugin;
    CLiwServiceHandler* iServiceHandler;    
    CSapiDataObserver* iObserver;
    
    EUNIT_DECLARE_TEST_TABLE;
    };

#endif      //  __UT_SAPIDATAOBSERVER_H__

// End of file
