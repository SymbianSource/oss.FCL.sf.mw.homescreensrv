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
#include "hs_app_PopupFSM.h"
#include "hs_app_MPopupFSMActions.h"
#include "hs_app_PopupTrace.h"

#pragma warning( disable:4100 )
#pragma warning( disable:4068 )
#pragma warn_unusedarg off

TPopupFSM::TPopupFSM( MPopupFSMActions& aPopupFSMActions ) : 
    iCurrentState( &iNotSetOffFocus ),
    iPopupFSMActions( aPopupFSMActions )
    {
    }

void TPopupFSM::TPopupStateBase::HandleForeground( TPopupFSM* /*aPopupFSM*/, MPopupFSMActions& /*aPopupFSMActions*/ )
    {
    TRACE( _L("TPopupStateBase::HandleForeground()") );
    }

void TPopupFSM::TPopupStateBase::HandleUpdate( TPopupFSM* /*aPopupFSM*/, MPopupFSMActions& /*aPopupFSMActions*/ )
    {
    TRACE( _L("TPopupStateBase::HandleUpdate()") );
    }

void TPopupFSM::TPopupStateBase::HandleRequestCompleted( TPopupFSM* /*aPopupFSM*/, MPopupFSMActions& /*aPopupFSMActions*/ )
    {
    TRACE( _L("TPopupStateBase::HandleRequestCompleted()") );
    }

void TPopupFSM::TPopupStateBase::HandleReset( TPopupFSM* /*aPopupFSM*/, MPopupFSMActions& /*aPopupFSMActions*/ )
    {
    TRACE( _L("TPopupStateBase::HandleReset()") );
    }

void TPopupFSM::TPopupStateBase::HandleBackground( TPopupFSM* /*aPopupFSM*/, MPopupFSMActions& /*aPopupFSMActions*/ )
    {
    TRACE( _L("TPopupStateBase::HandleBackground()") );
    }

void TPopupFSM::TPopupStateBase::HandleGotFocus( TPopupFSM* /*aPopupFSM*/, MPopupFSMActions& /*aPopupFSMActions*/ )
    {
    TRACE( _L("TPopupStateBase::HandleGotFocus()") );
    }

void TPopupFSM::TPopupStateBase::HandleLostFocus( TPopupFSM* /*aPopupFSM*/, MPopupFSMActions& /*aPopupFSMActions*/ )
    {
    TRACE( _L("TPopupStateBase::HandleLostFocus()") );
    }

void TPopupFSM::HandleForeground()
    {
    iCurrentState->HandleForeground( this, iPopupFSMActions );
    }

void TPopupFSM::HandleUpdate()
    {
    iCurrentState->HandleUpdate( this, iPopupFSMActions );
    }

void TPopupFSM::HandleRequestCompleted()
    {
    iCurrentState->HandleRequestCompleted( this, iPopupFSMActions );
    }

void TPopupFSM::HandleReset()
    {
    iCurrentState->HandleReset( this, iPopupFSMActions );
    }

void TPopupFSM::HandleBackground()
    {
    iCurrentState->HandleBackground( this, iPopupFSMActions );
    }

void TPopupFSM::HandleGotFocus()
    {
    iCurrentState->HandleGotFocus( this, iPopupFSMActions );
    }

void TPopupFSM::HandleLostFocus()
    {
    iCurrentState->HandleLostFocus( this, iPopupFSMActions );
    }

void TPopupFSM::SetState( TPopupStateBase* aNewState )
    {
    iCurrentState = aNewState;
    }

void TPopupFSM::TNotSetOffFocus::HandleUpdate( TPopupFSM* aPopupFSM, MPopupFSMActions& aPopupFSMActions )
    {
    TRACE( _L("TNotSetOffFocus::HandleUpdate()") );
    aPopupFSM->SetState( &aPopupFSM->iSetOffFocus );
    aPopupFSMActions.IssueCleanCaption();
    }

void TPopupFSM::TNotSetOffFocus::HandleGotFocus( TPopupFSM* aPopupFSM, MPopupFSMActions& /*aPopupFSMActions*/ )
    {
    TRACE( _L("TNotSetOffFocus::HandleGotFocus()") );
    aPopupFSM->SetState( &aPopupFSM->iNotSetOnFocus );
    }

void TPopupFSM::TNotSetOffFocus::HandleForeground( TPopupFSM* aPopupFSM, MPopupFSMActions& aPopupFSMActions )
    {
    TRACE( _L("TNotSetOffFocus::HandleForeground()") );
    aPopupFSM->SetState( &aPopupFSM->iNotSetOffFocus );
    aPopupFSMActions.IssuePublishCaption();
    }

