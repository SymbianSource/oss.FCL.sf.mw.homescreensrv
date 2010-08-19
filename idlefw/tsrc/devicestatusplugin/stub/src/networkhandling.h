/*
* Copyright (c) 2005 Nokia Corporation and/or its subsidiary(-ies).
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
* Description:  ?Description
*
*/


#ifndef NETWORKHANDLING_H
#define NETWORKHANDLING_H


#include <e32base.h>
#include <cnwsession.h>
#include <nwhandlingengine.h>

class MNWMessageObserver;


class CNWSessionStub : public CNWSession
    {
    public:
        CNWSessionStub( MNWMessageObserver& aObserver, TNWInfo& aInfo );
    };

#endif // NETWORKHANDLING_H
