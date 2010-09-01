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
* Description:  Test case ReplacePlugin(1) test data
*
*/


#ifndef MT_HSPS_REPLACEPLUGIN_1_H_
#define MT_HSPS_REPLACEPLUGIN_1_H_

/* 
ReplacePlugin(1)
----------------
 
Test purpose
Verify that the plugins can be replaced successfully in an application configuration.

Pre-conditions
•   There must be installed test themes for Active Idle application and Typical 
    configuration must be set as active

Test steps

Test step 1: replace plugin 2 with Widget Conf#1
•   Input:
    ReplacePlugin(2,0x2000B102)
•   Expected output:
    KErrNone
    
Test step 2: Get replaced plugin conf
•   Input:
    GetPluginConf(“2”)
•   Expected output:
    Correct widget configuration was returned
      
Test step 3: ConfUi argument was not provided 
•   Input:
    ReplacePlugin(1,null)
•   Expected output:
    Argument error
    
Test step 4: Plugin id argument was not provided 
•   Input:
    ReplacePlugin(null,0x2000B102)
•   Expected output:
    Argument error    
          
Test step 5: Test invalid plugin id 
•   Input:
    ReplacePlugin(testid,0x2000B102)
•   Expected output:
    Argument error

Test step 6: Test missing plugin id 
•   Input:
    ReplacePlugin(100,0x2000B102)
•   Expected output:
    Not found error
         
Test step 7: Test invalid configuration uid 
•   Input:
    ReplacePlugin(2,testuid)
•   Expected output:
    Argument error
             
Test step 8: Test missing configuration uid 
•   Input:
    ReplacePlugin(2,0x00000001)
•   Expected output:
    Not found error
    
Test step 9: Test replacing with an application configuration 
•   Input:
    ReplacePlugin(2,2000B110)
•   Expected output:
    Not found error

Test step 10: replace plugin 2 with Typical-Widget
•   Input:
    ReplacePlugin(2,0x2000B120)
•   Expected output:
    KErrNone
    
Test step 11: Get replaced plugin conf
•   Input:
    GetPluginConf(“2”)
•   Expected output:
    Correct widget configuration was returned    
          
*/

// Test step 1 method:

const TUint8 replaceplugin_1_ts_1_method[] = "ReplacePlugin";

// Test step 1 input:

const TUint8 replaceplugin_1_ts_1_input[] = {
// ReplacePlugin(Input)
// - Version 1.0
// - Item count (LE)
10,
2,0,0,0,
// - ReplacePlugin::pluginId
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
'p','l','u','g','i','n','I','d',
// - ReplacePlugin::confId
//   - Variant value
//     - Version 1.0
//     - Variant value type, EVariantTypeDesC
//     - Variant value length (LE)
//     - Variant value descriptor maximum length ( ( variant value length * 4 ) + 2 )
//     - Variant value
10,
5,
1,0,0,0,
6,
'2',
// - ReplacePlugin::confId
//   - Variant name
//     - Version 1.0
//     - Semantic ID (LE)    
//     - Variant name length (LE)
//     - Variant name descriptor maximum length ( ( variant name length * 4 ) + 2 )
//     - Variant name
10,
12,0,0,0,
7,0,0,0,
30,
'c','o','n','f','U','i','d',
// - ReplacePlugin::confId
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
'0','x','2','0','0','0','B','1','0','2',
};

// Test step 1 output:
const TUint8 replaceplugin_1_ts_1_output[] = {
// ReplacePlugin(Output)
// - Version 1.0
// - Item count (LE)
10,
1,0,0,0,
// - ReplacePlugin(Output)::status
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
// - ReplacePlugin(Output)::status
//   - Variant value
//     - Version 1.0
//     - Variant value type, EVariantTypeTInt32
//     - Variant value, KErrNone
10,
1,
0,0,0,0
};



// Test step 2 method:

const TUint8 replaceplugin_1_ts_2_method[] = "GetPluginConf";


// Test step 2 input:

const TUint8 replaceplugin_1_ts_2_input[] = {
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
34,
'p','l','u','g','i','n','I','d',
// - GetPluginConf(Input)::pluginId
//   - Variant value
//     - Version 1.0
//     - Variant value type, EVariantTypeDesC
//     - Variant value length (LE)
//     - Variant value descriptor maximum length ( ( variant value length * 4 ) + 2 )
//     - Variant value
10,
5,
1,0,0,0,
6,
'2'
};

// Test step 2 output:

