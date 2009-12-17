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


#ifndef AISCUTENGINEFACTORY_H
#define AISCUTENGINEFACTORY_H

#include <e32std.h>

class CAiScutEngine;
class CAiScutPlugin;
class CAiScutShortcut;

/**
 *  AiScutEngineFactory
 *
 *  Creates various classes used in AiScutPlugin.
 *
 *  @since S60 v3.2
 */
class AiScutFactory
    {
    public:
        /**
         * Creates CAiScutEngine
         * @since S60 v3.2
         */
        static CAiScutEngine* CreateAiScutEngineL( CAiScutPlugin& aPlugin );

        /**
         * Creates CAiScutShortcut
         * @since S60 v3.2
         */
        static CAiScutShortcut* CreateAiScutShortcutL( TInt aId, 
            const TDesC& aTarget, CAiScutEngine& aEngine );

        /**
         * Creates CAiScutShortcut
         * @since S60 v3.2
         */
        static CAiScutShortcut* CreateAiScutShortcutLC( TInt aId, 
            const TDesC& aTarget, CAiScutEngine& aEngine );
    };

#endif // AISCUTENGINEFACTORY_H

// End of File.
