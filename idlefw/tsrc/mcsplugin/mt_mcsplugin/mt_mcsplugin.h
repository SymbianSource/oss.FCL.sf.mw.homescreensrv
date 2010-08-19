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


#ifndef __MT_MCSPLUGIN_H__
#define __MT_MCSPLUGIN_H__

//  EXTERNAL INCLUDES
#include <digia/eunit/ceunittestsuiteclass.h>

// #include <aiscutdefs.h>
#include <hscontentpublisher.h>
// #include <aipropertyextension.h>
// #include <aieventhandlerextension.h>
#include <apgcli.h>
#include <aknappui.h>
#include <aiutility.h>
#include <aknkeylock.h>
#include <mcsplugin.h>

//  INTERNAL INCLUDES

//  FORWARD DECLARATIONS
class MAiPluginSettings;
class MAiPluginSettingsItem;
class CRepository;
class CAiContentPublisher;
class Observer;
// class MAiPropertyExtension;
class CAiCallStatusObserver;
class CAiFw;
class CAiSystemStateManager;
class CAiPluginActivityRegistry;
class CAiBackupRestoreStatusObserver;
class CAiSystemStateManager;

typedef RPointerArray<MAiPluginSettings> RAiSettingsItemArray;

#include <e32def.h>
#include <msvapi.h>

#ifndef NONSHARABLE_CLASS
#define NONSHARABLE_CLASS(x) class x
#endif

//  CLASS DEFINITION
/**
 *
 * EUnitWizard generated test class.
 *
 */
NONSHARABLE_CLASS( MT_mcsplugin )
    : public CEUnitTestSuiteClass,
    public MMsvSessionObserver  //, public CAknAppUi
{
public:     // Constructors and destructors

    /**
     * Two phase construction
     */
    /*IMPORT_C*/ static MT_mcsplugin* NewL();

    /*IMPORT_C*/ static MT_mcsplugin* NewLC();

    /**
     * Destructor
     */
    ~MT_mcsplugin();

private:    // Constructors and destructors

    MT_mcsplugin();

    void ConstructL();

public:     // From observer interface

private:    // New methods

    void HandleSessionEventL(TMsvSessionEvent aEvent, TAny* aArg1, TAny* aArg2, TAny* aArg3);

    /*################# SetupL ####################
     Called everytime at first
     to prepare plugin for test
     #############################################*/
    void SetupL();

    void EmptySetupL();

    /*############### TeardownL ###################
     Called everytime to deconstruct the plugin
     #############################################*/
    void Teardown();

    /*############### TestResumeL #################
     Tests ResumeL from caiscutplugin,
     ResumeL is called in SetupL but in this
     function plugin is suspended before
     #############################################*/
    void TestResumeL();

    /*############## TestSuspendL ################
     Tests if SuspendL pass
     #############################################*/
    void TestSuspendL();

    /*############## TestSubscribeL ###############
     Tests SubscribeL
     #############################################*/
    void TestSubscribeL();

    /*############# TestGetPropertyL ##############
     Tests getting property information from plugin
     #############################################*/
    void TestGetPropertyL();

    void TestFreeEngineL();

    void TestHandleEventErrorHandling();

private:    // Data

    EUNIT_DECLARE_TEST_TABLE;

    // CAiContentPublisher*                iPlugin;
    CHsContentPublisher*                iPlugin;

    Observer*                           iContentObserver;

    // MAiEventHandlerExtension*           iPluginEventHandler;
};

#endif      //  __MT_MCSPLUGIN_H__

// End of file
