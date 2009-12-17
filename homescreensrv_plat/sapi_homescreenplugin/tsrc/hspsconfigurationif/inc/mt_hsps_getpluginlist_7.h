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
* Description:  Test case GetPluginList(7) test data
*
*/


#ifndef C_MT_HSPS_GETPLUGINLIST_7_H
#define C_MT_HSPS_GETPLUGINLIST_7_H

/* 
GetPluginList(7)
----------------
 
Test purpose

The purpose of this test case is to verify that empty plugin list is returned 
when GetPluginList is called with plugin type not found from configuration

Pre-conditions

There must be installed test themes for Active Idle application and Operator 
configuration must be set as active

Test steps

Test step 1:
�   Input:
    GetPluginList(�widget�)
�   Expected output
    Empty plugin list

*/

// Test step 1 method:

const TUint8 getpluginlist_7_ts_1_method[] = "GetPluginList";

// Test step 1 input:

const TUint8 getpluginlist_7_ts_1_input[] = {
// GetPluginList(Input)
// - Version 1.0
// - Item count (LE)
10,
1,0,0,0,
// - GetPluginList(Input)::type
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
// - GetPluginList(Input)::pluginId
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

const TUint8 getpluginlist_7_ts_1_output[] = {
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
0,0,0,0
};

#endif // C_MT_HSPS_GETPLUGINLIST_7_H
