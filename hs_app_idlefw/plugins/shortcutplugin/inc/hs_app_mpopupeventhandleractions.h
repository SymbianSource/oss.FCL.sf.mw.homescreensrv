/*
* Copyright (c) 2009 Nokia Corporation and/or its subsidiary(-ies).
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
* Description:
*
*/
#ifndef MPOPUPEVENTHANDLERACTIONS_H
#define MPOPUPEVENTHANDLERACTIONS_H

//  EXTERNAL INCLUDES
#include <e32def.h>

//  CLASS DEFINITION
/**
 * MPopupEventHandlerActions.
 */
class MPopupEventHandlerActions
    {
    public:     // Abstract methods
        /**
         * Publishes the shortcut
         */
        virtual void IssuePublishShortcut() = 0;
    };

#endif      //  MPOPUPEVENTHANDLERACTIONS_H

// End of file
