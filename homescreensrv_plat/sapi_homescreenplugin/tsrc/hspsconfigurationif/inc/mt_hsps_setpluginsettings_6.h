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
* Description:  Test case SetPluginSettings(6) test data
*
*/


#ifndef C_MT_HSPS_SETPLUGINSETTINGS_6_H
#define C_MT_HSPS_SETPLUGINSETTINGS_6_H

/* 
SetPluginSettings(6)
----------------
 
Test purpose

The purpose of this test case is to verify that setting of plugin settings fails 
when settings are missing from input params.

Pre-conditions
•   There must be installed test themes for Active Idle application and 
    typical configuration must be set as active

Test steps

Test step 1:
•   Input:
    SetPluginSettings(NULL)
•   Expected output:
    Status not found
*/


// Test step 1 method:

const TUint8 setpluginsettings_6_ts_1_method[] = "SetPluginSettings";

// Test step 1 input:

const TUint8 setpluginsettings_6_ts_1_input[] = {
// SetPluginSettings(Input)
// - Version 1.0
// - Item count (LE)
10,
2,0,0,0,
// - SetPluginSettings(Input)::pluginId
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
// - SetPluginSettings(Input)::pluginId
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
'8',
// - pluginConf::settings
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
's','e','t','t'
};
// Test step 1 output:

const TUint8 setpluginsettings_6_ts_1_output[] =  {
// SetPluginSettings(Output)
// - Version 1.0
// - Item count (LE)
10,
1,0,0,0,
// - SetPluginSettings(Output)::status
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
// - SetPluginSettings::status
//   - Variant value
//     - Version 1.0
//     - Variant value type, EVariantTypeTInt32
//     - Variant value length (LE)
//     - Variant value descriptor maximum length ( ( variant value length * 4 ) + 2 )
//     - Variant value
10,
1,
250,255,255,255 // expected out KErrArgument (-6)
};

#endif // C_MT_HSPS_SETPLUGINSETTINGS_5_H
