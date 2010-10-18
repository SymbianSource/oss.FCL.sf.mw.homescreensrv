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
 * Description :
 *
 */

#include <apgwgnam.h>
#include "tsentrykey.h"
#include "tsrunningappimp.h"
#include "tsresourcemanager.h"

//------------------------------------------------------------------------------
CTsRunningApp* CTsRunningApp::NewLC( 
                             MTsResourceManager& aResources,
                             const RWsSession::TWindowGroupChainInfo& aInfo )
    {
    CTsRunningApp* self = new(ELeave) CTsRunningApp(aResources, aInfo);
    CleanupStack::PushL( self );
    self->ConstructL();
    return self;
    }

//------------------------------------------------------------------------------
CTsRunningApp::~CTsRunningApp()
    {
    delete iWindowGroupName;
    }

//------------------------------------------------------------------------------
CTsRunningApp::CTsRunningApp( MTsResourceManager& aResources,
               const RWsSession::TWindowGroupChainInfo& aInfo )
:
iResources(aResources),
iWindowGroupInfo(aInfo)
    {
    //No implementation required
    }

void CTsRunningApp::ConstructL()
    {
    iWindowGroupName = CApaWindowGroupName::NewL( iResources.WsSession(), 
                                                  WindowGroupId() );
    iCaption.Set(iWindowGroupName->Caption());
    
    }

//------------------------------------------------------------------------------
void CTsRunningApp::RefreshDataL()
    {
    delete iWindowGroupName;
    iWindowGroupName =0;
    ConstructL();
    }

//------------------------------------------------------------------------------
TUid CTsRunningApp::Uid()const
    {
    return iWindowGroupName->AppUid();
    }

//------------------------------------------------------------------------------
const TDesC& CTsRunningApp::DisplayName() const
    {
    return iCaption;
    
    }

//------------------------------------------------------------------------------
TInt CTsRunningApp::WindowGroupId() const
    {
    return iWindowGroupInfo.iId;
    }

//------------------------------------------------------------------------------
TInt CTsRunningApp::ParentWindowGroupId() const
    {
    return iWindowGroupInfo.iParentId;
    }

//------------------------------------------------------------------------------
TBool CTsRunningApp::IsEmbeded() const
    {
    return 0 < iWindowGroupInfo.iParentId;
    }

//------------------------------------------------------------------------------
MTsRunningApplication::ApplicationHideMode CTsRunningApp::HideMode() const
    {
    return iWindowGroupName->Hidden() ? 
           MTsRunningApplication::System :
           iHideMode;
    }

//------------------------------------------------------------------------------
TBool CTsRunningApp::IsSystem() const
    {
    return  iWindowGroupName->IsSystem();
    }

//------------------------------------------------------------------------------
void CTsRunningApp::SetHidden( TBool aHidden )
    {
    iHideMode = aHidden ? MTsRunningApplication::Software :
                          MTsRunningApplication::None;
    }

//------------------------------------------------------------------------------
TInt CTsRunningApp::IconHandle() const
    {
    return KErrNotFound;
    }

//------------------------------------------------------------------------------
TTime CTsRunningApp::Timestamp() const
    {
    return iTimestap;
    }

//------------------------------------------------------------------------------
TTime CTsRunningApp::TimestampUpdate() const
    {
    return iUpdateTimestap;
    }

//------------------------------------------------------------------------------
TTsEntryKey CTsRunningApp::Key() const
    {
    return TTsEntryKey(WindowGroupId(), ParentWindowGroupId());
    }

//------------------------------------------------------------------------------
TBool CTsRunningApp::IsActive() const
    {
    return ETrue;
    }

//------------------------------------------------------------------------------
TBool CTsRunningApp::IsClosable() const
    {
    return !iWindowGroupName->IsSystem();
    }

//------------------------------------------------------------------------------
TBool CTsRunningApp::Close() const
    {
    return EFalse;
    }

//------------------------------------------------------------------------------
TBool CTsRunningApp::Launch() const
    {
    return EFalse;
    }

//------------------------------------------------------------------------------
TBool CTsRunningApp::IsMandatory() const
    {
    return ETrue;
    }
