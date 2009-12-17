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
* Description:  
 *
*/


#include <CMessageData.h>
#include <sendui.h>
#include <SendUiConsts.h>
#include <liwvariant.h>

#include "ahmessagehandler.h"
#include "ahproperties.hrh"

// ======== MEMBER FUNCTIONS ========

// ----------------------------------------------------------------------------
// CAHMessageHandler::ConstructL
// Symbian 2nd phase constructor can leave.
// ----------------------------------------------------------------------------
//
void CAHMessageHandler::ConstructL()
    {
    //empty
    }

// ----------------------------------------------------------------------------
// CAHMessageHandler::NewL
// Two-phased constructor.
// ----------------------------------------------------------------------------
//
CAHMessageHandler* CAHMessageHandler::NewL()
    {
    CAHMessageHandler* self = CAHMessageHandler::NewLC( );
    CleanupStack::Pop( self );
    return self;
    }

// ----------------------------------------------------------------------------
// CAHMessageHandler::NewLC
// Two-phased constructor.
// ----------------------------------------------------------------------------
//
CAHMessageHandler* CAHMessageHandler::NewLC()
    {
    CAHMessageHandler* self = new( ELeave ) CAHMessageHandler;
    CleanupStack::PushL( self );
    self->ConstructL( );
    return self;
    }

// ---------------------------------------------------------------------------
// CAHMessageHandler::~CAHMessageHandler
// Destructor
// ---------------------------------------------------------------------------
CAHMessageHandler::~CAHMessageHandler()
    {

    }
// ---------------------------------------------------------------------------
// CAHMessageHandler::ExecuteActionL
// Executes provided action
// ---------------------------------------------------------------------------
//
TInt CAHMessageHandler::ExecuteActionL( const CLiwMap* aMap )
    {
    TInt errCode(KErrArgument);
    TLiwVariant variantAddress;
    variantAddress.PushL( );
    TLiwVariant variantType;
    variantType.PushL( );
    if ( aMap->FindL( KType, variantType ) 
        && aMap->FindL( KAddress, variantAddress ) )
        {
        TPtrC request( KNullDesC );
        TPtrC address( KNullDesC );
        if ( variantType.Get( request ) && variantAddress.Get( address ) )
            {
            TPtrC alias( KNullDesC );
            TLiwVariant variantAlias;
            variantAlias.PushL( );
            if ( aMap->FindL( KAlias, variantAlias ) )
                {
                variantAlias.Get( alias );
                }
            CleanupStack::PopAndDestroy( &variantAlias );
            CMessageData* message = CMessageData::NewLC( );
            message->AppendToAddressL( address, alias );
            CSendUi* sendUi = CSendUi::NewLC( );
            if ( !request.Compare( KActionValueSendSMS ) )
                {
                sendUi->CreateAndSendMessageL( KSenduiMtmSmsUid, message,
                    TUid::Null( ), EFalse );
                }
            else if ( !request.Compare( KActionValueSendMMS ) )
                {
                sendUi->CreateAndSendMessageL( KSenduiMtmMmsUid, message,
                    TUid::Null( ), EFalse );
                }
            else if ( !request.Compare( KActionValueSendEmail ) )
                {
                sendUi->CreateAndSendMessageL( KSenduiMtmSmtpUid, message,
                    TUid::Null( ), EFalse );
                }
            // clean up
            CleanupStack::PopAndDestroy( sendUi );
            CleanupStack::PopAndDestroy( message );
            errCode= KErrNone;
            }
        }
    CleanupStack::PopAndDestroy( &variantType );
    CleanupStack::PopAndDestroy( &variantAddress );
    return errCode;
    }

// End of file
