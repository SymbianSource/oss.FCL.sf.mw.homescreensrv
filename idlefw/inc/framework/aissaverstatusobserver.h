/*
* Copyright (c) 2005-2005 Nokia Corporation and/or its subsidiary(-ies).
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


#ifndef C_AISSAVERSTATUSOBSERVER_H
#define C_AISSAVERSTATUSOBSERVER_H

#include <e32base.h>
#include <e32property.h>
#include "aipsstatusobserver.h"

class MAiPSPropertyObserver;

/**
 *  Publish and subscribe key observer
 * 
 *  @lib aifw_hs
 *  @since S60 3.2
 */
 
NONSHARABLE_CLASS( CAiSSaverStatusObserver ) : public CAiPSStatusObserver
    {
public:

// Construction

    static CAiSSaverStatusObserver* NewL( MAiStateManager* aStateManager );

    virtual ~CAiSSaverStatusObserver();

// functions from base class CAiSSaverStatusObserver

    TAiStateChanges Status();

private:

// Construction
    
    CAiSSaverStatusObserver();
    
    void ConstructL( MAiStateManager* aStateManager );
    
// new functions

    static TInt HandleScreenSaverStateChanged( TAny* aPtr );

    };

#endif // C_AISSAVERSTATUSOBSERVER_H

// End of File.
