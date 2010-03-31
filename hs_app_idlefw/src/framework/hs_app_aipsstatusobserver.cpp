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
* Description:  Generic PS observer base class
*
*/


#include <aiutility.h>
#include <aipspropertyobserver.h>
#include "hs_app_aipsstatusobserver.h"

CAiPSStatusObserver::~CAiPSStatusObserver()
    {
    if( iObserver )
        iObserver->Release();
    }

TAiStateChanges CAiPSStatusObserver::Status()
    {
    return ESMAIUnknownState;
    }

CAiPSStatusObserver::CAiPSStatusObserver()
    {
    }
    
void CAiPSStatusObserver::BaseConstructL( TCallBack aCallBack,
                                              TUid aCategory,
                                              TInt aKey,
                                              MAiStateManager* aStateManager )
    {
    iStateManager = aStateManager;
    iObserver = AiUtility::CreatePSPropertyObserverL( aCallBack, aCategory, aKey );
    }

// End of File.
