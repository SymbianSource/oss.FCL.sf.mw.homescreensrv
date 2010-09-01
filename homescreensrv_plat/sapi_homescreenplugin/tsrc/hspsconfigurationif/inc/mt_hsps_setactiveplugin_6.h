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
* Description:  Test case SetActivePlugin(6) test data
*
*/


#ifndef C_MT_HSPS_SETACTIVEPLUGIN_6_H
#define C_MT_HSPS_SETACTIVEPLUGIN_6_H

/* 
SetActivePlugin(6)
------------------
 
Test purpose

The purpose of this test case is to verify that status element with KErrArgument 
error code is returned when plugin id parameter is not defined.

Pre-conditions

There must be installed test themes for Active Idle application and Typical 
configuration must be set as active (Typical view 1 is set as active).

Test steps

Test step 1:
•   Input:
    SetActivePlugin()
•   Expected output:
    Status element with KErrArgument error code

*/

// Test step 1 method:

const TUint8 setactiveplugin_6_ts_1_method[] = "SetActivePlugin";

// Test step 1 input:

const TUint8 setactiveplugin_6_ts_1_input[] = {
// SetActivePlugin(Input)
// - Version 1.0
// - Item count (LE)
10,
0,0,0,0
};

// Test step 1 output:

const TUint8 setactiveplugin_6_ts_1_output[] = {
// SetActivePlugin(Output)
// - Version 1.0
// - Item count (LE)
10,
1,0,0,0,
// - SetActivePlugin(Output)::status
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

#endif // C_MT_HSPS_SETACTIVEPLUGIN_6_H
