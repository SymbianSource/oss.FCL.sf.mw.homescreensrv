/*
* Copyright (c) 2007 Nokia Corporation and/or its subsidiary(-ies).
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
* Description:  Exemplary Action Handler Plugin
*
*/




#include "cpstestplugin.h"


// ======== MEMBER FUNCTIONS ========

// ----------------------------------------------------------------------------
// Symbian 2nd phase constructor can leave.
// ----------------------------------------------------------------------------
//
void CCPSTestPlugin::ConstructL()
    {
    iInt = KErrNotFound;
    }

// ----------------------------------------------------------------------------
// Two-phased constructor.
// ----------------------------------------------------------------------------
//
CCPSTestPlugin* CCPSTestPlugin::NewL()
    {
    CCPSTestPlugin* self = CCPSTestPlugin::NewLC();
    CleanupStack::Pop( self );
    return self;
    }

// ----------------------------------------------------------------------------
// Two-phased constructor.
// ----------------------------------------------------------------------------
//
CCPSTestPlugin* CCPSTestPlugin::NewLC()
    {
    CCPSTestPlugin* self = new( ELeave ) CCPSTestPlugin;
    CleanupStack::PushL( self );
    self->ConstructL();
    return self;
    }


// ---------------------------------------------------------------------------
// destructor
// ---------------------------------------------------------------------------
CCPSTestPlugin::~CCPSTestPlugin()
	{        
	}

// ---------------------------------------------------------------------------
// Executes provided action
// ---------------------------------------------------------------------------
//
void CCPSTestPlugin::UpdateL()
    {
    iInt = KErrNone; 
    }


