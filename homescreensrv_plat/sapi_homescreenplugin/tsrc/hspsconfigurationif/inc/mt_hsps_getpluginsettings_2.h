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
* Description:  Test case GetPluginSettings(2) test data
*
*/


#ifndef MT_HSPS_GETPLUGINSETTINGS_2_H_
#define MT_HSPS_GETPLUGINSETTINGS_2_H_

/* 
GetPluginSettings(1)
----------------
 
Test purpose

The purpose of this test case is to verify that get plugin setting 
works properly when no plugin found (invalid pluginuid).

Pre-conditions
•   There must be installed test themes for Active Idle application and 
    Typical configuration must be set as active. Plugin with 0x00001111 
    should not be available.

Test steps

Test step 1:
•   Input:
    GetPluginSettings(“plugin uid”)
•   Expected output:
    Status: not found

*/

// Test step 1 method:

const TUint8 getpluginsettings_2_ts_1_method[] = "GetPluginSettings";

// Test step 1 input:

const TUint8 getpluginsettings_2_ts_1_input[] = {
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
'0','x','0','0','0','0','1','1','1','1'
};

// Test step 1 output:

const TUint8 getpluginsettings_2_ts_1_output[] =  {
// GetPluginSettings(Output)
// - Version 1.0
// - Item count (LE)
10,
1,0,0,0,
// - GetPluginSettings::status
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
// - GetPluginSettings::status
//   - Variant value
//     - Version 1.0
//     - Variant value type, EVariantTypeTInt32
//     - Variant value length (LE)
//     - Variant value descriptor maximum length ( ( variant value length * 4 ) + 2 )
//     - Variant value
10,
1,
255,255,255,255 // expected out KErrNotFound (-1)
};



#endif /*MT_HSPS_GETPLUGINSETTINGS_2_H_*/
