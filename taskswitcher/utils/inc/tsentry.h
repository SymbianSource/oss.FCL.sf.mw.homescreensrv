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
* Description:  Task list entry
*
*/

#ifndef TSENTRY_H
#define TSENTRY_H

#include <e32base.h>

class TTsEntryKey;

NONSHARABLE_CLASS( MTsEntry )
    {
public:
    virtual const TDesC& DisplayName() const =0;
    virtual TInt IconHandle() const =0;
    virtual TTime Timestamp() const =0;
    virtual TTime TimestampUpdate() const =0; 
    virtual TTsEntryKey Key() const =0;
    virtual TBool IsActive() const =0;
    virtual TBool IsClosable() const =0;
    virtual TBool Close() const =0;
    virtual TBool Launch() const =0;
    virtual TBool IsMandatory() const =0;

    };

#endif //TSENTRY_H