const TUint8 replaceplugin_1_ts_2_output[] = {
// GetPluginConf(Output)
// - Version 1.0
// - Item count (LE)
10,
1,0,0,0,
// GetPluginConf(Output)::pluginConf
// - Variant name
//   - Version 1.0
//   - Semantic ID (LE)
//   - Variant name length (LE)
//   - Variant name descriptor maximum length ( ( variant name length * 4 ) + 2 )
//   - Variant name
10,
12,0,0,0,
10,0,0,0,
42,
'p','l','u','g','i','n','C','o','n','f',
// - Variant value
//   - Version 1.0
//   - Variant value type, EVariantTypeMap
10,
8,
// - pluginConf map
//   - Version 1.0
//   - Item count (LE)
10,
12,0,0,0,
// - pluginConf::id
//   - Variant name
//     - Version 1.0
//     - Semantic ID (LE)    
//     - Variant name length (LE)
//     - Variant name descriptor maximum length ( ( variant name length * 4 ) + 2 )
//     - Variant name
10,
12,0,0,0,
2,0,0,0,
10,
'i','d',
// - pluginConf::id
//   - Variant value
//     - Version 1.0
//     - Variant value type, EVariantTypeDesC
//     - Variant value length (LE)
//     - Variant value descriptor maximum length ( ( variant value length * 4 ) + 2 )
//     - Variant value
10,
5,
1,0,0,0,
6,
'3',
// - pluginConf::uid
//   - Variant name
//     - Version 1.0
//     - Semantic ID (LE)    
//     - Variant name length (LE)
//     - Variant name descriptor maximum length ( ( variant name length * 4 ) + 2 )
//     - Variant name
10,
12,0,0,0,
3,0,0,0,
14,
'u','i','d',
// - pluginConf::uid
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
'0','x','2','0','0','0','B','1','0','2',
// - pluginConf::type
//   - Variant name
//     - Version 1.0
//     - Semantic ID (LE)    
//     - Variant name length (LE)
//     - Variant name descriptor maximum length ( ( variant name length * 4 ) + 2 )
//     - Variant name
10,
12,0,0,0,
4,0,0,0,
18,
't','y','p','e',
// - pluginConf::type
//   - Variant value
//     - Version 1.0
//     - Variant value type, EVariantTypeDesC
//     - Variant value length (LE)
//     - Variant value descriptor maximum length ( ( variant value length * 4 ) + 2 )
//     - Variant value
10,
5,
6,0,0,0,
26,
'w','i','d','g','e','t',
// - pluginConf::interface
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
'i','n','t','e','r','f','a','c','e',
// - pluginConf::interface
//   - Variant value
//     - Version 1.0
//     - Variant value type, EVariantTypeDesC
//     - Variant value length (LE)
//     - Variant value descriptor maximum length ( ( variant value length * 4 ) + 2 )
//     - Variant value
10,
5,
5,0,0,0,
22,
'0','x','9','9','8',
// - pluginConf::name
//   - Variant name
//     - Version 1.0
//     - Semantic ID (LE)    
//     - Variant name length (LE)
//     - Variant name descriptor maximum length ( ( variant name length * 4 ) + 2 )
//     - Variant name
10,
12,0,0,0,
4,0,0,0,
18,
'n','a','m','e',
// - pluginConf::name
//   - Variant value
//     - Version 1.0
//     - Variant value type, EVariantTypeDesC
//     - Variant value length (LE)
//     - Variant value descriptor maximum length ( ( variant value length * 4 ) + 2 )
//     - Variant value
10,
5,
14,0,0,0,
58,
'W','i','d','g','e','t',' ','C','o','n','f',' ','#','1',
// - pluginConf::multiinstance
//   - Variant name
//     - Version 1.0
//     - Semantic ID (LE)    
//     - Variant name length (LE)
//     - Variant name descriptor maximum length ( ( variant name length * 4 ) + 2 )
//     - Variant name
10,
12,0,0,0,
13,0,0,0,
54,
'm','u','l','t','i','i','n','s','t','a','n','c','e',
// - pluginConf::multiinstance
//   - Variant value
//     - Version 1.0
//     - Variant value type, EVariantTypeDesC
//     - Variant value length (LE)
//     - Variant value descriptor maximum length ( ( variant value length * 4 ) + 2 )
//     - Variant value
10,
5,
1,0,0,0,
6,
'1',
// - pluginConf::description
//   - Variant name
//     - Version 1.0
//     - Semantic ID (LE)    
//     - Variant name length (LE)
//     - Variant name descriptor maximum length ( ( variant name length * 4 ) + 2 )
//     - Variant name
10,
12,0,0,0,
4,0,0,0,
18,
'd','e','s','c',
// - pluginConf::description
//   - Variant value
//     - Version 1.0
//     - Variant value type, EVariantTypeDesC
//     - Variant value length (LE)
//     - Variant value descriptor maximum length ( ( variant value length * 4 ) + 2 )
//     - Variant value
10,
5,
0,0,0,0,
2,
// - pluginConf::state
//   - Variant name
//     - Version 1.0
//     - Semantic ID (LE)    
//     - Variant name length (LE)
//     - Variant name descriptor maximum length ( ( variant name length * 4 ) + 2 )
//     - Variant name
10,
12,0,0,0,
5,0,0,0,
22,
's','t','a','t','e',
// - pluginConf::state
//   - Variant value
//     - Version 1.0
//     - Variant value type, EVariantTypeDesC
//     - Variant value length (LE)
//     - Variant value descriptor maximum length ( ( variant value length * 4 ) + 2 )
//     - Variant value
10,
5,
12,0,0,0,
50,
'N','o','t','C','o','n','f','i','r','m','e','d',
// - pluginConf::max_child
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
'm','a','x','_','c','h','i','l','d',
// - pluginConf::max_child
//   - Variant value
//     - Version 1.0
//     - Variant value type, EVariantTypeDesC
//     - Variant value length (LE)
//     - Variant value descriptor maximum length ( ( variant value length * 4 ) + 2 )
//     - Variant value
10,
5,
1,0,0,0,
6,
'0',
// - pluginConf::plugins
//   - Variant name
//     - Version 1.0
//     - Semantic ID (LE)    
//     - Variant name length (LE)
//     - Variant name descriptor maximum length ( ( variant name length * 4 ) + 2 )
//     - Variant name
10,
12,0,0,0,
7,0,0,0,
30,
'p','l','u','g','i','n','s',
// - pluginConf::plugins
//   - Variant value
//     - Version 1.0
//     - Variant value type, EVariantTypeList
10,
7,
// - pluginConf::plugins
//   - Version 1.0
//   - Item count (LE)
10,
0,0,0,0,
// - pluginConf::settings
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
// - pluginConf::settings
//   - Variant value
//     - Version 1.0
//     - Variant value type, EVariantTypeList
10,
7,
// - pluginConf::settings
//   - Version 1.0
//   - Item count (LE)
10,
1,0,0,0,
// - pluginConf::settings
//   - Version 1.0
//   - List item starts
10,
0,0,0,0,
// - pluginConf::settings[0]
//   - Version 1.0
//   - Variant value type, EVariantTypeMap
10,
8,
// - settings[0] map
//   - Version 1.0
//   - Item count (LE)
10,
3,0,0,0,
// - settings[0]::itemId
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
'i','t','e','m','I','d',
// - settings[0]::itemId
//   - Variant value
//     - Version 1.0
//     - Variant value type, EVariantTypeDesC
//     - Variant value length (LE)
//     - Variant value descriptor maximum length ( ( variant value length * 4 ) + 2 )
//     - Variant value (empty)
10,
5,
4,0,0,0,
18,
's','i','z','e',
// - settings[0]::name
//   - Variant name
//     - Version 1.0
//     - Semantic ID (LE)    
//     - Variant name length (LE)
//     - Variant name descriptor maximum length ( ( variant name length * 4 ) + 2 )
//     - Variant name
10,
12,0,0,0,
4,0,0,0,
18,
'n','a','m','e',
// - settings[0]::name
//   - Variant value
//     - Version 1.0
//     - Variant value type, EVariantTypeDesC
//     - Variant value length (LE)
//     - Variant value descriptor maximum length ( ( variant value length * 4 ) + 2 )
//     - Variant value (empty)
10,
5,
4,0,0,0,
18,
'S','i','z','e',
// - settings[0]::properties
//   - Variant name
//     - Version 1.0
//     - Semantic ID (LE)    
//     - Variant name length (LE)
//     - Variant name descriptor maximum length ( ( variant name length * 4 ) + 2 )
//     - Variant name
10,
12,0,0,0,
10,0,0,0,
42,
'p','r','o','p','e','r','t','i','e','s',
// - settings[0]::properties
//   - Variant value
//     - Version 1.0
//     - Variant value type, EVariantTypeList
10,
7,
// - settings[0]::properties
//   - Version 1.0
//   - Item count (LE)
10,
3,0,0,0,
// - settings[0]::properties
//   - Version 1.0
//   - List item starts
10,
0,0,0,0,
// - properties[0]
//   - Version 1.0
//   - Variant value type, EVariantTypeMap
10,
8,
// - properties[0] map
//   - Version 1.0
//   - Item count (LE)
10,
2,0,0,0,
// - prperties[0]::name
//   - Variant name
//     - Version 1.0
//     - Semantic ID (LE)    
//     - Variant name length (LE)
//     - Variant name descriptor maximum length ( ( variant name length * 4 ) + 2 )
//     - Variant name
10,
12,0,0,0,
4,0,0,0,
18,
'n','a','m','e',
// - properties[0]::name
//   - Variant value
//     - Version 1.0
//     - Variant value type, EVariantTypeDesC
//     - Variant value length (LE)
//     - Variant value descriptor maximum length ( ( variant value length * 4 ) + 2 )
//     - Variant value
10,
5,
6,0,0,0,
26,
'h','e','i','g','h','t',
// - prperties[0]::value
//   - Variant name
//     - Version 1.0
//     - Semantic ID (LE)    
//     - Variant name length (LE)
//     - Variant name descriptor maximum length ( ( variant name length * 4 ) + 2 )
//     - Variant name
10,
12,0,0,0,
5,0,0,0,
22,
'v','a','l','u','e',
// - properties[0]::value
//   - Variant value
//     - Version 1.0
//     - Variant value type, EVariantTypeDesC
//     - Variant value length (LE)
//     - Variant value descriptor maximum length ( ( variant value length * 4 ) + 2 )
//     - Variant value
10,
5,
2,0,0,0,
10,
'2','0',
// - settings[0]::properties
//   - Version 1.0
//   - List item starts
10,
0,0,0,0,
// - properties[1]
//   - Version 1.0
//   - Variant value type, EVariantTypeMap
10,
8,
// - properties[1] map
//   - Version 1.0
//   - Item count (LE)
10,
2,0,0,0,
// - prperties[1]::name
//   - Variant name
//     - Version 1.0
//     - Semantic ID (LE)    
//     - Variant name length (LE)
//     - Variant name descriptor maximum length ( ( variant name length * 4 ) + 2 )
//     - Variant name
10,
12,0,0,0,
4,0,0,0,
18,
'n','a','m','e',
// - properties[1]::name
//   - Variant value
//     - Version 1.0
//     - Variant value type, EVariantTypeDesC
//     - Variant value length (LE)
//     - Variant value descriptor maximum length ( ( variant value length * 4 ) + 2 )
//     - Variant value
10,
5,
5,0,0,0,
22,
'w','i','d','t','h',
// - prperties[1]::value
//   - Variant name
//     - Version 1.0
//     - Semantic ID (LE)    
//     - Variant name length (LE)
//     - Variant name descriptor maximum length ( ( variant name length * 4 ) + 2 )
//     - Variant name
10,
12,0,0,0,
5,0,0,0,
22,
'v','a','l','u','e',
// - properties[1]::value
//   - Variant value
//     - Version 1.0
//     - Variant value type, EVariantTypeDesC
//     - Variant value length (LE)
//     - Variant value descriptor maximum length ( ( variant value length * 4 ) + 2 )
//     - Variant value
10,
5,
3,0,0,0,
14,
'2','4','0',
// - settings[0]::properties
//   - Version 1.0
//   - List item starts
10,
0,0,0,0,
// - properties[2]
//   - Version 1.0
//   - Variant value type, EVariantTypeMap
10,
8,
// - properties[2] map
//   - Version 1.0
//   - Item count (LE)
10,
2,0,0,0,
// - properties[2]::name
//   - Variant name
//     - Version 1.0
//     - Semantic ID (LE)    
//     - Variant name length (LE)
//     - Variant name descriptor maximum length ( ( variant name length * 4 ) + 2 )
//     - Variant name
10,
12,0,0,0,
4,0,0,0,
18,
'n','a','m','e',
// - properties[2]::name
//   - Variant value
//     - Version 1.0
//     - Variant value type, EVariantTypeDesC
//     - Variant value length (LE)
//     - Variant value descriptor maximum length ( ( variant value length * 4 ) + 2 )
//     - Variant value
10,
5,
6,0,0,0,
26,
'l','o','c','k','e','d',
// - properties[2]::value
//   - Variant name
//     - Version 1.0
//     - Semantic ID (LE)    
//     - Variant name length (LE)
//     - Variant name descriptor maximum length ( ( variant name length * 4 ) + 2 )
//     - Variant name
10,
12,0,0,0,
5,0,0,0,
22,
'v','a','l','u','e',
// - properties[2]::value
//   - Variant value
//     - Version 1.0
//     - Variant value type, EVariantTypeDesC
//     - Variant value length (LE)
//     - Variant value descriptor maximum length ( ( variant value length * 4 ) + 2 )
//     - Variant value
10,
5,
3,0,0,0,
14,
'y','e','s',
// - pluginConf::resources
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
'r','e','s','o','u','r','c','e','s',
// - pluginConf::resources
//   - Variant value
//     - Version 1.0
//     - Variant value type, EVariantTypeList
10,
7,
// - pluginConf::resources
//   - Version 1.0
//   - Item count (LE)
10,
0,0,0,0
};



