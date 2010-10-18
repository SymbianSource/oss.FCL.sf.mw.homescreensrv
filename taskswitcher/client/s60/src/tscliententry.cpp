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
#include "tscliententry.h"
//------------------------------------------------------------------------------
CTsClientEntry* CTsClientEntry::NewLC(RReadStream &stream)
{
    CTsClientEntry* self = new (ELeave)CTsClientEntry();
    CleanupStack::PushL(self);
    self->BaseConstructL(stream);
    return self;
}

//------------------------------------------------------------------------------
CTsClientEntry::~CTsClientEntry()
{}

//------------------------------------------------------------------------------
CFbsBitmap* CTsClientEntry::Icon() const
{
    return iThumbail;
}

//------------------------------------------------------------------------------
TBool CTsClientEntry::Close() const
{
    return EFalse;
}

//------------------------------------------------------------------------------
TBool CTsClientEntry::Launch() const
    {
    return EFalse;
    }
