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
* Description:  Active Idle Framework internal state handling interface.
*
*/


#ifndef M_AIFWSTATEHANDLER_H
#define M_AIFWSTATEHANDLER_H

// System includes

// User includes
#include <aifwdefs.h>

// Forward declarations
class THsPublisherInfo;

/**
 * Active Idle Framework internal state handling interface. 
 * Events from UI controllers are forwarded to this interface for processing 
 * on the Active Idle Framework side.
 *
 * @since S60 5.2
 */
class MAiFwStateHandler
    {
public:
    // new functions
    
    /** 
     * Instructs to load plugin
     * 
     * @since S60 5.2
     * @param aPublisherInfo plugin to load
     * @param aReason startup reason 
     * @return Error code, KErrNone if loaded succesfully.
     */    
    virtual TInt LoadPlugin( const THsPublisherInfo& aPublisherInfo,
        TAiFwLoadReason aReason ) = 0;
    
    /** 
     * Instructs to destroy plugin
     * 
     * @since S60 5.2
     * @param aPublisherInfo plugin to destroy
     * @param aReason shutdown reason
     */
    virtual void DestroyPlugin( const THsPublisherInfo& aPublisherInfo,
        TAiFwDestroyReason aReason ) = 0;
    
    /**
     * Instructs to change all plugins' state
     * 
     * @since S60 5.2
     * @param aState State to change
     */
    virtual void ChangePluginState( TAiFwState aState ) = 0;

    };

#endif // M_AIFWSTATEHANDLER_H

// End of file
