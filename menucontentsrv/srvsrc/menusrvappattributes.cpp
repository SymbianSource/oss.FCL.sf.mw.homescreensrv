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

#include <apgcli.h>
#include "menusrvappattributes.h"

// -----------------------------------------------------------------------------
// Destructor
// -----------------------------------------------------------------------------
CMenuSrvAppAttributes* CMenuSrvAppAttributes::NewL(
        TApaAppCapabilityBuf& aCapability, TApaAppInfo& aAppInfo )
    {
    CMenuSrvAppAttributes* self = CMenuSrvAppAttributes::NewLC(
            aCapability, aAppInfo);
    CleanupStack::Pop( self );
    return self;
    }

// -----------------------------------------------------------------------------
// Destructor
// -----------------------------------------------------------------------------
CMenuSrvAppAttributes* CMenuSrvAppAttributes::NewLC(
        TApaAppCapabilityBuf& aCapability, TApaAppInfo& aAppInfo )
    {
    CMenuSrvAppAttributes* self = new ( ELeave ) CMenuSrvAppAttributes();
    CleanupStack::PushL( self );
    self->ConstructL( aCapability, aAppInfo ) ;
    return self;
    }

// -----------------------------------------------------------------------------
// Destructor
// -----------------------------------------------------------------------------
CMenuSrvAppAttributes::~CMenuSrvAppAttributes()
    {
    iGroupName.Close();
    iFullName.Close();
    iCaption.Close();
    iShortCaption.Close();
    }

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
//
CMenuSrvAppAttributes::CMenuSrvAppAttributes()
    {
    }

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
//
void CMenuSrvAppAttributes::ConstructL( TApaAppCapabilityBuf& aCapability,
        TApaAppInfo& aAppInfo )
    {
    iGroupName.CreateL( aCapability().iGroupName );
    iFullName.CreateL( aAppInfo.iFullName );
    iCaption.CreateL( aAppInfo.iCaption );
    iShortCaption.CreateL( aAppInfo.iShortCaption );
    iUid = aAppInfo.iUid;
    iAppType = CMenuEngObject::ENoTypeApp;
    iAppIsHidden = aCapability().iAppIsHidden; // not shown in the Shell etc.
    }

// ---------------------------------------------------------
//
// ---------------------------------------------------------
//
TUid CMenuSrvAppAttributes::GetUid() const
    {
    return iUid;
    }

// ---------------------------------------------------------
//
// ---------------------------------------------------------
//
TBool CMenuSrvAppAttributes::IsHidden() const
    {
    return iAppIsHidden;
    }

// ---------------------------------------------------------
//
// ---------------------------------------------------------
//
void CMenuSrvAppAttributes::SetHidden( TBool aAppIsHidden )
    {
    iAppIsHidden = aAppIsHidden;
    }

// ---------------------------------------------------------
//
// ---------------------------------------------------------
//
TBool CMenuSrvAppAttributes::MatchItems(const CMenuSrvAppAttributes& aItem1,
                                        const CMenuSrvAppAttributes& aItem2)
    {
    TBool result( EFalse );
    if ( aItem1.GetUid() == aItem2.GetUid() )
        {
        result = ETrue;
        }
    return result;
    }

// ---------------------------------------------------------
//
// ---------------------------------------------------------
//
void CMenuSrvAppAttributes::SetAppType( CMenuEngObject::TAppType aAppType )
    {
    iAppType = aAppType;
    }

// ---------------------------------------------------------
//
// ---------------------------------------------------------
//
CMenuEngObject::TAppType CMenuSrvAppAttributes::GetAppType() const
    {
    return iAppType;
    }

// ---------------------------------------------------------
//
// ---------------------------------------------------------
//
const TPtrC CMenuSrvAppAttributes::GetGroupName() const
    {
    return iGroupName;
    }

// ---------------------------------------------------------
//
// ---------------------------------------------------------
//
const TPtrC CMenuSrvAppAttributes::GetFullName() const
    {
    return iFullName;
    }

// ---------------------------------------------------------
//
// ---------------------------------------------------------
//
const TPtrC CMenuSrvAppAttributes::GetCaption() const
    {
    return iCaption;
    }

// ---------------------------------------------------------
//
// ---------------------------------------------------------
//
const TPtrC CMenuSrvAppAttributes::GetShortCaption() const
    {
    return iShortCaption;
    }
