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
* Description:  Implementation table for shortcut settings plug-in
*
*/


#include <e32std.h>
#include <ecom/implementationproxy.h>

#include "caiscutsettings.h"
#include <platform/mw/aiscutuids.hrh>

/** Implementation table for shortcut settings plug-in */
const TImplementationProxy KAiScutSettingsImplementationTable[] =
{
    IMPLEMENTATION_PROXY_ENTRY(
        AI_UID_ECOM_IMPLEMENTATION_SETTINGS_SCUTPLUGIN, CAiScutSettings::NewL)
};


// ---------------------------------------------------------------------------
// Gate/factory function.
// ---------------------------------------------------------------------------
//
EXPORT_C const TImplementationProxy* ImplementationGroupProxy(TInt& aTableCount)
{
    aTableCount =
        sizeof(KAiScutSettingsImplementationTable) /
        sizeof(TImplementationProxy);
    return KAiScutSettingsImplementationTable;
}

// End of File.
