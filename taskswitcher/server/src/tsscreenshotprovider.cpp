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
* Description:
*
*/
#include <s32mem.h>

#include "tsscreenshotprovider.h"
#include "tstaskmonitorglobals.h"
#include "tsscreenshotmsg.h"


const TUid KPluginUID = {0x200267AE};
// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
//
CTsScreenshotProvider* CTsScreenshotProvider::NewL( MTsDataStorage& aStorage )
    {
    CTsScreenshotProvider *self = new (ELeave)CTsScreenshotProvider( aStorage );
    CleanupStack::PushL( self );
    self->BaseConstructL(KPluginUID, KNullDesC8);
    CleanupStack::Pop( self );
    return self;
    }

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
//
CTsScreenshotProvider::CTsScreenshotProvider( MTsDataStorage& aStorage )
:
    iStorage(aStorage)
    {
    //No implementation required
    }

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
//
void CTsScreenshotProvider::HandleMessage( const TDesC8& aMsg )
    {
    TRAP_IGNORE( HandleMessageL( aMsg ) );
    }

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
//
void CTsScreenshotProvider::HandleMessageL( const TDesC8& aMsg )
    {
    RDesReadStream msgStream( aMsg );
    CleanupClosePushL( msgStream );
    switch ( msgStream.ReadInt32L() ) 
        {
        case RegisterScreenshotMessage:
            {
            //parse msg to ACK provider ASAP and be sure that bitmap still exists
            CTsScreenshotMsg* screenshotMsg = CTsScreenshotMsg::NewLC(msgStream); 
            SendMessage(aMsg);
            
            //reset stream
            msgStream.Close();
            msgStream.Open(aMsg);
            
            //forward stream to storage
            iStorage.HandleDataL(msgStream.ReadInt32L(), msgStream);
            
            CleanupStack::PopAndDestroy(screenshotMsg);            
            break;
            }
            
        case WindowGroupToBackgroundMessage:
            {
            //reset stream
            msgStream.Close();
            msgStream.Open(aMsg);
            
            //forward stream to storage
            iStorage.HandleDataL(msgStream.ReadInt32L(), msgStream);
            break;
            }
            
        default: User::Leave(KErrNotSupported);        
        }

    CleanupStack::PopAndDestroy(&msgStream);
    }

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
//
void CTsScreenshotProvider::OnReplace()
    {
    //No implementation required
    }
