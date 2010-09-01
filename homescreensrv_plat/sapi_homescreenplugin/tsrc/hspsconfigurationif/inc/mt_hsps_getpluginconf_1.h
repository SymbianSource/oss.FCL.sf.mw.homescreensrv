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
* Description:  Test case GetPluginConf(1) test data
*
*/


#ifndef C_MT_HSPS_GETPLUGINCONF_1_H
#define C_MT_HSPS_GETPLUGINCONF_1_H

/* 
GetPluginConf(1)
----------------
 
Test purpose:

The purpose of this test case is to verify that correct s60_view_plugin_1 
configuration is returned when GetPluginConf method is called.

Pre-conditions:

There must be installed test themes for Active Idle application and Minimal 
configuration must be set as active.

Test steps:

Test step 1:
•   Input:
    GetPluginConf(“s60_view_plugin_1”)
•   Expected output:
    s60_view_plugin_1 configuration with configuration state “NotConfirmed”

Test step 2:
•   Input:
    SetConfState(“Confirmed”)
•   Expected output:
    Configuration state successfully updated

Test step 3:
•   Input:
    GetPluginConf(“s60_view_plugin_1”)
•   Expected output:
    s60_view_plugin_1 configuration with configuration state “Confirmed”

Test step 4:
•   Input:
    Re-attach HSPS service
•   Expected output:
    HSPS service interface is successfully closed and opened

Test step 5:
•   Input:
    GetPluginConf(“s60_view_plugin_1”)
•   Expected output:
    s60_view_plugin_1 configuration with configuration state “Confirmed”

*/

// Test step 1 method:

const TUint8 getpluginconf_1_ts_1_method[] = "GetPluginConf";

// Test step 1 input:

const TUint8 getpluginconf_1_ts_1_input[] = {
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
'1'
};

// Test step 1 output:

