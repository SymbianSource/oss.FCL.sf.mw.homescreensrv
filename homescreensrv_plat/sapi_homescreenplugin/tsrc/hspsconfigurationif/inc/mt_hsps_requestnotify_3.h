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
* Description:  Test case RequestNotify(1) test data
*
*/


#ifndef C_MT_HSPS_REQUESTNOTIFY_3_H
#define C_MT_HSPS_REQUESTNOTIFY_3_H

/* 
RequestNotify(3)
----------------
 
Test purpose

The purpose of this test case is to verify that change notify request works

Pre-conditions
•   There must be installed test themes for Active Idle application and 
    Typical configuration must be set as active

Test steps

Test step 1:
•   Input:
    RequestNotify(“”)
•   Expected output:
    Notified added plugin

Test step 2:
•   Input:
    GetActiveAppConf()
•   Expected output:
    Active Idle Typical configuration

*/

// Test step 1 method:

const TUint8 requestnotify_3_ts_1_method[] = "RequestNotification";

// Test step 1 input:

const TUint8 requestnotify_3_ts_1_input = 0;
// Test step 1 output:

const TUint8 requestnotify_3_ts_1_output[] = {
// RequestNotification (Output)
// - Version 1.0
// - Item count (LE)
10,
1,0,0,0,
// - RequestNotification (Output)::notification (map)
//   - Variant name
//     - Version 1.0
//     - Semantic ID (LE)    
//     - Variant name length (LE)
//     - Variant name descriptor maximum length ( ( variant name length * 4 ) + 2 )
//     - Variant name
10,
12,0,0,0,
12,0,0,0,//6,0,0,0,
50,//26,
'n','o','t','i','f','i','c','a','t','i','o','n',
// - Variant value
//   - Version 1.0
//   - Variant value type, EVariantTypeMap
10,
8,
// - Notification
//   - Version 1.0
//   - Item count (LE)
10,
6,0,0,0,
// - Notification(Output)::event
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
'e','v','e','n','t',
// - Notification(Output)::event
//   - Variant value
//     - Version 1.0
//     - Variant value type, EVariantTypeDesC
//     - Variant value length (LE)
//     - Variant value descriptor maximum length ( ( variant value length * 4 ) + 2 )
//     - Variant value
10,
5,
15,0,0,0,
62,
'P','l','u','g','i','n','I','n','s','t','a','l','l','e','d',
// - Notification(Output)::appConfUid
//   - Variant name
//     - Version 1.0
//     - Semantic ID (LE)    
//     - Variant name length (LE)
//     - Variant name descriptor maximum length ( ( variant name length * 4 ) + 2 )
//     - Variant name
10,
12,0,0,0,
10,0,0,0,
42,
'a','p','p','C','o','n','f','U','i','d',
// - Notification(Output)::pluginUid
//   - Variant value
//     - Version 1.0
//     - Variant value type, EVariantTypeDesC
//     - Variant value length (LE)
//     - Variant value descriptor maximum length ( ( variant value length * 4 ) + 2 )
//     - Variant value
10,
5,
0,0,0,0,
2,
// - Notification(Output)::origUid
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
'o','r','i','g','U','i','d',
// - RequestNotification(Output)::origUid
//   - Variant value
//     - Version 1.0
//     - Variant value type, EVariantTypeDesC
//     - Variant value length (LE)
//     - Variant value descriptor maximum length ( ( variant value length * 4 ) + 2 )
//     - Variant value
10,
5,
0,0,0,0,
2,
// - Notification(Output)::pluginName
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
'n','a','m','e',
// - Notification(Output)::event
//   - Variant value
//     - Version 1.0
//     - Variant value type, EVariantTypeDesC
//     - Variant value length (LE)
//     - Variant value descriptor maximum length ( ( variant value length * 4 ) + 2 )
//     - Variant value
10,
5,
18,0,0,0,
74,
'I','n','s','t','a','l','l','e','d',' ','-',' ','W','i','d','g','e','t',
// - Notification(Output)::pluginUid
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
// - Notification(Output)::pluginUid
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
'0','x','2','0','0','0','b','1','3','3',
// -  Notification(Output)::pluginIds
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
'p','l','u','g','i','n','I','d','s',
// - Notification::pluginIds
//   - Variant value
//     - Version 1.0
//     - Variant value type, EVariantTypeList
10,
7,
// - Notification::pluginIds
//   - Version 1.0
//   - Item count (LE)
10,
1,0,0,0,
// Notification::pluginIds
// - Version 1.0
// - List item starts
10,
0,0,0,0,
// -  Notification::pluginIds[0]
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

const TInt requestnotify_3_ts_1_trigger = 3;

#endif // C_MT_HSPS_REMOVEPLUGIN_1_H
