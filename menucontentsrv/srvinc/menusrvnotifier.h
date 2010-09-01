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

#ifndef __MENUSRVNOTIFIER_H__
#define __MENUSRVNOTIFIER_H__

//  INCLUDES

#include "menusrvobject.h"

// CLASS DECLARATION

/**
* Server side notifier object.
*/
NONSHARABLE_CLASS( CMenuSrvNotifier ): public CMenuSrvObject
    {

public:     // new methods

    /**
    * Notification requested.
    * @param aFolder Request notification for this folder (0==any).
    * @param aEvents Request notification for these events.
    * @param aMessage Message requesting the notification.
    */
    void NotifyL( TInt aFolder, TInt aEvents, const RMessage2& aMessage );

    /**
    * Handle engine events. Completes the message, if appropriate.
    * @param aFolder Event occurred in this folder (0==unspecified, match all).
    * @param aEvents Engine events.
    */
    void HandleEvents( TInt aFolder, TInt aEvents );

private:    // data

    TInt iFolder; ///< Folder.
    TInt iEvents; ///< Event mask.

    };

#endif // __MENUSRVNOTIFIER_H__
            
// End of File
