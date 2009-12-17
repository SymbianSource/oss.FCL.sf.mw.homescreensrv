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
* Description:  Environment observer
*
*/


#ifndef C_AIENVIROMENTCHANGEOBSERVER_H
#define C_AIENVIROMENTCHANGEOBSERVER_H

#include <e32base.h>
#include "aidevicestatusobserver.h"
#include "aiuiframeworkobserver.h"

class MAiStateManager;
class CEnvironmentChangeNotifier;

/**
 * @ingroup group_aifw
 * 
 *  System enviroment change observer.
 * 
 *  @lib aifw
 *  @since S60 3.2
 */
NONSHARABLE_CLASS( CAiEnvironmentChangeObserver ) : public CBase,
                                                   public MAiDeviceStatusObserver
    {
public:

// Construction

    static CAiEnvironmentChangeObserver* NewL( MAiStateManager* aStateManager );

    virtual ~CAiEnvironmentChangeObserver();

// functions from base class CAiEnvironmentChangeObserver

    TAiStateChanges Status();

// new functions

    static TInt EnvironmentChangeCallBack(TAny* aPtr);
    
private:

// Construction
    
    CAiEnvironmentChangeObserver();
    
    void ConstructL( MAiStateManager* aStateManager );

protected: // Data

    /**
     * State manager.
     * Not own.
     */
    MAiStateManager*   iStateManager;

    /**
     * Enviroment change notifier: time, locale and midnight crossover
     * Own.
     */
    CEnvironmentChangeNotifier*                 iEnvironmentChangeNotifier;
    };

#endif // C_AIENVIROMENTCHANGEOBSERVER_H

