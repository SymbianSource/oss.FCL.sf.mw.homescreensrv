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
* Description:  EUnit module test class for CPS Wrapper
*
*/

// System include files
#include <hsccapiclient.h>
#include <hscontentcontrol.h>
#include <hscontentinfo.h>
#include <hscontentinfoarray.h>
#include <ccresource.h>

// User include files
#include "mthsccapiclient.h"

// Local constants
_LIT8( KInfoTypeWidget, "widget" );
_LIT8( KInfoTypeTemplate, "template" );
_LIT8( KInfoTypeView, "view" );
_LIT8( KInfoTypeApp, "application" );
_LIT8( KTemplateViewUid, "0x20026f50" );
_LIT8( KHsViewUid, "0x2001f48b" );
_LIT8( KDesktopWidgetUid, "0x20026f4f" );
_LIT8( KNotFoundAppUid, "0xffffffff" );
_LIT8( KNotFoundViewUid, "0xffffffff" );
_LIT8( KNotFoundWidgetUid, "0xffffffff" );
_LIT8( KNotFoundViewPluginId, "999" );

// ======== MEMBER FUNCTIONS ========

// -----------------------------------------------------------------------
// CMTHsCcApiClient::NewL()
// -----------------------------------------------------------------------
//
CMTHsCcApiClient* CMTHsCcApiClient::NewL()
    {
    CMTHsCcApiClient* self = new ( ELeave ) CMTHsCcApiClient();
    CleanupStack::PushL( self );
    self->ConstructL();
    CleanupStack::Pop( self );
    return self;
    }

// -----------------------------------------------------------------------
// CMTHsCcApiClient::ConstructL()
// -----------------------------------------------------------------------
//
void CMTHsCcApiClient::ConstructL()
    {
    // The ConstructL from the base class CEUnitTestSuiteClass must be called.
    // It generates the test case table.
    CEUnitTestSuiteClass::ConstructL();
    }

// -----------------------------------------------------------------------
// CMTHsCcApiClient::CMTHsCcApiClient()
// -----------------------------------------------------------------------
//
CMTHsCcApiClient::CMTHsCcApiClient()
    :iApiClient( NULL )
    {
    }

// -----------------------------------------------------------------------
// CMTHsCcApiClient::~CMTHsCcApiClient()
// -----------------------------------------------------------------------
//
CMTHsCcApiClient::~CMTHsCcApiClient()
    {
    delete iApiClient;
    }

// -----------------------------------------------------------------------
// CMTHsCcApiClient::SetupL()
// -----------------------------------------------------------------------
//
void CMTHsCcApiClient::SetupL()
    {
    iWidgetListNtfExpected = EFalse;
    iWidgetListNtfReceived = EFalse;
    iViewListNtfExpected = EFalse;
    iViewListNtfReceived = EFalse;
    iAppListNtfExpected = EFalse;
    iAppListNtfReceived = EFalse;

    if ( iApiClient )
        {
        delete iApiClient;
        iApiClient = NULL;
        }
    iApiClient = CHsCcApiClient::NewL( NULL );

    // Remove all removable views from Home screen
    CHsContentInfo* app = CHsContentInfo::NewL();
    CleanupStack::PushL( app );
    iApiClient->ActiveAppL( *app );
    
    CHsContentInfoArray* views = CHsContentInfoArray::NewL();
    CleanupStack::PushL( views );
    iApiClient->ViewListL( *app, *views );

    for ( TInt i = 0; i < views->Array().Count(); i++ )
        {
        CHsContentInfo* info = views->Array()[ i ];
        if ( info->CanBeRemoved() )
            {
            iApiClient->RemoveViewL( *info );
            }
        }

    CleanupStack::PopAndDestroy( views );

    // Remove all removable widgets from Home screen
    CHsContentInfoArray* widgets = CHsContentInfoArray::NewL();
    CleanupStack::PushL( widgets );
    iApiClient->WidgetListL( *app, *widgets );

    for ( TInt i = 0; i < widgets->Array().Count(); i++ )
        {
        CHsContentInfo* info = widgets->Array()[ i ];
        if ( info->CanBeRemoved() )
            {
            iApiClient->RemoveWidgetL( *info );
            }
        }

    CleanupStack::PopAndDestroy( widgets );

    CleanupStack::PopAndDestroy( app );
        
    delete iApiClient;
    iApiClient = NULL;
        
    }

// -----------------------------------------------------------------------
// CMTHsCcApiClient::Teardown()
// -----------------------------------------------------------------------
//
void CMTHsCcApiClient::Teardown()
    {
    }

// -----------------------------------------------------------------------
// CMTHsCcApiClient::NotifyWidgetListChanged()
// -----------------------------------------------------------------------
//
void CMTHsCcApiClient::NotifyWidgetListChanged()
    {
    if ( iWidgetListNtfExpected )
        {
        iWidgetListNtfReceived = ETrue;
        iWait.AsyncStop();
        }
    }

// -----------------------------------------------------------------------
// CMTHsCcApiClient::NotifyViewListChanged()
// -----------------------------------------------------------------------
//
void CMTHsCcApiClient::NotifyViewListChanged()
    {
    if ( iViewListNtfExpected )
        {
        iViewListNtfReceived = ETrue;
        iWait.AsyncStop();
        }
    }

// -----------------------------------------------------------------------
// CMTHsCcApiClient::NotifyAppListChanged()
// -----------------------------------------------------------------------
//
void CMTHsCcApiClient::NotifyAppListChanged()
    {
    if ( iAppListNtfExpected )
        {
        iAppListNtfReceived = ETrue;
        iWait.AsyncStop();
        }
    }

// -----------------------------------------------------------------------
// CMTHsCcApiClient::ConnectHsCcApiClientL()
// -----------------------------------------------------------------------
//
void CMTHsCcApiClient::ConnectHsCcApiClientL( MHsContentControl* aObserver )
    {
    iApiClient = CHsCcApiClient::NewL( aObserver );
    }

// -----------------------------------------------------------------------
// CMTHsCcApiClient::DisconnectHsCcApiClient()
// -----------------------------------------------------------------------
//
void CMTHsCcApiClient::DisconnectHsCcApiClient()
    {
    delete iApiClient;
    iApiClient = NULL;
    }

// -----------------------------------------------------------------------
// CMTHsCcApiClient::AddViewL()
// -----------------------------------------------------------------------
//
CHsContentInfo* CMTHsCcApiClient::AddViewL(
    const TDesC8& aUid )
    {
    CHsContentInfo* info = GetViewL( NULL, aUid );
    CleanupStack::PushL( info );

    TInt err = iApiClient->AddViewL( *info );
    User::LeaveIfError( err );
    CleanupStack::PopAndDestroy( info );
    info = NULL;

    CHsContentInfo* app = CHsContentInfo::NewL();
    CleanupStack::PushL( app );

    err = iApiClient->ActiveAppL( *app );
    User::LeaveIfError( err );

    info = GetViewL( app, aUid );

    CleanupStack::PopAndDestroy( app );

    User::LeaveIfNull( info );
    return info;
    }

