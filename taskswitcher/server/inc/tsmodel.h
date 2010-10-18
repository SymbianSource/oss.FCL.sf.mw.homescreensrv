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
#ifndef TSMODEL_H
#define TSMODEL_H

#include <e32base.h>
#include "tsmodelitem.h"

class MTsModelObserver;
class MTsModel 
{
public:
    virtual TInt Count() const = 0;
    TTsModelItem ItemL( TInt aOffset ) const;
    virtual void SetObserver( MTsModelObserver *observer ) = 0;
    
protected: //data access. should be used by TModelItem
    virtual const TDesC& DisplayName( TInt aOffset ) const = 0;
    virtual TInt IconHandle( TInt aOffset ) const = 0;
    virtual TTime Timestamp( TInt aOffset ) const = 0;
    virtual TTime TimestampUpdate(TInt offset) const = 0;
    virtual TTsEntryKey Key( TInt aOffset ) const = 0;
    virtual TBool IsActive( TInt aOffset ) const = 0;
    virtual TBool IsClosable( TInt aOffset ) const = 0;
    virtual TBool IsMandatory( TInt aOffset ) const = 0;

protected: //operations execution
    virtual TBool Close( TTsEntryKey aKey ) const = 0;
    virtual TBool Launch( TTsEntryKey aKey ) const = 0;

    friend class TTsModelItem;
};

#endif //TSMODEL_H
