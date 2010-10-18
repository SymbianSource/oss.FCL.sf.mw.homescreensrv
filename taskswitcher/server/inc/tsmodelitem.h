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
#ifndef TSMODELITEM_H
#define TSMODELITEM_H

#include <e32base.h>
#include "tsentry.h"
#include "tsentrykey.h"

class MTsModel;

class TTsModelItem : public MTsEntry
    {
public:
    TTsModelItem( const MTsModel& aModel, TInt aOffset );
    TTsModelItem( const TTsModelItem& aItem );
    TBool IsValid() const;
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

public:
    void ExternalizeL( RWriteStream& aStream ) const;

private:
    const MTsModel& iModel;
    const TInt iIndex;
    };

#endif //TSMODELITEM_H
