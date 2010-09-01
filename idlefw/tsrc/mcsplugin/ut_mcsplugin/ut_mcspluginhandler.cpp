/*
* Copyright (c) 2010 Nokia Corporation and/or its subsidiary(-ies).
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
* Description: unit test for the mcsplugin handler
*
*/


//  CLASS HEADER
#include "ut_mcspluginhandler.h"


//  EXTERNAL INCLUDES
#include <mcsmenu.h>
#include <mcsmenuitem.h>
#include <mcsmenufilter.h>
#include <mcsdef.h>
#include <digia/eunit/eunitmacros.h>
#include <e32cmn.h>

//  INTERNAL INCLUDES
#include "mcspluginhandler.h"
#include "mcsplugincompletedoperation.h"
#include "mcspluginparamval.h"
#include "mcspluginwatcher.h"
#include "caouserafter.h"


const TInt KWait_05_secs = 0.5 * 1000000;
const TInt KWait_1_secs = 1 * 1000000;
const TInt KWait_2_secs = 2 * 1000000;
const TInt KWait_3_secs = 3 * 1000000;

TInt32 KMCSCmailMtmUidValue = 0x2001F406;

_LIT( KUrl, "http://www.nokia.com" );
_LIT( KUrlInvalid, "services://nothingcanbeworse" );
_LIT( KDialed, "logs:dialed" );
_LIT( KBookmarkIconFile, "aimcsplugin.mif" );
_LIT( KBookmarkIconId, "16386" );
_LIT( KBookmarkIconMaskId, "16387" );

_LIT( KMyMenuData, "matrixmenudata" );
_LIT( KMenuUrl, "menu:url" );
_LIT( KMenuShortcut, "menu:shortcut" );
_LIT( KMenuWrong, "menu:failingmenu" );
_LIT( KMenuAttrParam, "param" );
_LIT8( KCmdOpen, "open" );

// CONSTRUCTION

// ---------------------------------------------------------------------------
//
// ---------------------------------------------------------------------------
//
UT_McsPluginHandler* UT_McsPluginHandler::NewL()
{
    UT_McsPluginHandler* self = UT_McsPluginHandler::NewLC();
    CleanupStack::Pop();

    return self;
}

// ---------------------------------------------------------------------------
//
// ---------------------------------------------------------------------------
//
UT_McsPluginHandler* UT_McsPluginHandler::NewLC()
{
    UT_McsPluginHandler* self = new(ELeave) UT_McsPluginHandler();
    CleanupStack::PushL(self);

    self->ConstructL();

    return self;
}

// ---------------------------------------------------------------------------
// Destructor (virtual by CBase)
// ---------------------------------------------------------------------------
//
UT_McsPluginHandler::~UT_McsPluginHandler()
{
    iMenu.Close();
}

// ---------------------------------------------------------------------------
// Default constructor
// ---------------------------------------------------------------------------
//
UT_McsPluginHandler::UT_McsPluginHandler()
{
}

// ---------------------------------------------------------------------------
// Second phase construct
// ---------------------------------------------------------------------------
//
void UT_McsPluginHandler::ConstructL()
{
    // The ConstructL from the base class CEUnitTestSuiteClass must be called.
    // It generates the test case table.
    CEUnitTestSuiteClass::ConstructL();
}


// ---------------------------------------------------------------------------
// void UT_McsPluginHandler::HandleSessionEventL
// ---------------------------------------------------------------------------
//
void UT_McsPluginHandler::HandleSessionEventL(
          TMsvSessionEvent /*aEvent*/, 
          TAny* /*aArg1*/, 
          TAny* /*aArg2*/,
          TAny* /*aArg3*/ )
    {
    // No event handling
    }

// ---------------------------------------------------------------------------
// void UT_McsPluginHandler::SetupL()
// ---------------------------------------------------------------------------
//
void UT_McsPluginHandler::SetupL()
{
    iMenu.OpenL( KMyMenuData );
    iHandler = CMCSPluginHandler::NewL( iMenu );
    iWatcher = CMCSPluginWatcher::NewL( CMCSPluginWatcher::EOperation );   
}

// ---------------------------------------------------------------------------
// void UT_McsPluginHandler::Teardown()
// ---------------------------------------------------------------------------
//
void UT_McsPluginHandler::Teardown()
{
    if ( iWatcher )
        {
        iWatcher->Cancel();
        delete iWatcher;
        iWatcher = NULL;
        }
    if ( iHandler )
       {
       delete iHandler;
       iHandler = NULL;
       }
   iMenu.Close();
}

// ---------------------------------------------------------------------------
// void UT_McsPluginHandler::SupportedTypesTestL()
// ---------------------------------------------------------------------------
//
void UT_McsPluginHandler::SupportedTypesTestL()
    {
    // Checks all supported types (excludes unsupported)
    // Change if added/removed
    EUNIT_ASSERT( iHandler->SupportsType( KMenuTypeFolder ) );
    EUNIT_ASSERT( iHandler->SupportsType( KMenuTypeSuite ) );
    EUNIT_ASSERT( iHandler->SupportsType( KMenuShortcut ) );    
    }

