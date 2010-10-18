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

#ifndef TSCLIENTENTRY_H
#define TSCLIENTENTRY_H

#include "tsentryimp.h"

/**
 * An entry in the task list.
 */
class CTsClientEntry: public CTsEntryImp
    {
public:
    static CTsClientEntry* NewLC(RReadStream &stream);
    ~CTsClientEntry();
    CFbsBitmap* Icon() const;
    TBool Close() const;
    TBool Launch() const;
    };

#endif //TSCLIENTENTRY_H
