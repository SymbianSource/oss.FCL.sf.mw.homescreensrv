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


#include <AiwServiceHandler.h>      // AIW
#include <aiwdialdata.h>    //include\oem
#include <LiwVariant.h>

#include "ahtelkeyhandler.h"
#include "ahproperties.hrh"

// ======== MEMBER FUNCTIONS ========

// ---------------------------------------------------------------------------
// 2nd phase constructor may leave
// ---------------------------------------------------------------------------
void CAHTelKeyHandler::ConstructL()
    {
    iServiceHandler = CAiwServiceHandler::NewL( );
    TAiwServiceCommands serviceCommand = KAiwCmdCall;

    //service command used as criteria item ID
    _LIT8( KStar, "*" );
    CAiwCriteriaItem* criteria = CAiwCriteriaItem::NewLC( serviceCommand,
        serviceCommand, KStar );

    RCriteriaArray interest;
    CleanupClosePushL( interest );
    // We are using a base service.
    TUid base( KNullUid);
    base.iUid = KAiwClassBase;
    criteria->SetServiceClass( base );
    User::LeaveIfError( interest.Append( criteria ) );
    iServiceHandler->AttachL( interest );
    CleanupStack::PopAndDestroy( &interest );
    CleanupStack::PopAndDestroy( criteria );
    }

// ---------------------------------------------------------------------------
// Two-phased constructor
// ---------------------------------------------------------------------------
CAHTelKeyHandler* CAHTelKeyHandler::NewL()
    {
    CAHTelKeyHandler* self = CAHTelKeyHandler::NewLC( );
    CleanupStack::Pop( self );
    return self;
    }

// ---------------------------------------------------------------------------
// Two-phased constructor
// ---------------------------------------------------------------------------
CAHTelKeyHandler* CAHTelKeyHandler::NewLC()
    {
    CAHTelKeyHandler* self = new( ELeave ) CAHTelKeyHandler;
    CleanupStack::PushL( self );
    self->ConstructL( );
    return self;
    }

// ---------------------------------------------------------------------------
// destructor
// ---------------------------------------------------------------------------
CAHTelKeyHandler::~CAHTelKeyHandler()
    {
    delete iServiceHandler;
    }

// ---------------------------------------------------------------------------
// Executes provided action
// ---------------------------------------------------------------------------
//
TInt CAHTelKeyHandler::ExecuteActionL( const CLiwMap* aMap )
    {
    TInt errCode(KErrArgument);
    TLiwVariant variantNumber;
    variantNumber.PushL( );
    if ( aMap->FindL( KNumber, variantNumber ) )
        {
        TPtrC number( KNullDesC );
        if ( variantNumber.Get( number ) )
            {
            CAiwDialData* dialData = CAiwDialData::NewLC( );
            dialData->SetCallType( CAiwDialData::EAIWVoice );
            dialData->SetPhoneNumberL( number );
            dialData->SetWindowGroup( AIWDialData::KAiwGoToIdle );
            // Attach the interest to the AIW framework.

            // Set parameters for the AIW call
            CAiwGenericParamList* paramList = CAiwGenericParamList::NewLC( );
            CAiwGenericParamList& paramListOut =
                    iServiceHandler->OutParamListL( );

            dialData->FillInParamListL( *paramList );
            iServiceHandler->ExecuteServiceCmdL( KAiwCmdCall, *paramList,
                paramListOut, 0, NULL );
            errCode = KErrNone;
            paramListOut.Reset( );
            CleanupStack::PopAndDestroy( paramList );
            CleanupStack::PopAndDestroy( dialData );
            }
        }
    CleanupStack::PopAndDestroy( &variantNumber );
    return errCode;
    }
//End of file
