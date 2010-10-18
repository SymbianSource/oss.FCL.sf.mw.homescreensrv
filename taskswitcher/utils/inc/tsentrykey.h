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


#ifndef TSENTRYKEY_H
#define TSENTRYKEY_H

#include <e32base.h>
#include <s32strm.h>

typedef TInt TTsKey;

class TTsEntryKey
{
public:
    static TInt Size();
    TTsEntryKey();
    TTsEntryKey( TTsKey aKey, TInt aRoot );
    TTsEntryKey( const TTsEntryKey &aKey );
    TTsEntryKey& operator =( const TTsEntryKey& aKey );
    TBool operator == ( const TTsEntryKey aKey ) const;
    TInt Key() const;
    void ExternalizeL( RWriteStream& aStream ) const;
    void InternalizeL( RReadStream& aStream );

private:
    TTsKey iKey;
    TInt iRoot;
};

#endif //TSENTRYKEY_H
