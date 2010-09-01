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
* Description:  EUnit module test class for Homescreen Content Control
*               server
*
*/

#ifndef MTHSCCAPICLIENT_H
#define MTHSCCAPICLIENT_H

// System includes
#include <e32base.h>
#include <digia/eunit/ceunittestsuiteclass.h>
#include <digia/eunit/eunitmacros.h>
#include <hscontentcontroller.h>
#include <hscontentcontrol.h>

// Forward declarations
class CHsCcApiClient;
class CHsContentInfo;
class CHsContentInfoArray;

class CMTHsCcApiClient : public CEUnitTestSuiteClass, public MHsContentControl
    {

public: // Constructors and destructors
    /**
     * Two-phased constructor.
     */
    static CMTHsCcApiClient* NewL();

    /**
     * Destructor.
     */
    virtual ~CMTHsCcApiClient();

private: // Constructors
    /**
     * Constructor
     */
    CMTHsCcApiClient();

    /**
     * Second phase constructor 
     */
    void ConstructL();

public: // from MHsContentControl
    
    /**
     *
     */
    void NotifyWidgetListChanged();

    /**
     *
     */
    void NotifyViewListChanged();

    /**
     *
     */
    void NotifyAppListChanged();

private: // from CEUnitTestSuiteClass

    /**
     * Setup for the unit test.
     */
    void SetupL();

    /**
     * Unit test teardown.
     */
    void Teardown();

    private: // test step functions

    void ConnectHsCcApiClientL( MHsContentControl* aObserver );

    void DisconnectHsCcApiClient();

    CHsContentInfo* AddViewL( const TDesC8& aUid );

    CHsContentInfo* AddWidgetL( const TDesC8& aUid );

    CHsContentInfo* GetViewL( CHsContentInfo* aApp, const TDesC8& aUid );

    CHsContentInfo* GetWidgetL( CHsContentInfo* aView, const TDesC8& aUid );

private: // test case functions

    void ConnectDisconnectHsCcApiClientL();

    void RegisterUnregisterHsCcApiObserverL();

    void RegisterUnregisterMultipleObserversL();

    void WidgetListL();

    void AddRemoveWidgetL();

    void AddWidgetFails1L();

    void RemoveWidgetFails1L();

    void WidgetListChangeNtfL();

    void ViewListL();

    void AddRemoveViewL();

    void ActivateViewL();

    void AddViewFails1L();

    void RemoveViewFails1L();

    void ViewListChangeNtfL();

    void AppListL();

    void ActivateAppL();

    void ActiveAppL();

    void ActiveAppFails1L();

    void ActiveViewL();

    void ViewWidgetList1L();

    void ViewWidgetList2L();

    void ViewWidgetListFailsL();

    void AppWidgetList1L();

    void AppWidgetListFailsL();

    void AppViewList1L();

    void AppViewList2L();

    void AppViewListFails1L();

private: // Data

    /**
     * Homescreen content control api client
     */
    CHsCcApiClient* iApiClient;

    /**
     * Widget list notification expected
     */
    TBool iWidgetListNtfExpected;

    /**
     * Widget list notification received
     */
    TBool iWidgetListNtfReceived;

    /**
     * View list notification expected
     */
    TBool iViewListNtfExpected;

    /**
     * View list notification received
     */
    TBool iViewListNtfReceived;

    /**
     * Application list notification expected
     */
    TBool iAppListNtfExpected;

    /**
     * Application list notification received
     */
    TBool iAppListNtfReceived;

    CActiveSchedulerWait iWait;

    /**
     * Unit test data
     */
    EUNIT_DECLARE_TEST_TABLE;

    };

#endif // MTHSCCAPICLIENT_H

// End of file
