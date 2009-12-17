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
* Description:  Test case AddPlugin(10) test data
*
*/


#ifndef C_MT_HSPS_ADDPLUGIN_10_H
#define C_MT_HSPS_ADDPLUGIN_10_H

/* 
AddPlugin(10)
----------------
 
Test purpose

Verify that the service fails when invalid inputs are provided.
Service inputs: configuration id(int), plug-in uid(hex), position(int). 

Pre-conditions
ï   None

Test steps

Test step 1:

ï   Input:
    AddPlugin( ìNULLì ) 
ï   Expected output:
    Failure due to the missing mandatory arguments

Test step 2:

ï   Input:
    AddPlugin( "1" ) 
ï   Expected output:
    Failure due to the missing mandatory arguments
        
Test step 3:

ï   Input:
    AddPlugin(".÷*", ".÷*") 
ï   Expected output:
    Failure due to the invalid inputs
   
Test step 4:

ï   Input:
    AddPlugin("÷÷÷", "÷÷÷","÷÷÷") 
ï   Expected output:
    Failure due to the invalid input


Test step 5:

ï   Input:
    AddPlugin("0xXX", "0xXX","0xXX")
ï   Expected output:
    Failure due to the invalid input 
    
Test step 6:

ï   Input:
    AddPlugin("0xXX9", "0xABC" )
ï   Expected output:
    Failure due to the invalid input

Test step 7:

ï   Input:
    AddPlugin(0, 0, 0)
ï   Expected output:
    Failure due to the invalid input

Test step 8:

ï   Input:
    AddPlugin(999999, 999999, 999999)
ï   Expected output:
    Failure due to the invalid input

*/

// Test step 1 method:

const TUint8 addplugin_10_ts_1_method[] = "AddPlugin";

// Test step 1 input:

const TUint8 addplugin_10_ts_1_input[] = {
// AddPlugin(Input)
// - Version 1.0
// - Item count (LE)
10,
0,0,0,0,
};

// Test step 1 output:

const TUint8 addplugin_10_ts_1_output[] = {
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
250,255,255,255
};



// Test step 2 method:

const TUint8 addplugin_10_ts_2_method[] = "AddPlugin";

// Test step 2 input:

const TUint8 addplugin_10_ts_2_input[] = {
// AddPlugin(Input)
// - Version 1.0
// - Item count (LE)
10,
1,0,0,0,
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
'1'
};

// Test step 2 output:

const TUint8 addplugin_10_ts_2_output[] = {
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
250,255,255,255
};


// Test step 3 method:

const TUint8 addplugin_10_ts_3_method[] = "AddPlugin";

// Test step 3 input:

const TUint8 addplugin_10_ts_3_input[] = {
// AddPlugin(Input)
// - Version 1.0
// - Item count (LE)
10,
1,0,0,0,
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
3,0,0,0,
14,
'.','÷','*',
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
3,0,0,0,
14,
'.','÷','*'
};

// Test step 3 output:

const TUint8 addplugin_10_ts_3_output[] = {
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
250,255,255,255
};



// Test step 4 method:

const TUint8 addplugin_10_ts_4_method[] = "AddPlugin";

// Test step 4 input:

const TUint8 addplugin_10_ts_4_input[] = {
// AddPlugin(Input)
// - Version 1.0
// - Item count (LE)
10,
2,0,0,0,
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
3,0,0,0,
14,
'÷','÷','÷',
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
3,0,0,0,
14,
'÷','÷','÷'
};

// Test step 4 output:

const TUint8 addplugin_10_ts_4_output[] = {
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



// Test step 5 method:

const TUint8 addplugin_10_ts_5_method[] = "AddPlugin";

// Test step 5 input:

const TUint8 addplugin_10_ts_5_input[] = {
// AddPlugin(Input)
// - Version 1.0
// - Item count (LE)
10,
2,0,0,0,
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
4,0,0,0,
18,
'0','x','X','X',
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
4,0,0,0,
16,
'0','x','X','X'
};

// Test step 5 output:

const TUint8 addplugin_10_ts_5_output[] = {
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



// Test step 6 method:

const TUint8 addplugin_10_ts_6_method[] = "AddPlugin";

// Test step 6 input:

const TUint8 addplugin_10_ts_6_input[] = {
// AddPlugin(Input)
// - Version 1.0
// - Item count (LE)
10,
2,0,0,0,
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
5,0,0,0,
22,
'0','x','X','X','9',
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
5,0,0,0,
22,
'0','x','A','B','C'
};

// Test step 6 output:

const TUint8 addplugin_10_ts_6_output[] = {
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
250,255,255,255
};



// Test step 7 method:

const TUint8 addplugin_10_ts_7_method[] = "AddPlugin";

// Test step 7 input:

const TUint8 addplugin_10_ts_7_input[] = {
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
'0',
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
1,0,0,0,
6,
'0',
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

// Test step 7 output:

const TUint8 addplugin_10_ts_7_output[] = {
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



// Test step 8 method:

const TUint8 addplugin_10_ts_8_method[] = "AddPlugin";

// Test step 7 input:

const TUint8 addplugin_10_ts_8_input[] = {
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
7,0,0,0,
30,
'9','9','9','9','9','9','9',
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
7,0,0,0,
30,
'9','9','9','9','9','9','9',
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
7,0,0,0,
30,
'9','9','9','9','9','9','9'
};

// Test step 8 output:

const TUint8 addplugin_10_ts_8_output[] = {
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




#endif // C_MT_HSPS_ADDPLUGIN_10_H
