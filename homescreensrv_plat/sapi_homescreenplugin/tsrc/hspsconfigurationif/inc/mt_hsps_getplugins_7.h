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
* Description:  Test case GetPlugins(7) test data
*
*/


#ifndef C_MT_HSPS_GETPLUGINS_7_H
#define C_MT_HSPS_GETPLUGINS_7_H

/* 
GetPlugins(7)
----------------
 
Test purpose
The purpose of this test case is to verify that GetPlugins fails 
correctly if interface is wrongly defined as a string.

Pre-conditions
There must be installed test themes for Active Idle application

Test steps
Test step 1:
•   Input:
    GetPlugins(“flash”, “widget”)
•   Expected output
    Error not found
*/

// Test step 1 method:

const TUint8 getplugins_7_ts_1_method[] = "GetPlugins";

// Test step 1 input:

const TUint8 getplugins_7_ts_1_input[] = {
// GetPlugins(Input)
// - Version 1.0
// - Item count (LE)
10,
2,0,0,0,
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
'f','l','a','s','h',
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
'w','i','d','g','e','t'
};

// Test step 1 output:

const TUint8 getplugins_7_ts_1_output[] = {
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

#endif // C_MT_HSPS_GETPLUGINS_5_H
