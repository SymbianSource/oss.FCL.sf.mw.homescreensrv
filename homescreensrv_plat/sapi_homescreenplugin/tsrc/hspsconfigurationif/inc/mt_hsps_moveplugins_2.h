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
* Description:  Test case MovePlugins(2) test data
*
*/


#ifndef MT_HSPS_MOVEPLUGINS_2_H_
#define MT_HSPS_MOVEPLUGINS_2_H_

/* 
MovePlugins(2)
----------------
 
Test purpose

The purpose of this test case is to verify that the plugins order change fails 
if the configuration id is invalid

Pre-conditions
•   There must be installed test themes for Active Idle application and Typical 
    configuration must be set as active

Test steps

Test step 1:
•   Input:
    MovePlugins(“Invalid configuration”,”4,3,2”)
•   Expected output:
    Plugins order change fails
    
*/

// Test step 1 method:

const TUint8 moveplugins_2_ts_1_method[] = "MovePlugins";

// Test step 1 input:

const TUint8 moveplugins_2_ts_1_input[] = {
// MovePlugins(Input)
// - Version 1.0
// - Item count (LE)
10,
2,0,0,0,
// - MovePlugins::confId
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
// - MovePlugins::confId
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
'9','9',
// - MovePlugins::plugins
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
// - MovePlugins::plugins
//   - Variant value
//     - Version 1.0
//     - Variant value type, EVariantTypeList
10,
7,
// - MovePlugins(Input)::plugins
//   - Version 1.0
//   - Item count (LE)
10,
3,0,0,0,
// - MovePlugins::plugins
//   - Version 1.0
//   - List item starts
10,
0,0,0,0,
// - plugins[0]
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
// - MovePlugins::plugins
//   - Version 1.0
//   - List item starts
10,
0,0,0,0,
// - plugins[1]
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
// - MovePlugins::plugins
//   - Version 1.0
//   - List item starts
10,
0,0,0,0,
// - plugins[2]
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
'2'
};

// Test step 1 output:
const TUint8 moveplugins_2_ts_1_output[] = {
// MovePlugins(Output)
// - Version 1.0
// - Item count (LE)
10,
1,0,0,0,
// - MovePlugins(Output)::status
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
// - MovePlugins(Output)::status
//   - Variant value
//     - Version 1.0
//     - Variant value type, EVariantTypeTInt32
//     - Variant value, KErrArgument
10,
1,
250,255,255,255
};


#endif /*MT_HSPS_MOVEPLUGINS_2_H_*/
