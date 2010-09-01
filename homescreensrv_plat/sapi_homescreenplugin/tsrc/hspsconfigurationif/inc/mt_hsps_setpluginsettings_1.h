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
* Description:  Test case SetPluginSettings(1) test data
*
*/


#ifndef C_MT_HSPS_SETPLUGINSETTINGS_1_H
#define C_MT_HSPS_SETPLUGINSETTINGS_1_H

/* 
SetPluginSettings(1)
----------------
 
Test purpose

The purpose of this test case is to verify that removing of a plugin fails, 
when a plugin request to be removed cannot be found.

Pre-conditions
•   There must be installed test themes for Active Idle application and 
    Minimal configuration must be set as active

Test steps

Test step 1:
•   Input:
    SetPluginSettings(“plugin id-Settings list”)
•   Expected output:
    Set Settings succeed

Test step 2:
•   Input:
    SetPluginSettings(“plugin id-Settings list”) same as installed
•   Expected output:
    Set Settings succeed

*/

// Test step 1 method:

const TUint8 setpluginsettings_1_ts_1_method[] = "SetPluginSettings";

// Test step 1 input:

const TUint8 setpluginsettings_1_ts_1_input[] = {
// SetPluginSettings(Input)
// - Version 1.0
// - Item count (LE)
10,
2,0,0,0,
// - SetPluginSettings(Input)::pluginId
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
// - SetPluginSettings(Input)::pluginId
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
12,0,0,0,
50,
'&','q','t','n','_','b','g','c','o','l','o','r',
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
'1','2','3',',','1','2','3',',','0'
};
// Test step 1 output:

const TUint8 setpluginsettings_1_ts_1_output[] =  {
// SetPluginSettings(Output)
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

const TUint8 setpluginsettings_1_ts_2_method[] = "SetPluginSettings";

// Test step 2 input:

const TUint8 setpluginsettings_1_ts_2_input[] = {
// SetPluginSettings(Input)
// - Version 1.0
// - Item count (LE)
10,
2,0,0,0,
// - SetPluginSettings(Input)::pluginId
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
// - SetPluginSettings(Input)::pluginId
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
12,0,0,0,
50,
'&','q','t','n','_','b','g','c','o','l','o','r',
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
'1','2','3',',','1','2','3',',','0'
};
// Test step 1 output:

const TUint8 setpluginsettings_1_ts_2_output[] =  {
// SetPluginSettings(Output)
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



// Test step 1 method:

const TUint8 setpluginsettings_1_ts_3_method[] = "SetPluginSettings";

// Test step 1 input:

const TUint8 setpluginsettings_1_ts_3_input[] = {
// SetPluginSettings(Input)
// - Version 1.0
// - Item count (LE)
10,
2,0,0,0,
// - SetPluginSettings(Input)::pluginId
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
// - SetPluginSettings(Input)::pluginId
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
12,0,0,0,
50,
'&','q','t','n','_','b','g','c','o','l','o','r',
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
'2','0','0',',','1','0','0',',','0'
};
// Test step 1 output:

const TUint8 setpluginsettings_1_ts_3_output[] =  {
// SetPluginSettings(Output)
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
#endif // C_MT_HSPS_REMOVEPLUGIN_1_H