// Test step 3 method:

const TUint8 replaceplugin_1_ts_3_method[] = "ReplacePlugin";

// Test step 3 input:

const TUint8 replaceplugin_1_ts_3_input[] = {
// ReplacePlugin(Input)
// - Version 1.0
// - Item count (LE)
10,
2,0,0,0,
// - ReplacePlugin::pluginId
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
'p','l','u','g','i','n','I','d',
// - ReplacePlugin::confId
//   - Variant value
//     - Version 1.0
//     - Variant value type, EVariantTypeDesC
//     - Variant value length (LE)
//     - Variant value descriptor maximum length ( ( variant value length * 4 ) + 2 )
//     - Variant value
10,
5,
1,0,0,0,
6,
'1'
};

// Test step 3 output:
const TUint8 replaceplugin_1_ts_3_output[] = {
// ReplacePlugin(Output)
// - Version 1.0
// - Item count (LE)
10,
1,0,0,0,
// - ReplacePlugin(Output)::status
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
// - ReplacePlugin(Output)::status
//   - Variant value
//     - Version 1.0
//     - Variant value type, EVariantTypeTInt32
//     - Variant value, KErrArgument
10,
1,
250,255,255,255
};



// Test step 4 method:

const TUint8 replaceplugin_1_ts_4_method[] = "ReplacePlugin";

