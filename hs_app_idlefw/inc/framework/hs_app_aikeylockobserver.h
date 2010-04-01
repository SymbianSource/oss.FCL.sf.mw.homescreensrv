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
* Description:  AI2 keylock status observer.
*
*/


#ifndef C_AIKEYLOCKOBSERVER_H
#define C_AIKEYLOCKOBSERVER_H

#include <e32base.h>
#include <e32property.h>
#include "hs_app_aipsstatusobserver.h"

class MAiPSPropertyObserver;

/**
 * @ingroup group_aifw
 * 
 *  Keylock status change observer.
 * 
 *  @lib aifw
 *  @since S60 3.2
 */
 
NONSHARABLE_CLASS( CAiKeylockObserver ) : public CAiPSStatusObserver
    {
public:

// Construction

    static CAiKeylockObserver* NewL( MAiStateManager* aStateManager );

    virtual ~CAiKeylockObserver();

// functions from base class CAiPSStatusObserver

    TAiStateChanges Status();

private:

// Construction
    
    CAiKeylockObserver();
    
    void ConstructL( MAiStateManager* aStateManager );
    
// new functions

    static TInt HandleKeylockStatusEvent( TAny* aPtr );

    };

#endif // C_AIKEYLOCKOBSERVER_H

// End of File.
