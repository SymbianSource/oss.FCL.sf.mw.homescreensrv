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
* Description:  Test case GetPluginList(1) test data
*
*/


#ifndef C_MT_HSPS_GETPLUGINLIST_1_H
#define C_MT_HSPS_GETPLUGINLIST_1_H

/* 
GetPluginList(1)
----------------
 
Test purpose

The purpose of this test case is to verify that correct list of plugins included 
in Typical configuration is returned

Pre-conditions

There must be installed test themes for Active Idle application and Typical 
configuration must be set as active

Test steps

Test step 1:
•   Input:
    GetPluginList()
•   Expected output
    List of all Typical configuration plugins:
    o   Typical view conf 1
    o   Typical view conf 2
    o   Typical widget conf (Typical view conf 1 plugin 1)
    o   Typical widget conf (Typical view conf 1 plugin 2)
    o   Typical widget conf (Typical view conf 1 plugin 3)
    o   Typical widget conf (Typical view conf 2 plugin 1)
    o   Typical widget conf (Typical view conf 2 plugin 2)
    o   Typical widget conf (Typical view conf 2 plugin 3)
•   Execution time measured

*/

// Test step 1 method:

const TUint8 getpluginlist_1_ts_1_method[] = "GetPluginList";

// Test step 1 input:

const TUint8 getpluginlist_1_ts_1_input = 0;

// Test step 1 output:

const TUint8 getpluginlist_1_ts_1_output[] = {
// GetPluginList(Output)
// - Version 1.0
// - Item count (LE)
10,
1,0,0,0,
// GetPluginList(Output)::plugins
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
// GetPluginList(Output)::plugins
// - Variant value
//   - Version 1.0
//   - Variant value type, EVariantTypeList
10,
7,
// GetPluginList(Output)::plugins
// - Version 1.0
// - Item count (LE)
10,
8,0,0,0,
// GetPluginList(Output)::plugins
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
6,0,0,0,
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
'1',
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
4,0,0,0,
18,
'v','i','e','w',
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
15,0,0,0,
62,
'T','y','p','i','c','a','l',' ','-',' ','V','i','e','w','1',
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
// GetPluginList(Output)::plugins
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
//   - 
10,
6,0,0,0,
// - plugins[1]::id
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
// - plugins[1]::id
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
'5',
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
4,0,0,0,
18,
'v','i','e','w',
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
15,0,0,0,
62,
'T','y','p','i','c','a','l',' ','-',' ','V','i','e','w','2',
// - plugins[1]::activationstate
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
// - plugins[1]::activationstate
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
// - plugins[1]::locking_status
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
// - plugins[1]::locking_status
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
// GetPluginList(Output)::plugins
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
//   - 
10,
6,0,0,0,
// - plugins[2]::id
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
// - plugins[2]::id
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
16,0,0,0,
66,
'T','y','p','i','c','a','l',' ','-',' ','W','i','d','g','e','t',
// - plugins[2]::activationstate
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
// - plugins[2]::activationstate
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
2,
// - plugins[2]::locking_status
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
// - plugins[2]::locking_status
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
// GetPluginList(Output)::plugins
// - Version 1.0
// - List item starts
10,
0,0,0,0,
// - plugins[3]
//   - Version 1.0
//   - Variant value type, EVariantTypeMap
10,
8,
// - plugins[3] map
//   - Version 1.0
//   - 
10,
6,0,0,0,
// - plugins[3]::id
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
// - plugins[3]::id
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
// - plugins[3]::type
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
// - plugins[3]::type
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
// - plugins[3]::name
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
// - plugins[3]::name
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
// - plugins[3]::activationstate
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
// - plugins[3]::activationstate
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
// - plugins[3]::description
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
// - plugins[3]::description
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
// - plugins[3]::locking_status
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
// - plugins[3]::locking_status
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
// GetPluginList(Output)::plugins
// - Version 1.0
// - List item starts
10,
0,0,0,0,
// - plugins[4]
//   - Version 1.0
//   - Variant value type, EVariantTypeMap
10,
8,
// - plugins[4] map
//   - Version 1.0
//   - 
10,
6,0,0,0,
// - plugins[4]::id
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
// - plugins[4]::id
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
'4',
// - plugins[4]::type
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
// - plugins[4]::type
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
// - plugins[4]::name
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
// - plugins[4]::name
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
// - plugins[4]::activationstate
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
// - plugins[4]::activationstate
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
// - plugins[4]::description
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
// - plugins[4]::description
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
// - plugins[4]::locking_status
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
// - plugins[4]::locking_status
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
// GetPluginList(Output)::plugins
// - Version 1.0
// - List item starts
10,
0,0,0,0,
// - plugins[5]
//   - Version 1.0
//   - Variant value type, EVariantTypeMap
10,
8,
// - plugins[5] map
//   - Version 1.0
//   - 
10,
6,0,0,0,
// - plugins[5]::id
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
// - plugins[5]::id
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
// - plugins[5]::type
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
// - plugins[5]::type
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
// - plugins[5]::name
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
// - plugins[5]::name
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
// - plugins[5]::activationstate
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
// - plugins[5]::activationstate
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
// - plugins[5]::description
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
// - plugins[5]::description
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
// - plugins[5]::locking_status
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
// - plugins[5]::locking_status
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
// GetPluginList(Output)::plugins
// - Version 1.0
// - List item starts
10,
0,0,0,0,
// - plugins[6]
//   - Version 1.0
//   - Variant value type, EVariantTypeMap
10,
8,
// - plugins[6] map
//   - Version 1.0
//   - 
10,
6,0,0,0,
// - plugins[6]::id
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
// - plugins[6]::id
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
'7',
// - plugins[6]::type
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
// - plugins[6]::type
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
// - plugins[6]::name
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
// - plugins[6]::name
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
// - plugins[6]::activationstate
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
// - plugins[6]::activationstate
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
// - plugins[6]::description
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
// - plugins[6]::description
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
// - plugins[6]::locking_status
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
// - plugins[6]::locking_status
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
// GetPluginList(Output)::plugins
// - Version 1.0
// - List item starts
10,
0,0,0,0,
// - plugins[7]
//   - Version 1.0
//   - Variant value type, EVariantTypeMap
10,
8,
// - plugins[7] map
//   - Version 1.0
//   - 
10,
6,0,0,0,
// - plugins[7]::id
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
// - plugins[7]::id
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
'8',
// - plugins[7]::type
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
// - plugins[7]::type
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
// - plugins[7]::name
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
// - plugins[7]::name
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
// - plugins[7]::activationstate
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
// - plugins[7]::activationstate
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
// - plugins[7]::description
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
// - plugins[7]::description
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
// - plugins[7]::locking_status
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
// - plugins[7]::locking_status
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
'n','o','n','e'
};

#endif // C_MT_HSPS_GETPLUGINLIST_1_H