void TPopupFSM::TSetOffFocus::HandleGotFocus( TPopupFSM* aPopupFSM, MPopupFSMActions& aPopupFSMActions )
    {
    TRACE( _L("TSetOffFocus::HandleGotFocus()") );
    aPopupFSM->SetState( &aPopupFSM->iGettingFocus );
    aPopupFSMActions.StartShortTimer();
    }

void TPopupFSM::TSetOffFocus::HandleReset( TPopupFSM* aPopupFSM, MPopupFSMActions& aPopupFSMActions )
    {
    TRACE( _L("TSetOffFocus::HandleReset()") );
    aPopupFSM->SetState( &aPopupFSM->iNotSetOffFocus );
    aPopupFSMActions.IssuePublishCaption();
    }

void TPopupFSM::TGettingFocus::HandleRequestCompleted( TPopupFSM* aPopupFSM, MPopupFSMActions& aPopupFSMActions )
    {
    TRACE( _L("TGettingFocus::HandleRequestCompleted()") );
    aPopupFSM->SetState( &aPopupFSM->iVisible );
    aPopupFSMActions.IssuePublishPopup();
    aPopupFSMActions.StartLongTimer();
    }

void TPopupFSM::TGettingFocus::HandleReset( TPopupFSM* aPopupFSM, MPopupFSMActions& aPopupFSMActions )
    {
    TRACE( _L("TGettingFocus::HandleReset()") );
    aPopupFSM->SetState( &aPopupFSM->iNotSetOnFocus );
    aPopupFSMActions.IssuePublishCaption();
    aPopupFSMActions.CancelRequest();
    }

void TPopupFSM::TGettingFocus::HandleLostFocus( TPopupFSM* aPopupFSM, MPopupFSMActions& aPopupFSMActions )
    {
    TRACE( _L("TGettingFocus::HandleLostFocus()") );
    aPopupFSM->SetState( &aPopupFSM->iSetOffFocus );
    aPopupFSMActions.CancelRequest();
    }

void TPopupFSM::TLosingFocus::HandleRequestCompleted( TPopupFSM* aPopupFSM, MPopupFSMActions& aPopupFSMActions )
    {
    TRACE( _L("TLosingFocus::HandleRequestCompleted()") );
    aPopupFSM->SetState( &aPopupFSM->iSetOffFocus );
    aPopupFSMActions.IssueCleanPopup();
    }

void TPopupFSM::TLosingFocus::HandleReset( TPopupFSM* aPopupFSM, MPopupFSMActions& aPopupFSMActions )
    {
    TRACE( _L("TLosingFocus::HandleReset()") );
    aPopupFSM->SetState( &aPopupFSM->iNotSetOffFocus );
    aPopupFSMActions.IssuePublishCaption();
    aPopupFSMActions.CancelRequest();
    }

void TPopupFSM::TLosingFocus::HandleGotFocus( TPopupFSM* aPopupFSM, MPopupFSMActions& aPopupFSMActions )
    {
    TRACE( _L("TLosingFocus::HandleGotFocus()") );
    aPopupFSM->SetState( &aPopupFSM->iVisible );
    aPopupFSMActions.CancelRequest();
    aPopupFSMActions.StartLongTimer();
    }

void TPopupFSM::TNotSetOnFocus::HandleUpdate( TPopupFSM* aPopupFSM, MPopupFSMActions& aPopupFSMActions )
    {
    TRACE( _L("TNotSetOnFocus::HandleUpdate()") );
    aPopupFSM->SetState( &aPopupFSM->iVisible );
    aPopupFSMActions.IssueCleanCaption();
    aPopupFSMActions.IssuePublishPopup();
    aPopupFSMActions.StartLongTimer();
    }

void TPopupFSM::TNotSetOnFocus::HandleLostFocus( TPopupFSM* aPopupFSM, MPopupFSMActions& /*aPopupFSMActions*/ )
    {
    TRACE( _L("TNotSetOnFocus::HandleLostFocus()") );
    aPopupFSM->SetState( &aPopupFSM->iNotSetOffFocus );
    }

void TPopupFSM::TNotSetOnFocus::HandleBackground( TPopupFSM* aPopupFSM, MPopupFSMActions& /*aPopupFSMActions*/ )
    {
    TRACE( _L("TNotSetOnFocus::HandleBackground()") );
    aPopupFSM->SetState( &aPopupFSM->iBackgroundNotSetOnFocus );
    }

void TPopupFSM::TNotSetOnFocus::HandleForeground( TPopupFSM* aPopupFSM, MPopupFSMActions& aPopupFSMActions )
    {
    TRACE( _L("TNotSetOnFocus::HandleForeground()") );
    aPopupFSM->SetState( &aPopupFSM->iNotSetOnFocus );
    aPopupFSMActions.IssuePublishCaption();
    }