const TUint8 getpluginconf_1_ts_1_output[] = {
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
'2',
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
'0','x','2','0','0','0','B','1','0','1',
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
4,0,0,0,
18,
'v','i','e','w',
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
12,0,0,0,
50,
'V','i','e','w',' ','C','o','n','f',' ','#','1',
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
'6',
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
1,0,0,0,
// - pluginConf::plugins
//   - Version 1.0
//   - List item starts
10,
0,0,0,0,
// - pluginConf::plugins[0]
//   - Version 1.0
//   - Variant value type, EVariantTypeMap
10,
8,
// - plugins[0] map
//   - Version 1.0
//   - Item count (LE)
10,
4,0,0,0,
// - plugins[0]::id
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
// - plugins[0]::id
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
// - plugins[0]::uid
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
// - plugins[0]::uid
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
// - plugins[0]::activationstate
//   - Variant name
//     - Version 1.0
//     - Semantic ID (LE)    
//     - Variant name length (LE)
//     - Variant name descriptor maximum length ( ( variant name length * 4 ) + 2 )
//     - Variant name
10,
12,0,0,0,
15,0,0,0,
62,
'a','c','t','i','v','a','t','i','o','n','s','t','a','t','e',
// - plugins[0]::activationstate
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
// - plugins[0]::locking_status
//   - Variant name
//     - Version 1.0
//     - Semantic ID (LE)    
//     - Variant name length (LE)
//     - Variant name descriptor maximum length ( ( variant name length * 4 ) + 2 )
//     - Variant name
10,
12,0,0,0,
14,0,0,0,
58,
'l','o','c','k','i','n','g','_','s','t','a','t','u','s',
// - plugins[0]::locking_status
//   - Variant value
//     - Version 1.0
//     - Variant value type, EVariantTypeDesC
//     - Variant value length (LE)
//     - Variant value descriptor maximum length ( ( variant value length * 4 ) + 2 )
//     - Variant value
10,
5,
4,0,0,0,
18,
'n','o','n','e',
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

// - items[0]
//   - Version 1.0
//   - Variant value type, EVariantTypeMap
10,
8,
// - items[0] map
//   - Version 1.0
//   - Item count (LE)
10,
3,0,0,0,
// - items[0]::itemId
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
// - items[0]::itemId
//   - Variant value
//     - Version 1.0
//     - Variant value type, EVariantTypeDesC
//     - Variant value length (LE)
//     - Variant value descriptor maximum length ( ( variant value length * 4 ) + 2 )
//     - Variant value (empty)
10,
5,
7,0,0,0,
30,
'b','g','c','o','l','o','r',
// - items[0]::name
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
// - items[0]::name
//   - Variant value
//     - Version 1.0
//     - Variant value type, EVariantTypeDesC
//     - Variant value length (LE)
//     - Variant value descriptor maximum length ( ( variant value length * 4 ) + 2 )
//     - Variant value (empty)
10,
5,
16,0,0,0,
66,
'B','a','c','k','g','r','o','u','n','d',' ','c','o','l','o','r',
// - items[0]::properties
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
// - items[0]::properties
//   - Variant value
//     - Version 1.0
//     - Variant value type, EVariantTypeList
10,
7,
// - items[0]::properties
//   - Version 1.0
//   - Item count (LE)
10,
1,0,0,0,
// - items[0]::properties
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
5,0,0,0,
22,
'v','a','l','u','e',
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
9,0,0,0,
38,
'2','0','0',',','1','0','0',',','0',
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
4,0,0,0,
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
8,0,0,0,
34,
'm','a','i','n','.','x','m','l',
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
'2','4','5','6','\\','2','7','0','5','1','3','7','5','1','\\','5','3','6','9','1','6','2','2','5','\\','1','.','0','\\','s','o','u','r','c','e','s','\\',
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
20,0,0,0,
82,
't','e','x','t','/','a','p','p','l','i','c','a','t','i','o','n','+','x','m','l',
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
23,0,0,0,
94,
'v','i','e','w','n','a','v','i','g','a','t','i','o','n','r','u','l','e','s','.','x','m','l',
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
'2','4','5','6','\\','2','7','0','5','1','3','7','5','1','\\','5','3','6','9','1','6','2','2','5','\\','1','.','0','\\','s','o','u','r','c','e','s','\\',
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
20,0,0,0,
82,
't','e','x','t','/','a','p','p','l','i','c','a','t','i','o','n','+','x','m','l',
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
// - pluginConf::resources
//   - Version 1.0
//   - List item starts
10,
0,0,0,0,
// - object[2]
//   - Version 1.0
//   - Variant value type, EVariantTypeMap
10,
8,
// - object[2] map
//   - Version 1.0
//   - Item count (LE)
10,
4,0,0,0,
// - object[2]::name
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
// - object[2]::name
//   - Variant value
//     - Version 1.0
//     - Variant value type, EVariantTypeDesC
//     - Variant value length (LE)
//     - Variant value descriptor maximum length ( ( variant value length * 4 ) + 2 )
//     - Variant value
10,
5,
13,0,0,0,
54,
'r','e','s','o','u','r','c','e','.','f','i','l','e',
// - object[2]::path
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
// - object[2]::path
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
'2','4','5','6','\\','2','7','0','5','1','3','7','5','1','\\','5','3','6','9','1','6','2','2','5','\\','1','.','0','\\','s','o','u','r','c','e','s','\\',
// - object[2]::mediatype
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
// - object[2]::mediatype
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
// - Object[2]::tag
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
// - Object[2]::tag
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
// - object[3]
//   - Version 1.0
//   - Variant value type, EVariantTypeMap
10,
8,
// - object[3] map
//   - Version 1.0
//   - Item count (LE)
10,
4,0,0,0,
// - object[3]::name
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
// - object[3]::name
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
'p','i','c','t','u','r','e','.','j','p','e','g',
// - object[3]::path
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
// - object[3]::path
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
'2','4','5','6','\\','2','7','0','5','1','3','7','5','1','\\','5','3','6','9','1','6','2','2','5','\\','1','.','0','\\','s','o','u','r','c','e','s','\\',
// - object[3]::mediatype
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
// - object[3]::mediatype
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
// - Object[3]::tag
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
// - Object[3]::tag
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

// Test step 2 method:

const TUint8 getpluginconf_1_ts_2_method[] = "SetConfState";

// Test step 2 input:

const TUint8 getpluginconf_1_ts_2_input[] = {
// AddPlugin(Input)
// - Version 1.0
// - Item count (LE)
10,
2,0,0,0,
// - SetConfState(Input)::confId
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
'c','o','n','f','I','d',
// - SetConfState(Input)::confId
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
// - SetConfState(Input)::state
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
// - SetConfState(Input)::state
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
'C','o','n','f','i','r','m','e','d'
};

// Test step 2 output:

const TUint8 getpluginconf_1_ts_2_output[] = {
// SetConfState(Output)
// - Version 1.0
// - Item count (LE)
10,
1,0,0,0,
// - SetConfState(Output)::status
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
// - SetConfState(Input)::status
//   - Variant value
//     - Version 1.0
//     - Variant value type, EVariantTypeTInt32
//     - Variant value length (LE)
//     - Variant value descriptor maximum length ( ( variant value length * 4 ) + 2 )
//     - Variant value
10,
1,
0,0,0,0
};

// Test step 3 method:

const TUint8 getpluginconf_1_ts_3_method[] = "GetPluginConf";

// Test step 3 input:

const TUint8 getpluginconf_1_ts_3_input[] = {
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
'1'
};

// Test step 3 output:

const TUint8 getpluginconf_1_ts_3_output[] = {
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
'2',
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
'0','x','2','0','0','0','B','1','0','1',
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
4,0,0,0,
18,
'v','i','e','w',
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
12,0,0,0,
50,
'V','i','e','w',' ','C','o','n','f',' ','#','1',
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
9,0,0,0,
38,
'C','o','n','f','i','r','m','e','d',
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
'6',
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
1,0,0,0,
// - pluginConf::plugins
//   - Version 1.0
//   - List item starts
10,
0,0,0,0,
// - pluginConf::plugins[0]
//   - Version 1.0
//   - Variant value type, EVariantTypeMap
10,
8,
// - plugins[0] map
//   - Version 1.0
//   - Item count (LE)
10,
4,0,0,0,
// - plugins[0]::id
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
// - plugins[0]::id
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
// - plugins[0]::uid
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
// - plugins[0]::uid
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
// - plugins[0]::activationstate
//   - Variant name
//     - Version 1.0
//     - Semantic ID (LE)    
//     - Variant name length (LE)
//     - Variant name descriptor maximum length ( ( variant name length * 4 ) + 2 )
//     - Variant name
10,
12,0,0,0,
15,0,0,0,
62,
'a','c','t','i','v','a','t','i','o','n','s','t','a','t','e',
// - plugins[0]::activationstate
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
// - plugins[0]::locking_status
//   - Variant name
//     - Version 1.0
//     - Semantic ID (LE)    
//     - Variant name length (LE)
//     - Variant name descriptor maximum length ( ( variant name length * 4 ) + 2 )
//     - Variant name
10,
12,0,0,0,
14,0,0,0,
58,
'l','o','c','k','i','n','g','_','s','t','a','t','u','s',
// - plugins[0]::locking_status
//   - Variant value
//     - Version 1.0
//     - Variant value type, EVariantTypeDesC
//     - Variant value length (LE)
//     - Variant value descriptor maximum length ( ( variant value length * 4 ) + 2 )
//     - Variant value
10,
5,
4,0,0,0,
18,
'n','o','n','e',
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

// - items[0]
//   - Version 1.0
//   - Variant value type, EVariantTypeMap
10,
8,
// - items[0] map
//   - Version 1.0
//   - Item count (LE)
10,
3,0,0,0,
// - items[0]::itemId
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
// - items[0]::itemId
//   - Variant value
//     - Version 1.0
//     - Variant value type, EVariantTypeDesC
//     - Variant value length (LE)
//     - Variant value descriptor maximum length ( ( variant value length * 4 ) + 2 )
//     - Variant value (empty)
10,
5,
7,0,0,0,
30,
'b','g','c','o','l','o','r',
// - items[0]::name
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
// - items[0]::name
//   - Variant value
//     - Version 1.0
//     - Variant value type, EVariantTypeDesC
//     - Variant value length (LE)
//     - Variant value descriptor maximum length ( ( variant value length * 4 ) + 2 )
//     - Variant value (empty)
10,
5,
16,0,0,0,
66,
'B','a','c','k','g','r','o','u','n','d',' ','c','o','l','o','r',
// - items[0]::properties
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
// - items[0]::properties
//   - Variant value
//     - Version 1.0
//     - Variant value type, EVariantTypeList
10,
7,
// - items[0]::properties
//   - Version 1.0
//   - Item count (LE)
10,
1,0,0,0,
// - items[0]::properties
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
5,0,0,0,
22,
'v','a','l','u','e',
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
9,0,0,0,
38,
'2','0','0',',','1','0','0',',','0',
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
4,0,0,0,
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
8,0,0,0,
34,
'm','a','i','n','.','x','m','l',
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
'2','4','5','6','\\','2','7','0','5','1','3','7','5','1','\\','5','3','6','9','1','6','2','2','5','\\','1','.','0','\\','s','o','u','r','c','e','s','\\',
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
20,0,0,0,
82,
't','e','x','t','/','a','p','p','l','i','c','a','t','i','o','n','+','x','m','l',
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
23,0,0,0,
94,
'v','i','e','w','n','a','v','i','g','a','t','i','o','n','r','u','l','e','s','.','x','m','l',
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
'2','4','5','6','\\','2','7','0','5','1','3','7','5','1','\\','5','3','6','9','1','6','2','2','5','\\','1','.','0','\\','s','o','u','r','c','e','s','\\',
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
20,0,0,0,
82,
't','e','x','t','/','a','p','p','l','i','c','a','t','i','o','n','+','x','m','l',
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
// - pluginConf::resources
//   - Version 1.0
//   - List item starts
10,
0,0,0,0,
// - object[2]
//   - Version 1.0
//   - Variant value type, EVariantTypeMap
10,
8,
// - object[2] map
//   - Version 1.0
//   - Item count (LE)
10,
4,0,0,0,
// - object[2]::name
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
// - object[2]::name
//   - Variant value
//     - Version 1.0
//     - Variant value type, EVariantTypeDesC
//     - Variant value length (LE)
//     - Variant value descriptor maximum length ( ( variant value length * 4 ) + 2 )
//     - Variant value
10,
5,
13,0,0,0,
54,
'r','e','s','o','u','r','c','e','.','f','i','l','e',
// - object[2]::path
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
// - object[2]::path
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
'2','4','5','6','\\','2','7','0','5','1','3','7','5','1','\\','5','3','6','9','1','6','2','2','5','\\','1','.','0','\\','s','o','u','r','c','e','s','\\',
// - object[2]::mediatype
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
// - object[2]::mediatype
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
// - Object[2]::tag
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
// - Object[2]::tag
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
// - object[3]
//   - Version 1.0
//   - Variant value type, EVariantTypeMap
10,
8,
// - object[3] map
//   - Version 1.0
//   - Item count (LE)
10,
4,0,0,0,
// - object[3]::name
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
// - object[3]::name
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
'p','i','c','t','u','r','e','.','j','p','e','g',
// - object[3]::path
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
// - object[3]::path
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
'2','4','5','6','\\','2','7','0','5','1','3','7','5','1','\\','5','3','6','9','1','6','2','2','5','\\','1','.','0','\\','s','o','u','r','c','e','s','\\',
// - object[3]::mediatype
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
// - object[3]::mediatype
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
// - Object[3]::tag
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
// - Object[3]::tag
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

// Test step 5 method:

#define getpluginconf_1_ts_5_method getpluginconf_1_ts_3_method

// Test step 5 input:

#define getpluginconf_1_ts_5_input getpluginconf_1_ts_3_input

// Test step 5 output:

#define getpluginconf_1_ts_5_output getpluginconf_1_ts_3_output

#endif // C_MT_HSPS_GETPLUGINCONF_1_H
