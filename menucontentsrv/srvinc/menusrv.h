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

#ifndef __MENUSRV_H__
#define __MENUSRV_H__

//  INCLUDES

#include <e32base.h>
#include <eikenv.h>
#include <eikappui.h>
#include "menuengobserver.h"

// FORWARD DECLARATION

class CMenuSrvEng;
class CTimeout;

// GLOBAL FUNCTIONS

/**
* Run the Menu Server (entry function).
* @return Error code.
*/
IMPORT_C TInt RunMenuServer();

// CLASS DECLARATION

/**
* Menu Server.
*/
NONSHARABLE_CLASS( CMenuSrv ) : public CPolicyServer
    {
public:     // Constructor

    /**
    * Destructor.
    */
    virtual ~CMenuSrv();

    /**
    * Two-phased constructor. Leaves on failure.
    * @return The constructed server.
    */
    static CMenuSrv* NewL();

public:     // New methods

    /**
    * Creates the object container.
    * @param -
    * @return A pointer to the newly created CObjectCon.
    */
    CObjectCon* NewContainerL();

    /**
    * Removes the specified object container from this container index 
    * and deletes it.
    * @param aCon A pointer to the object container to be removed.
    * @return None.
    */
    void RemoveContainer( CObjectCon* aCon );

    /**
    * Get a new shared engine with this name. Owner is the caller.
    * The returned object should be Close()-d, not deleted!
    * @param aName Content file name.
    * @return Shared Engine.
    */
    CMenuSrvEng* GetEngineL( const TDesC& aName );

    /**
    * A shared engine is deleted.
    * When the last engine is deleted, we can exit.
    */
    void EngineDeleted();

private:    // Constructors and destructor

    /**
    * Constructor.
    */
    CMenuSrv();

    /**
    * Second-phase constructor.
    */
    void ConstructL();

private:    // From CServer2

    /**
    * Create new session.
    * @param aVersion Version.
    * @param aMessage Message (unused).
    * @return New session.
    */
    CSession2* NewSessionL
        ( const TVersion& aVersion, const RMessage2& aMessage ) const;
    
    /** 
    * Performs a custom security check.
    * @param aMsg The message to check.
    * @param aAction A reference to the action to take if the security
    * check fails.
    * @param aMissing A reference to the list of security attributes missing
    * from the checked process.
    * @return A value from TCustomResult.
    */
    TCustomResult CustomSecurityCheckL(
        const RMessage2& aMsg, 
        TInt& aAction,
        TSecurityInfo& aMissing );
    


private:    // Data

    CTimeout* iExitTimer; ///< Exit timer. Own.
    CObjectConIx* iObjectConIx; ///< Container index. Own.
    CObjectCon* iEngines; ///< Engines. Own.

};

#endif // __MENUSRV_H__

// End of File
