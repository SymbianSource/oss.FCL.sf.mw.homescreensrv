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

#ifndef TSENTRYIMP_H
#define TSENTRYIMP_H

#include <e32base.h>
#include <e32cmn.h>

#include "tsentrykey.h"
#include "tsentry.h"
//#include "tstaskmonitorglobals.h"

class CFbsBitmap;
class RReadStream;

/**
 * An entry in the task list.
 */
NONSHARABLE_CLASS( CTsEntryImp ) : public CBase,
                                   public MTsEntry
    {
public:
    virtual ~CTsEntryImp();
    static void ExternalizeL( RWriteStream& aDst, const MTsEntry& aSrc );
    void InternalizeL( RReadStream& aSrc );

public:
    const TDesC& DisplayName() const;
    TInt IconHandle() const;
    TTime Timestamp() const;
    TTime TimestampUpdate() const; 
    TTsEntryKey Key() const;
    TBool IsActive() const;
    TBool IsClosable() const;
    TBool IsMandatory() const;

protected:
    void BaseConstructL(RReadStream& aStream);

protected:
    HBufC* iName;
    TBool iCloseableApp;
    TBool iIsActive;
    TBool iVisibility;
    CFbsBitmap* iThumbail;
    TTsEntryKey iKey;
    TInt iPriority;
    TTime iTimestamp;
    TTime iUpdateTimestamp;

    };

#endif //TSENTRYIMP_H
