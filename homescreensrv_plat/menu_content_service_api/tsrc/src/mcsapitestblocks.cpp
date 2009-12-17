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
// INCLUDE FILES
#include <e32svr.h>
#include <StifParser.h>
#include <Stiftestinterface.h>
#include "mcsapitest.h"

// ============================ MEMBER FUNCTIONS ===============================
    
// -----------------------------------------------------------------------------
// CMCSAPITest::RunMethodL
// Run specified method. Contains also table of test mothods and their names.
// -----------------------------------------------------------------------------
//
TInt CMCSAPITest::RunMethodL( 
    CStifItemParser& aItem ) 
    {
   
    static TStifFunctionInfo const KFunctions[] =
        {          		

		ENTRY( "Test_RMenuClose", 				CMCSAPITest::Test_RMenuCloseL ),
		ENTRY( "Test_RMenuOpen", 				CMCSAPITest::Test_RMenuOpenL ),
		ENTRY( "Test_RMenuRootFolder", 			CMCSAPITest::Test_RMenuRootFolderL ),
		ENTRY( "Test_RMenuRootGetItems", 		CMCSAPITest::Test_RMenuRootGetItemsL ),
		ENTRY( "Test_RMenuGetRunningApps", 		CMCSAPITest::Test_RMenuGetRunningAppsL ),
		ENTRY( "Test_RMenuRemove", 				CMCSAPITest::Test_RMenuRemoveL ),
		ENTRY( "Test_RMenuMoveToFolder", 		CMCSAPITest::Test_RMenuMoveToFolderL ),
		ENTRY( "Test_RMenuReorder", 			CMCSAPITest::Test_RMenuReorderL ),
		ENTRY( "Test_RMenuResourceMark", 		CMCSAPITest::Test_RMenuResourceMarkL ),
		ENTRY( "Test_RMenuResourceCheck", 		CMCSAPITest::Test_RMenuResourceCheckL ),
		ENTRY( "Test_RMenuResourceCount", 		CMCSAPITest::Test_RMenuResourceCountL ),
		ENTRY( "Test_RMenu__DbgSetAllocFailL", 	CMCSAPITest::Test_RMenu__DbgSetAllocFailL ),
		ENTRY( "Test_CMenuItemCreate", 			CMCSAPITest::Test_CMenuItemCreateL ),
		ENTRY( "Test_CMenuItemOpenId", 			CMCSAPITest::Test_CMenuItemOpenIdL ),
		ENTRY( "Test_CMenuItemOpenHdr", 		CMCSAPITest::Test_CMenuItemOpenHdrL ),
		ENTRY( "Test_CMenuItemId", 				CMCSAPITest::Test_CMenuItemIdL ),
		ENTRY( "Test_CMenuItemParent", 			CMCSAPITest::Test_CMenuItemParentL ),
		ENTRY( "Test_CMenuItemFlags",	 		CMCSAPITest::Test_CMenuItemFlagsL ),
		ENTRY( "Test_CMenuItemSetFlags", 		CMCSAPITest::Test_CMenuItemSetFlagsL ),
		ENTRY( "Test_CMenuItemType", 			CMCSAPITest::Test_CMenuItemTypeL ),
		ENTRY( "Test_CMenuItemGetAttribute",		CMCSAPITest::Test_CMenuItemGetAttributeL ),
		ENTRY( "Test_CMenuItemGetAttributeList",	CMCSAPITest::Test_CMenuItemGetAttributeListL ),
		ENTRY( "Test_CMenuItemRunningStatus", 		CMCSAPITest::Test_CMenuItemRunningStatusL ),
		ENTRY( "Test_CMenuItemSetAttribute", 		CMCSAPITest::Test_CMenuItemSetAttributeL ),
		ENTRY( "Test_CMenuItemRemoveAttribute", 	CMCSAPITest::Test_CMenuItemRemoveAttributeL ),
		ENTRY( "Test_CMenuItemSave", 				CMCSAPITest::Test_CMenuItemSaveL ),
		ENTRY( "Test_CMenuItemHandleCommand", 		CMCSAPITest::Test_CMenuItemHandleCommandL ),
		ENTRY( "Test_CMenuFilterNewL",         		CMCSAPITest::Test_CMenuFilterNewL ),
		ENTRY( "Test_CMenuFilterNewLC",         	CMCSAPITest::Test_CMenuFilterNewLC ),
		ENTRY( "Test_CMenuFilterFlagsOn",         	CMCSAPITest::Test_CMenuFilterFlagsOn ),
		ENTRY( "Test_CMenuFilterFlagsOff",         	CMCSAPITest::Test_CMenuFilterFlagsOff ),
		ENTRY( "Test_CMenuFilterType",         		CMCSAPITest::Test_CMenuFilterType ),
		ENTRY( "Test_CMenuFilterNumAttributes",     CMCSAPITest::Test_CMenuFilterNumAttributes ),
		ENTRY( "Test_CMenuFilterGetAttribute",      CMCSAPITest::Test_CMenuFilterGetAttribute ),
		ENTRY( "Test_CMenuFilterReset",         	CMCSAPITest::Test_CMenuFilterReset ),
		ENTRY( "Test_CMenuFilterSetFlags",         	CMCSAPITest::Test_CMenuFilterSetFlags ),
		ENTRY( "Test_CMenuFilterSetType",         	CMCSAPITest::Test_CMenuFilterSetType ),
        ENTRY( "Test_CMenuFilterHaveAttributeL",    CMCSAPITest::Test_CMenuFilterHaveAttributeL ),
        ENTRY( "Test_CMenuFilterDoNotHaveAttributeL",	CMCSAPITest::Test_CMenuFilterDoNotHaveAttributeL ),
        ENTRY( "Test_CMenuFilterRemoveAttribute",       CMCSAPITest::Test_CMenuFilterRemoveAttribute ),
        ENTRY( "Test_McsIconUtilityGetItemIconL",       CMCSAPITest::Test_McsIconUtilityGetItemIconL ),
        ENTRY( "Test_MenuUtilsGetTUint",         		CMCSAPITest::Test_MenuUtilsGetTUint ),
		
		ENTRY( "Test_RMenuNotifierOpenL", 		CMCSAPITest::Test_RMenuNotifierOpenL ),
		ENTRY( "Test_RMenuNotifierNotifyL", CMCSAPITest::Test_RMenuNotifierNotifyL ),
		ENTRY( "Test_RMenuNotifierCloseL", 		CMCSAPITest::Test_RMenuNotifierCloseL ),
		ENTRY( "Test_RMenuNotifierCancelL", 		CMCSAPITest::Test_RMenuNotifierCancelL ),
		
		ENTRY( "Test_CMenuFilterNewL",         CMCSAPITest::Test_CMenuFilterNewL ),
		ENTRY( "Test_CMenuFilterNewLC",         CMCSAPITest::Test_CMenuFilterNewLC ),
		ENTRY( "Test_CMenuFilterFlagsOn",         CMCSAPITest::Test_CMenuFilterFlagsOn ),
		ENTRY( "Test_CMenuFilterFlagsOff",         CMCSAPITest::Test_CMenuFilterFlagsOff ),
		ENTRY( "Test_CMenuFilterType",         CMCSAPITest::Test_CMenuFilterType ),
		ENTRY( "Test_CMenuFilterNumAttributes",         CMCSAPITest::Test_CMenuFilterNumAttributes ),
		ENTRY( "Test_CMenuFilterGetAttribute",         CMCSAPITest::Test_CMenuFilterGetAttribute ),
		ENTRY( "Test_CMenuFilterReset",         CMCSAPITest::Test_CMenuFilterReset ),
		ENTRY( "Test_CMenuFilterSetFlags",         CMCSAPITest::Test_CMenuFilterSetFlags ),
		ENTRY( "Test_CMenuFilterSetType",         CMCSAPITest::Test_CMenuFilterSetType ),
        ENTRY( "Test_CMenuFilterHaveAttributeL",         CMCSAPITest::Test_CMenuFilterHaveAttributeL ),
        ENTRY( "Test_CMenuFilterDoNotHaveAttributeL",         CMCSAPITest::Test_CMenuFilterDoNotHaveAttributeL ),
        ENTRY( "Test_CMenuFilterRemoveAttribute",         CMCSAPITest::Test_CMenuFilterRemoveAttribute ),
        ENTRY( "Test_McsIconUtilityGetItemIconL",         CMCSAPITest::Test_McsIconUtilityGetItemIconL ),
        ENTRY( "Test_MenuUtilsGetTUint",         CMCSAPITest::Test_MenuUtilsGetTUint ),
        ENTRY( "Test_MenuUtilsSetTUint",         CMCSAPITest::Test_MenuUtilsSetTUint ),
        
        ENTRY( "Test_MenuHandlerPluginNewL",         CMCSAPITest::Test_MenuHandlerPluginNewL ),
        ENTRY( "Test_MenuHandlerPluginDeleteL",         CMCSAPITest::Test_MenuHandlerPluginDeleteL ),
        ENTRY( "Test_MenuHandlerPluginConstructL",         CMCSAPITest::Test_MenuHandlerPluginConstructL ),
        
        
        ENTRY( "Test_TMenuItemConstructorL", 		CMCSAPITest::Test_TMenuItemConstructorL ),
        ENTRY( "Test_TMenuItemId", 		CMCSAPITest::Test_TMenuItemId ),
        ENTRY( "Test_TMenuItemParent", 		CMCSAPITest::Test_TMenuItemParent ),
        ENTRY( "Test_TMenuItemFlags", 		CMCSAPITest::Test_TMenuItemFlags ),
        ENTRY( "Test_TMenuItemType", 		CMCSAPITest::Test_TMenuItemType ),
        ENTRY( "Test_TMenuItemSetId", 		CMCSAPITest::Test_TMenuItemSetId ),
        ENTRY( "Test_TMenuItemSetParent", 		CMCSAPITest::Test_TMenuItemSetParent ),
        ENTRY( "Test_TMenuItemSetFlags", 		CMCSAPITest::Test_TMenuItemSetFlags ),
        ENTRY( "Test_TMenuItemSetType", 		CMCSAPITest::Test_TMenuItemSetType ),
        ENTRY( "Test_RMenuConstructorL", 		CMCSAPITest::Test_RMenuConstructorL ),
        ENTRY( "Test_CMenuFilterDestructorL", 		CMCSAPITest::Test_CMenuFilterDestructorL ),
        ENTRY( "Test_CMenuHandlerSupportsType", 		CMCSAPITest::Test_CMenuHandlerSupportsType ),
        ENTRY( "Test_CMenuHandlerHandleCommandL", 		CMCSAPITest::Test_CMenuHandlerHandleCommandL ),
        ENTRY( "Test_CMenuHandlerDestructor", 		CMCSAPITest::Test_CMenuHandlerDestructor ),
        ENTRY( "Test_RMenuNotifierConstructorL", 		CMCSAPITest::Test_RMenuNotifierConstructorL ),
		};

    const TInt count = sizeof( KFunctions ) / 
                        sizeof( TStifFunctionInfo );

    return RunInternalL( KFunctions, count, aItem );

    }    

//  End of File