// -----------------------------------------------------------------------
// CMTHsCcApiClient::AddWidgetL()
// -----------------------------------------------------------------------
//
CHsContentInfo* CMTHsCcApiClient::AddWidgetL(
    const TDesC8& aUid )
    {

    CHsContentInfo* info = GetWidgetL( NULL, aUid );
    CleanupStack::PushL( info );

    TInt err = iApiClient->AddWidgetL( *info );
    User::LeaveIfError( err );
    CleanupStack::PopAndDestroy( info );
    info = NULL;

    CHsContentInfo* view = CHsContentInfo::NewL();
    CleanupStack::PushL( view );

    err = iApiClient->ActiveViewL( *view );
    User::LeaveIfError( err );

    info = GetWidgetL( view, aUid );
    
    CleanupStack::PopAndDestroy( view );

    User::LeaveIfNull( info );
    return info;
    }

// -----------------------------------------------------------------------
// CMTHsCcApiClient::GetViewL()
// -----------------------------------------------------------------------
//
CHsContentInfo* CMTHsCcApiClient::GetViewL(
    CHsContentInfo* aApp,
    const TDesC8& aUid )
    {
    CHsContentInfoArray* views = CHsContentInfoArray::NewL();
    CleanupStack::PushL( views );
    TInt err( KErrNone );
    if ( aApp )
        {
        err = iApiClient->ViewListL( *aApp, *views );
        }
    else
        {
        err = iApiClient->ViewListL( *views );
        }
    User::LeaveIfError( err );

    CHsContentInfo* info = NULL;
    for ( TInt i = 0; i < views->Array().Count() && !info; i++ )
        {
        if ( views->Array()[ i ]->Uid().CompareF( aUid ) == 0 )
            {
            info = views->Array()[ i ];
            views->Array().Remove( i );
            break;
            }
        }
    CleanupStack::PopAndDestroy( views );

    User::LeaveIfNull( info );
    return info;
    }

// -----------------------------------------------------------------------
// CMTHsCcApiClient::GetWidgetL()
// -----------------------------------------------------------------------
//
CHsContentInfo* CMTHsCcApiClient::GetWidgetL(
    CHsContentInfo* aView,
    const TDesC8& aUid )
    {
    CHsContentInfoArray* widgets = CHsContentInfoArray::NewL();
    CleanupStack::PushL( widgets );
    TInt err( KErrNone );
    if ( aView )
        {
        err = iApiClient->WidgetListL( *aView, *widgets );
        }
    else
        {
        err = iApiClient->WidgetListL( *widgets );
        }
    User::LeaveIfError( err );

    CHsContentInfo* info = NULL;
    for ( TInt i = 0; i < widgets->Array().Count() && !info; i++ )
        {
        if ( widgets->Array()[ i ]->Uid().CompareF( aUid ) == 0 )
            {
            info = widgets->Array()[ i ];
            widgets->Array().Remove( i );
            break;
            }
        }
    CleanupStack::PopAndDestroy( widgets );

    User::LeaveIfNull( info );
    return info;
    }

// -----------------------------------------------------------------------
// CMTHsCcApiClient::ConnectDisconnectHsCcApiClientL()
// 
// Test purpose:
// The purpose of this test case is to verify that a connection to the
// Home screen Content Control server is successfully established and 
// closed via Home screen Content Control API client when the api observer
// is not defined
//
// Pre conditions (SetupL()):
// Empty homescreen
//
// Test step 1:
// Create CHsCcApiClient
//
// Expected result:
// Connection is successfully established
//
// Test step 2:
// Delete CHsCcApiClient
//
// Expected result:
// Connection is successfully closed
//
// Post conditions (Teardown()):
// -
//
// -----------------------------------------------------------------------
//
void CMTHsCcApiClient::ConnectDisconnectHsCcApiClientL()
    {
    // Test step 1
    ConnectHsCcApiClientL( NULL );

    // Test step 2
    DisconnectHsCcApiClient();
    }

// -----------------------------------------------------------------------
// CMTHsCcApiClient::RegisterUnregisterHsCcApiObserverL()
// 
// Test purpose:
// The purpose of this test case is to verify that a connection to the
// Home screen Content Control server is successfully established and 
// closed via Home screen Content Control API client when the api observer
// is defined
//
// Pre conditions (SetupL()):
// Empty homescreen
//
// Test step 1:
// Create CHsCcApiClient
//
// Expected result:
// Connection is successfully established and 
// observation is successfully started
//
// Test step 2:
// Delete CHsCcApiClient
//
// Expected result:
// Connection is successfully closed
//
// Post conditions (Teardown()):
// -
//
// -----------------------------------------------------------------------
//
void CMTHsCcApiClient::RegisterUnregisterHsCcApiObserverL()
    {
    // Test step 1
    ConnectHsCcApiClientL( this );

    // Test step 2
    DisconnectHsCcApiClient();
    }

// -----------------------------------------------------------------------
// CMTHsCcApiClient::RegisterUnregisterMultipleObserversL()
// 
// Test purpose:
// The purpose of this test case is to verify that multiple connections to 
// the Home screen Content Control server are successfully established and 
// closed via Home screen Content Control API client when the api observer
// is defined
//
// Pre conditions (SetupL()):
// Empty homescreen
//
// Test step 1:
// Create CHsCcApiClient
//
// Expected result:
// Connection is successfully established and 
// observation is successfully started
//
// Test step 2
// Create additional connection 1
//
// Expected result:
// Connection is successfully established and 
// observation is successfully started
//
// Test step 3
// Create additional connection 2
//
// Expected result:
// Connection is successfully established and 
// observation is successfully started
//
// Test step 4:
// Delete additional connection 1
//
// Expected result:
// Connection is successfully closed
//
// Test step 5:
// Delete additional connection 2
//
// Expected result:
// Connection is successfully closed
//
// Test step 6:
// Delete CHsCcApiClient
//
// Expected result:
// Connection is successfully closed
//
// Post conditions (Teardown()):
// -
//
// -----------------------------------------------------------------------
//
void CMTHsCcApiClient::RegisterUnregisterMultipleObserversL()
    {
    // Test step 1
    ConnectHsCcApiClientL( this );

    // Test step 2
    CHsCcApiClient* addClient1 = CHsCcApiClient::NewL( this );
    CleanupStack::PushL( addClient1 );

    // Test step 3
    CHsCcApiClient* addClient2 = CHsCcApiClient::NewL( this );

    // Test step 4
    CleanupStack::PopAndDestroy( addClient1 );
    
    // Test step 5
    delete addClient2;

    // Test step 6
    DisconnectHsCcApiClient();
    }

// -----------------------------------------------------------------------
// CMTHsCcApiClient::WidgetListL()
// 
// Test purpose:
// The purpose of this test case is to verify that a correct list of
// widgets is returned
//
// Pre conditions (SetupL()):
// Empty homescreen
//
// Test step 1:
// Create CHsCcApiClient
//
// Expected result:
// Connection is successfully established
//
// Test step 2:
// Get widget list
//
// Expected result:
// Content info list with widget or template type content info
//
// Test step 3:
// Delete CHsCcApiClient
//
// Expected result:
// Connection is successfully closed
//
// Post conditions (Teardown()):
// -
//
// -----------------------------------------------------------------------
//
void CMTHsCcApiClient::WidgetListL()
    {
    // Test step 1
    ConnectHsCcApiClientL( NULL );

    // Test step 2
    CHsContentInfoArray* widgets = CHsContentInfoArray::NewL();
    CleanupStack::PushL( widgets );
    TInt err = iApiClient->WidgetListL( *widgets );
    // Check widget list
    for ( TInt i = 0; i < widgets->Array().Count() && !err ; i++ )
        {
        CHsContentInfo* info = widgets->Array()[ i ];
        if ( info->Type().Compare( KInfoTypeWidget ) != 0 &&
             info->Type().Compare( KInfoTypeTemplate ) != 0 )
            {
            // Invalid widget type
            err = KErrArgument;
            }
        }
    CleanupStack::PopAndDestroy( widgets );
    User::LeaveIfError( err );

    // Test step 3
    DisconnectHsCcApiClient();
    }

