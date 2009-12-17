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
* Description:  AI2 focus observer.
*
*/


#ifndef C_AIFOCUSOBSERVER_H
#define C_AIFOCUSOBSERVER_H

#include <e32base.h>
#include <e32property.h>
//++HV 
#include <akntransitionutils.h>
//--HV 
#include "aipsstatusobserver.h"

class MAiPSPropertyObserver;

/**
 * @ingroup group_aifw
 * 
 *  Focus change observer. Using telephony idle visiblity PS.
 * 
 *  @lib aifw
 *  @since S60 3.2
 */
 
NONSHARABLE_CLASS( CAiFocusObserver ) : public CAiPSStatusObserver
										//++HV    
    									,public MAknTransitionUtilsObserver
										//--HV    
    {
public:

// Construction

    static CAiFocusObserver* NewL( MAiStateManager* aStateManager );

    virtual ~CAiFocusObserver();

// functions from base class CAiPSStatusObserver

    TAiStateChanges Status();

private:

// Construction
    
    CAiFocusObserver();
    
    void ConstructL( MAiStateManager* aStateManager );
    
// new functions

    //++HV
    
    static TInt StaticHandleFocusChangeEvent( TAny* aPtr );
    TInt HandleFocusChangeEvent();

    
	// From MAknTransitionUtilsObserver
	TInt AknTransitionCallback(TInt aEvent, TInt aState = 0, const TDesC8* aParams = NULL);
		
        
private:
	TBool iTfxEffectActive;


	//--HV 
    };

#endif // C_AIFOCUSOBSERVER_H

// End of File.
