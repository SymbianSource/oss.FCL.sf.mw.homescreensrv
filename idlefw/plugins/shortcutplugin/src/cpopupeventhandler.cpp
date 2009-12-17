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
* Description:  Shortcut plug-in Popup event handler class
*
*/


#include "cpopupeventhandler.h"
#include "PopupFSM.h"
#include "mpopupeventhandleractions.h"

/**
 * Timeout timer values
 */
const TInt KTimeoutShort = 900000;  //900 ms
const TInt KTimeoutLong = 6000000;  //6 sec


// ======== MEMBER FUNCTIONS ========

// ---------------------------------------------------------------------------
//
// ---------------------------------------------------------------------------
//
CPopupEventHandler* CPopupEventHandler::NewL(
    MPopupEventHandlerActions& aPopupEventHandlerActions )
    {
    CPopupEventHandler* self = CPopupEventHandler::NewLC( aPopupEventHandlerActions );
    CleanupStack::Pop( self );
    return self;
    }

// ---------------------------------------------------------------------------
//
// ---------------------------------------------------------------------------
//
CPopupEventHandler* CPopupEventHandler::NewLC(
    MPopupEventHandlerActions& aPopupEventHandlerActions )
    {
    CPopupEventHandler* self = new( ELeave ) CPopupEventHandler(
        aPopupEventHandlerActions );
    CleanupStack::PushL( self );
    self->ConstructL();
    return self;
    }

// ---------------------------------------------------------------------------
//
// ---------------------------------------------------------------------------
//
CPopupEventHandler::~CPopupEventHandler()
    {
    Cancel();
    iTimer.Close();
    }

// ---------------------------------------------------------------------------
//
// ---------------------------------------------------------------------------
//
CPopupEventHandler::CPopupEventHandler(
    MPopupEventHandlerActions& aPopupEventHandlerActions )
    : CActive( EPriorityStandard ),
    iPopupEventHandlerActions( aPopupEventHandlerActions ),
    iPopupFSM( *this ),
    iPublishPopup( ETrue ),
    iPublishCaption( ETrue ),
    iCaptionVisible( ETrue )
    {
    }

// ---------------------------------------------------------------------------
// Constructor
// ---------------------------------------------------------------------------
//
void CPopupEventHandler::ConstructL()
    {
    User::LeaveIfError( iTimer.CreateLocal() );
    CActiveScheduler::Add( this );
    }

// ---------------------------------------------------------------------------
//
// ---------------------------------------------------------------------------
//
TPopupFSM& CPopupEventHandler::PopupFSM()
    {
    return iPopupFSM;
    }

// ---------------------------------------------------------------------------
//
// ---------------------------------------------------------------------------
//
TBool CPopupEventHandler::PublishPopup() const
    {
    return iPublishPopup;
    }

TBool CPopupEventHandler::PopupVisible() const
    {
    return iPopupVisible;
    }

// ---------------------------------------------------------------------------
//
// ---------------------------------------------------------------------------
//
TBool CPopupEventHandler::PublishCaption() const
    {
    return iPublishCaption;
    }

TBool CPopupEventHandler::CaptionVisible() const
    {
    return iCaptionVisible;
    }

// ---------------------------------------------------------------------------
//
// ---------------------------------------------------------------------------
//
void CPopupEventHandler::DoCancel()
    {
    iTimer.Cancel();
    }

// ---------------------------------------------------------------------------
//
// ---------------------------------------------------------------------------
//
void CPopupEventHandler::RunL()
    {
    iPopupFSM.HandleRequestCompleted();
    }

// ---------------------------------------------------------------------------
//
// ---------------------------------------------------------------------------
//
void CPopupEventHandler::CompleteSelf()
    {
    TRequestStatus* status = &iStatus;
    User::RequestComplete( status, KErrNone );
    SetActive();
    }

// ---------------------------------------------------------------------------
//
// ---------------------------------------------------------------------------
//
void CPopupEventHandler::CancelRequest()
    {
    Cancel();
    }

// ---------------------------------------------------------------------------
//
// ---------------------------------------------------------------------------
//
void CPopupEventHandler::StartShortTimer()
    {
    Cancel();  // Cancel previous request by calling iTimer.Cancel()
    iTimer.After( iStatus, KTimeoutShort );
    SetActive();
    }

// ---------------------------------------------------------------------------
//
// ---------------------------------------------------------------------------
//
void CPopupEventHandler::StartLongTimer()
    {
    Cancel();  // Cancel previous request by calling iTimer.Cancel()
    iTimer.After( iStatus, KTimeoutLong );
    SetActive();
    }

// ---------------------------------------------------------------------------
//
// ---------------------------------------------------------------------------
//
void CPopupEventHandler::IssuePublishPopup()
    {
    iPublishPopup = ETrue;
    iPopupVisible = ETrue;
    iPopupEventHandlerActions.IssuePublishShortcut();
    iPublishPopup = EFalse;
    }

// ---------------------------------------------------------------------------
//
// ---------------------------------------------------------------------------
//
void CPopupEventHandler::IssueCleanPopup()
    {
    iPublishPopup = ETrue;
    iPopupVisible = EFalse;
    iPopupEventHandlerActions.IssuePublishShortcut();
    iPublishPopup = EFalse;
    }

// ---------------------------------------------------------------------------
//
// ---------------------------------------------------------------------------
//
void CPopupEventHandler::IssuePublishCaption()
    {
    iPublishCaption = ETrue;
    iCaptionVisible = ETrue;
    iPopupEventHandlerActions.IssuePublishShortcut();
    iPublishCaption = EFalse;
    }

// ---------------------------------------------------------------------------
//
// ---------------------------------------------------------------------------
//
void CPopupEventHandler::IssueCleanCaption()
    {
    iPublishCaption = ETrue;
    iCaptionVisible = EFalse;
    iPopupEventHandlerActions.IssuePublishShortcut();
    iPublishCaption = EFalse;
    }

// End of File.
