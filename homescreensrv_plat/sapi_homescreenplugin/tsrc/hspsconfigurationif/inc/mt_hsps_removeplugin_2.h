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
* Description:  Test case RemovePlugin(2) test data
*
*/


#ifndef C_MT_HSPS_REMOVEPLUGIN_2_H
#define C_MT_HSPS_REMOVEPLUGIN_2_H

/* 
RemovePlugin(2)
----------------
 
Test purpose

Verify that the service fails when invalid inputs are provided.
Service inputs: plug-in id(int).  

Pre-conditions
•   N/A

Test steps

Test step 1:
•   Input:
    RemovePlugin( “NULL”)
•   Expected output:
    Failure due to the missing mandatory argument

Test step 2:
•   Input:
    RemovePlugin( 0 )
•   Expected output:
    Failure due to the missing mandatory argument
    
Test step 3:
•   Input:
    RemovePlugin( 0xFF )
•   Expected output:
    Failure due to the invalid input    

Test step 4:
•   Input:
    RemovePlugin( -1 )
•   Expected output:
    Failure due to plugin not found error

Test step 5:
•   Input:
    RemovePlugin( "ÖP8" )
•   Expected output:
    Failure due to plugin not found error

*/



// Test step 1 method:

const TUint8 removeplugin_2_ts_1_method[] = "RemovePlugin";

// Test step 1 input:

const TUint8 removeplugin_2_ts_1_input[] = {
// RemovePlugin(Input)
// - Version 1.0
// - Item count (LE)
10,
0,0,0,0
};

// Test step 1 output:

const TUint8 removeplugin_2_ts_1_output[] = {
// RemovePlugin(Output)
// - Version 1.0
// - Item count (LE)
10,
1,0,0,0,
// - RemovePlugin(Output)::status
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
// - RemovePlugin(Input)::status
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

const TUint8 removeplugin_2_ts_2_method[] = "RemovePlugin";

// Test step 2 input:

const TUint8 removeplugin_2_ts_2_input[] = {
// RemovePlugin(Input)
// - Version 1.0
// - Item count (LE)
10,
1,0,0,0,
// - RemovePlugin(Input)::pluginId
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
'p','l','u','g','i','n','I','d',
// - RemovePlugin(Input)::pluginId
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

// Test step 2 output:

const TUint8 removeplugin_2_ts_2_output[] = {
// RemovePlugin(Output)
// - Version 1.0
// - Item count (LE)
10,
1,0,0,0,
// - RemovePlugin(Output)::status
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
// - RemovePlugin(Input)::status
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

const TUint8 removeplugin_2_ts_3_method[] = "RemovePlugin";

// Test step 3 input:

const TUint8 removeplugin_2_ts_3_input[] = {
// RemovePlugin(Input)
// - Version 1.0
// - Item count (LE)
10,
1,0,0,0,
// - RemovePlugin(Input)::pluginId
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
'p','l','u','g','i','n','I','d',
// - RemovePlugin(Input)::pluginId
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
'0','x','F','F'
};

// Test step 3 output:

const TUint8 removeplugin_2_ts_3_output[] = {
// RemovePlugin(Output)
// - Version 1.0
// - Item count (LE)
10,
1,0,0,0,
// - RemovePlugin(Output)::status
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
// - RemovePlugin(Input)::status
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

const TUint8 removeplugin_2_ts_4_method[] = "RemovePlugin";

// Test step 4 input:

const TUint8 removeplugin_2_ts_4_input[] = {
// RemovePlugin(Input)
// - Version 1.0
// - Item count (LE)
10,
1,0,0,0,
// - RemovePlugin(Input)::pluginId
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
'p','l','u','g','i','n','I','d',
// - RemovePlugin(Input)::pluginId
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
'-','1'
};

// Test step 4 output:

const TUint8 removeplugin_2_ts_4_output[] = {
// RemovePlugin(Output)
// - Version 1.0
// - Item count (LE)
10,
1,0,0,0,
// - RemovePlugin(Output)::status
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
// - RemovePlugin(Input)::status
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




// Test step 5 method:

const TUint8 removeplugin_2_ts_5_method[] = "RemovePlugin";

// Test step 5 input:

const TUint8 removeplugin_2_ts_5_input[] = {
// RemovePlugin(Input)
// - Version 1.0
// - Item count (LE)
10,
1,0,0,0,
// - RemovePlugin(Input)::pluginId
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
'p','l','u','g','i','n','I','d',
// - RemovePlugin(Input)::pluginId
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
'Ö','P','8'
};

// Test step 5 output:

const TUint8 removeplugin_2_ts_5_output[] = {
// RemovePlugin(Output)
// - Version 1.0
// - Item count (LE)
10,
1,0,0,0,
// - RemovePlugin(Output)::status
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
// - RemovePlugin(Input)::status
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

#endif // C_MT_HSPS_REMOVEPLUGIN_2_H
