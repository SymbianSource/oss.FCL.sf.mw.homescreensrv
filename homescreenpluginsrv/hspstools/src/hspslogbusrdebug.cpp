/*
* Copyright (c) 2008 Nokia Corporation and/or its subsidiary(-ies).
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
* Description:  Implementation of  ChspsLogBusRDebug.
*
*/


#include "hspslogbusrdebug.h"
#include <e32debug.h>

//----------------------------------------------------------------------------
// ChspsLogBusRDebug::~ChspsLogBusRDebug
// ----------------------------------------------------------------------------
//
EXPORT_C ChspsLogBusRDebug::~ChspsLogBusRDebug()
    {
    }

//----------------------------------------------------------------------------
// ChspsLogBusRDebug::NewL
// ----------------------------------------------------------------------------
//
EXPORT_C ChspsLogBusRDebug* ChspsLogBusRDebug::NewL()
    {
    ChspsLogBusRDebug* self=ChspsLogBusRDebug::NewLC();
    CleanupStack::Pop(); // self;
    return self;
    }

//----------------------------------------------------------------------------
// ChspsLogBusRDebug::NewLC
// ----------------------------------------------------------------------------
//
EXPORT_C ChspsLogBusRDebug* ChspsLogBusRDebug::NewLC()
    {
    ChspsLogBusRDebug* self = new (ELeave)ChspsLogBusRDebug();
    CleanupStack::PushL(self);
    self->ConstructL();
    return self;
    }

//----------------------------------------------------------------------------
// ChspsLogBusRDebug::_LogText
// ----------------------------------------------------------------------------
//
#ifdef HSPS_BUILD_LOG_IMPLEMENTATION
void ChspsLogBusRDebug::_LogText( const TDesC& aMessage)
    {
    RDebug::Print( aMessage );
    }
#else
void ChspsLogBusRDebug::_LogText( const TDesC& /*aMessage*/)
    {
    }
#endif

//----------------------------------------------------------------------------
// ChspsLogBusRDebug::ChspsLogBusRDebug
// ----------------------------------------------------------------------------
//
ChspsLogBusRDebug::ChspsLogBusRDebug()
    {
    // No implementation required
    }

//----------------------------------------------------------------------------
// ChspsLogBusRDebug::ConstructL
// ----------------------------------------------------------------------------
//
void ChspsLogBusRDebug::ConstructL()
    {
    }
