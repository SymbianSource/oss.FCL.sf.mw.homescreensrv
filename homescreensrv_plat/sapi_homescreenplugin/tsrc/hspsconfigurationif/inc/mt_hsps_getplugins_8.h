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
* Description:  Test case GetPlugins(8) test data
*
*/


#ifndef C_MT_HSPS_GETPLUGINS_8_H
#define C_MT_HSPS_GETPLUGINS_8_H

/* 
GetPlugins(8)
----------------
 
Test purpose
Verify that logo files are copied to client�s private folder.

Pre-conditions
There must be installed test themes for Active Idle application and Operator configuration must be set as active 

Test steps
Test step 1:
�   Input:
    GetPlugins(�0x0998�, �widget�,�True� )
�   Expected output
    Plugin list holding the widget configurations, logo files should exist in the target directory
*/

// Test step 1 method:

const TUint8 getplugins_8_ts_1_method[] = "GetPlugins";

// Test step 1 input:

const TUint8 getplugins_8_ts_1_input[] = {
// GetPlugins(Input)
// - Version 1.0
// - Item count (LE)
10,
3,0,0,0,
// - GetPlugins(Input)::interface
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
// - GetPlugins(Input)::interface
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
// - GetPlugins(Input)::type
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
// - GetPlugins(Input)::type
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
// - GetPlugins(Input)::copylogos
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
'c','o','p','y','l','o','g','o','s',
// - GetPlugins(Input)::copylogos
//   - Variant value
//     - Version 1.0
//     - Variant value type, EVariantTypeTBool
//     - Variant value length (LE)
//     - Variant value descriptor maximum length ( ( variant value length * 4 ) + 2 )
//     - Variant value
10,
11,
1,0,0,0,
6,
'1'
};

// Test step 1 output:

const TUint8 getplugins_8_ts_1_output[] = {
// GetPluginConf(Output)
// - Version 1.0
// - Item count (LE)
10,
1,0,0,0,
// GetPlugins(Output)::plugins
// - Variant name
//   - Version 1.0
//   - Semantic ID (LE)    
//   - Variant name length (LE)
//   - Variant name descriptor maximum length ( ( variant name length * 4 ) + 2 )
//   - Variant name
10,
12,0,0,0,
7,0,0,0,
30,
'p','l','u','g','i','n','s',
// GetPlugins(Output)::plugins
// - Variant value
//   - Version 1.0
//   - Variant value type, EVariantTypeList
10,
7,
// GetPlugins(Output)::plugins
// - Version 1.0
// - Item count (LE)
10,
3,0,0,0,
// GetPlugins(Output)::plugins
// - Version 1.0
// - List item starts
10,
0,0,0,0,
// - plugins[0]
//   - Version 1.0
//   - Variant value type, EVariantTypeMap
10,
8,
// - plugins[0] map
//   - Version 1.0
//   - Item count (LE)
10,
7,0,0,0,
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
'0','x','2','0','0','0','b','1','3','2',
// - plugins[0]::interface
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
// - plugins[0]::interface
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
// - plugins[0]::type
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
// - plugins[0]::type
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
// - plugins[0]::name
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
// - plugins[0]::name
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
// - plugins[0]::multiinstance
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
// - plugins[0]::multiinstance
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
// - plugins[0]::description
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
// - plugins[0]::description
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
// - plugins[0]::logo
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
'l','o','g','o',
// - plugins[0]::logo
//   - Variant value
//     - Version 1.0
//     - Variant value type, EVariantTypeDesC
//     - Variant value length (LE)
//     - Variant value descriptor maximum length ( ( variant value length * 4 ) + 2 )
//     - Variant value
10,
5,
75,0,0,0,
93,
2,
'm','i','f','(','c',':','\\','p','r','i','v','a','t','e','\\','2','0','0','0','0','F','B','1','\\','2','4','5','6','\\','2','7','0','5','1','3','7','5','1','\\','5','3','6','9','1','6','2','7','4','\\','1','.','0','\\','s','o','u','r','c','e','s','\\','d','u','m','m','y','.','m','i','f',' ','1',' ','2',')',
// GetPlugins(Output)::plugins
// - Version 1.0
// - List item starts
10,
0,0,0,0,
// - plugins[1]
//   - Version 1.0
//   - Variant value type, EVariantTypeMap
10,
8,
// - plugins[1] map
//   - Version 1.0
//   - Item count (LE)
10,
6,0,0,0,
// - plugins[1]::uid
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
// - plugins[1]::uid
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
'0','x','2','0','0','0','b','1','2','0',
// - plugins[1]::interface
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
// - plugins[1]::interface
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
// - plugins[1]::type
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
// - plugins[1]::type
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
// - plugins[1]::name
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
// - plugins[1]::name
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
// - plugins[1]::multiinstance
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
// - plugins[1]::multiinstance
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
// - plugins[1]::description
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
// - plugins[1]::description
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
// GetPlugins(Output)::plugins
// - Version 1.0
// - List item starts
10,
0,0,0,0,
// - plugins[2]
//   - Version 1.0
//   - Variant value type, EVariantTypeMap
10,
8,
// - plugins[2] map
//   - Version 1.0
//   - Item count (LE)
10,
6,0,0,0,
// - plugins[2]::uid
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
// - plugins[2]::uid
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
'0','x','2','0','0','0','b','1','0','2',
// - plugins[2]::interface
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
// - plugins[2]::interface
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
// - plugins[2]::type
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
// - plugins[2]::type
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
// - plugins[2]::name
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
// - plugins[2]::name
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
// - plugins[2]::multiinstance
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
// - plugins[2]::multiinstance
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
// - plugins[2]::description
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
// - plugins[2]::description
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

#endif // C_MT_HSPS_GETPLUGINS_8_H