// -----------------------------------------------------------------------
// CMTHsCcApiClient::AddRemoveWidgetL()
// 
// Test purpose:
// The purpose of this test case is to verify that a widget which can be
// added to the Home scree is successfully added
//
// Pre conditions (SetupL()):
// Empty homescreen
//
// Test step 1:
// Create CHsCcApiClient
//
// Expected result:
// Connection is successfully established
//
// Test step 2:
// Add test view
//
// Expected result:
// View successfully aded
//
// Test step 3:
// Activate test view
//
// Expected result:
// View successfully activated
//
// Test step 4:
// Add desktop widget
//
// Expected result:
// Widget is successfully added
//
// Test step 5:
// Remove desktop widget
//
// Expected result
// Widget is removed succesfully
//
// Test step 6:
// Delete CHsCcApiClient
//
// Expected result:
// Connection is successfully closed
//
// Post conditions (Teardown()):
// -
//
// -----------------------------------------------------------------------
//
void CMTHsCcApiClient::AddRemoveWidgetL()
    {
    // Test step 1
    ConnectHsCcApiClientL( NULL );

    // Test step 2
    CHsContentInfo* view = AddViewL( KTemplateViewUid );
    User::LeaveIfNull( view );
    CleanupStack::PushL( view );

    // Test step 3
    TInt err = iApiClient->ActivateViewL( *view );
    User::LeaveIfError( err );
    CleanupStack::PopAndDestroy( view );

    // Test step 4
    CHsContentInfo* widget = AddWidgetL( KDesktopWidgetUid );
    User::LeaveIfNull( widget );
    CleanupStack::PushL( widget );

    // Test step 5
    err = iApiClient->RemoveWidgetL( *widget );
    User::LeaveIfError( err );
    CleanupStack::PopAndDestroy( widget );

    // Test step 6
    DisconnectHsCcApiClient();
    }

// -----------------------------------------------------------------------
// CMTHsCcApiClient::AddWidgetFails1L()
// 
// Test purpose:
// The purpose of this test case is to verify that adding of widget fails
// if a widget which cannot be found is request to be added
//
// Pre conditions (SetupL()):
// Empty homescreen
//
// Test step 1:
// Create CHsCcApiClient
//
// Expected result:
// Connection is successfully established
//
// Test step 2:
// Add test view
//
// Expected result:
// View successfully added
//
// Test step 3:
// Activate test view
//
// Expected result:
// View successfully activated
//
// Test step 4:
// Add invalid widget
//
// Expected result:
// Widget adding fails
//
// Test step 5:
// Delete CHsCcApiClient
//
// Expected result:
// Connection is successfully closed
//
// Post conditions (Teardown()):
// -
//
// -----------------------------------------------------------------------
//
void CMTHsCcApiClient::AddWidgetFails1L()
    {
    // Test step 1
    ConnectHsCcApiClientL( NULL );

    // Test step 2
    CHsContentInfo* view = AddViewL( KTemplateViewUid );
    User::LeaveIfNull( view );
    CleanupStack::PushL( view );

    // Test step 3
    TInt err = iApiClient->ActivateViewL( *view );
    User::LeaveIfError( err );
    CleanupStack::PopAndDestroy( view );

    // Test step 4
    CHsContentInfo* widget = GetWidgetL( NULL, KDesktopWidgetUid );
    User::LeaveIfNull( widget );
    CleanupStack::PushL( widget );
    widget->SetUidL( KNotFoundWidgetUid );
    err = iApiClient->AddWidgetL( *widget );
    if ( err != KErrArgument )
        {
        User::Leave( KErrGeneral );
        }
    CleanupStack::PopAndDestroy( widget );

    // Test step 5
    DisconnectHsCcApiClient();
    }

// -----------------------------------------------------------------------
// CMTHsCcApiClient::RemoveWidgetFails1L()
// 
// Test purpose:
// The purpose of this test case is to verify that removing of widget fails
// if a widget request to be removed cannot be found
//
// Pre conditions (SetupL()):
// Empty homescreen
//
// Test step 1:
// Create CHsCcApiClient
//
// Expected result:
// Connection is successfully established
//
// Test step 2:
// Add test view
//
// Expected result:
// View successfully added
//
// Test step 3:
// Activate test view
//
// Expected result:
// View successfully activated
//
// Test step 4:
// Remove invalid widget
//
// Expected result:
// Widget removing fails
//
// Test step 5:
// Delete CHsCcApiClient
//
// Expected result:
// Connection is successfully closed
//
// Post conditions (Teardown()):
// -
//
// -----------------------------------------------------------------------
//
void CMTHsCcApiClient::RemoveWidgetFails1L()
    {
    // Test step 1
    ConnectHsCcApiClientL( NULL );

    // Test step 2
    CHsContentInfo* view = AddViewL( KTemplateViewUid );
    User::LeaveIfNull( view );
    CleanupStack::PushL( view );

    // Test step 3
    TInt err = iApiClient->ActivateViewL( *view );
    User::LeaveIfError( err );
    CleanupStack::PopAndDestroy( view );

    // Test step 4
    CHsContentInfo* widget = GetWidgetL( NULL, KDesktopWidgetUid );
    User::LeaveIfNull( widget );
    CleanupStack::PushL( widget );
    widget->SetUidL( KNotFoundWidgetUid );
    err = iApiClient->RemoveWidgetL( *widget );
    if ( err != KErrArgument )
        {
        User::Leave( KErrGeneral );
        }
    CleanupStack::PopAndDestroy( widget );

    // Test step 5
    DisconnectHsCcApiClient();
    }

// -----------------------------------------------------------------------
// CMTHsCcApiClient::WidgetListChangeNtfL()
// 
// Test purpose:
// The purpose of this test case is to verify that a notification is
// received after a widget is successfully addded to Home screen
//
// Pre conditions (SetupL()):
// Empty homescreen
//
// Test step 1:
// Create CHsCcApiClient
//
// Expected result:
// Connection is successfully established
//
// Test step 2:
// Add test view
//
// Expected result:
// View successfully added
//
// Test step 3:
// Activate test view
//
// Expected result:
// View successfully activated
//
// Test step 4:
// Delete CHsCcApiClient
//
// Expected result:
// Connection is successfully closed
//
// Test step 5:
// Create CHsCcApiClient and register it as Content Control server api 
// observer
//
// Expected result:
// Connection is successfully established observation registration 
// succeeded
//
// Test step 6:
// Add desktop widget
//
// Expected result:
// 1) Widget added successfully
// 2) Widget list change notification received
//
// Test step 7:
// Delete CHsCcApiClient
//
// Expected result:
// Connection is successfully closed
//
// Post conditions (Teardown()):
// -
//
// -----------------------------------------------------------------------
//
void CMTHsCcApiClient::WidgetListChangeNtfL()
    {
    // Test step 1
    ConnectHsCcApiClientL( NULL );

    // Test step 2
    CHsContentInfo* view = AddViewL( KTemplateViewUid );
    User::LeaveIfNull( view );
    CleanupStack::PushL( view );

    // Test step 3
    TInt err = iApiClient->ActivateViewL( *view );
    User::LeaveIfError( err );
    CleanupStack::PopAndDestroy( view );

    // Test step 4
    DisconnectHsCcApiClient();

    // Test step 5
    ConnectHsCcApiClientL( this );

    // Test step 6
    iWidgetListNtfExpected = ETrue;
    CHsContentInfo* widget = GetWidgetL( NULL, KDesktopWidgetUid );
    User::LeaveIfNull( widget );
    CleanupStack::PushL( widget );
    err = iApiClient->AddWidgetL( *widget );
    User::LeaveIfError( err );

    // Wait for notification
    iWait.Start();
    if ( !iWidgetListNtfReceived )
        {
        User::Leave( KErrGeneral );
        }
    CleanupStack::PopAndDestroy( widget );

    // Test step 7
    DisconnectHsCcApiClient();
    }

