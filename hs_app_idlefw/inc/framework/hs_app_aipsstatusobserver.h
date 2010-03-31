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
* Description:  PS status observer
*
*/


#ifndef C_AIPSSTATUSOBSERVER_H
#define C_AIPSSTATUSOBSERVER_H

#include <e32base.h>
#include <e32property.h>
#include "hs_app_aidevicestatusobserver.h"

class MAiPSPropertyObserver;
class MAiStateManager;

/**
 *  description
 * 
 *  @lib aifw
 *  @since S60 3.2
 */
 
NONSHARABLE_CLASS( CAiPSStatusObserver ) : public CBase,
                                           public MAiDeviceStatusObserver
    {

public:

// Construction

    virtual ~CAiPSStatusObserver();

// New functions

    virtual TAiStateChanges Status();

protected:

// Construction
    
    CAiPSStatusObserver();
    
    void BaseConstructL( TCallBack aCallBack,
                         TUid aCategory,
                         TInt aKey, 
                         MAiStateManager* aStateManager );
    
protected: // Data

    MAiPSPropertyObserver* iObserver;
    
    MAiStateManager* iStateManager;
    
    };

#endif // C_AIPSSTATUSOBSERVER_H

// End of File.
