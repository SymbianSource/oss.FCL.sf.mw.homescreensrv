/*
* Copyright (c) 2005 Nokia Corporation and/or its subsidiary(-ies).
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
* Description:  Collection of content plugin helper functions
*
*/

// System includes

// User includes 
#include <hscontentpublisher.h>

#include "caiplugintool.h"

// ======== MEMBER FUNCTIONS ========

// ----------------------------------------------------------------------------
// CAiPluginTool::CAiPluginTool
//
// ----------------------------------------------------------------------------
//
CAiPluginTool::CAiPluginTool()
    {
    }

// ----------------------------------------------------------------------------
// CAiPluginTool::NewL
//
// ----------------------------------------------------------------------------
//
CAiPluginTool* CAiPluginTool::NewL()
    { 
    CAiPluginTool* self = new ( ELeave ) CAiPluginTool();
    CleanupStack::PushL( self );
    self->ConstructL();
    CleanupStack::Pop( self );
    return self;
    }

// ----------------------------------------------------------------------------
// CAiPluginTool::ConstructL
//
// ----------------------------------------------------------------------------
//
void CAiPluginTool::ConstructL()
    {
    }

// ----------------------------------------------------------------------------
// CAiPluginTool::Release
//
// ----------------------------------------------------------------------------
//
void CAiPluginTool::Release()
    {
    delete this;
    }

// ----------------------------------------------------------------------------
// CAiPluginTool::ContentItemIterator
//
// ----------------------------------------------------------------------------
//
MAiContentItemIterator* CAiPluginTool::ContentItemIterator(
    CHsContentPublisher& aContentPublisher,
    CHsContentPublisher::TProperty aType )
    {
    return static_cast< MAiContentItemIterator* >( 
        aContentPublisher.GetProperty( aType ) );
    }
    
 // End of File.