// -----------------------------------------------------------------------
// CMTHsCcApiClient::ViewListL()
// 
// Test purpose:
// The purpose of this test case is to verify that a correct list of
// views is returned
//
// Pre conditions (SetupL()):
// Empty homescreen
//
// Test step 1:
// Create CHsCcApiClient
//
// Expected result:
// Connection is successfully established
//
// Test step 2:
// Get view list
//
// Expected result:
// Content info list with view type content info
//
// Test step 3:
// Delete CHsCcApiClient
//
// Expected result:
// Connection is successfully closed
//
// Post conditions (Teardown()):
// -
//
// -----------------------------------------------------------------------
//
void CMTHsCcApiClient::ViewListL()
    {
    // Test step 1
    ConnectHsCcApiClientL( NULL );

    // Test step 2
    CHsContentInfoArray* views = CHsContentInfoArray::NewL();
    CleanupStack::PushL( views );
    TInt err = iApiClient->ViewListL( *views );
    // Check widget list
    for ( TInt i = 0; i < views->Array().Count() && !err ; i++ )
        {
        CHsContentInfo* info = views->Array()[ i ];
        if ( info->Type().Compare( KInfoTypeView ) != 0 )
            {
            // Invalid widget type
            err = KErrArgument;
            }
        }
    CleanupStack::PopAndDestroy( views );
    User::LeaveIfError( err );

    // Test step 3
    DisconnectHsCcApiClient();
    }

// -----------------------------------------------------------------------
// CMTHsCcApiClient::AddRemoveViewL()
// 
// Test purpose:
// The purpose of this test case is to verify that a widget which can be
// added to the Home scree is successfully added
//
// Pre conditions (SetupL()):
// Empty homescreen
//
// Test step 1:
// Create CHsCcApiClient
//
// Expected result:
// Connection is successfully established
//
// Test step 2:
// Add test view
//
// Expected result:
// View added successfully
//
// Test step 3:
// Remove test view
//
// Expected result:
// View is successfully removed
//
// Test step 4:
// Delete CHsCcApiClient
//
// Expected result:
// Connection is successfully closed
//
// Post conditions (Teardown()):
// -
//
// -----------------------------------------------------------------------
//
void CMTHsCcApiClient::AddRemoveViewL()
    {
    // Test step 1
    ConnectHsCcApiClientL( NULL );

    // Test step 2
    CHsContentInfo* view = AddViewL( KTemplateViewUid );
    User::LeaveIfNull( view );
    CleanupStack::PushL( view );

    // Test step 3
    TInt err = iApiClient->RemoveViewL( *view );
    User::LeaveIfError( err );
    CleanupStack::PopAndDestroy( view );

    // Test step 4
    DisconnectHsCcApiClient();
    }

// -----------------------------------------------------------------------
// CMTHsCcApiClient::ActivateViewL()
// 
// Test purpose:
// The purpose of this test case is to verify that a view can be
// successfully activated
//
// Pre conditions (SetupL()):
// Empty homescreen
//
// Test step 1:
// Create CHsCcApiClient
//
// Expected result:
// Connection is successfully established
//
// Test step 2:
// Add test view
//
// Expected result:
// View successfully added
//
// Test step 3:
// Activate test view
//
// Expected result:
// View successfully activated
//
// Test step 4:
// Delete CHsCcApiClient
//
// Expected result:
// Connection is successfully closed
//
// Post conditions (Teardown()):
// -
//
// -----------------------------------------------------------------------
//
void CMTHsCcApiClient::ActivateViewL()
    {
    // Test step 1
    ConnectHsCcApiClientL( NULL );

    // Test step 2
    CHsContentInfo* view = AddViewL( KTemplateViewUid );
    User::LeaveIfNull( view );
    CleanupStack::PushL( view );

    // Test step 3
    TInt err = iApiClient->ActivateViewL( *view );
    User::LeaveIfError( err );
    CleanupStack::PopAndDestroy( view );

    // Test step 4
    DisconnectHsCcApiClient();
    }

// -----------------------------------------------------------------------
// CMTHsCcApiClient::AddViewFails1L()
// 
// Test purpose:
// The purpose of this test case is to verify that adding of view fails
// if an unknown view (invalid UID) is tried to add to Home screen
//
// Pre conditions (SetupL()):
// Empty homescreen
//
// Test step 1:
// Create CHsCcApiClient
//
// Expected result:
// Connection is successfully established
//
// Test step 2:
// Add view which cannot be found to the Home screen
//
// Expected result:
// View adding fails
//
// Test step 3:
// Delete CHsCcApiClient
//
// Expected result:
// Connection is successfully closed
//
// Post conditions (Teardown()):
// -
//
// -----------------------------------------------------------------------
//
void CMTHsCcApiClient::AddViewFails1L()
    {
    // Test step 1
    ConnectHsCcApiClientL( NULL );

    // Test step 2
    CHsContentInfo* view = GetViewL( NULL, KTemplateViewUid );
    User::LeaveIfNull( view );
    CleanupStack::PushL( view );
    view->SetUidL( KNotFoundViewUid );
    TInt err = iApiClient->AddViewL( *view );
    if ( err != KErrArgument )
        {
        User::Leave( KErrGeneral );
        }
    CleanupStack::PopAndDestroy( view );

    // Test step 3
    DisconnectHsCcApiClient();
    }

// -----------------------------------------------------------------------
// CMTHsCcApiClient::RemoveViewFails1L()
// 
// Test purpose:
// The purpose of this test case is to verify that removing of view fails
// if a view which cannot be removed (last view) is request to be removed
//
// Pre conditions (SetupL()):
// Empty homescreen
//
// Test step 1:
// Create CHsCcApiClient
//
// Expected result:
// Connection is successfully established
//
// Test step 2:
// Remove last view
//
// Expected result:
// Removing of last view fails
//
// Test step 3:
// Delete CHsCcApiClient
//
// Expected result:
// Connection is successfully closed
//
// Post conditions (Teardown()):
// -
//
// -----------------------------------------------------------------------
//
void CMTHsCcApiClient::RemoveViewFails1L()
    {
    // Test step 1
    ConnectHsCcApiClientL( NULL );

    // Test step 2
    CHsContentInfo* view = GetViewL( NULL, KHsViewUid );
    User::LeaveIfNull( view );
    CleanupStack::PushL( view );
    TInt err = iApiClient->RemoveViewL( *view );
    if ( err != KErrArgument )
        {
        User::Leave( KErrGeneral );
        }
    CleanupStack::PopAndDestroy( view );

    // Test step 3
    DisconnectHsCcApiClient();
    }