void TPopupFSM::TVisible::HandleUpdate( TPopupFSM* aPopupFSM, MPopupFSMActions& aPopupFSMActions )
    {
    TRACE( _L("TVisible::HandleUpdate()") );
    aPopupFSM->SetState( &aPopupFSM->iVisible );
    aPopupFSMActions.IssueCleanCaption();
    aPopupFSMActions.IssuePublishPopup();
    aPopupFSMActions.StartLongTimer();
    }

void TPopupFSM::TVisible::HandleReset( TPopupFSM* aPopupFSM, MPopupFSMActions& aPopupFSMActions )
    {
    TRACE( _L("TVisible::HandleReset()") );
    aPopupFSM->SetState( &aPopupFSM->iNotSetOnFocus );
    aPopupFSMActions.IssuePublishCaption();
    aPopupFSMActions.IssueCleanPopup();
    aPopupFSMActions.CancelRequest();
    }

void TPopupFSM::TVisible::HandleLostFocus( TPopupFSM* aPopupFSM, MPopupFSMActions& aPopupFSMActions )
    {
    TRACE( _L("TVisible::HandleLostFocus()") );
    aPopupFSM->SetState( &aPopupFSM->iLosingFocus );
    aPopupFSMActions.CancelRequest();
    aPopupFSMActions.CompleteSelf();
    }

void TPopupFSM::TVisible::HandleRequestCompleted( TPopupFSM* aPopupFSM, MPopupFSMActions& aPopupFSMActions )
    {
    TRACE( _L("TVisible::HandleRequestCompleted()") );
    aPopupFSM->SetState( &aPopupFSM->iNotVisible );
    aPopupFSMActions.IssueCleanPopup();
    }

void TPopupFSM::TVisible::HandleBackground( TPopupFSM* aPopupFSM, MPopupFSMActions& aPopupFSMActions )
    {
    TRACE( _L("TVisible::HandleBackground()") );
    aPopupFSM->SetState( &aPopupFSM->iBackgroundSetOnFocus );
    aPopupFSMActions.CancelRequest();
    aPopupFSMActions.IssueCleanPopup();
    }

void TPopupFSM::TNotVisible::HandleUpdate( TPopupFSM* aPopupFSM, MPopupFSMActions& aPopupFSMActions )
    {
    TRACE( _L("TNotVisible::HandleUpdate()") );
    aPopupFSM->SetState( &aPopupFSM->iVisible );
    aPopupFSMActions.IssueCleanCaption();
    aPopupFSMActions.IssuePublishPopup();
    aPopupFSMActions.StartLongTimer();
    }

void TPopupFSM::TNotVisible::HandleReset( TPopupFSM* aPopupFSM, MPopupFSMActions& aPopupFSMActions )
    {
    TRACE( _L("TNotVisible::HandleReset()") );
    aPopupFSM->SetState( &aPopupFSM->iNotSetOnFocus );
    aPopupFSMActions.IssuePublishCaption();
    }

void TPopupFSM::TNotVisible::HandleLostFocus( TPopupFSM* aPopupFSM, MPopupFSMActions& /*aPopupFSMActions*/ )
    {
    TRACE( _L("TNotVisible::HandleLostFocus()") );
    aPopupFSM->SetState( &aPopupFSM->iSetOffFocus );
    }

void TPopupFSM::TBackgroundNotSetOnFocus::HandleForeground( TPopupFSM* aPopupFSM, MPopupFSMActions& aPopupFSMActions )
    {
    TRACE( _L("TBackgroundNotSetOnFocus::HandleForeground()") );
    aPopupFSM->SetState( &aPopupFSM->iNotSetOnFocus );
    aPopupFSMActions.IssuePublishCaption();
    }

void TPopupFSM::TBackgroundNotSetOnFocus::HandleUpdate( TPopupFSM* aPopupFSM, MPopupFSMActions& aPopupFSMActions )
    {
    TRACE( _L("TBackgroundNotSetOnFocus::HandleUpdate()") );
    aPopupFSM->SetState( &aPopupFSM->iBackgroundSetOnFocus );
    aPopupFSMActions.IssueCleanCaption();
    }

void TPopupFSM::TBackgroundSetOnFocus::HandleReset( TPopupFSM* aPopupFSM, MPopupFSMActions& aPopupFSMActions )
    {
    TRACE( _L("TBackgroundSetOnFocus::HandleReset()") );
    aPopupFSM->SetState( &aPopupFSM->iBackgroundNotSetOnFocus );
    aPopupFSMActions.IssuePublishCaption();
    }

void TPopupFSM::TBackgroundSetOnFocus::HandleForeground( TPopupFSM* aPopupFSM, MPopupFSMActions& aPopupFSMActions )
    {
    TRACE( _L("TBackgroundSetOnFocus::HandleForeground()") );
    aPopupFSM->SetState( &aPopupFSM->iVisible );
    aPopupFSMActions.IssuePublishPopup();
    aPopupFSMActions.StartLongTimer();
    }

// End of file
