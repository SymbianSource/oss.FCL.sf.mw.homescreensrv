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
* Description:  Shortcut plug-in factory class.
*
*/


#include "aiscutfactory.h"
#include "caiscutengineext.h"
#include "caiscutshortcutext.h"

// ---------------------------------------------------------------------------
//
// ---------------------------------------------------------------------------
//
CAiScutEngine* AiScutFactory::CreateAiScutEngineL( CAiScutPlugin& aPlugin )
    {
    return CAiScutEngineExt::NewL( aPlugin );
    }

// ---------------------------------------------------------------------------
//
// ---------------------------------------------------------------------------
//
CAiScutShortcut* AiScutFactory::CreateAiScutShortcutL( TInt aId,
    const TDesC& aTarget, CAiScutEngine& aEngine )
    {
    return CAiScutShortcutExt::NewL( aId, aTarget, aEngine );
    }

// ---------------------------------------------------------------------------
//
// ---------------------------------------------------------------------------
//
CAiScutShortcut* AiScutFactory::CreateAiScutShortcutLC( TInt aId,
    const TDesC& aTarget, CAiScutEngine& aEngine )
    {
    return CAiScutShortcutExt::NewLC( aId, aTarget, aEngine );
    }

// End of File.
