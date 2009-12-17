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
* Description:  Inline implementation for Window server plug-in manager.
*
*/



#ifndef AIWSPLUGINANIM_H
#define AIWSPLUGINANIM_H

#include    <w32std.h>
#include    "aiwspluginanimdef.h"

class RAiWsPluginAnim : public RAnim
    {
    public: // constructor
    
        /**
        * c++ constructor
        * @param aDll
        */
        inline RAiWsPluginAnim( RAnimDll &aDll ) : RAnim( aDll ) {}
        
        
        /**
        * Constructor
        * @param aDevice a window device
        * @param aIdleWg window group id of idle application
        * @param aConsumerWg window group id of the application handling
        *        numeric keys on behalf of idle.
        */
        inline TInt Construct( 
        	const RWindowBase& aDevice, 
            TInt aIdleWg, 
            TInt aConsumerWg )
        	{
        	TPckgBuf<TAiWsPluginAnimInitData> init;
		    init().iAiWgId = aIdleWg;
		    init().iTargetWgId = aConsumerWg;
		 
		    return RAnim::Construct( aDevice, EAnimKeyForward, init );
        	}

    };

#endif // AIWSPLUGINANIM_H