// Test step 4 input:

const TUint8 replaceplugin_1_ts_4_input[] = {
// ReplacePlugin(Input)
// - Version 1.0
// - Item count (LE)
10,
1,0,0,0,
// - ReplacePlugin::confId
//   - Variant name
//     - Version 1.0
//     - Semantic ID (LE)    
//     - Variant name length (LE)
//     - Variant name descriptor maximum length ( ( variant name length * 4 ) + 2 )
//     - Variant name
10,
12,0,0,0,
7,0,0,0,
30,
'c','o','n','f','U','i','d',
// - ReplacePlugin::confId
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
'0','x','2','0','0','0','B','1','0','2'
};

// Test step 4 output:
const TUint8 replaceplugin_1_ts_4_output[] = {
// ReplacePlugin(Output)
// - Version 1.0
// - Item count (LE)
10,
1,0,0,0,
// - ReplacePlugin(Output)::status
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
// - ReplacePlugin(Output)::status
//   - Variant value
//     - Version 1.0
//     - Variant value type, EVariantTypeTInt32
//     - Variant value, KErrArgument
10,
1,
250,255,255,255
};



// Test step 5 method:

const TUint8 replaceplugin_1_ts_5_method[] = "ReplacePlugin";

// Test step 5 input:

const TUint8 replaceplugin_1_ts_5_input[] = {
// ReplacePlugin(Input)
// - Version 1.0
// - Item count (LE)
10,
2,0,0,0,
// - ReplacePlugin::pluginId
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
'p','l','u','g','i','n','I','d',
// - ReplacePlugin::confId
//   - Variant value
//     - Version 1.0
//     - Variant value type, EVariantTypeDesC
//     - Variant value length (LE)
//     - Variant value descriptor maximum length ( ( variant value length * 4 ) + 2 )
//     - Variant value
10,
5,
6,0,0,0,
26,
't','e','s','t','i','d',
// - ReplacePlugin::confId
//   - Variant name
//     - Version 1.0
//     - Semantic ID (LE)    
//     - Variant name length (LE)
//     - Variant name descriptor maximum length ( ( variant name length * 4 ) + 2 )
//     - Variant name
10,
12,0,0,0,
7,0,0,0,
30,
'c','o','n','f','U','i','d',
// - ReplacePlugin::confId
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
'0','x','2','0','0','0','B','1','0','2',
};

// Test step 5 output:
const TUint8 replaceplugin_1_ts_5_output[] = {
// ReplacePlugin(Output)
// - Version 1.0
// - Item count (LE)
10,
1,0,0,0,
// - ReplacePlugin(Output)::status
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
// - ReplacePlugin(Output)::status
//   - Variant value
//     - Version 1.0
//     - Variant value type, EVariantTypeTInt32
//     - Variant value, KErrArgument
10,
1,
250,255,255,255
};


// Test step 6 method:

const TUint8 replaceplugin_1_ts_6_method[] = "ReplacePlugin";

// Test step 6 input:

const TUint8 replaceplugin_1_ts_6_input[] = {
// ReplacePlugin(Input)
// - Version 1.0
// - Item count (LE)
10,
2,0,0,0,
// - ReplacePlugin::pluginId
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
'p','l','u','g','i','n','I','d',
// - ReplacePlugin::confId
//   - Variant value
//     - Version 1.0
//     - Variant value type, EVariantTypeDesC
//     - Variant value length (LE)
//     - Variant value descriptor maximum length ( ( variant value length * 4 ) + 2 )
//     - Variant value
10,
5,
3,0,0,0,
14,
'1','0','0',
// - ReplacePlugin::confId
//   - Variant name
//     - Version 1.0
//     - Semantic ID (LE)    
//     - Variant name length (LE)
//     - Variant name descriptor maximum length ( ( variant name length * 4 ) + 2 )
//     - Variant name
10,
12,0,0,0,
7,0,0,0,
30,
'c','o','n','f','U','i','d',
// - ReplacePlugin::confId
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
'0','x','2','0','0','0','B','1','0','2',
};

// Test step 6 output:
const TUint8 replaceplugin_1_ts_6_output[] = {
// ReplacePlugin(Output)
// - Version 1.0
// - Item count (LE)
10,
1,0,0,0,
// - ReplacePlugin(Output)::status
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
// - ReplacePlugin(Output)::status
//   - Variant value
//     - Version 1.0
//     - Variant value type, EVariantTypeTInt32
//     - Variant value, KErrNotFound
10,
1,
255,255,255,255
};



// Test step 7 method:

const TUint8 replaceplugin_1_ts_7_method[] = "ReplacePlugin";

// Test step 7 input:

const TUint8 replaceplugin_1_ts_7_input[] = {
// ReplacePlugin(Input)
// - Version 1.0
// - Item count (LE)
10,
2,0,0,0,
// - ReplacePlugin::pluginId
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
'p','l','u','g','i','n','I','d',
// - ReplacePlugin::confId
//   - Variant value
//     - Version 1.0
//     - Variant value type, EVariantTypeDesC
//     - Variant value length (LE)
//     - Variant value descriptor maximum length ( ( variant value length * 4 ) + 2 )
//     - Variant value
10,
5,
1,0,0,0,
6,
'2',
// - ReplacePlugin::confId
//   - Variant name
//     - Version 1.0
//     - Semantic ID (LE)    
//     - Variant name length (LE)
//     - Variant name descriptor maximum length ( ( variant name length * 4 ) + 2 )
//     - Variant name
10,
12,0,0,0,
7,0,0,0,
30,
'c','o','n','f','U','i','d',
// - ReplacePlugin::confId
//   - Variant value
//     - Version 1.0
//     - Variant value type, EVariantTypeDesC
//     - Variant value length (LE)
//     - Variant value descriptor maximum length ( ( variant value length * 4 ) + 2 )
//     - Variant value
10,
5,
7,0,0,0,
30,
't','e','s','t','u','i','d'
};