// -----------------------------------------------------------------------
// CMTHsCcApiClient::ViewListChangeNtfL()
// 
// Test purpose:
// The purpose of this test case is to verify that a notification is
// received after a view is successfully addded to Home screen
//
// Pre conditions (SetupL()):
// Empty homescreen
//
// Test step 1:
// Create CHsCcApiClient and register it as Content Control server api 
// observer
//
// Expected result:
// Connection is successfully established observation registration 
// succeeded
//
// Test step 2:
// Add template view
//
// Expected result:
// 1) View added successfully
// 2) View list change notification received
//
// Test step 3:
// Delete CHsCcApiClient
//
// Expected result:
// Connection is successfully closed
//
// Post conditions (Teardown()):
// -
//
// -----------------------------------------------------------------------
//
void CMTHsCcApiClient::ViewListChangeNtfL()
    {
    // Test step 1
    ConnectHsCcApiClientL( this );

    // Test step 2
    iViewListNtfExpected = ETrue;
    CHsContentInfo* view = AddViewL( KTemplateViewUid );
    User::LeaveIfNull( view );
    CleanupStack::PushL( view );

    // Wait for notification
    iWait.Start();
    if ( !iViewListNtfReceived )
        {
        User::Leave( KErrGeneral );
        }
    CleanupStack::PopAndDestroy( view );

    // Test step 6
    DisconnectHsCcApiClient();
    }

// -----------------------------------------------------------------------
// CMTHsCcApiClient::AppListL()
// 
// Test purpose:
// The purpose of this test case is to verify that a correct list of
// application configurations is returned
//
// Pre conditions (SetupL()):
// Empty homescreen
//
// Test step 1:
// Create CHsCcApiClient
//
// Expected result:
// Connection is successfully established
//
// Test step 2:
// Get app list
//
// Expected result:
// Content info list with app type content info
//
// Test step 3:
// Delete CHsCcApiClient
//
// Expected result:
// Connection is successfully closed
//
// Post conditions (Teardown()):
// -
//
// -----------------------------------------------------------------------
//
void CMTHsCcApiClient::AppListL()
    {
    // Test step 1
    ConnectHsCcApiClientL( NULL );

    // Test step 2
    CHsContentInfoArray* apps = CHsContentInfoArray::NewL();
    CleanupStack::PushL( apps );
    TInt err = iApiClient->AppListL( *apps );
    // Check app list
    for ( TInt i = 0; i < apps->Array().Count() && !err ; i++ )
        {
        CHsContentInfo* info = apps->Array()[ i ];
        if ( info->Type().Compare( KInfoTypeApp ) != 0 )
            {
            // Invalid widget type
            err = KErrArgument;
            }
        }
    CleanupStack::PopAndDestroy( apps );
    User::LeaveIfError( err );

    // Test step 3
    DisconnectHsCcApiClient();
    }

// -----------------------------------------------------------------------
// CMTHsCcApiClient::ActivateAppL()
// 
// Test purpose:
// The purpose of this test case is to verify that a application
// configuration can be successfully activated
//
// Pre conditions (SetupL()):
// Empty homescreen
//
// Test step 1:
// Create CHsCcApiClient
//
// Expected result:
// Connection is successfully established
//
// Test step 2:
// Get app list
//
// Expected result:
// Content info list with application type content info
//
// Test step 3:
// Activate application type content info
//
// Expected result
// Application content info is activated
//
// Test step 4:
// Delete CHsCcApiClient
//
// Expected result:
// Connection is successfully closed
//
// Post conditions (Teardown()):
// -
//
// -----------------------------------------------------------------------
//
void CMTHsCcApiClient::ActivateAppL()
    {
    // Test step 1
    ConnectHsCcApiClientL( NULL );

    // Test step 2
    CHsContentInfoArray* apps = CHsContentInfoArray::NewL();
    CleanupStack::PushL( apps );
    TInt err = iApiClient->AppListL( *apps );
    User::LeaveIfError( err );
    // Get application content info which can be activated
    CHsContentInfo* info = NULL;
    if ( apps->Array().Count() )
        {
        info = apps->Array()[ 0 ];
        apps->Array().Remove( 0 );
        }
    apps->Array().ResetAndDestroy();
    User::LeaveIfNull( info );
    CleanupStack::PushL( info );

    // Test step 3
    err = iApiClient->ActivateAppL( *info );
    User::LeaveIfError( err );

    CleanupStack::PopAndDestroy( info );
    CleanupStack::PopAndDestroy( apps );

    // Test step 4
    DisconnectHsCcApiClient();
    }

// -----------------------------------------------------------------------
// CMTHsCcApiClient::ActiveAppL()
// 
// Test purpose:
// The purpose of this test case is to verify that an active application
// configuration is successfully returned
//
// Pre conditions (SetupL()):
// Empty homescreen
//
// Test step 1:
// Create CHsCcApiClient
//
// Expected result:
// Connection is successfully established
//
// Test step 2:
// Get app list
//
// Expected result:
// Content info list with application type content info
//
// Test step 3:
// Activate application type content info
//
// Expected result
// Application content info is activated
//
// Test step 4:
// Get active app
//
// Expected result:
// Content info with active application info
//
// Test step 5:
// Delete CHsCcApiClient
//
// Expected result:
// Connection is successfully closed
//
// Post conditions (Teardown()):
// -
//
// -----------------------------------------------------------------------
//
void CMTHsCcApiClient::ActiveAppL()
    {
    // Test step 1
    ConnectHsCcApiClientL( NULL );

    // Test step 2
    CHsContentInfoArray* apps = CHsContentInfoArray::NewL();
    CleanupStack::PushL( apps );
    TInt err = iApiClient->AppListL( *apps );
    User::LeaveIfError( err );
    // Get application content info which can be activated
    CHsContentInfo* info = NULL;
    if ( apps->Array().Count() )
        {
        info = apps->Array()[ 0 ];
        apps->Array().Remove( 0 );
        }
    apps->Array().ResetAndDestroy();
    User::LeaveIfNull( info );
    CleanupStack::PushL( info );

    // Test step 3
    err = iApiClient->ActivateAppL( *info );
    User::LeaveIfError( err );


    // Test step 4
    CHsContentInfo* app = CHsContentInfo::NewL();
    CleanupStack::PushL( app );

    err = iApiClient->ActiveAppL( *app );
    User::LeaveIfError( err );
    
    if ( app->Uid().CompareF( info->Uid() ) != 0 )
        {
        User::Leave( KErrGeneral );
        }

    CleanupStack::PopAndDestroy( app );
    
    CleanupStack::PopAndDestroy( info );
    CleanupStack::PopAndDestroy( apps );

    // Test step 5
    DisconnectHsCcApiClient();
    }

// -----------------------------------------------------------------------
// CMTHsCcApiClient::ActiveViewL()
// 
// Test purpose:
// The purpose of this test case is to verify that an active view is
// successfully returned
//
// Pre conditions (SetupL()):
// Empty homescreen
//
// Test step 1:
// Create CHsCcApiClient
//
// Expected result:
// Connection is successfully established
//
// Test step 2:
// Get active view
//
// Expected result:
// Active view successfully returned
//
// Test step 3:
// Delete CHsCcApiClient
//
// Expected result:
// Connection is successfully closed
//
// Post conditions (Teardown()):
// -
//
// -----------------------------------------------------------------------
//
void CMTHsCcApiClient::ActiveViewL()
    {
    // Test step 1
    ConnectHsCcApiClientL( NULL );

    // Test step 2
    CHsContentInfo* view = CHsContentInfo::NewL();
    CleanupStack::PushL( view );
    TInt err = iApiClient->ActiveViewL( *view );
    User::LeaveIfError( err );
    if ( view->Uid().CompareF( KHsViewUid ) != 0 )
        {
        User::Leave( KErrGeneral );
        }
    CleanupStack::PopAndDestroy( view );

    // Test step 7
    DisconnectHsCcApiClient();
    }