// ---------------------------------------------------------------------------
// void UT_McsPluginHandler::CommandHandlingL()
// ---------------------------------------------------------------------------
//
void UT_McsPluginHandler::CommandHandlingL()
    {
    CMenuItem* menuItem = NULL;

    /**
     * Test 1 - incorrect command to handle
     */
    // Try with incorrect settings
    menuItem = CMenuItem::CreateL( iMenu, KMenuWrong, 0, 0 );
    CleanupStack::PushL( menuItem );
    menuItem->SetAttributeL( KMenuAttrLongName, KUrlInvalid );
    menuItem->SetAttributeL( KMenuAttrParam, KUrlInvalid );    
    menuItem->SetAttributeL( KMenuAttrIconFile, KBookmarkIconFile );
    menuItem->SetAttributeL( KMenuAttrIconId, KBookmarkIconId );
    menuItem->SetAttributeL( KMenuAttrMaskId, KBookmarkIconMaskId );
    EUNIT_ASSERT_LEAVE
        ( 
        // Should leave here!!!
        CMCSPluginCompletedOperation* opl = NULL;
        opl = static_cast<CMCSPluginCompletedOperation*> ( 
            iHandler->HandleCommandL( 
            *menuItem, KNullDesC8, KNullDesC8, iWatcher->iStatus ) );
        );

    CleanupStack::PopAndDestroy( menuItem ); // iWatcher is not incorporated 
    }

// ---------------------------------------------------------------------------
// UT_McsPluginHandler::LaunchShortcutL()
// ---------------------------------------------------------------------------
//
void UT_McsPluginHandler::LaunchShortcutL()
    {
    CMenuItem* menuItem = NULL;

    /**
     * Test 1 - correct command to handle, incorrect menu data
     */
    menuItem = CMenuItem::CreateL( iMenu, KMenuShortcut, 0, 0 );
    CleanupStack::PushL( menuItem );
    menuItem->SetAttributeL( KMenuAttrLongName, KDialed );
    // KMenuAttrParam is omitted - shortcut cannot be launched
    //menuItem->SetAttributeL( KMenuAttrParam, KDialed );    
    menuItem->SetAttributeL( KMenuAttrIconFile, KBookmarkIconFile );
    menuItem->SetAttributeL( KMenuAttrIconId, KBookmarkIconId );
    menuItem->SetAttributeL( KMenuAttrMaskId, KBookmarkIconMaskId );

    EUNIT_ASSERT_LEAVE
        ( 
        // Should leave here!!!
        CMCSPluginCompletedOperation* opl = NULL;
        opl = static_cast<CMCSPluginCompletedOperation*>
        ( iHandler->HandleCommandL( *menuItem, KCmdOpen, KNullDesC8, 
            iWatcher->iStatus ) );
        );

    CleanupStack::PopAndDestroy( menuItem ); // iWatcher is not incorporated  
    }

// ---------------------------------------------------------------------------
// UT_McsPluginHandler::LaunchItemL()
// ---------------------------------------------------------------------------
//
void UT_McsPluginHandler::LaunchItemL()
    {
    //ToDo: Is shortcut launching the best way to test the function? In case
    // when launched cannot be killed
        
    CMenuOperation* op = NULL;
    CMenuItem* menuItem = NULL;

    menuItem = CMenuItem::CreateL( iMenu, KMenuShortcut, 0, 0 );
    CleanupStack::PushL( menuItem );
    menuItem->SetAttributeL( KMenuAttrLongName, KParamValueEmail );
    menuItem->SetAttributeL( KMenuAttrParam, KParamValueEmail );    
    menuItem->SetAttributeL( KMenuAttrIconFile, KBookmarkIconFile );
    menuItem->SetAttributeL( KMenuAttrIconId, KBookmarkIconId );
    menuItem->SetAttributeL( KMenuAttrMaskId, KBookmarkIconMaskId );
    
    op = iHandler->HandleCommandL( *menuItem, KCmdOpen, KNullDesC8, 
        iWatcher->iStatus );
    iWatcher->Watch( op );
    
    // checks CActive iStatus
    TInt tcs = 0;
    while ( iWatcher->GetStatus() == KRequestPending )
        {
        CAOUserAfter::AfterLD( 1000000 );
        if ( ( tcs++ ) > 20 )
            break;
        }
    iWatcher->Cancel();
    
    CleanupStack::Pop( menuItem ); // destroyed by iWatcher
    //ToDo: Is it necessary to test all possibilities?    
    }

// ---------------------------------------------------------------------------
// UT_McsPluginHandler::SessionEventL()
// ---------------------------------------------------------------------------
//
void UT_McsPluginHandler::SessionEventL()
    {
    // Test HandleSessionEventL
    // The funciton is empty now, so it tests leave only
    EUNIT_ASSERT_NO_LEAVE( iHandler->HandleSessionEventL( 
        MMsvSessionObserver::EMsvEntriesCreated, NULL, NULL, NULL ) );
    }

//  TEST TABLE

EUNIT_BEGIN_TEST_TABLE
    (
    UT_McsPluginHandler,
    "MCS plugin handler tests",
    "UNIT"
    )

EUNIT_TEST
    (
    "Tests supported types for the mcs plugina handler",
    "CMcsPluginHandler",
    "SupportsType",
    "FUNCTIONALITY",
    SetupL, SupportedTypesTestL, Teardown
    )

EUNIT_TEST
    (
    "Tests command handling",
    "CMcsPluginHandler",
    "HandleCommandL",
    "FUNCTIONALITY",
    SetupL, CommandHandlingL, Teardown
    )

EUNIT_TEST
    (
    "Tests command handling & shortcut launching",
    "CMcsPluginHandler",
    "HandleCommandL, LaunchShortcutL",
    "FUNCTIONALITY",
    SetupL, LaunchShortcutL, Teardown
    )

EUNIT_TEST
    (
    "Tests command handling & shortcut launching 2",
    "CMcsPluginHandler",
    "HandleCommandL, LaunchShortcutL",
    "FUNCTIONALITY",
    SetupL, LaunchItemL, Teardown
    )

EUNIT_TEST
    (
    "Tests HandleSessionEventL",
    "CMcsPluginHandler",
    "HandleSessionEventL",
    "FUNCTIONALITY",
    SetupL, SessionEventL, Teardown
    )

EUNIT_END_TEST_TABLE

//  END OF FILE
