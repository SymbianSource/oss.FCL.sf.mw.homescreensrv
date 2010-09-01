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
* Description: EUnit unit test class for CAiStateProvider
*
*/


#ifndef _UT_AISTATEPROVIDER_H
#define _UT_AISTATEPROVIDER_H

// System includes
#include <e32base.h>
#include <digia/eunit/ceunittestsuiteclass.h>

// User includes 
#include <aifwdefs.h>
#include "aistateobserver.h"

// Forward declarations
class CAiStateProvider;
class THsPublisherInfo;

// Clas declaration
NONSHARABLE_CLASS( UT_AiStateProvider ) : public CEUnitTestSuiteClass,
    public MAiStateObserver
    {
public:
    // Constructors and destructors

    /**
     * Two phase construction
     */
    static UT_AiStateProvider* NewL();

    /**
     * Two phase construction
     */    
    static UT_AiStateProvider* NewLC();
    
    /**
     * Destructor
     */
    ~UT_AiStateProvider();

private: 
    // Constructors and destructors

    UT_AiStateProvider();
    
    void ConstructL();

private:
    // from MAiStateObserver
    
    /**
      * @see MAiStateObserver
      */
    void NotifyStateChange( TAiFwState aState );
    
    /**
     * @see MAiStateObserver
     */
    void NotifyLoadPlugin( const TAiFwPublisherInfo& aInfo );
    
    /**
     * @see MAiStateObserver
     */
    void NotifyDestroyPlugin( const TAiFwPublisherInfo& aInfo );
    
    /**
     * @see MAiStateObserver
     */
    //void NotifyUpdatePlugins();

    /**
     * Notifies to reload previously released plugins
     * 
     * @since S60 5.2
     */
    void NotifyReloadPlugins();

    /**
     * Notifies that defined ECom plugins should be released to enable
     * plugin upgrade
     * 
     * @since S60 5.2
     */
    void NotifyReleasePlugins( const RArray<TUid>& aUidList );

private:
    // New methods

    void SetupL();        
    void Teardown();
        
    void TestStartStopL();
    void TestFgBgStateL();
    void TestLightStateL();
    void TestSkinChangeL();
    void TestEcomRegistryChangeL();
    void TestLoadDestroyPluginL();
    void TestChangePluginStateL();
    void TestBackupRestoreStateChangeL();
    void SwiUidListEventL();
    
private:    
    // data

    EUNIT_DECLARE_TEST_TABLE; 
    
    CAiStateProvider* iProvider;
    
    TAiFwState iExpectedState;
    TBool iIgnoreNotifyStateChange;
    TBool iPluginsUpdated;
    
    };

#endif //  _UT_AISTATEPROVIDER_H

// End of file
