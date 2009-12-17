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
#ifndef TESTCLASS_H
#define TESTCLASS_H

//  INCLUDES



#include <e32svr.h>
#include <e32math.h>
#include <f32file.h>
#include <StifParser.h>
#include <Stiftestinterface.h>
#include <StifLogger.h>
#include <TestScripterInternal.h>
#include <StifTestModule.h>

#include <mcsmenu.h>

#include "testutils.h"

// DATA TYPES
//enum ?declaration
//typedef ?declaration
//extern ?data_type;

// CLASS DECLARATION

/**
*  CtestClass test class for STIF Test Framework TestScripter.
*  ?other_description_lines
*
*  @lib ?library
*  @since ?Series60_version
*/
NONSHARABLE_CLASS(CMCSAPITest) : public CScriptBase
    {
    public:  // Constructors and destructor
        
        /**
        * Two-phased constructor.
        */
        static CMCSAPITest* NewL( CTestModuleIf& aTestModuleIf );
       
        /**
        * Destructor.
        */
        virtual ~CMCSAPITest();

    public: // New functions
        
        /**
        * ?member_description.
        * @since ?Series60_version
        * @param ?arg1 ?description
        * @return ?description
        */
        //?type ?member_function( ?type ?arg1 );

    public: // Functions from base classes

        /**
        * From CScriptBase Runs a script line.
        * @since ?Series60_version
        * @param aItem Script line containing method name and parameters
        * @return Symbian OS error code
        */
        virtual TInt RunMethodL( CStifItemParser& aItem );
                
    protected:  // New functions
        
        /**
        * ?member_description.
        * @since ?Series60_version
        * @param ?arg1 ?description
        * @return ?description
        */
        //?type ?member_function( ?type ?arg1 );

    protected:  // Functions from base classes
        
        /**
        * From ?base_class ?member_description
        */
        //?type ?member_function();

    private:

        /**
        * C++ default constructor.
        */
        CMCSAPITest( CTestModuleIf& aTestModuleIf );

        /**
        * By default Symbian 2nd phase constructor is private.
        */
        void ConstructL();
        
        /**
        * Test methods are listed below. 
        */
        TInt Test_RMenuConstructorL(CStifItemParser& /*aItem*/ );
        TInt Test_RMenuCloseL(CStifItemParser& /*aItem*/ );
        TInt Test_RMenuOpenL(CStifItemParser& /*aItem*/ );
        TInt Test_RMenuRootFolderL(CStifItemParser& /*aItem*/ );
        TInt Test_RMenuRootGetItemsL(CStifItemParser& /*aItem*/ );
        TInt Test_RMenuGetRunningAppsL(CStifItemParser& /*aItem*/ );
        TInt Test_RMenuRemoveL(CStifItemParser& /*aItem*/ );
        TInt Test_RMenuMoveToFolderL(CStifItemParser& /*aItem*/ );
        TInt Test_RMenuReorderL(CStifItemParser& /*aItem*/ );
        TInt Test_RMenuResourceMarkL(CStifItemParser& /*aItem*/ );
        TInt Test_RMenuResourceCheckL(CStifItemParser& /*aItem*/ );
        TInt Test_RMenuResourceCountL(CStifItemParser& /*aItem*/ );
        TInt Test_RMenu__DbgSetAllocFailL(CStifItemParser& /*aItem*/ );
        
        
        TInt Test_TMenuItemConstructorL(CStifItemParser& /*aItem*/ );
        TInt Test_TMenuItemId(CStifItemParser& /*aItem*/ );
        TInt Test_TMenuItemParent(CStifItemParser& /*aItem*/ );
        TInt Test_TMenuItemFlags(CStifItemParser& /*aItem*/ );
        TInt Test_TMenuItemType(CStifItemParser& /*aItem*/ );
        TInt Test_TMenuItemSetId(CStifItemParser& /*aItem*/ );
        TInt Test_TMenuItemSetParent(CStifItemParser& /*aItem*/ );
        TInt Test_TMenuItemSetFlags(CStifItemParser& /*aItem*/ );
        TInt Test_TMenuItemSetType(CStifItemParser& /*aItem*/ );
        
        TInt Test_CMenuItemCreateL(CStifItemParser& /*aItem*/ );
        TInt Test_CMenuItemOpenIdL(CStifItemParser& /*aItem*/ );
        TInt Test_CMenuItemOpenHdrL(CStifItemParser& /*aItem*/ );
        TInt Test_CMenuItemIdL(CStifItemParser& /*aItem*/ );
        TInt Test_CMenuItemParentL(CStifItemParser& /*aItem*/ );
        TInt Test_CMenuItemFlagsL(CStifItemParser& /*aItem*/ );
        TInt Test_CMenuItemSetFlagsL(CStifItemParser& /*aItem*/ );
        TInt Test_CMenuItemTypeL(CStifItemParser& /*aItem*/ );
        TInt Test_CMenuItemGetAttributeL(CStifItemParser& /*aItem*/ );
        TInt Test_CMenuItemGetAttributeListL(CStifItemParser& /*aItem*/ );
        TInt Test_CMenuItemRunningStatusL(CStifItemParser& /*aItem*/ );
        TInt Test_CMenuItemSetAttributeL(CStifItemParser& /*aItem*/ );
        TInt Test_CMenuItemRemoveAttributeL(CStifItemParser& /*aItem*/ );
        TInt Test_CMenuItemSaveL(CStifItemParser& /*aItem*/ );
        TInt Test_CMenuItemHandleCommandL(CStifItemParser& /*aItem*/ );
        
        TInt Test_RMenuNotifierConstructorL(CStifItemParser& /*aItem*/ );
        TInt Test_RMenuNotifierOpenL(CStifItemParser& /*aItem*/ );
        TInt Test_RMenuNotifierNotifyL(CStifItemParser& /*aItem*/ );
        TInt Test_RMenuNotifierCloseL(CStifItemParser& /*aItem*/ );
        TInt Test_RMenuNotifierCancelL(CStifItemParser& /*aItem*/ );
        
        TInt Test_CMenuFilterDestructorL(CStifItemParser& /*aItem*/ );
        TInt Test_CMenuFilterNewL(CStifItemParser& /*aItem*/ );
        TInt Test_CMenuFilterNewLC(CStifItemParser& /*aItem*/ );
        TInt Test_CMenuFilterFlagsOn(CStifItemParser& /*aItem*/ );
        TInt Test_CMenuFilterFlagsOff(CStifItemParser& /*aItem*/ );
        TInt Test_CMenuFilterType(CStifItemParser& /*aItem*/ );
        TInt Test_CMenuFilterNumAttributes(CStifItemParser& /*aItem*/ );
        TInt Test_CMenuFilterGetAttribute(CStifItemParser& /*aItem*/ );
        TInt Test_CMenuFilterReset(CStifItemParser& /*aItem*/ );
        TInt Test_CMenuFilterSetFlags(CStifItemParser& /*aItem*/ );
        TInt Test_CMenuFilterSetType(CStifItemParser& /*aItem*/ );
        TInt Test_CMenuFilterHaveAttributeL(CStifItemParser& /*aItem*/ );
        TInt Test_CMenuFilterDoNotHaveAttributeL(CStifItemParser& /*aItem*/ );
        TInt Test_CMenuFilterRemoveAttribute(CStifItemParser& /*aItem*/ );
        TInt Test_McsIconUtilityGetItemIconL(CStifItemParser& /*aItem*/ );
        TInt Test_MenuUtilsGetTUint(CStifItemParser& /*aItem*/ );
        TInt Test_MenuUtilsSetTUint(CStifItemParser& /*aItem*/ );
        
        TInt Test_MenuHandlerPluginNewL(CStifItemParser& /*aItem*/ );
        TInt Test_MenuHandlerPluginDeleteL(CStifItemParser& /*aItem*/ );
        TInt Test_MenuHandlerPluginConstructL(CStifItemParser& /*aItem*/ );
        
        TInt Test_CMenuHandlerSupportsType(CStifItemParser& /*aItem*/ );
        TInt Test_CMenuHandlerHandleCommandL(CStifItemParser& /*aItem*/ );
        TInt Test_CMenuHandlerDestructor(CStifItemParser& /*aItem*/ );
        
        /**
        * Utility methods are listed below. 
        */
        TInt SetupMenuL(RMenu& aMenu);
        void WaitL(TInt aMicroSec);
        TInt SaveL(CMenuItem *aItem);
        
    public:     // Data
        // ?one_line_short_description_of_data
        //?data_declaration;
    
    protected:  // Data
        // ?one_line_short_description_of_data
        //?data_declaration;

    private:    // Data
    	CTestUtils *iTestUtils;
    	TInt iSetupSuffix;
        
    };

#endif      // TESTCLASS_H
            
// End of File