// Test step 7 output:
const TUint8 replaceplugin_1_ts_7_output[] = {
// ReplacePlugin(Output)
// - Version 1.0
// - Item count (LE)
10,
1,0,0,0,
// - ReplacePlugin(Output)::status
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
// - ReplacePlugin(Output)::status
//   - Variant value
//     - Version 1.0
//     - Variant value type, EVariantTypeTInt32
//     - Variant value, KErrArgument
10,
1,
254,255,255,255
};



// Test step 8 method:

const TUint8 replaceplugin_1_ts_8_method[] = "ReplacePlugin";

// Test step 8 input:

const TUint8 replaceplugin_1_ts_8_input[] = {
// ReplacePlugin(Input)
// - Version 1.0
// - Item count (LE)
10,
2,0,0,0,
// - ReplacePlugin::pluginId
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
'p','l','u','g','i','n','I','d',
// - ReplacePlugin::confId
//   - Variant value
//     - Version 1.0
//     - Variant value type, EVariantTypeDesC
//     - Variant value length (LE)
//     - Variant value descriptor maximum length ( ( variant value length * 4 ) + 2 )
//     - Variant value
10,
5,
1,0,0,0,
6,
'2',
// - ReplacePlugin::confId
//   - Variant name
//     - Version 1.0
//     - Semantic ID (LE)    
//     - Variant name length (LE)
//     - Variant name descriptor maximum length ( ( variant name length * 4 ) + 2 )
//     - Variant name
10,
12,0,0,0,
7,0,0,0,
30,
'c','o','n','f','U','i','d',
// - ReplacePlugin::confId
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
'0','x','0','0','0','0','0','0','0','1',
};

// Test step 8 output:
const TUint8 replaceplugin_1_ts_8_output[] = {
// ReplacePlugin(Output)
// - Version 1.0
// - Item count (LE)
10,
1,0,0,0,
// - ReplacePlugin(Output)::status
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
// - ReplacePlugin(Output)::status
//   - Variant value
//     - Version 1.0
//     - Variant value type, EVariantTypeTInt32
//     - Variant value, KErrNotFound
10,
1,
255,255,255,255
};



// Test step 9 method:

const TUint8 replaceplugin_1_ts_9_method[] = "ReplacePlugin";

// Test step 9 input:

const TUint8 replaceplugin_1_ts_9_input[] = {
// ReplacePlugin(Input)
// - Version 1.0
// - Item count (LE)
10,
2,0,0,0,
// - ReplacePlugin::pluginId
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
'p','l','u','g','i','n','I','d',
// - ReplacePlugin::confId
//   - Variant value
//     - Version 1.0
//     - Variant value type, EVariantTypeDesC
//     - Variant value length (LE)
//     - Variant value descriptor maximum length ( ( variant value length * 4 ) + 2 )
//     - Variant value
10,
5,
1,0,0,0,
6,
'2',
// - ReplacePlugin::confId
//   - Variant name
//     - Version 1.0
//     - Semantic ID (LE)    
//     - Variant name length (LE)
//     - Variant name descriptor maximum length ( ( variant name length * 4 ) + 2 )
//     - Variant name
10,
12,0,0,0,
7,0,0,0,
30,
'c','o','n','f','U','i','d',
// - ReplacePlugin::confId
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
'0','x','2','0','0','0','B','1','1','0',
};

// Test step 9 output:
const TUint8 replaceplugin_1_ts_9_output[] = {
// ReplacePlugin(Output)
// - Version 1.0
// - Item count (LE)
10,
1,0,0,0,
// - ReplacePlugin(Output)::status
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
// - ReplacePlugin(Output)::status
//   - Variant value
//     - Version 1.0
//     - Variant value type, EVariantTypeTInt32
//     - Variant value, KErrArgument
10,
1,
255,255,255,255
};


// Test step 10 method:

const TUint8 replaceplugin_1_ts_10_method[] = "ReplacePlugin";

// Test step 10 input:

const TUint8 replaceplugin_1_ts_10_input[] = {
// ReplacePlugin(Input)
// - Version 1.0
// - Item count (LE)
10,
2,0,0,0,
// - ReplacePlugin::pluginId
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
'p','l','u','g','i','n','I','d',
// - ReplacePlugin::confId
//   - Variant value
//     - Version 1.0
//     - Variant value type, EVariantTypeDesC
//     - Variant value length (LE)
//     - Variant value descriptor maximum length ( ( variant value length * 4 ) + 2 )
//     - Variant value
10,
5,
1,0,0,0,
6,
'2',
// - ReplacePlugin::confId
//   - Variant name
//     - Version 1.0
//     - Semantic ID (LE)    
//     - Variant name length (LE)
//     - Variant name descriptor maximum length ( ( variant name length * 4 ) + 2 )
//     - Variant name
10,
12,0,0,0,
7,0,0,0,
30,
'c','o','n','f','U','i','d',
// - ReplacePlugin::confId
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
'0','x','2','0','0','0','B','1','2','0',
};

// Test step 10 output:
const TUint8 replaceplugin_1_ts_10_output[] = {
// ReplacePlugin(Output)
// - Version 1.0
// - Item count (LE)
10,
1,0,0,0,
// - ReplacePlugin(Output)::status
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
// - ReplacePlugin(Output)::status
//   - Variant value
//     - Version 1.0
//     - Variant value type, EVariantTypeTInt32
//     - Variant value, KErrNone
10,
1,
0,0,0,0
};



// Test step 11 method:

const TUint8 replaceplugin_1_ts_11_method[] = "GetPluginConf";


// Test step 11 input:

const TUint8 replaceplugin_1_ts_11_input[] = {
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
34,
'p','l','u','g','i','n','I','d',
// - GetPluginConf(Input)::pluginId
//   - Variant value
//     - Version 1.0
//     - Variant value type, EVariantTypeDesC
//     - Variant value length (LE)
//     - Variant value descriptor maximum length ( ( variant value length * 4 ) + 2 )
//     - Variant value
10,
5,
1,0,0,0,
6,
'2'
};

// Test step 11 output:

