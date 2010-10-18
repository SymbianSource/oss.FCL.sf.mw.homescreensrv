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
#ifndef TSRUNNINGAPP_H
#define TSRUNNINGAPP_H

#include <e32base.h>
#include <w32std.h>

#include "tsrunningapp.h"

class CApaWindowGroupName;
class MTsResourceManager;

class CTsRunningApp: public CBase,
                     public MTsRunningApplication
{
public:
    static CTsRunningApp* NewLC( MTsResourceManager& aResources,
                                 const RWsSession::TWindowGroupChainInfo& aInfo );
    ~CTsRunningApp();
    
    void RefreshDataL();

protected:
    CTsRunningApp( MTsResourceManager& aResources,
                   const RWsSession::TWindowGroupChainInfo& aInfo );
    void ConstructL();

public://from MTsRunningApplication
    TUid Uid()const;
    MTsRunningApplication::ApplicationHideMode HideMode() const;
    void SetHidden( TBool aHidden );
    TBool IsSystem() const;
    TInt WindowGroupId() const;
    TInt ParentWindowGroupId() const;
    TBool IsEmbeded() const;
    
    const TDesC& DisplayName() const;
    TInt IconHandle() const;
    TTime Timestamp() const;
    TTime TimestampUpdate() const; 
    TTsEntryKey Key() const;
    TBool IsActive() const;
    TBool IsClosable() const;
    TBool Close() const;
    TBool Launch() const;
    TBool IsMandatory() const;

protected:
    MTsResourceManager& iResources;
    const RWsSession::TWindowGroupChainInfo iWindowGroupInfo;
    MTsRunningApplication::ApplicationHideMode iHideMode;
    mutable CApaWindowGroupName* iWindowGroupName;
    mutable TPtrC iCaption;
    TTime iTimestap;
    TTime iUpdateTimestap;

};

#endif//TSRUNNINGAPP_H
