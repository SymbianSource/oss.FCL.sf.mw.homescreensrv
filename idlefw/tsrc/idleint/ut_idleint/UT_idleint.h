/*
* Copyright (c) 2004 Nokia Corporation and/or its subsidiary(-ies).
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


#ifndef __UT_IDLEINT_H__
#define __UT_IDLEINT_H__

// System includes
#include <e32def.h>
#include <digia/eunit/ceunittestsuiteclass.h>

// User includes
#include <hscontentpublisher.h>
#include "AknSoundSystem.h"
#include "aifweventhandler.h"
#include "aifwdefs.h"

// Forward declarations
class CAiUiIdleIntegrationImpl;
class CAiWsPluginManager;
class CAiIdleAppRegister;
class CActiveIdleState;
class CPSPropertyObserver;
class MAiPSPropertyObserver;

// Class declaration
NONSHARABLE_CLASS( UT_IdleInt ) : public CEUnitTestSuiteClass, 
    public MAiFwEventHandler    
    {
public:    
    // Constructors and destructors

    /**
    * Two phase construction
    */
    static UT_IdleInt* NewL();

    static UT_IdleInt* NewLC();

    /**
    * Destructor
    */
    ~UT_IdleInt();

private:    
    // Constructors and destructors

    UT_IdleInt();

    void ConstructL();

private:  
    // From MAiFwEventHandler    

    void AppEnvReadyL();

    void HandleUiReadyEventL( CAiUiController& aUiController );

    void HandlePluginEvent( const TDesC& aParam );

    TBool HasMenuItem( const TDesC& aPluginName, const TDesC& aMenuItem );

    void HandleUiLayoutChangeL( CAiUiController& aUiController );

    void HandleUiThemeChangeStartL( CAiUiController& aUiController );

    void HandleUiShutdown( CAiUiController& aUiController );

    void HandleAllPluginsLoaded();

    void HandleEnterEditMode();

    void HandleExitEditMode();

    void HandleUIChangeEventL();

    void HandleActivateUI();

    void HandleLoadPluginL( const THsPublisherInfo& aPublisherInfo );

    void HandleDestroyPluginL( const THsPublisherInfo& aPublisherInfo );

    void HandlePluginEventL( const THsPublisherInfo& aPublisherInfo, const TDesC& aParam );

    TBool HasMenuItemL( const THsPublisherInfo& aPublisherInfo, const TDesC& aMenuItem );

    /**
    * Checks the internet using plugins status
    */
    TBool IsPluginsOnline();

    /**
    * Sets the internet using plugins status
    */
    void SetPluginsOnlineStatus(TBool aStatus);

    /**
    * Used for quering if network is in roaming mode
    */
    TBool IsRoaming();

    /**
    * Displays roaming-notification
    */
    void ShowRoamingNotificationL();

    TBool RefreshContent( const TDesC& aContentCid );
    
    TBool RefreshContent( const THsPublisherInfo& aPublisherInfo, const TDesC& aContentCid );
    
    TBool SuspendContent( const THsPublisherInfo& aPublisherInfo, const TDesC& aContentCid );

    // void DestroyingPlugin( /*CAiContentPublisher*/CHsContentPublisher& aPlugin );

    void CriticalStartupPhaseOver( TInt aStageInteger );

    TBool QueryIsMenuOpen();
    
    // void ProcessStateChange( TAifwStates aState );

    void StoreUserOnlineSelection (TBool /*aStatus*/){};

		/*
    void HandlePluginsOnlineStatus( 
            const RAiPublisherInfoArray& aPlugins,TBool aStatus );
		*/
		
    void SetPluginsVisibility( TBool aStatus );

    void ProcessOnlineState( TBool aOnline );

public:    // New methods

    static CPSPropertyObserver* PSPropertyObserver( TUid aCategory,
    TInt aKey);

    static void KeySetTo( TInt aValue );

private:    // New methods

    void SetupL();

    void Teardown();

    void TestIdleIntEventControlNullAndOthersL();

    void TestStateChangesL();

    void TestStartUpL();

private:    // Data

    EUNIT_DECLARE_TEST_TABLE;

    CAiUiIdleIntegrationImpl* iIdleInt;

    CAiWsPluginManager* iAiWsPluginManagerImpl;

    CAiIdleAppRegister* iAiIdleAppRegister;

    CActiveIdleState * iState;

    CAknKeySoundSystem iSoundSystemStub;

    public:
    /**
    * Observer for phone state Publish&Subscribe key.
    */
    CPSPropertyObserver* iPhoneStatusObserver;

    /**
    * Observer for system state Publish&Subscribe key.
    */
    CPSPropertyObserver* iSystemStatusObserver;

    /**
    * Observer for UI startup state Publish&Subscribe key.
    */
    CPSPropertyObserver* iUiStartupStatusObserver;

    TInt iIdleStatus;

    TBool iPhoneStatusOwned;

    TBool iSystemStatusOwned;

    TBool iUiStartupStatusOwned;

    };

#endif    //  __UT_IDLEINT_H__

// End of file