// -----------------------------------------------------------------------
// CMTHsCcApiClient::ViewWidgetList1L()
// 
// Test purpose:
// The purpose of this test case is to verify that an empty widget list
// is returned if the active view is empty
//
// Pre conditions (SetupL()):
// Empty homescreen
//
// Test step 1:
// Create CHsCcApiClient
//
// Expected result:
// Connection is successfully established
//
// Test step 2:
// Add empty view to Home screen
//
// Expected result:
// Empty view successfully added
//
// Test step 3:
// Activate added view
//
// Expected result:
// View is successfully activated
//
// Test step 4:
// Get widget list of active view
//
// Expected result:
// Empty widget list is returned
//
// Test step 5:
// Delete CHsCcApiClient
//
// Expected result:
// Connection is successfully closed
//
// Post conditions (Teardown()):
// -
//
// -----------------------------------------------------------------------
//
void CMTHsCcApiClient::ViewWidgetList1L()
    {
    // Test step 1
    ConnectHsCcApiClientL( NULL );

    // Test step 2
    CHsContentInfo* view = AddViewL( KTemplateViewUid );
    CleanupStack::PushL( view );

    // Test step 3
    TInt err = iApiClient->ActivateViewL( *view );
    User::LeaveIfError( err );

    // Test step 4
    CHsContentInfoArray* widgets = CHsContentInfoArray::NewL();
    CleanupStack::PushL( widgets );
    err = iApiClient->WidgetListL( *view, *widgets );
    User::LeaveIfError( err );

    if ( widgets->Array().Count() != 0 )
        {
        User::Leave( KErrGeneral );
        }
    CleanupStack::PopAndDestroy( widgets );
    CleanupStack::PopAndDestroy( view );

    // Test step 5
    DisconnectHsCcApiClient();
    }

// -----------------------------------------------------------------------
// CMTHsCcApiClient::ViewWidgetList2L()
// 
// Test purpose:
// The purpose of this test case is to verify that an empty widget list
// is returned if the active view is empty
//
// Pre conditions (SetupL()):
// Empty homescreen
//
// Test step 1:
// Create CHsCcApiClient
//
// Expected result:
// Connection is successfully established
//
// Test step 2:
// Add empty view to Home screen
//
// Expected result:
// Empty view successfully added
//
// Test step 3:
// Activate added view
//
// Expected result:
// View is successfully activated
//
// Test step 4:
// Add widget to Home screen view
//
// Expected result:
// Widget added successfully
//
// Test step 5:
// Get widget list of active view
//
// Expected result:
// Widget list including added widget
//
// Test step 6:
// Delete CHsCcApiClient
//
// Expected result:
// Connection is successfully closed
//
// Post conditions (Teardown()):
// -
//
// -----------------------------------------------------------------------
//
void CMTHsCcApiClient::ViewWidgetList2L()
    {
    // Test step 1
    ConnectHsCcApiClientL( NULL );

    // Test step 2
    CHsContentInfo* view = AddViewL( KTemplateViewUid );
    CleanupStack::PushL( view );

    // Test step 3
    TInt err = iApiClient->ActivateViewL( *view );
    User::LeaveIfError( err );

    // Test step 4
    CHsContentInfo* widget = AddWidgetL( KDesktopWidgetUid );
    CleanupStack::PushL( widget );

    // Test step 5
    CHsContentInfoArray* widgets = CHsContentInfoArray::NewL();
    CleanupStack::PushL( widgets );
    err = iApiClient->WidgetListL( *view, *widgets );
    User::LeaveIfError( err );

    err = KErrGeneral;
    if ( widgets->Array().Count() == 1 && 
         widgets->Array()[ 0 ]->PluginId().CompareF( widget->PluginId() ) ==
         0 && widgets->Array()[ 0 ]->Uid().CompareF( widget->Uid() ) == 0 )
        {
        err = KErrNone;
        }
    User::LeaveIfError( err );

    CleanupStack::PopAndDestroy( widgets );
    CleanupStack::PopAndDestroy( widget );
    CleanupStack::PopAndDestroy( view );

    // Test step 6
    DisconnectHsCcApiClient();
    }

// -----------------------------------------------------------------------
// CMTHsCcApiClient::ViewWidgetListFailsL()
// 
// Test purpose:
// The purpose of this test case is to verify that requesting of a
// widget list fails if the widget list is requested from a view which
// cannot be found
//
// Pre conditions (SetupL()):
// Empty homescreen
//
// Test step 1:
// Create CHsCcApiClient
//
// Expected result:
// Connection is successfully established
//
// Test step 2:
// Add empty view to Home screen
//
// Expected result:
// Empty view successfully added
//
// Test step 3:
// Get widget list of invalid view
//
// Expected result:
// Widget list request fails
//
// Test step 4:
// Delete CHsCcApiClient
//
// Expected result:
// Connection is successfully closed
//
// Post conditions (Teardown()):
// -
//
// -----------------------------------------------------------------------
//
void CMTHsCcApiClient::ViewWidgetListFailsL()
    {
    // Test step 1
    ConnectHsCcApiClientL( NULL );

    // Test step 2
    CHsContentInfo* view = AddViewL( KTemplateViewUid );
    CleanupStack::PushL( view );

    // Test step 3
    view->SetPluginIdL( KNotFoundViewPluginId );
    CHsContentInfoArray* widgets = CHsContentInfoArray::NewL();
    CleanupStack::PushL( widgets );
    TInt err = iApiClient->WidgetListL( *view, *widgets );
    if ( err != KErrArgument )
        {
        User::Leave( KErrGeneral );
        }

    CleanupStack::PopAndDestroy( widgets );
    CleanupStack::PopAndDestroy( view );

    // Test step 4
    DisconnectHsCcApiClient();
    }

