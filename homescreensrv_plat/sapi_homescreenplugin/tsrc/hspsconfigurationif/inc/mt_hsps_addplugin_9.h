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
* Description:  Test case AddPlugin(9) test data
*
*/


#ifndef C_MT_HSPS_ADDPLUGIN_9_H
#define C_MT_HSPS_ADDPLUGIN_9_H

/* 
AddPlugin(9)
----------------
 
Test purpose

Verify that default locale is used when plugin configuration doesn't have a locale 
for the active device language. In this case, device language should be other than
finnish and the test plugin should have only finnish localization.
In addition, make sure that all resources (locale specific and unlocalized) 
are copied into the client's private folder when the application configuration is 
fetched.   

Pre-conditions
•   There must be installed test themes for Active Idle application and Typical 
    configuration must be set as active

Test steps
   
Test step 1:

•   Input:
    AddPlugin(“Typical view”, “FinnishWidget”,”2”)
•   Expected output:
    Plugin id of successfully added plugin

Test step 2:

•   Input:
    GetPluginConf(“FinnishWidget”)
•   Expected output:
    Resource list was found 
    
Test step 3:

•   Input:
    RemovePlugin(“FinnishWidget”)
•   Expected output:
    Typical configuration was succesfully restored    

*/

// Test step 1 method:

const TUint8 addplugin_9_ts_1_method[] = "AddPlugin";

// Test step 4 input:

const TUint8 addplugin_9_ts_1_input[] = {
// AddPlugin(Input)
// - Version 1.0
// - Item count (LE)
10,
3,0,0,0,
// - AddPlugin(Input)::confId
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
// - AddPlugin(Input)::confId
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
// - AddPlugin(Input)::pluginUid
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
// - AddPlugin(Input)::pluginUid
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
'0','x','2','0','0','0','b','1','3','2',
};

// Test step 1 output:

const TUint8 addplugin_9_ts_1_output[] = {
// AddPlugin(Output)
// - Version 1.0
// - Item count (LE)
10,
1,0,0,0,
// - AddPlugin(Output)::pluginId
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
// - AddPlugin(Output)::pluginId
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
'9'
};



// Test step 2 method:

const TUint8 addplugin_9_ts_2_method[] = "GetPluginConf";

// Test step 2 input:

const TUint8 addplugin_9_ts_2_input[] = {
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
'9'
};

// Test step 2 output:

const TUint8 addplugin_9_ts_2_output[] = {
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
2,0,0,0,
10,
'1','0',
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
'0','x','2','0','0','0','B','1','3','2',
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
'F','i','n','n','i','s','h',' ','-',' ','W','i','d','g','e','t',
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
0,0,0,0,
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
3,0,0,0,
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
9,0,0,0,
38,
'd','u','m','m','y','.','m','i','f',
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
'2','4','5','6','\\','2','7','0','5','1','3','7','5','1','\\','5','3','6','9','1','6','2','7','4','\\','1','.','0','\\','s','o','u','r','c','e','s','\\',
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
0,0,0,0,
2,
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
4,0,0,0,
18,
'l','o','g','o',
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
10,0,0,0,
42,
'c','o','m','m','o','n','.','j','p','g',
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
'2','4','5','6','\\','2','7','0','5','1','3','7','5','1','\\','5','3','6','9','1','6','2','7','4','\\','1','.','0','\\','s','o','u','r','c','e','s','\\',
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
15,0,0,0,
62,
'l','o','c','a','l','i','z','e','d','b','g','.','j','p','g',
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
'2','4','5','6','\\','2','7','0','5','1','3','7','5','1','\\','5','3','6','9','1','6','2','7','4','\\','1','.','0','\\','s','o','u','r','c','e','s','\\',
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
0,0,0,0,
2,
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
2
};

// Test step 3 method:

const TUint8 addplugin_9_ts_3_method[] = "RemovePlugin";

// Test step 3 input:

const TUint8 addplugin_9_ts_3_input[] = {
// RemovePlugin(Input)
// - Version 1.0
// - Item count (LE)
10,
1,0,0,0,
// - RemovePlugin(Input)::pluginId
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
// - RemovePlugin(Input)::pluginId
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
'9'
};

// Test step 3 output:

const TUint8 addplugin_9_ts_3_output[] = {
// RemovePlugin(Output)
// - Version 1.0
// - Item count (LE)
10,
1,0,0,0,
// - RemovePlugin(Output)::status
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
// - RemovePlugin(Input)::status
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


#endif // C_MT_HSPS_ADDPLUGIN_9_H
