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
* Description:  Observer interface for UI framework events
*
*/


#ifndef M_AIUIFRAMEWORKOBSERVER_H
#define M_AIUIFRAMEWORKOBSERVER_H

#include <e32def.h>

/**
 *  MAiUiFrameworkObserver observes UI framework events.
 *
 *  @since S60 v3.2
 */
class MAiUiFrameworkObserver
    {

public:

    /**
     * Event handler for resource change in UI framework.
     *
     * @param aType resource change type.
     */
    virtual void HandleResourceChange( TInt aType ) = 0;
    
    /**
     * Event handler for focus change event.
     *
     * @param aForeground ETrue if UI is foreground.
     */
    virtual void HandleForegroundEvent( TBool aForeground ) = 0;

protected:

    virtual ~MAiUiFrameworkObserver() {};

    };


#endif // M_AIUIFRAMEWORKOBSERVER_H
