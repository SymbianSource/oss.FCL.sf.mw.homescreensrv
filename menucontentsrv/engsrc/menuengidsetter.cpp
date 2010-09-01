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

#include "menuengidsetter.h"
#include "menuengidmanager.h"
#include "menuengobject.h"

// ================= MEMBER FUNCTIONS =======================

// ---------------------------------------------------------
// TMenuEngIdSetter::SetIdL
// ---------------------------------------------------------
//
void TMenuEngIdSetter::SetIdL( CMenuEngObject& aObject )
    {
    if ( !aObject.Id() )
        {
        TInt id;
        iIdManager.AllocL( id );
        aObject.SetId( id );
        }
    }

//  End of File  