// -----------------------------------------------------------------------
// CMTHsCcApiClient::AppWidgetList1L()
// 
// Test purpose:
// The purpose of this test case is to verify that correct list of widgets
// is returned when a widget list of an application configuration is
// requested
//
// Pre conditions (SetupL()):
// Empty homescreen
//
// Test step 1:
// Create CHsCcApiClient
//
// Expected result:
// Connection is successfully established
//
// Test step 2:
// Get active application configuration
//
// Expected result:
// Application configuration successfully received
//
// Test step 3:
// Get widget list of active application configuration
//
// Expected result:
// Widget list is successfully returned
//
// Test step 4:
// Add empty view to Home screen
//
// Expected result:
// Empty view successfully added
//
// Test step 5:
// Activate added view
//
// Expected result:
// View is successfully activated
//
// Test step 6:
// Add widget to Home screen view
//
// Expected result:
// Widget added successfully
//
// Test step 7:
// Get widget list of active application configuration
//
// Expected result:
// Widget list is successfully returned
//
// Test step 8:
// Delete CHsCcApiClient
//
// Expected result:
// Connection is successfully closed
//
// Post conditions (Teardown()):
// -
//
// -----------------------------------------------------------------------
//
void CMTHsCcApiClient::AppWidgetList1L()
    {
    // Test step 1
    ConnectHsCcApiClientL( NULL );

    // Test step 2
    CHsContentInfo* app = CHsContentInfo::NewL();
    CleanupStack::PushL( app );

    TInt err = iApiClient->ActiveAppL( *app );
    User::LeaveIfError( err );
    
    // Test step 3
    CHsContentInfoArray* widgets1 = CHsContentInfoArray::NewL();
    CleanupStack::PushL( widgets1 );
    err = iApiClient->WidgetListL( *app, *widgets1 );
    User::LeaveIfError( err );

    // Test step 4
    CHsContentInfo* view = AddViewL( KTemplateViewUid );
    CleanupStack::PushL( view );

    // Test step 5
    err = iApiClient->ActivateViewL( *view );
    User::LeaveIfError( err );
    CleanupStack::PopAndDestroy( view );

    // Test step 6
    CHsContentInfo* widget = AddWidgetL( KDesktopWidgetUid );
    CleanupStack::PushL( widget );
    widgets1->Array().AppendL( widget );
    CleanupStack::Pop( widget );

    // Test step 7
    CHsContentInfoArray* widgets2 = CHsContentInfoArray::NewL();
    CleanupStack::PushL( widgets2 );
    err = iApiClient->WidgetListL( *app, *widgets2 );
    User::LeaveIfError( err );

    err = KErrGeneral;
    if ( widgets1->Array().Count() == widgets2->Array().Count() )
        {
        err = KErrNone;
        for ( TInt i = 0; i < widgets1->Array().Count() &&
            err == KErrNone; i++ )
            {
            TBool found( EFalse );
            for ( TInt j = 0; j < widgets2->Array().Count() && !found; j++ )
                {
                if ( widgets1->Array()[ i ]->PluginId() ==
                    widgets2->Array()[ j ]->PluginId() )
                    {
                    found = ETrue;
                    }
                }
            if ( !found )
                {
                err = KErrGeneral;
                }
            }
        }

    CleanupStack::PopAndDestroy( widgets2 );
    CleanupStack::PopAndDestroy( widgets1 );
    CleanupStack::PopAndDestroy( app );

    // Test step 8
    DisconnectHsCcApiClient();
    }

// -----------------------------------------------------------------------
// CMTHsCcApiClient::AppWidgetListFailsL()
// 
// Test purpose:
// The purpose of this test case is to verify that requesting of a
// widget list fails if the widget list is requested from a application 
// configuration which cannot be found
//
// Pre conditions (SetupL()):
// Empty homescreen
//
// Test step 1:
// Create CHsCcApiClient
//
// Expected result:
// Connection is successfully established
//
// Test step 2:
// Get active application configuration
//
// Expected result:
// Application configuration successfully received
//
// Test step 3:
// Get widget list of invalid application configuration
//
// Expected result:
// Widget list request fails
//
// Test step 4:
// Delete CHsCcApiClient
//
// Expected result:
// Connection is successfully closed
//
// Post conditions (Teardown()):
// -
//
// -----------------------------------------------------------------------
//
void CMTHsCcApiClient::AppWidgetListFailsL()
    {
    // Test step 1
    ConnectHsCcApiClientL( NULL );

    // Test step 2
    CHsContentInfo* app = CHsContentInfo::NewL();
    CleanupStack::PushL( app );

    TInt err = iApiClient->ActiveAppL( *app );
    User::LeaveIfError( err );

    // Test step 3
    CHsContentInfoArray* widgets = CHsContentInfoArray::NewL();
    CleanupStack::PushL( widgets );
    app->SetUidL( KNotFoundAppUid );
    err = iApiClient->WidgetListL( *app, *widgets );
    if ( err != KErrArgument )
        {
        User::Leave( KErrGeneral );
        }

    CleanupStack::PopAndDestroy( widgets );
    CleanupStack::PopAndDestroy( app );

    // Test step 4
    DisconnectHsCcApiClient();
    }

// -----------------------------------------------------------------------
// CMTHsCcApiClient::AppViewList1L()
// 
// Test purpose:
// The purpose of this test case is to verify that correct view list is
// returned when there is mandatory views included in active application 
// configuration
//
// Pre conditions (SetupL()):
// Empty homescreen
//
// Test step 1:
// Create CHsCcApiClient
//
// Expected result:
// Connection is successfully established
//
// Test step 2:
// Get active application configuration
//
// Expected result:
// Application configuration is successfully returned
//
// Test step 3:
// Get view list from active application configuration
//
// Expected result:
// View list is successfully returned
//
// Test step 4:
// Delete CHsCcApiClient
//
// Expected result:
// Connection is successfully closed
//
// Post conditions (Teardown()):
// -
//
// -----------------------------------------------------------------------
//
void CMTHsCcApiClient::AppViewList1L()
    {
    // Test step 1
    ConnectHsCcApiClientL( NULL );

    // Test step 2
    CHsContentInfo* app = CHsContentInfo::NewL();
    CleanupStack::PushL( app );

    TInt err = iApiClient->ActiveAppL( *app );
    User::LeaveIfError( err );

    // Test step 3
    CHsContentInfoArray* views = CHsContentInfoArray::NewL();
    CleanupStack::PushL( views );

    err = iApiClient->ViewListL( *app, *views );
    User::LeaveIfError( err );

    for ( TInt i = 0; i < views->Array().Count(); i++ )
        {
        CHsContentInfo* view = views->Array()[ i ];
        if ( view->Type().Compare( KInfoTypeView ) != 0 )
            {
            User::Leave( KErrGeneral );
            }
        }

    CleanupStack::PopAndDestroy( views );
    CleanupStack::PopAndDestroy( app );

    // Test step 4
    DisconnectHsCcApiClient();
    }

// -----------------------------------------------------------------------
// CMTHsCcApiClient::AppViewList2L()
// 
// Test purpose:
// The purpose of this test case is to verify that correct view list is
// returned when there is maximum count of views in application 
// configuration
//
// Pre conditions (SetupL()):
// Empty homescreen
//
// Test step 1:
// Create CHsCcApiClient
//
// Expected result:
// Connection is successfully established
//
// Test step 2:
// Add test views
//
// Expected result:
// Views added successfully
//
// Test step 3:
// Get active application configuration
//
// Expected result:
// Application configuration is successfully returned
//
// Test step 4:
// Get view list from active application configuration
//
// Expected result:
// View list is successfully returned
//
// Test step 5:
// Delete CHsCcApiClient
//
// Expected result:
// Connection is successfully closed
//
// Post conditions (Teardown()):
// -
//
// -----------------------------------------------------------------------
//
void CMTHsCcApiClient::AppViewList2L()
    {
    // Test step 1
    ConnectHsCcApiClientL( NULL );

    // Test step 2
    TInt err( KErrNone );
    CHsContentInfo* view( NULL );
    while ( !err )
        {
        TRAP( err, view = AddViewL( KTemplateViewUid ) );
        delete view;
        view = NULL;
        }

    // Test step 3
    CHsContentInfo* app = CHsContentInfo::NewL();
    CleanupStack::PushL( app );

    err = iApiClient->ActiveAppL( *app );
    User::LeaveIfError( err );

    // Test step 4
    CHsContentInfoArray* views = CHsContentInfoArray::NewL();
    CleanupStack::PushL( views );

    err = iApiClient->ViewListL( *app, *views );
    User::LeaveIfError( err );

    for ( TInt i = 0; i < views->Array().Count(); i++ )
        {
        CHsContentInfo* view = views->Array()[ i ];
        if ( view->Type().Compare( KInfoTypeView ) != 0 )
            {
            User::Leave( KErrGeneral );
            }
        }

    CleanupStack::PopAndDestroy( views );
    CleanupStack::PopAndDestroy( app );

    // Test step 5
    DisconnectHsCcApiClient();
    }

