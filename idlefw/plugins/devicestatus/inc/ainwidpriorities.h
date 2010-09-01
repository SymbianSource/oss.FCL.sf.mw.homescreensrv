/*
* Copyright (c) 2006 Nokia Corporation and/or its subsidiary(-ies).
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
* Description:  Network identity related priorities.
*
*/



#ifndef AINWIDPRIORITIES_H
#define AINWIDPRIORITIES_H

#include <e32base.h>


const TInt KHighestPriority = 20;

enum TAiNWIdPrioritities
    {
    EAiInvalidPriority      = -1,
    EAiOperatorNetInfoName  = 0,
    EAiMCC_MCN,
    EAiCountryMNC,
    EAiLatinOperatorName,
    EAiUnicodeOperatorName,
    EAiNITZ,
    EAiNetworkOperatorName,
    EAiServiceProviderName,
    EAiFlexiblePLMN,
    EAiProgOperatorLogo,
    EAiOTAOperatorLogo,
    EAiOfflineProfile,
    EAiBTSAP
    };


#endif // AINWIDPRIORITIES_H
