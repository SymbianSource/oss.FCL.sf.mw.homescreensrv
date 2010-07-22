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

#ifndef __CBACKSTEPPING_H
#define __CBACKSTEPPING_H

#include <w32std.h>
#include <tswindowgroupsobserver.h>


class CTsBackstepping : public CTsWindowGroupsObserver
{
public:
    IMPORT_C static CTsBackstepping* NewL(MTsWindowGroupsMonitor &);
    IMPORT_C static CTsBackstepping* NewLC(MTsWindowGroupsMonitor &);
    IMPORT_C virtual ~CTsBackstepping();

private:
    CTsBackstepping(MTsWindowGroupsMonitor &);
    void ConstructL();

    void HandleWindowGroupChanged(MTsResourceManager &, 
                                  const TArray<RWsSession::TWindowGroupChainInfo> &);
                                  
    void HandleWindowGroupChangedL(MTsResourceManager &, 
                                  const TArray<RWsSession::TWindowGroupChainInfo> &);
};

#endif // __CBACKSTEPPING_H
