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
* Description:  Test case RestoreActiveAppConf(2) test data
*
*/

#ifndef C_MT_HSPS_RESTOREACTIVEAPPCONF_2_H
#define C_MT_HSPS_RESTOREACTIVEAPPCONF_2_H

/* 
RestoreActiveAppConf(2)
-------------------
 
Test purpose:

Verify that Minimal configuration gets re-installed from ROM 
(due to the statuslicenceerestorable status) if the active application 
configuration is in error state prior to the GetActiveAppConf request.

Pre-conditions:

•   There must be installed test themes for ActiveIdle application and Minimal 
    configuration must be set as active.

Test steps:

Test step 1:
•   Input:
    SetConfState
•   Expected output:
    Active application configuration was successfully set to error state

Test step 2:
•   Input:
    GetActiveAppConf
•   Expected output:
    Minimal configuration with configuration state “Confirmed”

*/


// Test step 1 method:
const TUint8 restoreactiveappconf_2_ts_1_method[] = "SetConfState";

// Test step 1 input:
const TUint8 restoreactiveappconf_2_ts_1_input[] = {
// SetConfState(Input)
// - Version 1.0
// - Item count (LE)
10,
2,0,0,0,
// - SetConfState(Input)::confid
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
// - SetConfState(Input)::value
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
// - SetConfState(Input)::state
//   - Variant name
//     - Version 1.0
//     - Semantic ID (LE)    
//     - Variant name length (LE)
//     - Variant name descriptor maximum length ( ( variant name length * 4 ) + 2 )
//     - Variant name
10,
12,0,0,0,
5,0,0,0,
22,
's','t','a','t','e',
// - SetConfState(Input)::value
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
'e','r','r','o','r'
};

// Test step 1 expected output:
const TUint8 restoreactiveappconf_2_ts_1_output[] = {
// SetConfState(Output)
// - Version 1.0
// - Item count (LE)
10,
1,0,0,0,
// - SetConfState(Output)::status
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
// - SetConfState(Input)::value
//   - Variant value
//     - Version 1.0
//     - Variant value type, EVariantTypeTInt32
//     - Variant value length (LE)
//     - Variant value descriptor maximum length ( ( variant value length * 4 ) + 2 )
//     - Variant value
10,
1,
0,0,0,0
};




// Test step 2 method:
const TUint8 restoreactiveappconf_2_ts_2_method[] = "GetActiveAppConf";


// Test step 2 input:
const TUint8 restoreactiveappconf_2_ts_2_input = 0;

 
// Test step 2 expected output:
const TUint8 restoreactiveappconf_2_ts_2_output[] = {
// GetActiveAppConf(Output)
// - Version 1.0
// - Item count (LE)
10,
1,0,0,0,
// - GetActiveAppConf(Output)::status
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
// - GetActiveAppConf(Input)::value
//   - Variant value
//     - Version 1.0
//     - Variant value type, EVariantTypeTInt32
//     - Variant value length (LE)
//     - Variant value descriptor maximum length ( ( variant value length * 4 ) + 2 )
//     - Variant value
10,
1,
255,255,255,255
};

#endif // C_MT_HSPS_RESTOREACTIVEAPPCONF_2_H
