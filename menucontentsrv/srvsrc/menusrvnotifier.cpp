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

#include "menusrvnotifier.h"

// ================= MEMBER FUNCTIONS =======================

// ---------------------------------------------------------
// CMenuSrvNotifier::NotifyL
// ---------------------------------------------------------
//
void CMenuSrvNotifier::NotifyL
( TInt aFolder, TInt aEvents, const RMessage2& aMessage )
    {
    PanicIfPendingL( aMessage );
    iFolder = aFolder;
    iEvents = aEvents;
    SetPending( aMessage ); // No leaving after this!
    }

// ---------------------------------------------------------
// CMenuSrvNotifier::HandleEvents
// ---------------------------------------------------------
//
void CMenuSrvNotifier::HandleEvents( TInt aFolder, TInt aEvents )
    {
    if ( iPending )
        {
        // Notifier is up.
        if ( !iFolder || !aFolder || aFolder == iFolder )
            {
            // Folder match (or we don't care, or they don't tell).
            if ( aEvents & iEvents )
                {
                // Event matched.
                Complete( aEvents & iEvents );
                }
            }
        }
    }

//  End of File  
