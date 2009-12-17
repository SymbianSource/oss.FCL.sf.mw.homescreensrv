/*
* Copyright (c) 2005-2006 Nokia Corporation and/or its subsidiary(-ies).
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
* Description:  Common client/server definitions for Shortcut plug-in
*
*/


#ifndef AISCUTEXTDEFS_H
#define AISCUTEXTDEFS_H

_LIT( KAiScutExtServerName, "AiScutExtServer" );

const TUint KAiScutExtServMajorVersionNumber = 1;
const TUint KAiScutExtServMinorVersionNumber = 0;
const TUint KAiScutExtServBuildVersionNumber = 0;

const TUint KMaxPopupTextLines = 3;

/**
 * xSP extension server requests
 */
enum TAiScutExtServRequest
    {
    EAiScutExtServSetTargetDefinition = 1,
    EAiScutExtServSetPopupText,
    EAiScutExtServResetPopupText,
    EAiScutExtServSetIcon,
    EAiScutExtServResetIcon,
    EAiScutExtServIsInShortcuts,
    EAiScutExtServIssuePutInShortcuts
    };

/**
 * AiScutExtServer client panic codes
 */
enum TAiScutExtServerPanic
    {
    EBadRequest = 1
    };

#endif // AISCUTEXTDEFS_H

// End of File.
