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


#ifndef __UT_MCSPLUGINHANDLER_H__
#define __UT_MCSPLUGINHANDLER_H__

//  EXTERNAL INCLUDES
#include <digia/eunit/ceunittestsuiteclass.h>
#include <mcsmenu.h>
#include <msvapi.h>

//  INTERNAL INCLUDES

//  FORWARD DECLARATIONS
class CMCSPluginHandler;
class CMCSPluginWatcher;

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
NONSHARABLE_CLASS( UT_McsPluginHandler ) : 
    public CEUnitTestSuiteClass,
    public MMsvSessionObserver
    {
public:     // Constructors and destructors

    /**
     * Two phase construction
     */
    static UT_McsPluginHandler* NewL();
    static UT_McsPluginHandler* NewLC();
    /**
     * Destructor
     */
    ~UT_McsPluginHandler();

private:    // Constructors and destructors

    UT_McsPluginHandler();
    void ConstructL();

private:    // New methods

    void SetupL();

    void Teardown();

    /**
     * Test supported types function for correct output
     */
    void SupportedTypesTestL();

    /**
     * Tests command handling and spoectial shortcuts launching
     * functins for basic functionality
     */
    void CommandHandlingL();
    
    /**
     * Tests pure shortcut launching
     */
    void LaunchShortcutL();
    
    /**
     * Tests launch of a specific shortcut
     */
    void LaunchItemL();

    /**
     * Tests HandleSessionEvent function
     */
    void SessionEventL();

    /**
     * From class MMsvSessionObserver,
     * Handles an event from the message server.
     */
    void HandleSessionEventL(
              TMsvSessionEvent /*aEvent*/, 
              TAny* /*aArg1*/, 
              TAny* /*aArg2*/,
              TAny* /*aArg3*/ );

private:    // Data

    CMCSPluginHandler* iHandler;
    RMenu iMenu;
    CMCSPluginWatcher* iWatcher;

    EUNIT_DECLARE_TEST_TABLE;
    };

#endif      //  __UT_MCSPLUGINHANDLER_H__

// End of file
