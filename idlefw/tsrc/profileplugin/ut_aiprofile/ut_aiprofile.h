/*
* Copyright (c) 2005-2006 Nokia Corporation and/or its subsidiary(-ies).
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


#ifndef __UT_AIPROFILE_H__
#define __UT_AIPROFILE_H__

//  EXTERNAL INCLUDES
#include <digia/eunit/ceunittestsuiteclass.h>
#include <apgcli.h>         //RApaLsSession
#include <w32std.h>                 // For RWsSession

//  INTERNAL INCLUDES
#include <hscontentpublisher.h>

//  FORWARD DECLARATIONS
class CAiProfileEngine;
class CAiProfilePlugin;
class CAiContentPublisher;
class ProfileObserver;

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
NONSHARABLE_CLASS( UT_aiprofile ) : public CEUnitTestSuiteClass
{
public:     // Constructors and destructors

    /**
     * Two phase construction
     */
    static UT_aiprofile* NewL();
    static UT_aiprofile* NewLC();
    /**
     * Destructor
     */
    ~UT_aiprofile();

private:    // Constructors and destructors

    UT_aiprofile();
    void ConstructL();

public:     // From observer interface



private:    // New methods

    void SetupL();
    void Teardown();
	void UT_resumeL();
	// void UT_SetPropertyLL();
	void UT_HandleEventL();
	void UT_NotifyContentUpdateL();
	void UT_ExtensionL();
	void UT_ConfigureLL();
	void AddSettingL(RAiSettingsItemArray& aArray, const TInt32 aKey, const TDesC& aValue);

private:    // Data

	CAiProfilePlugin* iProfilePlugin;
    // CHsContentPublisher* iProfilePlugin;
	
    EUNIT_DECLARE_TEST_TABLE;
    ProfileObserver*         iContentObserver;
    

};

#endif      //  __UT_AIPROFILE_H__

// End of file
