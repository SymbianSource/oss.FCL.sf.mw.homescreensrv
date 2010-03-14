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
* Description:  Test case Customizations(1) test data
*
*/

#ifndef MT_HSPS_CUSTOMIZATION_1_H_
#define MT_HSPS_CUSTOMIZATION_1_H_

/* 
Customizations(1)
----------------
 
Test purpose

Verify that customized configurations are shadowing similar files 
in ROM drive. Customized content can written to similar folder
structure, to any internal drive which users cannot hack (too easily). 
HSPS's ROM installer should then utilize ecplised files from the 
customization folders instead.
This test utilizes a copy of the orginal xml file, with an 
exception that all the default settings have been removed.

Pre-conditions
•   There must be installed test themes for Active Idle application and 
    Minimal configuration must be set as active. Installed_widget with 
    an 2000B133 UID must be available. Customized configuration file is 
    written to the C's install folder for shadowing purposes.     

Test steps

Test step 1:
•   Input:
    InstallConfigurationL( KHSPSInstallInstalledWidgetConf );
•   Expected output:
    "Installed_widget" is installed successfully.

Test step 2:
•   Input:
    GetPluginSettings(“plugin uid”)
•   Expected output:
    All settings, which were there by default, are now gone.
        
*/

// Test step 2 method:
const TUint8 customization_1_ts_2_method[] = "GetPluginSettings";

// Test step 2 input:
const TUint8 customization_1_ts_2_input[] = {
// GetPluginSettings(Input)
// - Version 1.0
// - Item count (LE)
10,
1,0,0,0,
// - GetPluginSettings(Input)::pluginUid
//   - Variant name
//     - Version 1.0
//     - Semantic ID (LE)    
//     - Variant name length (LE)
//     - Variant name descriptor maximum length ( ( variant name length * 4 ) + 2 )
//     - Variant name
10,
12,0,0,0,
9,0,0,0,
38,
'p','l','u','g','i','n','U','i','d',
// - GetPluginSettings(Input)::pluginUid
//   - Variant value
//     - Version 1.0
//     - Variant value type, EVariantTypeDesC
//     - Variant value length (LE)
//     - Variant value descriptor maximum length ( ( variant value length * 4 ) + 2 )
//     - Variant value
10,
5,
10,0,0,0,
42,
'0','x','2','0','0','0','B','1','3','3'
};

// Test step 2 output:
const TUint8 customization_1_ts_2_output[] =  {
// GetPluginSettings(Output)
// - Version 1.0
// - Item count (LE)
10,
1,0,0,0,
// - GetPluginSettings::settings
//   - Variant name
//     - Version 1.0
//     - Semantic ID (LE)    
//     - Variant name length (LE)
//     - Variant name descriptor maximum length ( ( variant name length * 4 ) + 2 )
//     - Variant name
10,
12,0,0,0,
8,0,0,0,
34,
's','e','t','t','i','n','g','s',
// - GetPluginSettings::settings
//   - Variant value
//     - Version 1.0
//     - Variant value type, EVariantTypeList
10,
7,
// - GetPluginSettings::settings
//   - Version 1.0
//   - Item count (LE)
10,
0,0,0,0,
};



#endif /*MT_HSPS_CUSTOMIZATION_1_H_*/