// -----------------------------------------------------------------------
// CMTHsCcApiClient::AppViewListFails1L()
// 
// Test purpose:
// The purpose of this test case is to verify that requesting of a
// view list fails if the view list is requested from a application 
// configuration which cannot be found
//
// Pre conditions (SetupL()):
// Empty homescreen
//
// Test step 1:
// Create CHsCcApiClient
//
// Expected result:
// Connection is successfully established
//
// Test step 1:
// Create CHsCcApiClient
//
// Expected result:
// Connection is successfully established
//
// Test step 2:
// Get active application configuration
//
// Expected result:
// Application configuration is successfully returned
//
// Test step 3:
// Get view list from invalid active application configuration
//
// Expected result:
// Request fails
//
// Test step 4:
// Delete CHsCcApiClient
//
// Expected result:
// Connection is successfully closed
//
// Post conditions (Teardown()):
// -
//
// -----------------------------------------------------------------------
//
void CMTHsCcApiClient::AppViewListFails1L()
    {
    // Test step 1
    ConnectHsCcApiClientL( NULL );

    // Test step 2
    CHsContentInfo* app = CHsContentInfo::NewL();
    CleanupStack::PushL( app );

    TInt err = iApiClient->ActiveAppL( *app );
    User::LeaveIfError( err );

    // Test step 3
    app->SetUidL( KNotFoundAppUid );
    CHsContentInfoArray* views = CHsContentInfoArray::NewL();
    CleanupStack::PushL( views );

    err = iApiClient->ViewListL( *app, *views );

    if ( err != KErrArgument )
        {
        User::Leave( KErrGeneral );
        }

    CleanupStack::PopAndDestroy( views );
    CleanupStack::PopAndDestroy( app );

    // Test step 4
    DisconnectHsCcApiClient();
    }

//------------------------------------------------------------------------------
// Test case table
//------------------------------------------------------------------------------
EUNIT_BEGIN_TEST_TABLE(
    CMTHsCcApiClient,
    "This is a test suite for Home screen Content Control API Client",
    "UNIT" )

    EUNIT_TEST(
        "Connect/disconnect",
        "CHsCcApiClient",
        "",
        "FUNCTIONALITY",
        SetupL, ConnectDisconnectHsCcApiClientL, Teardown )

    EUNIT_TEST(
        "Register/Unregister observer",
        "CHsCcApiClient",
        "",
        "FUNCTIONALITY",
        SetupL, RegisterUnregisterHsCcApiObserverL, Teardown )

    EUNIT_TEST(
        "Register/Unregister multiple observers",
        "CHsCcApiClient",
        "",
        "FUNCTIONALITY",
        SetupL, RegisterUnregisterMultipleObserversL, Teardown )

    EUNIT_TEST(
        "Widget List",
        "CHsCcApiClient",
        "",
        "FUNCTIONALITY",
        SetupL, WidgetListL, Teardown )

    EUNIT_TEST(
        "Add/Remove Widget",
        "CHsCcApiClient",
        "",
        "FUNCTIONALITY",
        SetupL, AddRemoveWidgetL, Teardown )

    EUNIT_TEST(
        "Add Widget fails 1",
        "CHsCcApiClient",
        "",
        "FUNCTIONALITY",
        SetupL, AddWidgetFails1L, Teardown )

    EUNIT_TEST(
        "Remove Widget fails 1",
        "CHsCcApiClient",
        "",
        "FUNCTIONALITY",
        SetupL, RemoveWidgetFails1L, Teardown )

    EUNIT_TEST(
        "Widget list change notification",
        "CHsCcApiClient",
        "",
        "FUNCTIONALITY",
        SetupL, WidgetListChangeNtfL, Teardown )

    EUNIT_TEST(
        "View List",
        "CHsCcApiClient",
        "",
        "FUNCTIONALITY",
        SetupL, ViewListL, Teardown )

    EUNIT_TEST(
        "Add/Remove View",
        "CHsCcApiClient",
        "",
        "FUNCTIONALITY",
        SetupL, AddRemoveViewL, Teardown )

    EUNIT_TEST(
        "Activate View",
        "CHsCcApiClient",
        "",
        "FUNCTIONALITY",
        SetupL, ActivateViewL, Teardown )

    EUNIT_TEST(
        "Add View fails 1",
        "CHsCcApiClient",
        "",
        "FUNCTIONALITY",
        SetupL, AddViewFails1L, Teardown )

    EUNIT_TEST(
        "Remove View fails 1",
        "CHsCcApiClient",
        "",
        "FUNCTIONALITY",
        SetupL, RemoveViewFails1L, Teardown )

    EUNIT_TEST(
        "View list change notification",
        "CHsCcApiClient",
        "",
        "FUNCTIONALITY",
        SetupL, ViewListChangeNtfL, Teardown )

    EUNIT_TEST(
        "App list",
        "CHsCcApiClient",
        "",
        "FUNCTIONALITY",
        SetupL, AppListL, Teardown )

    EUNIT_TEST(
        "Activate App",
        "CHsCcApiClient",
        "",
        "FUNCTIONALITY",
        SetupL, ActivateAppL, Teardown )

    EUNIT_TEST(
        "Active App",
        "CHsCcApiClient",
        "",
        "FUNCTIONALITY",
        SetupL, ActiveAppL, Teardown )

    EUNIT_TEST(
        "Active View",
        "CHsCcApiClient",
        "",
        "FUNCTIONALITY",
        SetupL, ActiveViewL, Teardown )

    EUNIT_TEST(
        "Widget list of a view",
        "CHsCcApiClient",
        "",
        "FUNCTIONALITY",
        SetupL, ViewWidgetList1L, Teardown )

    EUNIT_TEST(
        "Widget list of a view",
        "CHsCcApiClient",
        "",
        "FUNCTIONALITY",
        SetupL, ViewWidgetList2L, Teardown )

    EUNIT_TEST(
        "Widget list of a view fails",
        "CHsCcApiClient",
        "",
        "FUNCTIONALITY",
        SetupL, ViewWidgetListFailsL, Teardown )

    EUNIT_TEST(
        "Widget list of an app",
        "CHsCcApiClient",
        "",
        "FUNCTIONALITY",
        SetupL, AppWidgetList1L, Teardown )
    
    EUNIT_TEST(
        "Widget list of an app fails",
        "CHsCcApiClient",
        "",
        "FUNCTIONALITY",
        SetupL, AppWidgetListFailsL, Teardown )

    EUNIT_TEST(
        "View list of an app mandatory views",
        "CHsCcApiClient",
        "",
        "FUNCTIONALITY",
        SetupL, AppViewList1L, Teardown )

    EUNIT_TEST(
        "View list of an app view list full",
        "CHsCcApiClient",
        "",
        "FUNCTIONALITY",
        SetupL, AppViewList2L, Teardown )

    EUNIT_TEST(
        "View list fails invalid app",
        "CHsCcApiClient",
        "",
        "FUNCTIONALITY",
        SetupL, AppViewListFails1L, Teardown )

    EUNIT_END_TEST_TABLE

// End of file
