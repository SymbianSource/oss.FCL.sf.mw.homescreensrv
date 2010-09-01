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

// INCLUDE FILES

#include "menuengdeletionchecker.h"
#include "menuengobject.h"
#include "mcsmenuitem.h"

// ================= MEMBER FUNCTIONS =======================

// ---------------------------------------------------------
// TMenuEngDeletionChecker::CheckL
// ---------------------------------------------------------
//
void TMenuEngDeletionChecker::CheckL( CMenuEngObject& aObject )
    {
    if ( aObject.Flags() & TMenuItem::ELockDelete )
        {
        User::Leave( KErrAccessDenied );
        }
    }

//  End of File  
