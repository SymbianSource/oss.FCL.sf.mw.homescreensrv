/*
* Copyright (c) 2006 Nokia Corporation and/or its subsidiary(-ies).
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
* Description:  Window server plug-in manager.
*
*/


#ifndef C_AIWSPLUGINMANAGERIMPL_H
#define C_AIWSPLUGINMANAGERIMPL_H

#include "aiwspluginmanager.h"
#include "aiwspluginanimdef.h"
#include "aiwspluginanim.inl"

class CCoeEnv;
class RWsSession;
class MAiPSPropertyObserver;

/**
 * Attaches Active Idle as the default idle app for Phone application.
 */
NONSHARABLE_CLASS( CAiWsPluginManagerImpl ) : public CAiWsPluginManager
    {
public:
    static CAiWsPluginManagerImpl* NewL(CCoeEnv& aCoeEnv);
    ~CAiWsPluginManagerImpl();

private:    
// Construction
    CAiWsPluginManagerImpl(RWsSession& aWsSession);
    void ConstructL(CCoeEnv& aCoeEnv);
    
private:
    void TryLoadWsPluginL();
	static TInt HandlePhoneEvent( TAny* aPtr );

private: // data
    RAnimDll        iAnimDll;
    RWindow         iWnd;
    RAiWsPluginAnim iAnim;
    CCoeEnv* 				iCoeEnv;  					
    MAiPSPropertyObserver* 	iPhoneStatusObserver;
    };


#endif // C_AIWSPLUGINMANAGERIMPL_H

