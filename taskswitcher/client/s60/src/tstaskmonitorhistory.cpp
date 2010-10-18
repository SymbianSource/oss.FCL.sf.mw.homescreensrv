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
#include "tstaskmonitorhistory.h"

TsTaskMonitorHistory::TsTaskMonitorHistory(const QSharedPointer< CTsClientEntry > item, int offset) :
mEntry(item),
mOffset(offset)
{
}

int TsTaskMonitorHistory::offset() const
{
    return mOffset;
}

bool TsTaskMonitorHistory::operator == (const TsTaskMonitorHistory &item) const
{
    return mEntry->Key() == item.mEntry->Key();
}

bool TsTaskMonitorHistory::isUpdated(const TsTaskMonitorHistory &item) const
{
    return mEntry->TimestampUpdate() != item.mEntry->TimestampUpdate();
}
