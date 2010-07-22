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
#include "hsdataprovidertask.h"
#include "tstaskmonitorglobals.h"

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
//
void HsDataProviderTask::ExecuteL(const MHsDataProvider& provider, const RMessage2& msg)
{
    switch (msg.Function()) {
    case GetRunningAppInfo:
        msg.WriteL(KRequestDataSizeOffset, TPckgBuf<TInt>(provider.Data().Length()));
        msg.Complete(KErrNone);
        break;
    
    case FlushData:
        if (provider.Data().Length() > msg.GetDesMaxLengthL(KRequestDataBufferOffset)) {
            User::Leave(KErrCorrupt);
        }
        msg.WriteL(KRequestDataBufferOffset, provider.Data());
        msg.Complete(KErrNone);
        break;
    }
}
