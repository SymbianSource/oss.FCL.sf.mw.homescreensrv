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
* Description: carunningapphandler.cpp
*
*/
#include "carunningapphandler.h"

#include "cadef.h"
#include "carunningappmonitor.h"

// -----------------------------------------------------------------------------
// CCaRunningAppHandler
// -----------------------------------------------------------------------------
//
CCaRunningAppHandler* CCaRunningAppHandler::NewL( TPluginParams* aParams )
    {
    if( aParams == 0 || aParams->storageProxy == 0 )
        {
        User::Leave(KErrCorrupt);
        }
    CCaRunningAppHandler* self = new (ELeave)CCaRunningAppHandler();
    CleanupStack::PushL( self );
    self->ConstructL( *aParams->storageProxy );
    CleanupStack::Pop( self );
    return self;
    }

// -----------------------------------------------------------------------------
// CCaRunningAppHandler
// -----------------------------------------------------------------------------
//
CCaRunningAppHandler::~CCaRunningAppHandler()
    {
    iRunningTasks.ResetAndDestroy();
    }

// -----------------------------------------------------------------------------
// CCaRunningAppHandler
// -----------------------------------------------------------------------------
//
CCaRunningAppHandler::CCaRunningAppHandler()
:
iRunningTasks( KDefaultGranularity )
    {
    //no implementation required
    }

// -----------------------------------------------------------------------------
// ConstructL
// -----------------------------------------------------------------------------
//
void CCaRunningAppHandler::ConstructL( CCaStorageProxy& aStorage )
    {
    CCaRunningAppMonitor::ExecuteL( *this, aStorage );
    }

// -----------------------------------------------------------------------------
// RegisterTaskL
// -----------------------------------------------------------------------------
//
void CCaRunningAppHandler::RegisterTaskL( CBase* aTask )
    {
    iRunningTasks.AppendL( aTask );
    }

// -----------------------------------------------------------------------------
// UnregisterTask
// -----------------------------------------------------------------------------
//
void CCaRunningAppHandler::UnregisterTask( CBase* aTask )
    {
    const TInt index( iRunningTasks.FindReverse( aTask ) );
    if( index != KErrNotFound )
        {
        iRunningTasks.Remove( index );
        }
    }
