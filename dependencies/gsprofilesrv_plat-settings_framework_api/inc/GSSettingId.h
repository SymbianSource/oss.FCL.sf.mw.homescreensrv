/*
* Copyright (c) 2005 Nokia Corporation and/or its subsidiary(-ies).
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
* Description:   All the setting IDs are declared here
*
*/


#ifndef GSSETTINGID_H
#define GSSETTINGID_H


// CONSTANTS

// buffer size constants
const TInt KGSBufSize128 = 128;

const TInt KGSMapSize = 8;

// connection packet data
const TInt KGSSettIdV42Comp = 1;

//    accessory display
const TInt KGSSettIdAccIU = 0;


enum {
    KGSHeadsetDefProfile,
    KGSLoopsetDefProfile,
    KGSBTAudioDefProfile,
    KGSTTYDefProfile
    };

// call display
enum {
    KGSSettIdOwnNumberSending,
    KGSSettIdCallWaiting,
    KGSSettIdCallCompletionWhenBusy,   
    KGSSettIdSummaryAfterCall,
    KGSSettIdPhoneLineUsed,
    KGSSettIdPhoneLineBlocking,
    KGSSettIdSoftRejectText
    };

#endif // GSSETTINGID_H

// End of File
