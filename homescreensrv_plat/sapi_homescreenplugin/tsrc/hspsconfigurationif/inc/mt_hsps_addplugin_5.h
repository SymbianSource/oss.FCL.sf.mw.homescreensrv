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
* Description:  Test case AddPlugin(5) test data
*
*/


#ifndef C_MT_HSPS_ADDPLUGIN_5_H
#define C_MT_HSPS_ADDPLUGIN_5_H

/* 
AddPlugin(5)
----------------
 
Test purpose

The purpose of this test case is to verify that adding of a plugin configuration 
fails, when a non existing plugin configuration is request to be added to the 
plugin configuration.

Pre-conditions

•   There must be installed test themes for Active Idle application and Minimal 
    configuration must be set as active

Test steps

Test step 1:
•   Input:
    AddPlugin(“Minimal configuration”,“Non existing plugin”, “1”)
•   Expected output:
    Adding fails

*/

// Test step 1 method:

const TUint8 addplugin_5_ts_1_method[] = "AddPlugin";

// Test step 1 input:

const TUint8 addplugin_5_ts_1_input[] = {
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
'1',
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
'0','x','2','f','f','f','f','f','f','f',
// - AddPlugin(Input)::position
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
'p','o','s','i','t','i','o','n',
// - AddPlugin(Input)::position
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
'0'
};

// Test step 1 output:

const TUint8 addplugin_5_ts_1_output[] = {
// AddPlugin(Output)
// - Version 1.0
// - Item count (LE)
10,
1,0,0,0,
// - AddPlugin(Output)::status
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
// - AddPlugin(Input)::status
//   - Variant value
//     - Version 1.0
//     - Variant value type, EVariantTypeTInt32
//     - Variant value length (LE)
//     - Variant value descriptor maximum length ( ( variant value length * 4 ) + 2 )
//     - Variant value
10,
1,
254,255,255,255
};

#endif // C_MT_HSPS_ADDPLUGIN_5_H
