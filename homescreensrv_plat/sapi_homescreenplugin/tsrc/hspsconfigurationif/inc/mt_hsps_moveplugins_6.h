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
* Description:  Test case MovePlugins(6) test data
*
*/


#ifndef MT_HSPS_MOVEPLUGINS_6_H_
#define MT_HSPS_MOVEPLUGINS_6_H_

/* 
MovePlugins(6)
----------------
 
Test purpose
Verify that the service fails when invalid inputs are provided.
Service inputs: configuration id(int), comma seperated plug-in ids(int).

Pre-conditions
•   N/A

Test steps

Test step 1:
•   Input:
    MovePlugins(“NULL”)
•   Expected output:
    Failure due to the missing/mandatory arguments
    
Test step 2:
•   Input:
    MovePlugins( "0xFF" )
•   Expected output:
    Failure due to the missing/mandatory arguments

Test step 3:
•   Input:
    MovePlugins( "0", "0" )
•   Expected output:
    Failure due to the invalid arguments
    
Test step 4:
•   Input:
    MovePlugins( "1", "0,1,2" )
•   Expected output:
    Failure due to the invalid arguments    
    
Test step 5:
•   Input:
    MovePlugins( "1", "abc,0xff,2" )
•   Expected output:
    Failure due to the invalid arguments

    
*/



// Test step 1 method:

const TUint8 moveplugins_6_ts_1_method[] = "MovePlugins";

// Test step 1 input:

const TUint8 moveplugins_6_ts_1_input[] = {
// MovePlugins(Input)
// - Version 1.0
// - Item count (LE)
10,
0,0,0,0
};

// Test step 1 output:
const TUint8 moveplugins_6_ts_1_output[] = {
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



// Test step 2 method:

const TUint8 moveplugins_6_ts_2_method[] = "MovePlugins";

// Test step 2 input:

const TUint8 moveplugins_6_ts_2_input[] = {
// MovePlugins(Input)
// - Version 1.0
// - Item count (LE)
10,
1,0,0,0,
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
1,0,0,0,
6,
'0xFF'
};

// Test step 2 output:
const TUint8 moveplugins_6_ts_2_output[] = {
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



// Test step 3 method:

const TUint8 moveplugins_6_ts_3_method[] = "MovePlugins";

// Test step 3 input:

const TUint8 moveplugins_6_ts_3_input[] = {
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
1,0,0,0,
6,
'0',
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
1,0,0,0,
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
'0',
// - MovePlugins::plugins
//   - Version 1.0
//   - List item starts
10,
0,0,0,0
};

// Test step 3 output:
const TUint8 moveplugins_6_ts_3_output[] = {
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



// Test step 4 method:

const TUint8 moveplugins_6_ts_4_method[] = "MovePlugins";

// Test step 4 input:

const TUint8 moveplugins_6_ts_4_input[] = {
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
1,0,0,0,
6,
'1',
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
'0',
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
'1',
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

// Test step 4 output:
const TUint8 moveplugins_6_ts_4_output[] = {
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



// Test step 5 method:

const TUint8 moveplugins_6_ts_5_method[] = "MovePlugins";

// Test step 5 input:

const TUint8 moveplugins_6_ts_5_input[] = {
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
1,0,0,0,
6,
'1',
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
3,0,0,0,
14,
'a','b','c',
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
4,0,0,0,
18,
'0','x','F','F',
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

// Test step 5 output:
const TUint8 moveplugins_6_ts_5_output[] = {
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
254,255,255,255
};



#endif /*MT_HSPS_MOVEPLUGINS_6_H_*/