const TUint8 replaceplugin_1_ts_11_output[] = {
// GetPluginConf(Output)
// - Version 1.0
// - Item count (LE)
10,
1,0,0,0,
// GetPluginConf(Output)::pluginConf
// - Variant name
//   - Version 1.0
//   - Semantic ID (LE)
//   - Variant name length (LE)
//   - Variant name descriptor maximum length ( ( variant name length * 4 ) + 2 )
//   - Variant name
10,
12,0,0,0,
10,0,0,0,
42,
'p','l','u','g','i','n','C','o','n','f',
// - Variant value
//   - Version 1.0
//   - Variant value type, EVariantTypeMap
10,
8,
// - pluginConf map
//   - Version 1.0
//   - Item count (LE)
10,
12,0,0,0,
// - pluginConf::id
//   - Variant name
//     - Version 1.0
//     - Semantic ID (LE)    
//     - Variant name length (LE)
//     - Variant name descriptor maximum length ( ( variant name length * 4 ) + 2 )
//     - Variant name
10,
12,0,0,0,
2,0,0,0,
10,
'i','d',
// - pluginConf::id
//   - Variant value
//     - Version 1.0
//     - Variant value type, EVariantTypeDesC
//     - Variant value length (LE)
//     - Variant value descriptor maximum length ( ( variant value length * 4 ) + 2 )
//     - Variant value
10,
5,
1,0,0,0,
6,
'3',
// - pluginConf::uid
//   - Variant name
//     - Version 1.0
//     - Semantic ID (LE)    
//     - Variant name length (LE)
//     - Variant name descriptor maximum length ( ( variant name length * 4 ) + 2 )
//     - Variant name
10,
12,0,0,0,
3,0,0,0,
14,
'u','i','d',
// - pluginConf::uid
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
'0','x','2','0','0','0','B','1','2','0',
// - pluginConf::type
//   - Variant name
//     - Version 1.0
//     - Semantic ID (LE)    
//     - Variant name length (LE)
//     - Variant name descriptor maximum length ( ( variant name length * 4 ) + 2 )
//     - Variant name
10,
12,0,0,0,
4,0,0,0,
18,
't','y','p','e',
// - pluginConf::type
//   - Variant value
//     - Version 1.0
//     - Variant value type, EVariantTypeDesC
//     - Variant value length (LE)
//     - Variant value descriptor maximum length ( ( variant value length * 4 ) + 2 )
//     - Variant value
10,
5,
6,0,0,0,
26,
'w','i','d','g','e','t',
// - pluginConf::interface
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
'i','n','t','e','r','f','a','c','e',
// - pluginConf::interface
//   - Variant value
//     - Version 1.0
//     - Variant value type, EVariantTypeDesC
//     - Variant value length (LE)
//     - Variant value descriptor maximum length ( ( variant value length * 4 ) + 2 )
//     - Variant value
10,
5,
5,0,0,0,
22,
'0','x','9','9','8',
// - pluginConf::name
//   - Variant name
//     - Version 1.0
//     - Semantic ID (LE)    
//     - Variant name length (LE)
//     - Variant name descriptor maximum length ( ( variant name length * 4 ) + 2 )
//     - Variant name
10,
12,0,0,0,
4,0,0,0,
18,
'n','a','m','e',
// - pluginConf::name
//   - Variant value
//     - Version 1.0
//     - Variant value type, EVariantTypeDesC
//     - Variant value length (LE)
//     - Variant value descriptor maximum length ( ( variant value length * 4 ) + 2 )
//     - Variant value
10,
5,
16,0,0,0,
66,
'T','y','p','i','c','a','l',' ','-',' ','W','i','d','g','e','t',
// - pluginConf::multiinstance
//   - Variant name
//     - Version 1.0
//     - Semantic ID (LE)    
//     - Variant name length (LE)
//     - Variant name descriptor maximum length ( ( variant name length * 4 ) + 2 )
//     - Variant name
10,
12,0,0,0,
13,0,0,0,
54,
'm','u','l','t','i','i','n','s','t','a','n','c','e',
// - pluginConf::multiinstance
//   - Variant value
//     - Version 1.0
//     - Variant value type, EVariantTypeDesC
//     - Variant value length (LE)
//     - Variant value descriptor maximum length ( ( variant value length * 4 ) + 2 )
//     - Variant value
10,
5,
1,0,0,0,
6,
'1',
// - pluginConf::description
//   - Variant name
//     - Version 1.0
//     - Semantic ID (LE)    
//     - Variant name length (LE)
//     - Variant name descriptor maximum length ( ( variant name length * 4 ) + 2 )
//     - Variant name
10,
12,0,0,0,
4,0,0,0,
18,
'd','e','s','c',
// - pluginConf::description
//   - Variant value
//     - Version 1.0
//     - Variant value type, EVariantTypeDesC
//     - Variant value length (LE)
//     - Variant value descriptor maximum length ( ( variant value length * 4 ) + 2 )
//     - Variant value
10,
5,
0,0,0,0,
2,
// - pluginConf::state
//   - Variant name
//     - Version 1.0
//     - Semantic ID (LE)    
//     - Variant name length (LE)
//     - Variant name descriptor maximum length ( ( variant name length * 4 ) + 2 )
//     - Variant name
10,
12,0,0,0,
5,0,0,0,
22,
's','t','a','t','e',
// - pluginConf::state
//   - Variant value
//     - Version 1.0
//     - Variant value type, EVariantTypeDesC
//     - Variant value length (LE)
//     - Variant value descriptor maximum length ( ( variant value length * 4 ) + 2 )
//     - Variant value
10,
5,
12,0,0,0,
50,
'N','o','t','C','o','n','f','i','r','m','e','d',
// - pluginConf::max_child
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
'm','a','x','_','c','h','i','l','d',
// - pluginConf::max_child
//   - Variant value
//     - Version 1.0
//     - Variant value type, EVariantTypeDesC
//     - Variant value length (LE)
//     - Variant value descriptor maximum length ( ( variant value length * 4 ) + 2 )
//     - Variant value
10,
5,
1,0,0,0,
6,
'0',
// - pluginConf::plugins
//   - Variant name
//     - Version 1.0
//     - Semantic ID (LE)    
//     - Variant name length (LE)
//     - Variant name descriptor maximum length ( ( variant name length * 4 ) + 2 )
//     - Variant name
10,
12,0,0,0,
7,0,0,0,
30,
'p','l','u','g','i','n','s',
// - pluginConf::plugins
//   - Variant value
//     - Version 1.0
//     - Variant value type, EVariantTypeList
10,
7,
// - pluginConf::plugins
//   - Version 1.0
//   - Item count (LE)
10,
0,0,0,0,
// - pluginConf::settings
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
// - pluginConf::settings
//   - Variant value
//     - Version 1.0
//     - Variant value type, EVariantTypeList
10,
7,
// - pluginConf::settings
//   - Version 1.0
//   - Item count (LE)
10,
1,0,0,0,
// - pluginConf::settings
//   - Version 1.0
//   - List item starts
10,
0,0,0,0,
// - pluginConf::settings[0]
//   - Version 1.0
//   - Variant value type, EVariantTypeMap
10,
8,
// - settings[0] map
//   - Version 1.0
//   - Item count (LE)
10,
3,0,0,0,
// - settings[0]::itemId
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
'i','t','e','m','I','d',
// - settings[0]::itemId
//   - Variant value
//     - Version 1.0
//     - Variant value type, EVariantTypeDesC
//     - Variant value length (LE)
//     - Variant value descriptor maximum length ( ( variant value length * 4 ) + 2 )
//     - Variant value (empty)
10,
5,
4,0,0,0,
18,
's','i','z','e',
// - settings[0]::name
//   - Variant name
//     - Version 1.0
//     - Semantic ID (LE)    
//     - Variant name length (LE)
//     - Variant name descriptor maximum length ( ( variant name length * 4 ) + 2 )
//     - Variant name
10,
12,0,0,0,
4,0,0,0,
18,
'n','a','m','e',
// - settings[0]::name
//   - Variant value
//     - Version 1.0
//     - Variant value type, EVariantTypeDesC
//     - Variant value length (LE)
//     - Variant value descriptor maximum length ( ( variant value length * 4 ) + 2 )
//     - Variant value (empty)
10,
5,
4,0,0,0,
18,
'S','i','z','e',
// - settings[0]::properties
//   - Variant name
//     - Version 1.0
//     - Semantic ID (LE)    
//     - Variant name length (LE)
//     - Variant name descriptor maximum length ( ( variant name length * 4 ) + 2 )
//     - Variant name
10,
12,0,0,0,
10,0,0,0,
42,
'p','r','o','p','e','r','t','i','e','s',
// - settings[0]::properties
//   - Variant value
//     - Version 1.0
//     - Variant value type, EVariantTypeList
10,
7,
// - settings[0]::properties
//   - Version 1.0
//   - Item count (LE)
10,
3,0,0,0,
// - settings[0]::properties
//   - Version 1.0
//   - List item starts
10,
0,0,0,0,
// - properties[0]
//   - Version 1.0
//   - Variant value type, EVariantTypeMap
10,
8,
// - properties[0] map
//   - Version 1.0
//   - Item count (LE)
10,
2,0,0,0,
// - prperties[0]::name
//   - Variant name
//     - Version 1.0
//     - Semantic ID (LE)    
//     - Variant name length (LE)
//     - Variant name descriptor maximum length ( ( variant name length * 4 ) + 2 )
//     - Variant name
10,
12,0,0,0,
4,0,0,0,
18,
'n','a','m','e',
// - properties[0]::name
//   - Variant value
//     - Version 1.0
//     - Variant value type, EVariantTypeDesC
//     - Variant value length (LE)
//     - Variant value descriptor maximum length ( ( variant value length * 4 ) + 2 )
//     - Variant value
10,
5,
6,0,0,0,
26,
'h','e','i','g','t','h',
// - prperties[0]::value
//   - Variant name
//     - Version 1.0
//     - Semantic ID (LE)    
//     - Variant name length (LE)
//     - Variant name descriptor maximum length ( ( variant name length * 4 ) + 2 )
//     - Variant name
10,
12,0,0,0,
5,0,0,0,
22,
'v','a','l','u','e',
// - properties[0]::value
//   - Variant value
//     - Version 1.0
//     - Variant value type, EVariantTypeDesC
//     - Variant value length (LE)
//     - Variant value descriptor maximum length ( ( variant value length * 4 ) + 2 )
//     - Variant value
10,
5,
2,0,0,0,
10,
'2','0',
// - settings[0]::properties
//   - Version 1.0
//   - List item starts
10,
0,0,0,0,
// - properties[1]
//   - Version 1.0
//   - Variant value type, EVariantTypeMap
10,
8,
// - properties[1] map
//   - Version 1.0
//   - Item count (LE)
10,
2,0,0,0,
// - prperties[1]::name
//   - Variant name
//     - Version 1.0
//     - Semantic ID (LE)    
//     - Variant name length (LE)
//     - Variant name descriptor maximum length ( ( variant name length * 4 ) + 2 )
//     - Variant name
10,
12,0,0,0,
4,0,0,0,
18,
'n','a','m','e',
// - properties[1]::name
//   - Variant value
//     - Version 1.0
//     - Variant value type, EVariantTypeDesC
//     - Variant value length (LE)
//     - Variant value descriptor maximum length ( ( variant value length * 4 ) + 2 )
//     - Variant value
10,
5,
5,0,0,0,
22,
'w','i','d','t','h',
// - prperties[1]::value
//   - Variant name
//     - Version 1.0
//     - Semantic ID (LE)    
//     - Variant name length (LE)
//     - Variant name descriptor maximum length ( ( variant name length * 4 ) + 2 )
//     - Variant name
10,
12,0,0,0,
5,0,0,0,
22,
'v','a','l','u','e',
// - properties[1]::value
//   - Variant value
//     - Version 1.0
//     - Variant value type, EVariantTypeDesC
//     - Variant value length (LE)
//     - Variant value descriptor maximum length ( ( variant value length * 4 ) + 2 )
//     - Variant value
10,
5,
3,0,0,0,
14,
'2','4','0',
// - settings[0]::properties
//   - Version 1.0
//   - List item starts
10,
0,0,0,0,
// - properties[2]
//   - Version 1.0
//   - Variant value type, EVariantTypeMap
10,
8,
// - properties[2] map
//   - Version 1.0
//   - Item count (LE)
10,
2,0,0,0,
// - properties[2]::name
//   - Variant name
//     - Version 1.0
//     - Semantic ID (LE)    
//     - Variant name length (LE)
//     - Variant name descriptor maximum length ( ( variant name length * 4 ) + 2 )
//     - Variant name
10,
12,0,0,0,
4,0,0,0,
18,
'n','a','m','e',
// - properties[2]::name
//   - Variant value
//     - Version 1.0
//     - Variant value type, EVariantTypeDesC
//     - Variant value length (LE)
//     - Variant value descriptor maximum length ( ( variant value length * 4 ) + 2 )
//     - Variant value
10,
5,
6,0,0,0,
26,
'l','o','c','k','e','d',
// - properties[2]::value
//   - Variant name
//     - Version 1.0
//     - Semantic ID (LE)    
//     - Variant name length (LE)
//     - Variant name descriptor maximum length ( ( variant name length * 4 ) + 2 )
//     - Variant name
10,
12,0,0,0,
5,0,0,0,
22,
'v','a','l','u','e',
// - properties[2]::value
//   - Variant value
//     - Version 1.0
//     - Variant value type, EVariantTypeDesC
//     - Variant value length (LE)
//     - Variant value descriptor maximum length ( ( variant value length * 4 ) + 2 )
//     - Variant value
10,
5,
3,0,0,0,
14,
'y','e','s',
// - pluginConf::resources
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
'r','e','s','o','u','r','c','e','s',
// - pluginConf::resources
//   - Variant value
//     - Version 1.0
//     - Variant value type, EVariantTypeList
10,
7,
// - pluginConf::resources
//   - Version 1.0
//   - Item count (LE)
10,
2,0,0,0,
// - pluginConf::resources
//   - Version 1.0
//   - List item starts
10,
0,0,0,0,
// - object[0]
//   - Version 1.0
//   - Variant value type, EVariantTypeMap
10,
8,
// - object[0] map
//   - Version 1.0
//   - Item count (LE)
10,
4,0,0,0,
// - object[0]::name
//   - Variant name
//     - Version 1.0
//     - Semantic ID (LE)    
//     - Variant name length (LE)
//     - Variant name descriptor maximum length ( ( variant name length * 4 ) + 2 )
//     - Variant name
10,
12,0,0,0,
4,0,0,0,
18,
'n','a','m','e',
// - object[0]::name
//   - Variant value
//     - Version 1.0
//     - Variant value type, EVariantTypeDesC
//     - Variant value length (LE)
//     - Variant value descriptor maximum length ( ( variant value length * 4 ) + 2 )
//     - Variant value
10,
5,
11,0,0,0,
46,
'h','s','_','l','o','g','o','.','j','p','g',
// - object[0]::path
//   - Variant name
//     - Version 1.0
//     - Semantic ID (LE)    
//     - Variant name length (LE)
//     - Variant name descriptor maximum length ( ( variant name length * 4 ) + 2 )
//     - Variant name
10,
12,0,0,0,
4,0,0,0,
18,
'p','a','t','h',
// - object[0]::path
//   - Variant value
//     - Version 1.0
//     - Variant value type, EVariantTypeDesC
//     - Variant value length (LE)
//     - Variant value descriptor maximum length ( ( variant value length * 4 ) + 2 )
//     - Variant value
10,
5,
37,0,0,0,
150,
'2','4','5','6','\\','2','7','0','5','1','3','7','5','1','\\','5','3','6','9','1','6','2','5','6','\\','1','.','0','\\','s','o','u','r','c','e','s','\\',
// - object[0]::mediatype
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
'm','e','d','i','a','t','y','p','e',
// - object[0]::mediatype
//   - Variant value
//     - Version 1.0
//     - Variant value type, EVariantTypeDesC
//     - Variant value length (LE)
//     - Variant value descriptor maximum length ( ( variant value length * 4 ) + 2 )
//     - Variant value
10,
5,
9,0,0,0,
38,
'm','i','m','e','_','t','y','p','e',
// - Object[0]::tag
//   - Variant name
//     - Version 1.0
//     - Semantic ID (LE)    
//     - Variant name length (LE)
//     - Variant name descriptor maximum length ( ( variant name length * 4 ) + 2 )
//     - Variant name
10,
12,0,0,0,
3,0,0,0,
14,
't','a','g',
// - Object[0]::tag
//   - Variant value
//     - Version 1.0
//     - Variant value type, EVariantTypeDesC
//     - Variant value length (LE)
//     - Variant value descriptor maximum length ( ( variant value length * 4 ) + 2 )
//     - Variant value
10,
5,
0,0,0,0,
2,
// - pluginConf::resources
//   - Version 1.0
//   - List item starts
10,
0,0,0,0,
// - object[1]
//   - Version 1.0
//   - Variant value type, EVariantTypeMap
10,
8,
// - object[1] map
//   - Version 1.0
//   - Item count (LE)
10,
4,0,0,0,
// - object[1]::name
//   - Variant name
//     - Version 1.0
//     - Semantic ID (LE)    
//     - Variant name length (LE)
//     - Variant name descriptor maximum length ( ( variant name length * 4 ) + 2 )
//     - Variant name
10,
12,0,0,0,
4,0,0,0,
18,
'n','a','m','e',
// - object[1]::name
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
'w','i','d','g','e','t','.','b','m','p',
// - object[1]::path
//   - Variant name
//     - Version 1.0
//     - Semantic ID (LE)    
//     - Variant name length (LE)
//     - Variant name descriptor maximum length ( ( variant name length * 4 ) + 2 )
//     - Variant name
10,
12,0,0,0,
4,0,0,0,
18,
'p','a','t','h',
// - object[1]::path
//   - Variant value
//     - Version 1.0
//     - Variant value type, EVariantTypeDesC
//     - Variant value length (LE)
//     - Variant value descriptor maximum length ( ( variant value length * 4 ) + 2 )
//     - Variant value
10,
5,
37,0,0,0,
150,
'2','4','5','6','\\','2','7','0','5','1','3','7','5','1','\\','5','3','6','9','1','6','2','5','6','\\','1','.','0','\\','s','o','u','r','c','e','s','\\',
// - object[1]::mediatype
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
'm','e','d','i','a','t','y','p','e',
// - object[1]::mediatype
//   - Variant value
//     - Version 1.0
//     - Variant value type, EVariantTypeDesC
//     - Variant value length (LE)
//     - Variant value descriptor maximum length ( ( variant value length * 4 ) + 2 )
//     - Variant value
10,
5,
9,0,0,0,
38,
'm','i','m','e','_','t','y','p','e',
// - Object[1]::tag
//   - Variant name
//     - Version 1.0
//     - Semantic ID (LE)    
//     - Variant name length (LE)
//     - Variant name descriptor maximum length ( ( variant name length * 4 ) + 2 )
//     - Variant name
10,
12,0,0,0,
3,0,0,0,
14,
't','a','g',
// - Object[1]::tag
//   - Variant value
//     - Version 1.0
//     - Variant value type, EVariantTypeDesC
//     - Variant value length (LE)
//     - Variant value descriptor maximum length ( ( variant value length * 4 ) + 2 )
//     - Variant value
10,
5,
0,0,0,0,
2,
};


#endif /* MT_HSPS_REPLACEPLUGIN_1_H_ */

