/*
* Copyright (c)  Nokia Corporation and/or its subsidiary(-ies).
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


//#include <?include_file>
#include "networkhandling.h"
#include "stubdataholder.h"

// ======== MEMBER FUNCTIONS ========

CNWSession::CNWSession()
    {    
    }
CNWSession::~CNWSession()
    {    
    }

CNWSessionStub::CNWSessionStub( MNWMessageObserver& aObserver, TNWInfo& aInfo )
    {    
    CStubDataHolder::Instance()->SetNWMessageObserver( aObserver, aInfo );
    };
