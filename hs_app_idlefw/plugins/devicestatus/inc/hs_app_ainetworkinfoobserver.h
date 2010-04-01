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
* Description:  Network info observer.
*
*/


#ifndef M_AINETWORKINFOOBSERVER_H
#define M_AINETWORKINFOOBSERVER_H

#include <e32std.h>
#include <NWHandlingEngine.h>

/**
 *  @ingroup group_devicestatusplugin
 *
 *  Network info observer.
 *
 *  Client gets network info changes through this interface.
 *
 *  @since S60 3.2
 */
class MAiNetworkInfoObserver
    {

protected:

    /**
     * Virtual destructor.
     * Cannot be used to destruct the object.
     */
    virtual ~MAiNetworkInfoObserver() {};

public:

    /**
     * Called when network info changes.
     *
     * @since S60 3.2
     * @param aMessage is type of the change.
     * @param aInfo is new network info structure.
     */
    virtual void HandleNetworkInfoChange( const MNWMessageObserver::TNWMessages& aMessage, 
    							  		  const TNWInfo& aInfo, 
    							  		  const TBool aShowOpInd ) = 0;
    };


#endif // M_AINETWORKINFOOBSERVER_H
