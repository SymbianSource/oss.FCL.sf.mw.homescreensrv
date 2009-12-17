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
* Description:  Test case GetAppConfs(1) test data
*
*/


#ifndef C_MT_HSPS_GETAPPCONFS_1_H
#define C_MT_HSPS_GETAPPCONFS_1_H

/* 
GetAppConfs(1)
-------------------
 
Test purpose

The purpose of this test case is to verify that the list of application 
configurations is successfully returned

Pre-conditions
•   There must be installed test themes for Active Idle application and Minimal 
    configuration must be set as active

Test steps

Test step 1:
•   Input:
    GetAppConfs()
•   Expected output:
    List of application configurations containing Minimal and Typical 
    configurations.

*/

// Test step 1 method:

const TUint8 getappconfs_1_ts_1_method[] = "GetAppConfs";


// Test step 1 input:

const TUint8 getappconfs_1_ts_1_input = 0;

 
// Test step 1 expected output:


const TUint8 getappconfs_1_ts_1_output[] = {
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
8,0,0,0,
34,
'a','p','p','C','o','n','f','s',
// GetPlugins(Output)::appConfs
// - Variant value
//   - Version 1.0
//   - Variant value type, EVariantTypeList
10,
7,
// GetPlugins(Output)::appConfs
// - Version 1.0
// - Item count (LE)
10,
3,0,0,0,
// GetPlugins(Output)::appConfs
// - Version 1.0
// - List item starts
10,
0,0,0,0,
// - appConfs[2]
//   - Version 1.0
//   - Variant value type, EVariantTypeMap
10,
8,
// - appConfs[2] map
//   - Version 1.0
//   - Item count (LE)
10,
2,0,0,0,
// - appConfs[2]::uid
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
// - appConfs[2]::uid
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
'0','x','2','0','0','0','b','1','3','0',
// - appConfs[2]::name
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
// - appConfs[2]::name
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
'O','p','e','r','a','t','o','r',' ','-',' ','r','o','o','t',
// GetPlugins(Output)::appConfs
// - Version 1.0
// - List item starts
10,
0,0,0,0,
// - appConfs[0]
//   - Version 1.0
//   - Variant value type, EVariantTypeMap
10,
8,
// - appConfs[0] map
//   - Version 1.0
//   - Item count (LE)
10,
2,0,0,0,
// - appConfs[0]::uid
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
// - appConfs[0]::uid
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
'0','x','2','0','0','0','b','1','1','0',
// - appConfs[0]::name
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
// - appConfs[0]::name
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
'T','y','p','i','c','a','l',' ','-',' ','R','o','o','t',
// GetPlugins(Output)::appConfs
// - Version 1.0
// - List item starts
10,
0,0,0,0,
// - appConfs[1]
//   - Version 1.0
//   - Variant value type, EVariantTypeMap
10,
8,
// - appConfs[1] map
//   - Version 1.0
//   - Item count (LE)
10,
2,0,0,0,
// - appConfs[1]::uid
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
// - appConfs[1]::uid
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
'0','x','2','0','0','0','b','1','0','0',
// - appConfs[1]::name
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
// - appConfs[1]::name
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
'R','o','o','t',' ','c','o','n','f',' ','#','1'
};

#endif // C_MT_HSPS_GETAPPCONFS_1_H
