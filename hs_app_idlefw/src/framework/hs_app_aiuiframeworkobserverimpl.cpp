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
* Description:  UI framework observer for Active Idle framework
*
*/


#include <AknsConstants.h> // for KAknsMessageSkinChange equal general theme changed
#include "hs_app_aiuiframeworkobserverimpl.h"
#include "debug.h"

CAiUiFrameworkObserverImpl::CAiUiFrameworkObserverImpl( MAiStateManager& aManager )
  : iManager( aManager )
    {
    }

void CAiUiFrameworkObserverImpl::ConstructL()
    {
    }

CAiUiFrameworkObserverImpl* CAiUiFrameworkObserverImpl::NewL( MAiStateManager& aManager )
                                                              
    {
    CAiUiFrameworkObserverImpl* self = 
                        new(ELeave) CAiUiFrameworkObserverImpl( aManager );
    CleanupStack::PushL( self );
    self->ConstructL();
    CleanupStack::Pop( self );
    return self;
    }

CAiUiFrameworkObserverImpl::~CAiUiFrameworkObserverImpl()
    {
    }

// ---------------------------------------------------------------------------
// From class MAiUiFrameworkObserver.
// 
// ---------------------------------------------------------------------------
//
void CAiUiFrameworkObserverImpl::HandleResourceChange( TInt aType )
    {
    switch( aType )
        {
        case KAknsMessageSkinChange:
            {
            iManager.ReportStateChange( ESMAIGeneralThemeChanged );
            break;
            }
        default:
            {
            // Do nothing
            break;
            }
        }
    }

// ---------------------------------------------------------------------------
// From class MAiUiFrameworkObserver.
// 
// ---------------------------------------------------------------------------
//    
void CAiUiFrameworkObserverImpl::HandleForegroundEvent( TBool /*aForeground*/ )
    {
    // Focus observer disabled from this location
    }
    
// ---------------------------------------------------------------------------
// From class MAiDeviceStatusObserver.
// 
// ---------------------------------------------------------------------------
//
TAiStateChanges CAiUiFrameworkObserverImpl::Status()
    {
    // Resource change event are by nature single shot, so no status can 
    // be defined for them.
    return ESMAIUnknownState;
    }
