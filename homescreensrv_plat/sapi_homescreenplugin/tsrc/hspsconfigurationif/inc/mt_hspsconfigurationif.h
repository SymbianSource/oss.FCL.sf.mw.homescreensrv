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
* Description:  Defination of class MT_CHSPSConfigurationIf
*
*/


#ifndef C_MT_HSPSCONFIGURATIONIF_H
#define C_MT_HSPSCONFIGURATIONIF_H

#include <digia/eunit/eunitdecorators.h>
#include <liwservicehandler.h>
#include "mt_hsps.h"

/**
 * This class is a tester class for HSPS SAPI configuration interface 
*/
NONSHARABLE_CLASS( MT_CHSPSConfigurationIf ) : public MT_CHSPS
    {
    public:     // Constructors and destructors

        /**
         * Two phase construction
         */
        static MT_CHSPSConfigurationIf* NewL();
    
        /**
         * Destructor
         */
        virtual ~MT_CHSPSConfigurationIf();

    private:    // Constructors and destructors

    	/**
    	 * Default constructor.
    	 */
        MT_CHSPSConfigurationIf();
    
        /**
         * 2nd tier constructor.
         */
        void ConstructL();

    private:    // Common test case methods

        /**
         * Remove resources from client's private folder
         */
        void ResetClientFolderL();
        
    	/**
    	 * Setup for the unit test.
    	 */
        void SetupL();
         
        /**
         * Unit test teardown.
         */
        void Teardown();

        /**
         * Prints return value to a file.
         * @param aOutParamList Parameter list to be printed.
         */
        void PrintReturnValuesL(  
            CLiwGenericParamList& aOutParamList );

    private:    // Test cases
        
        /**
         * Test case function for test case GetActiveAppConf(1)
         * See HSPS module test specification
         */
        void GetActiveAppConf_1_L();    
        /**
         * Test case function for test case GetActiveAppConf(2)
         * See HSPS module test specification
         */
        void GetActiveAppConf_2_L();    
        /**
         * Test case function for test case GetActiveAppConf(3)
         * See HSPS module test specification
         */
        void GetActiveAppConf_3_L();    
        /**
         * Test case function for test case GetPluginConf(1)
         * See HSPS module test specification
         */
        void GetPluginConf_1_L();
        /**
         * Test case function for test case GetPluginConf(2)
         * See HSPS module test specification
         */
        void GetPluginConf_2_L();
        /**
         * Test case function for test case GetPluginConf(3)
         * See HSPS module test specification
         */
        void GetPluginConf_3_L();
        /**
         * Test case function for test case GetPluginConf(4)
         * See HSPS module test specification
         */
        void GetPluginConf_4_L();
        /**
         * Test case function for test case GetPluginConf(5)
         * See HSPS module test specification
         */
        void GetPluginConf_5_L();
        /**
         * Test case function for test case GetPlugins(1)
         * See HSPS module test specification
         */
        void GetPlugins_1_L();
        /**
         * Test case function for test case GetPlugins(2)
         * See HSPS module test specification
         */
        void GetPlugins_2_L();
        /**
         * Test case function for test case GetPlugins(3)
         * See HSPS module test specification
         */
        void GetPlugins_3_L();
        /**
         * Test case function for test case GetPlugins(4)
         * See HSPS module test specification
         */
        void GetPlugins_4_L();
        /**
         * Test case function for test case GetPlugins(5)
         * See HSPS module test specification
         */
        void GetPlugins_5_L();
        /**
         * Test case function for test case GetPlugins(6)
         * See HSPS module test specification
         */
        void GetPlugins_6_L();
        /**
         * Test case function for test case GetPlugins(7)
         * See HSPS module test specification
         */
        void GetPlugins_7_L();
        /**
         * Test case function for test case GetPlugins(8)
         * See HSPS module test specification
         */
        void GetPlugins_8_L();        
        /**
         * Test case function for test case GetPluginList(1)
         * See HSPS module test specification
         */
        void GetPluginList_1_L();
        /**
         * Test case function for test case GetPluginList(2)
         * See HSPS module test specification
         */
        void GetPluginList_2_L();
        /**
         * Test case function for test case GetPluginList(3)
         * See HSPS module test specification
         */
        void GetPluginList_3_L();
        /**
         * Test case function for test case GetPluginList(4)
         * See HSPS module test specification
         */
        void GetPluginList_4_L();
        /**
         * Test case function for test case GetPluginList(5)
         * See HSPS module test specification
         */
        void GetPluginList_5_L();
        /**
         * Test case function for test case GetPluginList(6)
         * See HSPS module test specification
         */
        void GetPluginList_6_L();
        /**
         * Test case function for test case GetPluginList(7)
         * See HSPS module test specification
         */
        void GetPluginList_7_L();
        /**
         * Test case function for test case GetPluginList(8)
         * See HSPS module test specification
         */
        void GetPluginList_8_L();
        /**
         * Test case function for test case AddPlugin(1)
         * See HSPS module test specification
         */
        void AddPlugin_1_L();
        /**
         * Test case function for test case AddPlugin(2)
         * See HSPS module test specification
         */
        void AddPlugin_2_L();
        /**
         * Test case function for test case AddPlugin(3)
         * See HSPS module test specification
         */
        void AddPlugin_3_L();
        /**
         * Test case function for test case AddPlugin(4)
         * See HSPS module test specification
         */
        void AddPlugin_4_L();
        /**
         * Test case function for test case AddPlugin(5)
         * See HSPS module test specification
         */
        void AddPlugin_5_L();
        /**
         * Test case function for test case AddPlugin(6)
         * See HSPS module test specification
         */
        void AddPlugin_6_L();
        /**
         * Test case function for test case AddPlugin(7)
         * See HSPS module test specification
         */
        void AddPlugin_7_L();
        /**
         * Test case function for test case AddPlugin(8)
         * See HSPS module test specification
         */
        void AddPlugin_8_L();
        /**
         * Test case function for test case AddPlugin(9)
         * See HSPS module test specification
         */
        void AddPlugin_9_L();
        /**
         * Test case function for test case AddPlugin(10)
         * See HSPS module test specification
         */
        void AddPlugin_10_L();
        /**
         * Test case function for test case AddPlugin(11)
         * See HSPS module test specification
         */
        void AddPlugin_11_L();
        /**
         * Test case function for test case RemovePlugin(1)
         * See HSPS module test specification
         */
        void RemovePlugin_1_L();
        /**
         * Test case function for test case RemovePlugin(2)
         * See HSPS module test specification
         */
        void RemovePlugin_2_L();
        /**
         * Test case function for test case RemovePlugin(3)
         * See HSPS module test specification
         */
        void RemovePlugin_3_L();
        /**
         * Test case function for test case RemovePlugin(4)
         * See HSPS module test specification
         */
        void RemovePlugin_4_L();
        /**
         * Test case function for test case SetPluginSettings(1)
         * See HSPS module test specification
         */
        void SetPluginSettings_1_L();
        /**
         * Test case function for test case SetPluginSettings(2)
         * See HSPS module test specification
         */
        void SetPluginSettings_2_L();
        /**
         * Test case function for test case SetPluginSettings(3)
         * See HSPS module test specification
         */
        void SetPluginSettings_3_L();
        /**
         * Test case function for test case SetPluginSettings(4)
         * See HSPS module test specification
         */
        void SetPluginSettings_4_L();        
        /**
         * Test case function for test case SetPluginSettings(5)
         * See HSPS module test specification
         */
        void SetPluginSettings_5_L();        
        /**
         * Test case function for test case SetPluginSettings(6)
         * See HSPS module test specification
         */
        void SetPluginSettings_6_L();        
        /**
         * Test case function for test case GetPluginSettings(1)
         * See HSPS module test specification
         */
        void GetPluginSettings_1_L();
        /**
         * Test case function for test case GetPluginSettings(2)
         * See HSPS module test specification
         */
        void GetPluginSettings_2_L();
        /**
         * Test case function for test case GetPluginSettings(3)
         * See HSPS module test specification
         */
        void GetPluginSettings_3_L();
        /**
         * Test case function for test case MovePlugins(1)
         * See HSPS module test specification
         */
        void MovePlugins_1_L();
        /**
         * Test case function for test case MovePlugins(2)
         * See HSPS module test specification
         */
        void MovePlugins_2_L();
        /**
         * Test case function for test case MovePlugins(3)
         * See HSPS module test specification
         */
        void MovePlugins_3_L();
        /**
         * Test case function for test case MovePlugins(4)
         * See HSPS module test specification
         */
        void MovePlugins_4_L();
        /**
         * Test case function for test case MovePlugins(5)
         * See HSPS module test specification
         */
        void MovePlugins_5_L();
        /**
         * Test case function for test case MovePlugins(6)
         * See HSPS module test specification
         */
        void MovePlugins_6_L();
        /**
         * Test case function for test case ReplacePlugin(1)
         * See HSPS module test specification
         */
        void ReplacePlugin_1_L();
        /**
         * Test case function for test case GetAppConfs(1)
         * See HSPS module test specification
         */
        void GetAppConfs_1_L();
        /**
         * Test case function for test case SetActiveAppConf(1)
         * See HSPS module test specification
         */
        void SetActiveAppConf_1_L();
        /**
         * Test case function for test case SetActiveAppConf(2)
         * See HSPS module test specification
         */
        void SetActiveAppConf_2_L();
        /**
         * Test case function for test case SetActiveAppConf(3)
         * See HSPS module test specification
         */
        void SetActiveAppConf_3_L();
        /**
         * Test case function for test case SetActiveAppConf(4)
         * See HSPS module test specification
         */
        void SetActiveAppConf_4_L();
        /**
         * Test case function for test case RequestNotify(1)
         * See HSPS module test specification
         */
        void RequestNotify_1_L();
        /**
         * Test case function for test case RequestNotify(2)
         * See HSPS module test specification
         */
        void RequestNotify_2_L();
        /**
         * Test case function for test case RequestNotify(3)
         * See HSPS module test specification
         */
        void RequestNotify_3_L();
        /**
         * Test case function for test case RequestNotify(4)
         * See HSPS module test specification
         */
        void RequestNotify_4_L();
        /**
         * Test case function for test case RequestNotify(5)
         * See HSPS module test specification
         */
        void RequestNotify_5_L();
        /**
         * Test case function for test case RequestNotify(6)
         * See HSPS module test specification
         */
        void RequestNotify_6_L();
        /**
         * Test case function for test case SisxUpgrade(1)
         * See HSPS module test specification
         */
        void SisxUpgrade_1_L();
        /**
         * Test case function for test case SisxInstallation(1)
         * See HSPS module test specification
         */
        void SisxInstallation_1_L();
        /**
         * Test case function for test case NativeInstallation(1)
         * See HSPS module test specification
         */        
        void NativeInstallation_1_L();
        /**
         * Test case function for test case SetActivePlugin(1)
         * See HSPS module test specification
         */
        void SetActivePlugin_1_L();
        /**
         * Test case function for test case SetActivePlugin(2)
         * See HSPS module test specification
         */
        void SetActivePlugin_2_L();
        /**
         * Test case function for test case SetActivePlugin(3)
         * See HSPS module test specification
         */
        void SetActivePlugin_3_L();
        /**
         * Test case function for test case SetActivePlugin(4)
         * See HSPS module test specification
         */
        void SetActivePlugin_4_L();
        /**
         * Test case function for test case SetActivePlugin(5)
         * See HSPS module test specification
         */
        void SetActivePlugin_5_L();
        /**
         * Test case function for test case SetActivePlugin(6)
         * See HSPS module test specification
         */
        void SetActivePlugin_6_L();
        /**
         * Test case function for test case RestoreConfigurations(1)
         * See HSPS module test specification
         */
        void RestoreConfigurations_1_L();
        /**
         * Test case function for test case Customization(1)
         * See HSPS module test specification
         */
        void Customization_1_L();        
        /**
         * Test case function for test case RestoreActiveAppConf(1)
         * See HSPS module test specification 
         */
        void RestoreActiveAppConf_1_L();
        /**
         * Test case function for test case RestoreActiveAppConf(2)
         * See HSPS module test specification 
         */
        void RestoreActiveAppConf_2_L();
        
    private:    // Data

        EUNIT_DECLARE_TEST_TABLE; 
        
        RFs iFileserver;

    };

#endif      //  C_MT_HSPSCONFIGURATIONIF_H

// End of file
