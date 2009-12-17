/*
* Copyright (c) 2008-2009 Nokia Corporation and/or its subsidiary(-ies).
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
* Description:  Encapsulates name and value
*
*/
#ifndef MHOMESCREENSETTINGSOBSERVER_H_
#define MHOMESCREENSETTINGSOBSERVER_H_

#include <e32base.h>

namespace HSPluginSettingsIf{

/**
 *  Observer class for HomeScreenSettingsIf
 *
 *  @code
 *   
 *  @endcode
 *
 *  @lib HomeScreenSettingsIf.lib
 *  @since S60 v5.0
 */
class MHomeScreenSettingsObserver
    {
public:
    /**
     * Handle HSPS wrappper notifications 
     *
     * @param aEvent Event
     * @param aPluginName Plugin name
     * @param aPluginUid Plugin uid
     * @param aPluginId Plugin id
     */
    virtual TInt SettingsChangedL(
        const TDesC8& aEvent,
        const TDesC8& aPluginName,
        const TDesC8& aPluginUid,
        const TDesC8& aPluginId ) = 0;                                  

    };
}
#endif /*MHOMESCREENSETTINGSOBSERVER_H_*/
