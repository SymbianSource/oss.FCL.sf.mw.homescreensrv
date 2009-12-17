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
* Description:  Test case GetPlugins(2) test data
*
*/


#ifndef C_MT_HSPS_GETPLUGINS_2_H
#define C_MT_HSPS_GETPLUGINS_2_H

/* 
GetPlugins(2)
----------------
 
Test purpose

The purpose of this test case is to verify that an empty plugin list is returned.

Pre-conditions

- There must be installed test themes for Active Idle application

Test steps

Test step 1:
-   Input:
    GetPlugins(“0xFFFF”)
-   Expected output
    Empty plugin list
*/

// Test step 1 method:

const TUint8 getplugins_2_ts_1_method[] = "GetPlugins";

// Test step 1 input:

const TUint8 getplugins_2_ts_1_input[] = {
// GetPlugins(Input)
// - Version 1.0
// - Item count (LE)
10,
1,0,0,0,
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
6,0,0,0,
26,
'0','x','f','f','f','f'
};

// Test step 1 output:

const TUint8 getplugins_2_ts_1_output[] = {
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
0,0,0,0
};

#endif // C_MT_HSPS_GETPLUGINS_1_H
