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
* Description:  Network info observer
*
*/

#ifndef M_NETWORKOBSERVER_H
#define M_NETWORKOBSERVER_H

#include <e32std.h>
#include <NWHandlingEngine.h>

/**
 *  @ingroup group_xmluicontroller
 *
 *  Network info observer.
 *
 *  Client gets network state changes through this interface.
 *
 *  @since S60 5.0
 */
class MAiNetworkObserver
        {
    
public:
    enum TNetworkState
        {
        ENone = 0,
        EHomeNetwork,
        ERoaming
        };

protected:

    /**
     * Virtual destructor.
     * Cannot be used to destruct the object.
     */
    virtual ~MAiNetworkObserver() {};

public:

    /**
     * Called when network info changes.
     *
     * @since S60 5.0
     * @param aNewState new network state
      */
    virtual void HandleNetworkStateChange( TNetworkState aNewState ) = 0;
    
    };

#endif // M_AINETWORKINFOOBSERVER_H
