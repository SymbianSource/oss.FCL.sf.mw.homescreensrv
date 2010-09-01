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
* Description:  Test case GetPluginConf(3) test data
*
*/


#ifndef C_MT_HSPS_GETPLUGINCONF_3_H
#define C_MT_HSPS_GETPLUGINCONF_3_H

/* 
GetPluginConf(3)
----------------
 
Test purpose

The purpose of this test case is to verify that GetPluginConf method fails when 
mandatory plugin id field is missing.

Pre-Conditions
There must be installed test themes for Active Idle application and Minimal 
configuration must be set as active.

Test steps

Test step 1:
•   Input:
    GetPluginConf(“plugin id is missing”)
•   Expected output:
    Status err argument

*/

// Test step 1 method:
const TUint8 getpluginconf_3_ts_1_method[] = "GetPluginConf";

// Test step 2 input:
const TUint8 getpluginconf_3_ts_1_input[] = {
// GetPluginConf(Input)
// - Version 1.0
// - Item count (LE)
10,
1,0,0,0,
// - GetPluginConf(Input)::pluginId
//   - Variant name
//     - Version 1.0
//     - Semantic ID (LE)    
//     - Variant name length (LE)
//     - Variant name descriptor maximum length ( ( variant name length * 4 ) + 2 )
//     - Variant name
10,
12,0,0,0,
8,0,0,0,
34
};

// Test step 2 output:

const TUint8 getpluginconf_3_ts_1_output[] = {
// GetPluginConf(Output)
// - Version 1.0
// - Item count (LE)
10,
1,0,0,0,
// - SetPluginSettings(Output)::status
//   - Variant name
//     - Version 1.0
//     - Semantic ID (LE)    
//     - Variant name length (LE)
//     - Variant name descriptor maximum length ( ( variant name length * 4 ) + 2 )
//     - Variant name
10,
12,0,0,0,
6,0,0,0,
26,
's','t','a','t','u','s',
// - SetPluginSettings::status
//   - Variant value
//     - Version 1.0
//     - Variant value type, EVariantTypeTInt32
//     - Variant value length (LE)
//     - Variant value descriptor maximum length ( ( variant value length * 4 ) + 2 )
//     - Variant value
10,
1,
250,255,255,255 // expected error
};

#endif // C_MT_HSPS_GETPLUGINCONF_2_H
