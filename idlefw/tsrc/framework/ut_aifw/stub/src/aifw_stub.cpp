/*
* Copyright (c) 2005-2008 Nokia Corporation and/or its subsidiary(-ies).
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
* Description:  AiFw stub
*
*/

// System includes
#include <centralrepository.h>

// User includes
#include <activeidle2domaincrkeys.h>
#include "aifw_stub.h"

// Constants


// ======== MEMBER FUNCTIONS ========

// ----------------------------------------------------------------------------
// CAiFw::CAiFw()
//
// ----------------------------------------------------------------------------
//
CAiFw::CAiFw()
    {
    }

// ----------------------------------------------------------------------------
// CAiFw::ConstructL()
//
// ----------------------------------------------------------------------------
//
void CAiFw::ConstructL()
    {
    iRepository = CRepository::NewL( TUid::Uid( KCRUidActiveIdleLV ) );
    }

// ----------------------------------------------------------------------------
// CAiFw::NewL()
//
// ----------------------------------------------------------------------------
//
CAiFw* CAiFw::NewL()
    {    
    CAiFw* self = CAiFw::NewLC();
    CleanupStack::Pop( self );
           
    return self;
    }

// ----------------------------------------------------------------------------
// CAiFw::NewLC()
//
// ----------------------------------------------------------------------------
//
CAiFw* CAiFw::NewLC()
    {
    CAiFw* self = new ( ELeave ) CAiFw;
    CleanupStack::PushL( self );
    
    self->ConstructL();
    
    return self;
    }

// ----------------------------------------------------------------------------
// CAiFw::~CAiFw()
//
// ----------------------------------------------------------------------------
//
CAiFw::~CAiFw()
    {        
    delete iRepository;
    iRepository = NULL;
    }


// ----------------------------------------------------------------------------
// CAiFw::Repository()
//
// ----------------------------------------------------------------------------
//
CRepository& CAiFw::Repository() const
    {
    return *iRepository;
    }

// End of file
