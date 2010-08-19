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
* Description: EUnit test for homescreen settings API, main suite
*
*/

// External includes
#include <e32err.h>

// Internal includes
#include "CHomescreenSettingsObserver.h"

// ======== MEMBER FUNCTIONS ========

//----------------------------------------------------------------------------
// CHomescreenSettingsObserver::NewL( )
//----------------------------------------------------------------------------
//
CHomescreenSettingsObserver* CHomescreenSettingsObserver::NewL()
    {
    CHomescreenSettingsObserver* self = CHomescreenSettingsObserver::NewLC();
    CleanupStack::Pop( self );
    return self;
    }

//----------------------------------------------------------------------------
// CHomescreenSettingsObserver::NewLC( )
//----------------------------------------------------------------------------
//
CHomescreenSettingsObserver* CHomescreenSettingsObserver::NewLC()
    {
    CHomescreenSettingsObserver* self = 
        new( ELeave ) CHomescreenSettingsObserver();
    CleanupStack::PushL( self );
    self->ConstructL();
    return self;
    }

//----------------------------------------------------------------------------
// CHomescreenSettingsObserver::~CHomescreenSettingsObserver( )
//----------------------------------------------------------------------------
//
CHomescreenSettingsObserver::~CHomescreenSettingsObserver()
    {
    }

//----------------------------------------------------------------------------
// CHomescreenSettingsObserver::CHomescreenSettingsObserver( )
//----------------------------------------------------------------------------
//
CHomescreenSettingsObserver::CHomescreenSettingsObserver()
    {
    }

//----------------------------------------------------------------------------
// CHomescreenSettingsObserver::ConstructL( )
//----------------------------------------------------------------------------
//
void CHomescreenSettingsObserver::ConstructL()
    {
    }

//----------------------------------------------------------------------------
// CHomescreenSettingsObserver::SettingsChangedL( )
//----------------------------------------------------------------------------
//
void CHomescreenSettingsObserver::SettingsChangedL( const TDesC8& /*aEvent*/,
                                         const TDesC8& /*aPluginName*/,
                                         const TDesC8& /*aPluginUid*/,
                                         const TDesC8& /*aPluginId*/ )
    {
    }
